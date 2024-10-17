/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hitrace_adapter_impl.h"

#include "hitrace_meter.h"
#include "nweb_log.h"
#include "parameters.h"
#include "param/sys_param.h"

namespace OHOS::NWeb {
const int BASE = 10;
HiTraceAdapterImpl& HiTraceAdapterImpl::GetInstance()
{
    static HiTraceAdapterImpl instance;
    return instance;
}

uint64_t ConvertToUint64(const char *originValue, uint64_t defaultValue)
{
    return originValue == nullptr ? defaultValue : std::strtoull(originValue, nullptr, BASE);
}

void HiTraceAdapterImpl::StartTrace(const std::string& value, float limit)
{
    ::StartTrace(HITRACE_TAG_NWEB, value, limit);
}

void HiTraceAdapterImpl::FinishTrace()
{
    ::FinishTrace(HITRACE_TAG_NWEB);
}

void HiTraceAdapterImpl::StartAsyncTrace(const std::string& value, int32_t taskId, float limit)
{
    ::StartAsyncTrace(HITRACE_TAG_NWEB, value, taskId, limit);
}

void HiTraceAdapterImpl::FinishAsyncTrace(const std::string& value, int32_t taskId)
{
    ::FinishAsyncTrace(HITRACE_TAG_NWEB, value, taskId);
}

void HiTraceAdapterImpl::CountTrace(const std::string& name, int64_t count)
{
    ::CountTrace(HITRACE_TAG_NWEB, name, count);
}

bool HiTraceAdapterImpl::IsHiTraceEnable()
{
    static CachedHandle g_Handle = CachedParameterCreate("debug.hitrace.tags.enableflags", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    uint64_t tags = ConvertToUint64(enable, 0);
    return (tags & HITRACE_TAG_NWEB);
}

void HiTraceAdapterImpl::StartOHOSTrace(const std::string& value, float limit)
{
    if (IsHiTraceEnable()) {
        ::StartTrace(HITRACE_TAG_NWEB, value, limit);
    } else if (IsACETraceEnable()) {
        ::StartTrace(HITRACE_TAG_ACE, value, limit);
    }
}

void HiTraceAdapterImpl::FinishOHOSTrace()
{
    if (IsHiTraceEnable()) {
        ::FinishTrace(HITRACE_TAG_NWEB);
    } else if (IsACETraceEnable()) {
        ::FinishTrace(HITRACE_TAG_ACE);
    }
}

void HiTraceAdapterImpl::CountOHOSTrace(const std::string& name, int64_t count)
{
    if (IsHiTraceEnable()) {
        ::CountTrace(HITRACE_TAG_NWEB, name, count);
    } else if (IsACETraceEnable()) {
        ::CountTrace(HITRACE_TAG_ACE, name, count);
    }
}

bool HiTraceAdapterImpl::IsACETraceEnable()
{
    static CachedHandle g_Handle = CachedParameterCreate("debug.hitrace.tags.enableflags", "0");
    int changed = 0;
    const char *enable = CachedParameterGetChanged(g_Handle, &changed);
    uint64_t tags = ConvertToUint64(enable, 0);
    return (tags & HITRACE_TAG_ACE);
}
} // namespace OHOS::NWeb
