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

#include "ark_vsync_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkVSyncAdapterImpl::ArkVSyncAdapterImpl(NWeb::VSyncAdapter& ref) : real_(ref) {}

uint32_t ArkVSyncAdapterImpl::RequestVsync(void* data, ArkVSyncCb cb)
{
    return (uint32_t)real_.RequestVsync(data, cb);
}

int64_t ArkVSyncAdapterImpl::GetVSyncPeriod()
{
    return real_.GetVSyncPeriod();
}

} // namespace OHOS::ArkWeb