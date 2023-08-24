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

#ifndef HITRACE_ADAPTER_H
#define HITRACE_ADAPTER_H

#include <string>

namespace OHOS::NWeb {
class HiTraceAdapter {
public:
    virtual void StartTrace(const std::string& value, float limit = -1) = 0;

    virtual void FinishTrace() = 0;

    virtual void StartAsyncTrace(const std::string& value, int32_t taskId, float limit = -1) = 0;

    virtual void FinishAsyncTrace(const std::string& value, int32_t taskId) = 0;

    virtual void CountTrace(const std::string& name, int64_t count) = 0;

    virtual bool IsHiTraceEnable() = 0;

protected:
    HiTraceAdapter() = default;

    virtual ~HiTraceAdapter() = default;
};
} // namespace OHOS::NWeb

#endif // HITRACE_ADAPTER_H
