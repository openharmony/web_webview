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
#ifndef OHOS_NWEB_WEB_NATIVE_MESSAGING_CLIENT_H
#define OHOS_NWEB_WEB_NATIVE_MESSAGING_CLIENT_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>

#include "ability_connect_callback.h"
#include "iweb_native_messaging_service.h"
#include "singleton.h"
#include "start_options.h"
#include "want.h"
#include "web_native_messaging_common.h"

namespace OHOS::NWeb {
class WebNativeMessagingClient {
public:
    WebNativeMessagingClient();
    virtual ~WebNativeMessagingClient() = default;
    static WebNativeMessagingClient& GetInstance();

    void OnLoadSystemAbilitySuccess(const sptr<IRemoteObject>& object);
    void OnLoadSystemAbilityFail();
    void WebNativeMessagingOnRemoteDied(const wptr<IRemoteObject>& remoteObject);
    int ConnectWebNativeMessagingExtension(const sptr<IRemoteObject>& token, const AAFwk::Want& want,
        const sptr<IRemoteObject>& connectCallback, int32_t connectionId);
    int DisconnectWebNativeMessagingExtension(int32_t connectionId);
    int StartAbility(const sptr<IRemoteObject>& token, const AAFwk::Want& want, const AAFwk::StartOptions& options);
    int StopNativeConnectionFromExtension(int32_t connectionId);
    void SetUserDefineDiedRecipient(std::function<void()> deathCallback)
    {
        deathCallback_ = deathCallback;
    }

private:
    bool LoadNativeMessagingService();
    void SetWebNativeMessagingProxy(const sptr<IRemoteObject>& remoteObject);
    sptr<IWebNativeMessagingService> GetWebNativeMessaging();
    sptr<IWebNativeMessagingService> GetWebNativeMessagingProxy();
    class WebNativeMessagingDiedRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject>& remote) override;
    };

private:
    std::function<void()> deathCallback_;
    std::condition_variable loadSaCondition_;
    std::mutex loadSaMutex_;
    bool loadSaFinished_ { false };
    std::mutex mutex_;
    sptr<IWebNativeMessagingService> webNativeMessagingProxy_ = nullptr;
    sptr<WebNativeMessagingDiedRecipient> webNativeMessagingDiedRecipient_ = nullptr;
    DISALLOW_COPY_AND_MOVE(WebNativeMessagingClient);
};
} // namespace OHOS::NWeb
#endif // OHOS_NWEB_WEB_NATIVE_MESSAGING_CLIENT_H
