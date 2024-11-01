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

#ifndef OHOS_NATIVE_BUFFER_ADAPTER_H
#define OHOS_NATIVE_BUFFER_ADAPTER_H

#include <string>

namespace OHOS::NWeb {
class OhosNativeBufferAdapter {
public:
    OhosNativeBufferAdapter() = default;

    virtual ~OhosNativeBufferAdapter() = default;

    virtual void AcquireBuffer(void* buffer) = 0;

    virtual void Release(void* buffer) = 0;

    virtual int GetEGLBuffer(void* buffer, void** eglBuffer) = 0;

    virtual int FreeEGLBuffer(void* eglBuffer) = 0;

    virtual int NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer) = 0;

    virtual uint32_t GetSeqNum(void* nativeBuffer) = 0;
};

} // namespace OHOS::NWeb

#endif // OHOS_NATIVE_BUFFER_ADAPTER_H
