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

#include "ohos_adapter/bridge/ark_hiviewdfx_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkHiViewDfxAdapterImpl::ArkHiViewDfxAdapterImpl(NWeb::HiViewDfxAdapter& ref) : real_(ref) {}

void ArkHiViewDfxAdapterImpl::DfxSetSubmitterStackId(uint64_t ctx)
{
    real_.DfxSetSubmitterStackId(ctx);
}

void ArkHiViewDfxAdapterImpl::DfxPopSubmitterStackId(uint64_t ctx)
{
    real_.DfxPopSubmitterStackId(ctx);
}

void ArkHiViewDfxAdapterImpl::ReleaseAsyncContext(uint64_t ctx)
{
    real_.ReleaseAsyncContext(ctx);
}
} // namespace OHOS::ArkWeb
