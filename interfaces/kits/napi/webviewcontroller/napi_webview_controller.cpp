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

#include <unistd.h>
#include <uv.h>
#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "webview_javascript_execute_callback.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
thread_local napi_ref g_classWebMsgPort;
napi_value NapiWebviewController::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("setWebId", NapiWebviewController::SetWebId),
        DECLARE_NAPI_FUNCTION("jsProxy", NapiWebviewController::InnerJsProxy),
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
    
    return exports;
}

napi_value NapiWebviewController::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    return thisVar;
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
    WebviewController *webviewController = new WebviewController(webId);
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
    if (!controller) {
        WVLOG_E("Failed to run InnerJsProxy. The WebviewController must be associted with a Web component.");
        return result;
    }
    controller->SetNWebJavaScriptResultCallBack();
    controller->RegisterJavaScriptProxy(env, argv[INTEGER_ZERO], objName, methodList);
    return result;
}

napi_value NapiWebviewController::AccessForward(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    WebviewController *webviewController = nullptr;
    napi_status status = napi_unwrap(env, thisVar, (void **)&webviewController);
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if (webviewController == nullptr) {
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
    if (webviewController == nullptr) {
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
    if (valueType != napi_string) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    std::string message;
    if (!NapiParseUtils::ParseString(env, argv[INTEGER_ZERO], message)) {
        BusinessError::ThrowErrorByErrcode(env, PARAM_CHECK_ERROR);
        return result;
    }

    WebMessagePort *msgPort = nullptr;
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&msgPort));
    if (msgPort == nullptr) {
        WVLOG_E("post message failed, napi unwrap msg port failed");
        return nullptr;
    }
    ErrCode ret = msgPort->PostPortMessage(message);
    if (ret != NO_ERROR) {
        BusinessError::ThrowErrorByErrcode(env, ret);
        return result;
    }
    NAPI_CALL(env, napi_get_undefined(env, &result));

    return result;
}

void NWebValueCallbackImpl::OnReceiveValue(std::string result)
{
    WVLOG_D("message port received msg, msg = %{public}s", result.c_str());
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
        napi_value result[INTEGER_ONE] = {0};
        napi_create_string_utf8(data->env_, data->msg_.c_str(), data->msg_.length(), &result[INTEGER_ZERO]);
        napi_value onMsgEventFunc = nullptr;
        napi_get_reference_value(data->env_, data->callback_, &onMsgEventFunc);
        napi_value placeHodler = nullptr;
        napi_call_function(data->env_, nullptr, onMsgEventFunc, INTEGER_ONE, &result[INTEGER_ZERO], &placeHodler);
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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

    if (!webviewController) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if (!webviewController->ParseUrl(env, argv[INTEGER_ZERO], webSrc)) {
        BusinessError::ThrowErrorByErrcode(env, INVALID_URL);
    }
    std::string fileProtocolName = "file";
    if (webSrc.substr(INTEGER_ZERO, fileProtocolName.size()) == "file") {
        std::string filePath = webSrc;
        std::string fileProtocol = "file:///";
        filePath.erase(INTEGER_ZERO, fileProtocol.size());
        int isFileExist = access(filePath.c_str(), F_OK);
        if (isFileExist == -1) {
            BusinessError::ThrowErrorByErrcode(env, INVALID_RESOURCE);
        }
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
            NapiParseUtils::ParseString(env, keyObj, value);
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if ((!webviewController) || (status != napi_ok)) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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
    if (!controller) {
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

    if (!webviewController) {
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
    if ((!webviewController) || (status != napi_ok)) {
        BusinessError::ThrowErrorByErrcode(env, INIT_ERROR);
        return nullptr;
    }

    std::string url = "";
    url = webviewController->GetUrl();
    napi_create_string_utf8(env, url.c_str(), url.length(), &result);

    return result;
}

} // namespace NWeb
} // namespace OHOS
