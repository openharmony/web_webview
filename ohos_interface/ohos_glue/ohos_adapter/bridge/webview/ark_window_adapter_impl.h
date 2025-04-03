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

#ifndef ARK_WINDOW_ADAPTER_IMPL_H
#define ARK_WINDOW_ADAPTER_IMPL_H
#pragma once

#include "graphic_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

class ArkWindowAdapterImpl : public ArkWindowAdapter {
public:
    explicit ArkWindowAdapterImpl(NWeb::WindowAdapter&);

    void* CreateNativeWindowFromSurface(void* pSurface) override;

    void DestroyNativeWindow(void* window) override;

    int32_t NativeWindowSetBufferGeometry(void* window, int32_t width, int32_t height) override;

    void NativeWindowSurfaceCleanCache(void* window) override;

    void NativeWindowSurfaceCleanCacheWithPara(void* window, bool cleanAll) override;

    void AddNativeWindowRef(void* window) override;

    void NativeWindowUnRef(void* window) override;

private:
    NWeb::WindowAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkWindowAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_WINDOW_ADAPTER_IMPL_H
