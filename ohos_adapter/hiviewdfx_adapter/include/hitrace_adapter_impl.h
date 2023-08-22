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

#ifndef HITRACE_ADAPTER_IMPL_H
#define HITRACE_ADAPTER_IMPL_H

#include "hitrace_adapter.h"

namespace OHOS::NWeb {
class HiTraceAdapterImpl : public HiTraceAdapter {
public:
    static HiTraceAdapterImpl& GetInstance();

    ~HiTraceAdapterImpl() override = default;

    void StartTrace(const std::string& value, float limit = -1) override;

    void FinishTrace() override;

    void StartAsyncTrace(const std::string& value, int32_t taskId, float limit = -1) override;

    void FinishAsyncTrace(const std::string& value, int32_t taskId) override;

    void CountTrace(const std::string& name, int64_t count) override;

    bool IsHiTraceEnable() override;
};
} // namespace OHOS::NWeb

#endif // HITRACE_ADAPTER_IMPL_H
