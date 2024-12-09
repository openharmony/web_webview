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

#ifndef ARK_SYSTEM_PROPERTIES_ADAPTER_H
#define ARK_SYSTEM_PROPERTIES_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_frame_rate_setting_adapter_vector.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkSystemPropertiesAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool GetResourceUseHapPathEnable() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetDeviceInfoProductModel() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetDeviceInfoBrand() = 0;

    /*--ark web()--*/
    virtual int32_t GetDeviceInfoMajorVersion() = 0;

    /*--ark web()--*/
    virtual int32_t GetProductDeviceType() = 0;

    /*--ark web()--*/
    virtual bool GetWebOptimizationValue() = 0;

    /*--ark web()--*/
    virtual bool IsAdvancedSecurityMode() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetUserAgentOSName() = 0;

    /*--ark web()--*/
    virtual int32_t GetSoftwareMajorVersion() = 0;

    /*--ark web()--*/
    virtual int32_t GetSoftwareSeniorVersion() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetNetlogMode() = 0;

    /*--ark web()--*/
    virtual bool GetTraceDebugEnable() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetSiteIsolationMode() = 0;

    /*--ark web()--*/
    virtual int32_t GetFlowBufMaxFd() = 0;

    /*--ark web()--*/
    virtual bool GetOOPGPUEnable() = 0;

    /*--ark web()--*/
    virtual void SetOOPGPUDisable() = 0;

    /*--ark web()--*/
    virtual void AttachSysPropObserver(int32_t key, void* observer) = 0;

    /*--ark web()--*/
    virtual void DetachSysPropObserver(int32_t key, void* observer) = 0;

    /*--ark web()--*/
    virtual bool GetBoolParameter(ArkWebString key, bool defaultValue) = 0;

    /*--ark web()--*/
    virtual ArkFrameRateSettingAdapterVector GetLTPOConfig(const ArkWebString& settingName) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetOOPGPUStatus() = 0;

    /*--ark web()--*/
    virtual bool IsLTPODynamicApp(const ArkWebString& bundleName) = 0;

    /*--ark web()--*/
    virtual int32_t GetLTPOStrategy() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetUserAgentOSVersion() = 0;
    
    /*--ark web()--*/
    virtual ArkWebString GetUserAgentBaseOSName() = 0;

    /**
     * @Description: return VulkanStatus.
     * @Return：return ArkWebString to describe VulkanStatus.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual ArkWebString GetVulkanStatus() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetPRPPreloadMode() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_SYSTEM_PROPERTIES_ADAPTER_H
