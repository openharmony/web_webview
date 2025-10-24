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

#ifndef NWEB_NAPI_WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_H
#define NWEB_NAPI_WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "want.h"
#include "web_extension_connection_callback.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
struct JsConnectCallback {
    napi_ref onConnectMethodRef = nullptr;
    napi_ref onDisconnectMethodRef = nullptr;
    napi_ref onFailedMethodRef = nullptr;
};

enum ConnectCallbackType : int32_t {
    ON_CONNECT_TYPE = 0,
    ON_DISCONNECT_TYPE = 1,
    ON_FAILED_TYPE = 2
};

const std::string NATIVE_MESSAGING_ERROR_CODE_ENUM_NAME = "NmErrorCode";

namespace NmErrorMsg {
const std::string INNER_ERR_MSG = "Inner error for native messaging. Error code:";
const std::string WANT_CONTENT_ERR_MSG = "The want content is invalid.";
const std::string PERMISSION_DENY_ERR_MSG =
    "Permission denied due to missing ohos.permission.WEB_NATIVE_MESSAGING.";
}

enum class NmErrorCode : int {
    INNER_ERROR = 17100201,
    WANT_CONTENT_ERROR = 17100202,
    PERMISSION_DENY = 17100203,
};

class NapiExtensionConnectionCallback : public IExtensionConnectionCallback,
                                        public std::enable_shared_from_this<NapiExtensionConnectionCallback> {
public:
    NapiExtensionConnectionCallback(napi_env env, JsConnectCallback jsCallback) :
        env_(env), jsCallback_(jsCallback) {};
    virtual ~NapiExtensionConnectionCallback()
    {
        WNMLOG_D("~NapiExtensionConnectionCallback");
        DeleteJsCallInJsThread();
    }
    void OnExtensionConnect(ConnectionNativeInfo& info) override;
    void OnExtensionDisconnect(ConnectionNativeInfo& info) override;
    void OnExtensionFailed(int32_t connectionId, int32_t errorNum) override;

private:
    void DoJsExtensionConnectCallback(
        ConnectionNativeInfo& info, int32_t errorNum, ConnectCallbackType type);
    void DeleteJsCallInJsThread();
    napi_env env_;
    JsConnectCallback jsCallback_;
};

struct DeleteJsCallParam {
    napi_env env_;
    JsConnectCallback jsCallback_;
};

struct NapiExtensionConnectionCallbackParam {
    napi_env env_;
    JsConnectCallback callback_;
    ConnectionNativeInfo result_;
    int32_t errorNum_;
    ConnectCallbackType type_;
    std::shared_ptr<NapiExtensionConnectionCallback> caller;
};

struct CommonAsyncContext {
    explicit CommonAsyncContext(napi_env napiEnv);
    virtual ~CommonAsyncContext();
    napi_env env = nullptr;
    napi_status status = napi_invalid_arg;
    int32_t errCode = 0;
    napi_deferred deferred = nullptr; // promise handle
    napi_ref callbackRef = nullptr; // callback handle
    napi_async_work work = nullptr; // work handle
};

struct ConnectNativeAsyncContext : public CommonAsyncContext {
    explicit ConnectNativeAsyncContext(napi_env env) : CommonAsyncContext(env) {};
    sptr<IRemoteObject> token;
    AAFwk::Want want;
    sptr<WebExtensionConnectionCallback> connectCallback;
    int32_t connectId = 0;
};

struct DisconnectNativeAsyncContext : public CommonAsyncContext {
    explicit DisconnectNativeAsyncContext(napi_env env) : CommonAsyncContext(env) {};
    int32_t connectId = 0;
};

class NapiWebNativeMessagingExtensionManager {
public:
    NapiWebNativeMessagingExtensionManager() {}
    ~NapiWebNativeMessagingExtensionManager() = default;

    static void Init(napi_env env, napi_value exports);
    static napi_value ConnectNative(napi_env env, napi_callback_info info);
    static napi_value DisconnectNative(napi_env env, napi_callback_info info);

    static void ConnectNativeExcute(napi_env env, void* data);
    static void ConnectNativeComplete(napi_env env, napi_status status, void* data);
    static void DisconnectNativeExcute(napi_env env, void* data);
    static void DisconnectNativeComplete(napi_env env, napi_status status, void* data);
};

} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_H
