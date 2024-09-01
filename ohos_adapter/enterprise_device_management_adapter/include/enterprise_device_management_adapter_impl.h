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

#ifndef ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER_IMPL_H
#define ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER_IMPL_H

#include "enterprise_device_management_adapter.h"

namespace OHOS::NWeb {

class EnterpriseDeviceManagementAdapterImpl : public EnterpriseDeviceManagementAdapter {
public:
    static EnterpriseDeviceManagementAdapterImpl& GetInstance();
    ~EnterpriseDeviceManagementAdapterImpl() override = default;
    int32_t GetPolicies(std::string& policies) override;

    /**
     * Set an EDM policy change event callback.
     *
     * @param eventCallback EDM policy change event callback.
     */
    void RegistPolicyChangeEventCallback(std::shared_ptr<EdmPolicyChangedEventCallbackAdapter> eventCallback) override {}

    /**
     * Subscribe EDM policy change event from CommonEventSubscriber.
     *
     * @return Returns true if success; false otherwise.
     */
    bool StartObservePolicyChange() override{ return false; }

    /**
     * Unsubscribe EDM policy change event from CommonEventSubscriber.
     *
     * @return Returns true if success; false otherwise.
     */
    bool StopObservePolicyChange() override{ return false; }

private:
    EnterpriseDeviceManagementAdapterImpl() = default;
    EnterpriseDeviceManagementAdapterImpl(const EnterpriseDeviceManagementAdapterImpl& other) = delete;
    EnterpriseDeviceManagementAdapterImpl& operator=(const EnterpriseDeviceManagementAdapterImpl&) = delete;
};

}  // namespace OHOS::NWeb

#endif  // ENTERPRISE_DEVICE_MANAGEMENT_ADAPTER_IMPL_H
