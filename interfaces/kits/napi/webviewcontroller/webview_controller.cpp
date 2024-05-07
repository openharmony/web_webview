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
#include "business_error.h"
#include "napi_parse_utils.h"
#include "ohos_resource_adapter_impl.h"

#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"
#include "native_media_player_impl.h"

#include "nweb_log.h"
#include "nweb_store_web_archive_callback.h"
#include "web_errors.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"

#include "nweb_precompile_callback.h"
#include "nweb_cache_options_impl.h"

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
constexpr uint32_t URL_MAXIMUM = 2048;
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
std::unordered_map<int32_t, WebviewController*> g_webview_controller_map;
std::string WebviewController::customeSchemeCmdLine_ = "";
bool WebviewController::existNweb_ = false;
bool WebviewController::webDebuggingAccess_ = false;
std::set<std::string> WebviewController::webTagSet_;
int32_t WebviewController::webTagStrId_ = 0;

WebviewController::WebviewController(int32_t nwebId) : nwebId_(nwebId)
{
    if (IsInit()) {
        std::unique_lock<std::mutex> lk(webMtx_);
        g_webview_controller_map.emplace(nwebId, this);
    }
}

WebviewController::WebviewController(const std::string& webTag) : webTag_(webTag)
{
    NWebHelper::Instance().SetWebTag(-1, webTag_.c_str());
}

WebviewController::~WebviewController()
{
    std::unique_lock<std::mutex> lk(webMtx_);
    g_webview_controller_map.erase(nwebId_);
}

void WebviewController::SetWebId(int32_t nwebId)
{
    nwebId_ = nwebId;
    std::unique_lock<std::mutex> lk(webMtx_);
    g_webview_controller_map.emplace(nwebId, this);

    if (webTag_.empty()) {
        WVLOG_I("native webtag is empty, don't care because it's not a native instance");
        return;
    }

    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        OH_NativeArkWeb_BindWebTagToWebInstance(webTag_.c_str(), nweb_ptr);
        NWebHelper::Instance().SetWebTag(nwebId_, webTag_.c_str());
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

WebviewController* WebviewController::FromID(int32_t nwebId)
{
    std::unique_lock<std::mutex> lk(webMtx_);
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
    auto objs = parentControl->javaScriptResultCb_->GetNamedObjects();
    SetNWebJavaScriptResultCallBack();
    for (auto it = objs.begin(); it != objs.end(); it++) {
        if (it->second && IsInit()) {
            RegisterJavaScriptProxy(
                it->second->GetEnv(), it->second->GetValue(), it->first,
                it->second->GetSyncMethodNames(), it->second->GetAsyncMethodNames());
        }
    }
}

bool WebviewController::IsInit()
{
    return NWebHelper::Instance().GetNWeb(nwebId_) ? true : false;
}

bool WebviewController::AccessForward()
{
    bool access = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        access = nweb_ptr->IsNavigateForwardAllowed();
    }
    return access;
}

bool WebviewController::AccessBackward()
{
    bool access = false;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        access = nweb_ptr->IsNavigatebackwardAllowed();
    }
    return access;
}

bool WebviewController::AccessStep(int32_t step)
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

WebMessagePort::WebMessagePort(int32_t nwebId, std::string& port, bool isExtentionType)
    : nwebId_(nwebId), portHandle_(port), isExtentionType_(isExtentionType)
{}

ErrCode WebMessagePort::ClosePort()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->ClosePort(portHandle_);
    portHandle_.clear();
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::PostPortMessage(std::shared_ptr<NWebMessage> data)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    if (portHandle_.empty()) {
        WVLOG_E("can't post message, message port already closed");
        return CAN_NOT_POST_MESSAGE;
    }
    nweb_ptr->PostPortMessage(portHandle_, data);
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::SetPortMessageCallback(
    std::shared_ptr<NWebMessageValueCallback> callback)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    if (portHandle_.empty()) {
        WVLOG_E("can't register message port callback event, message port already closed");
        return CAN_NOT_REGISTER_MESSAGE_EVENT;
    }
    nweb_ptr->SetPortMessageCallback(portHandle_, callback);
    return NWebError::NO_ERROR;
}

std::string WebMessagePort::GetPortHandle() const
{
    return portHandle_;
}

std::shared_ptr<HitTestResult> WebviewController::GetHitTestValue()
{
    std::shared_ptr<HitTestResult> nwebResult;
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (nweb_ptr) {
        nwebResult = nweb_ptr->GetHitTestResult();
        if (nwebResult) {
            nwebResult->SetType(ConverToWebHitTestType(nwebResult->GetType()));
        } else {
            nwebResult->SetType(ConverToWebHitTestType(HitTestResult::UNKNOWN_TYPE));
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
    const std::string& bundleName, const std::string& moduleName, std::string &result)
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
    WVLOG_D("The parsed url is: %{public}s", result.c_str());
    return true;
}

bool WebviewController::ParseUrl(napi_env env, napi_value urlObj, std::string& result)
{
    napi_valuetype valueType = napi_null;
    napi_typeof(env, urlObj, &valueType);
    if ((valueType != napi_object) && (valueType != napi_string)) {
        WVLOG_E("Unable to parse url object.");
        return false;
    }
    if (valueType == napi_string) {
        NapiParseUtils::ParseString(env, urlObj, result);
        WVLOG_D("The parsed url is: %{public}s", result.c_str());
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

bool WebviewController::ParseRawFileUrl(napi_env env, napi_value urlObj, std::string& result)
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

bool WebviewController::GetResourceUrl(napi_env env, napi_value urlObj, std::string& result)
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

bool WebviewController::IsSafeBrowsingEnabled()
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

void WebviewController::RegisterJavaScriptProxy(
    napi_env env, napi_value obj, const std::string& objName,
    const std::vector<std::string>& syncMethodList,
    const std::vector<std::string>& asyncMethodList)
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

    if (syncMethodList.empty() && asyncMethodList.empty()) {
        WVLOG_E("WebviewController::RegisterJavaScriptProxy all methodList are "
                "empty");
        return;
    }

    std::vector<std::string> allMethodList;
    std::merge(syncMethodList.begin(), syncMethodList.end(),
               asyncMethodList.begin(), asyncMethodList.end(),
               std::back_inserter(allMethodList));
    objId = javaScriptResultCb_->RegisterJavaScriptProxy(env, obj, objName, allMethodList, asyncMethodList);

    nweb_ptr->RegisterArkJSfunction(objName, syncMethodList, objId);
}

void WebviewController::RunJavaScriptCallback(
    const std::string& script, napi_env env, napi_ref jsCallback, bool extention)
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

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, jsCallback, nullptr, extention);
    nweb_ptr->ExecuteJavaScript(script, callbackImpl, extention);
}

void WebviewController::RunJavaScriptPromise(const std::string &script, napi_env env,
    napi_deferred deferred, bool extention)
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

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, nullptr, deferred, extention);
    nweb_ptr->ExecuteJavaScript(script, callbackImpl, extention);
}

void WebviewController::RunJavaScriptCallbackExt(
    const int fd, const size_t scriptLength, napi_env env, napi_ref jsCallback, bool extention)
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
        close(fd);
        return;
    }

    if (jsCallback == nullptr) {
        close(fd);
        return;
    }

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, jsCallback, nullptr, extention);
    nweb_ptr->ExecuteJavaScriptExt(fd, scriptLength, callbackImpl, extention);
}

void WebviewController::RunJavaScriptPromiseExt(
    const int fd, const size_t scriptLength, napi_env env, napi_deferred deferred, bool extention)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        napi_value jsResult = nullptr;
        jsResult = NWebError::BusinessError::CreateError(env, NWebError::INIT_ERROR);
        napi_reject_deferred(env, deferred, jsResult);
        close(fd);
        return;
    }

    if (deferred == nullptr) {
        close(fd);
        return;
    }

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, nullptr, deferred, extention);
    nweb_ptr->ExecuteJavaScriptExt(fd, scriptLength, callbackImpl, extention);
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

bool WebviewController::TerminateRenderProcess()
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

ErrCode WebviewController::HasImagesCallback(napi_env env, napi_ref jsCallback)
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
        return NWebError::INIT_ERROR;
    }

    if (jsCallback == nullptr) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto callbackImpl = std::make_shared<WebviewHasImageCallback>(env, jsCallback, nullptr);
    nweb_ptr->HasImages(callbackImpl);
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::HasImagesPromise(napi_env env, napi_deferred deferred)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        napi_value jsResult = nullptr;
        jsResult = NWebError::BusinessError::CreateError(env, NWebError::INIT_ERROR);
        napi_reject_deferred(env, deferred, jsResult);
        return NWebError::INIT_ERROR;
    }

    if (deferred == nullptr) {
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto callbackImpl = std::make_shared<WebviewHasImageCallback>(env, nullptr, deferred);
    nweb_ptr->HasImages(callbackImpl);
    return NWebError::NO_ERROR;
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

std::shared_ptr<NWebHistoryItem> WebHistoryList::GetItem(int32_t index)
{
    if (!sptrHistoryList_) {
        return nullptr;
    }
    return sptrHistoryList_->GetItem(index);
}

int32_t WebHistoryList::GetListSize()
{
    int32_t listSize = 0;

    if (!sptrHistoryList_) {
        return listSize;
    }
    listSize = sptrHistoryList_->GetListSize();
    return listSize;
}

bool WebviewController::GetFavicon(
    const void **data, size_t &width, size_t &height, ImageColorType &colorType, ImageAlphaType &alphaType)
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

bool WebviewController::RestoreWebState(const std::vector<uint8_t> &state)
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

bool WebviewController::GetScrollable()
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

bool WebviewController::GetCertChainDerData(std::vector<std::string> &certChainDerData)
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

int WebviewController::GetSecurityLevel()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
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

bool WebviewController::IsIncognitoMode()
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

bool  WebviewController::GetPrintBackground()
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

bool WebviewController::IsIntelligentTrackingPreventionEnabled()
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

bool WebviewController::SetWebSchemeHandler(const char* scheme, WebSchemeHandler* handler)
{
    if (!handler || !scheme) {
        WVLOG_E("WebviewController::SetWebSchemeHandler handler or scheme is nullptr");
        return false;
    }
    ArkWeb_SchemeHandler* schemeHandler =
        const_cast<ArkWeb_SchemeHandler*>(WebSchemeHandler::GetArkWebSchemeHandler(handler));
    return OH_ArkWeb_SetSchemeHandler(scheme, webTag_.c_str(), schemeHandler);
}

int32_t WebviewController::ClearWebSchemeHandler()
{
    return OH_ArkWeb_ClearSchemeHandlers(webTag_.c_str());
}

bool WebviewController::SetWebServiveWorkerSchemeHandler(
    const char* scheme, WebSchemeHandler* handler)
{
    ArkWeb_SchemeHandler* schemeHandler =
        const_cast<ArkWeb_SchemeHandler*>(WebSchemeHandler::GetArkWebSchemeHandler(handler));
    return OH_ArkWebServiceWorker_SetSchemeHandler(scheme, schemeHandler);
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

std::string WebviewController::GetLastJavascriptProxyCallingFrameUrl()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return "";
    }

    return nweb_ptr->GetLastJavascriptProxyCallingFrameUrl();
}

void WebviewController::OnCreateNativeMediaPlayer(napi_env env, napi_ref callback)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return;
    }

    auto callbackImpl = std::make_shared<NWebCreateNativeMediaPlayerCallbackImpl>(nwebId_, env, callback);
    nweb_ptr->OnCreateNativeMediaPlayer(callbackImpl);
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

std::shared_ptr<CacheOptions> WebviewController::ParseCacheOptions(napi_env env, napi_value value) {
    std::map<std::string, std::string> responseHeaders;
    bool isModule = false;
    bool isTopLevel = false;
    auto defaultCacheOptions = std::make_shared<NWebCacheOptionsImpl>(responseHeaders, isModule, isTopLevel);

    napi_value responseHeadersValue = nullptr;
    if (napi_get_named_property(env, value, "responseHeaders", &responseHeadersValue) != napi_ok) {
        WVLOG_D("PrecompileJavaScript: cannot get 'responseHeaders' of CacheOptions.");
        return defaultCacheOptions;
    }

    if (!ParseResponseHeaders(env, responseHeadersValue, responseHeaders)) {
        WVLOG_D("PrecompileJavaScript: parse 'responseHeaders' of CacheOptions failed. use default options");
        return defaultCacheOptions;
    }

    return std::make_shared<NWebCacheOptionsImpl>(responseHeaders, isModule, isTopLevel);
}

ErrCode WebviewController::PrecompileJavaScriptPromise(
    napi_env env, napi_deferred deferred,
    const std::string &url, const std::string &script, std::shared_ptr<CacheOptions> cacheOptions)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return NWebError::INIT_ERROR;
    }

    if (!deferred) {
        return NWebError::INIT_ERROR;
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
    return NWebError::NO_ERROR;
}

bool WebviewController::ParseResponseHeaders(napi_env env,
                                             napi_value value,
                                             std::map<std::string, std::string> &responseHeaders)
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

bool WebviewController::CheckURL(std::string& url)
{
    if (url.size() > URL_MAXIMUM) {
        WVLOG_E("The URL exceeds the maximum length of %{public}d. URL: %{public}s", URL_MAXIMUM, url.c_str());
        return false;
    }

    if (!regex_match(url, std::regex("^http(s)?:\\/\\/.+", std::regex_constants::icase))) {
        WVLOG_E("The Parse URL error. URL: %{public}s", url.c_str());
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
} // namespace NWeb
} // namespace OHOS
