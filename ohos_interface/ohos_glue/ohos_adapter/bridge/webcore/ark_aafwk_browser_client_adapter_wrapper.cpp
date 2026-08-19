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

#include "ark_aafwk_browser_client_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkAafwkBrowserClientAdapterWrapper::ArkAafwkBrowserClientAdapterWrapper(ArkWebRefPtr<ArkAafwkBrowserClientAdapter> ref)
    : ctocpp_(ref)
{}

void* ArkAafwkBrowserClientAdapterWrapper::QueryRenderSurface(int32_t surface_id)
{
    return ctocpp_->QueryRenderSurface(surface_id);
}

void ArkAafwkBrowserClientAdapterWrapper::ReportThread(
    NWeb::ResSchedStatusAdapter status, int32_t process_id, int32_t thread_id, NWeb::ResSchedRoleAdapter role)
{
    ctocpp_->ReportThread((int32_t)status, process_id, thread_id, (int32_t)role);
}

void ArkAafwkBrowserClientAdapterWrapper::PassSurface(int64_t surface_id)
{
    ctocpp_->PassSurface(surface_id);
}

void ArkAafwkBrowserClientAdapterWrapper::DestroyRenderSurface(int32_t surface_id)
{
    ctocpp_->DestroyRenderSurface(surface_id);
}

void ArkAafwkBrowserClientAdapterWrapper::DestroyPassedSurface(int64_t surface_id)
{
    ctocpp_->DestroyPassedSurface(surface_id);
}

} // namespace OHOS::ArkWeb