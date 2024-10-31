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

#ifndef ARK_OHOS_NATIVE_BUFFER_ADAPTER_WRAPPER_H
#define ARK_OHOS_NATIVE_BUFFER_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_ohos_native_buffer_adapter.h"
#include "ohos_native_buffer_adapter.h"

namespace OHOS::ArkWeb {
using namespace OHOS::NWeb;

class ArkOhosNativeBufferAdapterWrapper : public OHOS::NWeb::OhosNativeBufferAdapter {
public:
    ArkOhosNativeBufferAdapterWrapper(ArkWebRefPtr<ArkOhosNativeBufferAdapter>);

    void AcquireBuffer(void* buffer) override;
    void Release(void* buffer) override;
    int GetEGLBuffer(void* buffer, void** eglBuffer) override;
    int FreeEGLBuffer(void* eglBuffer) override;
    int NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer) override;
    uint32_t GetSeqNum(void* nativeBuffer) override;

private:
    ArkWebRefPtr<ArkOhosNativeBufferAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_NATIVE_BUFFER_ADAPTER_WRAPPER_H
