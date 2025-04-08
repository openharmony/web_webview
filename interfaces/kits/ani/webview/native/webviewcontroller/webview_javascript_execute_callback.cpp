/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "webview_javascript_execute_callback.h"

#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS::NWeb {
using namespace NWebError;
const std::string JS_EXCUTE_MSG_ENUM_NAME = "JsMessageType";
const std::string JS_EXT_MSG_CLASS_NAME = "JsMessageExt";
thread_local napi_ref g_jsMsgExtClassRef;
// static
void WebviewJavaScriptExecuteCallback::InitJSExcute(napi_env env, napi_value exports)
{
    napi_value jsMsgTypeEnum = nullptr;
    napi_property_descriptor jsMsgTypeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NOT_SUPPORT", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(JsMessageType::NOTSUPPORT))),
        DECLARE_NAPI_STATIC_PROPERTY("STRING", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(JsMessageType::STRING))),
        DECLARE_NAPI_STATIC_PROPERTY("NUMBER", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(JsMessageType::NUMBER))),
        DECLARE_NAPI_STATIC_PROPERTY("BOOLEAN", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(JsMessageType::BOOLEAN))),
        DECLARE_NAPI_STATIC_PROPERTY("ARRAY_BUFFER", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(JsMessageType::ARRAYBUFFER))),
        DECLARE_NAPI_STATIC_PROPERTY("ARRAY", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(JsMessageType::ARRAY)))
    };
    napi_define_class(env, JS_EXCUTE_MSG_ENUM_NAME.c_str(), JS_EXCUTE_MSG_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(jsMsgTypeProperties) /
        sizeof(jsMsgTypeProperties[0]), jsMsgTypeProperties, &jsMsgTypeEnum);
    napi_set_named_property(env, exports, JS_EXCUTE_MSG_ENUM_NAME.c_str(), jsMsgTypeEnum);

    napi_value jsMsgExtClass = nullptr;
    napi_property_descriptor jsMsgExtClsProperties[] = {
        DECLARE_NAPI_FUNCTION("getType", NapiJsMessageExt::GetType),
        DECLARE_NAPI_FUNCTION("getString", NapiJsMessageExt::GetString),
        DECLARE_NAPI_FUNCTION("getNumber", NapiJsMessageExt::GetNumber),
        DECLARE_NAPI_FUNCTION("getBoolean", NapiJsMessageExt::GetBoolean),
        DECLARE_NAPI_FUNCTION("getArrayBuffer", NapiJsMessageExt::GetArrayBuffer),
        DECLARE_NAPI_FUNCTION("getArray", NapiJsMessageExt::GetArray)
    };
    napi_define_class(env, JS_EXT_MSG_CLASS_NAME.c_str(), JS_EXT_MSG_CLASS_NAME.length(),
        NapiJsMessageExt::JsConstructor, nullptr, sizeof(jsMsgExtClsProperties) / sizeof(jsMsgExtClsProperties[0]),
        jsMsgExtClsProperties, &jsMsgExtClass);
    napi_create_reference(env, jsMsgExtClass, 1, &g_jsMsgExtClassRef);
    napi_set_named_property(env, exports, JS_EXT_MSG_CLASS_NAME.c_str(), jsMsgExtClass);
}

void WebviewJavaScriptExecuteCallback::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::OnReceiveValue start");
    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;

    napi_get_uv_event_loop(env_, &loop);
    if (loop == nullptr) {
        return;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        return;
    }

    JavaScriptExecuteParam *param = new (std::nothrow) JavaScriptExecuteParam();
    if (param == nullptr) {
        delete work;
        return;
    }
    param->env_ = env_;
    param->callbackRef_ = callbackRef_;
    param->deferred_ = deferred_;
    param->result_ = result;
    param->extention_ = extention_;

    work->data = reinterpret_cast<void*>(param);

    int ret = uv_queue_work_with_qos(loop, work, [](uv_work_t *work) {}, UvAfterWorkCb, uv_qos_user_initiated);
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

void WebviewJavaScriptExecuteCallback::UvAfterWorkCb(uv_work_t* work, int status)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::UvAfterWorkCb");
    (void)status;
    if (!work) {
        return;
    }
    JavaScriptExecuteParam *param = reinterpret_cast<JavaScriptExecuteParam*>(work->data);
    if (!param) {
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(param->env_, &scope);
    if (scope == nullptr) {
        return;
    }

    if (param->callbackRef_) {
        UvAfterWorkCbAsync(param->env_, param->callbackRef_, param->result_, param->extention_);
    } else if (param->deferred_) {
        UvAfterWorkCbPromise(param->env_, param->deferred_, param->result_, param->extention_);
    }

    napi_close_handle_scope(param->env_, scope);
    delete param;
    param = nullptr;
    delete work;
    work = nullptr;
}

void WebviewJavaScriptExecuteCallback::UvAfterWorkCbAsync(napi_env env, napi_ref callbackRef,
    std::shared_ptr<NWebMessage> result, bool extention)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::UvAfterWorkCbAsync");
    napi_value setResult[INTEGER_TWO] = {0};
    if (result->GetType() == NWebValue::Type::STRING && result->GetString().empty()) {
        setResult[INTEGER_ZERO] = BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);
        napi_get_null(env, &setResult[INTEGER_ONE]);
    } else {
        napi_get_undefined(env, &setResult[INTEGER_ZERO]);
        if (!extention) {
            OHOS::NWeb::NapiParseUtils::ConvertNWebToNapiValue(env, result, setResult[INTEGER_ONE]);
        } else {
            napi_value jsMsgExt = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, g_jsMsgExtClassRef, &jsMsgExt));
            NAPI_CALL_RETURN_VOID(env, napi_new_instance(env, jsMsgExt, 0, NULL, &setResult[INTEGER_ONE]));

            WebJsMessageExt *webJsMessageExt = new (std::nothrow) WebJsMessageExt(result);
            if (webJsMessageExt == nullptr) {
                WVLOG_E("new WebJsMessageExt failed.");
                return;
            }

            napi_status status = napi_wrap(env, setResult[INTEGER_ONE], webJsMessageExt,
                [](napi_env env, void *data, void *hint) {
                    WebJsMessageExt *webJsMessageExt = static_cast<WebJsMessageExt *>(data);
                    delete webJsMessageExt;
                    webJsMessageExt = nullptr;
                },
                nullptr, nullptr);
            if (status != napi_status::napi_ok) {
                WVLOG_E("napi_wrap failed");
                return;
            }
        }
    }
    napi_value args[INTEGER_TWO] = {setResult[INTEGER_ZERO], setResult[INTEGER_ONE]};
    napi_value callback = nullptr;
    napi_value callbackResult = nullptr;

    napi_get_reference_value(env, callbackRef, &callback);
    napi_call_function(env, nullptr, callback, INTEGER_TWO, args, &callbackResult);
    napi_delete_reference(env, callbackRef);
}

void WebviewJavaScriptExecuteCallback::UvAfterWorkCbPromise(napi_env env, napi_deferred deferred,
    std::shared_ptr<NWebMessage> result, bool extention)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::UvAfterWorkCbPromise");
    napi_value setResult[INTEGER_TWO] = {0};
    setResult[INTEGER_ZERO] = NWebError::BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);
    if (!extention) {
        OHOS::NWeb::NapiParseUtils::ConvertNWebToNapiValue(env, result, setResult[INTEGER_ONE]);
    } else {
        napi_value jsMsgExt = nullptr;
        napi_status status = napi_get_reference_value(env, g_jsMsgExtClassRef, &jsMsgExt);
        if (status != napi_status::napi_ok) {
            WVLOG_E("napi_get_reference_value failed.");
            return;
        }
        status = napi_new_instance(env, jsMsgExt, 0, NULL, &setResult[INTEGER_ONE]);
        if (status != napi_status::napi_ok) {
            WVLOG_E("napi_new_instance failed.");
            return;
        }
        WebJsMessageExt *webJsMessageExt = new (std::nothrow) WebJsMessageExt(result);
        if (webJsMessageExt == nullptr) {
            WVLOG_E("new WebJsMessageExt failed.");
            return;
        }

        status = napi_wrap(env, setResult[INTEGER_ONE], webJsMessageExt,
            [](napi_env env, void *data, void *hint) {
                WebJsMessageExt *webJsMessageExt = static_cast<WebJsMessageExt *>(data);
                delete webJsMessageExt;
                webJsMessageExt = nullptr;
            },
            nullptr, nullptr);
        if (status != napi_status::napi_ok) {
            WVLOG_E("napi_wrap failed.");
            return;
        }
    }

    napi_value args[INTEGER_TWO] = {setResult[INTEGER_ZERO], setResult[INTEGER_ONE]};
    if (result->GetType() == NWebValue::Type::STRING && result->GetString().empty()) {
        napi_reject_deferred(env, deferred, args[INTEGER_ZERO]);
    } else {
        napi_resolve_deferred(env, deferred, args[INTEGER_ONE]);
    }
}

napi_value NapiJsMessageExt::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

int32_t WebJsMessageExt::ConvertToJsType(NWebValue::Type type)
{
    JsMessageType jsMessageType = JsMessageType::NOTSUPPORT;
    switch (type) {
        case NWebValue::Type::STRING:
            jsMessageType = JsMessageType::STRING;
            break;
        case NWebValue::Type::INTEGER:
        case NWebValue::Type::DOUBLE:
            jsMessageType = JsMessageType::NUMBER;
            break;
        case NWebValue::Type::BOOLEAN:
            jsMessageType = JsMessageType::BOOLEAN;
            break;
        case NWebValue::Type::BINARY:
            jsMessageType = JsMessageType::ARRAYBUFFER;
            break;
        case NWebValue::Type::STRINGARRAY:
        case NWebValue::Type::BOOLEANARRAY:
        case NWebValue::Type::DOUBLEARRAY:
        case NWebValue::Type::INT64ARRAY:
            jsMessageType = JsMessageType::ARRAY;
            break;
        default:
            jsMessageType = JsMessageType::NOTSUPPORT;
            break;
    }
    return static_cast<int32_t>(jsMessageType);
}

int32_t WebJsMessageExt::GetType()
{
    if (value_) {
        return ConvertToJsType(value_->GetType());
    }
    return static_cast<int32_t>(JsMessageType::NOTSUPPORT);
}

std::string WebJsMessageExt::GetString()
{
    if (value_) {
        return value_->GetString();
    }
    return "";
}

double WebJsMessageExt::GetNumber()
{
    if (value_) {
        return value_->GetDouble();
    }
    return 0;
}

bool WebJsMessageExt::GetBoolean()
{
    if (value_) {
        return value_->GetBoolean();
    }
    return false;
}

napi_value NapiJsMessageExt::GetType(napi_env env, napi_callback_info info)
{
    WVLOG_D("GetType webJsMessageExt start");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsMessageExt *webJsMessageExt = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_get_cb_info failed.");
        return result;
    }
    status = napi_unwrap(env, thisVar, (void **)&webJsMessageExt);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_unwrap failed.");
        return result;
    }
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    int32_t type = webJsMessageExt->GetType();
    status = napi_create_int32(env, type, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_create_int32 failed.");
        return result;
    }
    return result;
}

napi_value NapiJsMessageExt::GetString(napi_env env, napi_callback_info info)
{
    WVLOG_D("GetString webJsMessageExt");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::STRING)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetJsMsgResult(), result);
    return result;
}

napi_value NapiJsMessageExt::GetNumber(napi_env env, napi_callback_info info)
{
    WVLOG_D("GetNumber webJsMessageExt");
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::NUMBER)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        WVLOG_E("GetNumber webJsMessageExt failed not match");
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetJsMsgResult(), result);
    return result;
}

napi_value NapiJsMessageExt::GetBoolean(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::BOOLEAN)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetJsMsgResult(), result);
    return result;
}

napi_value NapiJsMessageExt::GetArrayBuffer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAYBUFFER)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }
    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetJsMsgResult(), result);
    return result;
}

napi_value NapiJsMessageExt::GetArray(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsMessageExt *webJsMessageExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void **)&webJsMessageExt));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAY)) {
        BusinessError::ThrowErrorByErrcode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    NapiParseUtils::ConvertNWebToNapiValue(env, webJsMessageExt->GetJsMsgResult(), result);
    return result;
}

} // namespace OHOS::NWeb
