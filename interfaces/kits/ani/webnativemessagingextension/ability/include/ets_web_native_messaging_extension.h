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

#ifndef ETS_WEB_NATIVE_MESSAGING_EXTENSION_H
#define ETS_WEB_NATIVE_MESSAGING_EXTENSION_H

#include "web_native_messaging_extension.h"
#include "ets_runtime.h"
#include "ets_native_reference.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
using namespace OHOS::AbilityRuntime;

class ETSWebNativeMessagingExtension : public WebNativeMessagingExtension {
public:
    ETSWebNativeMessagingExtension(ETSRuntime &etsRuntime);
    ~ETSWebNativeMessagingExtension() override;

    static ETSWebNativeMessagingExtension *Create(const std::unique_ptr<Runtime>& runtime);

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
                for (const auto& [id, conn] : connections_) {
                    close(conn.fdRead);
                    close(conn.fdWrite);
                }
            }
            void AddConnection(const WNMEConnectionInfo& conn)
            {
                auto tmp = GetConnection(conn.connectionId);
                if (tmp) {
                    WNMLOG_E("connectionId exists!");
                }
                connections_[conn.connectionId] = conn;
            }
            void RemoveConnection(const WNMEConnectionInfo& conn)
            {
                auto tmp = GetConnection(conn.connectionId);
                if (tmp) {
                    close(tmp->fdRead);
                    close(tmp->fdWrite);
                    connections_.erase(conn.connectionId);
                    return;
                }
                WNMLOG_E("connectionId not exists!");
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
    void BindContext(ani_env* env, ani_object obj);
    void InvokeCallback(const char* methodName, const WNMEConnectionInfo& params);
    int32_t InvokeCallbackInMainThread(const std::string& methodName, WNMEConnectionInfo& params);
    void GetSrcPath(std::string& srcPath);

    ETSRuntime& etsRuntime_;
    std::unique_ptr<AppExecFwk::ETSNativeReference> etsObj_;
    sptr<IRemoteObject> providerRemoteObject_;
    ConnectionManager connmgr_;
};
} // namespace NWeb
} // namespace OHOS
#endif // ETS_WEB_NATIVE_MESSAGING_EXTENSION_H