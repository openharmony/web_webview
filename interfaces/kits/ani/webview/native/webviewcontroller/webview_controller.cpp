/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "webview_controller.h"

#include <memory>
#include <unordered_map>
#include <securec.h>
#include <regex>

#include "application_context.h"
#include "ani_business_error.h"
#include "business_error.h"
#include "napi_parse_utils.h"
#include "ohos_resource_adapter_impl.h"

#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"

#include "nweb_log.h"
#include "nweb_store_web_archive_callback.h"
#include "web_errors.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_result_callback.h"
#include "native_media_player_impl.h"

#include "nweb_precompile_callback.h"
#include "nweb_cache_options_impl.h"

#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "system_ability_definition.h"
#include "nweb_snapshot_callback_impl.h"
#include "../../../../../../ohos_interface/ohos_glue/base/include/ark_web_errno.h"

namespace {
constexpr int32_t PARAMZERO = 0;
constexpr int32_t PARAMONE = 1;
constexpr int32_t RESULT_COUNT = 2;
const std::string BUNDLE_NAME_PREFIX = "bundleName:";
const std::string MODULE_NAME_PREFIX = "moduleName:";
} // namespace

namespace OHOS {
namespace NWeb {
namespace {
constexpr uint32_t URL_MAXIMUM = 2 * 1024 * 1024;

struct WaitForAttachParam {
    arkts::concurrency_helpers::AsyncWork* asyncWork;
    ani_resolver resolver;
    int32_t timeout;
    WebviewController* webviewController;
    int32_t state;
    ani_vm* vm;
};

bool GetAppBundleNameAndModuleName(std::string& bundleName, std::string& moduleName)
{
    static std::string applicationBundleName;
    static std::string applicationModuleName;
    if (!applicationBundleName.empty() && !applicationModuleName.empty()) {
        bundleName = applicationBundleName;
        moduleName = applicationModuleName;
        return true;
    }
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!context) {
        WVLOG_E("Failed to get application context.");
        return false;
    }
    auto resourceManager = context->GetResourceManager();
    if (!resourceManager) {
        WVLOG_E("Failed to get resource manager.");
        return false;
    }
    applicationBundleName = resourceManager->bundleInfo.first;
    applicationModuleName = resourceManager->bundleInfo.second;
    bundleName = applicationBundleName;
    moduleName = applicationModuleName;
    WVLOG_D("application bundleName: %{public}s, moduleName: %{public}s", bundleName.c_str(), moduleName.c_str());
    return true;
}
}
using namespace NWebError;
std::mutex g_objectMtx;
std::unordered_map<int32_t, WebviewController*> g_webview_controller_map;
std::string WebviewController::customeSchemeCmdLine_ = "";
bool WebviewController::existNweb_ = false;
bool WebviewController::webDebuggingAccess_ = OHOS::system::GetBoolParameter("web.debug.devtools", false);
int32_t WebviewController::webDebuggingPort_ = 0;
std::set<std::string> WebviewController::webTagSet_;
int32_t WebviewController::webTagStrId_ = 0;

WebviewController::WebviewController(int32_t nwebId) : nwebId_(nwebId)
{
    if (IsInit()) {
        std::unique_lock<std::mutex> lk(g_objectMtx);
        g_webview_controller_map.emplace(nwebId, this);
    }
}

WebviewController::WebviewController(const std::string& webTag) : webTag_(webTag)
{
    NWebHelper::Instance().SetWebTag(-1, webTag_.c_str());
}

WebviewController::~WebviewController()
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    g_webview_controller_map.erase(nwebId_);

    {
        std::unique_lock<std::mutex> attachLock(attachMtx_);
        attachState_ = AttachState::ATTACHED;
        attachCond_.notify_all();
    }

    for (auto& [eventName, regObjs] : attachEventRegisterInfo_) {
        for (auto& regObj : regObjs) {
            if (regObj.m_regHandlerRef) {
                regObj.m_regEnv->GlobalReference_Delete(regObj.m_regHandlerRef);
                regObj.m_regHandlerRef = nullptr;
            }
        }
        regObjs.clear();
    }
    attachEventRegisterInfo_.clear();
}

void WebviewController::TriggerStateChangeCallback(const std::string& type)
{
    auto iter = attachEventRegisterInfo_.find(type);
    if (iter == attachEventRegisterInfo_.end()) {
        WVLOG_D("TriggerStateChangeCallback event %{public}s not found", type.c_str());
        return;
    }

    const std::vector<WebRegObj>& regObjs = iter->second;
    for (const auto& regObj : regObjs) {
        if (!regObj.m_isMarked) {
            if (!regObj.m_regHandlerRef) {
                WVLOG_E("Handler for event %{public}s is null", type.c_str());
                continue;
            }

            ani_enum_item stateRst = nullptr;
            ani_enum enumType;
            if (regObj.m_regEnv->FindEnum("@ohos.web.webview.webview.ControllerAttachState", &enumType) != ANI_OK) {
                WVLOG_E("Find enum ControllerAttachState failed");
                continue;
            }

            if (regObj.m_regEnv->Enum_GetEnumItemByIndex(enumType, static_cast<ani_int>(attachState_), &stateRst) !=
                ANI_OK) {
                WVLOG_E("Convert attachState(%{public}d) failed", attachState_);
                continue;
            }

            std::vector<ani_ref> vec;
            vec.push_back(stateRst);
            ani_ref callbackResult = nullptr;
            regObj.m_regEnv->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(regObj.m_regHandlerRef), vec.size(),
                vec.data(), &callbackResult);
        }
    }

    for (auto it = iter->second.begin(); it != iter->second.end();) {
        if (it->m_isMarked) {
            it->m_regEnv->GlobalReference_Delete(it->m_regHandlerRef);
            it = iter->second.erase(it);
        } else {
            ++it;
        }
    }
}

void WebviewController::SetWebId(int32_t nwebId)
{
    nwebId_ = nwebId;
    std::unique_lock<std::mutex> lk(g_objectMtx);
    g_webview_controller_map.emplace(nwebId, this);

    if (webTag_.empty()) {
        WVLOG_I("native webtag is empty, don't care because it's not a native instance");
        return;
    }

    AttachState prevState = attachState_;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        OH_NativeArkWeb_BindWebTagToWebInstance(webTag_.c_str(), nweb_ptr);
        NWebHelper::Instance().SetWebTag(nwebId_, webTag_.c_str());

        {
            std::unique_lock<std::mutex> attachLock(attachMtx_);
            attachState_ = AttachState::ATTACHED;
            attachCond_.notify_all();
        }

        if (prevState != attachState_) {
            TriggerStateChangeCallback(CONTROLLER_ATTACH_STATE_CHANGE);
        }
    }

    SetNWebJavaScriptResultCallBack();
    NativeArkWeb_OnValidCallback validCallback = OH_NativeArkWeb_GetJavaScriptProxyValidCallback(webTag_.c_str());
    if (validCallback) {
        WVLOG_I("native validCallback start to call");
        (*validCallback)(webTag_.c_str());
    } else {
        WVLOG_W("native validCallback is null, callback nothing");
    }
}

void WebviewController::SetWebDetach(int32_t nwebId)
{
    if (nwebId != nwebId_) {
        WVLOG_W("Web detach nwebId(%{public}d) is not equal to current nwebId(%{public}d)", nwebId, nwebId_);
        return;
    }

    if (attachState_ != AttachState::NOT_ATTACHED) {
        std::unique_lock<std::mutex> attachLock(attachMtx_);
        attachState_ = AttachState::NOT_ATTACHED;
        TriggerStateChangeCallback(CONTROLLER_ATTACH_STATE_CHANGE);
    }
}

WebviewController* WebviewController::FromID(int32_t nwebId)
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    if (auto it = g_webview_controller_map.find(nwebId); it != g_webview_controller_map.end()) {
        auto control = it->second;
        return control;
    }
    return nullptr;
}

void WebviewController::InnerCompleteWindowNew(int32_t parentNwebId)
{
    WVLOG_D("WebviewController::InnerCompleteWindowNew parentNwebId == "
            "%{public}d ",
        parentNwebId);
    if (parentNwebId < 0) {
        WVLOG_E("WebviewController::InnerCompleteWindowNew parentNwebId == %{public}d "
                "error",
            parentNwebId);
        return;
    }
    auto parentControl = FromID(parentNwebId);
    if (!parentControl || !(parentControl->javaScriptResultCb_)) {
        WVLOG_E("WebviewController::InnerCompleteWindowNew parentControl or "
                "javaScriptResultCb_ is null");
        return;
    }

    auto parNamedObjs = parentControl->javaScriptResultCb_->GetNamedObjects();

    auto currentControl = FromID(nwebId_);
    if (!currentControl || !(currentControl->javaScriptResultCb_)) {
        WVLOG_E("WebviewController::InnerCompleteWindowNew currentControl or "
                "javaScriptResultCb_ is null");
        return;
    }

    std::unique_lock<std::mutex> lock(webMtx_);
    {
        auto curNamedObjs = currentControl->javaScriptResultCb_->GetNamedObjects();
        SetNWebJavaScriptResultCallBack();
        for (auto it = parNamedObjs.begin(); it != parNamedObjs.end(); it++) {
            if (curNamedObjs.find(it->first) != curNamedObjs.end()) {
                continue;
            }
            if (it->second && IsInit()) {
                RegisterJavaScriptProxyParam param;
                param.env = it->second->GetEnv();
                param.obj = it->second->GetValue();
                param.objName = it->first;
                param.syncMethodList = it->second->GetSyncMethodNames();
                param.asyncMethodList = it->second->GetAsyncMethodNames();
                param.permission = it->second->GetPermission();
                RegisterJavaScriptProxy(param);
            }
        }
    }
}

bool WebviewController::IsInit() const
{
    return NWebHelper::Instance().GetNWeb(nwebId_) ? true : false;
}

bool WebviewController::AccessForward() const
{
    bool access = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        access = nweb_ptr->IsNavigateForwardAllowed();
    } else {
        WVLOG_E("WebviewController::AccessForward nweb_ptr is null");
    }
    return access;
}

bool WebviewController::AccessBackward() const
{
    bool access = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        access = nweb_ptr->IsNavigatebackwardAllowed();
    }
    return access;
}

bool WebviewController::AccessStep(int32_t step) const
{
    bool access = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        access = nweb_ptr->CanNavigateBackOrForward(step);
    }
    return access;
}

void WebviewController::ClearHistory()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->DeleteNavigateHistory();
    }
}

void WebviewController::Forward()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->NavigateForward();
    }
}

void WebviewController::Backward()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->NavigateBack();
    }
}

void WebviewController::OnActive()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->OnContinue();
    }
}

void WebviewController::OnInactive()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->OnPause();
    }
}

void WebviewController::Refresh()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->Reload();
    }
}

void WebviewController::ReloadIgnoreCache()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ReloadIgnoreCache();
    }
}

ErrCode WebviewController::ZoomIn()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_ptr->ZoomIn();

    return result;
}

ErrCode WebviewController::ZoomOut()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_ptr->ZoomOut();

    return result;
}

int32_t WebviewController::GetWebId() const
{
    int32_t webId = -1;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        webId = static_cast<int32_t>(nweb_ptr->GetWebId());
    }
    return webId;
}

std::string WebviewController::GetUserAgent()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return "";
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return "";
    }
    return setting->DefaultUserAgent();
}

std::string WebviewController::GetCustomUserAgent() const
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return "";
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return "";
    }
    return setting->UserAgent();
}

ErrCode WebviewController::SetCustomUserAgent(const std::string& userAgent)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return NWebError::INIT_ERROR;
    }
    setting->PutUserAgent(userAgent);
    return NWebError::NO_ERROR;
}

std::string WebviewController::GetTitle()
{
    std::string title = "";
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        title = nweb_ptr->Title();
    }
    return title;
}

int32_t WebviewController::GetProgress()
{
    int32_t progress = 0;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        progress = nweb_ptr->PageLoadProgress();
    }
    return progress;
}

int32_t WebviewController::GetPageHeight()
{
    int32_t pageHeight = 0;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        pageHeight = nweb_ptr->ContentHeight();
    }
    return pageHeight;
}

ErrCode WebviewController::BackOrForward(int32_t step)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->NavigateBackOrForward(step);
    return NWebError::NO_ERROR;
}

void WebviewController::StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env,
    napi_ref jsCallback)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        napi_value setResult[RESULT_COUNT] = {0};
        setResult[PARAMZERO] = BusinessError::CreateError(env, NWebError::INIT_ERROR);
        napi_get_null(env, &setResult[PARAMONE]);

        napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
        napi_value callback = nullptr;
        napi_get_reference_value(env, jsCallback, &callback);
        napi_value callbackResult = nullptr;
        napi_call_function(env, nullptr, callback, RESULT_COUNT, args, &callbackResult);
        napi_delete_reference(env, jsCallback);
        return;
    }

    if (jsCallback == nullptr) {
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebStoreWebArchiveCallback>();
    callbackImpl->SetCallBack([env, jCallback = std::move(jsCallback)](std::string result) {
        if (!env) {
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }

        napi_value setResult[RESULT_COUNT] = {0};
        if (result.empty()) {
            setResult[PARAMZERO] = BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);
            napi_get_null(env, &setResult[PARAMONE]);
        } else {
            napi_get_undefined(env, &setResult[PARAMZERO]);
            napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &setResult[PARAMONE]);
        }
        napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
        napi_value callback = nullptr;
        napi_get_reference_value(env, jCallback, &callback);
        napi_value callbackResult = nullptr;
        napi_call_function(env, nullptr, callback, RESULT_COUNT, args, &callbackResult);

        napi_delete_reference(env, jCallback);
        napi_close_handle_scope(env, scope);
    });
    nweb_ptr->StoreWebArchive(baseName, autoName, callbackImpl);
    return;
}

void WebviewController::StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env,
    napi_deferred deferred)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        napi_value jsResult = nullptr;
        jsResult = NWebError::BusinessError::CreateError(env, NWebError::INIT_ERROR);
        napi_reject_deferred(env, deferred, jsResult);
        return;
    }

    if (deferred == nullptr) {
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebStoreWebArchiveCallback>();
    callbackImpl->SetCallBack([env, deferred](std::string result) {
        if (!env) {
            return;
        }
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }

        napi_value setResult[RESULT_COUNT] = {0};
        setResult[PARAMZERO] = NWebError::BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);
        napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &setResult[PARAMONE]);
        napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
        if (!result.empty()) {
            napi_resolve_deferred(env, deferred, args[PARAMONE]);
        } else {
            napi_reject_deferred(env, deferred, args[PARAMZERO]);
        }
        napi_close_handle_scope(env, scope);
    });
    nweb_ptr->StoreWebArchive(baseName, autoName, callbackImpl);
    return;
}

std::vector<std::string> WebviewController::CreateWebMessagePorts()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        std::vector<std::string> empty;
        return empty;
    }

    return nweb_ptr->CreateWebMessagePorts();
}

ErrCode WebviewController::PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->PostWebMessage(message, ports, targetUrl);
    return NWebError::NO_ERROR;
}

std::shared_ptr<HitTestResult> WebviewController::GetHitTestValue()
{
    std::shared_ptr<HitTestResult> nwebResult;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nwebResult = nweb_ptr->GetHitTestResult();
        if (nwebResult) {
            nwebResult->SetType(ConverToWebHitTestType(nwebResult->GetType()));
        }
    }
    return nwebResult;
}

void WebviewController::RequestFocus()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->OnFocus();
    }
}

std::string WebviewController::GenerateWebTag()
{
    std::string webTag = "arkweb:" + std::to_string(WebviewController::webTagStrId_);
    while (WebviewController::webTagSet_.find(webTag) != WebviewController::webTagSet_.end()) {
        WebviewController::webTagStrId_++;
        webTag = "arkweb:" + std::to_string(WebviewController::webTagStrId_);
    }
    return webTag;
}

bool WebviewController::GetRawFileUrl(const std::string &fileName,
    const std::string& bundleName, const std::string& moduleName, std::string &result) const
{
    if (fileName.empty()) {
        WVLOG_E("File name is empty.");
        return false;
    }
    if (hapPath_.empty()) {
        std::shared_ptr<AbilityRuntime::ApplicationContext> context =
            AbilityRuntime::ApplicationContext::GetApplicationContext();
        std::string packagePath = "file:///" + context->GetBundleCodeDir() + "/";
        std::string contextBundleName = context->GetBundleName() + "/";
        std::shared_ptr<AppExecFwk::ApplicationInfo> appInfo = context->GetApplicationInfo();
        std::string entryDir = appInfo->entryDir;
        bool isStage = entryDir.find("entry") == std::string::npos ? false : true;
        result = isStage ? packagePath + "entry/resources/rawfile/" + fileName :
            packagePath + contextBundleName + "assets/entry/resources/rawfile/" + fileName;
    } else {
        std::string appBundleName;
        std::string appModuleName;
        result = "resource://RAWFILE/";
        if (!bundleName.empty() && !moduleName.empty() &&
            GetAppBundleNameAndModuleName(appBundleName, appModuleName)) {
            if (appBundleName != bundleName || appModuleName != moduleName) {
                result += BUNDLE_NAME_PREFIX + bundleName + "/" + MODULE_NAME_PREFIX + moduleName + "/";
            }
        }
        result += fileName;
    }
    WVLOG_D("The parsed url is: ***");
    return true;
}

bool WebviewController::ParseUrl(napi_env env, napi_value urlObj, std::string& result) const
{
    napi_valuetype valueType = napi_null;
    napi_typeof(env, urlObj, &valueType);
    if ((valueType != napi_object) && (valueType != napi_string)) {
        WVLOG_E("Unable to parse url object.");
        return false;
    }
    if (valueType == napi_string) {
        NapiParseUtils::ParseString(env, urlObj, result);
        WVLOG_D("The parsed url is: ***");
        return true;
    }
    napi_value type = nullptr;
    napi_valuetype typeVlueType = napi_null;
    napi_get_named_property(env, urlObj, "type", &type);
    napi_typeof(env, type, &typeVlueType);
    if (typeVlueType == napi_number) {
        int32_t typeInteger;
        NapiParseUtils::ParseInt32(env, type, typeInteger);
        if (typeInteger == static_cast<int>(ResourceType::RAWFILE)) {
            return ParseRawFileUrl(env, urlObj, result);
        } else if (typeInteger == static_cast<int>(ResourceType::STRING)) {
            if (!GetResourceUrl(env, urlObj, result)) {
                WVLOG_E("Unable to parse string from url object.");
                return false;
            }
            return true;
        }
        WVLOG_E("The type parsed from url object is not RAWFILE.");
        return false;
    }
    WVLOG_E("Unable to parse type from url object.");
    return false;
}

bool WebviewController::ParseRawFileUrl(napi_env env, napi_value urlObj, std::string& result) const
{
    napi_value paraArray = nullptr;
    napi_get_named_property(env, urlObj, "params", &paraArray);
    bool isArray = false;
    napi_is_array(env, paraArray, &isArray);
    if (!isArray) {
        WVLOG_E("Unable to parse parameter array from url object.");
        return false;
    }
    napi_value fileNameObj;
    napi_value bundleNameObj;
    napi_value moduleNameObj;
    std::string fileName;
    std::string bundleName;
    std::string moduleName;
    napi_get_element(env, paraArray, 0, &fileNameObj);
    napi_get_named_property(env, urlObj, "bundleName", &bundleNameObj);
    napi_get_named_property(env, urlObj, "moduleName", &moduleNameObj);
    NapiParseUtils::ParseString(env, fileNameObj, fileName);
    NapiParseUtils::ParseString(env, bundleNameObj, bundleName);
    NapiParseUtils::ParseString(env, moduleNameObj, moduleName);
    return GetRawFileUrl(fileName, bundleName, moduleName, result);
}

bool WebviewController::GetResourceUrl(napi_env env, napi_value urlObj, std::string& result) const
{
    napi_value resIdObj = nullptr;
    napi_value bundleNameObj = nullptr;
    napi_value moduleNameObj = nullptr;

    int32_t resId;
    std::string bundleName;
    std::string moduleName;

    if ((napi_get_named_property(env, urlObj, "id", &resIdObj) != napi_ok) ||
        (napi_get_named_property(env, urlObj, "bundleName", &bundleNameObj) != napi_ok) ||
        (napi_get_named_property(env, urlObj, "moduleName", &moduleNameObj) != napi_ok)) {
        return false;
    }

    if (!NapiParseUtils::ParseInt32(env, resIdObj, resId) ||
        !NapiParseUtils::ParseString(env, bundleNameObj, bundleName) ||
        !NapiParseUtils::ParseString(env, moduleNameObj, moduleName)) {
        return false;
    }

    if (OhosResourceAdapterImpl::GetResourceString(bundleName, moduleName, resId, result)) {
        return true;
    }
    return false;
}

ErrCode WebviewController::PostUrl(std::string& url, std::vector<char>& postData)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    return nweb_ptr->PostUrl(url, postData);
}

ErrCode WebviewController::LoadUrl(std::string url)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    return nweb_ptr->Load(url);
}

ErrCode WebviewController::LoadUrl(std::string url, std::map<std::string, std::string> httpHeaders)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    return nweb_ptr->Load(url, httpHeaders);
}

ErrCode WebviewController::LoadData(std::string data, std::string mimeType, std::string encoding,
    std::string baseUrl, std::string historyUrl)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    if (baseUrl.empty() && historyUrl.empty()) {
        return nweb_ptr->LoadWithData(data, mimeType, encoding);
    }
    return nweb_ptr->LoadWithDataAndBaseUrl(baseUrl, data, mimeType, encoding, historyUrl);
}

int WebviewController::ConverToWebHitTestType(int hitType)
{
    WebHitTestType webHitType;
    switch (hitType) {
        case HitTestResult::UNKNOWN_TYPE:
            webHitType = WebHitTestType::UNKNOWN;
            break;
        case HitTestResult::ANCHOR_TYPE:
            webHitType = WebHitTestType::HTTP;
            break;
        case HitTestResult::PHONE_TYPE:
            webHitType = WebHitTestType::PHONE;
            break;
        case HitTestResult::GEO_TYPE:
            webHitType = WebHitTestType::MAP;
            break;
        case HitTestResult::EMAIL_TYPE:
            webHitType = WebHitTestType::EMAIL;
            break;
        case HitTestResult::IMAGE_TYPE:
            webHitType = WebHitTestType::IMG;
            break;
        case HitTestResult::IMAGE_ANCHOR_TYPE:
            webHitType = WebHitTestType::HTTP_IMG;
            break;
        case HitTestResult::SRC_ANCHOR_TYPE:
            webHitType = WebHitTestType::HTTP;
            break;
        case HitTestResult::SRC_IMAGE_ANCHOR_TYPE:
            webHitType = WebHitTestType::HTTP_IMG;
            break;
        case HitTestResult::EDIT_TEXT_TYPE:
            webHitType = WebHitTestType::EDIT;
            break;
        default:
            webHitType = WebHitTestType::UNKNOWN;
            break;
    }
    return static_cast<int>(webHitType);
}

int WebviewController::GetHitTest()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        std::shared_ptr<HitTestResult> nwebResult = nweb_ptr->GetHitTestResult();
        if (nwebResult) {
            return ConverToWebHitTestType(nwebResult->GetType());
        } else {
            return ConverToWebHitTestType(HitTestResult::UNKNOWN_TYPE);
        }
    }
    return static_cast<int>(WebHitTestType::UNKNOWN);
}


void WebviewController::ClearMatches()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ClearMatches();
    }
}

void WebviewController::SearchNext(bool forward)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->FindNext(forward);
    }
}

void WebviewController::EnableSafeBrowsing(bool enable)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->EnableSafeBrowsing(enable);
    }
}

bool WebviewController::IsSafeBrowsingEnabled() const
{
    bool isSafeBrowsingEnabled = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        isSafeBrowsingEnabled = nweb_ptr->IsSafeBrowsingEnabled();
    }
    return isSafeBrowsingEnabled;
}

void WebviewController::SearchAllAsync(const std::string& searchString)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->FindAllAsync(searchString);
    }
}

void WebviewController::ClearSslCache()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ClearSslCache();
    }
}

void WebviewController::ClearClientAuthenticationCache()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ClearClientAuthenticationCache();
    }
}

void WebviewController::Stop()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->Stop();
    }
}

ErrCode WebviewController::Zoom(float factor)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_ptr->Zoom(factor);

    return result;
}

ErrCode WebviewController::DeleteJavaScriptRegister(const std::string& objName,
    const std::vector<std::string>& methodList)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->UnregisterArkJSfunction(objName, methodList);
    }

    if (javaScriptResultCb_) {
        bool ret = javaScriptResultCb_->DeleteJavaScriptRegister(objName);
        if (!ret) {
            return CANNOT_DEL_JAVA_SCRIPT_PROXY;
        }
    }

    return NWebError::NO_ERROR;
}

void WebviewController::SetNWebJavaScriptResultCallBack()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }

    if (javaScriptResultCb_ && (javaScriptResultCb_->GetNWebId() == nwebId_)) {
        return;
    }

    javaScriptResultCb_ = std::make_shared<WebviewJavaScriptResultCallBack>(nwebId_);
    nweb_ptr->SetNWebJavaScriptResultCallBack(javaScriptResultCb_);
}

void WebviewController::RegisterJavaScriptProxy(RegisterJavaScriptProxyParam& param)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy nweb_ptr is null");
        return;
    }
    JavaScriptOb::ObjectID objId =
        static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBCONTROLLERERROR);

    if (!javaScriptResultCb_) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy javaScriptResultCb_ is "
                "null");
        return;
    }

    if (param.syncMethodList.empty() && param.asyncMethodList.empty()) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy all methodList are "
                "empty");
        return;
    }

    std::vector<std::string> allMethodList;
    std::merge(param.syncMethodList.begin(), param.syncMethodList.end(),
               param.asyncMethodList.begin(), param.asyncMethodList.end(),
               std::back_inserter(allMethodList));

    RegisterJavaScriptProxyParam param_tmp;
    param_tmp.env = param.env;
    param_tmp.obj = param.obj;
    param_tmp.objName = param.objName;
    param_tmp.syncMethodList = allMethodList;
    param_tmp.asyncMethodList = param.asyncMethodList;
    param_tmp.permission = param.permission;
    objId = javaScriptResultCb_->RegisterJavaScriptProxy(param_tmp);

    nweb_ptr->RegisterArkJSfunction(param_tmp.objName, param_tmp.syncMethodList,
                                    std::vector<std::string>(), objId, param_tmp.permission);
}

void WebviewController::RegisterJavaScriptProxy(AniRegisterJavaScriptProxyParam& param)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy nweb_ptr is null");
        return;
    }
    JavaScriptOb::ObjectID objId =
        static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBCONTROLLERERROR);

    if (!javaScriptResultCb_) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy javaScriptResultCb_ is null");
        return;
    }

    if (param.syncMethodList.empty() && param.asyncMethodList.empty()) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy all methodList are empty");
        return;
    }

    std::vector<std::string> allMethodList;
    std::merge(param.syncMethodList.begin(), param.syncMethodList.end(), param.asyncMethodList.begin(),
        param.asyncMethodList.end(), std::back_inserter(allMethodList));

    AniRegisterJavaScriptProxyParam tmp;
    tmp.env = param.env;
    tmp.obj = param.obj;
    tmp.objName = param.objName;
    tmp.syncMethodList = allMethodList;
    tmp.asyncMethodList = param.asyncMethodList;
    tmp.permission = param.permission;
    tmp.webviewObj = param.webviewObj;
    objId = javaScriptResultCb_->RegisterJavaScriptProxy(tmp);

    nweb_ptr->RegisterArkJSfunction(tmp.objName, tmp.syncMethodList, std::vector<std::string>(), objId, tmp.permission);
}

void WebviewController::CreatePDFExt(
    std::shared_ptr<NWebPDFConfigArgs> pdfConfig, std::shared_ptr<NWebArrayBufferValueCallback> callbackImpl)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("nweb_ptr is nullptr");
        return;
    }
    if (callbackImpl == nullptr) {
        WVLOG_E("callbackImpl is nullptr");
        return;
    }
    nweb_ptr->ExecuteCreatePDFExt(pdfConfig, callbackImpl);
}

std::string WebviewController::GetUrl()
{
    std::string url = "";
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        url = nweb_ptr->GetUrl();
    }
    return url;
}

std::string WebviewController::GetOriginalUrl()
{
    std::string url = "";
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        url = nweb_ptr->GetOriginalUrl();
    }
    return url;
}

bool WebviewController::TerminateRenderProcess() const
{
    bool ret = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        ret = nweb_ptr->TerminateRenderProcess();
    }
    return ret;
}

void WebviewController::PutNetworkAvailable(bool available)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->PutNetworkAvailable(available);
    }
}

ErrCode WebviewController::HasImagesCallback(ani_vm *vm, ani_ref jsCallback)
{
    if (vm == nullptr) {
        WVLOG_E("vm is nullptr");
        return NWebError::INIT_ERROR;
    }
    ani_env* env = nullptr;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return NWebError::INIT_ERROR;
    }
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return NWebError::INIT_ERROR;
    }
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        std::vector<ani_ref> vec;
        ani_ref errorValue = AniBusinessErrorError::CreateError(env, NWebError::INIT_ERROR);
        ani_ref dataValue = nullptr;
        auto status = env->GetUndefined(&dataValue);
        if (status != ANI_OK) {
            WVLOG_E("get null failed, status is : %{public}d", status);
            return NWebError::INIT_ERROR;
        }
        if (errorValue == nullptr || dataValue == nullptr) {
            WVLOG_E("errorValue or dataValue is nullptr");
            return NWebError::INIT_ERROR;
        }
        vec.push_back(errorValue);
        vec.push_back(dataValue);
        ani_ref callbackResult = nullptr;
        auto stat = env->FunctionalObject_Call(
            reinterpret_cast<ani_fn_object>(jsCallback), vec.size(), vec.data(), &callbackResult);
        if (stat != ANI_OK) {
            WVLOG_E("FunctionalObject_Call failed, status is : %{public}d", stat);
            return NWebError::INIT_ERROR;
        }
        env->GlobalReference_Delete(jsCallback);
        return NWebError::INIT_ERROR;
    }

    if (jsCallback == nullptr) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto callbackImpl = std::make_shared<WebviewHasImageCallback>(vm, jsCallback, nullptr);
    if (callbackImpl) {
        nweb_ptr->HasImages(callbackImpl);
        return NWebError::NO_ERROR;
    }
    return NWebError::PARAM_CHECK_ERROR;
}

ErrCode WebviewController::HasImagesPromise(ani_vm *vm, ani_resolver deferred)
{
    if (vm == nullptr) {
        WVLOG_E("vm is nullptr");
        return NWebError::INIT_ERROR;
    }
    ani_env* env = nullptr;
    if (vm->GetEnv(ANI_VERSION_1, &env) != ANI_OK) {
        return NWebError::INIT_ERROR;
    }
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return NWebError::INIT_ERROR;
    }
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        ani_ref jsResult = nullptr;
        jsResult = AniBusinessErrorError::CreateError(env, NWebError::INIT_ERROR);
        if (jsResult == nullptr) {
            return NWebError::INIT_ERROR;
        }
        auto status = env->PromiseResolver_Reject(deferred, static_cast<ani_error>(jsResult));
        if (status != ANI_OK) {
            WVLOG_E("PromiseResolver_Reject failed, status is : %{public}d", status);
            return NWebError::INIT_ERROR;
        }
        return NWebError::INIT_ERROR;
    }

    if (deferred == nullptr) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto callbackImpl = std::make_shared<WebviewHasImageCallback>(vm, nullptr, deferred);
    if (callbackImpl) {
        nweb_ptr->HasImages(callbackImpl);
        return NWebError::NO_ERROR;
    }
    return NWebError::PARAM_CHECK_ERROR;
}

void WebviewController::RemoveCache(bool includeDiskFiles)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->RemoveCache(includeDiskFiles);
    }
}

std::shared_ptr<NWebHistoryList> WebviewController::GetHistoryList()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return nullptr;
    }
    return nweb_ptr->GetHistoryList();
}

bool WebviewController::GetFavicon(
    const void **data, size_t &width, size_t &height, ImageColorType &colorType, ImageAlphaType &alphaType) const
{
    bool isGetFavicon = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        isGetFavicon = nweb_ptr->GetFavicon(data, width, height, colorType, alphaType);
    }
    return isGetFavicon;
}

std::vector<uint8_t> WebviewController::SerializeWebState()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        return nweb_ptr->SerializeWebState();
    }
    std::vector<uint8_t> empty;
    return empty;
}

bool WebviewController::RestoreWebState(const std::vector<uint8_t> &state) const
{
    bool isRestored = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        isRestored = nweb_ptr->RestoreWebState(state);
    }
    return isRestored;
}

void WebviewController::ScrollPageDown(bool bottom)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->PageDown(bottom);
    }
    return;
}

void WebviewController::ScrollPageUp(bool top)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->PageUp(top);
    }
    return;
}

void WebviewController::ScrollTo(float x, float y)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ScrollTo(x, y);
    }
    return;
}

void WebviewController::ScrollBy(float deltaX, float deltaY)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ScrollBy(deltaX, deltaY);
    }
    return;
}

void WebviewController::SlideScroll(float vx, float vy)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->SlideScroll(vx, vy);
    }
    return;
}

void WebviewController::SetScrollable(bool enable)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return;
    }
    return setting->SetScrollable(enable);
}

bool WebviewController::GetScrollable() const
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return true;
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return true;
    }
    return setting->GetScrollable();
}

void WebviewController::InnerSetHapPath(const std::string &hapPath)
{
    hapPath_ = hapPath;
}

bool WebviewController::GetCertChainDerData(std::vector<std::string> &certChainDerData) const
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("GetCertChainDerData failed, nweb ptr is null");
        return false;
    }

    return nweb_ptr->GetCertChainDerData(certChainDerData, true);
}

ErrCode WebviewController::SetAudioMuted(bool muted)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->SetAudioMuted(muted);
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->PrefetchPage(url, additionalHttpHeaders);
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders,
                                        std::shared_ptr<NWebPrefetchOptions> prefetchOptions)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->PrefetchPageV2(url, additionalHttpHeaders,
        prefetchOptions->GetminTimeBetweenPrefetchesMs(), prefetchOptions->GetignoreCacheControlNoStore());
    return NWebError::NO_ERROR;
}

void WebPrintDocument::OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
    const PrintAttributesAdapter& newAttrs, uint32_t fd, std::function<void(std::string, uint32_t)> writeResultCallback)
{
    if (printDocAdapter_) {
        std::shared_ptr<PrintWriteResultCallbackAdapter> callback =
            std::make_shared<WebPrintWriteResultCallbackAdapter>(writeResultCallback);
        printDocAdapter_->OnStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, callback);
    }
}

void WebPrintDocument::OnJobStateChanged(const std::string& jobId, uint32_t state)
{
    if (printDocAdapter_) {
        printDocAdapter_->OnJobStateChanged(jobId, state);
    }
}

void* WebviewController::CreateWebPrintDocumentAdapter(const std::string& jobName)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return nullptr;
    }
    return nweb_ptr->CreateWebPrintDocumentAdapter(jobName);
}

void WebviewController::CloseAllMediaPresentations()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->CloseAllMediaPresentations();
    }
}

void WebviewController::StopAllMedia()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->StopAllMedia();
    }
}

void WebviewController::ResumeAllMedia()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ResumeAllMedia();
    }
}

void WebviewController::PauseAllMedia()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->PauseAllMedia();
    }
}

int WebviewController::GetMediaPlaybackState()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return static_cast<int>(MediaPlaybackState::NONE);
    }
    return nweb_ptr->GetMediaPlaybackState();
}

ErrCode WebviewController::AvoidVisibleViewportBottom(int32_t avoidHeight)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    nweb_ptr->AvoidVisibleViewportBottom(avoidHeight);
    return NWebError::NO_ERROR;
}

int WebviewController::GetSecurityLevel(ani_env* env)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return static_cast<int>(SecurityLevel::NONE);
    }

    int nwebSecurityLevel = nweb_ptr->GetSecurityLevel();
    SecurityLevel securityLevel;
    switch (nwebSecurityLevel) {
        case static_cast<int>(CoreSecurityLevel::NONE):
            securityLevel = SecurityLevel::NONE;
            break;
        case static_cast<int>(CoreSecurityLevel::SECURE):
            securityLevel = SecurityLevel::SECURE;
            break;
        case static_cast<int>(CoreSecurityLevel::WARNING):
            securityLevel = SecurityLevel::WARNING;
            break;
        case static_cast<int>(CoreSecurityLevel::DANGEROUS):
            securityLevel = SecurityLevel::DANGEROUS;
            break;
        default:
            securityLevel = SecurityLevel::NONE;
            break;
    }

    return static_cast<int>(securityLevel);
}

bool WebviewController::IsIncognitoMode() const
{
    bool incognitoMode = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        incognitoMode = nweb_ptr->IsIncognitoMode();
    }
    return incognitoMode;
}

void WebviewController::SetPrintBackground(bool enable)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->SetPrintBackground(enable);
    }
}

bool  WebviewController::GetPrintBackground() const
{
    bool printBackgroundEnabled = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        printBackgroundEnabled = nweb_ptr->GetPrintBackground();
    }

    return printBackgroundEnabled;
}

void WebviewController::EnableIntelligentTrackingPrevention(bool enable)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->EnableIntelligentTrackingPrevention(enable);
    }
}

bool WebviewController::IsIntelligentTrackingPreventionEnabled() const
{
    bool enabled = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        enabled = nweb_ptr->IsIntelligentTrackingPreventionEnabled();
    }
    return enabled;
}

void WebPrintWriteResultCallbackAdapter::WriteResultCallback(std::string jobId, uint32_t code)
{
    cb_(jobId, code);
}

bool WebviewController::SetWebSchemeHandler(const char* scheme, WebSchemeHandler* handler) const
{
    if (!handler || !scheme) {
        WVLOG_E("WebviewController::SetWebSchemeHandler handler or scheme is nullptr");
        return false;
    }

    auto schemeHandler_ptr = WebSchemeHandler::GetArkWebSchemeHandler(handler);
    if (!schemeHandler_ptr) {
        WVLOG_E("WebviewController::SetWebSchemeHandler ArkWebSchemeHandler is nullptr");
        return false;
    }

    ArkWeb_SchemeHandler* schemeHandler = const_cast<ArkWeb_SchemeHandler*>(schemeHandler_ptr);
    return OH_ArkWeb_SetSchemeHandler(scheme, webTag_.c_str(), schemeHandler);
}

bool WebviewController::SetWebServiveWorkerSchemeHandler(const char* scheme, WebSchemeHandler* handler)
{
    auto schemeHandler_ptr = WebSchemeHandler::GetArkWebSchemeHandler(handler);
    if (!schemeHandler_ptr) {
        WVLOG_E("WebviewController::SetWebServiveWorkerSchemeHandler ArkWebSchemeHandler is nullptr");
        return false;
    }
    ArkWeb_SchemeHandler* schemeHandler = const_cast<ArkWeb_SchemeHandler*>(schemeHandler_ptr);
    return OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler);
}

int32_t WebviewController::ClearWebSchemeHandler()
{
    return OH_ArkWeb_ClearSchemeHandlers(webTag_.c_str());
}

int32_t WebviewController::ClearWebServiceWorkerSchemeHandler()
{
    return OH_ArkWebServiceWorker_ClearSchemeHandlers();
}

ErrCode WebviewController::StartCamera()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->StartCamera();
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::StopCamera()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->StopCamera();
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::CloseCamera()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->CloseCamera();
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::ResumeMicrophone()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->ResumeMicrophone();
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::StopMicrophone()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->StopMicrophone();
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::PauseMicrophone()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    nweb_ptr->PauseMicrophone();
    return NWebError::NO_ERROR;
}

std::string WebviewController::GetLastJavascriptProxyCallingFrameUrl()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return "";
    }

    return nweb_ptr->GetLastJavascriptProxyCallingFrameUrl();
}

bool WebviewController::ParseScriptContent(napi_env env, napi_value value, std::string &script)
{
    napi_valuetype valueType;
    napi_typeof(env, value, &valueType);
    if (valueType == napi_string) {
        std::string str;
        if (!NapiParseUtils::ParseString(env, value, str)) {
            WVLOG_E("PrecompileJavaScript: parse script text to string failed.");
            return false;
        }

        script = str;
        return true;
    }

    std::vector<uint8_t> vec = ParseUint8Array(env, value);
    if (!vec.size()) {
        WVLOG_E("PrecompileJavaScript: parse script text to Uint8Array failed.");
        return false;
    }

    std::string str(vec.begin(), vec.end());
    script = str;
    return true;
}

std::shared_ptr<CacheOptions> WebviewController::ParseCacheOptions(napi_env env, napi_value value)
{
    std::map<std::string, std::string> responseHeaders;
    auto defaultCacheOptions = std::make_shared<NWebCacheOptionsImpl>(responseHeaders);

    napi_value responseHeadersValue = nullptr;
    if (napi_get_named_property(env, value, "responseHeaders", &responseHeadersValue) != napi_ok) {
        WVLOG_D("PrecompileJavaScript: cannot get 'responseHeaders' of CacheOptions.");
        return defaultCacheOptions;
    }

    if (!ParseResponseHeaders(env, responseHeadersValue, responseHeaders)) {
        WVLOG_D("PrecompileJavaScript: parse 'responseHeaders' of CacheOptions failed. use default options");
        return defaultCacheOptions;
    }

    return std::make_shared<NWebCacheOptionsImpl>(responseHeaders);
}

void WebviewController::PrecompileJavaScriptPromise(
    napi_env env, napi_deferred deferred,
    const std::string &url, const std::string &script, std::shared_ptr<CacheOptions> cacheOptions)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr || !deferred) {
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebPrecompileCallback>();
    callbackImpl->SetCallback([env, deferred](int64_t result) {
        if (!env) {
            return;
        }

        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }

        napi_value setResult[RESULT_COUNT] = {0};
        napi_create_int64(env, result, &setResult[PARAMZERO]);
        napi_value args[RESULT_COUNT] = {setResult[PARAMZERO]};
        if (result == static_cast<int64_t>(PrecompileError::OK)) {
            napi_resolve_deferred(env, deferred, args[PARAMZERO]);
        } else {
            napi_reject_deferred(env, deferred, args[PARAMZERO]);
        }

        napi_close_handle_scope(env, scope);
    });

    nweb_ptr->PrecompileJavaScript(url, script, cacheOptions, callbackImpl);
}

int32_t WebviewController::PrecompileJavaScript(const std::string& url, const std::string& script,
    std::shared_ptr<OHOS::NWeb::CacheOptions> cacheOptions,
    std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callbackImpl)
{
    WVLOG_D("PrecompDleJavaScript begin");
    if (url.empty() || script.empty()) {
        WVLOG_E("url or script is empty");
        return NWebError::PARAM_CHECK_ERROR;
    }
    if (!callbackImpl) {
        WVLOG_E("PrecompileJavaScript !callbackImpl");
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("nweb_ptr is null");
        return NWebError::INIT_ERROR;
    }
    nweb_ptr->PrecompileJavaScript(url, script, cacheOptions, callbackImpl);
    return NWebError::NO_ERROR;
}

bool WebviewController::ParseResponseHeaders(napi_env env,
                                             napi_value value,
                                             std::map<std::string, std::string> &responseHeaders) const
{
    bool isArray = false;
    napi_is_array(env, value, &isArray);
    if (!isArray) {
        WVLOG_E("Response headers is not array.");
        return false;
    }

    uint32_t length = INTEGER_ZERO;
    napi_get_array_length(env, value, &length);
    for (uint32_t i = 0; i < length; i++) {
        std::string keyString;
        std::string valueString;
        napi_value header = nullptr;
        napi_value keyObj = nullptr;
        napi_value valueObj = nullptr;
        napi_get_element(env, value, i, &header);

        if (napi_get_named_property(env, header, "headerKey", &keyObj) != napi_ok ||
            !NapiParseUtils::ParseString(env, keyObj, keyString)) {
            continue;
        }

        if (napi_get_named_property(env, header, "headerValue", &valueObj) != napi_ok ||
            !NapiParseUtils::ParseString(env, valueObj, valueString)) {
            continue;
        }

        responseHeaders[keyString] = valueString;
    }

    return true;
}

ParseURLResult WebviewController::ParseURLList(napi_env env, napi_value value, std::vector<std::string>& urlList)
{
    if (!NapiParseUtils::ParseStringArray(env, value, urlList)) {
        return ParseURLResult::FAILED;
    }

    for (auto url : urlList) {
        if (!CheckURL(url)) {
            return ParseURLResult::INVALID_URL;
        }
    }

    return ParseURLResult::OK;
}

bool WebviewController::CheckURL(std::string& url) const
{
    if (url.size() > URL_MAXIMUM) {
        WVLOG_E("The URL exceeds the maximum length of %{public}d. URL: %{private}s", URL_MAXIMUM, url.c_str());
        return false;
    }

    if (!regex_match(url, std::regex("^http(s)?:\\/\\/.+", std::regex_constants::icase))) {
        WVLOG_E("The Parse URL error. URL: %{private}s", url.c_str());
        return false;
    }

    return true;
}

std::vector<uint8_t> WebviewController::ParseUint8Array(napi_env env, napi_value value)
{
    napi_typedarray_type typedArrayType;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    napi_get_typedarray_info(env, value, &typedArrayType, &length, nullptr, &buffer, &offset);
    if (typedArrayType != napi_uint8_array) {
        WVLOG_E("Param is not Unit8Array.");
        return std::vector<uint8_t>();
    }

    uint8_t *data = nullptr;
    size_t total = 0;
    napi_get_arraybuffer_info(env, buffer, reinterpret_cast<void **>(&data), &total);
    length = std::min<size_t>(length, total - offset);
    std::vector<uint8_t> vec(length);
    int retCode = memcpy_s(vec.data(), vec.size(), &data[offset], length);
    if (retCode != 0) {
        WVLOG_E("Parse Uint8Array failed.");
        return std::vector<uint8_t>();
    }

    return vec;
}

void WebviewController::InjectOfflineResource(const std::vector<std::string>& urlList,
                                              const std::vector<uint8_t>& resource,
                                              const std::map<std::string, std::string>& response_headers,
                                              const uint32_t type)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }

    std::string originUrl = urlList[0];
    if (urlList.size() == 1) {
        nweb_ptr->InjectOfflineResource(originUrl, originUrl, resource, response_headers, type);
        return;
    }

    for (size_t i = 1 ; i < urlList.size() ; i++) {
        nweb_ptr->InjectOfflineResource(urlList[i], originUrl, resource, response_headers, type);
    }
}

void WebviewController::EnableAdsBlock(bool enable)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->EnableAdsBlock(enable);
    }
}

bool WebviewController::IsAdsBlockEnabled() const
{
    bool enabled = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        enabled = nweb_ptr->IsAdsBlockEnabled();
    }
    return enabled;
}

bool WebviewController::IsAdsBlockEnabledForCurPage() const
{
    bool enabled = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        enabled = nweb_ptr->IsAdsBlockEnabledForCurPage();
    }
    return enabled;
}

std::string WebviewController::GetSurfaceId()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return "";
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return "";
    }
    return setting->GetSurfaceId();
}

void WebviewController::UpdateInstanceId(int32_t newId)
{
    if (javaScriptResultCb_) {
        javaScriptResultCb_->UpdateInstanceId(newId);
    }
}

ErrCode WebviewController::SetUrlTrustList(const std::string& urlTrustList, std::string& detailErrMsg)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    int ret = NWebError::NO_ERROR;
    switch (nweb_ptr->SetUrlTrustListWithErrMsg(urlTrustList, detailErrMsg)) {
        case static_cast<int>(UrlListSetResult::INIT_ERROR):
            ret = NWebError::INIT_ERROR;
            break;
        case static_cast<int>(UrlListSetResult::PARAM_ERROR):
            ret = NWebError::PARAM_CHECK_ERROR;
            break;
        case static_cast<int>(UrlListSetResult::SET_OK):
            ret = NWebError::NO_ERROR;
            break;
        default:
            ret = NWebError::PARAM_CHECK_ERROR;
            break;
    }
    return ret;
}

ErrCode WebviewController::SetUrlTrustList(const std::string& urlTrustList,
    bool allowOpaqueOrigin, bool supportWildcard, std::string& detailErrMsg)
{
    auto nwebPtr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nwebPtr) {
        return NWebError::INIT_ERROR;
    }

    int ret = NWebError::NO_ERROR;
    int setUrlTrustListRet = nwebPtr->SetUrlTrustListWithErrMsg(urlTrustList,
        allowOpaqueOrigin, supportWildcard, detailErrMsg);
    switch (setUrlTrustListRet) {
        case static_cast<int>(UrlListSetResult::INIT_ERROR):
            ret = NWebError::INIT_ERROR;
            break;
        case static_cast<int>(UrlListSetResult::PARAM_ERROR):
            ret = NWebError::PARAM_CHECK_ERROR;
            break;
        case static_cast<int>(UrlListSetResult::SET_OK):
            ret = NWebError::NO_ERROR;
            break;
        default:
            ret = NWebError::PARAM_CHECK_ERROR;
            break;
    }
    return ret;
}

bool WebviewController::ParseJsLengthResourceToInt(
    napi_env env, napi_value jsLength, PixelUnit &type, int32_t &result) const
{
    napi_value resIdObj = nullptr;
    int32_t resId;

    if ((napi_get_named_property(env, jsLength, "id", &resIdObj) != napi_ok)) {
        return false;
    }

    if (!NapiParseUtils::ParseInt32(env, resIdObj, resId)) {
        return false;
    }

    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!context) {
        WVLOG_E("WebPageSnapshot Failed to get application context.");
        return false;
    }
    auto resourceManager = context->GetResourceManager();
    if (!resourceManager) {
        WVLOG_E("WebPageSnapshot Failed to get resource manager.");
        return false;
    }

    napi_value jsResourceType = nullptr;
    napi_valuetype resourceType = napi_null;
    napi_get_named_property(env, jsLength, "type", &jsResourceType);
    napi_typeof(env, jsResourceType, &resourceType);
    if (resourceType == napi_number) {
        int32_t resourceTypeNum;
        NapiParseUtils::ParseInt32(env, jsResourceType, resourceTypeNum);
        std::string resourceString;
        switch (resourceTypeNum) {
            case static_cast<int>(ResourceType::INTEGER):
                if (resourceManager->GetIntegerById(resId, result) == Global::Resource::SUCCESS) {
                    type = PixelUnit::VP;
                    return true;
                }
                break;
            case static_cast<int>(ResourceType::STRING):
                if (resourceManager->GetStringById(resId, resourceString) == Global::Resource::SUCCESS) {
                    return NapiParseUtils::ParseJsLengthStringToInt(resourceString, type, result);
                }
                break;
            default:
                break;
        }
        WVLOG_E("WebPageSnapshot resource type not support");
        return false;
    }
    WVLOG_E("WebPageSnapshot resource type error");
    return false;
}

bool WebviewController::ParseJsLengthToInt(
    napi_env env, napi_value jsLength, PixelUnit &type, int32_t &result) const
{
    napi_valuetype jsType = napi_null;
    napi_typeof(env, jsLength, &jsType);
    if ((jsType != napi_object) && (jsType != napi_string) && (jsType != napi_number)) {
        WVLOG_E("WebPageSnapshot Unable to parse js length object.");
        return false;
    }

    if (jsType == napi_number) {
        NapiParseUtils::ParseInt32(env, jsLength, result);
        type = PixelUnit::VP;
        return true;
    }

    if (jsType == napi_string) {
        std::string nativeString;
        NapiParseUtils::ParseString(env, jsLength, nativeString);
        if (!NapiParseUtils::ParseJsLengthStringToInt(nativeString, type, result)) {
            return false;
        }
        return true;
    }

    if (jsType == napi_object) {
        return ParseJsLengthResourceToInt(env, jsLength, type, result);
    }
    return false;
}

ErrCode WebviewController::WebPageSnapshot(
    const char *id, PixelUnit type, int32_t width, int32_t height, const WebSnapshotCallback callback)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    auto snapshotCallback =  std::make_shared<NWebSnapshotCallbackImpl>(std::move(callback));
    bool init = nweb_ptr->WebPageSnapshotV2(id, type, width, height, snapshotCallback);

    if (ArkWebGetErrno() != RESULT_OK) {
        WVLOG_I("WebPageSnapshotV2 isn't existing, using old");
        init = nweb_ptr->WebPageSnapshot(
            id, type, width, height, snapshotCallback->extract());
        snapshotCallback.reset();
    }

    if (!init) {
        return INIT_ERROR;
    }

    return NWebError::NO_ERROR;
}

bool WebviewController::GetHapModuleInfo()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
    SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        WVLOG_E("get SystemAbilityManager failed");
        return false;
    }
    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        WVLOG_E("get Bundle Manager failed");
        return false;
    }
    auto bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (bundleMgr == nullptr) {
        WVLOG_E("get Bundle Manager failed");
        return false;
    }
    AppExecFwk::BundleInfo bundleInfo;
    if (bundleMgr->GetBundleInfoForSelf(
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE),
        bundleInfo) != 0) {
        WVLOG_E("get bundle info failed");
        return false;
    }
    moduleName_ = bundleInfo.moduleNames;
    return true;
}

void WebviewController::SetPathAllowingUniversalAccess(
    const std::vector<std::string>& pathList, std::string& errorPath)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }
    if (moduleName_.empty()) {
        WVLOG_I("need to get module name for path");
        if (!GetHapModuleInfo()) {
            WVLOG_E("GetHapModuleInfo failed");
            moduleName_.clear();
            return;
        }
    }
    nweb_ptr->SetPathAllowingUniversalAccess(pathList, moduleName_, errorPath);
}

void WebviewController::ScrollToWithAnime(float x, float y, int32_t duration)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ScrollToWithAnime(x, y, duration);
    }
    return;
}

void WebviewController::ScrollByWithAnime(float deltaX, float deltaY, int32_t duration)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->ScrollByWithAnime(deltaX, deltaY, duration);
    }
    return;
}

void WebviewController::SetBackForwardCacheOptions(int32_t size, int32_t timeToLive)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }

    nweb_ptr->SetBackForwardCacheOptions(size, timeToLive);
}

void WebviewController::GetScrollOffset(float* offset_x, float* offset_y)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->GetScrollOffset(offset_x, offset_y);
    }
}

void WebviewController::GetPageOffset(float* offsetX, float* offsetY)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nweb_ptr->GetPageOffset(offsetX, offsetY);
    }
}

bool WebviewController::ScrollByWithResult(float deltaX, float deltaY) const
{
    bool enabled = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        enabled = nweb_ptr->ScrollByWithResult(deltaX, deltaY);
    }
    return enabled;
}

void WebviewController::SetScrollable(bool enable, int32_t scrollType)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return;
    }
    return setting->SetScrollable(enable, scrollType);
}

void WebviewController::SetSoftKeyboardBehaviorMode(int32_t mode)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }
    nweb_ptr->SetSoftKeyboardBehaviorMode(static_cast<WebSoftKeyboardBehaviorMode>(mode));
}

void WebMessageExt::SetType(int type)
{
    type_ = type;
    WebMessageType jsType = static_cast<WebMessageType>(type);
    NWebValue::Type nwebType = NWebValue::Type::NONE;
    switch (jsType) {
        case WebMessageType::STRING: {
            nwebType = NWebValue::Type::STRING;
            break;
        }
        case WebMessageType::NUMBER: {
            nwebType = NWebValue::Type::DOUBLE;
            break;
        }
        case WebMessageType::BOOLEAN: {
            nwebType = NWebValue::Type::BOOLEAN;
            break;
        }
        case WebMessageType::ARRAYBUFFER: {
            nwebType = NWebValue::Type::BINARY;
            break;
        }
        case WebMessageType::ARRAY: {
            nwebType = NWebValue::Type::STRINGARRAY;
            break;
        }
        case WebMessageType::ERROR: {
            nwebType = NWebValue::Type::ERROR;
            break;
        }
        default: {
            nwebType = NWebValue::Type::NONE;
            break;
        }
    }
    if (data_) {
        data_->SetType(nwebType);
    }
}

int WebMessageExt::ConvertNwebType2JsType(NWebValue::Type type)
{
    WebMessageType jsType = WebMessageType::NOTSUPPORT;
    switch (type) {
        case NWebValue::Type::STRING: {
            jsType = WebMessageType::STRING;
            break;
        }
        case NWebValue::Type::DOUBLE:
        case NWebValue::Type::INTEGER: {
            jsType = WebMessageType::NUMBER;
            break;
        }
        case NWebValue::Type::BOOLEAN: {
            jsType = WebMessageType::BOOLEAN;
            break;
        }
        case NWebValue::Type::STRINGARRAY:
        case NWebValue::Type::DOUBLEARRAY:
        case NWebValue::Type::INT64ARRAY:
        case NWebValue::Type::BOOLEANARRAY: {
            jsType = WebMessageType::ARRAY;
            break;
        }
        case NWebValue::Type::BINARY: {
            jsType = WebMessageType::ARRAYBUFFER;
            break;
        }
        case NWebValue::Type::ERROR: {
            jsType = WebMessageType::ERROR;
            break;
        }
        default: {
            jsType = WebMessageType::NOTSUPPORT;
            break;
        }
    }
    return static_cast<int>(jsType);
}

std::shared_ptr<HitTestResult> WebviewController::GetLastHitTest()
{
    std::shared_ptr<HitTestResult> nwebResult;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nwebResult = nweb_ptr->GetLastHitTestResult();
        if (nwebResult) {
            nwebResult->SetType(ConverToWebHitTestType(nwebResult->GetType()));
        }
    }
    return nwebResult;
}

void WebviewController::OnCreateNativeMediaPlayer(ani_vm* vm, ani_fn_object callback)
{
    WVLOG_I("put on_create_native_media_player callback");
    if (vm == nullptr) {
        WVLOG_E("vm is nullptr");
        return;
    }
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }

    auto callbackImpl = std::make_shared<NWebCreateNativeMediaPlayerCallbackImpl>(nwebId_, vm, callback);
    if(!callbackImpl){
        return;
    }
    nweb_ptr->OnCreateNativeMediaPlayer(callbackImpl);
}

int32_t WebviewController::GetNWebId()
{
    return nwebId_;
}

ErrCode WebviewController::SetErrorPageEnabled(bool enable)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    nweb_ptr->SetErrorPageEnabled(enable);
    return NWebError::NO_ERROR;
}

bool WebviewController::GetErrorPageEnabled()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return false;
    }
    return nweb_ptr->GetErrorPageEnabled();
}

int32_t WebviewController::GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        return nweb_ptr->GetBlanklessInfoWithKey(key, similarity, loadingTime);
    }
    return -1;
}

int32_t WebviewController::SetBlanklessLoadingWithKey(const std::string& key, bool isStart)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        return nweb_ptr->SetBlanklessLoadingWithKey(key, isStart);
    }
    return -1;
}

int32_t WebviewController::SetBlanklessLoadingParams(ani_env* env, const std::string& key,
    AniBlanklessLoadingParam& param)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    std::shared_ptr<OHOS::NWeb::AniWebviewBlanklessCallback> callbackImpl = nullptr;
    if (!nweb_ptr) {
        return -1;
    }
    if (param.callbackRef) {
        callbackImpl = std::make_shared<OHOS::NWeb::AniWebviewBlanklessCallback>(env, param.callbackRef);
    }
    return nweb_ptr->SetBlanklessLoadingParams(key, param.enable, param.duration, param.expirationTime, callbackImpl);
}

int32_t WebviewController::GetAttachState()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        return static_cast<int32_t>(attachState_);
    }
    return static_cast<int32_t>(AttachState::NOT_ATTACHED);
}

void WebviewController::RegisterStateChangeCallback(ani_env *env, const std::string& type, ani_object handler)
{
    ani_ref callbackRef;
    if (env->GlobalReference_Create(handler, &callbackRef) != ANI_OK) {
        WVLOG_E("RegisterStateChangeCallback failed to create reference for callback");
        return;
    }

    WebRegObj regObj(env, callbackRef);
    auto iter = attachEventRegisterInfo_.find(type);
    if (iter == attachEventRegisterInfo_.end()) {
        attachEventRegisterInfo_[type] = std::vector<WebRegObj> { regObj };
        WVLOG_D("RegisterStateChangeCallback add new type:%{public}s", type.c_str());
        return;
    }

    bool found = false;
    for (auto& regObjInList : iter->second) {
        if (env == regObjInList.m_regEnv) {
            ani_boolean isEqual = ANI_FALSE;
            if (env->Reference_StrictEquals(regObjInList.m_regHandlerRef, handler, &isEqual) != ANI_OK) {
                WVLOG_E("RegisterStateChangeCallback failed to compare handlers");
                env->GlobalReference_Delete(callbackRef);
                return;
            }

            if (isEqual) {
                WVLOG_W("RegisterStateChangeCallback handler function is same");
                found = true;
                regObjInList.m_isMarked = false;
                break;
            }
        }
    }
    if (!found) {
        iter->second.emplace_back(regObj);
        WVLOG_D("RegisterStateChangeCallback add new handler for type:%{public}s", type.c_str());
    }

    return;
}

void WebviewController::DeleteRegisterObj(ani_env *env, std::vector<WebRegObj>& vecRegObjs, ani_object& handler)
{
    for (auto iter = vecRegObjs.begin(); iter != vecRegObjs.end(); ++iter) {
        if (!handler) {
            WVLOG_E("DeleteRegisterObj handler is null");
            break;
        }
        if (env == iter->m_regEnv && !iter->m_isMarked) {
            ani_boolean isEqual = ANI_FALSE;
            if (env->Reference_StrictEquals(iter->m_regHandlerRef, handler, &isEqual) != ANI_OK) {
                WVLOG_E("DeleteRegisterObj failed to compare handlers");
                continue;
            }

            if (isEqual) {
                iter->m_isMarked = true;
                WVLOG_I("DeleteRegisterObj mark register object ref delete");
                break;
            } else {
                WVLOG_D("DeleteRegisterObj register handler is not equal to HandlerRef");
            }
        } else {
            WVLOG_D("DeleteRegisterObj unregister event, env(%{private}p) is not equal to m_regEnv(%{private}p)", env,
                iter->m_regEnv);
        }
    }
}

void WebviewController::DeleteAllRegisterObj(ani_env *env, std::vector<WebRegObj>& vecRegObjs)
{
    for (auto iter = vecRegObjs.begin(); iter != vecRegObjs.end(); ++iter) {
        if (env == iter->m_regEnv && !iter->m_isMarked) {
            iter->m_isMarked = true;
            WVLOG_D("DeleteAllRegisterObj mark register object ref delete");
        } else {
            WVLOG_D("DeleteAllRegisterObj unregister all event, env(%{private}p) is not equal to m_regEnv(%{private}p)",
                env, iter->m_regEnv);
        }
    }
}

void WebviewController::UnregisterStateChangeCallback(ani_env *env, const std::string& type, ani_object handler)
{
    auto iter = attachEventRegisterInfo_.find(type);
    if (iter == attachEventRegisterInfo_.end()) {
        WVLOG_W("UnregisterStateChangeCallback unregister type:%{public}s not found", type.c_str());
        return;
    }

    if (handler) {
        DeleteRegisterObj(env, iter->second, handler);
    } else {
        WVLOG_D("UnregisterStateChangeCallback unregister all callback for type:%{public}s", type.c_str());
        DeleteAllRegisterObj(env, iter->second);
    }

    if (iter->second.empty()) {
        attachEventRegisterInfo_.erase(iter);
    }

    return;
}

void WebviewController::WaitForAttachedDeal(ani_env *env, void *data)
{
    WaitForAttachParam *param = static_cast<WaitForAttachParam *>(data);
    std::unique_lock<std::mutex> attachLock(param->webviewController->attachMtx_);
    param->webviewController->attachCond_.wait_for(attachLock, std::chrono::milliseconds(param->timeout), [param] {
        return param->webviewController->attachState_ == AttachState::ATTACHED;
    });
    param->state = static_cast<int32_t>(param->webviewController->attachState_);
    WVLOG_D("WaitForAttachedDeal finish wait_for, param->state = %{public}d", param->state);
}

void WebviewController::WaitForAttachedFinish(ani_env *env, arkts::concurrency_helpers::WorkStatus status, void *data)
{
    WaitForAttachParam *param = static_cast<WaitForAttachParam *>(data);

    ani_env *envUse = nullptr;
    if (param->vm->GetEnv(ANI_VERSION_1, &envUse) != ANI_OK || !envUse) {
        WVLOG_E("WaitForAttachedFinish get env failed");
        delete param;
        param = nullptr;
        return;
    }

    ani_size refs = REFERENCES_MAX_NUMBER;
    envUse->CreateLocalScope(refs);

    if (param->resolver) {
        ani_enum enumType;
        if (envUse->FindEnum("@ohos.web.webview.webview.ControllerAttachState", &enumType) != ANI_OK) {
            WVLOG_E("WaitForAttachedFinish find enumType failed");
        }

        ani_enum_item result = nullptr;
        ani_status rst = ANI_ERROR;
        if ((rst = envUse->Enum_GetEnumItemByIndex(enumType, param->state, &result)) != ANI_OK) {
            WVLOG_E("WaitForAttachedFinish convert attachState(%{public}d) failed, rst = %{public}d", param->state,
                rst);
        }

        if ((rst = envUse->PromiseResolver_Resolve(param->resolver, result)) != ANI_OK) {
            WVLOG_E("WaitForAttachedFinish resolve promise failed, rst = %{public}d", rst);
        }
    }

    envUse->DestroyLocalScope();
    DeleteAsyncWork(envUse, param->asyncWork);
    delete param;
    param = nullptr;

    return;
}

void WebviewController::WaitForAttachedInternal(ani_env *env, ani_int timeout, ani_resolver resolver)
{
    ani_vm *vm = nullptr;
    if (env->GetVM(&vm) != ANI_OK || !vm) {
        WVLOG_E("WaitForAttachedInternal get vm failed");
        return;
    }

    WaitForAttachParam *param = new (std::nothrow) WaitForAttachParam {
        .asyncWork = nullptr,
        .resolver = resolver,
        .timeout = timeout,
        .webviewController = this,
        .state = static_cast<int32_t>(attachState_),
        .vm = vm,
    };

    if (!param) {
        WVLOG_E("WaitForAttachedInternal create param failed");
        return;
    }

    auto status = CreateAsyncWork(env, WaitForAttachedDeal, WaitForAttachedFinish, static_cast<void *>(param),
        &param->asyncWork);
    if (status != arkts::concurrency_helpers::WorkStatus::OK) {
        WVLOG_E("WaitForAttachedInternal create asyncWork failed");
        delete param;
        param = nullptr;
        return;
    }

    QueueAsyncWork(env, param->asyncWork);
    return;
}

void WebviewController::SetUserAgentMetadata(
    const std::string& userAgent, std::shared_ptr<NWebUserAgentMetadata> metadata)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("nweb is nullptr");
        return;
    }

    nweb_ptr->SetUserAgentMetadata(userAgent, metadata);
}

std::shared_ptr<NWebUserAgentMetadata> WebviewController::GetUserAgentMetadata(const std::string& userAgent)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("nweb is nullptr");
        return nullptr;
    }
    return nweb_ptr->GetUserAgentMetadata(userAgent);
}
} // namespace NWeb
} // namespace OHOS
