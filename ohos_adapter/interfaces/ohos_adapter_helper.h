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

#ifndef OHOS_ADAPTER_HELPER_H
#define OHOS_ADAPTER_HELPER_H

#include <memory>

#include "aafwk_app_mgr_client_adapter.h"
#include "access_token_adapter.h"
#include "audio_renderer_adapter.h"
#include "audio_capturer_adapter.h"
#include "audio_system_manager_adapter.h"
#include "battery_mgr_client_adapter.h"
#include "camera_manager_adapter.h"
#include "cert_mgr_adapter.h"
#include "datashare_adapter.h"
#include "display_manager_adapter.h"
#include "enterprise_device_management_adapter.h"
#include "event_handler_adapter.h"
#include "graphic_adapter.h"
#include "hisysevent_adapter.h"
#include "hitrace_adapter.h"
#include "imf_adapter.h"
#include "keystore_adapter.h"
#include "media_adapter.h"
#include "mmi_adapter.h"
#include "net_connect_adapter.h"
#include "net_proxy_adapter.h"
#include "ohos_init_web_adapter.h"
#include "ohos_resource_adapter.h"
#include "ohos_web_data_base_adapter.h"
#include "pasteboard_client_adapter.h"
#include "power_mgr_client_adapter.h"
#include "print_manager_adapter.h"
#include "screen_capture_adapter.h"
#include "soc_perf_client_adapter.h"
#include "system_properties_adapter.h"

namespace OHOS::NWeb {
class OhosAdapterHelper {
public:
    static OhosAdapterHelper& GetInstance();

    virtual ~OhosAdapterHelper() = default;

    std::unique_ptr<AafwkAppMgrClientAdapter> CreateAafwkAdapter();

    std::unique_ptr<PowerMgrClientAdapter> CreatePowerMgrClientAdapter();

    std::unique_ptr<DisplayManagerAdapter> CreateDisplayMgrAdapter();

    std::unique_ptr<BatteryMgrClientAdapter> CreateBatteryClientAdapter();

    std::unique_ptr<NetConnectAdapter> CreateNetConnectAdapter();

    OhosWebDataBaseAdapter& GetOhosWebDataBaseAdapterInstance();

    PasteBoardClientAdapter& GetPasteBoard() const;

    std::unique_ptr<AudioRendererAdapter> CreateAudioRendererAdapter();

    std::unique_ptr<AudioCapturerAdapter> CreateAudioCapturerAdapter();

    AudioSystemManagerAdapter& GetAudioSystemManager() const;

    OhosWebPermissionDataBaseAdapter& GetWebPermissionDataBaseInstance();

    std::unique_ptr<MMIAdapter> CreateMMIAdapter();

    std::unique_ptr<SocPerfClientAdapter> CreateSocPerfClientAdapter();

    OhosWebDnsDataBaseAdapter& GetWebDnsDataBaseInstance();

    std::unique_ptr<OhosResourceAdapter> GetResourceAdapter(const std::string& hapPath = "") const;

    SystemPropertiesAdapter& GetSystemPropertiesInstance() const;

    std::unique_ptr<VSyncAdapter> GetVSyncAdapter() const;

    std::unique_ptr<OhosInitWebAdapter> GetInitWebAdapter() const;

    KeystoreAdapter& GetKeystoreAdapterInstance() const;

    EnterpriseDeviceManagementAdapter& GetEnterpriseDeviceManagementInstance() const;

    DatashareAdapter& GetDatashareInstance() const;

    std::unique_ptr<IMFAdapter> CreateIMFAdapter() const;

    std::unique_ptr<CertManagerAdapter> GetRootCertDataAdapter() const;

    AccessTokenAdapter& GetAccessTokenAdapterInstance() const;

    std::unique_ptr<EventHandlerAdapter> GetEventHandlerAdapter() const;

    PrintManagerAdapter& GetPrintManagerInstance() const;

    std::unique_ptr<IConsumerSurfaceAdapter> CreateConsumerSurfaceAdapter() const;

    std::unique_ptr<PlayerAdapter> CreatePlayerAdapter() const;

    WindowAdapter& GetWindowAdapterInstance() const;

    HiSysEventAdapter& GetHiSysEventAdapterInstance() const;

    HiTraceAdapter& GetHiTraceAdapterInstance() const;

    NetProxyAdapter& GetNetProxyInstance() const;

    CameraManagerAdapter& GetCameraManagerAdapter() const;

    std::unique_ptr<ScreenCaptureAdapter> CreateScreenCaptureAdapter() const;

private:
    OhosAdapterHelper() = default;

    OhosAdapterHelper(const OhosAdapterHelper&) = delete;

    OhosAdapterHelper& operator=(const OhosAdapterHelper&) = delete;
};
} // namespace OHOS::NWeb

#endif // OHOS_ADAPTER_HELPER_H
