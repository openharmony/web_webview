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

#include "power_mgr_client_adapter_impl.h"
#include "nweb_log.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {
bool RunningLockAdapterImpl::IsUsed()
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

int32_t RunningLockAdapterImpl::Lock(uint32_t timeOutMs)
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

int32_t RunningLockAdapterImpl::UnLock()
{
    PRINT_NOT_IMPL_FUNC_LOG();
}

std::shared_ptr<RunningLockAdapter> PowerMgrClientAdapterImpl::CreateRunningLock(
    const std::string& name, RunningLockAdapterType type)
{
    PRINT_NOT_IMPL_FUNC_LOG();
    return std::make_shared<RunningLockAdapterImpl>();
}
}