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

#include <native_window/external_window.h>
#include <native_buffer/native_buffer.h>

#include "nweb_log.h"


namespace OHOS::NWeb {

WindowAdapterImpl& WindowAdapterImpl::GetInstance()
{
    static WindowAdapterImpl instance;
    return instance;
}

NWebNativeWindow WindowAdapterImpl::CreateNativeWindowFromSurface(void* pSurface)
{
    WVLOG_D("CreateNativeWindowFromSurface");
    OHNativeWindow* window = OH_NativeWindow_CreateNativeWindow(pSurface);
    if (!window) {
        return nullptr;
    }
    int32_t usage = NATIVEBUFFER_USAGE_MEM_DMA;
    OH_NativeWindow_NativeWindowHandleOpt(window, SET_USAGE, usage);
    return reinterpret_cast<NWebNativeWindow>(window);
}

void WindowAdapterImpl::DestroyNativeWindow(NWebNativeWindow window)
{
    WVLOG_D("DestroyWindow");
    OH_NativeWindow_DestroyNativeWindow(reinterpret_cast<OHNativeWindow*>(window));
}

int32_t WindowAdapterImpl::NativeWindowSetBufferGeometry(NWebNativeWindow window, int32_t width, int32_t height)
{
    WVLOG_D("NativeWindowSetBufferGeometry %{public}d * %{public}d", width, height);
    return OH_NativeWindow_NativeWindowHandleOpt(reinterpret_cast<OHNativeWindow*>(window),
        SET_BUFFER_GEOMETRY, width, height);
}

void WindowAdapterImpl::NativeWindowSurfaceCleanCache(NWebNativeWindow window)
{
    WVLOG_D("[adapter mock] WindowAdapterImpl::NativeWindowSurfaceCleanCache");
}

void WindowAdapterImpl::NativeWindowSurfaceCleanCacheWithPara(NWebNativeWindow window, bool cleanAll)
{
    WVLOG_D("[adapter mock] WindowAdapterImpl::NativeWindowSurfaceCleanCacheWithPara");
}

void WindowAdapterImpl::AddNativeWindowRef(NWebNativeWindow window)
{
    if (window == nullptr) {
        WVLOG_E("window is nullptr.");
        return;
    } 
    int32_t ret = OH_NativeWindow_NativeObjectReference(window);
    if (ret != 0) {
        WVLOG_E("add window reference failed.");
    }
}

void WindowAdapterImpl::NativeWindowUnRef(NWebNativeWindow window)
{
    if (window == nullptr) {
        WVLOG_E("window is nullptr.");
        return;
    } 
    int32_t ret = OH_NativeWindow_NativeObjectUnreference(window);
    if (ret != 0) {
        WVLOG_E("cancel window reference failed.");
    }
}
} // namespace OHOS::NWeb
