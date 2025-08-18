/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "native_interface_arkweb.h"

#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "parameters.h"

#include "arkweb_error_code.h"
#include "arkweb_type.h"
#include "arkweb_utils.h"
#include "event_handler.h"
#include "native_arkweb_utils.h"
#include "native_javascript_execute_callback.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"

#include "arkweb_utils.h"

namespace {
std::mutex g_mtxMap; // the mutex to protect the shared resource
std::unordered_map<std::string, NativeArkWeb_OnValidCallback> g_validMap;
std::unordered_map<std::string, NativeArkWeb_OnDestroyCallback> g_destroyMap;
constexpr uint32_t MAX_DATABASE_SIZE_IN_MB = 100;
constexpr uint32_t MAX_KEYS_COUNT = 100;
constexpr size_t MAX_KEY_LENGTH = 2048;
std::mutex g_mtxMainHandler;
std::shared_ptr<OHOS::AppExecFwk::EventHandler> g_mainHandler = nullptr;
} // namespace

namespace OHOS::NWeb {

class NWebJsProxyCallbackImpl : public NWebJsProxyCallback {
public:
    NWebJsProxyCallbackImpl(const char *methodName, NativeArkWeb_OnJavaScriptProxyCallback methodCallback)
        : methodName_(methodName), methodCallback_(methodCallback) {
    }
    ~NWebJsProxyCallbackImpl() = default;

    std::string GetMethodName() override
    {
        return methodName_;
    }

    NativeArkWeb_OnJavaScriptProxyCallback GetMethodCallback() override
    {
        return methodCallback_;
    }

private:
    std::string methodName_;
    NativeArkWeb_OnJavaScriptProxyCallback methodCallback_ = nullptr;
};

class NWebSaveCookieCallbackImpl : public NWebBoolValueCallback {
public:
    explicit NWebSaveCookieCallbackImpl(
        std::function<void(ArkWeb_ErrorCode errorCode)> callback) : callback_(callback) {}
    ~NWebSaveCookieCallbackImpl() = default;

    void OnReceiveValue(bool result) override
    {
        WVLOG_D("save cookie received result, result = %{public}d", result);
        if (callback_) {
            ArkWeb_ErrorCode errorCode =
                result ? ArkWeb_ErrorCode::ARKWEB_SUCCESS : ArkWeb_ErrorCode::ARKWEB_COOKIE_SAVE_FAILED;
            callback_(errorCode);
        }
    }
private:
    std::function<void(ArkWeb_ErrorCode errorCode)> callback_;
};

}; // namespace OHOS::NWeb

using namespace OHOS;
void OH_NativeArkWeb_RunJavaScript(const char* webTag, const char* jsCode, NativeArkWeb_OnJavaScriptCallback callback)
{
    std::weak_ptr<OHOS::NWeb::NWeb> nwebWeak = OH_NativeArkWeb_GetWebInstanceByWebTag(webTag);
    if (auto nweb = nwebWeak.lock()) {
        auto callbackImpl = std::make_shared<OHOS::NWeb::NativeJavaScriptExecuteCallback>(callback);
        WVLOG_I("native RunJavaScript webTag: %{public}s", webTag);
        nweb->ExecuteJavaScript(jsCode, callbackImpl, false);
    } else {
        WVLOG_E("native RunJavaScript get nweb null: %{public}s", webTag);
    }
}

void OH_NativeArkWeb_RegisterJavaScriptProxy(const char* webTag, const char* objName, const char** methodList,
    NativeArkWeb_OnJavaScriptProxyCallback* callback, int32_t size, bool isNeedRefresh)
{
    WVLOG_I("native OH_NativeArkWeb_RegisterJavaScriptProxy webTag:%{public}s", webTag);
    std::vector<std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback>> proxyCallbacks;
    for (int i = 0; i < size; i++) {
        std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback> proxyCallback =
            std::make_shared<OHOS::NWeb::NWebJsProxyCallbackImpl>(methodList[i], callback[i]);
        proxyCallbacks.push_back(proxyCallback);
    }

    std::weak_ptr<OHOS::NWeb::NWeb> nwebWeak = OH_NativeArkWeb_GetWebInstanceByWebTag(webTag);
    if (auto nweb = nwebWeak.lock()) {
        nweb->RegisterNativeArkJSFunction(objName, proxyCallbacks);
        if (isNeedRefresh) {
            nweb->Reload();
        }
    } else {
        WVLOG_E("native RegisterJavaScriptProxy get nweb null: %{public}s", webTag);
    }
}

void OH_NativeArkWeb_UnregisterJavaScriptProxy(const char* webTag, const char* objName)
{
    WVLOG_I("native OH_NativeArkWeb_RegisterJavaScriptProxy: %{public}s", webTag);
    std::weak_ptr<OHOS::NWeb::NWeb> nwebWeak = OH_NativeArkWeb_GetWebInstanceByWebTag(webTag);
    if (auto nweb = nwebWeak.lock()) {
        nweb->UnRegisterNativeArkJSFunction(objName);
    } else {
        WVLOG_E("native RegisterJavaScriptProxy get nweb null: %{public}s", webTag);
    }
}

void OH_NativeArkWeb_SetDestroyCallback(const char* webTag, NativeArkWeb_OnDestroyCallback callback)
{
    WVLOG_I("native RegisterDestroyCallback, webTag: %{public}s", webTag);
    std::lock_guard<std::mutex> guard(g_mtxMap);
    g_destroyMap[webTag] = callback;
    std::weak_ptr<OHOS::NWeb::NWeb> nwebWeak = OH_NativeArkWeb_GetWebInstanceByWebTag(webTag);
    if (auto nweb = nwebWeak.lock()) {
        WVLOG_I("native RegisterNativeDestroyCallback call nweb");
        nweb->RegisterNativeDestroyCallback(webTag, callback);
    } else {
        WVLOG_E("native RegisterDestroyCallback get nweb null: %{public}s", webTag);
    }
}

NativeArkWeb_OnDestroyCallback OH_NativeArkWeb_GetDestroyCallback(const char* webTag)
{
    WVLOG_I("native OH_Web_GetDestroyCallback, webTag: %{public}s", webTag);
    std::lock_guard<std::mutex> guard(g_mtxMap);
    std::unordered_map<std::string, NativeArkWeb_OnDestroyCallback>::iterator iter;
    if ((iter = g_destroyMap.find(webTag)) != g_destroyMap.end()) {
        return iter->second;
    }
    return nullptr;
}

void OH_NativeArkWeb_SetJavaScriptProxyValidCallback(const char* webTag, NativeArkWeb_OnValidCallback callback)
{
    WVLOG_I("native RegisterValidCallback, webTag: %{public}s", webTag);
    std::lock_guard<std::mutex> guard(g_mtxMap);
    g_validMap[webTag] = callback;
    std::weak_ptr<OHOS::NWeb::NWeb> nwebWeak = OH_NativeArkWeb_GetWebInstanceByWebTag(webTag);
    if (auto nweb = nwebWeak.lock()) {
        WVLOG_I("native OH_NativeArkWeb_SetJavaScriptProxyValidCallback call nweb");
        nweb->RegisterNativeValideCallback(webTag, callback);
    } else {
        WVLOG_E("native RegisterDestroyCallback get nweb null: %{public}s", webTag);
    }
}

NativeArkWeb_OnValidCallback OH_NativeArkWeb_GetJavaScriptProxyValidCallback(const char* webTag)
{
    WVLOG_I("native OH_Web_GetValidCallback, webTag: %{public}s", webTag);
    std::lock_guard<std::mutex> guard(g_mtxMap);
    std::unordered_map<std::string, NativeArkWeb_OnValidCallback>::iterator iter;
    if ((iter = g_validMap.find(webTag)) != g_validMap.end()) {
        return iter->second;
    }
    return nullptr;
}

template<typename Fn>
static bool LoadFunction(const char* functionName, Fn* fnOut)
{
    void* fn = OHOS::NWeb::NWebHelper::Instance().LoadFuncSymbol(functionName);
    if (!fn) {
        WVLOG_E("%{public}s not found.", functionName);
        return false;
    }
    *fnOut = reinterpret_cast<Fn>(fn);
    return true;
}

ArkWeb_ErrorCode OH_NativeArkWeb_LoadData(const char* webTag,
                                          const char* data,
                                          const char* mimeType,
                                          const char* encoding,
                                          const char* baseUrl,
                                          const char* historyUrl)
{
    WVLOG_I("native OH_NativeArkWeb_LoadData, webTag: %{public}s", webTag);
    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return ArkWeb_ErrorCode::ARKWEB_LIBRARY_OPEN_FAILURE;
    }

    ArkWeb_ErrorCode (*loadData)(const char* webTag,
                                 const char* data,
                                 const char* mimeType,
                                 const char* encoding,
                                 const char* baseUrl,
                                 const char* historyUrl) = nullptr;

#define ARKWEB_NATIVE_LOAD_FN_PTR(apiMember, funcImpl) LoadFunction(#funcImpl, &(apiMember))
    ARKWEB_NATIVE_LOAD_FN_PTR(loadData, OH_NativeArkWeb_LoadData);
#undef ARKWEB_NATIVE_LOAD_FN_PTR
    if (!loadData) {
        WVLOG_E("OH_NativeArkWeb_LoadData failed to load function loadData");
        return ArkWeb_ErrorCode::ARKWEB_LIBRARY_SYMBOL_NOT_FOUND;
    }
    return loadData(webTag, data, mimeType, encoding, baseUrl, historyUrl);
}

void OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(const char* webTag,
                                                        const ArkWeb_ProxyObjectWithResult* proxyObject,
                                                        const char* permission)
{
    WVLOG_I("native OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy, webTag: %{public}s", webTag);
    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return;
    }

    void (*registerAsyncThreadJavaScriptProxy)(const char* webTag,
                                               const ArkWeb_ProxyObjectWithResult* proxyObject,
                                               const char* permission) = nullptr;

#define ARKWEB_NATIVE_LOAD_FN_PTR(apiMember, funcImpl) LoadFunction(#funcImpl, &(apiMember))
    ARKWEB_NATIVE_LOAD_FN_PTR(registerAsyncThreadJavaScriptProxy, OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy);
#undef ARKWEB_NATIVE_LOAD_FN_PTR
    if (!registerAsyncThreadJavaScriptProxy) {
        WVLOG_E("failed to load function OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy");
        return;
    }
    return registerAsyncThreadJavaScriptProxy(webTag, proxyObject, permission);
}

ArkWeb_BlanklessInfo OH_NativeArkWeb_GetBlanklessInfoWithKey(const char* webTag, const char* key)
{
    if (!OHOS::system::GetBoolParameter("web.blankless.enabled", false) || IS_CALLING_FROM_M114()) {
        WVLOG_E("blankless OH_NativeArkWeb_GetBlanklessInfoWithKey capability not supported");
        return { ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT, 0.0, 0 };
    }

    size_t keyLen = strlen(key);
    if (keyLen == 0 || keyLen > MAX_KEY_LENGTH) {
        WVLOG_E("blankless OH_NativeArkWeb_GetBlanklessInfoWithKey key length is invalid");
        return { ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS, 0.0, 0 };
    }

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("blankless OH_NativeArkWeb_GetBlanklessInfoWithKey load web engine failed");
        return { ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_UNKNOWN, 0.0, 0 };
    }

    ArkWeb_BlanklessInfo (*getBlanklessInfoWithKey)(const char* webTag, const char* key) = nullptr;

#define ARKWEB_NATIVE_LOAD_FN_PTR(apiMember, funcImpl) LoadFunction(#funcImpl, &(apiMember))
    ARKWEB_NATIVE_LOAD_FN_PTR(getBlanklessInfoWithKey, OH_NativeArkWeb_GetBlanklessInfoWithKey);
#undef ARKWEB_NATIVE_LOAD_FN_PTR
    if (!getBlanklessInfoWithKey) {
        WVLOG_E("blankless OH_NativeArkWeb_GetBlanklessInfoWithKey failed to load function");
        return { ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_UNKNOWN, 0.0, 0 };
    }

    return getBlanklessInfoWithKey(webTag, key);
}

ArkWeb_BlanklessErrorCode OH_NativeArkWeb_SetBlanklessLoadingWithKey(const char* webTag,
                                                                     const char* key,
                                                                     bool isStarted)
{
    if (!OHOS::system::GetBoolParameter("web.blankless.enabled", false) || IS_CALLING_FROM_M114()) {
        WVLOG_E("blankless OH_NativeArkWeb_SetBlanklessLoadingWithKey capability not supported");
        return ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT;
    }

    size_t keyLen = strlen(key);
    if (keyLen == 0 || keyLen > MAX_KEY_LENGTH) {
        WVLOG_E("blankless OH_NativeArkWeb_SetBlanklessLoadingWithKey key length is invalid");
        return ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS;
    }

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("blankless OH_NativeArkWeb_SetBlanklessLoadingWithKey load web engine failed");
        return ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_UNKNOWN;
    }

    ArkWeb_BlanklessErrorCode (*setBlanklessLoadingWithKey)(const char* webTag,
                                                            const char* key,
                                                            bool isStarted) = nullptr;

#define ARKWEB_NATIVE_LOAD_FN_PTR(apiMember, funcImpl) LoadFunction(#funcImpl, &(apiMember))
    ARKWEB_NATIVE_LOAD_FN_PTR(setBlanklessLoadingWithKey, OH_NativeArkWeb_SetBlanklessLoadingWithKey);
#undef ARKWEB_NATIVE_LOAD_FN_PTR
    if (!setBlanklessLoadingWithKey) {
        WVLOG_E("blankless OH_NativeArkWeb_SetBlanklessLoadingWithKey failed to load function");
        return ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_UNKNOWN;
    }

    return setBlanklessLoadingWithKey(webTag, key, isStarted);
}

void OH_NativeArkWeb_ClearBlanklessLoadingCache(const char* key[], uint32_t size)
{
    if (!OHOS::system::GetBoolParameter("web.blankless.enabled", false) || IS_CALLING_FROM_M114()) {
        WVLOG_E("blankless OH_NativeArkWeb_ClearBlanklessLoadingCache capability not supported");
        return;
    }

    std::vector<std::string> keys;
    if (key == nullptr) {
        OHOS::NWeb::NWebHelper::Instance().ClearBlanklessLoadingCache(keys);
        return;
    }

    if (size > MAX_KEYS_COUNT) {
        WVLOG_W("blankless OH_NativeArkWeb_ClearBlanklessLoadingCache array size should not exceed 100");
        size = MAX_KEYS_COUNT;
    }

    for (uint32_t idx = 0; idx < size; idx++) {
        if (key[idx] == nullptr) {
            continue;
        }
        size_t keyLen = strlen(key[idx]);
        if (keyLen == 0 || keyLen > MAX_KEY_LENGTH) {
            continue;
        }
        keys.push_back(key[idx]);
    }

    if (keys.size() == 0) {
        WVLOG_W("blankless OH_NativeArkWeb_ClearBlanklessLoadingCache valid keys are 0");
        return;
    }
    OHOS::NWeb::NWebHelper::Instance().ClearBlanklessLoadingCache(keys);
}

uint32_t OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(uint32_t capacity)
{
    if (!OHOS::system::GetBoolParameter("web.blankless.enabled", false) || IS_CALLING_FROM_M114()) {
        WVLOG_E("blankless OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity capability not supported");
        return 0;
    }

    if (capacity > MAX_DATABASE_SIZE_IN_MB) {
        capacity = MAX_DATABASE_SIZE_IN_MB;
    }

    OHOS::NWeb::NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(static_cast<int32_t>(capacity));
    return capacity;
}

static std::shared_ptr<OHOS::AppExecFwk::EventHandler> GetMainThreadEventHandler()
{
    std::lock_guard<std::mutex> guard(g_mtxMainHandler);
    if (!g_mainHandler) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return nullptr;
        }
        g_mainHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    return g_mainHandler;
}

static void PostSaveCookieToUIThread(OH_ArkWeb_OnCookieSaveCallback callback)
{
    auto mainHandler = GetMainThreadEventHandler();
    if (!mainHandler) {
        WVLOG_E("get main event runner failed");
        if (callback) {
            callback(ArkWeb_ErrorCode::ARKWEB_COOKIE_SAVE_FAILED);
        }
        return;
    }

    bool succ = mainHandler->PostTask([callback]() {
        std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
            OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
        if (cookieManager == nullptr) {
            WVLOG_E("cookieManager is nullptr");
            if (callback) {
                callback(ArkWeb_ErrorCode::ARKWEB_COOKIE_MANAGER_INITIALIZE_FAILED);
            }
            return;
        }
        auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallbackImpl>(callback);
        cookieManager->Store(callbackImpl);
        }, "", 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    if (!succ) {
        WVLOG_E("post cookie task to UI thread failed");
        if (callback) {
            callback(ArkWeb_ErrorCode::ARKWEB_COOKIE_SAVE_FAILED);
        }
    }
}

ArkWeb_ErrorCode OH_ArkWebCookieManager_SaveCookieSync()
{
    if (getpid() != gettid() && !OHOS::NWeb::NWebHelper::Instance().HasLoadWebEngine()) {
        WVLOG_E("cookieManager not initialize");
        return ArkWeb_ErrorCode::ARKWEB_COOKIE_MANAGER_NOT_INITIALIZED;
    }

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        return ArkWeb_ErrorCode::ARKWEB_COOKIE_MANAGER_INITIALIZE_FAILED;
    }

    if (!cookieManager->Store()) {
        return ArkWeb_ErrorCode::ARKWEB_COOKIE_SAVE_FAILED;
    }
    return ArkWeb_ErrorCode::ARKWEB_SUCCESS;
}

void OH_ArkWebCookieManager_SaveCookieAsync(OH_ArkWeb_OnCookieSaveCallback callback)
{
    if (getpid() != gettid() && !OHOS::NWeb::NWebHelper::Instance().HasLoadWebEngine()) {
        WVLOG_D("post save cookie to UI thread");
        PostSaveCookieToUIThread(callback);
        return;
    }

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> cookieManager =
        OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager == nullptr) {
        WVLOG_E("cookieManager is nullptr");
        if (callback) {
            callback(ArkWeb_ErrorCode::ARKWEB_COOKIE_MANAGER_INITIALIZE_FAILED);
        }
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebSaveCookieCallbackImpl>(callback);
    cookieManager->Store(callbackImpl);
}

void OH_NativeArkWeb_SetActiveWebEngineVersion(ArkWebEngineVersion webEngineVersion) {
    OHOS::ArkWeb::setActiveWebEngineVersion(
        static_cast<OHOS::ArkWeb::ArkWebEngineVersion>(static_cast<int>(webEngineVersion)));
}

ArkWebEngineVersion OH_NativeArkWeb_GetActiveWebEngineVersion() {
    return static_cast<ArkWebEngineVersion>(static_cast<int>(OHOS::ArkWeb::getActiveWebEngineVersion()));
}

bool OH_NativeArkWeb_IsActiveWebEngineEvergreen() {
    return OHOS::ArkWeb::IsActiveWebEngineEvergreen();
}