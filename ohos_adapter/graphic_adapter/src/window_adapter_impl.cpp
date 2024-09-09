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

#include "window_adapter_impl.h"

#include <cstdarg>

#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface.h"
#include "foundation/graphic/graphic_surface/surface/include/native_window.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

WindowAdapterImpl& WindowAdapterImpl::GetInstance()
{
    static WindowAdapterImpl instance;
    return instance;
}

NWebNativeWindow WindowAdapterImpl::CreateNativeWindowFromSurface(void* pSurface)
{
    OHNativeWindow* window = ::CreateNativeWindowFromSurface(pSurface);
    if (!window) {
        return nullptr;
    }
    int32_t usage = BUFFER_USAGE_MEM_DMA;
    NativeWindowHandleOpt(window, SET_USAGE, usage);
    return reinterpret_cast<NWebNativeWindow>(window);
}

void WindowAdapterImpl::DestroyNativeWindow(NWebNativeWindow window)
{
    ::DestoryNativeWindow(reinterpret_cast<OHNativeWindow*>(window));
}

int32_t WindowAdapterImpl::NativeWindowSetBufferGeometry(NWebNativeWindow window, int32_t width, int32_t height)
{
    return ::NativeWindowHandleOpt(reinterpret_cast<OHNativeWindow*>(window), SET_BUFFER_GEOMETRY, width, height);
}

void WindowAdapterImpl::NativeWindowSurfaceCleanCache(NWebNativeWindow window)
{
    WVLOG_D("WindowAdapterImpl::NativeWindowSurfaceCleanCache");
    reinterpret_cast<OHNativeWindow*>(window)->surface->CleanCache();
}

void WindowAdapterImpl::NativeWindowSurfaceCleanCacheWithPara(NWebNativeWindow window, bool cleanAll)
{
    WVLOG_D("WindowAdapterImpl::NativeWindowSurfaceCleanCacheWithPara");
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    if (!nativeWindow || !nativeWindow->surface) {
        WVLOG_D("window or surface is null, no need to clean surface cache");
        return;
    }
    
    // eglDestroySurface has disconnected the surface link
    GSError ret = nativeWindow->surface->Connect();
    if (ret == (int32_t)GSERROR_OK) {
        nativeWindow->surface->CleanCache(cleanAll);
        nativeWindow->surface->Disconnect();
    }
}
} // namespace OHOS::NWeb
