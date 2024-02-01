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

#ifndef SYSTEM_PROPERTIES_ADAPTER_IMPL_H
#define SYSTEM_PROPERTIES_ADAPTER_IMPL_H

#include "system_properties_adapter.h"

namespace OHOS::NWeb {

class SystemPropertiesAdapterImpl : public SystemPropertiesAdapter {
public:
    static SystemPropertiesAdapterImpl& GetInstance();

    ~SystemPropertiesAdapterImpl() override = default;

    bool GetResourceUseHapPathEnable() const override;

    std::string GetDeviceInfoProductModel() const override;

    std::string GetDeviceInfoBrand() const override;

    int32_t GetDeviceInfoMajorVersion() const override;

    ProductDeviceType GetProductDeviceType() const override;

    bool GetWebOptimizationValue() const override;

    bool GetLockdownModeStatus() const override;

    std::string GetUserAgentOSName() const override;

    int32_t GetSoftwareMajorVersion() const override;

    int32_t GetSoftwareSeniorVersion() const override;

    std::string GetNetlogMode() const override;

    bool GetTraceDebugEnable() const override;
private:
    SystemPropertiesAdapterImpl();

    SystemPropertiesAdapterImpl(const SystemPropertiesAdapterImpl& other) = delete;

    SystemPropertiesAdapterImpl& operator=(const SystemPropertiesAdapterImpl&) = delete;
    int softwareMajorVersion_ = 4;
    int softwareSeniorVersion_ = 1;
};

}  // namespace OHOS::NWeb

#endif  // SYSTEM_PROPERTIES_ADAPTER_IMPL_H