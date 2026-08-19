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

#include "ark_aafwk_browser_client_adapter_impl.h"

namespace OHOS::ArkWeb {
ArkAafwkBrowserClientAdapterImpl::ArkAafwkBrowserClientAdapterImpl(
    std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> ref)
    : real_(ref)
{}

void* ArkAafwkBrowserClientAdapterImpl::QueryRenderSurface(int32_t surface_id)
{
    return real_->QueryRenderSurface(surface_id);
}

void ArkAafwkBrowserClientAdapterImpl::ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role)
{
    real_->ReportThread((NWeb::ResSchedStatusAdapter)status, process_id, thread_id, (NWeb::ResSchedRoleAdapter)role);
}

void ArkAafwkBrowserClientAdapterImpl::PassSurface(int64_t surface_id)
{
    real_->PassSurface(surface_id);
}

void ArkAafwkBrowserClientAdapterImpl::DestroyRenderSurface(int32_t surface_id)
{
    return real_->DestroyRenderSurface(surface_id);
}

void ArkAafwkBrowserClientAdapterImpl::DestroyPassedSurface(int64_t surface_id)
{
    return real_->DestroyPassedSurface(surface_id);
}

} // namespace OHOS::ArkWeb