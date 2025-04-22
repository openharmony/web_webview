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

#ifndef WINDOW_ADAPTER_IMPL_H
#define WINDOW_ADAPTER_IMPL_H

#include "graphic_adapter.h"

namespace OHOS::NWeb {
class WindowAdapterImpl : public WindowAdapter {
public:
    static WindowAdapterImpl& GetInstance();

    ~WindowAdapterImpl() = default;

    NWebNativeWindow CreateNativeWindowFromSurface(void* pSurface) override;

    void DestroyNativeWindow(NWebNativeWindow window) override;

    int32_t NativeWindowSetBufferGeometry(NWebNativeWindow window, int32_t width, int32_t height) override;

    void NativeWindowSurfaceCleanCache(NWebNativeWindow window) override;

    void NativeWindowSurfaceCleanCacheWithPara(NWebNativeWindow window, bool cleanAll) override;

    void SetTransformHint(uint32_t rotation, NWebNativeWindow window) override;

    void AddNativeWindowRef(NWebNativeWindow window) override;

    void NativeWindowUnRef(NWebNativeWindow window) override;
};
} // namespace OHOS::NWeb

#endif // WINDOW_ADAPTER_IMPL_H
