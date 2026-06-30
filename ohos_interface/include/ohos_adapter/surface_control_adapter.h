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
#ifndef SURFACE_CONTROL_ADAPTER_H
#define SURFACE_CONTROL_ADAPTER_H

#include <cstdint>
#include <string>

namespace OHOS::NWeb {

typedef void (*SurfaceControlOnCompleteCallback)(void*, uint64_t);
typedef void (*SurfaceControlSetBufferCallback)(void*, int);

class SurfaceControlAdapter {
public:
    SurfaceControlAdapter() = default;

    virtual ~SurfaceControlAdapter() = default;

    virtual void* CreateSurfaceControlFromNativeWindow(void* parent, const char* name) = 0;

    virtual void* Create(const char* name) = 0;

    virtual void Release(void* surfaceControl) = 0;
};

class SurfaceTransactionAdapter {
public:
    SurfaceTransactionAdapter() = default;

    virtual ~SurfaceTransactionAdapter() = default;

    virtual void* CreateSurfaceTransaction(void* nativeWindow) = 0;

    virtual void DeleteTransaction(void* transaction) = 0;

    virtual void Commit(void* transaction) = 0;

    virtual void SetOnComplete(void* transaction, void* context, SurfaceControlOnCompleteCallback func) = 0;

    virtual void Reparent(void* transaction, void* surfaceControl, void* newParent) = 0;

    virtual void SetVisibility(void* transaction, void* surfaceControl, int8_t visibility) = 0;

    virtual void SetZOrder(void* transaction, void* surfaceControl, int32_t zOrder) = 0;

    virtual void SetBuffer(void* transaction, void* surfaceControl, void* buffer, int acquireFenceFd, void* context,
        SurfaceControlSetBufferCallback func) = 0;

    virtual void SetBounds(void* transaction, void* surfaceControl, float x, float y, float w, float h) = 0;

    virtual void SetPivot(void* transaction, void* surfaceControl, float x, float y) = 0;

    virtual void SetBufferTransform(void* transaction, void* surfaceControl, int32_t transform) = 0;

    virtual void SetTranslate(
        void* transaction, void* surfaceControl, float translateX, float translateY, float translateZ) = 0;

    virtual void SetDamageRegion(void* transaction, void* surfaceControl, const void* rects, uint32_t count) = 0;

    virtual void SetBufferAlpha(void* transaction, void* surfaceControl, float alpha) = 0;

    virtual void SetForegroundColor(
        void* transaction, void* surfaceControl, float red, float green, float blue, float alpha) = 0;

    virtual void SetBackgroundColor(
        void* transaction, void* surfaceControl, float red, float green, float blue, float alpha) = 0;

    virtual void SetBorderColor(
        void* transaction, void* surfaceControl, float red, float green, float blue, float alpha) = 0;

    virtual void SetBorderWidth(
        void* transaction, void* surfaceControl, float left, float top, float right, float bottom) = 0;

    virtual void SetBorderStyle(
        void* transaction, void* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom) = 0;

    virtual void SetHardwareEnableHint(void* transaction, void* surfaceControl, bool enable) = 0;

    virtual void SetName(void* transaction, void* surfaceControl, const char* name) = 0;

    virtual void SetFrameGravity(void* transaction, void* surfaceControl, int32_t gravity) = 0;

    virtual void SetSrcRect(void* transaction, void* surfaceControl, float x, float y, float w, float h) = 0;

    virtual void SetDisplayRect(void* transaction, void* surfaceControl, float x, float y, float w, float h) = 0;

    virtual void ClearBufferQueueCache(void* transaction, void* surfaceControl, bool cleanAll) = 0;
};
} // namespace OHOS::NWeb
#endif // SURFACE_CONTROL_ADAPTER_H