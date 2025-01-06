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
#ifndef NWEB_WEBVIEW_CREAT_PDF_EXECUTE_CALLBACK_H
#define NWEB_WEBVIEW_CREAT_PDF_EXECUTE_CALLBACK_H

#include <string>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_parse_utils.h"
#include "nweb_value_callback.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {
class NapiArrayBufferExt {
public:
    NapiArrayBufferExt() = default;
    ~NapiArrayBufferExt() = default;

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value GetArrayBuffer(napi_env env, napi_callback_info info);
};

class WebJsArrayBufferExt {
public:
    explicit WebJsArrayBufferExt(const char* value, const long size) : size_(size)
    {
        if (value != nullptr && size > 0) {
            value_ = new (std::nothrow) char[size];
            if (value_ != nullptr) {
                if (memcpy_s(value_, size, value, size) != 0) {
                    WVLOG_E("[CreatePDF] memcpy failed");
                    delete[] value_;
                    value_ = nullptr;
                    size_ = 0;
                }
            }
        }
    };
    ~WebJsArrayBufferExt()
    {
        size_ = 0;
        if (value_ != nullptr) {
            delete[] value_;
            value_ = nullptr;
        }
    }

    const char* GetPDFResult() const
    {
        return value_;
    }

    long GetPDFSize() const
    {
        return size_;
    }

private:
    char* value_ = nullptr;
    long size_ = 0;
};

class WebviewCreatePDFExecuteCallback : public NWebArrayBufferValueCallback {
public:
    explicit WebviewCreatePDFExecuteCallback(napi_env env, napi_ref callbackRef, napi_deferred deferred)
        : env_(env), callbackRef_(callbackRef), deferred_(deferred)
    {}
    ~WebviewCreatePDFExecuteCallback() = default;
    void OnReceiveValue(const char* value, const long size) override;

private:
    struct ArrayBufferExecuteParam {
        napi_env env_;
        napi_ref callbackRef_;
        napi_deferred deferred_;
        char* result_;
        long size_;
    };

    napi_env env_ = nullptr;
    napi_ref callbackRef_ = nullptr;
    napi_deferred deferred_ = nullptr;

    static void UvAfterWorkCb(uv_work_t* work, int status);
    static void UvAfterWorkCbAsync(napi_env env, napi_ref callbackRef, const char* result, const long size);
    static void UvAfterWorkCbPromise(napi_env env, napi_deferred deferred, const char* result, const long size);
    static void ReleaseArrayBufferExecuteParamAndUvWork(ArrayBufferExecuteParam* param, uv_work_t* work);

public:
    static void InitJSExcute(napi_env env, napi_value exports);
};

} // namespace OHOS::NWeb
#endif
