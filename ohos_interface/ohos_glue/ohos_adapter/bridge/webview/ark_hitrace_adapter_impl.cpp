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

#include "ohos_adapter/bridge/ark_hitrace_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkHiTraceAdapterImpl::ArkHiTraceAdapterImpl(NWeb::HiTraceAdapter& ref) : real_(ref) {}

void ArkHiTraceAdapterImpl::StartTrace(const ArkWebString& value, float limit)
{
    real_.StartTrace(ArkWebStringStructToClass(value), limit);
}

void ArkHiTraceAdapterImpl::FinishTrace()
{
    real_.FinishTrace();
}

void ArkHiTraceAdapterImpl::StartAsyncTrace(const ArkWebString& value, int32_t taskId, float limit)
{
    real_.StartAsyncTrace(ArkWebStringStructToClass(value), taskId, limit);
}

void ArkHiTraceAdapterImpl::FinishAsyncTrace(const ArkWebString& value, int32_t taskId)
{
    real_.FinishAsyncTrace(ArkWebStringStructToClass(value), taskId);
}

void ArkHiTraceAdapterImpl::CountTrace(const ArkWebString& name, int64_t count)
{
    real_.CountTrace(ArkWebStringStructToClass(name), count);
}

bool ArkHiTraceAdapterImpl::IsHiTraceEnable()
{
    return real_.IsHiTraceEnable();
}

void ArkHiTraceAdapterImpl::StartOHOSTrace(const ArkWebString& value, float limit)
{
    real_.StartOHOSTrace(ArkWebStringStructToClass(value), limit);
}

void ArkHiTraceAdapterImpl::FinishOHOSTrace()
{
    real_.FinishOHOSTrace();
}

void ArkHiTraceAdapterImpl::CountOHOSTrace(const ArkWebString& name, int64_t count)
{
    real_.CountOHOSTrace(ArkWebStringStructToClass(name), count);
}

bool ArkHiTraceAdapterImpl::IsACETraceEnable()
{
    return real_.IsACETraceEnable();
}

} // namespace OHOS::ArkWeb
