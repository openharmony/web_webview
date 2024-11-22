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

#include "ohos_adapter/bridge/ark_system_properties_adapter_wrapper.h"

namespace OHOS::ArkWeb {
using OHOS::NWeb::FrameRateSetting;
ArkSystemPropertiesAdapterWrapper::ArkSystemPropertiesAdapterWrapper(ArkWebRefPtr<ArkSystemPropertiesAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkSystemPropertiesAdapterWrapper::GetResourceUseHapPathEnable()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->GetResourceUseHapPathEnable();
}

std::string ArkSystemPropertiesAdapterWrapper::GetDeviceInfoProductModel()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetDeviceInfoProductModel();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkSystemPropertiesAdapterWrapper::GetDeviceInfoBrand()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetDeviceInfoBrand();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkSystemPropertiesAdapterWrapper::GetDeviceInfoMajorVersion()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->GetDeviceInfoMajorVersion();
}

OHOS::NWeb::ProductDeviceType ArkSystemPropertiesAdapterWrapper::GetProductDeviceType()
{
    if (!ctocpp_) {
        return OHOS::NWeb::ProductDeviceType::DEVICE_TYPE_UNKNOWN;
    }
    int32_t result = ctocpp_->GetProductDeviceType();
    return (OHOS::NWeb::ProductDeviceType)result;
}

bool ArkSystemPropertiesAdapterWrapper::GetWebOptimizationValue()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->GetWebOptimizationValue();
}

bool ArkSystemPropertiesAdapterWrapper::IsAdvancedSecurityMode()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsAdvancedSecurityMode();
}

std::string ArkSystemPropertiesAdapterWrapper::GetUserAgentOSName()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetUserAgentOSName();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkSystemPropertiesAdapterWrapper::GetUserAgentOSVersion()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetUserAgentOSVersion();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkSystemPropertiesAdapterWrapper::GetUserAgentBaseOSName()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetUserAgentBaseOSName();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkSystemPropertiesAdapterWrapper::GetSoftwareMajorVersion()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->GetSoftwareMajorVersion();
}

int32_t ArkSystemPropertiesAdapterWrapper::GetSoftwareSeniorVersion()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->GetSoftwareSeniorVersion();
}

std::string ArkSystemPropertiesAdapterWrapper::GetNetlogMode()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetNetlogMode();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

bool ArkSystemPropertiesAdapterWrapper::GetTraceDebugEnable()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->GetTraceDebugEnable();
}

std::string ArkSystemPropertiesAdapterWrapper::GetSiteIsolationMode()
{
    ArkWebString str = ctocpp_->GetSiteIsolationMode();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkSystemPropertiesAdapterWrapper::GetFlowBufMaxFd()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->GetFlowBufMaxFd();
}

bool ArkSystemPropertiesAdapterWrapper::GetOOPGPUEnable()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->GetOOPGPUEnable();
}

void ArkSystemPropertiesAdapterWrapper::SetOOPGPUDisable()
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->SetOOPGPUDisable();
}

void ArkSystemPropertiesAdapterWrapper::AttachSysPropObserver(
    NWeb::PropertiesKey key, NWeb::SystemPropertiesObserver* observer)
{
    ctocpp_->AttachSysPropObserver((int32_t)key, (void*)observer);
}

void ArkSystemPropertiesAdapterWrapper::DetachSysPropObserver(
    NWeb::PropertiesKey key, NWeb::SystemPropertiesObserver* observer)
{
    ctocpp_->DetachSysPropObserver((int32_t)key, (void*)observer);
}

bool ArkSystemPropertiesAdapterWrapper::GetBoolParameter(const std::string& key, bool defaultValue)
{
    ArkWebString str = ArkWebStringClassToStruct(key);
    bool result = ctocpp_->GetBoolParameter(str, defaultValue);
    ArkWebStringStructRelease(str);
    return result;
}

std::vector<FrameRateSetting> ArkSystemPropertiesAdapterWrapper::GetLTPOConfig(const std::string& settingName)
{
    if (!ctocpp_) {
        return {};
    }
    ArkWebString ark_setting_name = ArkWebStringClassToStruct(settingName);
    ArkFrameRateSettingAdapterVector ark_vector = ctocpp_->GetLTPOConfig(ark_setting_name);
    std::vector<FrameRateSetting> result {};

    for (int i = 0; i < ark_vector.size; i++) {
        result.emplace_back(FrameRateSetting {
            ark_vector.value[i].min_, ark_vector.value[i].max_, ark_vector.value[i].preferredFrameRate_ });
    }
    ArkWebStringStructRelease(ark_setting_name);
    SAFE_FREE(ark_vector.value, ark_vector.ark_web_mem_free_func);

    return result;
}

std::string ArkSystemPropertiesAdapterWrapper::GetOOPGPUStatus()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetOOPGPUStatus();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}
} // namespace OHOS::ArkWeb
