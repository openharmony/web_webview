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

#include "nweb_log.h"
#if defined(WEBVIWE_ONLY) && defined(NWEB_SOC_PERF)
#include "res_sched_client.h"
#include "res_type.h"
#include "socperf_client.h"
#endif

namespace OHOS::NWeb {
namespace {
#if defined(WEBVIWE_ONLY) && defined(NWEB_SOC_PERF)
    const int32_t SOC_PERF_WEB_DRAG_RESIZE_ID = 10073;
    const uint32_t SOC_PERF_INVALID = UINT32_MAX;
#endif
}

SocPerfClientAdapterImpl::SocPerfClientAdapterImpl()
{
#if defined(WEBVIWE_ONLY) && defined(NWEB_SOC_PERF)
    convertMap_ = {
        { SOC_PERF_WEB_GESTURE_ID, ResourceSchedule::ResType::RES_TYPE_WEB_GESTURE },
        { SOC_PERF_WEB_GESTURE_MOVE_ID, ResourceSchedule::ResType::RES_TYPE_WEB_GESTURE_MOVE },
        { SOC_PERF_SLIDE_NORMAL_ID, ResourceSchedule::ResType::RES_TYPE_WEB_SLIDE_NORMAL },
        { SOC_PERF_LOAD_URL_ID, ResourceSchedule::ResType::RES_TYPE_LOAD_URL },
        { SOC_PERF_MOUSEWHEEL_ID, ResourceSchedule::ResType::RES_TYPE_MOUSEWHEEL },
        { SOC_PERF_WEB_DRAG_RESIZE_ID, ResourceSchedule::ResType::RES_TYPE_WEB_DRAG_RESIZE },
    };
#else
    PRINT_NOT_IMPL_FUNC_LOG();
#endif
}

SocPerfClientAdapterImpl::~SocPerfClientAdapterImpl()
{
#ifdef WEBVIWE_ONLY
    convertMap_.clear();
#else
    PRINT_NOT_IMPL_FUNC_LOG();
#endif
}

void SocPerfClientAdapterImpl::ApplySocPerfConfigById(int32_t id)
{
#if defined(WEBVIWE_ONLY) && defined(NWEB_SOC_PERF)
    uint32_t resType = ConvertId(id);
    if (resType == SOC_PERF_INVALID) {
        return;
    }

    std::unordered_map<std::string, std::string> mapPayload;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(resType, SOC_PERF_START, mapPayload);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
#endif
}

void SocPerfClientAdapterImpl::ApplySocPerfConfigByIdEx(int32_t id, bool onOffTag)
{
#if defined(WEBVIWE_ONLY) && defined(NWEB_SOC_PERF)
    uint32_t resType = ConvertId(id);
    if (resType == SOC_PERF_INVALID) {
        return;
    }

    std::unordered_map<std::string, std::string> mapPayload;
    int64_t value = onOffTag ? SOC_PERF_START : SOC_PERF_END;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(resType, value, mapPayload);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
#endif
}

uint32_t SocPerfClientAdapterImpl::ConvertId(int32_t id)
{
#if defined(WEBVIWE_ONLY) && defined(NWEB_SOC_PERF)
    auto mit = convertMap_.find(id);
    if (mit == convertMap_.end()) {
        WVLOG_E("invalid id: %{public}d", id);
        return SOC_PERF_INVALID;
    }
    return mit->second;
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}
}  // namespace OHOS::NWeb
