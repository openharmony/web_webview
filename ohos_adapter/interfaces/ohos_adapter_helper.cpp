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
#include "battery_mgr_client_adapter_impl.h"
#include "display_manager_adapter_impl.h"
#include "net_connect_adapter_impl.h"
#include "pasteboard_client_adapter_impl.h"
#include "power_mgr_client_adapter_impl.h"
#include "ohos_web_data_base_adapter_impl.h"

namespace OHOS::NWeb {
// static
OhosAdapterHelper &OhosAdapterHelper::GetInstance()
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

OhosWebDataBaseAdapter &OhosAdapterHelper::GetOhosWebDataBaseAdapterInstance()
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
}  // namespace OHOS::NWeb
