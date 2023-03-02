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
#include "audio_renderer_adapter.h"
#include "audio_system_manager_adapter.h"
#include "battery_mgr_client_adapter.h"
#include "datashare_adapter.h"
#include "display_manager_adapter.h"
#include "imf_adapter.h"
#include "mmi_adapter.h"
#include "net_connect_adapter.h"
#include "ohos_init_web_adapter.h"
#include "ohos_web_data_base_adapter.h"
#include "pasteboard_client_adapter.h"
#include "power_mgr_client_adapter.h"
#include "soc_perf_client_adapter.h"

namespace OHOS::NWeb {
class OhosAdapterHelper {
public:
    static OhosAdapterHelper &GetInstance();

    virtual ~OhosAdapterHelper() = default;

    std::unique_ptr<AafwkAppMgrClientAdapter> CreateAafwkAdapter();

    std::unique_ptr<PowerMgrClientAdapter> CreatePowerMgrClientAdapter();

    std::unique_ptr<DisplayManagerAdapter> CreateDisplayMgrAdapter();

    std::unique_ptr<BatteryMgrClientAdapter> CreateBatteryClientAdapter();

    std::unique_ptr<NetConnectAdapter> CreateNetConnectAdapter();

    OhosWebDataBaseAdapter& GetOhosWebDataBaseAdapterInstance();

    PasteBoardClientAdapter& GetPasteBoard() const;

    std::unique_ptr<AudioRendererAdapter> CreateAudioRendererAdapter();

    AudioSystemManagerAdapter& GetAudioSystemManager() const;

    OhosWebPermissionDataBaseAdapter& GetWebPermissionDataBaseInstance();

    std::unique_ptr<MMIAdapter> CreateMMIAdapter();

    std::unique_ptr<SocPerfClientAdapter> CreateSocPerfClientAdapter();

    OhosWebDnsDataBaseAdapter& GetWebDnsDataBaseInstance();

    std::unique_ptr<OhosInitWebAdapter> GetInitWebAdapter() const;

    DatashareAdapter& GetDatashareInstance() const;

    std::unique_ptr<IMFAdapter> CreateIMFAdapter() const;

private:
    OhosAdapterHelper() = default;

    OhosAdapterHelper(const OhosAdapterHelper&) = delete;

    OhosAdapterHelper& operator=(const OhosAdapterHelper&) = delete;
};
} // namespace OHOS::NWeb

#endif // OHOS_ADAPTER_HELPER_H
