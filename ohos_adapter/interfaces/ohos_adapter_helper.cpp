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

#include "ohos_adapter_helper.h"

#include "aafwk_app_mgr_client_adapter_impl.h"
#include "access_token_adapter_impl.h"
#include "audio_renderer_adapter_impl.h"
#include "audio_capturer_adapter_impl.h"
#include "audio_system_manager_adapter_impl.h"
#include "battery_mgr_client_adapter_impl.h"
#include "camera_manager_adapter_impl.h"
#include "cert_mgr_adapter_impl.h"
#include "datashare_adapter_impl.h"
#include "display_manager_adapter_impl.h"
#include "enterprise_device_management_adapter_impl.h"
#include "event_handler_adapter_impl.h"
#include "hisysevent_adapter_impl.h"
#include "hitrace_adapter_impl.h"
#include "imf_adapter_impl.h"
#include "keystore_adapter_impl.h"
#include "mmi_adapter_impl.h"
#include "net_connect_adapter_impl.h"
#include "net_proxy_adapter_impl.h"
#include "ohos_init_web_adapter_impl.h"
#include "ohos_resource_adapter_impl.h"
#include "ohos_web_data_base_adapter_impl.h"
#include "ohos_web_dns_data_base_adapter_impl.h"
#include "ohos_web_permission_data_base_adapter_impl.h"
#include "pasteboard_client_adapter_impl.h"
#include "player_framework_adapter_impl.h"
#include "power_mgr_client_adapter_impl.h"
#include "print_manager_adapter_impl.h"
#include "screen_capture_adapter_impl.h"
#include "soc_perf_client_adapter_impl.h"
#include "surface_adapter_impl.h"
#include "system_properties_adapter_impl.h"
#include "vsync_adapter_impl.h"
#include "window_adapter_impl.h"

namespace OHOS::NWeb {
// static
OhosAdapterHelper& OhosAdapterHelper::GetInstance()
{
    static OhosAdapterHelper ohosAdapter;
    return ohosAdapter;
}

std::unique_ptr<AafwkAppMgrClientAdapter> OhosAdapterHelper::CreateAafwkAdapter()
{
    return std::make_unique<AafwkAppMgrClientAdapterImpl>();
}

std::unique_ptr<PowerMgrClientAdapter> OhosAdapterHelper::CreatePowerMgrClientAdapter()
{
    return std::make_unique<PowerMgrClientAdapterImpl>();
}

std::unique_ptr<DisplayManagerAdapter> OhosAdapterHelper::CreateDisplayMgrAdapter()
{
    return std::make_unique<DisplayManagerAdapterImpl>();
}

std::unique_ptr<BatteryMgrClientAdapter> OhosAdapterHelper::CreateBatteryClientAdapter()
{
    return std::make_unique<BatteryMgrClientAdapterImpl>();
}

OhosWebDataBaseAdapter& OhosAdapterHelper::GetOhosWebDataBaseAdapterInstance()
{
    return OhosWebDataBaseAdapterImpl::GetInstance();
}

std::unique_ptr<NetConnectAdapter> OhosAdapterHelper::CreateNetConnectAdapter()
{
    return std::make_unique<NetConnectAdapterImpl>();
}

PasteBoardClientAdapter& OhosAdapterHelper::GetPasteBoard() const
{
    return PasteBoardClientAdapterImpl::GetInstance();
}

std::unique_ptr<AudioRendererAdapter> OhosAdapterHelper::CreateAudioRendererAdapter()
{
    return std::make_unique<AudioRendererAdapterImpl>();
}

std::unique_ptr<AudioCapturerAdapter> OhosAdapterHelper::CreateAudioCapturerAdapter()
{
    return std::make_unique<AudioCapturerAdapterImpl>();
}

AudioSystemManagerAdapter& OhosAdapterHelper::GetAudioSystemManager() const
{
    return AudioSystemManagerAdapterImpl::GetInstance();
}

OhosWebPermissionDataBaseAdapter& OhosAdapterHelper::GetWebPermissionDataBaseInstance()
{
    return OhosWebPermissionDataBaseAdapterImpl::GetInstance();
}

std::unique_ptr<MMIAdapter> OhosAdapterHelper::CreateMMIAdapter()
{
    return std::make_unique<MMIAdapterImpl>();
}

std::unique_ptr<SocPerfClientAdapter> OhosAdapterHelper::CreateSocPerfClientAdapter()
{
    return std::make_unique<SocPerfClientAdapterImpl>();
}

OhosWebDnsDataBaseAdapter& OhosAdapterHelper::GetWebDnsDataBaseInstance()
{
    return OhosWebDnsDataBaseAdapterImpl::GetInstance();
}

std::unique_ptr<OhosResourceAdapter> OhosAdapterHelper::GetResourceAdapter(const std::string& hapPath) const
{
    return std::make_unique<OhosResourceAdapterImpl>(hapPath);
}

SystemPropertiesAdapter& OhosAdapterHelper::GetSystemPropertiesInstance() const
{
    return SystemPropertiesAdapterImpl::GetInstance();
}

std::unique_ptr<VSyncAdapter> OhosAdapterHelper::GetVSyncAdapter() const
{
    return std::make_unique<VSyncAdapterImpl>();
}

std::unique_ptr<OhosInitWebAdapter> OhosAdapterHelper::GetInitWebAdapter() const
{
    return std::make_unique<OhosInitWebAdapterImpl>();
}

KeystoreAdapter& OhosAdapterHelper::GetKeystoreAdapterInstance() const
{
    return KeystoreAdapterImpl::GetInstance();
}

EnterpriseDeviceManagementAdapter& OhosAdapterHelper::GetEnterpriseDeviceManagementInstance() const
{
    return EnterpriseDeviceManagementAdapterImpl::GetInstance();
}

DatashareAdapter& OhosAdapterHelper::GetDatashareInstance() const
{
    return DatashareAdapterImpl::GetInstance();
}

std::unique_ptr<IMFAdapter> OhosAdapterHelper::CreateIMFAdapter() const
{
    return std::make_unique<IMFAdapterImpl>();
}

std::unique_ptr<CertManagerAdapter> OhosAdapterHelper::GetRootCertDataAdapter() const
{
    return std::make_unique<CertManagerAdapterImpl>();
}

AccessTokenAdapter& OhosAdapterHelper::GetAccessTokenAdapterInstance() const
{
    return AccessTokenAdapterImpl::GetInstance();
}

std::unique_ptr<EventHandlerAdapter> OhosAdapterHelper::GetEventHandlerAdapter() const
{
    return std::make_unique<EventHandlerAdapterImpl>();
}

PrintManagerAdapter& OhosAdapterHelper::GetPrintManagerInstance() const
{
    return PrintManagerAdapterImpl::GetInstance();
}

std::unique_ptr<IConsumerSurfaceAdapter> OhosAdapterHelper::CreateConsumerSurfaceAdapter() const
{
    return std::make_unique<ConsumerSurfaceAdapterImpl>();
}

std::unique_ptr<PlayerAdapter> OhosAdapterHelper::CreatePlayerAdapter() const
{
    return std::make_unique<PlayerAdapterImpl>();
}

WindowAdapter& OhosAdapterHelper::GetWindowAdapterInstance() const
{
    return WindowAdapterImpl::GetInstance();
}

HiSysEventAdapter& OhosAdapterHelper::GetHiSysEventAdapterInstance() const
{
    return HiSysEventAdapterImpl::GetInstance();
}

HiTraceAdapter& OhosAdapterHelper::GetHiTraceAdapterInstance() const
{
    return HiTraceAdapterImpl::GetInstance();
}

NetProxyAdapter& OhosAdapterHelper::GetNetProxyInstance() const
{
    return NetProxyAdapterImpl::GetInstance();
}

CameraManagerAdapter& OhosAdapterHelper::GetCameraManagerAdapter() const
{
    return CameraManagerAdapterImpl::GetInstance();
}

std::unique_ptr<ScreenCaptureAdapter> OhosAdapterHelper::CreateScreenCaptureAdapter() const
{
    return std::make_unique<ScreenCaptureAdapterImpl>();
}
} // namespace OHOS::NWeb
