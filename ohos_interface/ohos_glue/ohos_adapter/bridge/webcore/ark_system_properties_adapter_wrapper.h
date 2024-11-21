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

#ifndef ARK_SYSTEM_PROPERTIES_ADAPTER_WRAPPER_H
#define ARK_SYSTEM_PROPERTIES_ADAPTER_WRAPPER_H
#pragma once

#include "adapter_base.h"
#include "ohos_adapter/include/ark_system_properties_adapter.h"
#include "system_properties_adapter.h"

namespace OHOS::ArkWeb {

class ArkSystemPropertiesAdapterWrapper : public OHOS::NWeb::SystemPropertiesAdapter {
public:
    explicit ArkSystemPropertiesAdapterWrapper(ArkWebRefPtr<ArkSystemPropertiesAdapter>);

    bool GetResourceUseHapPathEnable() override;

    std::string GetDeviceInfoProductModel() override;

    std::string GetDeviceInfoBrand() override;

    int32_t GetDeviceInfoMajorVersion() override;

    OHOS::NWeb::ProductDeviceType GetProductDeviceType() override;

    bool GetWebOptimizationValue() override;

    bool IsAdvancedSecurityMode() override;

    std::string GetUserAgentOSName() override;

    std::string GetUserAgentOSVersion() override;

    std::string GetUserAgentBaseOSName() override;

    int32_t GetSoftwareMajorVersion() override;

    int32_t GetSoftwareSeniorVersion() override;

    std::string GetNetlogMode() override;

    bool GetTraceDebugEnable() override;

    std::string GetSiteIsolationMode() override;

    int32_t GetFlowBufMaxFd() override;

    bool GetOOPGPUEnable() override;

    void SetOOPGPUDisable() override;

    void AttachSysPropObserver(NWeb::PropertiesKey, NWeb::SystemPropertiesObserver* observer) override;

    void DetachSysPropObserver(NWeb::PropertiesKey, NWeb::SystemPropertiesObserver* observer) override;

    bool GetBoolParameter(const std::string& key, bool defaultValue) override;

    std::vector<NWeb::FrameRateSetting> GetLTPOConfig(const std::string& settingName) override;

    std::string GetOOPGPUStatus() override;


    std::string GetCompatibleDeviceType() override;
private:
    ArkWebRefPtr<ArkSystemPropertiesAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_SYSTEM_PROPERTIES_ADAPTER_WRAPPER_H
