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

#ifndef JS_WEB_NATIVE_MESSAGING_EXTENSION_H
#define JS_WEB_NATIVE_MESSAGING_EXTENSION_H

#include "web_native_messaging_extension.h"
#include "js_runtime.h"
#include "native_engine/native_value.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
using namespace OHOS::AbilityRuntime;
class JsWebNativeMessagingExtension : public WebNativeMessagingExtension {
public:
    JsWebNativeMessagingExtension(JsRuntime& jsRuntime);
    ~JsWebNativeMessagingExtension() override;

    static JsWebNativeMessagingExtension *Create(const std::unique_ptr<Runtime>& runtime);

    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord>& record,
        const std::shared_ptr<AppExecFwk::OHOSApplication>& application,
        std::shared_ptr<AppExecFwk::AbilityHandler>& handler, const sptr<IRemoteObject>& token) override;

    sptr<IRemoteObject> OnConnect(const AAFwk::Want& want) override;

    void OnStop() override;

    virtual int32_t ConnectNative(WNMEConnectionInfo& connection) override;

    virtual int32_t DisconnectNative(WNMEConnectionInfo& connection) override;

private:
    class ConnectionManager {
        public:
            ConnectionManager() = default;
            ~ConnectionManager()
            {
                ClearAllConnections();
            }
            // 关闭 conn 持有的 fd（fdsan 校验所有权）并置 -1 标记已释放：fd 号可能被复用且
            // 新 fd 已接管同一 tag，fdsan 无法拦截同 tag 误关，置 -1 切断误用过期 fd 的路径
            // （幂等，可安全重复调用）。
            static void CloseConnectionFds(WNMEConnectionInfo& conn)
            {
                if (conn.fdRead >= 0) {
                    fdsan_close_with_tag(conn.fdRead, WNM_FD_OWNER_TAG);
                    conn.fdRead = -1;
                }
                if (conn.fdWrite >= 0) {
                    fdsan_close_with_tag(conn.fdWrite, WNM_FD_OWNER_TAG);
                    conn.fdWrite = -1;
                }
            }
            void ClearAllConnections()
            {
                for (auto& [id, conn] : connections_) {
                    CloseConnectionFds(conn);
                }
                connections_.clear();
            }
            void AddConnection(const WNMEConnectionInfo& conn)
            {
                auto tmp = GetConnection(conn.connectionId);
                if (tmp) {
                    WNMLOG_E("connectionId exists, closing old fds");
                    CloseConnectionFds(*tmp);
                }
                connections_[conn.connectionId] = conn;
            }
            void RemoveConnection(WNMEConnectionInfo& conn)
            {
                auto tmp = GetConnection(conn.connectionId);
                if (tmp) {
                    CloseConnectionFds(*tmp);
                } else {
                    WNMLOG_E("connectionId not exists!");
                }
                // 关闭本次 IPC 传入的 fd：它是独立 dup（上面关闭的是 map 中旧连接的 fd），不关则泄漏。
                // 若 conn 即 map 内对象，其 fd 已被置 -1，此处跳过，不会 Double Close。
                CloseConnectionFds(conn);
                // erase 放在关闭之后，避免误传 map 内对象引用时产生悬垂访问
                connections_.erase(conn.connectionId);
            }

            WNMEConnectionInfo* GetConnection(int32_t connectionId)
            {
                auto it = connections_.find(connectionId);
                return (it != connections_.end()) ? &it->second : nullptr;
            }
        private:
            std::map<int32_t, WNMEConnectionInfo> connections_;
    };

    void OnDestroy();
    void BindContext(napi_env env, napi_value obj);
    void InvokeCallback(const char* methodName, WNMEConnectionInfo& params);
    int32_t InvokeCallbackInMainThread(const std::string& methodName, WNMEConnectionInfo& params);
    void GetSrcPath(std::string& srcPath);

    JsRuntime& jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_ = nullptr;
    std::shared_ptr<NativeReference> shellContextRef_ = nullptr;
    sptr<IRemoteObject> providerRemoteObject_ = nullptr;
    ConnectionManager connmgr_;
};
} // namespace NWeb
} // namespace OHOS
#endif // JS_WEB_NATIVE_MESSAGING_EXTENSION_H
