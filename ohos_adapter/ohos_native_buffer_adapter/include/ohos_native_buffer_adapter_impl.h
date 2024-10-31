/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_NATIVE_BUFFER_ADAPTER_IMPL_H
#define OHOS_NATIVE_BUFFER_ADAPTER_IMPL_H

#include "ohos_native_buffer_adapter.h"

#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/external_window.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/native_buffer.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/native_buffer_inner.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface_buffer.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"

namespace OHOS::NWeb {

class OhosNativeBufferAdapterImpl : public OhosNativeBufferAdapter {
public:
    static OhosNativeBufferAdapter& GetInstance();

    OhosNativeBufferAdapterImpl();

    ~OhosNativeBufferAdapterImpl() override;

    void AcquireBuffer(void* buffer) override;

    void Release(void* buffer) override;

    int GetEGLBuffer(void* buffer, void** eglBuffer) override;

    int FreeEGLBuffer(void* eglBuffer) override;

    int NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer) override;

    uint32_t GetSeqNum(void* nativeBuffer) override;
};

} // namespace OHOS::NWeb

#endif // OHOS_NATIVE_BUFFER_ADAPTER_IMPL_H
