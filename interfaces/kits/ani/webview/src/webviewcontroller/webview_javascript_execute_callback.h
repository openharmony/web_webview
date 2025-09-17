/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ani.h"
#include "event_handler.h"

namespace OHOS::NWeb {
enum class JsMessageType : int {
    NOTSUPPORT = 0,
    STRING,
    NUMBER,
    BOOLEAN,
    ARRAYBUFFER,
    ARRAY
};

ani_ref CreateStsError(ani_env* env, ani_int code, const std::string& msg);

class WebviewJavaScriptExecuteCallback : public std::enable_shared_from_this<WebviewJavaScriptExecuteCallback>,
                                         public NWebMessageValueCallback {
public:
    explicit WebviewJavaScriptExecuteCallback(ani_env* env, ani_ref callbackRef, ani_resolver resolver, bool extension);
    ~WebviewJavaScriptExecuteCallback();
    void SetJavaScriptCallBackRef(ani_object callback);
    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;
    ani_env* GetEnv() {
        ani_env* env = nullptr;
        if (vm_) {
            vm_->GetEnv(ANI_VERSION_1, &env);
        }
        return env;
    }

    ani_ref GetCallBackRef() {
        return callbackRef_;
    }

    bool GetExtension()
    {
        return extension_;
    }
    
    ani_resolver GetResolver() {
        return resolver_;
    }

private:
    ani_vm* vm_ = nullptr;
    ani_ref callbackRef_ = nullptr;
    ani_resolver resolver_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    bool extension_ = false;
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

ani_status StsJsMessageExtInit(ani_env* env);

} // namespace OHOS::NWeb
#endif