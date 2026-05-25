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

#ifndef ARK_SURFACE_CONTROL_ADAPTER_H
#define ARK_SURFACE_CONTROL_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkSurfaceControlAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkSurfaceControlAdapter() = default;

    virtual ~ArkSurfaceControlAdapter() = default;

    /*--ark web()--*/
    void* FromNativeWindow(void* parent, const char* name);

    /*--ark web()--*/
    void* Create(const char* name);

    /*--ark web()--*/
    void Release(void* surface_control);

    /*--ark web()--*/
    void SetWindowNodeIdAndRSHandle(void* native_window, uint64_t node_id, uintptr_t handle);
};

/*--ark web(source=webview)--*/
class ArkSurfaceTransactionAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkSurfaceTransactionAdapter() = default;

    virtual ~ArkSurfaceTransactionAdapter() = default;

    /*--ark web()--*/
    void* Create();

    /*--ark web()--*/
    void DeleteTransaction(void* transaction);

    /*--ark web()--*/
    void Commit(void* transaction);

    /*--ark web()--*/
    void SetOnComplete(void* transaction, void* context, void* func);

    /*--ark web()--*/
    void Reparent(void* transaction, void* surface_control, void* new_parent);

    /*--ark web()--*/
    void SetVisibility(void* transaction, void* surface_control, int8_t visibility);

    /*--ark web()--*/
    void SetZOrder(void* transaction, void* surface_control, int32_t z_order);

    /*--ark web()--*/
    void SetBuffer(
        void* transaction, void* surface_control, void* buffer, int acquire_fence_fd, void* context, void* func);

    /*--ark web()--*/
    void SetBounds(void* transaction, void* surface_control, float x, float y, float w, float h);

    /*--ark web()--*/
    void SetFrame(void* transaction, void* surface_control, float x, float y, float w, float h);

    /*--ark web()--*/
    void SetPivot(void* transaction, void* surface_control, float x, float y);

    /*--ark web()--*/
    void SetBufferTransform(void* transaction, void* surface_control, int32_t transform);

    /*--ark web()--*/
    void SetScale(void* transaction, void* surface_control, float scaleX, float scaleY, float scaleZ);

    /*--ark web()--*/
    void SetTranslate(void* transaction, void* surface_control, float translateX, float translateY, float translateZ);

    /*--ark web()--*/
    void SetDamageRegion(void* transaction, void* surface_control, const void* rects, uint32_t count);

    /*--ark web()--*/
    void SetBufferAlpha(void* transaction, void* surface_control, float alpha);

    /*--ark web()--*/
    void SetForegroundColor(void* transaction, void* surface_control, float red, float green, float blue, float alpha);

    /*--ark web()--*/
    void SetBackgroundColor(void* transaction, void* surface_control, float red, float green, float blue, float alpha);

    /*--ark web()--*/
    void SetBorderColor(void* transaction, void* surface_control, float red, float green, float blue, float alpha);

    /*--ark web()--*/
    void SetBorderWidth(void* transaction, void* surface_control, float left, float top, float right, float bottom);

    /*--ark web()--*/
    void SetBorderStyle(
        void* transaction, void* surface_control, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);

    /*--ark web()--*/
    void SetHardwareEnableHint(void* transaction, void* surface_control, bool enable);

    /*--ark web()--*/
    void SetName(void* transaction, void* surface_control, const char* name);

    /*--ark web()--*/
    void SetFrameGravity(void* transaction, void* surface_control, int32_t gravity);

    /*--ark web()--*/
    void SetSrcRect(void* transaction, void* surface_control, float x, float y, float w, float h);

    /*--ark web()--*/
    void SetDisplayRect(void* transaction, void* surface_control, float x, float y, float w, float h);

    /*--ark web()--*/
    void ClearBufferQueueCache(void* transaction, void* surface_control, bool cleanAll);
};

} // namespace OHOS::ArkWeb

#endif // ARK_SURFACE_CONTROL_ADAPTER_H