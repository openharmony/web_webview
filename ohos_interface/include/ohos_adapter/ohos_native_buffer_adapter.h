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
#include <memory>

namespace OHOS::NWeb {
class NativeBufferConfigAdapter {
public:
    NativeBufferConfigAdapter() = default;

    virtual ~NativeBufferConfigAdapter() = default;

    virtual int GetBufferWidth() = 0;

    virtual int GetBufferHeight() = 0;

    virtual int GetBufferFormat() = 0;

    virtual int GetBufferUsage() = 0;

    virtual int GetBufferStride() = 0;

    virtual void SetBufferWidth(int width) = 0;

    virtual void SetBufferHeight(int height) = 0;

    virtual void SetBufferFormat(int format) = 0;

    virtual void SetBufferUsage(int usage) = 0;

    virtual void SetBufferStride(int stride) = 0;
};

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

    virtual void Allocate(const std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void** outBuffer) = 0;

    virtual void Describe(std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void* buffer) = 0;

    virtual int Lock(void* buffer,
        uint64_t usage, int32_t fence, void** out_virtual_address) = 0;
    
    virtual int RecvHandleFromUnixSocket(int socketFd, void** outBuffer) = 0;

    virtual int SendHandleToUnixSocket(const void* buffer, int socketFd) = 0;

    virtual int Unlock(void* buffer, int32_t* fence) = 0;

    virtual int FreeNativeBuffer(void* nativeBuffer) = 0;

    // 未经过NWeb胶水层直接访问OhosNativeBufferAdapterImpl单例，提供默认实现
    virtual bool IsColorSpaceSupported(void* nativeBuffer) { return false; }
};

} // namespace OHOS::NWeb

#endif // OHOS_NATIVE_BUFFER_ADAPTER_H
