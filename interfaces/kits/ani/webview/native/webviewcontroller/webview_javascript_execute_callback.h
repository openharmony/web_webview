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
#ifndef NWEB_WEBVIEW_JAVA_SCRIPT_EXECUTE_CALLBACK_H
#define NWEB_WEBVIEW_JAVA_SCRIPT_EXECUTE_CALLBACK_H

#include <string>
#include <uv.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_parse_utils.h"
#include "nweb_value_callback.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {
enum class JsMessageType : int {
    NOTSUPPORT = 0,
    STRING,
    NUMBER,
    BOOLEAN,
    ARRAYBUFFER,
    ARRAY
};

class NapiJsMessageExt {
public:
    NapiJsMessageExt() = default;
    ~NapiJsMessageExt() = default;

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value GetType(napi_env env, napi_callback_info info);
    static napi_value GetString(napi_env env, napi_callback_info info);
    static napi_value GetNumber(napi_env env, napi_callback_info info);
    static napi_value GetBoolean(napi_env env, napi_callback_info info);
    static napi_value GetArrayBuffer(napi_env env, napi_callback_info info);
    static napi_value GetArray(napi_env env, napi_callback_info info);
};

class WebJsMessageExt {
public:
    explicit WebJsMessageExt(std::shared_ptr<NWebMessage> value) : value_(value) {};
    ~WebJsMessageExt() = default;

    int32_t ConvertToJsType(NWebValue::Type type);
    int32_t GetType();
    std::string GetString();
    double GetNumber();
    bool GetBoolean();
    std::shared_ptr<NWebMessage> GetJsMsgResult()
    {
        return value_;
    }

private:
    std::shared_ptr<NWebMessage> value_ = nullptr;
};

class WebviewJavaScriptExecuteCallback : public NWebMessageValueCallback {
public:
    explicit WebviewJavaScriptExecuteCallback(
        napi_env env, napi_ref callbackRef, napi_deferred deferred, bool extention)
        : env_(env), callbackRef_(callbackRef), deferred_(deferred), extention_(extention)
    {}
    ~WebviewJavaScriptExecuteCallback() = default;
    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;

private:
    struct JavaScriptExecuteParam {
        napi_env env_;
        napi_ref callbackRef_;
        napi_deferred deferred_;
        std::shared_ptr<NWebMessage> result_;
        bool extention_;
    };

    napi_env env_ = nullptr;
    napi_ref callbackRef_ = nullptr;
    napi_deferred deferred_ = nullptr;
    bool extention_ = false;

    static void UvAfterWorkCb(uv_work_t* work, int status);
    static void UvAfterWorkCbAsync(
        napi_env env, napi_ref callbackRef, std::shared_ptr<NWebMessage> result, bool extention);
    static void UvAfterWorkCbPromise(
        napi_env env, napi_deferred deferred, std::shared_ptr<NWebMessage> result, bool extention);

public:
    static void InitJSExcute(napi_env env, napi_value exports);
};

} // namespace OHOS::NWeb
#endif
