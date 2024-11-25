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

#include "ohos_adapter/bridge/ark_window_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkWindowAdapterWrapper::ArkWindowAdapterWrapper(ArkWebRefPtr<ArkWindowAdapter> ref) : ctocpp_(ref) {}

ArkWebNativeWindow ArkWindowAdapterWrapper::CreateNativeWindowFromSurface(void* pSurface)
{
    if (!ctocpp_) {
        return nullptr;
    }
    return ctocpp_->CreateNativeWindowFromSurface(pSurface);
}

void ArkWindowAdapterWrapper::DestroyNativeWindow(ArkWebNativeWindow window)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->DestroyNativeWindow(window);
}

int32_t ArkWindowAdapterWrapper::NativeWindowSetBufferGeometry(ArkWebNativeWindow window, int32_t width, int32_t height)
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->NativeWindowSetBufferGeometry(window, width, height);
}

void ArkWindowAdapterWrapper::NativeWindowSurfaceCleanCache(ArkWebNativeWindow window)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->NativeWindowSurfaceCleanCache(window);
}

void ArkWindowAdapterWrapper::NativeWindowSurfaceCleanCacheWithPara(ArkWebNativeWindow window, bool cleanAll)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->NativeWindowSurfaceCleanCacheWithPara(window, cleanAll);
}

void ArkWindowAdapterWrapper::SetTransformHint(uint32_t rotation, ArkWebNativeWindow window)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->SetTransformHint(rotation, window);
}
} // namespace OHOS::ArkWeb
