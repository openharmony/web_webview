/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "enterprise_device_management_adapter_impl.h"

#include "nweb_log.h"

#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
#include "browser_proxy.h"
#endif

namespace OHOS::NWeb {
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
using namespace OHOS::EDM;
#endif

namespace {
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
const char* const BROWSER_POLICY_CHANGED_EVENT = "com.ohos.edm.browserpolicychanged";
#endif
}

// static
EnterpriseDeviceManagementAdapterImpl& EnterpriseDeviceManagementAdapterImpl::GetInstance()
{
    static EnterpriseDeviceManagementAdapterImpl instance;
    return instance;
}

#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
NWebEdmEventSubscriber::NWebEdmEventSubscriber(
    EventFwk::CommonEventSubscribeInfo& in,
    std::shared_ptr<EdmPolicyChangedEventCallbackAdapter> eventCallback)
    : EventFwk::CommonEventSubscriber(in), eventCallback_(eventCallback) {}

void NWebEdmEventSubscriber::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    const std::string action = data.GetWant().GetAction();
    WVLOG_I("Receive edm policy action: %{public}s", action.c_str());
    if (action != BROWSER_POLICY_CHANGED_EVENT) {
        return;
    }

    if (eventCallback_) {
        eventCallback_->Changed();
    }
}
#endif

void EnterpriseDeviceManagementAdapterImpl::RegistPolicyChangeEventCallback(
    std::shared_ptr<EdmPolicyChangedEventCallbackAdapter> eventCallback)
{
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
    WVLOG_I("Regist edm policy change event callback");
    eventCallback_ = std::move(eventCallback);
#endif
}

bool EnterpriseDeviceManagementAdapterImpl::StartObservePolicyChange()
{
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
    WVLOG_I("Start observing edm policy change event");

    EventFwk::MatchingSkills skill = EventFwk::MatchingSkills();
    skill.AddEvent(BROWSER_POLICY_CHANGED_EVENT);
    EventFwk::CommonEventSubscribeInfo info(skill);
    int32_t systemEdmUid = 3057;
    info.SetPublisherUid(systemEdmUid);
    this->commonEventSubscriber_ = std::make_shared<NWebEdmEventSubscriber>(info, this->eventCallback_);
    bool ret = EventFwk::CommonEventManager::SubscribeCommonEvent(this->commonEventSubscriber_);
    if (ret == false) {
        WVLOG_E("Start observing edm policy failed");
    }
    return ret;
#else
    return false;
#endif
}

bool EnterpriseDeviceManagementAdapterImpl::StopObservePolicyChange()
{
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
    WVLOG_I("Stop observing edm policy change event");

    if (this->commonEventSubscriber_ != nullptr) {
        bool ret = EventFwk::CommonEventManager::UnSubscribeCommonEvent(this->commonEventSubscriber_);
        if (ret == false) {
            WVLOG_E("Stop observing edm policy change failed");
        }
        return ret;
    }
    return true;
#else
    return true;
#endif
}

int32_t EnterpriseDeviceManagementAdapterImpl::GetPolicies(std::string& policies)
{
#if defined(NWEB_ENTERPRISE_DEVICE_MANAGER_ENABLE)
    auto proxy = BrowserProxy::GetBrowserProxy();
    if (!proxy) {
        WVLOG_E("EnterpriseDeviceManagementAdapterImpl BrowserProxy is null");
        return -1;
    }
    return proxy->GetPolicies(policies);
#else
    WVLOG_I("Enterprise device management not supported.");
    return -1;
#endif
}

} // namespace OHOS::NWeb
