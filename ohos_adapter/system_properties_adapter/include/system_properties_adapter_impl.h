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

#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include "system_properties_adapter.h"

namespace OHOS::NWeb {

class SystemPropertiesAdapterImpl : public SystemPropertiesAdapter {
public:
    static SystemPropertiesAdapterImpl& GetInstance();

    ~SystemPropertiesAdapterImpl() override;

    bool GetResourceUseHapPathEnable() override;

    std::string GetDeviceInfoProductModel() override;

    std::string GetDeviceInfoBrand() override;

    int32_t GetDeviceInfoMajorVersion() override;

    ProductDeviceType GetProductDeviceType() override;

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

    void AttachSysPropObserver(PropertiesKey key, SystemPropertiesObserver* observer) override;

    void DetachSysPropObserver(PropertiesKey key, SystemPropertiesObserver* observer) override;

    void DispatchAllWatcherInfo(const char* key, const char* value);

    bool GetBoolParameter(const std::string& key, bool defaultValue) override;

    std::vector<FrameRateSetting> GetLTPOConfig(const std::string& settingName) override;

    std::string GetOOPGPUStatus() override;

    std::string GetCompatibleDeviceType() override;
private:
    SystemPropertiesAdapterImpl();

    SystemPropertiesAdapterImpl(const SystemPropertiesAdapterImpl& other) = delete;

    SystemPropertiesAdapterImpl& operator=(const SystemPropertiesAdapterImpl&) = delete;

    ProductDeviceType AnalysisFromConfig();

    void AddAllSysPropWatchers();

    void RemoveAllSysPropWatchers();

    int softwareMajorVersion_ = -1;
    int softwareSeniorVersion_ = -1;
    std::string baseOsName_ ;

    std::unordered_map<PropertiesKey, std::vector<SystemPropertiesObserver*>> sysPropObserver_;
    std::unordered_map<PropertiesKey, std::shared_mutex> sysPropMutex_;
};

}  // namespace OHOS::NWeb

#endif  // SYSTEM_PROPERTIES_ADAPTER_IMPL_H
