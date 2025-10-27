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

#include "ohos_native_buffer_adapter_impl.h"
#include "surface_buffer_impl.h"
#include "nweb_log.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

namespace OHOS::NWeb {

OhosNativeBufferAdapter& OhosNativeBufferAdapterImpl::GetInstance()
{
    WVLOG_D("Native buffer adapter impl get instance.");
    static OhosNativeBufferAdapterImpl instance;
    return instance;
}

OhosNativeBufferAdapterImpl::OhosNativeBufferAdapterImpl()
{
    WVLOG_D("Native buffer adapter impl constructor.");
}

OhosNativeBufferAdapterImpl::~OhosNativeBufferAdapterImpl()
{
    WVLOG_D("Native buffer adapter impl destructor.");
}

//LCOV_EXCL_START
bool OhosNativeBufferAdapterImpl::IsBufferLocked(OH_NativeBuffer* buffer) const
{
    if (buffer == nullptr) {
        return false;
    }
    auto it = lockedBuffers_.find(buffer);
    bool result = (it != lockedBuffers_.end() && it->second);
    WVLOG_D("Native buffer is locked: %{public}s", result ? "yes" : "no");
    return result;
}

void OhosNativeBufferAdapterImpl::AcquireBuffer(void* buffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer acquire, buffer is null.");
        return;
    }
    WVLOG_D("native buffer acquired buffer %{private}p.", buffer);
    OH_NativeBuffer_Reference(static_cast<OH_NativeBuffer*>(buffer));
}

void OhosNativeBufferAdapterImpl::Release(void* buffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer release, buffer is null.");
        return;
    }

    WVLOG_D("native buffer release buffer %{private}p.", buffer);
    if (OH_NativeBuffer_Unreference(static_cast<OH_NativeBuffer*>(buffer)) == 0) {
        WVLOG_D("native buffer release, unreference buffer.");
    }

    buffer = nullptr;
}

int OhosNativeBufferAdapterImpl::GetEGLBuffer(void* buffer, void** eglBuffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer get egl buffer, buffer is null.");
        return -1;
    }
    if (eglBuffer == nullptr) {
        WVLOG_E("native buffer get egl buffer, eglBuffer pointer is null.");
        return -1;
    }
    WVLOG_D("native buffer GetEGLBuffer %{private}p.", buffer);

    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    if (nativeWindowBuffer == nullptr) {
        WVLOG_E("native buffer failed to create native window buffer from native buffer.");
        return -1;
    } else {
        WVLOG_D("native buffer create native window buffer from native bufferL %{private}p.", *eglBuffer);
        *eglBuffer = nativeWindowBuffer;
        return 0;
    }
}

int OhosNativeBufferAdapterImpl::FreeEGLBuffer(void* eglBuffer)
{
    if (eglBuffer == nullptr) {
        WVLOG_E("native buffer free EGLBuffer is null.");
        return -1;
    }

    WVLOG_D("native buffer free EGLBuffer %{private}p", eglBuffer);
    OH_NativeWindow_NativeObjectUnreference(eglBuffer);
    return 0;
}

int OhosNativeBufferAdapterImpl::NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer)
{
    if (nativeWindowBuffer == nullptr) {
        WVLOG_E("native buffer NativeBufferFromNativeWindowBuffer, native window buffer is null.");
        return -1;
    }

    if (nativeBuffer == nullptr) {
        WVLOG_E("output nativeBuffer pointer is null");
        return -1;
    }

    *nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(static_cast<NativeWindowBuffer*>(nativeWindowBuffer));
    if (*nativeBuffer == nullptr) {
        WVLOG_E("native buffer NativeBufferFromNativeWindowBuffer, native buffer is null.");
        return -1;
    }
    WVLOG_D("native buffer NativeBufferFromNativeWindowBuffer %{private}p", nativeWindowBuffer);
    return 0;
}

uint32_t OhosNativeBufferAdapterImpl::GetSeqNum(void* nativeBuffer)
{
    if (nativeBuffer == nullptr) {
        WVLOG_E("native buffer GetSeqNum, nativeBuffer is null.");
        return 0;
    }
    SurfaceBufferImpl* buffer = reinterpret_cast<SurfaceBufferImpl *>(nativeBuffer);
    return buffer->GetSeqNum();
}

void OhosNativeBufferAdapterImpl::Allocate(
    const std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void** outBuffer)
{
    if (bufferConfig == nullptr) {
        return;
    }
    if (outBuffer == nullptr) {
        WVLOG_E("Output buffer pointer is null");
        return;
    }
    int width = bufferConfig->GetBufferWidth();
    int height = bufferConfig->GetBufferHeight();
    int usage = bufferConfig->GetBufferUsage();
    OH_NativeBuffer_Config config = {
        .width = width,
        .height = height,
        .format = OH_NativeBuffer_Format::NATIVEBUFFER_PIXEL_FMT_RGBA_8888,
        .usage = usage,
    };

    // create a new OH_NativeBuffer using the OHOS native buffer allocation function
    // The plan here is that the actual buffer holder will be held onto by chromium.
    OH_NativeBuffer* buffer = OH_NativeBuffer_Alloc(&config);
    if (buffer != nullptr) {
        WVLOG_D("native buffer allocate success, rawbuffer stored %{private}p", buffer);
        *outBuffer = buffer;
    } else {
        WVLOG_E("native buffer allocate failed.");
        *outBuffer = nullptr;
    }
}

void OhosNativeBufferAdapterImpl::Describe(std::shared_ptr<NativeBufferConfigAdapter> bufferConfig, void* buffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer describe, buffer is null.");
        return;
    }
    WVLOG_D("native buffer describe buffer %{private}p.", buffer);

    OH_NativeBuffer_Config config = {};
    OH_NativeBuffer_GetConfig(static_cast<OH_NativeBuffer*>(buffer), &config);    

    bufferConfig->SetBufferWidth(config.width);
    bufferConfig->SetBufferHeight(config.height);
    bufferConfig->SetBufferFormat(config.format);
    bufferConfig->SetBufferUsage(config.usage);
    bufferConfig->SetBufferStride(config.stride);

    return;
}

int OhosNativeBufferAdapterImpl::Lock(void* buffer, uint64_t usage, int32_t fence, void** out_virtual_address)
{
    WVLOG_D("native buffer waiting for lock.");
    if (buffer == nullptr) {
        WVLOG_E("native buffer lock, buffer is null.");
        return -1;
    }

    if (out_virtual_address == nullptr) {
        WVLOG_E("output address pointer is null.");
        return -1;
    }

    if (IsBufferLocked(static_cast<OH_NativeBuffer*>(buffer))) {
        WVLOG_D("native buffer lock - buffer already locked.");
        return -1;
    }

    lockedBuffers_[static_cast<OH_NativeBuffer*>(buffer)] = true;

    return OH_NativeBuffer_Map(static_cast<OH_NativeBuffer*>(buffer), out_virtual_address);
}

int OhosNativeBufferAdapterImpl::RecvHandleFromUnixSocket(int socketFd, void** outBuffer)
{
    WVLOG_D("native buffer receive handle from unix socket.");
    return 0;
}

int OhosNativeBufferAdapterImpl::SendHandleToUnixSocket(const void* buffer, int socketFd)
{
    WVLOG_D("native buffer send handle to unix socket.");

    if (buffer == nullptr) {
        WVLOG_E("native buffer SendHandleToUnixSocket, buffer is null.");
        return -1;
    }

    return 0;
}

int OhosNativeBufferAdapterImpl::Unlock(void* buffer, int32_t* fence)
{
    WVLOG_D("native buffer waiting for unlock.");
    if (buffer == nullptr) {
        WVLOG_E("native buffer lock, buffer is null.");
        return -1;
    }

    if (!IsBufferLocked(static_cast<OH_NativeBuffer*>(buffer))) {
        WVLOG_D("native buffer unlock - buffer is already unlocked.");
        return -1;
    }

    int result = OH_NativeBuffer_Unmap(static_cast<OH_NativeBuffer*>(buffer));
    lockedBuffers_.erase(static_cast<OH_NativeBuffer*>(buffer));
    return result;
}

int OhosNativeBufferAdapterImpl::FreeNativeBuffer(void* nativeBuffer)
{
    if (nativeBuffer == nullptr) {
        WVLOG_E("native buffer FreeNativeBuffer, native buffer is null.");
        return -1;
    }
    WVLOG_D("native buffer FreeNativeBuffer freeing: %{private}p.", nativeBuffer);
    OH_NativeBuffer_Unreference(static_cast<OH_NativeBuffer*>(nativeBuffer));
    return 0;
}
//LCOV_EXCL_STOP
} // namespace OHOS::NWeb
