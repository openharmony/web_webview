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

#include "ohos_adapter/bridge/ark_window_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWindowAdapterImpl::ArkWindowAdapterImpl(NWeb::WindowAdapter& ref) : real_(ref) {}

void* ArkWindowAdapterImpl::CreateNativeWindowFromSurface(void* pSurface)
{
    return real_.CreateNativeWindowFromSurface(pSurface);
}

void ArkWindowAdapterImpl::DestroyNativeWindow(void* window)
{
    return real_.DestroyNativeWindow(window);
}

int32_t ArkWindowAdapterImpl::NativeWindowSetBufferGeometry(void* window, int32_t width, int32_t height)
{
    return real_.NativeWindowSetBufferGeometry(window, width, height);
}

void ArkWindowAdapterImpl::NativeWindowSurfaceCleanCache(void* window)
{
    return real_.NativeWindowSurfaceCleanCache(window);
}

void ArkWindowAdapterImpl::NativeWindowSurfaceCleanCacheWithPara(void* window, bool cleanAll)
{
    return real_.NativeWindowSurfaceCleanCacheWithPara(window, cleanAll);
}

void ArkWindowAdapterImpl::SetTransformHint(uint32_t rotation, void* window)
{
    return real_.SetTransformHint(rotation, window);
}

void ArkWindowAdapterImpl::AddNativeWindowRef(void* window)
{
    return real_.AddNativeWindowRef(window);
}

void ArkWindowAdapterImpl::NativeWindowUnRef(void* window)
{
    return real_.NativeWindowUnRef(window);
}
} // namespace OHOS::ArkWeb
