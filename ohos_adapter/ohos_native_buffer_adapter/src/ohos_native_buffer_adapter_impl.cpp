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

void OhosNativeBufferAdapterImpl::AcquireBuffer(void* buffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer acquire, buffer is null.");
        return;
    }
    WVLOG_D("native buffer acquired buffer %{public}p.", buffer);
    OH_NativeBuffer_Reference(static_cast<OH_NativeBuffer*>(buffer));
}

void OhosNativeBufferAdapterImpl::Release(void* buffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer release, buffer is null.");
        return;
    }

    WVLOG_D("native buffer release buffer %{public}p.", buffer);
    if (OH_NativeBuffer_Unreference(static_cast<OH_NativeBuffer*>(buffer)) == 0) {
        WVLOG_D("native buffer release, unreference buffer.");
    }
}

int OhosNativeBufferAdapterImpl::GetEGLBuffer(void* buffer, void** eglBuffer)
{
    if (buffer == nullptr) {
        WVLOG_E("native buffer get egl buffer, buffer is null.");
        return -1;
    }
    WVLOG_D("native buffer GetEGLBuffer %{public}p.", buffer);

    OHNativeWindowBuffer* nativeWindowBuffer =
        OH_NativeWindow_CreateNativeWindowBufferFromNativeBuffer(static_cast<OH_NativeBuffer*>(buffer));
    if (nativeWindowBuffer == nullptr) {
        WVLOG_E("native buffer failed to create native window buffer from native buffer.");
        return -1;
    } else {
        WVLOG_D("native buffer create native window buffer from native bufferL %{public}p.", *eglBuffer);
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

    WVLOG_D("native buffer free EGLBuffer %{public}p", eglBuffer);
    OH_NativeWindow_NativeObjectUnreference(eglBuffer);
    return 0;
}

int OhosNativeBufferAdapterImpl::NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer)
{
    if (nativeWindowBuffer == nullptr) {
        WVLOG_E("native buffer NativeBufferFromNativeWindowBuffer, native window buffer is null.");
        return -1;
    }

    *nativeBuffer = OH_NativeBufferFromNativeWindowBuffer(static_cast<NativeWindowBuffer*>(nativeWindowBuffer));
    if (*nativeBuffer == nullptr) {
        WVLOG_E("native buffer NativeBufferFromNativeWindowBuffer, native buffer is null.");
        return -1;
    }
    WVLOG_D("native buffer NativeBufferFromNativeWindowBuffer %{public}p", nativeWindowBuffer);
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
} // namespace OHOS::NWeb
