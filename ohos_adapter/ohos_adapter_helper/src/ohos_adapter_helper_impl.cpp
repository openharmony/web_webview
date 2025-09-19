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

#include "ohos_adapter_helper_impl.h"

#include "aafwk_app_mgr_client_adapter_impl.h"
#include "access_token_adapter_impl.h"
#include "audio_capturer_adapter_impl.h"
#include "audio_renderer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"
#if defined(NWEB_BATTERY_MANAGER_ENABLE)
#include "battery_mgr_client_adapter_impl.h"
#endif
#include "camera_manager_adapter_impl.h"
#include "cert_mgr_adapter_impl.h"
#include "datashare_adapter_impl.h"
#include "date_time_format_adapter_impl.h"
#include "display_manager_adapter_impl.h"
#include "enterprise_device_management_adapter_impl.h"
#include "event_handler_adapter_impl.h"
#include "flowbuffer_adapter_impl.h"
#include "hisysevent_adapter_impl.h"
#include "hitrace_adapter_impl.h"
#include "imf_adapter_impl.h"
#include "keystore_adapter_impl.h"
#if defined(NWEB_MEDIA_AVSESSION_ENABLE)
#include "media_avsession_adapter_impl.h"
#endif
#if defined(NWEB_MEDIA_AVCODEC_ENABLE)
#include "media_codec_decoder_adapter_impl.h"
#include "media_codec_encoder_adapter_impl.h"
#include "media_codec_list_adapter_impl.h"
#endif
#include "migration_manager_adapter_impl.h"
#include "mmi_adapter_impl.h"
#include "native_image_adapter_impl.h"
#if defined(NWEB_TEL_ENABLE)
#include "net_connect_adapter_impl.h"
#endif
#include "net_proxy_adapter_impl.h"
#include "ohos_drawing_text_adapter_impl.h"
#include "ohos_image_decoder_adapter_impl.h"
#include "ohos_init_web_adapter_impl.h"
#include "ohos_native_buffer_adapter_impl.h"
#include "ohos_resource_adapter_impl.h"
#include "ohos_web_data_base_adapter_impl.h"
#include "ohos_web_permission_data_base_adapter_impl.h"
#include "pasteboard_client_adapter_impl.h"
#if defined(NWEB_MEDIA_PLAYER_ENABLE)
#include "player_framework_adapter_impl.h"
#endif
#if defined(NWEB_POWER_MANAGER_ENABLE)
#include "power_mgr_client_adapter_impl.h"
#endif
#include "print_manager_adapter_impl.h"
#if defined(NWEB_CAMERA_ENABLE)
#include "screen_capture_adapter_impl.h"
#endif
#include "soc_perf_client_adapter_impl.h"
#include "screenlock_manager_adapter_impl.h"
#include "surface_adapter_impl.h"
#include "system_properties_adapter_impl.h"
#include "vsync_adapter_impl.h"
#include "window_adapter_impl.h"
#if defined(NWEB_SENSORS_SENSOR_ENABLE)
#include "sensor_adapter_impl.h"
#endif
#if defined(NWEB_AUDIO_DRM_ENABLE)
#include "audio_codec_decoder_adapter_impl.h"
#include "drm_adapter_impl.h"
#endif
#include "background_task_impl.h"

namespace OHOS::NWeb {
// static
OhosAdapterHelper& OhosAdapterHelper::GetInstance()
{
    static OhosAdapterHelperImpl ohosAdapter;
    return ohosAdapter;
}

std::unique_ptr<AafwkAppMgrClientAdapter> OhosAdapterHelperImpl::CreateAafwkAdapter()
{
    return std::make_unique<AafwkAppMgrClientAdapterImpl>();
}

std::unique_ptr<PowerMgrClientAdapter> OhosAdapterHelperImpl::CreatePowerMgrClientAdapter()
{
#if defined(NWEB_POWER_MANAGER_ENABLE)
    return std::make_unique<PowerMgrClientAdapterImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<DisplayManagerAdapter> OhosAdapterHelperImpl::CreateDisplayMgrAdapter()
{
    return std::make_unique<DisplayManagerAdapterImpl>();
}

std::unique_ptr<BatteryMgrClientAdapter> OhosAdapterHelperImpl::CreateBatteryClientAdapter()
{
#if defined(NWEB_BATTERY_MANAGER_ENABLE)
    return std::make_unique<BatteryMgrClientAdapterImpl>();
#else
    return nullptr;
#endif
}

OhosWebDataBaseAdapter& OhosAdapterHelperImpl::GetOhosWebDataBaseAdapterInstance()
{
    return OhosWebDataBaseAdapterImpl::GetInstance();
}

std::unique_ptr<NetConnectAdapter> OhosAdapterHelperImpl::CreateNetConnectAdapter()
{
#if defined(NWEB_TEL_ENABLE)
    return std::make_unique<NetConnectAdapterImpl>();
#else
    return nullptr;
#endif
}

PasteBoardClientAdapter& OhosAdapterHelperImpl::GetPasteBoard()
{
    return PasteBoardClientAdapterImpl::GetInstance();
}

std::unique_ptr<AudioRendererAdapter> OhosAdapterHelperImpl::CreateAudioRendererAdapter()
{
#if defined(NWEB_AUDIO_ENABLE)
    return std::make_unique<AudioRendererAdapterImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<AudioCapturerAdapter> OhosAdapterHelperImpl::CreateAudioCapturerAdapter()
{
#if defined(NWEB_AUDIO_ENABLE)
    return std::make_unique<AudioCapturerAdapterImpl>();
#else
    return nullptr;
#endif
}

AudioSystemManagerAdapter& OhosAdapterHelperImpl::GetAudioSystemManager()
{
    return AudioSystemManagerAdapterImpl::GetInstance();
}

OhosWebPermissionDataBaseAdapter& OhosAdapterHelperImpl::GetWebPermissionDataBaseInstance()
{
    return OhosWebPermissionDataBaseAdapterImpl::GetInstance();
}

std::unique_ptr<MMIAdapter> OhosAdapterHelperImpl::CreateMMIAdapter()
{
    return std::make_unique<MMIAdapterImpl>();
}

std::unique_ptr<SocPerfClientAdapter> OhosAdapterHelperImpl::CreateSocPerfClientAdapter()
{
    return std::make_unique<SocPerfClientAdapterImpl>();
}

std::unique_ptr<OhosResourceAdapter> OhosAdapterHelperImpl::GetResourceAdapter(const std::string& hapPath)
{
    return std::make_unique<OhosResourceAdapterImpl>(hapPath);
}

SystemPropertiesAdapter& OhosAdapterHelperImpl::GetSystemPropertiesInstance()
{
    return SystemPropertiesAdapterImpl::GetInstance();
}

VSyncAdapter& OhosAdapterHelperImpl::GetVSyncAdapter()
{
    return VSyncAdapterImpl::GetInstance();
}

std::unique_ptr<OhosInitWebAdapter> OhosAdapterHelperImpl::GetInitWebAdapter()
{
    return std::make_unique<OhosInitWebAdapterImpl>();
}

KeystoreAdapter& OhosAdapterHelperImpl::GetKeystoreAdapterInstance()
{
    return KeystoreAdapterImpl::GetInstance();
}

EnterpriseDeviceManagementAdapter& OhosAdapterHelperImpl::GetEnterpriseDeviceManagementInstance()
{
    return EnterpriseDeviceManagementAdapterImpl::GetInstance();
}

DatashareAdapter& OhosAdapterHelperImpl::GetDatashareInstance()
{
    return DatashareAdapterImpl::GetInstance();
}

std::unique_ptr<IMFAdapter> OhosAdapterHelperImpl::CreateIMFAdapter()
{
    return std::make_unique<IMFAdapterImpl>();
}

std::unique_ptr<CertManagerAdapter> OhosAdapterHelperImpl::GetRootCertDataAdapter()
{
    return std::make_unique<CertManagerAdapterImpl>();
}

std::unique_ptr<CertManagerAdapter> OhosAdapterHelperImpl::GetCertManagerAdapter()
{
    return std::make_unique<CertManagerAdapterImpl>();
}

AccessTokenAdapter& OhosAdapterHelperImpl::GetAccessTokenAdapterInstance()
{
    return AccessTokenAdapterImpl::GetInstance();
}

std::unique_ptr<EventHandlerAdapter> OhosAdapterHelperImpl::GetEventHandlerAdapter()
{
    return std::make_unique<EventHandlerAdapterImpl>();
}

PrintManagerAdapter& OhosAdapterHelperImpl::GetPrintManagerInstance()
{
    return PrintManagerAdapterImpl::GetInstance();
}

std::unique_ptr<IConsumerSurfaceAdapter> OhosAdapterHelperImpl::CreateConsumerSurfaceAdapter()
{
    return std::make_unique<ConsumerSurfaceAdapterImpl>();
}

std::unique_ptr<PlayerAdapter> OhosAdapterHelperImpl::CreatePlayerAdapter()
{
#if defined(NWEB_MEDIA_PLAYER_ENABLE)
    return std::make_unique<PlayerAdapterImpl>();
#else
    return nullptr;
#endif
}

WindowAdapter& OhosAdapterHelperImpl::GetWindowAdapterInstance()
{
    return WindowAdapterImpl::GetInstance();
}

HiSysEventAdapter& OhosAdapterHelperImpl::GetHiSysEventAdapterInstance()
{
    return HiSysEventAdapterImpl::GetInstance();
}

HiTraceAdapter& OhosAdapterHelperImpl::GetHiTraceAdapterInstance()
{
    return HiTraceAdapterImpl::GetInstance();
}

NetProxyAdapter& OhosAdapterHelperImpl::GetNetProxyInstance()
{
    return NetProxyAdapterImpl::GetInstance();
}

CameraManagerAdapter& OhosAdapterHelperImpl::GetCameraManagerAdapter()
{
    return CameraManagerAdapterImpl::GetInstance();
}

std::unique_ptr<ScreenCaptureAdapter> OhosAdapterHelperImpl::CreateScreenCaptureAdapter()
{
#if defined(NWEB_CAMERA_ENABLE)
    return std::make_unique<ScreenCaptureAdapterImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<DateTimeFormatAdapter> OhosAdapterHelperImpl::CreateDateTimeFormatAdapter()
{
    return std::make_unique<DateTimeFormatAdapterImpl>();
}

std::unique_ptr<MediaCodecDecoderAdapter> OhosAdapterHelperImpl::CreateMediaCodecDecoderAdapter()
{
#if defined(NWEB_MEDIA_AVCODEC_ENABLE)
    return std::make_unique<MediaCodecDecoderAdapterImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<NativeImageAdapter> OhosAdapterHelperImpl::CreateNativeImageAdapter()
{
    return std::make_unique<NativeImageAdapterImpl>();
}

std::unique_ptr<MediaCodecAdapter> OhosAdapterHelperImpl::CreateMediaCodecEncoderAdapter()
{
#if defined(NWEB_MEDIA_AVCODEC_ENABLE)
    return std::make_unique<MediaCodecEncoderAdapterImpl>();
#else
    return nullptr;
#endif
}

MediaCodecListAdapter& OhosAdapterHelperImpl::GetMediaCodecListAdapter()
{
    return MediaCodecListAdapterImpl::GetInstance();
}

std::unique_ptr<FlowbufferAdapter> OhosAdapterHelperImpl::CreateFlowbufferAdapter()
{
    return std::make_unique<FlowbufferAdapterImpl>();
}

std::unique_ptr<MediaAVSessionAdapter> OhosAdapterHelperImpl::CreateMediaAVSessionAdapter()
{
#if defined(NWEB_MEDIA_AVSESSION_ENABLE)
    return std::make_unique<MediaAVSessionAdapterImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<OhosImageDecoderAdapter> OhosAdapterHelperImpl::CreateOhosImageDecoderAdapter()
{
    return std::make_unique<OhosImageDecoderAdapterImpl>();
}

std::unique_ptr<SensorAdapter> OhosAdapterHelperImpl::CreateSensorAdapter()
{
#if defined(NWEB_SENSORS_SENSOR_ENABLE)
    return std::make_unique<SensorAdapterImpl>();
#else
    return nullptr;
#endif
}

void OhosAdapterHelperImpl::SetArkWebCoreHapPathOverride(const std::string& hapPath)
{
    OhosResourceAdapterImpl::SetArkWebCoreHapPathOverride(hapPath);
}

OhosNativeBufferAdapter& OhosAdapterHelperImpl::GetOhosNativeBufferAdapter()
{
    return OhosNativeBufferAdapterImpl::GetInstance();
}

std::unique_ptr<AudioCodecDecoderAdapter> OhosAdapterHelperImpl::CreateAudioCodecDecoderAdapter()
{
#if defined(NWEB_AUDIO_DRM_ENABLE)
    return std::make_unique<AudioCodecDecoderAdapterImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<DrmAdapter> OhosAdapterHelperImpl::CreateDrmAdapter()
{
#if defined(NWEB_AUDIO_DRM_ENABLE)
    return std::make_unique<DrmAdapterImpl>();
#else
    return nullptr;
#endif
}

OhosDrawingTextFontAdapter& OhosAdapterHelperImpl::GetOhosDrawingTextFontAdapter()
{
    return OhosDrawingTextFontAdapterImpl::GetInstance();
}

OhosDrawingTextTypographyAdapter& OhosAdapterHelperImpl::GetOhosDrawingTextTypographyAdapter()
{
    return OhosDrawingTextTypographyAdapterImpl::GetInstance();
}

std::unique_ptr<MigrationManagerAdapter> OhosAdapterHelperImpl::CreateMigrationMgrAdapter()
{
    return std::make_unique<MigrationManagerAdapterImpl>();
}

std::unique_ptr<ScreenlockManagerAdapter> OhosAdapterHelperImpl::CreateScreenlockManagerAdapter()
{
    return std::make_unique<ScreenlockManagerAdapterImpl>();
}

std::unique_ptr<BackgroundTaskAdapter> OhosAdapterHelperImpl::CreateBackgroundTaskAdapter()
{
    return std::make_unique<BackgroundTaskAdapterImpl>();
}
} // namespace OHOS::NWeb
