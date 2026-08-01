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

#include "ohos_adapter/bridge/ark_hiviewdfx_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkHiViewDfxAdapterWrapper::ArkHiViewDfxAdapterWrapper(ArkWebRefPtr<ArkHiViewDfxAdapter> ref) : ctocpp_(ref) {}

void ArkHiViewDfxAdapterWrapper::DfxSetSubmitterStackId(uint64_t ctx)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->DfxSetSubmitterStackId(ctx);
}

void ArkHiViewDfxAdapterWrapper::DfxPopSubmitterStackId(uint64_t ctx)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->DfxPopSubmitterStackId(ctx);
}

void ArkHiViewDfxAdapterWrapper::ReleaseAsyncContext(uint64_t ctx)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->ReleaseAsyncContext(ctx);
}

uint64_t ArkHiViewDfxAdapterWrapper::DfxCollectAsyncStack()
{
    if (!ctocpp_) {
        return 0;
    }
    return ctocpp_->DfxCollectAsyncStack();
}
} // namespace OHOS::ArkWeb
