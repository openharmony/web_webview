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

#include "napi_webview_controller.h"

#include <cctype>
#include <climits>
#include <cstdint>
#include <regex>
#include <securec.h>
#include <unistd.h>
#include <uv.h>

#include "application_context.h"
#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "parameters.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "web_errors.h"
#include "webview_javascript_execute_callback.h"

#include "web_download_delegate.h"
#include "web_download_manager.h"
#include "arkweb_scheme_handler.h"
#include "web_scheme_handler_request.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;

namespace {
constexpr uint32_t URL_MAXIMUM = 2048;
constexpr uint32_t SOCKET_MAXIMUM = 6;
constexpr char URL_REGEXPR[] = "^http(s)?:\\/\\/.+";
constexpr size_t MAX_RESOURCES_COUNT = 30;
constexpr size_t MAX_RESOURCE_SIZE = 10 * 1024 * 1024;
using WebPrintWriteResultCallback = std::function<void(std::string, uint32_t)>;

bool ParsePrepareUrl(napi_env env, napi_value urlObj, std::string& url)
{
    napi_valuetype valueType = napi_null;
    napi_typeof(env, urlObj, &valueType);

    if (valueType == napi_string) {
        NapiParseUtils::ParseString(env, urlObj, url);
        if (url.size() > URL_MAXIMUM) {
            WVLOG_E("The URL exceeds the maximum length of %{public}d", URL_MAXIMUM);
            return false;
        }

        if (!regex_match(url, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            WVLOG_E("ParsePrepareUrl error");
            return false;
        }

        return true;
    }

    WVLOG_E("Unable to parse type from url object.");
    return false;
}

napi_valuetype GetArrayValueType(napi_env env, napi_value array, bool& isDouble)
{
    uint32_t arrayLength = 0;
    napi_get_array_length(env, array, &arrayLength);
    napi_valuetype valueTypeFirst = napi_undefined;
    napi_valuetype valueTypeCur = napi_undefined;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_value obj = nullptr;
        napi_get_element(env, array, i, &obj);
        napi_typeof(env, obj, &valueTypeCur);
        if (i == 0) {
            valueTypeFirst = valueTypeCur;
        }
        if (valueTypeCur != napi_string && valueTypeCur != napi_number && valueTypeCur != napi_boolean) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return napi_undefined;
        }
        if (valueTypeCur != valueTypeFirst) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return napi_undefined;
        }
        if (valueTypeFirst == napi_number) {
            int32_t elementInt32 = 0;
            double elementDouble = 0.0;
            bool isReadValue32 = napi_get_value_int32(env, obj, &elementInt32) == napi_ok;
            bool isReadDouble = napi_get_value_double(env, obj, &elementDouble) == napi_ok;
            constexpr double MINIMAL_ERROR = 0.000001;
            if (isReadValue32 && isReadDouble) {
                isDouble = abs(elementDouble - elementInt32 * 1.0) > MINIMAL_ERROR;
            } else if (isReadDouble) {
                isDouble = true;
            }
        }
    }
    return valueTypeFirst;
}

void SetArrayHandlerBoolean(napi_env env, napi_value array, WebMessageExt* webMessageExt)
{
    std::vector<bool> outValue;
    if (!NapiParseUtils::ParseBooleanArray(env, array, outValue)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }
    webMessageExt->SetBooleanArray(outValue);
}

void SetArrayHandlerString(napi_env env, napi_value array, WebMessageExt* webMessageExt)
{
    std::vector<std::string> outValue;
    if (!NapiParseUtils::ParseStringArray(env, array, outValue)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }
    webMessageExt->SetStringArray(outValue);
}

void SetArrayHandlerInteger(napi_env env, napi_value array, WebMessageExt* webMessageExt)
{
    std::vector<int64_t> outValue;
    if (!NapiParseUtils::ParseInt64Array(env, array, outValue)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }
    webMessageExt->SetInt64Array(outValue);
}

void SetArrayHandlerDouble(napi_env env, napi_value array, WebMessageExt* webMessageExt)
{
    std::vector<double> outValue;
    if (!NapiParseUtils::ParseDoubleArray(env, array, outValue)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }
    webMessageExt->SetDoubleArray(outValue);
}

WebviewController* GetWebviewController(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    return webviewController;
}

bool ParsePrepareRequestMethod(napi_env env, napi_value methodObj, std::string& method)
{
    napi_valuetype valueType = napi_null;
    napi_typeof(env, methodObj, &valueType);

    if (valueType == napi_string) {
        NapiParseUtils::ParseString(env, methodObj, method);
        if (method != "POST") {
            WVLOG_E("The method %{public}s is not supported.", method.c_str());
            return false;
        }
        return true;
    }

    WVLOG_E("Unable to parse type from method object.");
    return false;
}

bool ParseHttpHeaders(napi_env env, napi_value headersArray, std::map<std::string, std::string>* headers)
{
    bool isArray = false;
    napi_is_array(env, headersArray, &isArray);
    if (isArray) {
        uint32_t arrayLength = INTEGER_ZERO;
        napi_get_array_length(env, headersArray, &arrayLength);
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string key;
            std::string value;
            napi_value obj = nullptr;
            napi_value keyObj = nullptr;
            napi_value valueObj = nullptr;
            napi_get_element(env, headersArray, i, &obj);
            if (napi_get_named_property(env, obj, "headerKey", &keyObj) != napi_ok) {
                continue;
            }
            if (napi_get_named_property(env, obj, "headerValue", &valueObj) != napi_ok) {
                continue;
            }
            if (!NapiParseUtils::ParseString(env, keyObj, key) || !NapiParseUtils::ParseString(env, valueObj, value)) {
                WVLOG_E("Unable to parse string from headers array object.");
                return false;
            }
            if (key.empty()) {
                WVLOG_E("Key from headers is empty.");
                return false;
            }
            (*headers)[key] = value;
        }
    } else {
        WVLOG_E("Unable to parse type from headers array object.");
        return false;
    }
    return true;
}

bool ParseCacheKeyList(napi_env env, napi_value cacheKeyArray, std::vector<std::string>* cacheKeyList)
{
    bool isArray = false;
    napi_is_array(env, cacheKeyArray, &isArray);
    if (!isArray) {
        WVLOG_E("Unable to parse type from CacheKey array object.");
        return false;
    }
    uint32_t arrayLength = INTEGER_ZERO;
    napi_get_array_length(env, cacheKeyArray, &arrayLength);
    if (arrayLength == 0) {
        WVLOG_E("cacheKey array length is invalid");
        return false;
    }
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_value cacheKeyItem = nullptr;
        napi_get_element(env, cacheKeyArray, i, &cacheKeyItem);
        std::string cacheKeyStr;
        if (!NapiParseUtils::ParseString(env, cacheKeyItem, cacheKeyStr)) {
            WVLOG_E("Unable to parse string from cacheKey array object.");
            return false;
        }
        if (cacheKeyStr.empty()) {
            WVLOG_E("Cache Key is empty.");
            return false;
        }
        for (char c : cacheKeyStr) {
            if (!isalnum(c)) {
                WVLOG_E("Cache Key is invalid.");
                return false;
            }
        }
        cacheKeyList->emplace_back(cacheKeyStr);
    }
    return true;
}

std::shared_ptr<NWebEnginePrefetchArgs> ParsePrefetchArgs(napi_env env, napi_value preArgs)
{
    napi_value urlObj = nullptr;
    std::string url;
    napi_get_named_property(env, preArgs, "url", &urlObj);
    if (!ParsePrepareUrl(env, urlObj, url)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
        return nullptr;
    }

    napi_value methodObj = nullptr;
    std::string method;
    napi_get_named_property(env, preArgs, "method", &methodObj);
    if (!ParsePrepareRequestMethod(env, methodObj, method)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_value formDataObj = nullptr;
    std::string formData;
    napi_get_named_property(env, preArgs, "formData", &formDataObj);
    if (!NapiParseUtils::ParseString(env, formDataObj, formData)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    std::shared_ptr<NWebEnginePrefetchArgs> prefetchArgs = std::make_shared<NWebEnginePrefetchArgsImpl>(
        url, method, formData);
    return prefetchArgs;
}
} // namespace

int32_t NapiWebviewController::maxFdNum_ = -1;
std::atomic<int32_t> NapiWebviewController::usedFd_ {0};

thread_local napi_ref g_classWebMsgPort;
thread_local napi_ref g_historyListRef;
thread_local napi_ref g_webMsgExtClassRef;
thread_local napi_ref g_webPrintDocClassRef;
napi_value NapiWebviewController::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("initializeWebEngine", NapiWebviewController::InitializeWebEngine),
        DECLARE_NAPI_STATIC_FUNCTION("setHttpDns", NapiWebviewController::SetHttpDns),
        DECLARE_NAPI_STATIC_FUNCTION("setWebDebuggingAccess", NapiWebviewController::SetWebDebuggingAccess),
        DECLARE_NAPI_STATIC_FUNCTION("setServiceWorkerWebSchemeHandler",
                                     NapiWebviewController::SetServiceWorkerWebSchemeHandler),
        DECLARE_NAPI_STATIC_FUNCTION("clearServiceWorkerWebSchemeHandler",
                                     NapiWebviewController::ClearServiceWorkerWebSchemeHandler),
        DECLARE_NAPI_FUNCTION("getWebDebuggingAccess", NapiWebviewController::InnerGetWebDebuggingAccess),
        DECLARE_NAPI_FUNCTION("setWebId", NapiWebviewController::SetWebId),
        DECLARE_NAPI_FUNCTION("jsProxy", NapiWebviewController::InnerJsProxy),
        DECLARE_NAPI_FUNCTION("getCustomeSchemeCmdLine", NapiWebviewController::InnerGetCustomeSchemeCmdLine),
        DECLARE_NAPI_FUNCTION("accessForward", NapiWebviewController::AccessForward),
        DECLARE_NAPI_FUNCTION("accessBackward", NapiWebviewController::AccessBackward),
        DECLARE_NAPI_FUNCTION("accessStep", NapiWebviewController::AccessStep),
        DECLARE_NAPI_FUNCTION("clearHistory", NapiWebviewController::ClearHistory),
        DECLARE_NAPI_FUNCTION("forward", NapiWebviewController::Forward),
        DECLARE_NAPI_FUNCTION("backward", NapiWebviewController::Backward),
        DECLARE_NAPI_FUNCTION("onActive", NapiWebviewController::OnActive),
        DECLARE_NAPI_FUNCTION("onInactive", NapiWebviewController::OnInactive),
        DECLARE_NAPI_FUNCTION("refresh", NapiWebviewController::Refresh),
        DECLARE_NAPI_FUNCTION("zoomIn", NapiWebviewController::ZoomIn),
        DECLARE_NAPI_FUNCTION("zoomOut", NapiWebviewController::ZoomOut),
        DECLARE_NAPI_FUNCTION("getWebId", NapiWebviewController::GetWebId),
        DECLARE_NAPI_FUNCTION("getUserAgent", NapiWebviewController::GetUserAgent),
        DECLARE_NAPI_FUNCTION("getCustomUserAgent", NapiWebviewController::GetCustomUserAgent),
        DECLARE_NAPI_FUNCTION("setCustomUserAgent", NapiWebviewController::SetCustomUserAgent),
        DECLARE_NAPI_FUNCTION("getTitle", NapiWebviewController::GetTitle),
        DECLARE_NAPI_FUNCTION("getPageHeight", NapiWebviewController::GetPageHeight),
        DECLARE_NAPI_FUNCTION("backOrForward", NapiWebviewController::BackOrForward),
        DECLARE_NAPI_FUNCTION("storeWebArchive", NapiWebviewController::StoreWebArchive),
        DECLARE_NAPI_FUNCTION("createWebMessagePorts", NapiWebviewController::CreateWebMessagePorts),
        DECLARE_NAPI_FUNCTION("postMessage", NapiWebviewController::PostMessage),
        DECLARE_NAPI_FUNCTION("getHitTestValue", NapiWebviewController::GetHitTestValue),
        DECLARE_NAPI_FUNCTION("requestFocus", NapiWebviewController::RequestFocus),
        DECLARE_NAPI_FUNCTION("loadUrl", NapiWebviewController::LoadUrl),
        DECLARE_NAPI_FUNCTION("postUrl", NapiWebviewController::PostUrl),
        DECLARE_NAPI_FUNCTION("loadData", NapiWebviewController::LoadData),
        DECLARE_NAPI_FUNCTION("getHitTest", NapiWebviewController::GetHitTest),
        DECLARE_NAPI_FUNCTION("clearMatches", NapiWebviewController::ClearMatches),
        DECLARE_NAPI_FUNCTION("searchNext", NapiWebviewController::SearchNext),
        DECLARE_NAPI_FUNCTION("searchAllAsync", NapiWebviewController::SearchAllAsync),
        DECLARE_NAPI_FUNCTION("clearSslCache", NapiWebviewController::ClearSslCache),
        DECLARE_NAPI_FUNCTION("clearClientAuthenticationCache", NapiWebviewController::ClearClientAuthenticationCache),
        DECLARE_NAPI_FUNCTION("stop", NapiWebviewController::Stop),
        DECLARE_NAPI_FUNCTION("zoom", NapiWebviewController::Zoom),
        DECLARE_NAPI_FUNCTION("registerJavaScriptProxy", NapiWebviewController::RegisterJavaScriptProxy),
        DECLARE_NAPI_FUNCTION("innerCompleteWindowNew", NapiWebviewController::InnerCompleteWindowNew),
        DECLARE_NAPI_FUNCTION("deleteJavaScriptRegister", NapiWebviewController::DeleteJavaScriptRegister),
        DECLARE_NAPI_FUNCTION("runJavaScript", NapiWebviewController::RunJavaScript),
        DECLARE_NAPI_FUNCTION("runJavaScriptExt", NapiWebviewController::RunJavaScriptExt),
        DECLARE_NAPI_FUNCTION("getUrl", NapiWebviewController::GetUrl),
        DECLARE_NAPI_FUNCTION("terminateRenderProcess", NapiWebviewController::TerminateRenderProcess),
        DECLARE_NAPI_FUNCTION("getOriginalUrl", NapiWebviewController::GetOriginalUrl),
        DECLARE_NAPI_FUNCTION("setNetworkAvailable", NapiWebviewController::SetNetworkAvailable),
        DECLARE_NAPI_FUNCTION("innerGetWebId", NapiWebviewController::InnerGetWebId),
        DECLARE_NAPI_FUNCTION("hasImage", NapiWebviewController::HasImage),
        DECLARE_NAPI_FUNCTION("removeCache", NapiWebviewController::RemoveCache),
        DECLARE_NAPI_FUNCTION("getFavicon", NapiWebviewController::GetFavicon),
        DECLARE_NAPI_FUNCTION("getBackForwardEntries", NapiWebviewController::getBackForwardEntries),
        DECLARE_NAPI_FUNCTION("serializeWebState", NapiWebviewController::SerializeWebState),
        DECLARE_NAPI_FUNCTION("restoreWebState", NapiWebviewController::RestoreWebState),
        DECLARE_NAPI_FUNCTION("pageDown", NapiWebviewController::ScrollPageDown),
        DECLARE_NAPI_FUNCTION("pageUp", NapiWebviewController::ScrollPageUp),
        DECLARE_NAPI_FUNCTION("scrollTo", NapiWebviewController::ScrollTo),
        DECLARE_NAPI_FUNCTION("scrollBy", NapiWebviewController::ScrollBy),
        DECLARE_NAPI_FUNCTION("slideScroll", NapiWebviewController::SlideScroll),
        DECLARE_NAPI_FUNCTION("setScrollable", NapiWebviewController::SetScrollable),
        DECLARE_NAPI_FUNCTION("getScrollable", NapiWebviewController::GetScrollable),
        DECLARE_NAPI_STATIC_FUNCTION("customizeSchemes", NapiWebviewController::CustomizeSchemes),
        DECLARE_NAPI_FUNCTION("innerSetHapPath", NapiWebviewController::InnerSetHapPath),
        DECLARE_NAPI_FUNCTION("innerGetCertificate", NapiWebviewController::InnerGetCertificate),
        DECLARE_NAPI_FUNCTION("setAudioMuted", NapiWebviewController::SetAudioMuted),
        DECLARE_NAPI_FUNCTION("innerGetThisVar", NapiWebviewController::InnerGetThisVar),
        DECLARE_NAPI_FUNCTION("prefetchPage", NapiWebviewController::PrefetchPage),
        DECLARE_NAPI_FUNCTION("setDownloadDelegate", NapiWebviewController::SetDownloadDelegate),
        DECLARE_NAPI_FUNCTION("startDownload", NapiWebviewController::StartDownload),
        DECLARE_NAPI_STATIC_FUNCTION("prepareForPageLoad", NapiWebviewController::PrepareForPageLoad),
        DECLARE_NAPI_STATIC_FUNCTION("setConnectionTimeout", NapiWebviewController::SetConnectionTimeout),
        DECLARE_NAPI_FUNCTION("createWebPrintDocumentAdapter", NapiWebviewController::CreateWebPrintDocumentAdapter),
        DECLARE_NAPI_FUNCTION("getSecurityLevel", NapiWebviewController::GetSecurityLevel),
        DECLARE_NAPI_FUNCTION("enableSafeBrowsing", NapiWebviewController::EnableSafeBrowsing),
        DECLARE_NAPI_FUNCTION("isSafeBrowsingEnabled", NapiWebviewController::IsSafeBrowsingEnabled),
        DECLARE_NAPI_FUNCTION("isIncognitoMode", NapiWebviewController::IsIncognitoMode),
        DECLARE_NAPI_FUNCTION("setPrintBackground", NapiWebviewController::SetPrintBackground),
        DECLARE_NAPI_FUNCTION("getPrintBackground", NapiWebviewController::GetPrintBackground),
        DECLARE_NAPI_FUNCTION("setWebSchemeHandler", NapiWebviewController::SetWebSchemeHandler),
        DECLARE_NAPI_FUNCTION("clearWebSchemeHandler", NapiWebviewController::ClearWebSchemeHandler),
        DECLARE_NAPI_FUNCTION("closeAllMediaPresentations", NapiWebviewController::CloseAllMediaPresentations),
        DECLARE_NAPI_FUNCTION("stopAllMedia", NapiWebviewController::StopAllMedia),
        DECLARE_NAPI_FUNCTION("resumeAllMedia", NapiWebviewController::ResumeAllMedia),
        DECLARE_NAPI_FUNCTION("pauseAllMedia", NapiWebviewController::PauseAllMedia),
        DECLARE_NAPI_FUNCTION("getMediaPlaybackState", NapiWebviewController::GetMediaPlaybackState),
        DECLARE_NAPI_FUNCTION("enableIntelligentTrackingPrevention",
            NapiWebviewController::EnableIntelligentTrackingPrevention),
        DECLARE_NAPI_FUNCTION("isIntelligentTrackingPreventionEnabled",
            NapiWebviewController::IsIntelligentTrackingPreventionEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("addIntelligentTrackingPreventionBypassingList",
            NapiWebviewController::AddIntelligentTrackingPreventionBypassingList),
        DECLARE_NAPI_STATIC_FUNCTION("removeIntelligentTrackingPreventionBypassingList",
            NapiWebviewController::RemoveIntelligentTrackingPreventionBypassingList),
        DECLARE_NAPI_STATIC_FUNCTION("clearIntelligentTrackingPreventionBypassingList",
            NapiWebviewController::ClearIntelligentTrackingPreventionBypassingList),
        DECLARE_NAPI_STATIC_FUNCTION("pauseAllTimers", NapiWebviewController::PauseAllTimers),
        DECLARE_NAPI_STATIC_FUNCTION("resumeAllTimers", NapiWebviewController::ResumeAllTimers),
        DECLARE_NAPI_FUNCTION("startCamera", NapiWebviewController::StartCamera),
        DECLARE_NAPI_FUNCTION("stopCamera", NapiWebviewController::StopCamera),
        DECLARE_NAPI_FUNCTION("closeCamera", NapiWebviewController::CloseCamera),
        DECLARE_NAPI_FUNCTION("getLastJavascriptProxyCallingFrameUrl",
            NapiWebviewController::GetLastJavascriptProxyCallingFrameUrl),
        DECLARE_NAPI_FUNCTION("onCreateNativeMediaPlayer", NapiWebviewController::OnCreateNativeMediaPlayer),
        DECLARE_NAPI_STATIC_FUNCTION("prefetchResource", NapiWebviewController::PrefetchResource),
        DECLARE_NAPI_STATIC_FUNCTION("clearPrefetchedResource", NapiWebviewController::ClearPrefetchedResource),
        DECLARE_NAPI_STATIC_FUNCTION("setRenderProcessMode", NapiWebviewController::SetRenderProcessMode),
        DECLARE_NAPI_STATIC_FUNCTION("getRenderProcessMode", NapiWebviewController::GetRenderProcessMode),
        DECLARE_NAPI_FUNCTION("precompileJavaScript", NapiWebviewController::PrecompileJavaScript),
        DECLARE_NAPI_STATIC_FUNCTION("warmupServiceWorker", NapiWebviewController::WarmupServiceWorker),
        DECLARE_NAPI_FUNCTION("injectOfflineResources", NapiWebviewController::InjectOfflineResource),
        DECLARE_NAPI_STATIC_FUNCTION("setHostIP", NapiWebviewController::SetHostIP),
        DECLARE_NAPI_STATIC_FUNCTION("clearHostIP", NapiWebviewController::ClearHostIP),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, WEBVIEW_CONTROLLER_CLASS_NAME.c_str(), WEBVIEW_CONTROLLER_CLASS_NAME.length(),
        NapiWebviewController::JsConstructor, nullptr, sizeof(properties) / sizeof(properties[0]),
        properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "define js class WebviewController failed");
    napi_status status = napi_set_named_property(env, exports, "WebviewController", constructor);
    NAPI_ASSERT(env, status == napi_ok, "set property WebviewController failed");

    napi_value webMsgTypeEnum = nullptr;
    napi_property_descriptor webMsgTypeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NOT_SUPPORT", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::NOTSUPPORT))),
        DECLARE_NAPI_STATIC_PROPERTY("STRING", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::STRING))),
        DECLARE_NAPI_STATIC_PROPERTY("NUMBER", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::NUMBER))),
        DECLARE_NAPI_STATIC_PROPERTY("BOOLEAN", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::BOOLEAN))),
        DECLARE_NAPI_STATIC_PROPERTY("ARRAY_BUFFER", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::ARRAYBUFFER))),
        DECLARE_NAPI_STATIC_PROPERTY("ARRAY", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::ARRAY))),
        DECLARE_NAPI_STATIC_PROPERTY("ERROR", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebMessageType::ERROR)))
    };
    napi_define_class(env, WEB_PORT_MSG_ENUM_NAME.c_str(), WEB_PORT_MSG_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(webMsgTypeProperties) /
        sizeof(webMsgTypeProperties[0]), webMsgTypeProperties, &webMsgTypeEnum);
    napi_set_named_property(env, exports, WEB_PORT_MSG_ENUM_NAME.c_str(), webMsgTypeEnum);

    napi_value webMsgExtClass = nullptr;
    napi_property_descriptor webMsgExtClsProperties[] = {
        DECLARE_NAPI_FUNCTION("getType", NapiWebMessageExt::GetType),
        DECLARE_NAPI_FUNCTION("getString", NapiWebMessageExt::GetString),
        DECLARE_NAPI_FUNCTION("getNumber", NapiWebMessageExt::GetNumber),
        DECLARE_NAPI_FUNCTION("getBoolean", NapiWebMessageExt::GetBoolean),
        DECLARE_NAPI_FUNCTION("getArrayBuffer", NapiWebMessageExt::GetArrayBuffer),
        DECLARE_NAPI_FUNCTION("getArray", NapiWebMessageExt::GetArray),
        DECLARE_NAPI_FUNCTION("getError", NapiWebMessageExt::GetError),
        DECLARE_NAPI_FUNCTION("setType", NapiWebMessageExt::SetType),
        DECLARE_NAPI_FUNCTION("setString", NapiWebMessageExt::SetString),
        DECLARE_NAPI_FUNCTION("setNumber", NapiWebMessageExt::SetNumber),
        DECLARE_NAPI_FUNCTION("setBoolean", NapiWebMessageExt::SetBoolean),
        DECLARE_NAPI_FUNCTION("setArrayBuffer", NapiWebMessageExt::SetArrayBuffer),
        DECLARE_NAPI_FUNCTION("setArray", NapiWebMessageExt::SetArray),
        DECLARE_NAPI_FUNCTION("setError", NapiWebMessageExt::SetError)
    };
    napi_define_class(env, WEB_EXT_MSG_CLASS_NAME.c_str(), WEB_EXT_MSG_CLASS_NAME.length(),
        NapiWebMessageExt::JsConstructor, nullptr, sizeof(webMsgExtClsProperties) / sizeof(webMsgExtClsProperties[0]),
        webMsgExtClsProperties, &webMsgExtClass);
    napi_create_reference(env, webMsgExtClass, 1, &g_webMsgExtClassRef);
    napi_set_named_property(env, exports, WEB_EXT_MSG_CLASS_NAME.c_str(), webMsgExtClass);

    napi_value securityLevelEnum = nullptr;
    napi_property_descriptor securityLevelProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NONE", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecurityLevel::NONE))),
        DECLARE_NAPI_STATIC_PROPERTY("SECURE", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecurityLevel::SECURE))),
        DECLARE_NAPI_STATIC_PROPERTY("WARNING", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecurityLevel::WARNING))),
        DECLARE_NAPI_STATIC_PROPERTY("DANGEROUS", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecurityLevel::DANGEROUS)))
    };
    napi_define_class(env, WEB_SECURITY_LEVEL_ENUM_NAME.c_str(), WEB_SECURITY_LEVEL_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(securityLevelProperties) /
        sizeof(securityLevelProperties[0]), securityLevelProperties, &securityLevelEnum);
    napi_set_named_property(env, exports, WEB_SECURITY_LEVEL_ENUM_NAME.c_str(), securityLevelEnum);

    napi_value msgPortCons = nullptr;
    napi_property_descriptor msgPortProperties[] = {
        DECLARE_NAPI_FUNCTION("close", NapiWebMessagePort::Close),
        DECLARE_NAPI_FUNCTION("postMessageEvent", NapiWebMessagePort::PostMessageEvent),
        DECLARE_NAPI_FUNCTION("onMessageEvent", NapiWebMessagePort::OnMessageEvent),
        DECLARE_NAPI_FUNCTION("postMessageEventExt", NapiWebMessagePort::PostMessageEventExt),
        DECLARE_NAPI_FUNCTION("onMessageEventExt", NapiWebMessagePort::OnMessageEventExt)
    };
    NAPI_CALL(env, napi_define_class(env, WEB_MESSAGE_PORT_CLASS_NAME.c_str(), WEB_MESSAGE_PORT_CLASS_NAME.length(),
        NapiWebMessagePort::JsConstructor, nullptr, sizeof(msgPortProperties) / sizeof(msgPortProperties[0]),
        msgPortProperties, &msgPortCons));
    NAPI_CALL(env, napi_create_reference(env, msgPortCons, 1, &g_classWebMsgPort));
    NAPI_CALL(env, napi_set_named_property(env, exports, WEB_MESSAGE_PORT_CLASS_NAME.c_str(), msgPortCons));

    napi_value hitTestTypeEnum = nullptr;
    napi_property_descriptor hitTestTypeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("EditText", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::EDIT))),
        DECLARE_NAPI_STATIC_PROPERTY("Email", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::EMAIL))),
        DECLARE_NAPI_STATIC_PROPERTY("HttpAnchor", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::HTTP))),
        DECLARE_NAPI_STATIC_PROPERTY("HttpAnchorImg", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::HTTP_IMG))),
        DECLARE_NAPI_STATIC_PROPERTY("Img", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::IMG))),
        DECLARE_NAPI_STATIC_PROPERTY("Map", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::MAP))),
        DECLARE_NAPI_STATIC_PROPERTY("Phone", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::PHONE))),
        DECLARE_NAPI_STATIC_PROPERTY("Unknown", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(WebHitTestType::UNKNOWN))),
    };
    napi_define_class(env, WEB_HITTESTTYPE_V9_ENUM_NAME.c_str(), WEB_HITTESTTYPE_V9_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(hitTestTypeProperties) /
        sizeof(hitTestTypeProperties[0]), hitTestTypeProperties, &hitTestTypeEnum);
    napi_set_named_property(env, exports, WEB_HITTESTTYPE_V9_ENUM_NAME.c_str(), hitTestTypeEnum);

    napi_define_class(env, WEB_HITTESTTYPE_ENUM_NAME.c_str(), WEB_HITTESTTYPE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(hitTestTypeProperties) /
        sizeof(hitTestTypeProperties[0]), hitTestTypeProperties, &hitTestTypeEnum);
    napi_set_named_property(env, exports, WEB_HITTESTTYPE_ENUM_NAME.c_str(), hitTestTypeEnum);

    napi_value secureDnsModeEnum = nullptr;
    napi_property_descriptor secureDnsModeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("Off", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecureDnsModeType::OFF))),
        DECLARE_NAPI_STATIC_PROPERTY("Auto", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecureDnsModeType::AUTO))),
        DECLARE_NAPI_STATIC_PROPERTY("SecureOnly", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecureDnsModeType::SECURE_ONLY))),
        DECLARE_NAPI_STATIC_PROPERTY("OFF", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecureDnsModeType::OFF))),
        DECLARE_NAPI_STATIC_PROPERTY("AUTO", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecureDnsModeType::AUTO))),
        DECLARE_NAPI_STATIC_PROPERTY("SECURE_ONLY", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(SecureDnsModeType::SECURE_ONLY))),
    };
    napi_define_class(env, WEB_SECURE_DNS_MODE_ENUM_NAME.c_str(), WEB_SECURE_DNS_MODE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(secureDnsModeProperties) /
        sizeof(secureDnsModeProperties[0]), secureDnsModeProperties, &secureDnsModeEnum);
    napi_set_named_property(env, exports, WEB_SECURE_DNS_MODE_ENUM_NAME.c_str(), secureDnsModeEnum);

    napi_value historyList = nullptr;
    napi_property_descriptor historyListProperties[] = {
        DECLARE_NAPI_FUNCTION("getItemAtIndex", NapiWebHistoryList::GetItem)
    };
    napi_define_class(env, WEB_HISTORY_LIST_CLASS_NAME.c_str(), WEB_HISTORY_LIST_CLASS_NAME.length(),
        NapiWebHistoryList::JsConstructor, nullptr, sizeof(historyListProperties) / sizeof(historyListProperties[0]),
        historyListProperties, &historyList);
    napi_create_reference(env, historyList, 1, &g_historyListRef);
    napi_set_named_property(env, exports, WEB_HISTORY_LIST_CLASS_NAME.c_str(), historyList);

    napi_value webPrintDoc = nullptr;
    napi_property_descriptor WebPrintDocumentClass[] = {
        DECLARE_NAPI_FUNCTION("onStartLayoutWrite", NapiWebPrintDocument::OnStartLayoutWrite),
        DECLARE_NAPI_FUNCTION("onJobStateChanged", NapiWebPrintDocument::OnJobStateChanged),
    };
    napi_define_class(env, WEB_PRINT_DOCUMENT_CLASS_NAME.c_str(), WEB_PRINT_DOCUMENT_CLASS_NAME.length(),
        NapiWebPrintDocument::JsConstructor, nullptr,
        sizeof(WebPrintDocumentClass) / sizeof(WebPrintDocumentClass[0]),
        WebPrintDocumentClass, &webPrintDoc);
    napi_create_reference(env, webPrintDoc, 1, &g_webPrintDocClassRef);
    napi_set_named_property(env, exports, WEB_PRINT_DOCUMENT_CLASS_NAME.c_str(), webPrintDoc);

    napi_value renderProcessModeEnum = nullptr;
    napi_property_descriptor renderProcessModeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("SINGLE", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(RenderProcessMode::SINGLE_MODE))),
        DECLARE_NAPI_STATIC_PROPERTY("MULTIPLE", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(RenderProcessMode::MULTIPLE_MODE))),
    };
    napi_define_class(env, WEB_RENDER_PROCESS_MODE_ENUM_NAME.c_str(), WEB_RENDER_PROCESS_MODE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(renderProcessModeProperties) /
        sizeof(renderProcessModeProperties[0]), renderProcessModeProperties, &renderProcessModeEnum);
    napi_set_named_property(env, exports, WEB_RENDER_PROCESS_MODE_ENUM_NAME.c_str(), renderProcessModeEnum);

    napi_value offlineResourceTypeEnum = nullptr;
    napi_property_descriptor offlineResourceTypeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("IMAGE", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(OfflineResourceType::IMAGE))),
        DECLARE_NAPI_STATIC_PROPERTY("CSS", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(OfflineResourceType::CSS))),
        DECLARE_NAPI_STATIC_PROPERTY("CLASSIC_JS", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(OfflineResourceType::CLASSIC_JS))),
        DECLARE_NAPI_STATIC_PROPERTY("MODULE_JS", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(OfflineResourceType::MODULE_JS))),
    };
    napi_define_class(env, OFFLINE_RESOURCE_TYPE_ENUM_NAME.c_str(), OFFLINE_RESOURCE_TYPE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(offlineResourceTypeProperties) /
        sizeof(offlineResourceTypeProperties[0]), offlineResourceTypeProperties, &offlineResourceTypeEnum);
    napi_set_named_property(env, exports, OFFLINE_RESOURCE_TYPE_ENUM_NAME.c_str(), offlineResourceTypeEnum);

    WebviewJavaScriptExecuteCallback::InitJSExcute(env, exports);
    return exports;
}

napi_value NapiWebviewController::JsConstructor(napi_env env, napi_callback_info info)
{
    WVLOG_I("NapiWebviewController::JsConstructor start");
    napi_value thisVar = nullptr;

    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    WebviewController *webviewController;
    std::string webTag;
    if (argc == 1) {
        NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], webTag);
        if (webTag.empty()) {
            WVLOG_E("native webTag is empty");
            return nullptr;
        }
        webviewController = new (std::nothrow) WebviewController(webTag);
        WVLOG_I("new webview controller webname:%{public}s", webTag.c_str());
    } else {
        webTag = WebviewController::GenerateWebTag();
        webviewController = new (std::nothrow) WebviewController(webTag);
    }
    WebviewController::webTagSet_.insert(webTag);

    if (webviewController == nullptr) {
        WVLOG_E("new webview controller failed");
        return nullptr;
    }
    napi_status status = napi_wrap(
        env, thisVar, webviewController,
        [](napi_env env, void *data, void *hint) {
            WebviewController *webviewController = static_cast<WebviewController *>(data);
            delete webviewController;
        },
        nullptr, nullptr);
    if (status != napi_ok) {
        WVLOG_E("Wrap native webviewController failed.");
        return nullptr;
    }
    WebviewController::webDebuggingAccess_ = OHOS::system::GetBoolParameter("web.debug.devtools", false);
    return thisVar;
}

napi_value NapiWebviewController::InitializeWebEngine(napi_env env, napi_callback_info info)
{
    WVLOG_D("InitializeWebEngine invoked.");

    // obtain bundle path
    std::shared_ptr<AbilityRuntime::ApplicationContext> ctx =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!ctx) {
        WVLOG_E("Failed to init web engine due to nil application context.");
        return nullptr;
    }

    // load so
    const std::string& bundlePath = ctx->GetBundleCodeDir();
    NWebHelper::Instance().SetBundlePath(bundlePath);
    if (!NWebHelper::Instance().InitAndRun(true)) {
        WVLOG_E("Failed to init web engine due to NWebHelper failure.");
        return nullptr;
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    WVLOG_I("NWebHelper initialized, init web engine done, bundle_path: %{public}s", bundlePath.c_str());
    return result;
}

napi_value NapiWebviewController::SetHttpDns(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    int dohMode;
    std::string dohConfig;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], dohMode)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (dohMode < static_cast<int>(SecureDnsModeType::OFF) ||
        dohMode > static_cast<int>(SecureDnsModeType::SECURE_ONLY)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], dohConfig)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (dohConfig.rfind("https", 0) != 0 && dohConfig.rfind("HTTPS", 0) != 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
    config->SetMode(dohMode);
    config->SetConfig(dohConfig);
    WVLOG_I("set http dns mode:%{public}d doh_config:%{public}s", dohMode, dohConfig.c_str());

    NWebHelper::Instance().SetHttpDns(config);

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::SetWebDebuggingAccess(napi_env env, napi_callback_info info)
{
    WVLOG_D("SetWebDebuggingAccess start");
    napi_value result = nullptr;
    if (OHOS::system::GetBoolParameter("web.debug.devtools", false)) {
        return result;
    }
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    bool webDebuggingAccess = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], webDebuggingAccess)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "webDebuggingAccess","boolean"));
        return result;
    }
    WebviewController::webDebuggingAccess_ = webDebuggingAccess;

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::EnableSafeBrowsing(napi_env env, napi_callback_info info)
{
    WVLOG_D("EnableSafeBrowsing start");
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool safe_browsing_enable = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], safe_browsing_enable)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->EnableSafeBrowsing(safe_browsing_enable);
    return result;
}

napi_value NapiWebviewController::IsSafeBrowsingEnabled(napi_env env, napi_callback_info info)
{
    WVLOG_D("IsSafeBrowsingEnabled start");
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    bool is_safe_browsing_enabled = webviewController->IsSafeBrowsingEnabled();
    NAPI_CALL(env, napi_get_boolean(env, is_safe_browsing_enabled, &result));
    return result;
}

napi_value NapiWebviewController::InnerGetWebDebuggingAccess(napi_env env, napi_callback_info info)
{
    WVLOG_D("InnerGetWebDebuggingAccess start");
    bool webDebuggingAccess = WebviewController::webDebuggingAccess_;
    napi_value result = nullptr;
    napi_get_boolean(env, webDebuggingAccess, &result);
    return result;
}

napi_value NapiWebviewController::InnerGetThisVar(napi_env env, napi_callback_info info)
{
    WVLOG_D("InnerGetThisVar start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok)) {
        WVLOG_E("webviewController is nullptr.");
        napi_create_int64(env, 0, &result);
    } else {
        napi_create_int64(env, reinterpret_cast<int64_t>(webviewController), &result);
    }
    return result;
}

napi_value NapiWebviewController::SetWebId(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    int32_t webId = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[0], webId)) {
        WVLOG_E("Parse web id failed.");
        return nullptr;
    }
    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok)) {
        WVLOG_E("webviewController is nullptr.");
        return nullptr;
    }
    webviewController->SetWebId(webId);
    return thisVar;
}

napi_value NapiWebviewController::InnerSetHapPath(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("Failed to run InnerSetHapPath beacuse of wrong Param number.");
        return result;
    }
    std::string hapPath;
    if (!NapiParseUtils::ParseString(env, argv[0], hapPath)) {
        WVLOG_E("Parse hap path failed.");
        return result;
    }
    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        WVLOG_E("Wrap webviewController failed. WebviewController must be associated with a Web component.");
        return result;
    }
    webviewController->InnerSetHapPath(hapPath);
    return result;
}

napi_value NapiWebviewController::InnerJsProxy(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebviewController::InnerJsProxy");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_FOUR;
    napi_value argv[INTEGER_FOUR] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_FOUR) {
        WVLOG_E("Failed to run InnerJsProxy beacuse of wrong Param number.");
        return result;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_object) {
        WVLOG_E("Failed to run InnerJsProxy beacuse of wrong Param type.");
        return result;
    }

    std::string objName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], objName)) {
        WVLOG_E("Failed to run InnerJsProxy beacuse of wrong object name.");
        return result;
    }

    std::vector<std::string> methodList;
    bool hasSyncMethod = NapiParseUtils::ParseStringArray(env, argv[INTEGER_TWO], methodList);
    std::vector<std::string> asyncMethodList;
    bool hasAsyncMethod = NapiParseUtils::ParseStringArray(env, argv[INTEGER_THREE], asyncMethodList);
    if (!hasSyncMethod && !hasAsyncMethod) {
        WVLOG_E("Failed to run InnerJsProxy beacuse of empty method lists.");
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        WVLOG_E("Failed to run InnerJsProxy. The WebviewController must be associted with a Web component.");
        return result;
    }
    controller->SetNWebJavaScriptResultCallBack();
    controller->RegisterJavaScriptProxy(env, argv[INTEGER_ZERO], objName, methodList, asyncMethodList);
    return result;
}

napi_value NapiWebviewController::InnerGetCustomeSchemeCmdLine(napi_env env, napi_callback_info info)
{
    WebviewController::existNweb_ = true;
    napi_value result = nullptr;
    std::string cmdLine = WebviewController::customeSchemeCmdLine_;
    napi_create_string_utf8(env, cmdLine.c_str(), cmdLine.length(), &result);
    return result;
}

napi_value NapiWebviewController::AccessForward(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    bool access = webviewController->AccessForward();
    NAPI_CALL(env, napi_get_boolean(env, access, &result));
    return result;
}

napi_value NapiWebviewController::AccessBackward(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    bool access = webviewController->AccessBackward();
    NAPI_CALL(env, napi_get_boolean(env, access, &result));
    return result;
}

napi_value NapiWebviewController::Forward(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->Forward();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::Backward(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->Backward();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::AccessStep(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    int32_t step = INTEGER_ZERO;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], step)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "step", "number"));
        return nullptr;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    bool access = webviewController->AccessStep(step);
    NAPI_CALL(env, napi_get_boolean(env, access, &result));
    return result;
}

napi_value NapiWebviewController::ClearHistory(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->ClearHistory();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::OnActive(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->OnActive();
    WVLOG_D("The web component has been successfully activated");
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::OnInactive(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->OnInactive();
    WVLOG_D("The web component has been successfully inactivated");
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::Refresh(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->Refresh();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}


napi_value NapiWebMessageExt::JsConstructor(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::JsConstructor");
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
    WebMessageExt *webMessageExt = new (std::nothrow) WebMessageExt(webMsg);
    if (webMessageExt == nullptr) {
        WVLOG_E("new msg port failed");
        return nullptr;
    }
    NAPI_CALL(env, napi_wrap(env, thisVar, webMessageExt,
        [](napi_env env, void *data, void *hint) {
            WebMessageExt *webMessageExt = static_cast<WebMessageExt *>(data);
            delete webMessageExt;
        },
        nullptr, nullptr));
    return thisVar;
}

napi_value NapiWebMessageExt::GetType(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::GetType start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_get_cb_info status not ok");
        return result;
    }

    if (thisVar == nullptr) {
        WVLOG_E("napi_get_cb_info thisVar is nullptr");
        return result;
    }

    WebMessageExt *webMessageExt = nullptr;
    status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    int32_t type = webMessageExt->GetType();
    status = napi_create_int32(env, type, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_create_int32 failed.");
        return result;
    }
    return result;
}

napi_value NapiWebMessageExt::GetString(napi_env env, napi_callback_info info)
{
    WVLOG_D(" GetString webJsMessageExt start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(WebMessageType::STRING)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetData(), result);
    return result;
}

napi_value NapiWebMessageExt::GetNumber(napi_env env, napi_callback_info info)
{
    WVLOG_D("GetNumber webJsMessageExt start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(WebMessageType::NUMBER)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        WVLOG_E("GetNumber webJsMessageExt failed,not match");
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetData(), result);
    return result;
}

napi_value NapiWebMessageExt::GetBoolean(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(WebMessageType::BOOLEAN)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetData(), result);
    return result;
}

napi_value NapiWebMessageExt::GetArrayBuffer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAYBUFFER)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }
    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetData(), result);
    return result;
}

napi_value NapiWebMessageExt::GetArray(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetData(), result);
    return result;
}

napi_value NapiWebMessageExt::GetError(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ERROR)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetData(), result);
    return result;
}

napi_value NapiWebMessageExt::SetType(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetType");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    int type = -1;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_ok) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "type"));
        WVLOG_E("NapiWebMessageExt::SetType napi_get_cb_info failed");
        return result;
    }
    if (thisVar == nullptr) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NOT_NULL, "type"));
        WVLOG_E("NapiWebMessageExt::SetType thisVar is null");
        return result;
    }
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    if (!NapiParseUtils::ParseInt32(env, argv[0], type)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, ParamCheckErrorMsgTemplate::TYPE_ALL_INT);
        return result;
    }
    if (type <= static_cast<int>(WebMessageType::NOTSUPPORT) || type > static_cast<int>(WebMessageType::ERROR)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    WebMessageExt *webMessageExt = nullptr;
    status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if (status != napi_ok) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "type"));
        WVLOG_E("NapiWebMessageExt::SetType napi_unwrap failed");
        return result;
    }
    if (!webMessageExt) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NOT_NULL, "type"));
        WVLOG_E("NapiWebMessageExt::SetType webMessageExt is null");
        return result;
    }
    webMessageExt->SetType(type);
    return result;
}

napi_value NapiWebMessageExt::SetString(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetString start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    std::string value;
    if (!NapiParseUtils::ParseString(env, argv[0], value)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "string"));
        return result;
    }
    WebMessageExt *webMessageExt = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    int32_t type = webMessageExt->GetType();
    if (type != static_cast<int32_t>(WebMessageType::STRING)) {
        WVLOG_E("web message SetString error type:%{public}d", type);
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    webMessageExt->SetString(value);
    return result;
}

napi_value NapiWebMessageExt::SetNumber(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetNumber start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    double value = 0;
    if (!NapiParseUtils::ParseDouble(env, argv[0], value)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "number"));
        return result;
    }

    WebMessageExt *webMessageExt = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    int32_t type = webMessageExt->GetType();
    if (type != static_cast<int32_t>(WebMessageType::NUMBER)) {
        WVLOG_E("web message SetNumber error type:%{public}d", type);
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    webMessageExt->SetNumber(value);
    return result;
}

napi_value NapiWebMessageExt::SetBoolean(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetBoolean start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    bool value = 0;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], value)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "boolean"));
        return result;
    }

    WebMessageExt *webMessageExt = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    int32_t type = webMessageExt->GetType();
    if (type != static_cast<int32_t>(WebMessageType::BOOLEAN)) {
        WVLOG_E("web message SetBoolean error type:%{public}d", type);
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    webMessageExt->SetBoolean(value);
    return result;
}

napi_value NapiWebMessageExt::SetArrayBuffer(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetArrayBuffer start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    bool isArrayBuffer = false;
    NAPI_CALL(env, napi_is_arraybuffer(env, argv[INTEGER_ZERO], &isArrayBuffer));
    if (!isArrayBuffer) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "arrayBuffer"));
        return result;
    }

    uint8_t *arrBuf = nullptr;
    size_t byteLength = 0;
    napi_get_arraybuffer_info(env, argv[INTEGER_ZERO], (void**)&arrBuf, &byteLength);
    std::vector<uint8_t> vecData(arrBuf, arrBuf + byteLength);
    WebMessageExt *webMessageExt = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
             NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    int32_t type = webMessageExt->GetType();
    if (type != static_cast<int32_t>(WebMessageType::ARRAYBUFFER)) {
        WVLOG_E("web message SetArrayBuffer error type:%{public}d", type);
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    webMessageExt->SetArrayBuffer(vecData);
    return result;
}

napi_value NapiWebMessageExt::SetArray(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetArray start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[INTEGER_ZERO], &isArray));
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "array"));
        return result;
    }
    WebMessageExt *webMessageExt = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }
    int32_t type = webMessageExt->GetType();
    if (type != static_cast<int32_t>(WebMessageType::ARRAY)) {
        WVLOG_E("web message SetArray error type:%{public}d", type);
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    bool isDouble = false;
    napi_valuetype valueType = GetArrayValueType(env, argv[INTEGER_ZERO], isDouble);
    if (valueType == napi_undefined) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "number"));
        return result;
    }
    using SetArrayHandler = std::function<void(napi_env, napi_value, WebMessageExt*)>;
    const std::unordered_map<napi_valuetype, SetArrayHandler> functionMap = {
        { napi_boolean, SetArrayHandlerBoolean },
        { napi_string, SetArrayHandlerString },
        { napi_number, [isDouble](napi_env env, napi_value array, WebMessageExt* msgExt) {
            isDouble ? SetArrayHandlerDouble(env, array, msgExt)
                     : SetArrayHandlerInteger(env, array, msgExt);
        } }
    };
    auto it = functionMap.find(valueType);
    if (it != functionMap.end()) {
        it->second(env, argv[INTEGER_ZERO], webMessageExt);
    }
    return result;
}

napi_value NapiWebMessageExt::SetError(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::SetError start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    bool isError = false;
    NAPI_CALL(env, napi_is_error(env, argv[INTEGER_ZERO], &isError));
    if (!isError) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "message", "error"));
        return result;
    }

    napi_value nameObj = 0;
    napi_get_named_property(env, argv[INTEGER_ZERO], "name", &nameObj);
    std::string nameVal;
    if (!NapiParseUtils::ParseString(env, nameObj, nameVal)) {
        return result;
    }

    napi_value msgObj = 0;
    napi_get_named_property(env, argv[INTEGER_ZERO], "message", &msgObj);
    std::string msgVal;
    if (!NapiParseUtils::ParseString(env, msgObj, msgVal)) {
        return result;
    }

    WebMessageExt *webMessageExt = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webMessageExt);
    if ((!webMessageExt) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    int32_t type = webMessageExt->GetType();
    if (type != static_cast<int32_t>(WebMessageType::ERROR)) {
        WVLOG_E("web message SetError error type:%{public}d", type);
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }
    webMessageExt->SetError(nameVal, msgVal);
    return result;
}

napi_value NapiWebviewController::CreateWebMessagePorts(napi_env env, napi_callback_info info)
{
    WVLOG_D("create web message port");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    bool isExtentionType = false;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_ZERO && argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "zero", "one"));
        return result;
    }

    if (argc == INTEGER_ONE) {
        if (!NapiParseUtils::ParseBoolean(env, argv[0], isExtentionType)) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "isExtentionType", "boolean"));
            return result;
        }
    }

    WebviewController *webviewController = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        WVLOG_E("create message port failed, napi unwrap webviewController failed");
        return nullptr;
    }
    int32_t nwebId = webviewController->GetWebId();
    std::vector<std::string> ports = webviewController->CreateWebMessagePorts();
    if (ports.size() != INTEGER_TWO) {
        WVLOG_E("create web message port failed");
        return result;
    }
    napi_value msgPortcons = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, g_classWebMsgPort, &msgPortcons));
    napi_create_array(env, &result);
    napi_value consParam[INTEGER_TWO][INTEGER_THREE] = {{0}};
    for (uint32_t i = 0; i < INTEGER_TWO; i++) {
        napi_value msgPortObj = nullptr;
        NAPI_CALL(env, napi_create_int32(env, nwebId, &consParam[i][INTEGER_ZERO]));
        NAPI_CALL(env, napi_create_string_utf8(env, ports[i].c_str(), ports[i].length(), &consParam[i][INTEGER_ONE]));
        NAPI_CALL(env, napi_get_boolean(env, isExtentionType, &consParam[i][INTEGER_TWO]));
        NAPI_CALL(env, napi_new_instance(env, msgPortcons, INTEGER_THREE, consParam[i], &msgPortObj));
        napi_value jsExtention;
        napi_get_boolean(env, isExtentionType, &jsExtention);
        napi_set_named_property(env, msgPortObj, "isExtentionType", jsExtention);

        napi_set_element(env, result, i, msgPortObj);
    }

    return result;
}

bool GetSendPorts(napi_env env, napi_value argv, std::vector<std::string>& sendPorts)
{
    uint32_t arrayLen = 0;
    napi_get_array_length(env, argv, &arrayLen);
    if (arrayLen == 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return false;
    }

    napi_valuetype valueType = napi_undefined;
    for (uint32_t i = 0; i < arrayLen; i++) {
        napi_value portItem = nullptr;
        napi_get_element(env, argv, i, &portItem);
        napi_typeof(env, portItem, &valueType);
        if (valueType != napi_object) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return false;
        }
        WebMessagePort *msgPort = nullptr;
        napi_status status = napi_unwrap(env, portItem, (void **)&msgPort);
        if ((!msgPort) || (status != napi_ok)) {
            WVLOG_E("post port to html failed, napi unwrap msg port fail");
            return false;
        }
        std::string portHandle = msgPort->GetPortHandle();
        sendPorts.emplace_back(portHandle);
    }
    return true;
}

napi_value NapiWebviewController::PostMessage(napi_env env, napi_callback_info info)
{
    WVLOG_D("post message port");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_THREE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "three"));
        return result;
    }

    std::string portName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], portName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "name", "string"));
        return result;
    }

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[INTEGER_ONE], &isArray));
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "ports", "array"));
        return result;
    }
    std::vector<std::string> sendPorts;
    if (!GetSendPorts(env, argv[INTEGER_ONE], sendPorts)) {
        WVLOG_E("post port to html failed, getSendPorts fail");
        return result;
    }

    std::string urlStr;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_TWO], urlStr)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "uri", "string"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        WVLOG_E("post port to html failed, napi unwrap webviewController failed");
        return nullptr;
    }

    webviewController->PostWebMessage(portName, sendPorts, urlStr);
    NAPI_CALL(env, napi_get_undefined(env, &result));

    return result;
}

napi_value NapiWebMessagePort::JsConstructor(napi_env env, napi_callback_info info)
{
    WVLOG_D("web message port construct");
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = {0};
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    int32_t webId = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], webId)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string portHandle;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], portHandle)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    bool isExtentionType = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_TWO], isExtentionType)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    WebMessagePort *msgPort = new (std::nothrow) WebMessagePort(webId, portHandle, isExtentionType);
    if (msgPort == nullptr) {
        WVLOG_E("new msg port failed");
        return nullptr;
    }
    NAPI_CALL(env, napi_wrap(env, thisVar, msgPort,
        [](napi_env env, void *data, void *hint) {
            WebMessagePort *msgPort = static_cast<WebMessagePort *>(data);
            delete msgPort;
        },
        nullptr, nullptr));
    return thisVar;
}

napi_value NapiWebMessagePort::Close(napi_env env, napi_callback_info info)
{
    WVLOG_D("close message port");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    WebMessagePort *msgPort = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&msgPort));
    if (msgPort == nullptr) {
        WVLOG_E("close message port failed, napi unwrap msg port failed");
        return nullptr;
    }
    ErrCode ret = msgPort->ClosePort();
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));

    return result;
}

bool PostMessageEventMsgHandler(napi_env env, napi_value argv, napi_valuetype valueType, bool isArrayBuffer,
    std::shared_ptr<NWebMessage> webMsg)
{
    if (valueType == napi_string) {
        size_t bufferSize = 0;
        napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
        if (bufferSize > UINT_MAX) {
            WVLOG_E("String length is too long");
            return false;
        }
        char* stringValue = new (std::nothrow) char[bufferSize + 1];
        if (stringValue == nullptr) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return false;
        }
        size_t jsStringLength = 0;
        napi_get_value_string_utf8(env, argv, stringValue, bufferSize + 1, &jsStringLength);
        std::string message(stringValue);
        delete [] stringValue;
        stringValue = nullptr;

        webMsg->SetType(NWebValue::Type::STRING);
        webMsg->SetString(message);
    } else if (isArrayBuffer) {
        uint8_t *arrBuf = nullptr;
        size_t byteLength = 0;
        napi_get_arraybuffer_info(env, argv, (void**)&arrBuf, &byteLength);
        std::vector<uint8_t> vecData(arrBuf, arrBuf + byteLength);
        webMsg->SetType(NWebValue::Type::BINARY);
        webMsg->SetBinary(vecData);
    }
    return true;
}

napi_value NapiWebMessagePort::PostMessageEvent(napi_env env, napi_callback_info info)
{
    WVLOG_D("message port post message");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);

    bool isArrayBuffer = false;
    NAPI_CALL(env, napi_is_arraybuffer(env, argv[INTEGER_ZERO], &isArrayBuffer));
    if (valueType != napi_string && !isArrayBuffer) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
          NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
    if (!PostMessageEventMsgHandler(env, argv[INTEGER_ZERO], valueType, isArrayBuffer, webMsg)) {
        WVLOG_E("post message failed, PostMessageEventMsgHandler failed");
        return result;
    }

    WebMessagePort *msgPort = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&msgPort));
    if (msgPort == nullptr) {
        WVLOG_E("post message failed, napi unwrap msg port failed");
        return nullptr;
    }
    ErrCode ret = msgPort->PostPortMessage(webMsg);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));

    return result;
}

napi_value NapiWebMessagePort::PostMessageEventExt(napi_env env, napi_callback_info info)
{
    WVLOG_D("message PostMessageEventExt start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_object) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    WebMessageExt *webMessageExt = nullptr;
    NAPI_CALL(env, napi_unwrap(env, argv[INTEGER_ZERO], (void **)&webMessageExt));
    if (webMessageExt == nullptr) {
        WVLOG_E("post message failed, napi unwrap msg port failed");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NOT_NULL, "message"));
        return nullptr;
    }

    WebMessagePort *msgPort = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&msgPort));
    if (msgPort == nullptr) {
        WVLOG_E("post message failed, napi unwrap msg port failed");
        return nullptr;
    }

    if (!msgPort->IsExtentionType()) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
             NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return result;
    }

    ErrCode ret = msgPort->PostPortMessage(webMessageExt->GetData());
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));

    return result;
}


napi_value NapiWebMessagePort::OnMessageEventExt(napi_env env, napi_callback_info info)
{
    WVLOG_D("message port set OnMessageEventExt callback");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_function) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback","function"));
        return result;
    }

    napi_ref onMsgEventFunc = nullptr;
    NAPI_CALL(env, napi_create_reference(env, argv[INTEGER_ZERO], INTEGER_ONE, &onMsgEventFunc));

    auto callbackImpl = std::make_shared<NWebValueCallbackImpl>(env, onMsgEventFunc, true);

    WebMessagePort *msgPort = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&msgPort));
    if (msgPort == nullptr) {
        WVLOG_E("set message event callback failed, napi unwrap msg port failed");
        return nullptr;
    }
    ErrCode ret = msgPort->SetPortMessageCallback(callbackImpl);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

bool UvWebMsgOnReceiveCbDataHandler(NapiWebMessagePort::WebMsgPortParam *data, napi_value& result)
{
    if (data->extention_) {
        napi_value webMsgExt = nullptr;
        napi_status status = napi_get_reference_value(data->env_, g_webMsgExtClassRef, &webMsgExt);
        if (status != napi_status::napi_ok) {
            WVLOG_E("napi_get_reference_value failed.");
            return false;
        }
        status = napi_new_instance(data->env_, webMsgExt, 0, NULL, &result);
        if (status != napi_status::napi_ok) {
            WVLOG_E("napi_new_instance failed.");
            return false;
        }

        WebMessageExt *webMessageExt = new (std::nothrow) WebMessageExt(data->msg_);
        if (webMessageExt == nullptr) {
            WVLOG_E("new WebMessageExt failed.");
            return false;
        }

        status = napi_wrap(data->env_, result, webMessageExt,
            [](napi_env env, void *data, void *hint) {
                WebMessageExt *webMessageExt = static_cast<WebMessageExt *>(data);
                delete webMessageExt;
            },
            nullptr, nullptr);
        if (status != napi_status::napi_ok) {
            WVLOG_E("napi_wrap failed.");
            return false;
        }
    } else {
        NapiParseUtils::ConvertNWebToNapiValue(data->env_, data->msg_, result);
    }
    return true;
}

void NWebValueCallbackImpl::UvWebMessageOnReceiveValueCallback(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("uv work is null");
        return;
    }
    NapiWebMessagePort::WebMsgPortParam *data = reinterpret_cast<NapiWebMessagePort::WebMsgPortParam*>(work->data);
    if (data == nullptr) {
        WVLOG_E("WebMsgPortParam is null");
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(data->env_, &scope);
    if (scope == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    napi_value result[INTEGER_ONE] = {0};
    if (!UvWebMsgOnReceiveCbDataHandler(data, result[INTEGER_ZERO])) {
        delete work;
        work = nullptr;
        napi_close_handle_scope(data->env_, scope);
        return;
    }

    napi_value onMsgEventFunc = nullptr;
    napi_get_reference_value(data->env_, data->callback_, &onMsgEventFunc);
    napi_value placeHodler = nullptr;
    napi_call_function(data->env_, nullptr, onMsgEventFunc, INTEGER_ONE, &result[INTEGER_ZERO], &placeHodler);

    std::unique_lock<std::mutex> lock(data->mutex_);
    data->ready_ = true;
    data->condition_.notify_all();
    napi_close_handle_scope(data->env_, scope);
}

void NWebValueCallbackImpl::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("message port received msg");
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return;
    }
    NapiWebMessagePort::WebMsgPortParam *param = new (std::nothrow) NapiWebMessagePort::WebMsgPortParam();
    if (param == nullptr) {
        WVLOG_E("new WebMsgPortParam failed");
        delete work;
        return;
    }
    param->env_ = env_;
    param->callback_ = callback_;
    param->msg_ = result;
    param->extention_ = extention_;
    work->data = reinterpret_cast<void*>(param);
    uv_queue_work_with_qos(
        loop, work, [](uv_work_t* work) {}, UvWebMessageOnReceiveValueCallback, uv_qos_user_initiated);

    {
        std::unique_lock<std::mutex> lock(param->mutex_);
        param->condition_.wait(lock, [&param] { return param->ready_; });
    }
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
}

void UvNWebValueCallbackImplThreadWoker(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("uv work is null");
        return;
    }
    NapiWebMessagePort::WebMsgPortParam *data = reinterpret_cast<NapiWebMessagePort::WebMsgPortParam*>(work->data);
    if (data == nullptr) {
        WVLOG_E("WebMsgPortParam is null");
        delete work;
        return;
    }

    napi_delete_reference(data->env_, data->callback_);
    delete data;
    data = nullptr;
    delete work;
    work = nullptr;
}

NWebValueCallbackImpl::~NWebValueCallbackImpl()
{
    WVLOG_D("~NWebValueCallbackImpl");
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return;
    }
    NapiWebMessagePort::WebMsgPortParam *param = new (std::nothrow) NapiWebMessagePort::WebMsgPortParam();
    if (param == nullptr) {
        WVLOG_E("new WebMsgPortParam failed");
        delete work;
        return;
    }
    param->env_ = env_;
    param->callback_ = callback_;
    work->data = reinterpret_cast<void*>(param);
    int ret = uv_queue_work_with_qos(
        loop, work, [](uv_work_t *work) {}, UvNWebValueCallbackImplThreadWoker, uv_qos_user_initiated);
    if (ret != 0) {
        if (param != nullptr) {
            delete param;
            param = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

napi_value NapiWebMessagePort::OnMessageEvent(napi_env env, napi_callback_info info)
{
    WVLOG_D("message port set OnMessageEvent callback");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_function) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback","function"));
        return result;
    }

    napi_ref onMsgEventFunc = nullptr;
    NAPI_CALL(env, napi_create_reference(env, argv[INTEGER_ZERO], INTEGER_ONE, &onMsgEventFunc));

    auto callbackImpl = std::make_shared<NWebValueCallbackImpl>(env, onMsgEventFunc, false);

    WebMessagePort *msgPort = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&msgPort));
    if (msgPort == nullptr) {
        WVLOG_E("set message event callback failed, napi unwrap msg port failed");
        return nullptr;
    }
    ErrCode ret = msgPort->SetPortMessageCallback(callbackImpl);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::ZoomIn(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    ErrCode ret = webviewController->ZoomIn();
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("ZoomIn failed.");
            return nullptr;
        }
        BusinessError::ThrowErrorByErrcode(env, ret);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::ZoomOut(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    ErrCode ret = webviewController->ZoomOut();
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("ZoomOut failed.");
            return nullptr;
        }
        BusinessError::ThrowErrorByErrcode(env, ret);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::GetWebId(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    int32_t webId = webviewController->GetWebId();
    napi_create_int32(env, webId, &result);

    return result;
}

napi_value NapiWebviewController::GetUserAgent(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    std::string userAgent = "";
    userAgent = webviewController->GetUserAgent();
    napi_create_string_utf8(env, userAgent.c_str(), userAgent.length(), &result);

    return result;
}

napi_value NapiWebviewController::GetCustomUserAgent(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    napi_value result = nullptr;
    std::string userAgent = webviewController->GetCustomUserAgent();
    napi_create_string_utf8(env, userAgent.c_str(), userAgent.length(), &result);
    return result;
}

napi_value NapiWebviewController::SetCustomUserAgent(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_undefined(env, &result));

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    std::string userAgent;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], userAgent)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "userAgent", "string"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    ErrCode ret = webviewController->SetCustomUserAgent(userAgent);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
    }
    return result;
}

napi_value NapiWebviewController::GetTitle(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    std::string title = "";
    title = webviewController->GetTitle();
    napi_create_string_utf8(env, title.c_str(), title.length(), &result);

    return result;
}

napi_value NapiWebviewController::GetPageHeight(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    int32_t pageHeight = webviewController->GetPageHeight();
    napi_create_int32(env, pageHeight, &result);

    return result;
}

napi_value NapiWebviewController::BackOrForward(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    if (argc != INTEGER_ONE) {
        WVLOG_E("Requires 1 parameters.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }

    int32_t step = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], step)) {
        WVLOG_E("Parameter is not integer number type.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "step", "number"));
        return nullptr;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    ErrCode ret = webviewController->BackOrForward(step);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::StoreWebArchive(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_TWO;
    size_t argcCallback = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (argc != argcPromise && argc != argcCallback) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return result;
    }
    std::string baseName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], baseName)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return result;
    }

    if (baseName.empty()) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return result;
    }

    bool autoName = false;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_ONE], autoName)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return result;
    }

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
            return result;
        }
    }
    return StoreWebArchiveInternal(env, info, baseName, autoName);
}

napi_value NapiWebviewController::StoreWebArchiveInternal(napi_env env, napi_callback_info info,
    const std::string &baseName, bool autoName)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_TWO;
    size_t argcCallback = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = {0};

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);

    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    if (argc == argcCallback) {
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], 1, &jsCallback);

        if (jsCallback) {
            webviewController->StoreWebArchiveCallback(baseName, autoName, env, std::move(jsCallback));
        }
        return result;
    } else if (argc == argcPromise) {
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        napi_create_promise(env, &deferred, &promise);
        if (promise && deferred) {
            webviewController->StoreWebArchivePromise(baseName, autoName, env, deferred);
        }
        return promise;
    }
    return result;
}

napi_value NapiWebviewController::GetHitTestValue(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    std::shared_ptr<HitTestResult> nwebResult = webviewController->GetHitTestValue();

    napi_create_object(env, &result);

    napi_value type;
    if (nwebResult) {
        napi_create_uint32(env, nwebResult->GetType(), &type);
    } else {
        napi_create_uint32(env, HitTestResult::UNKNOWN_TYPE, &type);
    }
    napi_set_named_property(env, result, "type", type);

    napi_value extra;
    if (nwebResult) {
        napi_create_string_utf8(env, nwebResult->GetExtra().c_str(), NAPI_AUTO_LENGTH, &extra);
    } else {
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &extra);
    }
    napi_set_named_property(env, result, "extra", extra);

    return result;
}

napi_value NapiWebviewController::RequestFocus(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    webviewController->RequestFocus();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::PostUrl(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebMessageExt::PostUrl start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "two"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::string url;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], url)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return result;
    }

    bool isArrayBuffer = false;
    NAPI_CALL(env, napi_is_arraybuffer(env, argv[INTEGER_ONE], &isArrayBuffer));
    if (!isArrayBuffer) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "postData", "array"));
        return result;
    }

    char *arrBuf = nullptr;
    size_t byteLength = 0;
    napi_get_arraybuffer_info(env, argv[INTEGER_ONE], (void **)&arrBuf, &byteLength);

    std::vector<char> postData(arrBuf, arrBuf + byteLength);
    ErrCode ret = webviewController->PostUrl(url, postData);
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("PostData failed");
            return result;
        }
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::LoadUrl(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO];
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if ((argc != INTEGER_ONE) && (argc != INTEGER_TWO)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "one", "two"));
        return nullptr;
    }
    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    napi_valuetype webSrcType;
    napi_typeof(env, argv[INTEGER_ZERO], &webSrcType);
    if (webSrcType != napi_string && webSrcType != napi_object) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "url"));
        return nullptr;
    }
    std::string webSrc;
    if (!webviewController->ParseUrl(env, argv[INTEGER_ZERO], webSrc)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
        return nullptr;
    }
    if (argc == INTEGER_ONE) {
        ErrCode ret = webviewController->LoadUrl(webSrc);
        if (ret != NO_ERROR) {
            if (ret == NWEB_ERROR) {
                return nullptr;
            }
            BusinessError::ThrowErrorByErrcode(env, ret);
            return nullptr;
        }
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }
    return LoadUrlWithHttpHeaders(env, info, webSrc, argv, webviewController);
}

napi_value NapiWebviewController::LoadUrlWithHttpHeaders(napi_env env, napi_callback_info info, const std::string& url,
    const napi_value* argv, WebviewController* webviewController)
{
    napi_value result = nullptr;
    std::map<std::string, std::string> httpHeaders;
    napi_value array = argv[INTEGER_ONE];
    bool isArray = false;
    napi_is_array(env, array, &isArray);
    if (isArray) {
        uint32_t arrayLength = INTEGER_ZERO;
        napi_get_array_length(env, array, &arrayLength);
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string key;
            std::string value;
            napi_value obj = nullptr;
            napi_value keyObj = nullptr;
            napi_value valueObj = nullptr;
            napi_get_element(env, array, i, &obj);
            if (napi_get_named_property(env, obj, "headerKey", &keyObj) != napi_ok) {
                continue;
            }
            if (napi_get_named_property(env, obj, "headerValue", &valueObj) != napi_ok) {
                continue;
            }
            NapiParseUtils::ParseString(env, keyObj, key);
            NapiParseUtils::ParseString(env, valueObj, value);
            httpHeaders[key] = value;
        }
    } else {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    ErrCode ret = webviewController->LoadUrl(url, httpHeaders);
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("LoadUrl failed.");
            return nullptr;
        }
        BusinessError::ThrowErrorByErrcode(env, ret);
        return nullptr;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::LoadData(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_FIVE;
    napi_value argv[INTEGER_FIVE];
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if ((argc != INTEGER_THREE) && (argc != INTEGER_FOUR) &&
        (argc != INTEGER_FIVE)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, 
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "three", "four"));
        return nullptr;
    }
    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    std::string data;
    std::string mimeType;
    std::string encoding;
    std::string baseUrl;
    std::string historyUrl;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], data) ||
        !NapiParseUtils::ParseString(env, argv[INTEGER_ONE], mimeType) ||
        !NapiParseUtils::ParseString(env, argv[INTEGER_TWO], encoding)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, ParamCheckErrorMsgTemplate::TYPE_ALL_STRING);
        return nullptr;
    }
    if ((argc >= INTEGER_FOUR) && !NapiParseUtils::ParseString(env, argv[INTEGER_THREE], baseUrl)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, ParamCheckErrorMsgTemplate::TYPE_ALL_STRING);
        return nullptr;
    }
    if ((argc == INTEGER_FIVE) && !NapiParseUtils::ParseString(env, argv[INTEGER_FOUR], historyUrl)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR, ParamCheckErrorMsgTemplate::TYPE_ALL_STRING);
        return nullptr;
    }
    ErrCode ret = webviewController->LoadData(data, mimeType, encoding, baseUrl, historyUrl);
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("LoadData failed.");
            return nullptr;
        }
        BusinessError::ThrowErrorByErrcode(env, ret);
        return nullptr;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::GetHitTest(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    int32_t type = webviewController->GetHitTest();
    napi_create_int32(env, type, &result);
    return result;
}

napi_value NapiWebviewController::ClearMatches(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->ClearMatches();
    return result;
}

napi_value NapiWebviewController::SearchNext(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    bool forward;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], forward)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "forward", "boolean"));
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->SearchNext(forward);
    return result;
}

napi_value NapiWebviewController::SearchAllAsync(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    std::string searchString;
    if (!NapiParseUtils::ParseString(env, argv[0], searchString)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "searchString", "number"));
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->SearchAllAsync(searchString);
    return result;
}

napi_value NapiWebviewController::ClearSslCache(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->ClearSslCache();
    return result;
}

napi_value NapiWebviewController::ClearClientAuthenticationCache(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->ClearClientAuthenticationCache();

    return result;
}

napi_value NapiWebviewController::Stop(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->Stop();

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::Zoom(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    float factor = 0.0;
    if (!NapiParseUtils::ParseFloat(env, argv[0], factor)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "factor", "number"));
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    ErrCode ret = controller->Zoom(factor);
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("Zoom failed.");
            return result;
        }
        BusinessError::ThrowErrorByErrcode(env, ret);
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::InnerCompleteWindowNew(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    int32_t parentNwebId = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[0], parentNwebId) || parentNwebId == -1) {
        WVLOG_E("Parse parent nweb id failed.");
        return nullptr;
    }
    WebviewController* webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&webviewController);
    if ((!webviewController) || (status != napi_ok)) {
        WVLOG_E("webviewController is nullptr.");
        return nullptr;
    }
    webviewController->InnerCompleteWindowNew(parentNwebId);
    return thisVar;
}

napi_value NapiWebviewController::RegisterJavaScriptProxy(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebviewController::RegisterJavaScriptProxy");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_FOUR;
    napi_value argv[INTEGER_FOUR] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE && argc != INTEGER_FOUR) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "three", "four"));
        return result;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_object) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "object", "object"));
        return result;
    }

    std::string objName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], objName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "name", "string"));
        return result;
    }

    std::vector<std::string> methodList;
    if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_TWO], methodList)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "methodList", "array"));
        return result;
    }

    std::vector<std::string> asyncMethodList;
    if (argc == INTEGER_FOUR) {
        if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_THREE], asyncMethodList)) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return result;
        }
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->SetNWebJavaScriptResultCallBack();
    controller->RegisterJavaScriptProxy(env, argv[INTEGER_ZERO], objName, methodList, asyncMethodList);
    return result;
}

napi_value NapiWebviewController::DeleteJavaScriptRegister(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    std::string objName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], objName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "name", "string"));
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    ErrCode ret = controller->DeleteJavaScriptRegister(objName, {});
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::RunJavaScript(napi_env env, napi_callback_info info)
{
    return RunJS(env, info, false);
}

napi_value NapiWebviewController::RunJavaScriptExt(napi_env env, napi_callback_info info)
{
    return RunJS(env, info, true);
}

napi_value NapiWebviewController::RunJS(napi_env env, napi_callback_info info, bool extention)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ONE;
    size_t argcCallback = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (argc != argcPromise && argc != argcCallback) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "two"));
        return result;
    }

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback","function"));
            return result;
        }
    }

    if (maxFdNum_ == -1) {
        maxFdNum_ =
            std::atoi(NWebAdapterHelper::Instance().ParsePerfConfig("flowBufferConfig", "maxFdNumber").c_str());
    }

    if (usedFd_.load() < maxFdNum_) {
        return RunJavaScriptInternalExt(env, info, extention);
    }

    std::string script;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    bool parseResult = (valueType == napi_string) ? NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], script) :
        NapiParseUtils::ParseArrayBuffer(env, argv[INTEGER_ZERO], script);
    if (!parseResult) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "script", "string"));
        return result;
    }
    return RunJavaScriptInternal(env, info, script, extention);
}

napi_value NapiWebviewController::RunJavaScriptInternal(napi_env env, napi_callback_info info,
    const std::string &script, bool extention)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ONE;
    size_t argcCallback = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = {0};

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);

    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    if (argc == argcCallback) {
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], 1, &jsCallback);

        if (jsCallback) {
            webviewController->RunJavaScriptCallback(script, env, std::move(jsCallback), extention);
        }
        return result;
    } else if (argc == argcPromise) {
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        napi_create_promise(env, &deferred, &promise);
        if (promise && deferred) {
            webviewController->RunJavaScriptPromise(script, env, deferred, extention);
        }
        return promise;
    }
    return result;
}

ErrCode NapiWebviewController::ConstructFlowbuf(napi_env env, napi_value argv, int& fd, size_t& scriptLength)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        return NWebError::NEW_OOM;
    }
    flowbufferAdapter->StartPerformanceBoost();

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);

    ErrCode constructResult = (valueType == napi_string) ?
        NapiParseUtils::ConstructStringFlowbuf(env, argv, fd, scriptLength) :
        NapiParseUtils::ConstructArrayBufFlowbuf(env, argv, fd, scriptLength);
    return constructResult;
}

napi_value NapiWebviewController::RunJSBackToOriginal(napi_env env, napi_callback_info info,
    bool extention, napi_value argv, napi_value result)
{
    std::string script;
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    bool parseResult = (valueType == napi_string) ? NapiParseUtils::ParseString(env, argv, script) :
        NapiParseUtils::ParseArrayBuffer(env, argv, script);
    if (!parseResult) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    return RunJavaScriptInternal(env, info, script, extention);
}

napi_value NapiWebviewController::RunJavaScriptInternalExt(napi_env env, napi_callback_info info, bool extention)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ONE;
    size_t argcCallback = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = {0};

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    int fd;
    size_t scriptLength;
    ErrCode constructResult = ConstructFlowbuf(env, argv[INTEGER_ZERO], fd, scriptLength);
    if (constructResult != NO_ERROR) {
        return RunJSBackToOriginal(env, info, extention, argv[INTEGER_ZERO], result);
    }
    usedFd_++;

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);

    if (!webviewController || !webviewController->IsInit()) {
        close(fd);
        usedFd_--;
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    if (argc == argcCallback) {
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallback - 1], 1, &jsCallback);

        if (jsCallback) {
            // RunJavaScriptCallbackExt will close fd after IPC
            webviewController->RunJavaScriptCallbackExt(fd, scriptLength, env, std::move(jsCallback), extention);
        }
        usedFd_--;
        return result;
    } else if (argc == argcPromise) {
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        napi_create_promise(env, &deferred, &promise);
        if (promise && deferred) {
            // RunJavaScriptCallbackExt will close fd after IPC
            webviewController->RunJavaScriptPromiseExt(fd, scriptLength, env, deferred, extention);
        }
        usedFd_--;
        return promise;
    }
    close(fd);
    usedFd_--;
    return result;
}

napi_value NapiWebviewController::GetUrl(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    std::string url = "";
    url = webviewController->GetUrl();
    napi_create_string_utf8(env, url.c_str(), url.length(), &result);

    return result;
}

napi_value NapiWebviewController::GetOriginalUrl(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    std::string url = "";
    url = webviewController->GetOriginalUrl();
    napi_create_string_utf8(env, url.c_str(), url.length(), &result);
    return result;
}

napi_value NapiWebviewController::TerminateRenderProcess(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }
    bool ret = false;
    ret = webviewController->TerminateRenderProcess();
    NAPI_CALL(env, napi_get_boolean(env, ret, &result));
    return result;
}

napi_value NapiWebviewController::SetNetworkAvailable(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    bool enable;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], enable)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "enable", "booleane"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->PutNetworkAvailable(enable);
    return result;
}

napi_value NapiWebviewController::InnerGetWebId(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    int32_t webId = -1;
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        WVLOG_E("Init error. The WebviewController must be associated with a Web component.");
        napi_create_int32(env, webId, &result);
        return result;
    }

    webId = webviewController->GetWebId();
    napi_create_int32(env, webId, &result);

    return result;
}

napi_value NapiWebviewController::HasImage(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromiseParaNum = INTEGER_ZERO;
    size_t argcCallbackParaNum = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (argc != argcPromiseParaNum && argc != argcCallbackParaNum) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "zero", "one"));
        return result;
    }

    if (argc == argcCallbackParaNum) {
        napi_valuetype valueType = napi_null;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_typeof(env, argv[argcCallbackParaNum - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
            return result;
        }
    }
    return HasImageInternal(env, info);
}

napi_value NapiWebviewController::HasImageInternal(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromiseParaNum = INTEGER_ZERO;
    size_t argcCallbackParaNum = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);

    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    if (argc == argcCallbackParaNum) {
        napi_ref jsCallback = nullptr;
        napi_create_reference(env, argv[argcCallbackParaNum - 1], 1, &jsCallback);

        if (jsCallback) {
            ErrCode ret = webviewController->HasImagesCallback(env, std::move(jsCallback));
            if (ret == NWEB_ERROR) {
                return nullptr;
            } else if (ret != NO_ERROR) {
                BusinessError::ThrowErrorByErrcode(env, ret);
                return nullptr;
            }
        }
        return result;
    } else if (argc == argcPromiseParaNum) {
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        napi_create_promise(env, &deferred, &promise);
        if (promise && deferred) {
            ErrCode ret = webviewController->HasImagesPromise(env, deferred);
            if (ret == NWEB_ERROR) {
                return nullptr;
            } else if (ret != NO_ERROR) {
                BusinessError::ThrowErrorByErrcode(env, ret);
                return nullptr;
            }
        }
        return promise;
    }
    return result;
}

napi_value NapiWebviewController::RemoveCache(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    bool includeDiskFiles;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], includeDiskFiles)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "clearRom", "boolean"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->RemoveCache(includeDiskFiles);
    return result;
}

napi_value NapiWebviewController::IsIncognitoMode(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    bool incognitoMode = false;
    incognitoMode = webviewController->IsIncognitoMode();
    NAPI_CALL(env, napi_get_boolean(env, incognitoMode, &result));
    return result;
}

napi_value NapiWebHistoryList::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

Media::PixelFormat getColorType(ImageColorType colorType)
{
    Media::PixelFormat pixelFormat_;
    switch (colorType) {
        case ImageColorType::COLOR_TYPE_UNKNOWN:
            pixelFormat_ = Media::PixelFormat::UNKNOWN;
            break;
        case ImageColorType::COLOR_TYPE_RGBA_8888:
            pixelFormat_ = Media::PixelFormat::RGBA_8888;
            break;
        case ImageColorType::COLOR_TYPE_BGRA_8888:
            pixelFormat_ = Media::PixelFormat::BGRA_8888;
            break;
        default:
            pixelFormat_ = Media::PixelFormat::UNKNOWN;
            break;
    }
    return pixelFormat_;
}

Media::AlphaType getAlphaType(ImageAlphaType alphaType)
{
    Media::AlphaType alphaType_;
    switch (alphaType) {
        case ImageAlphaType::ALPHA_TYPE_UNKNOWN:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
            break;
        case ImageAlphaType::ALPHA_TYPE_OPAQUE:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
            break;
        case ImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
            break;
        case ImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
            break;
        default:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
            break;
    }
    return alphaType_;
}

napi_value NapiWebHistoryList::GetFavicon(napi_env env, std::shared_ptr<NWebHistoryItem> item)
{
    napi_value result = nullptr;
    void *data = nullptr;
    int32_t width = 0;
    int32_t height = 0;
    ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
    ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
    bool isGetFavicon = item->GetFavicon(&data, width, height, colorType, alphaType);
    napi_get_null(env, &result);

    if (!isGetFavicon) {
        return result;
    }

    Media::InitializationOptions opt;
    opt.size.width = width;
    opt.size.height = height;
    opt.pixelFormat = getColorType(colorType);
    opt.alphaType = getAlphaType(alphaType);
    opt.editable = true;
    auto pixelMap = Media::PixelMap::Create(opt);
    if (pixelMap == nullptr) {
        return result;
    }
    uint64_t stride = static_cast<uint64_t>(width) << 2;
    uint64_t bufferSize = stride * static_cast<uint64_t>(height);
    pixelMap->WritePixels(static_cast<const uint8_t *>(data), bufferSize);
    std::shared_ptr<Media::PixelMap> pixelMapToJs(pixelMap.release());
    napi_value jsPixelMap = OHOS::Media::PixelMapNapi::CreatePixelMap(env, pixelMapToJs);
    return jsPixelMap;
}

napi_value NapiWebHistoryList::GetItem(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    int32_t index;
    WebHistoryList *historyList = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&historyList));
    if (historyList == nullptr) {
        WVLOG_E("unwrap historyList failed.");
        return result;
    }
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    if (!NapiParseUtils::ParseInt32(env, argv[0], index)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NOT_NULL_TWO, "index", "int"));
        return result;
    }
    if (index >= historyList->GetListSize() || index < 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The value of index must be greater than or equal to 0");
        return result;
    }

    std::shared_ptr<NWebHistoryItem> item = historyList->GetItem(index);
    if (!item) {
        return result;
    }

    napi_create_object(env, &result);
    std::string historyUrl = item->GetHistoryUrl();
    std::string historyRawUrl = item->GetHistoryRawUrl();
    std::string title = item->GetHistoryTitle();

    napi_value js_historyUrl;
    napi_create_string_utf8(env, historyUrl.c_str(), historyUrl.length(), &js_historyUrl);
    napi_set_named_property(env, result, "historyUrl", js_historyUrl);

    napi_value js_historyRawUrl;
    napi_create_string_utf8(env, historyRawUrl.c_str(), historyRawUrl.length(), &js_historyRawUrl);
    napi_set_named_property(env, result, "historyRawUrl", js_historyRawUrl);

    napi_value js_title;
    napi_create_string_utf8(env, title.c_str(), title.length(), &js_title);
    napi_set_named_property(env, result, "title", js_title);

    napi_value js_icon = GetFavicon(env, item);
    napi_set_named_property(env, result, "icon", js_icon);
    return result;
}

napi_value NapiWebviewController::getBackForwardEntries(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    WebviewController *webviewController = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::shared_ptr<NWebHistoryList> list = webviewController->GetHistoryList();
    if (!list) {
        return result;
    }

    WebHistoryList *webHistoryList = new (std::nothrow) WebHistoryList(list);
    if (webHistoryList == nullptr) {
        return result;
    }

    int32_t currentIndex = list->GetCurrentIndex();
    int32_t size = list->GetListSize();

    napi_value historyList = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, g_historyListRef, &historyList));
    NAPI_CALL(env, napi_new_instance(env, historyList, 0, NULL, &result));

    napi_value js_currentIndex;
    napi_create_int32(env, currentIndex, &js_currentIndex);
    napi_set_named_property(env, result, "currentIndex", js_currentIndex);

    napi_value js_size;
    napi_create_int32(env, size, &js_size);
    napi_set_named_property(env, result, "size", js_size);
    NAPI_CALL(env, napi_wrap(env, result, webHistoryList,
        [](napi_env env, void *data, void *hint) {
            WebHistoryList *webHistoryList = static_cast<WebHistoryList *>(data);
            delete webHistoryList;
        },
        nullptr, nullptr));

    return result;
}

napi_value NapiWebviewController::GetFavicon(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_null(env, &result);
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);

    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    const void *data = nullptr;
    size_t width = 0;
    size_t height = 0;
    ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
    ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
    bool isGetFavicon = webviewController->GetFavicon(&data, width, height, colorType, alphaType);
    if (!isGetFavicon) {
        return result;
    }

    Media::InitializationOptions opt;
    opt.size.width = static_cast<int32_t>(width);
    opt.size.height = static_cast<int32_t>(height);
    opt.pixelFormat = getColorType(colorType);
    opt.alphaType = getAlphaType(alphaType);
    opt.editable = true;
    auto pixelMap = Media::PixelMap::Create(opt);
    if (pixelMap == nullptr) {
        return result;
    }
    uint64_t stride = static_cast<uint64_t>(width) << 2;
    uint64_t bufferSize = stride * static_cast<uint64_t>(height);
    pixelMap->WritePixels(static_cast<const uint8_t *>(data), bufferSize);
    std::shared_ptr<Media::PixelMap> pixelMapToJs(pixelMap.release());
    napi_value jsPixelMap = OHOS::Media::PixelMapNapi::CreatePixelMap(env, pixelMapToJs);
    return jsPixelMap;
}

napi_value NapiWebviewController::SerializeWebState(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_get_null(env, &result);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);
    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    void *data = nullptr;
    napi_value buffer = nullptr;
    auto webState = webviewController->SerializeWebState();

    NAPI_CALL(env, napi_create_arraybuffer(env, webState.size(), &data, &buffer));
    int retCode = memcpy_s(data, webState.size(), webState.data(), webState.size());
    if (retCode != 0) {
        return result;
    }
    NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, webState.size(), buffer, 0, &result));
    return result;
}

napi_value NapiWebviewController::RestoreWebState(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_get_null(env, &result);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);
    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    bool isTypedArray = false;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }
    NAPI_CALL(env, napi_is_typedarray(env, argv[0], &isTypedArray));
    if (!isTypedArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "state", "uint8Array"));
        return result;
    }

    napi_typedarray_type type;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    NAPI_CALL(env, napi_get_typedarray_info(env, argv[0], &type, &length, nullptr, &buffer, &offset));
    if (type != napi_uint8_array) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "state", "uint8Array"));
        return result;
    }
    uint8_t *data = nullptr;
    size_t total = 0;
    NAPI_CALL(env, napi_get_arraybuffer_info(env, buffer, reinterpret_cast<void **>(&data), &total));
    length = std::min<size_t>(length, total - offset);
    std::vector<uint8_t> state(length);
    int retCode = memcpy_s(state.data(), state.size(), &data[offset], length);
    if (retCode != 0) {
        return result;
    }
    webviewController->RestoreWebState(state);
    return result;
}

napi_value NapiWebviewController::ScrollPageDown(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    bool bottom;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], bottom)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "bottom", "booleane"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->ScrollPageDown(bottom);
    return result;
}

napi_value NapiWebviewController::ScrollPageUp(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    bool top;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], top)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "top", "booleane"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->ScrollPageUp(top);
    return result;
}

bool CheckSchemeName(const std::string& schemeName)
{
    if (schemeName.empty() || schemeName.size() > MAX_CUSTOM_SCHEME_NAME_LENGTH) {
        WVLOG_E("Invalid scheme name length");
        return false;
    }
    for (auto it = schemeName.begin(); it != schemeName.end(); it++) {
        char chr = *it;
        if (!((chr >= 'a' && chr <= 'z') || (chr >= '0' && chr <= '9') ||
            (chr == '.') || (chr == '+') || (chr == '-'))) {
            WVLOG_E("invalid character %{public}c", chr);
            return false;
        }
    }
    return true;
}

void SetCustomizeSchemeOption(Scheme& scheme)
{
    std::map<int, bool Scheme::*> schemeProperties = {
        {0, &Scheme::isStandard},
        {1, &Scheme::isLocal},
        {2, &Scheme::isDisplayIsolated},
        {3, &Scheme::isSecure},
        {4, &Scheme::isSupportCORS},
        {5, &Scheme::isCspBypassing},
        {6, &Scheme::isSupportFetch},
        {7, &Scheme::isCodeCacheSupported}
    };

    for (const auto& property : schemeProperties) {
        if (scheme.*(property.second)) {
            scheme.option += 1 << property.first;
        }
    }
}

bool SetCustomizeScheme(napi_env env, napi_value obj, Scheme& scheme)
{
    std::map<std::string, bool Scheme::*> schemeBooleanProperties = {
        {"isSupportCORS", &Scheme::isSupportCORS},
        {"isSupportFetch", &Scheme::isSupportFetch},
        {"isStandard", &Scheme::isStandard},
        {"isLocal", &Scheme::isLocal},
        {"isDisplayIsolated", &Scheme::isDisplayIsolated},
        {"isSecure", &Scheme::isSecure},
        {"isCspBypassing", &Scheme::isCspBypassing},
        {"isCodeCacheSupported", &Scheme::isCodeCacheSupported}
    };

    for (const auto& property : schemeBooleanProperties) {
        napi_value propertyObj = nullptr;
        std::string property_name = property.first.c_str();
        napi_get_named_property(env, obj, property.first.c_str(), &propertyObj);
        if (!NapiParseUtils::ParseBoolean(env, propertyObj, scheme.*(property.second))) {
            if (property_name == "isSupportCORS" || property_name == "isSupportFetch") {
                return false;
            }
        }
    }

    napi_value schemeNameObj = nullptr;
    if (napi_get_named_property(env, obj, "schemeName", &schemeNameObj) != napi_ok) {
        return false;
    }
    if (!NapiParseUtils::ParseString(env, schemeNameObj, scheme.name)) {
        return false;
    }

    if (!CheckSchemeName(scheme.name)) {
        return false;
    }

    SetCustomizeSchemeOption(scheme);
    return true;
}

int32_t CustomizeSchemesArrayDataHandler(napi_env env, napi_value array)
{
    uint32_t arrayLength = 0;
    napi_get_array_length(env, array, &arrayLength);
    if (arrayLength > MAX_CUSTOM_SCHEME_SIZE) {
        return PARAM_CHECK_ERROR;
    }
    std::vector<Scheme> schemeVector;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_value obj = nullptr;
        napi_get_element(env, array, i, &obj);
        Scheme scheme;
        bool result = SetCustomizeScheme(env, obj, scheme);
        if (!result) {
            return PARAM_CHECK_ERROR;
        }
        schemeVector.push_back(scheme);
    }
    int32_t registerResult;
    for (auto it = schemeVector.begin(); it != schemeVector.end(); ++it) {
        registerResult = OH_ArkWeb_RegisterCustomSchemes(it->name.c_str(), it->option);
        if (registerResult != NO_ERROR) {
            return registerResult;
        }
    }
    return NO_ERROR;
}

napi_value NapiWebviewController::CustomizeSchemes(napi_env env, napi_callback_info info)
{
    if (WebviewController::existNweb_) {
        WVLOG_E("There exist web component which has been already created.");
    }

    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return nullptr;
    }
    napi_value array = argv[INTEGER_ZERO];
    bool isArray = false;
    napi_is_array(env, array, &isArray);
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "schemes", "array"));
        return nullptr;
    }
    int32_t registerResult = CustomizeSchemesArrayDataHandler(env, array);
    if (registerResult == NO_ERROR) {
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }
    if (registerResult == PARAM_CHECK_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "schemeName", "string"));
        return nullptr;
    }
    BusinessError::ThrowErrorByErrcode(env, REGISTER_CUSTOM_SCHEME_FAILED);
    return nullptr;
}

napi_value NapiWebviewController::ScrollTo(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    float x;
    float y;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "two"));
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ZERO], x)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "x", "number"));
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ONE], y)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "y", "number"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->ScrollTo(x, y);
    return result;
}

napi_value NapiWebviewController::ScrollBy(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    float deltaX;
    float deltaY;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "two"));
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ZERO], deltaX)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "deltaX", "number"));
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ONE], deltaY)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
             NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "deltaY", "number"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->ScrollBy(deltaX, deltaY);
    return result;
}

napi_value NapiWebviewController::SlideScroll(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };
    float vx;
    float vy;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "two"));
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ZERO], vx)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "vx", "number"));
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ONE], vy)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "vy", "number"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->SlideScroll(vx, vy);
    return result;
}

napi_value NapiWebviewController::SetScrollable(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    bool isEnableScroll;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], isEnableScroll)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "enable", "boolean"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->SetScrollable(isEnableScroll);
    return result;
}

napi_value NapiWebviewController::GetScrollable(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    bool isScrollable = webviewController->GetScrollable();
    NAPI_CALL(env, napi_get_boolean(env, isScrollable, &result));
    return result;
}

napi_value NapiWebviewController::InnerGetCertificate(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    napi_create_array(env, &result);

    WebviewController *webviewController = nullptr;
    napi_unwrap(env, thisVar, (void **)&webviewController);
    if (!webviewController || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    std::vector<std::string> certChainDerData;
    bool ans = webviewController->GetCertChainDerData(certChainDerData);
    if (!ans) {
        WVLOG_E("get cert chain data failed");
        return result;
    }

    for (uint8_t i = 0; i < certChainDerData.size(); i++) {
        if (i == UINT8_MAX) {
            WVLOG_E("error, cert chain data array reach max");
            break;
        }
        void *data = nullptr;
        napi_value buffer = nullptr;
        napi_value item = nullptr;
        NAPI_CALL(env, napi_create_arraybuffer(env, certChainDerData[i].size(), &data, &buffer));
        int retCode = memcpy_s(data, certChainDerData[i].size(),
                               certChainDerData[i].data(), certChainDerData[i].size());
        if (retCode != 0) {
            WVLOG_E("memcpy_s cert data failed, index = %{public}u,", i);
            continue;
        }
        NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, certChainDerData[i].size(), buffer, 0, &item));
        NAPI_CALL(env, napi_set_element(env, result, i, item));
    }
    return result;
}

napi_value NapiWebviewController::SetAudioMuted(napi_env env, napi_callback_info info)
{
    WVLOG_D("SetAudioMuted invoked");

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));

    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("SetAudioMuted failed due to wrong param quantity: %{public}zu", argc);
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool muted = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], muted)) {
        WVLOG_E("SetAudioMuted failed due to invalid argument value");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebviewController* webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void**)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        WVLOG_E("SetAudioMuted failed due to no associated Web component");
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    ErrCode ret = webviewController->SetAudioMuted(muted);
    if (ret != NO_ERROR) {
        WVLOG_E("SetAudioMuted failed, error code: %{public}d", ret);
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }

    WVLOG_I("SetAudioMuted: %{public}s", (muted ? "true" : "false"));
    return result;
}

napi_value NapiWebviewController::PrefetchPage(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO];
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((argc != INTEGER_ONE) && (argc != INTEGER_TWO)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    std::string url;
    if (!ParsePrepareUrl(env, argv[INTEGER_ZERO], url)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
        return nullptr;
    }
    std::map<std::string, std::string> additionalHttpHeaders;
    if (argc == INTEGER_ONE) {
        ErrCode ret = webviewController->PrefetchPage(url, additionalHttpHeaders);
        if (ret != NO_ERROR) {
            WVLOG_E("PrefetchPage failed, error code: %{public}d", ret);
            BusinessError::ThrowErrorByErrcode(env, ret);
            return nullptr;
        }
        NAPI_CALL(env, napi_get_undefined(env, &result));
        return result;
    }
    return PrefetchPageWithHttpHeaders(env, info, url, argv, webviewController);
}

napi_value NapiWebviewController::PrefetchPageWithHttpHeaders(napi_env env, napi_callback_info info, std::string& url,
    const napi_value* argv, WebviewController* webviewController)
{
    napi_value result = nullptr;
    std::map<std::string, std::string> additionalHttpHeaders;
    napi_value array = argv[INTEGER_ONE];
    bool isArray = false;
    napi_is_array(env, array, &isArray);
    if (isArray) {
        uint32_t arrayLength = INTEGER_ZERO;
        napi_get_array_length(env, array, &arrayLength);
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string key;
            std::string value;
            napi_value obj = nullptr;
            napi_value keyObj = nullptr;
            napi_value valueObj = nullptr;
            napi_get_element(env, array, i, &obj);
            if (napi_get_named_property(env, obj, "headerKey", &keyObj) != napi_ok) {
                continue;
            }
            if (napi_get_named_property(env, obj, "headerValue", &valueObj) != napi_ok) {
                continue;
            }
            NapiParseUtils::ParseString(env, keyObj, key);
            NapiParseUtils::ParseString(env, valueObj, value);
            additionalHttpHeaders[key] = value;
        }
    } else {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    ErrCode ret = webviewController->PrefetchPage(url, additionalHttpHeaders);
    if (ret != NO_ERROR) {
        WVLOG_E("PrefetchPage failed, error code: %{public}d", ret);
        BusinessError::ThrowErrorByErrcode(env, ret);
        return nullptr;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::GetLastJavascriptProxyCallingFrameUrl(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    std::string lastCallingFrameUrl = webviewController->GetLastJavascriptProxyCallingFrameUrl();
    napi_create_string_utf8(env, lastCallingFrameUrl.c_str(), lastCallingFrameUrl.length(), &result);
    return result;
}

napi_value NapiWebviewController::PrepareForPageLoad(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = { 0 };
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string url;
    if (!ParsePrepareUrl(env, argv[INTEGER_ZERO], url)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
        return nullptr;
    }

    bool preconnectable = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_ONE], preconnectable)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    int32_t numSockets = 0;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_TWO], numSockets)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    if (numSockets <= 0 || static_cast<uint32_t>(numSockets) > SOCKET_MAXIMUM) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_SOCKET_NUMBER);
        return nullptr;
    }

    NWebHelper::Instance().PrepareForPageLoad(url, preconnectable, numSockets);
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::PrefetchResource(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_FOUR;
    napi_value argv[INTEGER_FOUR] = { 0 };
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc > INTEGER_FOUR || argc < INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::shared_ptr<NWebEnginePrefetchArgs> prefetchArgs = ParsePrefetchArgs(env, argv[INTEGER_ZERO]);
    if (prefetchArgs == nullptr) {
        return nullptr;
    }

    std::map<std::string, std::string> additionalHttpHeaders;
    if (argc >= INTEGER_TWO && !ParseHttpHeaders(env, argv[INTEGER_ONE], &additionalHttpHeaders)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::string cacheKey;
    if ((argc >= INTEGER_THREE) && !NapiParseUtils::ParseString(env, argv[INTEGER_TWO], cacheKey)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    if (cacheKey.empty()) {
        cacheKey = prefetchArgs->GetUrl();
    } else {
        for (char c : cacheKey) {
            if (!isalnum(c)) {
                BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
                return nullptr;
            }
        }
    }

    int32_t cacheValidTime = 0;
    if (argc >= INTEGER_FOUR) {
        if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_THREE], cacheValidTime) || cacheValidTime <= 0 ||
            cacheValidTime > INT_MAX) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return nullptr;
        }
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    NWebHelper::Instance().PrefetchResource(prefetchArgs, additionalHttpHeaders, cacheKey, cacheValidTime);
    return result;
}

napi_value NapiWebviewController::ClearPrefetchedResource(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    std::vector<std::string> cacheKeyList;
    if (!ParseCacheKeyList(env, argv[INTEGER_ZERO], &cacheKeyList)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    NAPI_CALL(env, napi_get_undefined(env, &result));
    NWebHelper::Instance().ClearPrefetchedResource(cacheKeyList);
    return result;
}

napi_value NapiWebviewController::SetDownloadDelegate(napi_env env, napi_callback_info info)
{
    WVLOG_E("WebDownloader::JS_SetDownloadDelegate");
    NWebHelper::Instance().LoadNWebSDK();
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    WebDownloadDelegate* delegate = nullptr;
    napi_value obj = argv[0];
    napi_unwrap(env, obj, (void**)&delegate);
    napi_create_reference(env, obj, 1, &delegate->delegate_);

    WebviewController *webviewController = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        WVLOG_E("create message port failed, napi unwrap webviewController failed");
        return nullptr;
    }
    int32_t nwebId = webviewController->GetWebId();
    WebDownloadManager::AddDownloadDelegateForWeb(nwebId, delegate);
    return nullptr;
}

napi_value NapiWebviewController::StartDownload(napi_env env, napi_callback_info info)
{
    WVLOG_I("[DOWNLOAD] NapiWebviewController::StartDownload");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    WebviewController *webviewController = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        WVLOG_E("create message port failed, napi unwrap webviewController failed");
        return nullptr;
    }

    std::string url;
    if (!ParsePrepareUrl(env, argv[INTEGER_ZERO], url)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
        return nullptr;
    }
    int32_t nwebId = webviewController->GetWebId();
    NWebHelper::Instance().LoadNWebSDK();
    WebDownloader_StartDownload(nwebId, url.c_str());
    return nullptr;
}

napi_value NapiWebviewController::CloseAllMediaPresentations(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }

    webviewController->CloseAllMediaPresentations();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::StopAllMedia(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }

    webviewController->StopAllMedia();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::ResumeAllMedia(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }

    webviewController->ResumeAllMedia();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::PauseAllMedia(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }

    webviewController->PauseAllMedia();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::GetMediaPlaybackState(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }

    int32_t mediaPlaybackState = webviewController->GetMediaPlaybackState();
    napi_create_int32(env, mediaPlaybackState, &result);
    return result;
}

napi_value NapiWebviewController::SetConnectionTimeout(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    int32_t timeout = 0;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], timeout) || (timeout <= 0)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    NWebHelper::Instance().SetConnectionTimeout(timeout);
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::CreateWebPrintDocumentAdapter(napi_env env, napi_callback_info info)
{
    WVLOG_I("Create web print document adapter.");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    std::string jobName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], jobName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "jopName", "string"));
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    void* webPrintDocument = webviewController->CreateWebPrintDocumentAdapter(jobName);
    if (!webPrintDocument) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    napi_value webPrintDoc = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, g_webPrintDocClassRef, &webPrintDoc));
    napi_value consParam[INTEGER_ONE] = {0};
    NAPI_CALL(env, napi_create_bigint_uint64(env, reinterpret_cast<uint64_t>(webPrintDocument),
                                             &consParam[INTEGER_ZERO]));
    napi_value proxy = nullptr;
    status = napi_new_instance(env, webPrintDoc, INTEGER_ONE, &consParam[INTEGER_ZERO], &proxy);
    if (status!= napi_ok) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    return proxy;
}

napi_value NapiWebviewController::GetSecurityLevel(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }

    int32_t securityLevel = webviewController->GetSecurityLevel();
    napi_create_int32(env, securityLevel, &result);
    return result;
}

void ParsePrintRangeAdapter(napi_env env, napi_value pageRange, PrintAttributesAdapter& printAttr)
{
    if (!pageRange) {
        WVLOG_E("ParsePrintRangeAdapter failed.");
        return;
    }
    napi_value startPage = nullptr;
    napi_value endPage = nullptr;
    napi_value pages = nullptr;
    napi_get_named_property(env, pageRange, "startPage", &startPage);
    napi_get_named_property(env, pageRange, "endPage", &endPage);
    if (startPage) {
        NapiParseUtils::ParseUint32(env, startPage, printAttr.pageRange.startPage);
    }
    if (endPage) {
        NapiParseUtils::ParseUint32(env, endPage, printAttr.pageRange.endPage);
    }
    napi_get_named_property(env, pageRange, "pages", &pages);
    uint32_t pageArrayLength = 0;
    napi_get_array_length(env, pages, &pageArrayLength);
    for (uint32_t i = 0; i < pageArrayLength; ++i) {
        napi_value pagesNumObj = nullptr;
        napi_get_element(env, pages, i, &pagesNumObj);
        uint32_t pagesNum;
        NapiParseUtils::ParseUint32(env, pagesNumObj, pagesNum);
        printAttr.pageRange.pages.push_back(pagesNum);
    }
}

void ParsePrintPageSizeAdapter(napi_env env, napi_value pageSize, PrintAttributesAdapter& printAttr)
{
    if (!pageSize) {
        WVLOG_E("ParsePrintPageSizeAdapter failed.");
        return;
    }
    napi_value id = nullptr;
    napi_value name = nullptr;
    napi_value width = nullptr;
    napi_value height = nullptr;
    napi_get_named_property(env, pageSize, "id", &id);
    napi_get_named_property(env, pageSize, "name", &name);
    napi_get_named_property(env, pageSize, "width", &width);
    napi_get_named_property(env, pageSize, "height", &height);
    if (width) {
        NapiParseUtils::ParseUint32(env, width, printAttr.pageSize.width);
    }
    if (height) {
        NapiParseUtils::ParseUint32(env, height, printAttr.pageSize.height);
    }
}

void ParsePrintMarginAdapter(napi_env env, napi_value margin, PrintAttributesAdapter& printAttr)
{
    if (!margin) {
        WVLOG_E("ParsePrintMarginAdapter failed.");
        return;
    }
    napi_value top = nullptr;
    napi_value bottom = nullptr;
    napi_value left = nullptr;
    napi_value right = nullptr;
    napi_get_named_property(env, margin, "top", &top);
    napi_get_named_property(env, margin, "bottom", &bottom);
    napi_get_named_property(env, margin, "left", &left);
    napi_get_named_property(env, margin, "right", &right);
    if (top) {
        NapiParseUtils::ParseUint32(env, top, printAttr.margin.top);
    }
    if (bottom) {
        NapiParseUtils::ParseUint32(env, bottom, printAttr.margin.bottom);
    }
    if (left) {
        NapiParseUtils::ParseUint32(env, left, printAttr.margin.left);
    }
    if (right) {
        NapiParseUtils::ParseUint32(env, right, printAttr.margin.right);
    }
}

WebPrintWriteResultCallback ParseWebPrintWriteResultCallback(napi_env env, napi_value argv)
{
    if (!argv) {
        WVLOG_E("ParseWebPrintWriteResultCallback failed.");
        return nullptr;
    }
    napi_ref jsCallback = nullptr;
    napi_create_reference(env, argv, 1, &jsCallback);
    if (jsCallback) {
        WebPrintWriteResultCallback callbackImpl =
            [env, jCallback = std::move(jsCallback)](std::string jobId, uint32_t state) {
            if (!env) {
                return;
            }
            napi_handle_scope scope = nullptr;
            napi_open_handle_scope(env, &scope);
            if (scope == nullptr) {
                return;
            }
            napi_value setResult[INTEGER_TWO] = {0};
            napi_create_string_utf8(env, jobId.c_str(), NAPI_AUTO_LENGTH, &setResult[INTEGER_ZERO]);
            napi_create_uint32(env, state, &setResult[INTEGER_ONE]);
            napi_value args[INTEGER_TWO] = {setResult[INTEGER_ZERO], setResult[INTEGER_ONE]};
            napi_value callback = nullptr;
            napi_get_reference_value(env, jCallback, &callback);
            napi_value callbackResult = nullptr;
            napi_call_function(env, nullptr, callback, INTEGER_TWO, args, &callbackResult);
            napi_delete_reference(env, jCallback);
            napi_close_handle_scope(env, scope);
        };
        return callbackImpl;
    }
    return nullptr;
}

bool ParseWebPrintAttrParams(napi_env env, napi_value obj, PrintAttributesAdapter& printAttr)
{
    if (!obj) {
        WVLOG_E("ParseWebPrintAttrParams failed.");
        return false;
    }
    napi_value copyNumber = nullptr;
    napi_value pageRange = nullptr;
    napi_value isSequential = nullptr;
    napi_value pageSize = nullptr;
    napi_value isLandscape = nullptr;
    napi_value colorMode = nullptr;
    napi_value duplexMode = nullptr;
    napi_value margin = nullptr;
    napi_value option = nullptr;
    napi_get_named_property(env, obj, "copyNumber", &copyNumber);
    napi_get_named_property(env, obj, "pageRange", &pageRange);
    napi_get_named_property(env, obj, "isSequential", &isSequential);
    napi_get_named_property(env, obj, "pageSize", &pageSize);
    napi_get_named_property(env, obj, "isLandscape", &isLandscape);
    napi_get_named_property(env, obj, "colorMode", &colorMode);
    napi_get_named_property(env, obj, "duplexMode", &duplexMode);
    napi_get_named_property(env, obj, "margin", &margin);
    napi_get_named_property(env, obj, "option", &option);
    if (copyNumber) {
        NapiParseUtils::ParseUint32(env, copyNumber, printAttr.copyNumber);
    }
    if (isSequential) {
        NapiParseUtils::ParseBoolean(env, isSequential, printAttr.isSequential);
    }
    if (isLandscape) {
        NapiParseUtils::ParseBoolean(env, isLandscape,  printAttr.isLandscape);
    }
    if (colorMode) {
        NapiParseUtils::ParseUint32(env, colorMode, printAttr.colorMode);
    }
    if (duplexMode) {
        NapiParseUtils::ParseUint32(env, duplexMode, printAttr.duplexMode);
    }
    if (option) {
        NapiParseUtils::ParseString(env, option, printAttr.option);
    }
    ParsePrintRangeAdapter(env, pageRange, printAttr);
    ParsePrintPageSizeAdapter(env, pageSize, printAttr);
    ParsePrintMarginAdapter(env, margin, printAttr);
    return true;
}

napi_value NapiWebPrintDocument::OnStartLayoutWrite(napi_env env, napi_callback_info info)
{
    WVLOG_I("On Start Layout Write.");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_FIVE;
    napi_value argv[INTEGER_FIVE] = { 0 };
    WebPrintDocument *webPrintDocument = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webPrintDocument));
    if (webPrintDocument == nullptr) {
        WVLOG_E("unwrap webPrintDocument failed.");
        return result;
    }

    std::string jobId;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], jobId)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    int32_t fd;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_THREE], fd)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    PrintAttributesAdapter oldPrintAttr;
    PrintAttributesAdapter newPrintAttr;
    bool ret = false;
    ret = ParseWebPrintAttrParams(env, argv[INTEGER_ONE], oldPrintAttr);
    if (!ret) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    ret = ParseWebPrintAttrParams(env, argv[INTEGER_TWO], newPrintAttr);
    if (!ret) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    WebPrintWriteResultCallback writeResultCallback = nullptr;
    writeResultCallback = ParseWebPrintWriteResultCallback(env, argv[INTEGER_FOUR]);
    webPrintDocument->OnStartLayoutWrite(jobId, oldPrintAttr, newPrintAttr, fd, writeResultCallback);
    return result;
}

napi_value NapiWebPrintDocument::OnJobStateChanged(napi_env env, napi_callback_info info)
{
    WVLOG_I("On Job State Changed.");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO];
    NAPI_CALL(env, napi_get_undefined(env, &result));

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    WebPrintDocument *webPrintDocument = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webPrintDocument));
    if (webPrintDocument == nullptr) {
        WVLOG_E("unwrap webPrintDocument failed.");
        return result;
    }
    if (argc != INTEGER_TWO) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string jobId;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], jobId)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    int32_t state;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ONE], state)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    webPrintDocument->OnJobStateChanged(jobId, state);
    return result;
}

napi_value NapiWebPrintDocument::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    uint64_t addrWebPrintDoc = 0;
    bool loseLess = true;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (!NapiParseUtils::ParseUint64(env, argv[INTEGER_ZERO], addrWebPrintDoc, &loseLess)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    void *webPrintDocPtr = reinterpret_cast<void *>(addrWebPrintDoc);
    WebPrintDocument *webPrintDoc = new (std::nothrow) WebPrintDocument(webPrintDocPtr);
    if (webPrintDoc == nullptr) {
        WVLOG_E("new web print failed");
        return nullptr;
    }
    NAPI_CALL(env, napi_wrap(env, thisVar, webPrintDoc,
        [](napi_env env, void *data, void *hint) {
            WebPrintDocument *webPrintDocument = static_cast<WebPrintDocument *>(data);
            delete webPrintDocument;
        },
        nullptr, nullptr));
    return thisVar;
}

napi_value NapiWebviewController::SetPrintBackground(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    bool printBackgroundEnabled = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], printBackgroundEnabled)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "enable", "boolean"));
        return result;
    }

    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }
    webviewController->SetPrintBackground(printBackgroundEnabled);
    return result;
}

napi_value NapiWebviewController::GetPrintBackground(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);

    if (!webviewController) {
        return result;
    }

    bool printBackgroundEnabled = webviewController->GetPrintBackground();
    NAPI_CALL(env, napi_get_boolean(env, printBackgroundEnabled, &result));
    return result;
}

napi_value NapiWebviewController::SetWebSchemeHandler(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    WebviewController *webviewController = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        WVLOG_E("create message port failed, napi unwrap webviewController failed");
        return nullptr;
    }

    std::string scheme = "";
    if (!NapiParseUtils::ParseString(env, argv[0], scheme)) {
        WVLOG_E("NapiWebviewController::SetWebSchemeHandler parse scheme failed");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scheme", "string"));
        return nullptr;
    }

    WebSchemeHandler* handler = nullptr;
    napi_value obj = argv[1];
    napi_unwrap(env, obj, (void**)&handler);
    napi_create_reference(env, obj, 1, &handler->delegate_);

    if (!webviewController->SetWebSchemeHandler(scheme.c_str(), handler)) {
        WVLOG_E("NapiWebviewController::SetWebSchemeHandler failed");
    }
    return nullptr;
}

napi_value NapiWebviewController::ClearWebSchemeHandler(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return nullptr;
    }

    int32_t ret = webviewController->ClearWebSchemeHandler();
    if (ret != 0) {
        WVLOG_E("NapiWebviewController::ClearWebSchemeHandler failed");
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::SetServiceWorkerWebSchemeHandler(
    napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    std::string scheme = "";
    if (!NapiParseUtils::ParseString(env, argv[0], scheme)) {
        WVLOG_E("NapiWebviewController::SetWebSchemeHandler parse scheme failed");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scheme", "string"));
        return nullptr;
    }

    WebSchemeHandler* handler = nullptr;
    napi_value obj = argv[1];
    napi_unwrap(env, obj, (void**)&handler);
    napi_create_reference(env, obj, 1, &handler->delegate_);

    if (!WebviewController::SetWebServiveWorkerSchemeHandler(
        scheme.c_str(), handler)) {
        WVLOG_E("NapiWebviewController::SetWebSchemeHandler failed");
    }
    return nullptr;
}

napi_value NapiWebviewController::ClearServiceWorkerWebSchemeHandler(
    napi_env env, napi_callback_info info)
{
    int32_t ret = WebviewController::ClearWebServiceWorkerSchemeHandler();
    if (ret != 0) {
        WVLOG_E("ClearServiceWorkerWebSchemeHandler ret=%{public}d", ret);
        return nullptr;
    }
    return nullptr;
}

napi_value NapiWebviewController::EnableIntelligentTrackingPrevention(
    napi_env env, napi_callback_info info)
{
    WVLOG_I("enable/disable intelligent tracking prevention.");
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool enabled = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[INTEGER_ZERO], enabled)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebviewController *webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }
    webviewController->EnableIntelligentTrackingPrevention(enabled);
    return result;
}

napi_value NapiWebviewController::IsIntelligentTrackingPreventionEnabled(
    napi_env env, napi_callback_info info)
{
    WVLOG_I("get intelligent tracking prevention enabled value.");
    napi_value result = nullptr;
    WebviewController *webviewController = GetWebviewController(env, info);

    if (!webviewController) {
        return result;
    }

    bool enabled = webviewController->
        IsIntelligentTrackingPreventionEnabled();
    NAPI_CALL(env, napi_get_boolean(env, enabled, &result));
    return result;
}

bool GetHostList(napi_env env, napi_value array, std::vector<std::string>& hosts)
{
    uint32_t arrayLen = 0;
    napi_get_array_length(env, array, &arrayLen);
    if (arrayLen == 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return false;
    }

    for (uint32_t i = 0; i < arrayLen; i++) {
        napi_value hostItem = nullptr;
        napi_get_element(env, array, i, &hostItem);

        size_t hostLen = 0;
        napi_get_value_string_utf8(env, hostItem, nullptr, 0, &hostLen);
        if (hostLen == 0 || hostLen > UINT_MAX) {
            WVLOG_E("hostitem length is invalid");
            return false;
        }

        char host[hostLen + 1];
        int retCode = memset_s(host, sizeof(host), 0, hostLen + 1);
        if (retCode < 0) {
            WVLOG_E("memset_s failed, retCode=%{public}d", retCode);
            return false;
        }
        napi_get_value_string_utf8(env, hostItem, host, sizeof(host), &hostLen);
        std::string hostStr(host);
        hosts.emplace_back(hostStr);
    }
    return true;
}

napi_value NapiWebviewController::AddIntelligentTrackingPreventionBypassingList(
    napi_env env, napi_callback_info info)
{
    WVLOG_I("Add intelligent tracking prevention bypassing list.");
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[INTEGER_ZERO], &isArray));
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::vector<std::string> hosts;
    if (!GetHostList(env, argv[INTEGER_ZERO], hosts)) {
        WVLOG_E("get host list failed, GetHostList fail");
        return result;
    }

    NWebHelper::Instance().AddIntelligentTrackingPreventionBypassingList(hosts);
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::RemoveIntelligentTrackingPreventionBypassingList(
    napi_env env, napi_callback_info info)
{
    WVLOG_I("Remove intelligent tracking prevention bypassing list.");
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[INTEGER_ZERO], &isArray));
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::vector<std::string> hosts;
    if (!GetHostList(env, argv[INTEGER_ZERO], hosts)) {
        WVLOG_E("get host list failed, GetHostList fail");
        return result;
    }

    NWebHelper::Instance().RemoveIntelligentTrackingPreventionBypassingList(hosts);
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::ClearIntelligentTrackingPreventionBypassingList(
    napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    WVLOG_I("Clear intelligent tracking prevention bypassing list.");
    NWebHelper::Instance().ClearIntelligentTrackingPreventionBypassingList();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::PauseAllTimers(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NWebHelper::Instance().PauseAllTimers();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::ResumeAllTimers(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NWebHelper::Instance().ResumeAllTimers();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::StartCamera(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }
    webviewController->StartCamera();

    return result;
}

napi_value NapiWebviewController::StopCamera(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }
    webviewController->StopCamera();

    return result;
}

napi_value NapiWebviewController::CloseCamera(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        return result;
    }
    webviewController->CloseCamera();

    return result;
}

napi_value NapiWebviewController::OnCreateNativeMediaPlayer(napi_env env, napi_callback_info info)
{
    WVLOG_D("put on_create_native_media_player callback");

    size_t argc = INTEGER_ONE;
    napi_value value = nullptr;
    napi_value argv[INTEGER_ONE];
    napi_get_cb_info(env, info, &argc, argv, &value, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("arg count %{public}zu is not equal to 1", argc);
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_function) {
        WVLOG_E("arg type is invalid");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    napi_ref callback = nullptr;
    napi_create_reference(env, argv[INTEGER_ZERO], INTEGER_ONE, &callback);
    if (!callback) {
        WVLOG_E("failed to create reference for callback");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController || !webviewController->IsInit()) {
        WVLOG_E("webview controller is null or not init");
        return nullptr;
    }

    webviewController->OnCreateNativeMediaPlayer(env, std::move(callback));
    return nullptr;
}

napi_value NapiWebviewController::SetRenderProcessMode(
    napi_env env, napi_callback_info info)
{
    WVLOG_I("set render process mode.");
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    NAPI_CALL(env, napi_get_undefined(env, &result));
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    int32_t mode = 0;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], mode)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    NWebHelper::Instance().SetRenderProcessMode(
        static_cast<RenderProcessMode>(mode));

    return result;
}

napi_value NapiWebviewController::GetRenderProcessMode(
    napi_env env, napi_callback_info info)
{
    WVLOG_I("get render mode.");
    napi_value result = nullptr;

    int32_t mode = static_cast<int32_t>(NWebHelper::Instance().GetRenderProcessMode());
    NAPI_CALL(env, napi_create_int32(env, mode, &result));
    return result;
}

napi_value NapiWebviewController::PrecompileJavaScript(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = {0};
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE) {
        WVLOG_E("PrecompileJavaScript: args count is not allowed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        WVLOG_E("PrecompileJavaScript: init webview controller error.");
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }

    std::string url;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], url) || url.empty()) {
        WVLOG_E("PrecompileJavaScript: the given url is not allowed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string script;
    bool parseResult = webviewController->ParseScriptContent(env, argv[INTEGER_ONE], script);
    if (!parseResult) {
        WVLOG_E("PrecompileJavaScript: the given script is not allowed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    auto cacheOptions = webviewController->ParseCacheOptions(env, argv[INTEGER_TWO]);

    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);
    if (promise && deferred) {
        ErrCode code = webviewController->PrecompileJavaScriptPromise(
            env, deferred, url, script, cacheOptions);
        if (code != NO_ERROR) {
            WVLOG_E("PrecompileJavaScript: failed to compile javascript.");
            BusinessError::ThrowErrorByErrcode(env, code);
            return promise;
        }
    }

    return promise;
}

napi_value NapiWebviewController::WarmupServiceWorker(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string url;
    if (!ParsePrepareUrl(env, argv[INTEGER_ZERO], url)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
        return result;
    }

    NWebHelper::Instance().WarmupServiceWorker(url);
    return result;
}

napi_value NapiWebviewController::InjectOfflineResource(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        WVLOG_E("InjectOfflineResource: args count is not allowed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    napi_value resourcesList = argv[INTEGER_ZERO];
    bool isArray = false;
    napi_is_array(env, resourcesList, &isArray);
    if (!isArray) {
        WVLOG_E("InjectOfflineResource: param is not array.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    AddResourcesToMemoryCache(env, info, resourcesList);
    return result;
}

void NapiWebviewController::AddResourcesToMemoryCache(napi_env env,
                                                      napi_callback_info info,
                                                      napi_value& resourcesList)
{
    uint32_t resourcesCount = 0;
    napi_get_array_length(env, resourcesList, &resourcesCount);

    if (resourcesCount > MAX_RESOURCES_COUNT || resourcesCount == 0) {
        WVLOG_E("InjectOfflineResource: too many resources or empty resources.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }

    for (uint32_t i = 0 ; i < resourcesCount ; i++) {
        napi_value urlListObj = nullptr;    
        napi_value resourceObj = nullptr;
        napi_value headersObj = nullptr;
        napi_value typeObj = nullptr;
        napi_value obj = nullptr;

        napi_create_array(env, &headersObj);
        napi_create_array(env, &urlListObj);

        napi_get_element(env, resourcesList, i, &obj);
        if ((napi_get_named_property(env, obj, "urlList", &urlListObj) != napi_ok) ||
            (napi_get_named_property(env, obj, "resource", &resourceObj) != napi_ok) ||
            (napi_get_named_property(env, obj, "responseHeaders", &headersObj) != napi_ok) ||
            (napi_get_named_property(env, obj, "type", &typeObj) != napi_ok)) {
            WVLOG_E("InjectOfflineResource: parse params from resource map failed.");
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            continue;
        }

        OfflineResourceValue resourceValue;
        resourceValue.urlList = urlListObj;
        resourceValue.resource = resourceObj;
        resourceValue.responseHeaders = headersObj;
        resourceValue.type = typeObj;
        AddResourceItemToMemoryCache(env, info, resourceValue);
    }
}

void NapiWebviewController::AddResourceItemToMemoryCache(napi_env env,
                                                         napi_callback_info info,
                                                         OfflineResourceValue resourceValue)
{
    WebviewController* webviewController = GetWebviewController(env, info);
    if (!webviewController) {
        WVLOG_E("InjectOfflineResource: init webview controller error.");
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return;
    }

    std::vector<std::string> urlList;
    ParseURLResult result = webviewController->ParseURLList(env, resourceValue.urlList, urlList);
    if (result != ParseURLResult::OK) {
        WVLOG_E("InjectOfflineResource: parse url list failed.");
        auto errCode = result == ParseURLResult::FAILED ? PARAM_CHECK_ERROR : INVALID_URL;
        BusinessError::ThrowErrorByErrcode(env, errCode);
        return;
    }

    std::vector<uint8_t> resource = webviewController->ParseUint8Array(env, resourceValue.resource);
    if (resource.empty() || resource.size() > MAX_RESOURCE_SIZE) {
        WVLOG_E("InjectOfflineResource: parse resource failed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }

    std::map<std::string, std::string> responseHeaders;
    if (!webviewController->ParseResponseHeaders(env, resourceValue.responseHeaders, responseHeaders)) {
        WVLOG_E("InjectOfflineResource: parse response headers failed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }

    uint32_t type = 0;
    if (!NapiParseUtils::ParseUint32(env, resourceValue.type, type)) {
        WVLOG_E("InjectOfflineResource: parse type failed.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return;
    }

    webviewController->InjectOfflineResource(urlList, resource, responseHeaders, type);
}

napi_value NapiWebviewController::SetHostIP(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = { 0 };
    std::string hostName;
    std::string address;
    int32_t aliveTime = INTEGER_ZERO;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "three"));
        return result;
    }

    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], hostName) ||
        !NapiParseUtils::ParseString(env, argv[INTEGER_ONE], address) ||
        !NapiParseUtils::ParseInt32(env, argv[INTEGER_TWO], aliveTime) ||
        aliveTime <= 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,ParamCheckErrorMsgTemplate::PARAM_TYEPS_ERROR);
        return result;
    }

    NWebHelper::Instance().SetHostIP(hostName, address, aliveTime);
    NAPI_CALL(env, napi_get_undefined(env, &result));

    return result;
}

napi_value NapiWebviewController::ClearHostIP(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };
    std::string hostName;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_ONE, "one"));
        return result;
    }

    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], hostName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "hostName", "string"));
        return result;
    }

    NWebHelper::Instance().ClearHostIP(hostName);
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}
} // namespace NWeb
} // namespace OHOS
