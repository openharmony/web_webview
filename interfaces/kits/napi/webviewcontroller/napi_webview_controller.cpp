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

#include <securec.h>
#include <unistd.h>
#include <uv.h>

#include "business_error.h"
#include "context/application_context.h"
#include "napi_parse_utils.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "web_errors.h"
#include "webview_javascript_execute_callback.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
thread_local napi_ref g_classWebMsgPort;
thread_local napi_ref g_historyListRef;
napi_value NapiWebviewController::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("initializeWebEngine", NapiWebviewController::InitializeWebEngine),
        DECLARE_NAPI_STATIC_FUNCTION("setWebDebuggingAccess", NapiWebviewController::SetWebDebuggingAccess),
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
        DECLARE_NAPI_FUNCTION("getTitle", NapiWebviewController::GetTitle),
        DECLARE_NAPI_FUNCTION("getPageHeight", NapiWebviewController::GetPageHeight),
        DECLARE_NAPI_FUNCTION("backOrForward", NapiWebviewController::BackOrForward),
        DECLARE_NAPI_FUNCTION("storeWebArchive", NapiWebviewController::StoreWebArchive),
        DECLARE_NAPI_FUNCTION("createWebMessagePorts", NapiWebviewController::CreateWebMessagePorts),
        DECLARE_NAPI_FUNCTION("postMessage", NapiWebviewController::PostMessage),
        DECLARE_NAPI_FUNCTION("getHitTestValue", NapiWebviewController::GetHitTestValue),
        DECLARE_NAPI_FUNCTION("requestFocus", NapiWebviewController::RequestFocus),
        DECLARE_NAPI_FUNCTION("loadUrl", NapiWebviewController::LoadUrl),
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
        DECLARE_NAPI_FUNCTION("deleteJavaScriptRegister", NapiWebviewController::DeleteJavaScriptRegister),
        DECLARE_NAPI_FUNCTION("runJavaScript", NapiWebviewController::RunJavaScript),
        DECLARE_NAPI_FUNCTION("getUrl", NapiWebviewController::GetUrl),
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
        DECLARE_NAPI_STATIC_FUNCTION("customizeSchemes", NapiWebviewController::CustomizeSchemes),
        DECLARE_NAPI_FUNCTION("innerGetThisVar", NapiWebviewController::InnerGetThisVar),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, WEBVIEW_CONTROLLER_CLASS_NAME.c_str(), WEBVIEW_CONTROLLER_CLASS_NAME.length(),
        NapiWebviewController::JsConstructor, nullptr, sizeof(properties) / sizeof(properties[0]),
        properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "define js class WebviewController failed");
    napi_status status = napi_set_named_property(env, exports, "WebviewController", constructor);
    NAPI_ASSERT(env, status == napi_ok, "set property WebviewController failed");

    napi_value msgPortCons = nullptr;
    napi_property_descriptor msgPortProperties[] = {
        DECLARE_NAPI_FUNCTION("close", NapiWebMessagePort::Close),
        DECLARE_NAPI_FUNCTION("postMessageEvent", NapiWebMessagePort::PostMessageEvent),
        DECLARE_NAPI_FUNCTION("onMessageEvent", NapiWebMessagePort::OnMessageEvent),
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

    napi_value historyList = nullptr;
    napi_property_descriptor historyListProperties[] = {
        DECLARE_NAPI_FUNCTION("getItemAtIndex", NapiWebHistoryList::GetItem)
    };
    napi_define_class(env, WEB_HISTORY_LIST_CLASS_NAME.c_str(), WEB_HISTORY_LIST_CLASS_NAME.length(),
        NapiWebHistoryList::JsConstructor, nullptr, sizeof(historyListProperties) / sizeof(historyListProperties[0]),
        historyListProperties, &historyList);
    napi_create_reference(env, historyList, 1, &g_historyListRef);
    napi_set_named_property(env, exports, WEB_HISTORY_LIST_CLASS_NAME.c_str(), historyList);

    return exports;
}

napi_value NapiWebviewController::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    WebviewController *webviewController = new (std::nothrow) WebviewController();
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
    const std::string& bundle_path = ctx->GetBundleCodeDir();
    NWebHelper::Instance().SetBundlePath(bundle_path);
    if (!NWebHelper::Instance().Init(true)) {
        WVLOG_E("Failed to init web engine due to NWebHelper failure.");
        return nullptr;
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    WVLOG_I("NWebHelper initialized, init web engine done, bundle_path: %{public}s", bundle_path.c_str());
    return result;
}

napi_value NapiWebviewController::SetWebDebuggingAccess(napi_env env, napi_callback_info info)
{
    WVLOG_D("SetWebDebuggingAccess start");

    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool webDebuggingAccess = false;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], webDebuggingAccess)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    WebviewController::webDebuggingAccess_ = webDebuggingAccess;

    NAPI_CALL(env, napi_get_undefined(env, &result));
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

napi_value NapiWebviewController::InnerJsProxy(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebviewController::InnerJsProxy");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE) {
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
    std::vector<std::string> methodList;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], objName) ||
        !NapiParseUtils::ParseStringArray(env, argv[INTEGER_TWO], methodList)) {
        WVLOG_E("Failed to run InnerJsProxy beacuse of wrong Param type.");
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        WVLOG_E("Failed to run InnerJsProxy. The WebviewController must be associted with a Web component.");
        return result;
    }
    controller->SetNWebJavaScriptResultCallBack();
    controller->RegisterJavaScriptProxy(env, argv[INTEGER_ZERO], objName, methodList);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    bool access = webviewController->AccessForward();
    NAPI_CALL(env, napi_get_boolean(env, access, &result));
    return result;
}

napi_value NapiWebviewController::AccessBackward(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    bool access = webviewController->AccessBackward();
    NAPI_CALL(env, napi_get_boolean(env, access, &result));
    return result;
}

napi_value NapiWebviewController::Forward(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    webviewController->Forward();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::Backward(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    int32_t step = INTEGER_ZERO;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], step)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    webviewController->ClearHistory();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::OnActive(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    webviewController->OnActive();
    WVLOG_D("The web component has been successfully activated");
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::OnInactive(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    webviewController->OnInactive();
    WVLOG_D("The web component has been successfully inactivated");
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::Refresh(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    webviewController->Refresh();
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

napi_value NapiWebviewController::CreateWebMessagePorts(napi_env env, napi_callback_info info)
{
    WVLOG_D("create web message port");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    WebviewController *webviewController = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webviewController));
    if (webviewController == nullptr || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        WVLOG_E("create message port failed, napi unwrap webviewController failed");
        return nullptr;
    }
    int32_t nwebId = webviewController->GetWebId();
    std::vector<std::string> ports;
    webviewController->CreateWebMessagePorts(ports);
    if (ports.size() != INTEGER_TWO) {
        WVLOG_E("create web message port failed");
        return result;
    }
    napi_value msgPortcons = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, g_classWebMsgPort, &msgPortcons));
    napi_create_array(env, &result);
    napi_value consParam[INTEGER_TWO][INTEGER_TWO] = {{0}};
    for (uint32_t i = 0; i < INTEGER_TWO; i++) {
        napi_value msgPortObj = nullptr;
        NAPI_CALL(env, napi_create_int32(env, nwebId, &consParam[i][INTEGER_ZERO]));
        NAPI_CALL(env, napi_create_string_utf8(env, ports[i].c_str(), ports[i].length(), &consParam[i][INTEGER_ONE]));
        NAPI_CALL(env, napi_new_instance(env, msgPortcons, INTEGER_TWO, consParam[i], &msgPortObj));
        napi_set_element(env, result, i, msgPortObj);
    }

    return result;
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string portName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], portName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[INTEGER_ONE], &isArray));
    if (!isArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    uint32_t arrayLen = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[INTEGER_ONE], &arrayLen));
    if (arrayLen == 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    std::vector<std::string> sendPorts;
    napi_valuetype valueType = napi_undefined;
    for (uint32_t i = 0; i < arrayLen; i++) {
        napi_value portItem = nullptr;
        napi_get_element(env, argv[INTEGER_ONE], i, &portItem);
        NAPI_CALL(env, napi_typeof(env, portItem, &valueType));
        if (valueType != napi_object) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return result;
        }
        WebMessagePort *msgPort = nullptr;
        NAPI_CALL(env, napi_unwrap(env, portItem, (void **)&msgPort));
        if (!msgPort) {
            WVLOG_E("post port to html failed, napi unwrap msg port fail");
            return nullptr;
        }
        std::string portHandle = msgPort->GetPortHandle();
        sendPorts.emplace_back(portHandle);
    }

    std::string urlStr;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_TWO], urlStr)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    size_t argc = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = {0};
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
    WebMessagePort *msgPort = new (std::nothrow) WebMessagePort(webId, portHandle);
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

napi_value NapiWebMessagePort::PostMessageEvent(napi_env env, napi_callback_info info)
{
    WVLOG_D("message port post message");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);

    bool isArrayBuffer = false;
    NAPI_CALL(env, napi_is_arraybuffer(env, argv[INTEGER_ZERO], &isArrayBuffer));
    if (valueType != napi_string && !isArrayBuffer) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
    if (valueType == napi_string) {
        size_t bufferSize = 0;
        napi_get_value_string_utf8(env, argv[INTEGER_ZERO], nullptr, 0, &bufferSize);
        if (bufferSize > UINT_MAX) {
            WVLOG_E("String length is too long");
            return result;
        }
        char* stringValue = new (std::nothrow) char[bufferSize + 1];
        if (stringValue == nullptr) {
            BusinessError::ThrowErrorByErrcode(env, NEW_OOM);
            return result;
        }
        size_t jsStringLength = 0;
        napi_get_value_string_utf8(env, argv[INTEGER_ZERO], stringValue, bufferSize + 1, &jsStringLength);
        std::string message(stringValue);
        delete [] stringValue;
        stringValue = nullptr;

        webMsg->SetType(NWebValue::Type::STRING);
        webMsg->SetString(message);
    } else if (isArrayBuffer) {
        uint8_t *arrBuf = nullptr;
        size_t byteLength = 0;
        napi_get_arraybuffer_info(env, argv[INTEGER_ZERO], (void**)&arrBuf, &byteLength);
        std::vector<uint8_t> vecData(arrBuf, arrBuf + byteLength);
        webMsg->SetType(NWebValue::Type::BINARY);
        webMsg->SetBinary(vecData);
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
        return;
    }

    napi_value result[INTEGER_ONE] = {0};
    std::shared_ptr<NWebMessage> webMsg = data->msg_;
    if (webMsg->GetType() == NWebValue::Type::STRING) {
        std::string msgStr = webMsg->GetString();
        napi_create_string_utf8(data->env_, msgStr.c_str(), msgStr.length(), &result[INTEGER_ZERO]);
    } else if (webMsg->GetType() == NWebValue::Type::BINARY) {
        std::vector<uint8_t> msgArr = webMsg->GetBinary();
        void *arrayData = nullptr;
        napi_create_arraybuffer(data->env_, msgArr.size(), &arrayData, &result[INTEGER_ZERO]);
        if (arrayData == nullptr) {
            WVLOG_E("Create arraybuffer failed");
            napi_close_handle_scope(data->env_, scope);
            return;
        }
        for (size_t i = 0; i < msgArr.size(); ++i) {
            *(uint8_t*)((uint8_t*)arrayData + i) = msgArr[i];
        }
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
    work->data = reinterpret_cast<void*>(param);
    uv_queue_work(loop, work, [](uv_work_t *work) {}, UvWebMessageOnReceiveValueCallback);
    std::unique_lock<std::mutex> lock(param->mutex_);
    param->condition_.wait(lock, [&param] { return param->ready_; });
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
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
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, [](uv_work_t *work, int status) {
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

        napi_delete_reference(data->env_, data->callback_);
        delete data;
        data = nullptr;
        delete work;
        work = nullptr;
    });
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_function) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    napi_ref onMsgEventFunc = nullptr;
    NAPI_CALL(env, napi_create_reference(env, argv[INTEGER_ZERO], INTEGER_ONE, &onMsgEventFunc));

    auto callbackImpl = std::make_shared<NWebValueCallbackImpl>(env, onMsgEventFunc);

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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    int32_t webId = webviewController->GetWebId();
    napi_create_int32(env, webId, &result);

    return result;
}

napi_value NapiWebviewController::GetUserAgent(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::string userAgent = "";
    userAgent = webviewController->GetUserAgent();
    napi_create_string_utf8(env, userAgent.c_str(), userAgent.length(), &result);

    return result;
}

napi_value NapiWebviewController::GetTitle(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::string title = "";
    title = webviewController->GetTitle();
    napi_create_string_utf8(env, title.c_str(), title.length(), &result);

    return result;
}

napi_value NapiWebviewController::GetPageHeight(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    int32_t step = -1;
    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], step)) {
        WVLOG_E("Parameter is not integer number type.");
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    HitTestResult nwebResult = webviewController->GetHitTestValue();

    napi_create_object(env, &result);

    napi_value type;
    napi_create_uint32(env, nwebResult.GetType(), &type);
    napi_set_named_property(env, result, "type", type);

    napi_value extra;
    napi_create_string_utf8(env, nwebResult.GetExtra().c_str(), NAPI_AUTO_LENGTH, &extra);
    napi_set_named_property(env, result, "extra", extra);

    return result;
}

napi_value NapiWebviewController::RequestFocus(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    webviewController->RequestFocus();
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    std::string webSrc;
    ErrCode ret = webviewController->ParseUrl(env, argv[INTEGER_ZERO], webSrc);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
        return nullptr;
    }
    if (argc == INTEGER_ONE) {
        ret = webviewController->LoadUrl(webSrc);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    if ((argc >= INTEGER_FOUR) && !NapiParseUtils::ParseString(env, argv[INTEGER_THREE], baseUrl)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    if ((argc == INTEGER_FIVE) && !NapiParseUtils::ParseString(env, argv[INTEGER_FOUR], historyUrl)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    bool forward;
    if (!NapiParseUtils::ParseBoolean(env, argv[0], forward)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    std::string searchString;
    if (!NapiParseUtils::ParseString(env, argv[0], searchString)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    float factor = 0.0;
    if (!NapiParseUtils::ParseFloat(env, argv[0], factor)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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

napi_value NapiWebviewController::RegisterJavaScriptProxy(napi_env env, napi_callback_info info)
{
    WVLOG_D("NapiWebviewController::RegisterJavaScriptProxy");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[INTEGER_ZERO], &valueType);
    if (valueType != napi_object) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string objName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ONE], objName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::vector<std::string> methodList;
    if (!NapiParseUtils::ParseStringArray(env, argv[INTEGER_TWO], methodList)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebviewController *controller = nullptr;
    napi_unwrap(env, thisVar, (void **)&controller);
    if (!controller || !controller->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return result;
    }
    controller->SetNWebJavaScriptResultCallBack();
    controller->RegisterJavaScriptProxy(env, argv[INTEGER_ZERO], objName, methodList);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string objName;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], objName)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    size_t argcPromise = INTEGER_ONE;
    size_t argcCallback = INTEGER_TWO;
    napi_value argv[INTEGER_TWO] = { 0 };

    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (argc != argcPromise && argc != argcCallback) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    std::string script;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], script)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (argc == argcCallback) {
        napi_valuetype valueType = napi_null;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType != napi_function) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return result;
        }
    }
    return RunJavaScriptInternal(env, info, script);
}

napi_value NapiWebviewController::RunJavaScriptInternal(napi_env env, napi_callback_info info,
    const std::string &script)
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
            webviewController->RunJavaScriptCallback(script, env, std::move(jsCallback));
        }
        return result;
    } else if (argc == argcPromise) {
        napi_deferred deferred = nullptr;
        napi_value promise = nullptr;
        napi_create_promise(env, &deferred, &promise);
        if (promise && deferred) {
            webviewController->RunJavaScriptPromise(script, env, deferred);
        }
        return promise;
    }
    return result;
}

napi_value NapiWebviewController::GetUrl(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::string url = "";
    url = webviewController->GetUrl();
    napi_create_string_utf8(env, url.c_str(), url.length(), &result);

    return result;
}

napi_value NapiWebviewController::GetOriginalUrl(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::string url = "";
    url = webviewController->GetOriginalUrl();
    napi_create_string_utf8(env, url.c_str(), url.length(), &result);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], enable)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
        return result;
    }

    if (argc == argcCallbackParaNum) {
        napi_valuetype valueType = napi_null;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
        napi_typeof(env, argv[argcCallbackParaNum - 1], &valueType);
        if (valueType != napi_function) {
            NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR);
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
            if (ret != NO_ERROR) {
                if (ret == NWEB_ERROR) {
                    return nullptr;
                }
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
            if (ret != NO_ERROR) {
                if (ret == NWEB_ERROR) {
                    return nullptr;
                }
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
    bool include_disk_files;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_ONE) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], include_disk_files)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok) || !webviewController->IsInit()) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }
    webviewController->RemoveCache(include_disk_files);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    if (!NapiParseUtils::ParseInt32(env, argv[0], index)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    if (index >= historyList->GetListSize() || index < 0) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    if (!webState) {
        return result;
    }

    NAPI_CALL(env, napi_create_arraybuffer(env, webState->size(), &data, &buffer));
    int retCode = memcpy_s(data, webState->size(), webState->data(), webState->size());
    if (retCode != 0) {
        return result;
    }
    NAPI_CALL(env, napi_create_typedarray(env, napi_uint8_array, webState->size(), buffer, 0, &result));
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }
    NAPI_CALL(env, napi_is_typedarray(env, argv[0], &isTypedArray));
    if (!isTypedArray) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    napi_typedarray_type type;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    NAPI_CALL(env, napi_get_typedarray_info(env, argv[0], &type, &length, nullptr, &buffer, &offset));
    if (type != napi_uint8_array) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
    webviewController->RestoreWebState(std::make_shared<std::vector<uint8_t>>(state));
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], bottom)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseBoolean(env, argv[0], top)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    napi_value array = argv[INTEGER_ZERO];
    bool isArray = false;
    napi_is_array(env, array, &isArray);
    if (isArray) {
        std::string cmdLine;
        uint32_t arrayLength = INTEGER_ZERO;
        napi_get_array_length(env, array, &arrayLength);
        if (arrayLength > MAX_CUSTOM_SCHEME_SIZE) {
            BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
            return nullptr;
        }
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string schemeName;
            bool isSupportCORS;
            bool isSupportFetch;
            napi_value schemeNameObj = nullptr;
            napi_value isSupportCORSObj = nullptr;
            napi_value isSupportFetchObj = nullptr;
            napi_value obj = nullptr;
            napi_get_element(env, array, i, &obj);
            if ((napi_get_named_property(env, obj, "schemeName", &schemeNameObj) != napi_ok) ||
                (napi_get_named_property(env, obj, "isSupportCORS", &isSupportCORSObj) != napi_ok) ||
                (napi_get_named_property(env, obj, "isSupportFetch", &isSupportFetchObj) != napi_ok)) {
                BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
                return nullptr;
            }
            NapiParseUtils::ParseString(env, schemeNameObj, schemeName);
            if (!CheckSchemeName(schemeName)) {
                BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
                return nullptr;
            }
            NapiParseUtils::ParseBoolean(env, isSupportCORSObj, isSupportCORS);
            NapiParseUtils::ParseBoolean(env, isSupportFetchObj, isSupportFetch);
            std::string corsCmdLine = isSupportCORS ? "1," : "0,";
            std::string fetchCmdLine = isSupportFetch ? "1;" : "0;";
            cmdLine.append(schemeName + "," + corsCmdLine + fetchCmdLine);
        }
        cmdLine.pop_back();
        WVLOG_I("Reg scheme cmdline %{public}s", cmdLine.c_str());
        WebviewController::customeSchemeCmdLine_ = cmdLine;
    } else {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ZERO], x)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ONE], y)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ZERO], deltaX)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ONE], deltaY)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ZERO], vx)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    if (!NapiParseUtils::ParseFloat(env, argv[INTEGER_ONE], vy)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
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
} // namespace NWeb
} // namespace OHOS
