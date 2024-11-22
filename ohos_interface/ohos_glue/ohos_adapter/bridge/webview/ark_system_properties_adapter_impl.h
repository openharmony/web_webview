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

#ifndef ARK_SYSTEM_PROPERTIES_ADAPTER_IMPL_H
#define ARK_SYSTEM_PROPERTIES_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_system_properties_adapter.h"
#include "system_properties_adapter.h"

namespace OHOS::ArkWeb {

class ArkSystemPropertiesAdapterImpl : public ArkSystemPropertiesAdapter {
public:
    explicit ArkSystemPropertiesAdapterImpl(NWeb::SystemPropertiesAdapter&);

    bool GetResourceUseHapPathEnable() override;

    ArkWebString GetDeviceInfoProductModel() override;

    ArkWebString GetDeviceInfoBrand() override;

    int32_t GetDeviceInfoMajorVersion() override;

    int32_t GetProductDeviceType() override;

    bool GetWebOptimizationValue() override;

    bool IsAdvancedSecurityMode() override;

    ArkWebString GetUserAgentOSName() override;
    
    ArkWebString GetUserAgentOSVersion() override;
    
    ArkWebString GetUserAgentBaseOSName() override;

    int32_t GetSoftwareMajorVersion() override;

    int32_t GetSoftwareSeniorVersion() override;

    ArkWebString GetNetlogMode() override;

    bool GetTraceDebugEnable() override;

    ArkWebString GetSiteIsolationMode() override;

    bool GetOOPGPUEnable() override;

    void SetOOPGPUDisable() override;

    int32_t GetFlowBufMaxFd() override;

    void AttachSysPropObserver(int32_t key, void* observer) override;

    void DetachSysPropObserver(int32_t key, void* observer) override;

    bool GetBoolParameter(ArkWebString key, bool defaultValue) override;

    ArkFrameRateSettingAdapterVector GetLTPOConfig(const ArkWebString& settingName) override;

    ArkWebString GetOOPGPUStatus() override;

private:
    NWeb::SystemPropertiesAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkSystemPropertiesAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_SYSTEM_PROPERTIES_ADAPTER_IMPL_H
