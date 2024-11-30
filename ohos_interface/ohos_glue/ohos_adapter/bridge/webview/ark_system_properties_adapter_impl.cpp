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

#include "ohos_adapter/bridge/ark_system_properties_adapter_impl.h"

#include "include/nweb_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkSystemPropertiesAdapterImpl::ArkSystemPropertiesAdapterImpl(NWeb::SystemPropertiesAdapter& ref) : real_(ref) {}

bool ArkSystemPropertiesAdapterImpl::GetResourceUseHapPathEnable()
{
    return real_.GetResourceUseHapPathEnable();
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetDeviceInfoProductModel()
{
    std::string str = real_.GetDeviceInfoProductModel();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetDeviceInfoBrand()
{
    std::string str = real_.GetDeviceInfoBrand();
    return ArkWebStringClassToStruct(str);
}

int32_t ArkSystemPropertiesAdapterImpl::GetDeviceInfoMajorVersion()
{
    return real_.GetDeviceInfoMajorVersion();
}

int32_t ArkSystemPropertiesAdapterImpl::GetProductDeviceType()
{
    return (int32_t)real_.GetProductDeviceType();
}

bool ArkSystemPropertiesAdapterImpl::GetWebOptimizationValue()
{
    return real_.GetWebOptimizationValue();
}

bool ArkSystemPropertiesAdapterImpl::IsAdvancedSecurityMode()
{
    return real_.IsAdvancedSecurityMode();
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetUserAgentOSName()
{
    std::string str = real_.GetUserAgentOSName();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetUserAgentOSVersion()
{
    std::string str = real_.GetUserAgentOSVersion();
    return ArkWebStringClassToStruct(str);
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetUserAgentBaseOSName()
{
    std::string str = real_.GetUserAgentBaseOSName();
    return ArkWebStringClassToStruct(str);
}

int32_t ArkSystemPropertiesAdapterImpl::GetSoftwareMajorVersion()
{
    return real_.GetSoftwareMajorVersion();
}

int32_t ArkSystemPropertiesAdapterImpl::GetSoftwareSeniorVersion()
{
    return real_.GetSoftwareSeniorVersion();
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetNetlogMode()
{
    std::string str = real_.GetNetlogMode();
    return ArkWebStringClassToStruct(str);
}

bool ArkSystemPropertiesAdapterImpl::GetTraceDebugEnable()
{
    return real_.GetTraceDebugEnable();
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetSiteIsolationMode()
{
    std::string str = real_.GetSiteIsolationMode();
    return ArkWebStringClassToStruct(str);
}

int32_t ArkSystemPropertiesAdapterImpl::GetFlowBufMaxFd()
{
    return real_.GetFlowBufMaxFd();
}

bool ArkSystemPropertiesAdapterImpl::GetOOPGPUEnable()
{
    return real_.GetOOPGPUEnable();
}

void ArkSystemPropertiesAdapterImpl::SetOOPGPUDisable()
{
    real_.SetOOPGPUDisable();
}

void ArkSystemPropertiesAdapterImpl::AttachSysPropObserver(int32_t key, void* observer)
{
    real_.AttachSysPropObserver((NWeb::PropertiesKey)key, (NWeb::SystemPropertiesObserver*)observer);
}

void ArkSystemPropertiesAdapterImpl::DetachSysPropObserver(int32_t key, void* observer)
{
    real_.DetachSysPropObserver((NWeb::PropertiesKey)key, (NWeb::SystemPropertiesObserver*)observer);
}

bool ArkSystemPropertiesAdapterImpl::GetBoolParameter(ArkWebString key, bool defaultValue)
{
    return real_.GetBoolParameter(ArkWebStringStructToClass(key), defaultValue);
}

ArkFrameRateSettingAdapterVector ArkSystemPropertiesAdapterImpl::GetLTPOConfig(const ArkWebString& settingName)
{
    std::vector<NWeb::FrameRateSetting> frameRateSettingVector =
        real_.GetLTPOConfig(ArkWebStringStructToClass(settingName));
    ArkFrameRateSettingAdapterVector result = { .size = frameRateSettingVector.size(),
        .ark_web_mem_free_func = ArkWebMemFree };
    if (result.size > 0) {
        result.value = (ArkFrameRateSettingAdapter*)ArkWebMemMalloc(sizeof(ArkFrameRateSettingAdapter) * result.size);
        if (result.value == nullptr) {
            result.size = 0;
            return result;
        }

        int count = 0;
        for (auto it = frameRateSettingVector.begin(); it != frameRateSettingVector.end(); it++) {
            result.value[count] = { it->min_, it->max_, it->preferredFrameRate_ };
            count++;
        }
    }

    return result;
}

ArkWebString ArkSystemPropertiesAdapterImpl::GetOOPGPUStatus()
{
    std::string str = real_.GetOOPGPUStatus();
    return ArkWebStringClassToStruct(str);
}

} // namespace OHOS::ArkWeb
