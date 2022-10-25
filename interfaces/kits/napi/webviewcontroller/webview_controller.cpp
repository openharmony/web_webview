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

#include "context/application_context.h"
#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "nweb_store_web_archive_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"
#include "web_errors.h"

namespace {
    constexpr int32_t PARAMZERO = 0;
    constexpr int32_t PARAMONE = 1;
    constexpr int32_t RESULT_COUNT = 2;
}

namespace OHOS {
namespace NWeb {
using namespace NWebError;
WebviewController::WebviewController(int32_t nwebId)
{
    nweb_ = OHOS::NWeb::NWebHelper::Instance().GetNWeb(nwebId);
}

bool WebviewController::AccessForward()
{
    bool access = false;
    if (nweb_) {
        access = nweb_->IsNavigateForwardAllowed();
    }
    return access;
}

bool WebviewController::AccessBackward()
{
    bool access = false;
    if (nweb_) {
        access = nweb_->IsNavigatebackwardAllowed();
    }
    return access;
}

bool WebviewController::AccessStep(int32_t step)
{
    bool access = false;
    if (nweb_) {
        access = nweb_->CanNavigateBackOrForward(step);
    }
    return access;
}

void WebviewController::ClearHistory()
{
    if (nweb_) {
        nweb_->DeleteNavigateHistory();
    }
}

void WebviewController::Forward()
{
    if (nweb_) {
        nweb_->NavigateForward();
    }
}

void WebviewController::Backward()
{
    if (nweb_) {
        nweb_->NavigateBack();
    }
}

void WebviewController::OnActive()
{
    if (nweb_) {
        nweb_->OnContinue();
    }
}

void WebviewController::OnInactive()
{
    if (nweb_) {
        nweb_->OnPause();
    }
}

void WebviewController::Refresh()
{
    if (nweb_) {
        nweb_->Reload();
    }
}

ErrCode WebviewController::ZoomIn()
{
    if (!nweb_) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_->ZoomIn();

    return result;
}

ErrCode WebviewController::ZoomOut()
{
    if (!nweb_) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_->ZoomOut();

    return result;
}

int32_t WebviewController::GetWebId() const
{
    int32_t webId = -1;
    if (nweb_) {
        webId = static_cast<int32_t>(nweb_->GetWebId());
    }
    return webId;
}

std::string WebviewController::GetUserAgent()
{
    if (!nweb_) {
        return "";
    }
    std::shared_ptr<OHOS::NWeb::NWebPreference> setting = nweb_->GetPreference();
    if (!setting) {
        return "";
    }
    return setting->DefaultUserAgent();
}

std::string WebviewController::GetTitle()
{
    std::string title = "";
    if (nweb_) {
        title = nweb_->Title();
    }
    return title;
}

int32_t WebviewController::GetPageHeight()
{
    int32_t pageHeight = 0;
    if (nweb_) {
        pageHeight = nweb_->ContentHeight();
    }
    return pageHeight;
}

ErrCode WebviewController::BackOrForward(int32_t step)
{
    if (!nweb_) {
        return INIT_ERROR;
    }

    nweb_->NavigateBackOrForward(step);
    return NWebError::NO_ERROR;
}

void WebviewController::StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env,
    napi_ref jsCallback)
{
    if (!nweb_) {
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
    });
    nweb_->StoreWebArchive(baseName, autoName, callbackImpl);
    return;
}

void WebviewController::StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env,
    napi_deferred deferred)
{
    if (!nweb_) {
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
        napi_value setResult[RESULT_COUNT] = {0};
        setResult[PARAMZERO] = NWebError::BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);
        napi_create_string_utf8(env, result.c_str(), NAPI_AUTO_LENGTH, &setResult[PARAMONE]);
        napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
        if (!result.empty()) {
            napi_resolve_deferred(env, deferred, args[PARAMONE]);
        } else {
            napi_reject_deferred(env, deferred, args[PARAMZERO]);
        }
    });
    nweb_->StoreWebArchive(baseName, autoName, callbackImpl);
    return;
}

ErrCode WebviewController::CreateWebMessagePorts(std::vector<std::string>& ports)
{
    if (!nweb_) {
        return INIT_ERROR;
    }

    nweb_->CreateWebMessagePorts(ports);
    return NWebError::NO_ERROR;
}

ErrCode WebviewController::PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl)
{
    if (!nweb_) {
        return INIT_ERROR;
    }

    nweb_->PostWebMessage(message, ports, targetUrl);
    return NWebError::NO_ERROR;
}

WebMessagePort::WebMessagePort(int32_t nwebId, std::string& port)
{
    nweb_ = OHOS::NWeb::NWebHelper::Instance().GetNWeb(nwebId);
    portHandle_ = port;
}

ErrCode WebMessagePort::ClosePort()
{
    if (!nweb_) {
        return INIT_ERROR;
    }

    nweb_->ClosePort(portHandle_);
    portHandle_.clear();
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::PostPortMessage(std::string& data)
{
    if (!nweb_) {
        return INIT_ERROR;
    }

    if (portHandle_.empty()) {
        WVLOG_E("can't post message, message port already closed");
        return CAN_NOT_POST_MESSAGE;
    }
    nweb_->PostPortMessage(portHandle_, data);
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::SetPortMessageCallback(std::shared_ptr<NWebValueCallback<std::string>> callback)
{
    if (!nweb_) {
        return INIT_ERROR;
    }

    if (portHandle_.empty()) {
        WVLOG_E("can't register message port callback event, message port already closed");
        return CAN_NOT_REGISTER_MESSAGE_EVENT;
    }
    nweb_->SetPortMessageCallback(portHandle_, callback);
    return NWebError::NO_ERROR;
}

std::string WebMessagePort::GetPortHandle() const
{
    return portHandle_;
}

HitTestResult WebviewController::GetHitTestValue()
{
    OHOS::NWeb::HitTestResult nwebResult;
    if (nweb_) {
        nwebResult = nweb_->GetHitTestResult();
    }
    return nwebResult;
}

void WebviewController::RequestFocus()
{
    if (nweb_) {
        nweb_->OnFocus();
    }
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
            napi_value paraArray = nullptr;
            napi_get_named_property(env, urlObj, "params", &paraArray);
            bool isArray = false;
            napi_is_array(env, paraArray, &isArray);
            if (!isArray) {
                WVLOG_E("Unable to parse parameter array from url object.");
                return false;
            }
            napi_value fileNameObj;
            std::string fileName;
            napi_get_element(env, paraArray, 0, &fileNameObj);
            NapiParseUtils::ParseString(env, fileNameObj, fileName);
            std::shared_ptr<AbilityRuntime::ApplicationContext> context =
                AbilityRuntime::ApplicationContext::GetApplicationContext();
            std::string packagePath = "file:///" + context->GetBundleCodeDir() + "/";
            std::string bundleName = context->GetBundleName() + "/";
            std::shared_ptr<AppExecFwk::ApplicationInfo> appInfo = context->GetApplicationInfo();
            std::string entryDir = appInfo->entryDir;
            bool isStage = entryDir.find("entry") == std::string::npos ? false : true;
            result = isStage ? packagePath + "entry/resources/rawfile/" + fileName :
                packagePath + bundleName + "assets/entry/resources/rawfile/" + fileName;
            WVLOG_D("The parsed url is: %{public}s", result.c_str());
            return true;
        }
        WVLOG_E("The type parsed from url object is not RAWFILE.");
        return false;
    }
    WVLOG_E("Unable to parse type from url object.");
    return false;
}

ErrCode WebviewController::LoadUrl(std::string url)
{
    if (!nweb_) {
        return INIT_ERROR;
    }
    return nweb_->Load(url);
}

ErrCode WebviewController::LoadUrl(std::string url, std::map<std::string, std::string> httpHeaders)
{
    if (!nweb_) {
        return INIT_ERROR;
    }
    return nweb_->Load(url, httpHeaders);
}

ErrCode WebviewController::LoadData(std::string data, std::string mimeType, std::string encoding,
    std::string baseUrl, std::string historyUrl)
{
    if (!nweb_) {
        return INIT_ERROR;
    }
    if (baseUrl.empty() && historyUrl.empty()) {
        return nweb_->LoadWithData(data, mimeType, encoding);
    }
    return nweb_->LoadWithDataAndBaseUrl(data, mimeType, encoding, baseUrl, historyUrl);
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
    if (nweb_) {
        return ConverToWebHitTestType(nweb_->GetHitTestResult().GetType());
    }
    return static_cast<int>(WebHitTestType::UNKNOWN);
}


void WebviewController::ClearMatches()
{
    if (nweb_) {
        nweb_->ClearMatches();
    }
}

void WebviewController::SearchNext(bool forward)
{
    if (nweb_) {
        nweb_->FindNext(forward);
    }
}

void WebviewController::SearchAllAsync(const std::string& searchString)
{
    if (nweb_) {
        nweb_->FindAllAsync(searchString);
    }
}

void WebviewController::ClearSslCache()
{
    if (nweb_) {
        nweb_->ClearSslCache();
    }
}

void WebviewController::ClearClientAuthenticationCache()
{
    if (nweb_) {
        nweb_->ClearClientAuthenticationCache();
    }
}

void WebviewController::Stop()
{
    if (nweb_) {
        nweb_->Stop();
    }
}

ErrCode WebviewController::Zoom(float factor)
{
    if (!nweb_) {
        return INIT_ERROR;
    }
    ErrCode result = NWebError::NO_ERROR;
    result = nweb_->Zoom(factor);

    return result;
}

ErrCode WebviewController::DeleteJavaScriptRegister(const std::string& objName,
    const std::vector<std::string>& methodList)
{
    if (nweb_) {
        nweb_->UnregisterArkJSfunction(objName, methodList);
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
    if (!nweb_ || javaScriptResultCb_) {
        return;
    }

    javaScriptResultCb_ = std::make_shared<WebviewJavaScriptResultCallBack>();
    nweb_->SetNWebJavaScriptResultCallBack(javaScriptResultCb_);
}

void WebviewController::RegisterJavaScriptProxy(napi_env env, napi_value obj,
    const std::string& objName, const std::vector<std::string>& methodList)
{
    if (!nweb_) {
        return;
    }
    if (javaScriptResultCb_) {
        javaScriptResultCb_->RegisterJavaScriptProxy(env, obj, objName, methodList);
    }
    nweb_->RegisterArkJSfunction(objName, methodList);
}

void WebviewController::RunJavaScriptCallback(const std::string &script, napi_env env, napi_ref jsCallback)
{
    if (!nweb_) {
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

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>();
    callbackImpl->SetCallBack([env, jCallback = std::move(jsCallback)](std::string result) {
        if (!env) {
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
    });
    nweb_->ExecuteJavaScript(script, callbackImpl);
}

void WebviewController::RunJavaScriptPromise(const std::string &script, napi_env env,
    napi_deferred deferred)
{
    if (!nweb_) {
        napi_value jsResult = nullptr;
        jsResult = NWebError::BusinessError::CreateError(env, NWebError::INIT_ERROR);
        napi_reject_deferred(env, deferred, jsResult);
        return;
    }

    if (deferred == nullptr) {
        return;
    }

    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>();
    callbackImpl->SetCallBack([env, deferred](std::string result) {
        if (!env) {
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
    });
    nweb_->ExecuteJavaScript(script, callbackImpl);
}

std::string WebviewController::GetUrl()
{
    std::string url = "";
    if (nweb_) {
        url = nweb_->GetUrl();
    }
    return url;
}

} // namespace NWeb
} // namespace OHOS
