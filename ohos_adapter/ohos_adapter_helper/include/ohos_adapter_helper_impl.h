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

#ifndef OHOS_ADAPTER_HELPER_IMPL_H
#define OHOS_ADAPTER_HELPER_IMPL_H

#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
class OhosAdapterHelperImpl : public OhosAdapterHelper {
public:
    OhosAdapterHelperImpl() = default;

    OhosAdapterHelperImpl(const OhosAdapterHelperImpl&) = delete;

    OhosAdapterHelperImpl& operator=(const OhosAdapterHelperImpl&) = delete;

    ~OhosAdapterHelperImpl() = default;

    std::unique_ptr<AafwkAppMgrClientAdapter> CreateAafwkAdapter() override;

    std::unique_ptr<PowerMgrClientAdapter> CreatePowerMgrClientAdapter() override;

    std::unique_ptr<DisplayManagerAdapter> CreateDisplayMgrAdapter() override;

    std::unique_ptr<BatteryMgrClientAdapter> CreateBatteryClientAdapter() override;

    std::unique_ptr<NetConnectAdapter> CreateNetConnectAdapter() override;

    OhosWebDataBaseAdapter& GetOhosWebDataBaseAdapterInstance() override;

    PasteBoardClientAdapter& GetPasteBoard() override;

    std::unique_ptr<AudioRendererAdapter> CreateAudioRendererAdapter() override;

    std::unique_ptr<AudioCapturerAdapter> CreateAudioCapturerAdapter() override;

    AudioSystemManagerAdapter& GetAudioSystemManager() override;

    OhosWebPermissionDataBaseAdapter& GetWebPermissionDataBaseInstance() override;

    std::unique_ptr<MMIAdapter> CreateMMIAdapter() override;

    std::unique_ptr<SocPerfClientAdapter> CreateSocPerfClientAdapter() override;

    std::unique_ptr<OhosResourceAdapter> GetResourceAdapter(const std::string& hapPath = "") override;

    SystemPropertiesAdapter& GetSystemPropertiesInstance() override;

    VSyncAdapter& GetVSyncAdapter() override;

    std::unique_ptr<OhosInitWebAdapter> GetInitWebAdapter() override;

    KeystoreAdapter& GetKeystoreAdapterInstance() override;

    EnterpriseDeviceManagementAdapter& GetEnterpriseDeviceManagementInstance() override;

    DatashareAdapter& GetDatashareInstance() override;

    std::unique_ptr<IMFAdapter> CreateIMFAdapter() override;

    std::unique_ptr<CertManagerAdapter> GetRootCertDataAdapter() override;

    AccessTokenAdapter& GetAccessTokenAdapterInstance() override;

    std::unique_ptr<EventHandlerAdapter> GetEventHandlerAdapter() override;

    PrintManagerAdapter& GetPrintManagerInstance() override;

    std::unique_ptr<IConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() override;

    std::unique_ptr<PlayerAdapter> CreatePlayerAdapter() override;

    WindowAdapter& GetWindowAdapterInstance() override;

    HiSysEventAdapter& GetHiSysEventAdapterInstance() override;

    HiTraceAdapter& GetHiTraceAdapterInstance() override;

    NetProxyAdapter& GetNetProxyInstance() override;

    CameraManagerAdapter& GetCameraManagerAdapter() override;

    std::unique_ptr<ScreenCaptureAdapter> CreateScreenCaptureAdapter() override;

    std::unique_ptr<DateTimeFormatAdapter> CreateDateTimeFormatAdapter() override;

    std::unique_ptr<MediaCodecDecoderAdapter> CreateMediaCodecDecoderAdapter() override;

    std::unique_ptr<NativeImageAdapter> CreateNativeImageAdapter() override;

    std::unique_ptr<MediaCodecAdapter> CreateMediaCodecEncoderAdapter() override;

    MediaCodecListAdapter& GetMediaCodecListAdapter() override;

    std::unique_ptr<FlowbufferAdapter> CreateFlowbufferAdapter() override;

    std::unique_ptr<MediaAVSessionAdapter> CreateMediaAVSessionAdapter() override;

    std::unique_ptr<OhosImageDecoderAdapter> CreateOhosImageDecoderAdapter() override;

    std::unique_ptr<SensorAdapter> CreateSensorAdapter() override;

    void SetArkWebCoreHapPathOverride(const std::string& hapPath) override;

    OhosNativeBufferAdapter& GetOhosNativeBufferAdapter() override;

    OhosDrawingTextFontAdapter& GetOhosDrawingTextFontAdapter() override;

    OhosDrawingTextTypographyAdapter& GetOhosDrawingTextTypographyAdapter() override;
};
} // namespace OHOS::NWeb

#endif // OHOS_ADAPTER_HELPER_IMPL_H
