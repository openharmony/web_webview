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

#include "soc_perf_client_adapter_impl.h"
#if defined(NWEB_SOC_PERF)
#include "socperf_client.h"
#endif
namespace OHOS::NWeb {
SocPerfClientAdapterImpl::SocPerfClientAdapterImpl() {}

void SocPerfClientAdapterImpl::ApplySocPerfConfigById(int32_t id)
{
#if defined(NWEB_SOC_PERF)
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequest(id, "");
#endif
}

void SocPerfClientAdapterImpl::ApplySocPerfConfigByIdEx(int32_t id, bool onOffTag)
{
#if defined(NWEB_SOC_PERF)
    OHOS::SOCPERF::SocPerfClient::GetInstance().PerfRequestEx(id, onOffTag, "");
#endif
}

}  // namespace OHOS::NWeb
