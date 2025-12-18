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

#ifndef SYSTEM_PROPERTIES_ADAPTER_H
#define SYSTEM_PROPERTIES_ADAPTER_H

#include <cstdint>
#include <string>

namespace OHOS::NWeb {

struct FrameRateSetting;

enum class ProductDeviceType : int32_t {
    DEVICE_TYPE_MOBILE,
    DEVICE_TYPE_TABLET,
    DEVICE_TYPE_2IN1,
    DEVICE_TYPE_WEARABLE,
    DEVICE_TYPE_TV,
    DEVICE_TYPE_UNKNOWN
};

enum class PropertiesKey : int32_t { PROP_RENDER_DUMP, PROP_DEBUG_TRACE, PROP_HITRACE_ENABLEFLAGS};

class SystemPropertiesObserver {
public:
    SystemPropertiesObserver() = default;

    virtual ~SystemPropertiesObserver() = default;

    virtual void PropertiesUpdate(const char* value) = 0;
};

class SystemPropertiesAdapter {
public:
    SystemPropertiesAdapter() = default;

    virtual ~SystemPropertiesAdapter() = default;

    virtual bool GetResourceUseHapPathEnable() = 0;

    virtual std::string GetDeviceInfoProductModel() = 0;

    virtual std::string GetDeviceInfoBrand() = 0;

    virtual int32_t GetDeviceInfoMajorVersion() = 0;

    virtual ProductDeviceType GetProductDeviceType() = 0;

    virtual bool GetWebOptimizationValue() = 0;

    virtual bool IsAdvancedSecurityMode() = 0;

    virtual std::string GetUserAgentOSName() = 0;

    virtual int32_t GetSoftwareMajorVersion() = 0;

    virtual int32_t GetSoftwareSeniorVersion() = 0;

    virtual std::string GetNetlogMode() = 0;

    virtual bool GetTraceDebugEnable() = 0;

    virtual std::string GetSiteIsolationMode() = 0;

    virtual int32_t GetFlowBufMaxFd() = 0;

    virtual bool GetOOPGPUEnable() = 0;

    virtual void SetOOPGPUDisable() = 0;

    virtual void AttachSysPropObserver(PropertiesKey key, SystemPropertiesObserver* observer) = 0;

    virtual void DetachSysPropObserver(PropertiesKey key, SystemPropertiesObserver* observer) = 0;

    virtual bool GetBoolParameter(const std::string& key, bool defaultValue) = 0;

    virtual std::vector<FrameRateSetting> GetLTPOConfig(const std::string& settingName) = 0;

    virtual std::string GetOOPGPUStatus() = 0;

    virtual bool IsLTPODynamicApp(const std::string& bundleName) = 0;

    virtual int32_t GetLTPOStrategy() = 0;

    virtual std::string GetUserAgentBaseOSName() = 0;

    virtual std::string GetUserAgentOSVersion() = 0;

    virtual std::string GetVulkanStatus() = 0;

    virtual std::string GetCompatibleDeviceType() = 0;

    virtual std::string GetDeviceInfoApiVersion() = 0;

    virtual std::string GetPRPPreloadMode() = 0;

    virtual std::string GetScrollVelocityScale() = 0;

    virtual std::string GetScrollFriction() = 0;

    virtual std::string GetBundleName() = 0;

    virtual std::string GetStringParameter(const std::string& key, const std::string& defaultValue) = 0;

    virtual int32_t GetInitialCongestionWindowSize() = 0;

    virtual int32_t GetIntParameter(const std::string& key, int32_t defaultValue) = 0;

    virtual std::string GetTopPriorityConfigPath(const std::string& relativePath) = 0;

    virtual std::vector<std::string> GetConfigPathsInPriorityOrder(const std::string& relativePath) = 0;
};

} // namespace OHOS::NWeb

#endif // SYSTEM_PROPERTIES_ADAPTER_H
