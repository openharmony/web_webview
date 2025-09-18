/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License") override;
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

#ifndef ARK_OHOS_ADAPTER_HELPER_WRAPPER_H
#define ARK_OHOS_ADAPTER_HELPER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_ohos_adapter_helper.h"
#include "ohos_adapter_helper.h"

namespace OHOS::ArkWeb {

class ArkOhosAdapterHelperWrapper : public NWeb::OhosAdapterHelper {
public:
    explicit ArkOhosAdapterHelperWrapper(ArkWebRefPtr<ArkOhosAdapterHelper>);

    std::unique_ptr<NWeb::AafwkAppMgrClientAdapter> CreateAafwkAdapter() override;

    std::unique_ptr<NWeb::PowerMgrClientAdapter> CreatePowerMgrClientAdapter() override;

    std::unique_ptr<NWeb::DisplayManagerAdapter> CreateDisplayMgrAdapter() override;

    std::unique_ptr<NWeb::BatteryMgrClientAdapter> CreateBatteryClientAdapter() override;

    std::unique_ptr<NWeb::NetConnectAdapter> CreateNetConnectAdapter() override;

    NWeb::OhosWebDataBaseAdapter& GetOhosWebDataBaseAdapterInstance() override;

    NWeb::PasteBoardClientAdapter& GetPasteBoard() override;

    std::unique_ptr<NWeb::AudioRendererAdapter> CreateAudioRendererAdapter() override;

    std::unique_ptr<NWeb::AudioCapturerAdapter> CreateAudioCapturerAdapter() override;

    NWeb::AudioSystemManagerAdapter& GetAudioSystemManager() override;

    NWeb::OhosWebPermissionDataBaseAdapter& GetWebPermissionDataBaseInstance() override;

    std::unique_ptr<NWeb::MMIAdapter> CreateMMIAdapter() override;

    std::unique_ptr<NWeb::SocPerfClientAdapter> CreateSocPerfClientAdapter() override;

    std::unique_ptr<NWeb::OhosResourceAdapter> GetResourceAdapter(const std::string& hapPath) override;

    NWeb::SystemPropertiesAdapter& GetSystemPropertiesInstance() override;

    NWeb::VSyncAdapter& GetVSyncAdapter() override;

    std::unique_ptr<NWeb::OhosInitWebAdapter> GetInitWebAdapter() override;

    NWeb::KeystoreAdapter& GetKeystoreAdapterInstance() override;

    NWeb::EnterpriseDeviceManagementAdapter& GetEnterpriseDeviceManagementInstance() override;

    NWeb::DatashareAdapter& GetDatashareInstance() override;

    std::unique_ptr<NWeb::IMFAdapter> CreateIMFAdapter() override;

    std::unique_ptr<NWeb::CertManagerAdapter> GetRootCertDataAdapter() override;

    std::unique_ptr<NWeb::CertManagerAdapter> GetCertManagerAdapter() override;

    NWeb::AccessTokenAdapter& GetAccessTokenAdapterInstance() override;

    std::unique_ptr<NWeb::EventHandlerAdapter> GetEventHandlerAdapter() override;

    NWeb::PrintManagerAdapter& GetPrintManagerInstance() override;

    std::unique_ptr<NWeb::IConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() override;

    std::unique_ptr<NWeb::PlayerAdapter> CreatePlayerAdapter() override;

    NWeb::WindowAdapter& GetWindowAdapterInstance() override;

    NWeb::HiSysEventAdapter& GetHiSysEventAdapterInstance() override;

    NWeb::HiTraceAdapter& GetHiTraceAdapterInstance() override;

    NWeb::NetProxyAdapter& GetNetProxyInstance() override;

    NWeb::CameraManagerAdapter& GetCameraManagerAdapter() override;

    std::unique_ptr<NWeb::ScreenCaptureAdapter> CreateScreenCaptureAdapter() override;

    std::unique_ptr<NWeb::DateTimeFormatAdapter> CreateDateTimeFormatAdapter() override;

    std::unique_ptr<NWeb::MediaCodecDecoderAdapter> CreateMediaCodecDecoderAdapter() override;

    std::unique_ptr<NWeb::NativeImageAdapter> CreateNativeImageAdapter() override;

    std::unique_ptr<NWeb::MediaCodecAdapter> CreateMediaCodecEncoderAdapter() override;

    NWeb::MediaCodecListAdapter& GetMediaCodecListAdapter() override;

    std::unique_ptr<NWeb::FlowbufferAdapter> CreateFlowbufferAdapter() override;

    std::unique_ptr<NWeb::MediaAVSessionAdapter> CreateMediaAVSessionAdapter() override;

    std::unique_ptr<NWeb::OhosImageDecoderAdapter> CreateOhosImageDecoderAdapter() override;

    std::unique_ptr<NWeb::SensorAdapter> CreateSensorAdapter() override;

    void SetArkWebCoreHapPathOverride(const std::string& hapPath) override;

    NWeb::OhosNativeBufferAdapter& GetOhosNativeBufferAdapter() override;

    std::unique_ptr<NWeb::MigrationManagerAdapter> CreateMigrationMgrAdapter() override;

    NWeb::OhosDrawingTextFontAdapter& GetOhosDrawingTextFontAdapter() override;

    NWeb::OhosDrawingTextTypographyAdapter& GetOhosDrawingTextTypographyAdapter() override;

    std::unique_ptr<NWeb::AudioCodecDecoderAdapter> CreateAudioCodecDecoderAdapter() override;

    std::unique_ptr<NWeb::DrmAdapter> CreateDrmAdapter() override;

    std::unique_ptr<NWeb::ScreenlockManagerAdapter> CreateScreenlockManagerAdapter() override;

private:
    ArkWebRefPtr<ArkOhosAdapterHelper> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // OHOS_ADAPTER_HELPER_H
