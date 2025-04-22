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

#include "nweb_log.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/surface.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/window.h"
#include "foundation/graphic/graphic_surface/surface/include/native_window.h"
#include "foundation/graphic/graphic_surface/interfaces/inner_api/surface/external_window.h"

namespace OHOS::NWeb {
constexpr uint32_t ROTATE_NONE = 0;
constexpr uint32_t ROTATE_90 = 90;
constexpr uint32_t ROTATE_180 = 180;
constexpr uint32_t ROTATE_270 = 270;
GraphicTransformType ConvertRotation(uint32_t rotation)
{
    GraphicTransformType transform = GraphicTransformType::GRAPHIC_ROTATE_BUTT;
    switch (rotation) {
        case ROTATE_NONE:
            transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
            break;
        case ROTATE_90:
            transform = GraphicTransformType::GRAPHIC_ROTATE_90;
            break;
        case ROTATE_180:
            transform = GraphicTransformType::GRAPHIC_ROTATE_180;
            break;
        case ROTATE_270:
            transform = GraphicTransformType::GRAPHIC_ROTATE_270;
            break;
        default:
            transform = GraphicTransformType::GRAPHIC_ROTATE_NONE;
            break;
    }
    return transform;
}

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
    uint64_t usage = BUFFER_USAGE_MEM_DMA;
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
    auto nativeWindow = reinterpret_cast<OHNativeWindow *>(window);
    if (!nativeWindow || !nativeWindow->surface) {
        WVLOG_D("window or surface is null, no need to clean surface cache");
        return;
    }
    nativeWindow->surface->CleanCache();
}

void WindowAdapterImpl::NativeWindowSurfaceCleanCacheWithPara(NWebNativeWindow window, bool cleanAll)
{
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    if (!nativeWindow || !nativeWindow->surface) {
        WVLOG_D("window or surface is null, no need to clean surface cache");
        return;
    }
    WVLOG_D("NativeWindowSurfaceCleanCacheWithPara");

    // eglDestroySurface has disconnected the surface link
    GSError ret = nativeWindow->surface->Connect();
    if (ret == (int32_t)GSERROR_OK) {
        nativeWindow->surface->CleanCache(cleanAll);
        nativeWindow->surface->Disconnect();
    }
}

void WindowAdapterImpl::SetTransformHint(uint32_t rotation, NWebNativeWindow window)
{
    auto nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    if (!nativeWindow || !nativeWindow->surface) {
        WVLOG_E("window or buffer is null, fail to set buffer rotation");
        return;
    }

    auto transform = ConvertRotation(rotation);
    nativeWindow->surface->SetTransformHint(transform);
}

void WindowAdapterImpl::AddNativeWindowRef(NWebNativeWindow window)
{
    int32_t ret = OH_NativeWindow_NativeObjectReference(reinterpret_cast<OHNativeWindow*>(window));
    if (ret != 0) {
        WVLOG_E("add window reference failed.");
    }
}

void WindowAdapterImpl::NativeWindowUnRef(NWebNativeWindow window)
{
    int32_t ret = OH_NativeWindow_NativeObjectUnreference(reinterpret_cast<OHNativeWindow*>(window));
    if (ret != 0) {
        WVLOG_E("cancel window reference failed.");
    }
}
} // namespace OHOS::NWeb
