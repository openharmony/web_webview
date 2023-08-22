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

namespace OHOS::NWeb {
HiTraceAdapterImpl& HiTraceAdapterImpl::GetInstance()
{
    static HiTraceAdapterImpl instance;
    return instance;
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
    const std::string KEY_TRACE_TAG = "debug.hitrace.tags.enableflags";

    uint64_t tags = OHOS::system::GetUintParameter<uint64_t>(KEY_TRACE_TAG, 0);
    if (tags == 0) {
        WVLOG_E("HiTraceAdapterImpl GetUintParameter %s error.", KEY_TRACE_TAG.c_str());
        return false;
    }
    return (tags & HITRACE_TAG_NWEB);
}
} // namespace OHOS::NWeb
