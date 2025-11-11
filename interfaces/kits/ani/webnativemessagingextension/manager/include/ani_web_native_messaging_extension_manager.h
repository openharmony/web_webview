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
#ifndef OHOS_NWEB_ANI_WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_H
#define OHOS_NWEB_ANI_WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_H

#include <uv.h>
#include "want.h"

#include "ani.h"
#include "ets_native_reference.h"
#include "event_handler.h"
#include "web_extension_connection_callback.h"
#include "web_native_messaging_log.h"


namespace OHOS {
namespace NWeb {

ani_status StsExtensionManagerInit(ani_env* env);

enum ConnectCallbackType : int32_t {
    ON_CONNECT_TYPE = 0,
    ON_DISCONNECT_TYPE = 1,
    ON_FAILED_TYPE = 2
};

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

struct CommonAsyncContext {
    explicit CommonAsyncContext(ani_env* env);
    virtual ~CommonAsyncContext();
    ani_vm* vm_ = nullptr;
    int32_t errCode = 0;
    ani_ref callbackRef = nullptr; // callback handle
};

struct ConnectNativeAsyncContext : public CommonAsyncContext {
    explicit ConnectNativeAsyncContext(ani_env* env) : CommonAsyncContext(env) {};
    sptr<IRemoteObject> token;
    AAFwk::Want want;
    sptr<WebExtensionConnectionCallback> connectCallback;
    int32_t connectId;
};

class AniExtensionConnectionCallback : public IExtensionConnectionCallback,
                                       public std::enable_shared_from_this<AniExtensionConnectionCallback> {
public:
    AniExtensionConnectionCallback(ani_env* env);
    virtual ~AniExtensionConnectionCallback()
    {
        WNMLOG_D("~AniExtensionConnectionCallback");
        DeleteCallInThread();
    }
    ani_env* GetEnv()
    {
        ani_env* env = nullptr;
        if (vm_) {
            vm_->GetEnv(ANI_VERSION_1, &env);
        }
        return env;
    }
    std::shared_ptr<AppExecFwk::ETSNativeReference> GetCallBack()
    {
        return callback_;
    }
    bool Register(ani_object object);
    void OnExtensionConnect(ConnectionNativeInfo& info) override;
    void OnExtensionDisconnect(ConnectionNativeInfo& info) override;
    void OnExtensionFailed(int32_t connectionId, int32_t errnum) override;

private:
    void DoExtensionConnectCallback(
        ConnectionNativeInfo& info, int32_t errorNum, ConnectCallbackType type);
    void DeleteCallInThread();
    ani_vm* vm_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> mainHandler_;
    std::shared_ptr<AppExecFwk::ETSNativeReference> callback_;
};

struct AniExtensionConnectionCallbackParam {
    ani_ref aniRef_;
    ConnectionNativeInfo result_;
    int32_t errorNum_;
    ConnectCallbackType type_;
};
} // namespace NWeb
} // namespace OHOS
#endif // OHOS_NWEB_ANI_WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_H