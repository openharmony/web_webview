/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "arkweb_interface.h"

#include "arkweb_type.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "arkweb_error_code.h"

namespace {
#define ARKWEB_NATIVE_FOR_EACH_COMPONENT_API_FN(DO)           \
    DO(onControllerAttached, OH_ArkWeb_OnControllerAttached); \
    DO(onPageBegin, OH_ArkWeb_OnPageBegin);                   \
    DO(onPageEnd, OH_ArkWeb_OnPageEnd);                       \
    DO(onDestroy, OH_ArkWeb_OnDestroy)

#define ARKWEB_NATIVE_FOR_EACH_COMPONENT_GLOBAL_API_FN(DO)    \
    DO(onScroll, OH_ArkWeb_OnScroll)

#define ARKWEB_NATIVE_FOR_EACH_CONTROLLER_API_FN(DO)                          \
    DO(runJavaScript, OH_ArkWeb_RunJavaScript);                               \
    DO(registerJavaScriptProxy, OH_ArkWeb_RegisterJavaScriptProxy);           \
    DO(deleteJavaScriptRegister, OH_ArkWeb_DeleteJavaScriptProxy);            \
    DO(refresh, OH_ArkWeb_Refresh);                                           \
    DO(registerAsyncJavaScriptProxy, OH_ArkWeb_RegisterAsyncJavaScriptProxy); \
    DO(createWebMessagePorts, OH_ArkWeb_CreateWebMessagePorts);               \
    DO(destroyWebMessagePorts, OH_ArkWeb_DestroyWebMessagePorts);             \
    DO(postWebMessage, OH_ArkWeb_PostWebMessage);                             \
    DO(getLastJavascriptProxyCallingFrameUrl, OH_ArkWeb_GetLastJavascriptProxyCallingFrameUrl)

#define ARKWEB_NATIVE_FOR_EACH_CONTROLLER_API_FN_2(DO)                        \
    DO(registerJavaScriptProxyEx,                                             \
        OH_ArkWeb_RegisterJavaScriptProxyEx);                                 \
    DO(registerAsyncJavaScriptProxyEx,                                        \
        OH_ArkWeb_RegisterAsyncJavaScriptProxyEx)

#define ARKWEB_NATIVE_FOR_EACH_WEBMESSAGEPORT_API_FN(DO) \
    DO(postMessage, OH_WebMessage_PostMessage);          \
    DO(close, OH_WebMessage_Close);                      \
    DO(setMessageEventHandler, OH_WebMessage_SetMessageEventHandler)

#define ARKWEB_NATIVE_FOR_EACH_WEBMESSAGE_API_FN(DO)        \
    DO(createWebMessage, OH_WebMessage_CreateWebMessage);   \
    DO(destroyWebMessage, OH_WebMessage_DestroyWebMessage); \
    DO(setType, OH_WebMessage_SetType);                     \
    DO(getType, OH_WebMessage_GetType);                     \
    DO(setData, OH_WebMessage_SetData);                     \
    DO(getData, OH_WebMessage_GetData)

#define ARKWEB_NATIVE_FOR_EACH_WEBCOOKIEMANAGER_API_FN(DO)                \
    DO(fetchCookieSync, OH_CookieManager_FetchCookieSync);                \
    DO(configCookieSync, OH_CookieManager_ConfigCookieSync);              \
    DO(existCookies, OH_CookieManager_ExistCookies);                      \
    DO(clearAllCookiesSync, OH_CookieManager_ClearAllCookiesSync);        \
    DO(clearSessionCookiesSync, OH_CookieManager_ClearSessionCookiesSync)

#define ARKWEB_NATIVE_FOR_EACH_JAVASCRIPTVALUE_API_FN(DO)             \
    DO(createJavaScriptValue, OH_JavaScript_CreateJavaScriptValue)

ArkWeb_ComponentAPI* g_ComponentImpl = nullptr;
ArkWeb_ControllerAPI* g_ControllerImpl = nullptr;
ArkWeb_WebMessagePortAPI* g_WebMessagePortImpl = nullptr;
ArkWeb_WebMessageAPI* g_WebMessageImpl = nullptr;
ArkWeb_CookieManagerAPI* g_CookieManagerImpl = nullptr;
ArkWeb_JavaScriptValueAPI* g_JavaScriptValueImpl = nullptr;

typedef struct {
    /** The ArkWeb_ComponentGlobalAPI struct size. */
    size_t size;
    /** Register the onScroll callback. */
    bool (*onScroll)(const char* webTag, ArkWeb_OnScrollCallback callback, void* userData);
} ArkWeb_ComponentGlobalAPI;

ArkWeb_ComponentGlobalAPI* g_ComponentGlobalImpl = nullptr;

} // namespace

template<typename Fn>
static void LoadFunction(const char* functionName, Fn* fnOut)
{
    void* fn = OHOS::NWeb::NWebHelper::Instance().LoadFuncSymbol(functionName);
    if (!fn) {
        WVLOG_E("%{public}s not found.", functionName);
        return;
    }
    *fnOut = reinterpret_cast<Fn>(fn);
}

static bool LoadComponentAPI()
{
    if (g_ComponentImpl) {
        WVLOG_D("NativeArkWeb component api already loaded");
        return true;
    }
    g_ComponentImpl = new (std::nothrow) ArkWeb_ComponentAPI();
    if (!g_ComponentImpl) {
        WVLOG_E("NativeArkWeb component api is nullptr");
        return false;
    }
    g_ComponentImpl->size = sizeof(ArkWeb_ComponentAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }
#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_ComponentImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_COMPONENT_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

static bool LoadComponentGlobalAPI()
{
    if (g_ComponentGlobalImpl) {
        WVLOG_D("NativeArkWeb component api already loaded");
        return true;
    }

    g_ComponentGlobalImpl = new (std::nothrow) ArkWeb_ComponentGlobalAPI();
    if (!g_ComponentGlobalImpl) {
        WVLOG_E("NativeArkWeb component global api is nullptr");
        return false;
    }
    g_ComponentGlobalImpl->size = sizeof(ArkWeb_ComponentGlobalAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }

#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_ComponentGlobalImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_COMPONENT_GLOBAL_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

static bool LoadControllerAPI()
{
    if (g_ControllerImpl) {
        WVLOG_D("NativeArkWeb controller api already loaded");
        return true;
    }
    g_ControllerImpl = new (std::nothrow) ArkWeb_ControllerAPI();
    if (!g_ControllerImpl) {
        WVLOG_E("NativeArkWeb controller api is nullptr");
        return false;
    }
    g_ControllerImpl->size = sizeof(ArkWeb_ControllerAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }
#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_ControllerImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_CONTROLLER_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_ControllerImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_CONTROLLER_API_FN_2(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

static bool LoadWebMessagePortAPI()
{
    if (g_WebMessagePortImpl) {
        WVLOG_I("NativeArkWeb web message port api already loaded");
        return true;
    }
    g_WebMessagePortImpl = new (std::nothrow) ArkWeb_WebMessagePortAPI();
    if (!g_WebMessagePortImpl) {
        WVLOG_E("NativeArkWeb web message port api is nullptr");
        return false;
    }
    g_WebMessagePortImpl->size = sizeof(ArkWeb_WebMessagePortAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }
#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_WebMessagePortImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_WEBMESSAGEPORT_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

static bool LoadWebMessageAPI()
{
    if (g_WebMessageImpl) {
        WVLOG_I("NativeArkWeb web message api already loaded");
        return true;
    }
    g_WebMessageImpl = new (std::nothrow) ArkWeb_WebMessageAPI();
    if (!g_WebMessageImpl) {
        WVLOG_E("NativeArkWeb web message api is nullptr");
        return false;
    }
    g_WebMessageImpl->size = sizeof(ArkWeb_WebMessageAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }
#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_WebMessageImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_WEBMESSAGE_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

static bool LoadCookieManagerAPI()
{
    if (g_CookieManagerImpl) {
        WVLOG_I("NativeArkWeb cookie manager api already loaded");
        return true;
    }

    g_CookieManagerImpl = new ArkWeb_CookieManagerAPI();
    if (!g_CookieManagerImpl) {
        WVLOG_E("NativeArkWeb cookie manager api is nullptr");
        return false;
    }
    g_CookieManagerImpl->size = sizeof(ArkWeb_CookieManagerAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, true)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }
#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_CookieManagerImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_WEBCOOKIEMANAGER_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

static bool LoadJavaScriptValueAPI()
{
    if (g_JavaScriptValueImpl) {
        WVLOG_I("NativeArkWeb javaScript value api already loaded");
        return true;
    }

    g_JavaScriptValueImpl = new (std::nothrow) ArkWeb_JavaScriptValueAPI();
    if (!g_JavaScriptValueImpl) {
        WVLOG_E("NativeArkWeb javaScript value api is nullptr");
        return false;
    }
    g_JavaScriptValueImpl->size = sizeof(ArkWeb_JavaScriptValueAPI);

    if (!OHOS::NWeb::NWebHelper::Instance().LoadWebEngine(true, false)) {
        WVLOG_E("NativeArkWeb webEngineHandle is nullptr");
        return false;
    }
#define ARKWEB_NATIVE_LOAD_FN_PTR(fn, ndkFn) LoadFunction(#ndkFn, &(g_JavaScriptValueImpl->fn))
    ARKWEB_NATIVE_FOR_EACH_JAVASCRIPTVALUE_API_FN(ARKWEB_NATIVE_LOAD_FN_PTR);
#undef ARKWEB_NATIVE_LOAD_FN_PTR

    return true;
}

ArkWeb_AnyNativeAPI* OH_ArkWeb_GetNativeAPI(ArkWeb_NativeAPIVariantKind type)
{
    switch (type) {
        case ARKWEB_NATIVE_COMPONENT: {
            if (!LoadComponentAPI()) {
                return nullptr;
            }
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_ComponentImpl);
        }
        case ARKWEB_NATIVE_CONTROLLER: {
            if (!LoadControllerAPI()) {
                return nullptr;
            }
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_ControllerImpl);
        }
        case ARKWEB_NATIVE_WEB_MESSAGE_PORT: {
            if (!LoadWebMessagePortAPI()) {
                return nullptr;
            }
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_WebMessagePortImpl);
        }
        case ARKWEB_NATIVE_WEB_MESSAGE: {
            if (!LoadWebMessageAPI()) {
                return nullptr;
            }
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_WebMessageImpl);
        }
        case ARKWEB_NATIVE_COOKIE_MANAGER: {
            if (!LoadCookieManagerAPI()) {
                return nullptr;
            }
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_CookieManagerImpl);
        }
        case ARKWEB_NATIVE_JAVASCRIPT_VALUE: {
            if (!LoadJavaScriptValueAPI()) {
                return nullptr;
            }
            return reinterpret_cast<ArkWeb_AnyNativeAPI*>(g_JavaScriptValueImpl);
        }
        default: {
            WVLOG_E("fail to get %{public}d arkweb api family", type);
            return nullptr;
        }
    }
}

bool OH_ArkWeb_RegisterScrollCallback(
    const char* webTag, ArkWeb_OnScrollCallback callback, void* userData)
{
    if (!LoadComponentGlobalAPI()) {
        return false;
    }
    if (!g_ComponentGlobalImpl->onScroll) {
        return false;
    }
    return g_ComponentGlobalImpl->onScroll(webTag, callback, userData);
}

[[maybe_unused]] void OH_ArkWeb_DestroyGlobalPointersForTest(void)
{
    if (g_ComponentImpl != nullptr) {
        delete g_ComponentImpl;
        g_ComponentImpl = nullptr;
    }

    if (g_ControllerImpl != nullptr) {
        delete g_ControllerImpl;
        g_ControllerImpl = nullptr;
    }

    if (g_WebMessagePortImpl != nullptr) {
        delete g_WebMessagePortImpl;
        g_WebMessagePortImpl = nullptr;
    }

    if (g_WebMessageImpl != nullptr) {
        delete g_WebMessageImpl;
        g_WebMessageImpl = nullptr;
    }

    if (g_CookieManagerImpl != nullptr) {
        delete g_CookieManagerImpl;
        g_CookieManagerImpl = nullptr;
    }

    if (g_JavaScriptValueImpl != nullptr) {
        delete g_JavaScriptValueImpl;
        g_JavaScriptValueImpl = nullptr;
    }

    if (g_ComponentGlobalImpl != nullptr) {
        delete g_ComponentGlobalImpl;
        g_ComponentGlobalImpl = nullptr;
    }
}
