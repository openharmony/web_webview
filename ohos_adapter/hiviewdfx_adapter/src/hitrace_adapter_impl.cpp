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

#include "nweb_log.h"

#include <hitrace/trace.h>

namespace OHOS::NWeb {
HiTraceAdapterImpl& HiTraceAdapterImpl::GetInstance()
{
    static HiTraceAdapterImpl instance;
    return instance;
}

int ConvertToInt(const char *originValue, int defaultValue)
{
    return originValue == nullptr ? defaultValue : std::atoi(originValue);
}

void HiTraceAdapterImpl::StartTrace(const std::string& value, float limit)
{
    OH_HiTrace_StartTrace(value.c_str());
}

void HiTraceAdapterImpl::FinishTrace()
{
    OH_HiTrace_FinishTrace();
}

void HiTraceAdapterImpl::StartAsyncTrace(const std::string& value, int32_t taskId, float limit)
{
    OH_HiTrace_StartAsyncTrace(value.c_str(), taskId);
}

void HiTraceAdapterImpl::FinishAsyncTrace(const std::string& value, int32_t taskId)
{
    OH_HiTrace_FinishAsyncTrace(value.c_str(), taskId);
}

void HiTraceAdapterImpl::CountTrace(const std::string& name, int64_t count)
{
    OH_HiTrace_CountTrace(name.c_str(), count);
}

bool HiTraceAdapterImpl::IsHiTraceEnable()
{
    return false;
}

void HiTraceAdapterImpl::StartOHOSTrace(const std::string& value, float limit)
{
    OH_HiTrace_StartTrace(value.c_str());
}

void HiTraceAdapterImpl::FinishOHOSTrace()
{
    OH_HiTrace_FinishTrace();
}

void HiTraceAdapterImpl::CountOHOSTrace(const std::string& name, int64_t count)
{
    OH_HiTrace_CountTrace(name.c_str(), count);
}

bool HiTraceAdapterImpl::IsACETraceEnable()
{
    return false;
}
} // namespace OHOS::NWeb
