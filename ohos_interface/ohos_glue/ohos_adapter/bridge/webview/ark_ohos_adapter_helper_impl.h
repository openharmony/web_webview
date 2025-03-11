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

#ifndef ARK_OHOS_ADAPTER_HELPER_IMPL_H
#define ARK_OHOS_ADAPTER_HELPER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_adapter_helper.h"
#include "ohos_adapter_helper.h"

namespace OHOS::ArkWeb {

class ArkOhosAdapterHelperImpl : public ArkOhosAdapterHelper {
public:
    explicit ArkOhosAdapterHelperImpl(NWeb::OhosAdapterHelper&);

    ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> CreateAafwkAdapter() override;

    ArkWebRefPtr<ArkPowerMgrClientAdapter> CreatePowerMgrClientAdapter() override;

    ArkWebRefPtr<ArkDisplayManagerAdapter> CreateDisplayMgrAdapter() override;

    ArkWebRefPtr<ArkBatteryMgrClientAdapter> CreateBatteryClientAdapter() override;

    ArkWebRefPtr<ArkNetConnectAdapter> CreateNetConnectAdapter() override;

    ArkWebRefPtr<ArkOhosWebDataBaseAdapter> GetOhosWebDataBaseAdapterInstance() override;

    ArkWebRefPtr<ArkPasteBoardClientAdapter> GetPasteBoard() override;

    ArkWebRefPtr<ArkAudioRendererAdapter> CreateAudioRendererAdapter() override;

    ArkWebRefPtr<ArkAudioCapturerAdapter> CreateAudioCapturerAdapter() override;

    ArkWebRefPtr<ArkAudioSystemManagerAdapter> GetAudioSystemManager() override;

    ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter> GetWebPermissionDataBaseInstance() override;

    ArkWebRefPtr<ArkMMIAdapter> CreateMMIAdapter() override;

    ArkWebRefPtr<ArkSocPerfClientAdapter> CreateSocPerfClientAdapter() override;

    ArkWebRefPtr<ArkOhosResourceAdapter> GetResourceAdapter(const ArkWebString& hapPath) override;

    ArkWebRefPtr<ArkSystemPropertiesAdapter> GetSystemPropertiesInstance() override;

    ArkWebRefPtr<ArkVSyncAdapter> GetVSyncAdapter() override;

    ArkWebRefPtr<ArkOhosInitWebAdapter> GetInitWebAdapter() override;

    ArkWebRefPtr<ArkKeystoreAdapter> GetKeystoreAdapterInstance() override;

    ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter> GetEnterpriseDeviceManagementInstance() override;

    ArkWebRefPtr<ArkDatashareAdapter> GetDatashareInstance() override;

    ArkWebRefPtr<ArkIMFAdapter> CreateIMFAdapter() override;

    ArkWebRefPtr<ArkCertManagerAdapter> GetRootCertDataAdapter() override;

    ArkWebRefPtr<ArkAccessTokenAdapter> GetAccessTokenAdapterInstance() override;

    ArkWebRefPtr<ArkEventHandlerAdapter> GetEventHandlerAdapter() override;

    ArkWebRefPtr<ArkPrintManagerAdapter> GetPrintManagerInstance() override;

    ArkWebRefPtr<ArkIConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() override;

    ArkWebRefPtr<ArkPlayerAdapter> CreatePlayerAdapter() override;

    ArkWebRefPtr<ArkWindowAdapter> GetWindowAdapterInstance() override;

    ArkWebRefPtr<ArkHiSysEventAdapter> GetHiSysEventAdapterInstance() override;

    ArkWebRefPtr<ArkHiTraceAdapter> GetHiTraceAdapterInstance() override;

    ArkWebRefPtr<ArkNetProxyAdapter> GetNetProxyInstance() override;

    ArkWebRefPtr<ArkCameraManagerAdapter> GetCameraManagerAdapter() override;

    ArkWebRefPtr<ArkScreenCaptureAdapter> CreateScreenCaptureAdapter() override;

    ArkWebRefPtr<ArkDateTimeFormatAdapter> CreateDateTimeFormatAdapter() override;

    ArkWebRefPtr<ArkMediaCodecDecoderAdapter> CreateMediaCodecDecoderAdapter() override;

    ArkWebRefPtr<ArkNativeImageAdapter> CreateNativeImageAdapter() override;

    ArkWebRefPtr<ArkMediaCodecAdapter> CreateMediaCodecEncoderAdapter() override;

    ArkWebRefPtr<ArkMediaCodecListAdapter> GetMediaCodecListAdapter() override;

    ArkWebRefPtr<ArkFlowbufferAdapter> CreateFlowbufferAdapter() override;

    ArkWebRefPtr<ArkMediaAVSessionAdapter> CreateMediaAVSessionAdapter() override;

    ArkWebRefPtr<ArkOhosImageDecoderAdapter> CreateOhosImageDecoderAdapter() override;

    ArkWebRefPtr<ArkSensorAdapter> CreateSensorAdapter() override;

    void SetArkWebCoreHapPathOverride(const ArkWebString& hapPath) override;

    ArkWebRefPtr<ArkOhosNativeBufferAdapter> GetOhosNativeBufferAdapter() override;

    ArkWebRefPtr<ArkOhosDrawingTextFontAdapter> GetOhosDrawingTextFontAdapter() override;

    ArkWebRefPtr<ArkOhosDrawingTextTypographyAdapter> GetOhosDrawingTextTypographyAdapter() override;

private:
    NWeb::OhosAdapterHelper& real_;

    IMPLEMENT_REFCOUNTING(ArkOhosAdapterHelperImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_ADAPTER_HELPER_IMPL_H
