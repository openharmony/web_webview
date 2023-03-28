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

#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "nweb_store_web_archive_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"
#include "webview_hasimage_callback.h"
#include "web_errors.h"

namespace {
    constexpr int32_t PARAMZERO = 0;
    constexpr int32_t PARAMONE = 1;
    constexpr int32_t RESULT_COUNT = 2;
}

namespace OHOS {
namespace NWeb {
using namespace NWebError;
std::string WebviewController::customeSchemeCmdLine_ = "";
bool WebviewController::existNweb_ = false;
bool WebviewController::webDebuggingAccess_ = false;
WebviewController::WebviewController(int32_t nwebId) : nweb_(NWebHelper::Instance().GetNWeb(nwebId)) {}

void WebviewController::SetWebId(int32_t nwebId)
{
    nweb_ = NWebHelper::Instance().GetNWeb(nwebId);
}

bool WebviewController::IsInit()
{
    return nweb_.lock() ? true : false;
}

bool WebviewController::AccessForward()
{
    bool access = false;
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        access = nweb_ptr->IsNavigateForwardAllowed();
    }
    return access;
}

bool WebviewController::AccessBackward()
{
    bool access = false;
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        access = nweb_ptr->IsNavigatebackwardAllowed();
    }
    return access;
}

bool WebviewController::AccessStep(int32_t step)
{
    bool access = false;
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        access = nweb_ptr->CanNavigateBackOrForward(step);
    }
    return access;
}

void WebviewController::ClearHistory()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->DeleteNavigateHistory();
    }
}

void WebviewController::Forward()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->NavigateForward();
    }
}

void WebviewController::Backward()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->NavigateBack();
    }
}

void WebviewController::OnActive()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->OnContinue();
    }
}

void WebviewController::OnInactive()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->OnPause();
    }
}

void WebviewController::Refresh()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->Reload();
    }
}

ErrCode WebviewController::ZoomIn()
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_ptr->ZoomIn();

    return result;
}

ErrCode WebviewController::ZoomOut()
{
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        webId = static_cast<int32_t>(nweb_ptr->GetWebId());
    }
    return webId;
}

std::string WebviewController::GetUserAgent()
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return "";
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_ptr->GetPreference();
    if (!setting) {
        return "";
    }
    return setting->DefaultUserAgent();
}

std::string WebviewController::GetTitle()
{
    std::string title = "";
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        title = nweb_ptr->Title();
    }
    return title;
}

int32_t WebviewController::GetPageHeight()
{
    int32_t pageHeight = 0;
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        pageHeight = nweb_ptr->ContentHeight();
    }
    return pageHeight;
}

ErrCode WebviewController::BackOrForward(int32_t step)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->NavigateBackOrForward(step);
    return NWebError::NO_ERROR;
}

void WebviewController::StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env,
    napi_ref jsCallback)
{
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
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

ErrCode WebviewController::CreateWebMessagePorts(std::vector<std::string>& ports)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->CreateWebMessagePorts(ports);
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->PostWebMessage(message, ports, targetUrl);
    return NWebError::NO_ERROR;
}

WebMessagePort::WebMessagePort(int32_t nwebId, std::string& port)
    : nweb_(NWebHelper::Instance().GetNWeb(nwebId)), portHandle_(port)
{}

ErrCode WebMessagePort::ClosePort()
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->ClosePort(portHandle_);
    portHandle_.clear();
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::PostPortMessage(std::shared_ptr<NWebMessage> data)
{
    auto nweb_ptr = nweb_.lock();
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
    std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback)
{
    auto nweb_ptr = nweb_.lock();
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

HitTestResult WebviewController::GetHitTestValue()
{
    OHOS::NWeb::HitTestResult nwebResult;
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nwebResult = nweb_ptr->GetHitTestResult();
        nwebResult.SetType(ConverToWebHitTestType(nwebResult.GetType()));
    }
    return nwebResult;
}

void WebviewController::RequestFocus()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->OnFocus();
    }
}

std::shared_ptr<Global::Resource::ResourceManager> WebviewController::GetResourceMgr(std::string bundleName,
    std::string moduleName)
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!context) {
        WVLOG_E("Failed to get application Context.");
        return nullptr;
    }
    if (bundleName.empty() || moduleName.empty()) {
        return context->GetResourceManager();
    }
    auto moduleContext = context->CreateModuleContext(bundleName, moduleName);
    if (!moduleContext) {
        WVLOG_E("Failed to get module Context.");
        return nullptr;
    }
    return moduleContext->GetResourceManager();
}

ErrCode WebviewController::ParseUrl(napi_env env, napi_value urlObj, std::string& result)
{
    napi_valuetype valueType = napi_null;
    napi_typeof(env, urlObj, &valueType);
    if ((valueType != napi_object) && (valueType != napi_string)) {
        WVLOG_E("Unable to parse url object.");
        return NWebError::INVALID_URL;
    }
    if (valueType == napi_string) {
        NapiParseUtils::ParseString(env, urlObj, result);
        WVLOG_D("The parsed url is: %{public}s", result.c_str());
        return NWebError::NO_ERROR;
    }
    napi_value type = nullptr;
    napi_valuetype typeVlueType = napi_null;
    napi_get_named_property(env, urlObj, "type", &type);
    napi_typeof(env, type, &typeVlueType);
    if (typeVlueType == napi_number) {
        int32_t typeInteger;
        NapiParseUtils::ParseInt32(env, type, typeInteger);
        if (typeInteger == static_cast<int>(ResourceType::RAWFILE)) {
            napi_value paraArray = nullptr;
            napi_get_named_property(env, urlObj, "params", &paraArray);
            bool isArray = false;
            napi_is_array(env, paraArray, &isArray);
            if (!isArray) {
                WVLOG_E("Unable to parse parameter array from url object.");
                return NWebError::INVALID_RESOURCE;
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
            auto resourceManager = GetResourceMgr(bundleName, moduleName);
            if (!resourceManager) {
                WVLOG_E("Get resourceManager failed.");
                return NWebError::INVALID_RESOURCE;
            }

            // Adapt to the input like: "file:///index.html?a=1", before the new solution comes, copy from arkui.
            auto it = std::find_if(fileName.begin(), fileName.end(), [](char c) {
                return (c == '#') || (c == '?');
            });
            std::string params;
            std::string newFileName = fileName;
            if (it != fileName.end()) {
                newFileName = std::string(fileName.begin(), it);
                params = std::string(it, fileName.end());
            }
            auto state = resourceManager->GetRawFilePathByName(newFileName, result);
            if (state != Global::Resource::SUCCESS) {
                WVLOG_E("GetRawfile error, filename:%{public}s, error:%{public}u, newfilename = %{public}s",
                    fileName.c_str(), state, newFileName.c_str());
                return NWebError::INVALID_RESOURCE;
            }
            result = "file:///" + result + params;
            WVLOG_D("The parsed url is: %{public}s", result.c_str());
            return NWebError::NO_ERROR;
        }
        WVLOG_E("The type parsed from url object is not RAWFILE.");
        return NWebError::INVALID_URL;
    }
    WVLOG_E("Unable to parse type from url object.");
    return NWebError::INVALID_URL;
}

ErrCode WebviewController::LoadUrl(std::string url)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    return nweb_ptr->Load(url);
}

ErrCode WebviewController::LoadUrl(std::string url, std::map<std::string, std::string> httpHeaders)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return INIT_ERROR;
    }
    return nweb_ptr->Load(url, httpHeaders);
}

ErrCode WebviewController::LoadData(std::string data, std::string mimeType, std::string encoding,
    std::string baseUrl, std::string historyUrl)
{
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        return ConverToWebHitTestType(nweb_ptr->GetHitTestResult().GetType());
    }
    return static_cast<int>(WebHitTestType::UNKNOWN);
}


void WebviewController::ClearMatches()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->ClearMatches();
    }
}

void WebviewController::SearchNext(bool forward)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->FindNext(forward);
    }
}

void WebviewController::SearchAllAsync(const std::string& searchString)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->FindAllAsync(searchString);
    }
}

void WebviewController::ClearSslCache()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->ClearSslCache();
    }
}

void WebviewController::ClearClientAuthenticationCache()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->ClearClientAuthenticationCache();
    }
}

void WebviewController::Stop()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->Stop();
    }
}

ErrCode WebviewController::Zoom(float factor)
{
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr || javaScriptResultCb_) {
        return;
    }

    javaScriptResultCb_ = std::make_shared<WebviewJavaScriptResultCallBack>();
    nweb_ptr->SetNWebJavaScriptResultCallBack(javaScriptResultCb_);
}

void WebviewController::RegisterJavaScriptProxy(napi_env env, napi_value obj,
    const std::string& objName, const std::vector<std::string>& methodList)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        return;
    }
    if (javaScriptResultCb_) {
        javaScriptResultCb_->RegisterJavaScriptProxy(env, obj, objName, methodList);
    }
    nweb_ptr->RegisterArkJSfunction(objName, methodList);
}

void WebviewController::RunJavaScriptCallback(const std::string &script, napi_env env, napi_ref jsCallback)
{
    auto nweb_ptr = nweb_.lock();
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

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, jsCallback, nullptr);
    nweb_ptr->ExecuteJavaScript(script, callbackImpl);
}

void WebviewController::RunJavaScriptPromise(const std::string &script, napi_env env,
    napi_deferred deferred)
{
    auto nweb_ptr = nweb_.lock();
    if (!nweb_ptr) {
        napi_value jsResult = nullptr;
        jsResult = NWebError::BusinessError::CreateError(env, NWebError::INIT_ERROR);
        napi_reject_deferred(env, deferred, jsResult);
        return;
    }

    if (deferred == nullptr) {
        return;
    }

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, nullptr, deferred);
    nweb_ptr->ExecuteJavaScript(script, callbackImpl);
}

std::string WebviewController::GetUrl()
{
    std::string url = "";
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        url = nweb_ptr->GetUrl();
    }
    return url;
}

std::string WebviewController::GetOriginalUrl()
{
    std::string url = "";
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        url = nweb_ptr->GetOriginalUrl();
    }
    return url;
}

void WebviewController::PutNetworkAvailable(bool available)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->PutNetworkAvailable(available);
    }
}

ErrCode WebviewController::HasImagesCallback(napi_env env, napi_ref jsCallback)
{
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
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

void WebviewController::RemoveCache(bool include_disk_files)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->RemoveCache(include_disk_files);
    }
}

std::shared_ptr<NWebHistoryList> WebviewController::GetHistoryList()
{
    auto nweb_ptr = nweb_.lock();
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
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        isGetFavicon = nweb_ptr->GetFavicon(data, width, height, colorType, alphaType);
    }
    return isGetFavicon;
}

WebState WebviewController::SerializeWebState()
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        return nweb_ptr->SerializeWebState();
    }
    return nullptr;
}

bool WebviewController::RestoreWebState(WebState state)
{
    bool isRestored = false;
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        isRestored = nweb_ptr->RestoreWebState(state);
    }
    return isRestored;
}

void WebviewController::ScrollPageDown(bool bottom)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->PageDown(bottom);
    }
    return;
}

void WebviewController::ScrollPageUp(bool top)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->PageUp(top);
    }
    return;
}

void WebviewController::ScrollTo(float x, float y)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->ScrollTo(x, y);
    }
    return;
}

void WebviewController::ScrollBy(float deltaX, float deltaY)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->ScrollBy(deltaX, deltaY);
    }
    return;
}

void WebviewController::SlideScroll(float vx, float vy)
{
    auto nweb_ptr = nweb_.lock();
    if (nweb_ptr) {
        nweb_ptr->SlideScroll(vx, vy);
    }
    return;
}
} // namespace NWeb
} // namespace OHOS
