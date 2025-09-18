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

#ifndef ARK_OHOS_ADAPTER_HELPER_H
#define ARK_OHOS_ADAPTER_HELPER_H
#pragma once

#include "ohos_adapter/include/ark_aafwk_app_mgr_client_adapter.h"
#include "ohos_adapter/include/ark_access_token_adapter.h"
#include "ohos_adapter/include/ark_audio_capturer_adapter.h"
#include "ohos_adapter/include/ark_audio_renderer_adapter.h"
#include "ohos_adapter/include/ark_audio_system_manager_adapter.h"
#include "ohos_adapter/include/ark_battery_mgr_client_adapter.h"
#include "ohos_adapter/include/ark_camera_manager_adapter.h"
#include "ohos_adapter/include/ark_cert_manager_adapter.h"
#include "ohos_adapter/include/ark_datashare_adapter.h"
#include "ohos_adapter/include/ark_date_time_format_adapter.h"
#include "ohos_adapter/include/ark_display_manager_adapter.h"
#include "ohos_adapter/include/ark_drm_adapter.h"
#include "ohos_adapter/include/ark_enterprise_device_management_adapter.h"
#include "ohos_adapter/include/ark_event_handler_adapter.h"
#include "ohos_adapter/include/ark_flowbuffer_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"
#include "ohos_adapter/include/ark_hisysevent_adapter.h"
#include "ohos_adapter/include/ark_hitrace_adapter.h"
#include "ohos_adapter/include/ark_imf_adapter.h"
#include "ohos_adapter/include/ark_keystore_adapter.h"
#include "ohos_adapter/include/ark_media_adapter.h"
#include "ohos_adapter/include/ark_media_avsession_adapter.h"
#include "ohos_adapter/include/ark_media_codec_adapter.h"
#include "ohos_adapter/include/ark_media_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_migration_manager_adapter.h"
#include "ohos_adapter/include/ark_audio_codec_decoder_adapter.h"
#include "ohos_adapter/include/ark_mmi_adapter.h"
#include "ohos_adapter/include/ark_net_connect_adapter.h"
#include "ohos_adapter/include/ark_net_proxy_adapter.h"
#include "ohos_adapter/include/ark_ohos_drawing_text_adapter.h"
#include "ohos_adapter/include/ark_ohos_image_decoder_adapter.h"
#include "ohos_adapter/include/ark_ohos_init_web_adapter.h"
#include "ohos_adapter/include/ark_ohos_native_buffer_adapter.h"
#include "ohos_adapter/include/ark_ohos_resource_adapter.h"
#include "ohos_adapter/include/ark_ohos_web_data_base_adapter.h"
#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "ohos_adapter/include/ark_power_mgr_client_adapter.h"
#include "ohos_adapter/include/ark_print_manager_adapter.h"
#include "ohos_adapter/include/ark_screen_capture_adapter.h"
#include "ohos_adapter/include/ark_screenlock_manager_adapter.h"
#include "ohos_adapter/include/ark_sensor_adapter.h"
#include "ohos_adapter/include/ark_soc_perf_client_adapter.h"
#include "ohos_adapter/include/ark_system_properties_adapter.h"
#include "ohos_adapter/include/ark_background_task_adapter.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkOhosAdapterHelper : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    static ArkWebRefPtr<ArkOhosAdapterHelper> GetInstance();

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAafwkAppMgrClientAdapter> CreateAafwkAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkPowerMgrClientAdapter> CreatePowerMgrClientAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkDisplayManagerAdapter> CreateDisplayMgrAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkBatteryMgrClientAdapter> CreateBatteryClientAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkNetConnectAdapter> CreateNetConnectAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosWebDataBaseAdapter> GetOhosWebDataBaseAdapterInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkPasteBoardClientAdapter> GetPasteBoard() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioRendererAdapter> CreateAudioRendererAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioCapturerAdapter> CreateAudioCapturerAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioSystemManagerAdapter> GetAudioSystemManager() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosWebPermissionDataBaseAdapter> GetWebPermissionDataBaseInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkMMIAdapter> CreateMMIAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkSocPerfClientAdapter> CreateSocPerfClientAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosResourceAdapter> GetResourceAdapter(const ArkWebString& hapPath) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkSystemPropertiesAdapter> GetSystemPropertiesInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkVSyncAdapter> GetVSyncAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosInitWebAdapter> GetInitWebAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkKeystoreAdapter> GetKeystoreAdapterInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkEnterpriseDeviceManagementAdapter> GetEnterpriseDeviceManagementInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkDatashareAdapter> GetDatashareInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkIMFAdapter> CreateIMFAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkCertManagerAdapter> GetRootCertDataAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAccessTokenAdapter> GetAccessTokenAdapterInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkEventHandlerAdapter> GetEventHandlerAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkPrintManagerAdapter> GetPrintManagerInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkIConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkPlayerAdapter> CreatePlayerAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWindowAdapter> GetWindowAdapterInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkHiSysEventAdapter> GetHiSysEventAdapterInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkHiTraceAdapter> GetHiTraceAdapterInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkNetProxyAdapter> GetNetProxyInstance() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkCameraManagerAdapter> GetCameraManagerAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkScreenCaptureAdapter> CreateScreenCaptureAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkDateTimeFormatAdapter> CreateDateTimeFormatAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkMediaCodecDecoderAdapter> CreateMediaCodecDecoderAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkNativeImageAdapter> CreateNativeImageAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkMediaCodecAdapter> CreateMediaCodecEncoderAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkMediaCodecListAdapter> GetMediaCodecListAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkFlowbufferAdapter> CreateFlowbufferAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkMediaAVSessionAdapter> CreateMediaAVSessionAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosImageDecoderAdapter> CreateOhosImageDecoderAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkSensorAdapter> CreateSensorAdapter() = 0;

    /*--ark web()--*/
    virtual void SetArkWebCoreHapPathOverride(const ArkWebString& hapPath) = 0;

    /**
     * @Description: Get OhosNativeBufferAdapter.
     * @Return：Returns the pointer to a ArkOhosNativeBufferAdapter instance.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosNativeBufferAdapter> GetOhosNativeBufferAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkMigrationManagerAdapter> CreateMigrationMgrAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkAudioCodecDecoderAdapter> CreateAudioCodecDecoderAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkDrmAdapter> CreateDrmAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosDrawingTextFontAdapter> GetOhosDrawingTextFontAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosDrawingTextTypographyAdapter> GetOhosDrawingTextTypographyAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkScreenlockManagerAdapter> CreateScreenlockManagerAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkCertManagerAdapter> GetCertManagerAdapter() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkBackgroundTaskAdapter> CreateBackgroundTaskAdapter() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_ADAPTER_HELPER_H
