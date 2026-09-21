/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ark_vpe_video_adapter_wrapper.h"
#include "vpe_video_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

ArkVpeVideoAdapterWrapper::ArkVpeVideoAdapterWrapper(ArkWebRefPtr<ArkVpeVideoAdapter> ref) : ctocpp_(ref) {}

void* ArkVpeVideoAdapterWrapper::CreateVpeSurface(uint64_t surfaceId, void* window)
{
    return ctocpp_->CreateVpeSurface(surfaceId, window);
}

void ArkVpeVideoAdapterWrapper::ReleaseVpeSurface(uint64_t surfaceId)
{
    ctocpp_->ReleaseVpeSurface(surfaceId);
}

} // namespace OHOS::ArkWeb
