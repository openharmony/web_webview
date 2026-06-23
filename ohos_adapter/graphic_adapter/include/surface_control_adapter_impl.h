/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef SURFACE_CONTROL_ADAPTER_IMPL_H
#define SURFACE_CONTROL_ADAPTER_IMPL_H

#include "surface_control_adapter.h"

namespace OHOS::NWeb {
class SurfaceControlAdapterImpl : public SurfaceControlAdapter {
public:
    static SurfaceControlAdapter& GetInstance();

    SurfaceControlAdapterImpl() = default;

    ~SurfaceControlAdapterImpl() = default;

    void* CreateSurfaceControlFromNativeWindow(void* parent, const char* name) override;

    void* Create(const char* name) override;

    void Release(void* surfaceControl) override;
};

class SurfaceTransactionAdapterImpl : public SurfaceTransactionAdapter {
public:
    static SurfaceTransactionAdapter& GetInstance();

    SurfaceTransactionAdapterImpl() = default;

    ~SurfaceTransactionAdapterImpl() = default;

    void* CreateSurfaceTransaction(void* nativeWindow) override;

    void DeleteTransaction(void* transaction) override;

    void Commit(void* transaction) override;

    void SetOnComplete(void* transaction, void* context, SurfaceControlOnCompleteCallback func) override;

    void Reparent(void* transaction, void* surfaceControl, void* newParent) override;

    void SetVisibility(void* transaction, void* surfaceControl, int8_t visibility) override;

    void SetZOrder(void* transaction, void* surfaceControl, int32_t zOrder) override;

    void SetBuffer(void* transaction, void* surfaceControl, void* buffer, int acquireFenceFd, void* context,
        SurfaceControlSetBufferCallback func) override;

    void SetBounds(void* transaction, void* surfaceControl, float x, float y, float w, float h) override;

    void SetPivot(void* transaction, void* surfaceControl, float x, float y) override;

    void SetBufferTransform(void* transaction, void* surfaceControl, int32_t transform) override;

    void SetTranslate(
        void* transaction, void* surfaceControl, float translateX, float translateY, float translateZ) override;

    void SetDamageRegion(void* transaction, void* surfaceControl, const void* rects, uint32_t count) override;

    void SetBufferAlpha(void* transaction, void* surfaceControl, float alpha) override;

    void SetForegroundColor(
        void* transaction, void* surfaceControl, float red, float green, float blue, float alpha) override;

    void SetBackgroundColor(
        void* transaction, void* surfaceControl, float red, float green, float blue, float alpha) override;

    void SetBorderColor(
        void* transaction, void* surfaceControl, float red, float green, float blue, float alpha) override;

    void SetBorderWidth(
        void* transaction, void* surfaceControl, float left, float top, float right, float bottom) override;

    void SetBorderStyle(
        void* transaction, void* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) override;

    void SetHardwareEnableHint(void* transaction, void* surfaceControl, bool enable) override;

    void SetName(void* transaction, void* surfaceControl, const char* name) override;

    void SetFrameGravity(void* transaction, void* surfaceControl, int32_t gravity) override;

    void SetSrcRect(void* transaction, void* surfaceControl, float x, float y, float w, float h) override;

    void SetDisplayRect(void* transaction, void* surfaceControl, float x, float y, float w, float h) override;

    void ClearBufferQueueCache(void* transaction, void* surfaceControl, bool cleanAll) override;
};
} // namespace OHOS::NWeb
#endif // SURFACE_CONTROL_ADAPTER_IMPL_H