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

#include "ark_aafwk_browser_host_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkAafwkBrowserHostAdapterImpl::ArkAafwkBrowserHostAdapterImpl(std::shared_ptr<OHOS::NWeb::AafwkBrowserHostAdapter> ref)
    : real_(ref)
{}

void* ArkAafwkBrowserHostAdapterImpl::GetSurfaceFromKernel(int32_t surface_id)
{
    return real_->GetSurfaceFromKernel(surface_id);
}

void ArkAafwkBrowserHostAdapterImpl::DestroySurfaceFromKernel(int32_t surface_id)
{
    real_->DestroySurfaceFromKernel(surface_id);
}

} // namespace OHOS::ArkWeb