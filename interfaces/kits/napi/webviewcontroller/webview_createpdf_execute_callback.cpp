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

#include <fstream>
#include <iostream>
#include <securec.h>

#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_napi_scope.h"
#include "nweb_log.h"
#include "web_errors.h"
#include "webview_createpdf_execute_callback.h"
namespace OHOS::NWeb {
using namespace NWebError;
const std::string JS_EXT_ARR_CLASS_NAME = "PdfData";
thread_local napi_ref g_jsArrExtClassRef;
// static
void WebviewCreatePDFExecuteCallback::InitJSExcute(napi_env env, napi_value exports)
{
    napi_value jsArrExtClass = nullptr;
    napi_property_descriptor jsArrExtClsProperties[] = { DECLARE_NAPI_FUNCTION(
        "pdfArrayBuffer", NapiArrayBufferExt::GetArrayBuffer) };
    napi_define_class(env, JS_EXT_ARR_CLASS_NAME.c_str(), JS_EXT_ARR_CLASS_NAME.length(),
        NapiArrayBufferExt::JsConstructor, nullptr, sizeof(jsArrExtClsProperties) / sizeof(jsArrExtClsProperties[0]),
        jsArrExtClsProperties, &jsArrExtClass);
    napi_create_reference(env, jsArrExtClass, 1, &g_jsArrExtClassRef);
    napi_set_named_property(env, exports, JS_EXT_ARR_CLASS_NAME.c_str(), jsArrExtClass);
}

void WebviewCreatePDFExecuteCallback::ReleaseArrayBufferExecuteParamAndUvWork(
    ArrayBufferExecuteParam* param, uv_work_t* work)
{
    if (param != nullptr) {
        if (param->result_ != nullptr) {
            delete[] param->result_;
            param->result_ = nullptr;
        }
        delete param;
        param = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
}

void WebviewCreatePDFExecuteCallback::OnReceiveValue(const char* value, const long size)
{
    if (value == nullptr || size <= 0) {
        WVLOG_E("[CreatePDF] value is null or size is invalid");
        return;
    }

    ArrayBufferExecuteParam* param = new (std::nothrow) ArrayBufferExecuteParam();
    if (param == nullptr) {
        WVLOG_E("[CreatePDF] allocate ArrayBufferExecuteParam failed");
        return;
    }
    param->env_ = env_;
    param->callbackRef_ = callbackRef_;
    param->deferred_ = deferred_;
    param->result_ = new (std::nothrow) char[size + 1];
    if (param->result_ == nullptr) {
        WVLOG_E("new char failed");
        delete param;
        return;
    }
    if (memcpy_s(param->result_, size, value, size) != 0) {
        WVLOG_E("[CreatePDF] memcpy failed");
        delete[] param->result_;
        delete param;
        return;
    }
    param->size_ = size;
    auto task = [param]() {
        std::shared_ptr<ArrayBufferExecuteParam> context(
            static_cast<ArrayBufferExecuteParam*>(param), [](ArrayBufferExecuteParam* ptr) { delete ptr; });
        napi_env env = param->env_;
        NApiScope scope(env);
        if (!scope.IsVaild()) {
            WVLOG_E("[CreatePDF] open handle scope failed");
            return;
        }

        if (param->callbackRef_) {
            UvAfterWorkCbAsync(env, param->callbackRef_, param->result_, param->size_);
        } else if (param->deferred_) {
            UvAfterWorkCbPromise(env, param->deferred_, param->result_, param->size_);
        }
    };
    if (napi_status::napi_ok != napi_send_event(env_, task, napi_eprio_immediate)) {
        WVLOG_E("OnReceiveValue: Failed to SendEvent");
    }
}

void WebviewCreatePDFExecuteCallback::UvAfterWorkCbAsync(
    napi_env env, napi_ref callbackRef, const char* result, const long size)
{
    napi_value setResult[INTEGER_TWO] = { 0 };

    if (result == nullptr) {
        setResult[INTEGER_ZERO] = BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);
        napi_get_null(env, &setResult[INTEGER_ONE]);
    } else {
        napi_get_undefined(env, &setResult[INTEGER_ZERO]);
        napi_value jsArrExt = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, g_jsArrExtClassRef, &jsArrExt));
        NAPI_CALL_RETURN_VOID(env, napi_new_instance(env, jsArrExt, 0, NULL, &setResult[INTEGER_ONE]));

        WebJsArrayBufferExt* webArrayBufferExt = new (std::nothrow) WebJsArrayBufferExt(result, size);
        if (webArrayBufferExt == nullptr) {
            WVLOG_E("new WebJsArrayBufferExt failed.");
            return;
        }

        napi_status status = napi_wrap(
            env, setResult[INTEGER_ONE], webArrayBufferExt,
            [](napi_env env, void* data, void* hint) {
                WebJsArrayBufferExt* webArrayBufferExt = static_cast<WebJsArrayBufferExt*>(data);
                delete webArrayBufferExt;
                webArrayBufferExt = nullptr;
            },
            nullptr, nullptr);
        if (status != napi_status::napi_ok) {
            if (webArrayBufferExt) {
                delete webArrayBufferExt;
                webArrayBufferExt = nullptr;
            }
            WVLOG_E("napi_wrap failed");
            return;
        }
    }
    napi_value args[INTEGER_TWO] = { setResult[INTEGER_ZERO], setResult[INTEGER_ONE] };
    napi_value callback = nullptr;
    napi_value callbackResult = nullptr;

    napi_get_reference_value(env, callbackRef, &callback);
    napi_call_function(env, nullptr, callback, INTEGER_TWO, args, &callbackResult);
    napi_delete_reference(env, callbackRef);
}

void WebviewCreatePDFExecuteCallback::UvAfterWorkCbPromise(
    napi_env env, napi_deferred deferred, const char* result, const long size)
{
    napi_value setResult[INTEGER_TWO] = { 0 };
    setResult[INTEGER_ZERO] = NWebError::BusinessError::CreateError(env, NWebError::INVALID_RESOURCE);

    napi_value jsArrExt = nullptr;
    napi_status status = napi_get_reference_value(env, g_jsArrExtClassRef, &jsArrExt);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_get_reference_value failed.");
        return;
    }
    status = napi_new_instance(env, jsArrExt, 0, NULL, &setResult[INTEGER_ONE]);
    if (status != napi_status::napi_ok) {
        WVLOG_E("napi_new_instance failed.");
        return;
    }
    WebJsArrayBufferExt* webArrayBufferExt = new (std::nothrow) WebJsArrayBufferExt(result, size);
    if (webArrayBufferExt == nullptr) {
        WVLOG_E("new WebJsArrayBufferExt failed.");
        return;
    }

    status = napi_wrap(
        env, setResult[INTEGER_ONE], webArrayBufferExt,
        [](napi_env env, void* data, void* hint) {
            WebJsArrayBufferExt* webArrayBufferExt = static_cast<WebJsArrayBufferExt*>(data);
            delete webArrayBufferExt;
            webArrayBufferExt = nullptr;
        },
        nullptr, nullptr);
    if (status != napi_status::napi_ok) {
        if (webArrayBufferExt) {
            delete webArrayBufferExt;
            webArrayBufferExt = nullptr;
        }
        WVLOG_E("napi_wrap failed.");
        return;
    }

    napi_value args[INTEGER_TWO] = { setResult[INTEGER_ZERO], setResult[INTEGER_ONE] };
    if (result == nullptr) {
        napi_reject_deferred(env, deferred, args[INTEGER_ZERO]);
    } else {
        napi_resolve_deferred(env, deferred, args[INTEGER_ONE]);
    }
}

napi_value NapiArrayBufferExt::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiArrayBufferExt::GetArrayBuffer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value result = nullptr;
    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = { 0 };

    WebJsArrayBufferExt* webArrayBufferExt = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, (void**)&webArrayBufferExt));
    if (webArrayBufferExt == nullptr) {
        WVLOG_E("unwrap webArrayBufferExt failed.");
        return result;
    }

    const char* pdfResult = webArrayBufferExt->GetPDFResult();
    const long size = webArrayBufferExt->GetPDFSize();
    if (pdfResult == nullptr || size <= 0) {
        WVLOG_E("[CreatePDF] invalid PDF result or size");
        return nullptr;
    }
    napi_value arraybuffer = nullptr;
    void* bufferData = nullptr;

    napi_status status = napi_create_arraybuffer(env, size, &bufferData, &arraybuffer);
    if (status != napi_ok) {
        WVLOG_E("[CreatePDF] create array buffer failed, status: %{public}d", status);
        return nullptr;
    }
    if (bufferData == nullptr) {
        WVLOG_E("[CreatePDF] bufferData is null after array buffer creation");
        return nullptr;
    }
    if (memcpy_s(bufferData, size, pdfResult, size) != 0) {
        WVLOG_E("[CreatePDF] memcpy failed");
        return nullptr;
    }
    status = napi_create_typedarray(env, napi_typedarray_type::napi_uint8_array, size, arraybuffer, 0, &result);
    if (status != napi_ok) {
        WVLOG_E("[CreatePDF] create typed array failed, status: %{public}d", status);
        return nullptr;
    }
    return result;
}

} // namespace OHOS::NWeb
