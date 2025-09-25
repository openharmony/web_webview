/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef AAFWK_BROWSER_CLIENT_ADAPTER_H
#define AAFWK_BROWSER_CLIENT_ADAPTER_H

#include <cstdint>

#include "res_sched_client_adapter.h"

namespace OHOS::NWeb {
class AafwkBrowserClientAdapter {
public:
    AafwkBrowserClientAdapter() = default;
    virtual ~AafwkBrowserClientAdapter() = default;
    virtual void* QueryRenderSurface(int32_t surface_id) = 0;

    virtual void ReportThread(
        ResSchedStatusAdapter status, int32_t process_id, int32_t thread_id, ResSchedRoleAdapter role) = 0;

    virtual void PassSurface(int64_t surface_id) = 0;

    virtual void DestroyRenderSurface(int32_t surface_id) = 0;

    virtual void DestroyPassedSurface(int64_t surface_id) = 0;
};
} // namespace OHOS::NWeb

#endif // AAFWK_BROWSER_CLIENT_ADAPTER_H
