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

#include "ohos_adapter/bridge/ark_ohos_adapter_helper_wrapper.h"

#include "ohos_adapter/bridge/ark_aafwk_app_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_access_token_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_capturer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_renderer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_audio_system_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_battery_info_wrapper.h"
#include "ohos_adapter/bridge/ark_battery_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_camera_surface_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_cert_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_datashare_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_date_time_format_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_enterprise_device_management_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_event_handler_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_flowbuffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hisysevent_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_hitrace_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_iconsumer_surface_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imfadapter_wrapper.h"
#include "ohos_adapter/bridge/ark_keystore_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_avsession_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_encoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_media_codec_list_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_migration_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_mmi_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_native_image_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_connect_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_net_proxy_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_drawing_text_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_file_mapper_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_image_decoder_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_init_web_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_native_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_resource_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_ohos_web_data_base_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_paste_board_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_player_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_power_mgr_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_print_manager_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_running_lock_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_screen_capture_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_sensor_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_soc_perf_client_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_surface_buffer_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_system_properties_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_vsync_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_web_timezone_info_wrapper.h"
#include "ohos_adapter/bridge/ark_window_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::NWeb {

OhosAdapterHelper& OhosAdapterHelper::GetInstance()
{
    static ArkWeb::ArkOhosAdapterHelperWrapper instance(ArkWeb::ArkOhosAdapterHelper::GetInstance());
    return instance;
}

} // namespace OHOS::NWeb

namespace OHOS::ArkWeb {

ArkOhosAdapterHelperWrapper::ArkOhosAdapterHelperWrapper(ArkWebRefPtr<ArkOhosAdapterHelper> ref) : ctocpp_(ref) {}

std::unique_ptr<NWeb::AafwkAppMgrClientAdapter> ArkOhosAdapterHelperWrapper::CreateAafwkAdapter()
{
    ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> adapter = ctocpp_->CreateAafwkAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkAafwkAppMgrClientAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::PowerMgrClientAdapter> ArkOhosAdapterHelperWrapper::CreatePowerMgrClientAdapter()
{
    ArkWebRefPtr<ArkPowerMgrClientAdapter> adapter = ctocpp_->CreatePowerMgrClientAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkPowerMgrClientAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::DisplayManagerAdapter> ArkOhosAdapterHelperWrapper::CreateDisplayMgrAdapter()
{
    ArkWebRefPtr<ArkDisplayManagerAdapter> adapter = ctocpp_->CreateDisplayMgrAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkDisplayManagerAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::BatteryMgrClientAdapter> ArkOhosAdapterHelperWrapper::CreateBatteryClientAdapter()
{
    ArkWebRefPtr<ArkBatteryMgrClientAdapter> adapter = ctocpp_->CreateBatteryClientAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkBatteryMgrClientAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::NetConnectAdapter> ArkOhosAdapterHelperWrapper::CreateNetConnectAdapter()
{
    ArkWebRefPtr<ArkNetConnectAdapter> adapter = ctocpp_->CreateNetConnectAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkNetConnectAdapterWrapper>(adapter);
}

NWeb::OhosWebDataBaseAdapter& ArkOhosAdapterHelperWrapper::GetOhosWebDataBaseAdapterInstance()
{
    static ArkOhosWebDataBaseAdapterWrapper instance(ctocpp_->GetOhosWebDataBaseAdapterInstance());
    return instance;
}

NWeb::PasteBoardClientAdapter& ArkOhosAdapterHelperWrapper::GetPasteBoard()
{
    static ArkPasteBoardClientAdapterWrapper instance(ctocpp_->GetPasteBoard());
    return instance;
}

std::unique_ptr<NWeb::AudioRendererAdapter> ArkOhosAdapterHelperWrapper::CreateAudioRendererAdapter()
{
    ArkWebRefPtr<ArkAudioRendererAdapter> adapter = ctocpp_->CreateAudioRendererAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkAudioRendererAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::AudioCapturerAdapter> ArkOhosAdapterHelperWrapper::CreateAudioCapturerAdapter()
{
    ArkWebRefPtr<ArkAudioCapturerAdapter> adapter = ctocpp_->CreateAudioCapturerAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkAudioCapturerAdapterWrapper>(adapter);
}

NWeb::AudioSystemManagerAdapter& ArkOhosAdapterHelperWrapper::GetAudioSystemManager()
{
    static ArkAudioSystemManagerAdapterWrapper instance(ctocpp_->GetAudioSystemManager());
    return instance;
}

NWeb::OhosWebPermissionDataBaseAdapter& ArkOhosAdapterHelperWrapper::GetWebPermissionDataBaseInstance()
{
    static ArkOhosWebPermissionDataBaseAdapterWrapper instance(ctocpp_->GetWebPermissionDataBaseInstance());
    return instance;
}

std::unique_ptr<NWeb::MMIAdapter> ArkOhosAdapterHelperWrapper::CreateMMIAdapter()
{
    ArkWebRefPtr<ArkMMIAdapter> adapter = ctocpp_->CreateMMIAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkMMIAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::SocPerfClientAdapter> ArkOhosAdapterHelperWrapper::CreateSocPerfClientAdapter()
{
    ArkWebRefPtr<ArkSocPerfClientAdapter> adapter = ctocpp_->CreateSocPerfClientAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkSocPerfClientAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::OhosResourceAdapter> ArkOhosAdapterHelperWrapper::GetResourceAdapter(const std::string& hapPath)
{
    ArkWebString str = ArkWebStringClassToStruct(hapPath);
    ArkWebRefPtr<ArkOhosResourceAdapter> adapter = ctocpp_->GetResourceAdapter(str);
    ArkWebStringStructRelease(str);

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkOhosResourceAdapterWrapper>(adapter);
}

NWeb::SystemPropertiesAdapter& ArkOhosAdapterHelperWrapper::GetSystemPropertiesInstance()
{
    static ArkSystemPropertiesAdapterWrapper instance(ctocpp_->GetSystemPropertiesInstance());
    return instance;
}

NWeb::VSyncAdapter& ArkOhosAdapterHelperWrapper::GetVSyncAdapter()
{
    static ArkVSyncAdapterWrapper instance(ctocpp_->GetVSyncAdapter());
    return instance;
}

std::unique_ptr<NWeb::OhosInitWebAdapter> ArkOhosAdapterHelperWrapper::GetInitWebAdapter()
{
    ArkWebRefPtr<ArkOhosInitWebAdapter> adapter = ctocpp_->GetInitWebAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkOhosInitWebAdapterWrapper>(adapter);
}

NWeb::KeystoreAdapter& ArkOhosAdapterHelperWrapper::GetKeystoreAdapterInstance()
{
    static ArkKeystoreAdapterWrapper instance(ctocpp_->GetKeystoreAdapterInstance());
    return instance;
}

NWeb::EnterpriseDeviceManagementAdapter& ArkOhosAdapterHelperWrapper::GetEnterpriseDeviceManagementInstance()
{
    static ArkEnterpriseDeviceManagementAdapterWrapper instance(ctocpp_->GetEnterpriseDeviceManagementInstance());
    return instance;
}

NWeb::DatashareAdapter& ArkOhosAdapterHelperWrapper::GetDatashareInstance()
{
    static ArkDatashareAdapterWrapper instance(ctocpp_->GetDatashareInstance());
    return instance;
}

std::unique_ptr<NWeb::IMFAdapter> ArkOhosAdapterHelperWrapper::CreateIMFAdapter()
{
    ArkWebRefPtr<ArkIMFAdapter> adapter = ctocpp_->CreateIMFAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkIMFAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::CertManagerAdapter> ArkOhosAdapterHelperWrapper::GetRootCertDataAdapter()
{
    ArkWebRefPtr<ArkCertManagerAdapter> adapter = ctocpp_->GetRootCertDataAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkCertManagerAdapterWrapper>(adapter);
}

NWeb::AccessTokenAdapter& ArkOhosAdapterHelperWrapper::GetAccessTokenAdapterInstance()
{
    static ArkAccessTokenAdapterWrapper instance(ctocpp_->GetAccessTokenAdapterInstance());
    return instance;
}

std::unique_ptr<NWeb::EventHandlerAdapter> ArkOhosAdapterHelperWrapper::GetEventHandlerAdapter()
{
    ArkWebRefPtr<ArkEventHandlerAdapter> adapter = ctocpp_->GetEventHandlerAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkEventHandlerAdapterWrapper>(adapter);
}

NWeb::PrintManagerAdapter& ArkOhosAdapterHelperWrapper::GetPrintManagerInstance()
{
    static ArkPrintManagerAdapterWrapper instance(ctocpp_->GetPrintManagerInstance());
    return instance;
}

std::unique_ptr<NWeb::IConsumerSurfaceAdapter> ArkOhosAdapterHelperWrapper::CreateConsumerSurfaceAdapter()
{
    ArkWebRefPtr<ArkIConsumerSurfaceAdapter> adapter = ctocpp_->CreateConsumerSurfaceAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkIConsumerSurfaceAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::PlayerAdapter> ArkOhosAdapterHelperWrapper::CreatePlayerAdapter()
{
    ArkWebRefPtr<ArkPlayerAdapter> adapter = ctocpp_->CreatePlayerAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkPlayerAdapterWrapper>(adapter);
}

NWeb::WindowAdapter& ArkOhosAdapterHelperWrapper::GetWindowAdapterInstance()
{
    static ArkWindowAdapterWrapper instance(ctocpp_->GetWindowAdapterInstance());
    return instance;
}

NWeb::HiSysEventAdapter& ArkOhosAdapterHelperWrapper::GetHiSysEventAdapterInstance()
{
    static ArkHiSysEventAdapterWrapper instance(ctocpp_->GetHiSysEventAdapterInstance());
    return instance;
}

NWeb::HiTraceAdapter& ArkOhosAdapterHelperWrapper::GetHiTraceAdapterInstance()
{
    static ArkHiTraceAdapterWrapper instance(ctocpp_->GetHiTraceAdapterInstance());
    return instance;
}

NWeb::NetProxyAdapter& ArkOhosAdapterHelperWrapper::GetNetProxyInstance()
{
    static ArkNetProxyAdapterWrapper instance(ctocpp_->GetNetProxyInstance());
    return instance;
}

NWeb::CameraManagerAdapter& ArkOhosAdapterHelperWrapper::GetCameraManagerAdapter()
{
    static ArkCameraManagerAdapterWrapper instance(ctocpp_->GetCameraManagerAdapter());
    return instance;
}

std::unique_ptr<NWeb::ScreenCaptureAdapter> ArkOhosAdapterHelperWrapper::CreateScreenCaptureAdapter()
{
    ArkWebRefPtr<ArkScreenCaptureAdapter> adapter = ctocpp_->CreateScreenCaptureAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkScreenCaptureAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::DateTimeFormatAdapter> ArkOhosAdapterHelperWrapper::CreateDateTimeFormatAdapter()
{
    ArkWebRefPtr<ArkDateTimeFormatAdapter> adapter = ctocpp_->CreateDateTimeFormatAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkDateTimeFormatAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::MediaCodecDecoderAdapter> ArkOhosAdapterHelperWrapper::CreateMediaCodecDecoderAdapter()
{
    ArkWebRefPtr<ArkMediaCodecDecoderAdapter> adapter = ctocpp_->CreateMediaCodecDecoderAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkMediaCodecDecoderAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::NativeImageAdapter> ArkOhosAdapterHelperWrapper::CreateNativeImageAdapter()
{
    ArkWebRefPtr<ArkNativeImageAdapter> adapter = ctocpp_->CreateNativeImageAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkNativeImageAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::MediaCodecAdapter> ArkOhosAdapterHelperWrapper::CreateMediaCodecEncoderAdapter()
{
    ArkWebRefPtr<ArkMediaCodecAdapter> adapter = ctocpp_->CreateMediaCodecEncoderAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkMediaCodecEncoderAdapterWrapper>(adapter);
}

NWeb::MediaCodecListAdapter& ArkOhosAdapterHelperWrapper::GetMediaCodecListAdapter()
{
    static ArkMediaCodecListAdapterWrapper instance(ctocpp_->GetMediaCodecListAdapter());
    return instance;
}

std::unique_ptr<NWeb::FlowbufferAdapter> ArkOhosAdapterHelperWrapper::CreateFlowbufferAdapter()
{
    ArkWebRefPtr<ArkFlowbufferAdapter> adapter = ctocpp_->CreateFlowbufferAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkFlowbufferAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::MediaAVSessionAdapter> ArkOhosAdapterHelperWrapper::CreateMediaAVSessionAdapter()
{
    ArkWebRefPtr<ArkMediaAVSessionAdapter> adapter = ctocpp_->CreateMediaAVSessionAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkMediaAVSessionAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::OhosImageDecoderAdapter> ArkOhosAdapterHelperWrapper::CreateOhosImageDecoderAdapter()
{
    ArkWebRefPtr<ArkOhosImageDecoderAdapter> adapter = ctocpp_->CreateOhosImageDecoderAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkOhosImageDecoderAdapterWrapper>(adapter);
}

std::unique_ptr<NWeb::SensorAdapter> ArkOhosAdapterHelperWrapper::CreateSensorAdapter()
{
    ArkWebRefPtr<ArkSensorAdapter> adapter = ctocpp_->CreateSensorAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkSensorAdapterWrapper>(adapter);
}

void ArkOhosAdapterHelperWrapper::SetArkWebCoreHapPathOverride(const std::string& hapPath)
{
    ArkWebString str = ArkWebStringClassToStruct(hapPath);
    ctocpp_->SetArkWebCoreHapPathOverride(str);
    ArkWebStringStructRelease(str);
}

NWeb::OhosNativeBufferAdapter& ArkOhosAdapterHelperWrapper::GetOhosNativeBufferAdapter()
{
    static ArkOhosNativeBufferAdapterWrapper instance(ctocpp_->GetOhosNativeBufferAdapter());
    return instance;
}

NWeb::OhosDrawingTextFontAdapter& ArkOhosAdapterHelperWrapper::GetOhosDrawingTextFontAdapter()
{
    static ArkOhosDrawingTextFontAdapterWrapper instance(ctocpp_->GetOhosDrawingTextFontAdapter());
    return instance;
}

NWeb::OhosDrawingTextTypographyAdapter& ArkOhosAdapterHelperWrapper::GetOhosDrawingTextTypographyAdapter()
{
    static ArkOhosDrawingTextTypographyAdapterWrapper instance(ctocpp_->GetOhosDrawingTextTypographyAdapter());
    return instance;
}

std::unique_ptr<NWeb::MigrationManagerAdapter> ArkOhosAdapterHelperWrapper::CreateMigrationMgrAdapter()
{
    ArkWebRefPtr<ArkMigrationManagerAdapter> adapter = ctocpp_->CreateMigrationMgrAdapter();

    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_unique<ArkMigrationManagerAdapterWrapper>(adapter);
}

} // namespace OHOS::ArkWeb
