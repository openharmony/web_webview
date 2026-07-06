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

#include "ohos_adapter/bridge/ark_surface_control_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {
ArkSurfaceControlAdapterImpl::ArkSurfaceControlAdapterImpl(NWeb::SurfaceControlAdapter& ref)
    : real_(ref) {}

void* ArkSurfaceControlAdapterImpl::CreateSurfaceControlFromNativeWindow(void* parent, const char* name)
{
    return real_.CreateSurfaceControlFromNativeWindow(parent, name);
}

void* ArkSurfaceControlAdapterImpl::Create(const char* name)
{
    return real_.Create(name);
}

void ArkSurfaceControlAdapterImpl::Release(void* surfaceControl)
{
    return real_.Release(surfaceControl);
}

// transaction
ArkSurfaceTransactionAdapterImpl::ArkSurfaceTransactionAdapterImpl(NWeb::SurfaceTransactionAdapter& ref)
    : real_(ref) {}

void* ArkSurfaceTransactionAdapterImpl::CreateSurfaceTransaction(void* nativeWindow)
{
    return real_.CreateSurfaceTransaction(nativeWindow);
}

void ArkSurfaceTransactionAdapterImpl::DeleteTransaction(void* transaction)
{
    real_.DeleteTransaction(transaction);
}

void ArkSurfaceTransactionAdapterImpl::Commit(void* transaction)
{
    real_.Commit(transaction);
}

void ArkSurfaceTransactionAdapterImpl::SetOnComplete(void* transaction, void* context, void* func)
{
    real_.SetOnComplete(
        transaction, context, reinterpret_cast<NWeb::SurfaceControlOnCompleteCallback>(func));
}

void ArkSurfaceTransactionAdapterImpl::Reparent(void* transaction, void* surfaceControl, void* newParent)
{
    real_.Reparent(transaction, surfaceControl, newParent);
}

void ArkSurfaceTransactionAdapterImpl::SetVisibility(void* transaction, void* surfaceControl, int8_t visibility)
{
    real_.SetVisibility(transaction, surfaceControl, visibility);
}

void ArkSurfaceTransactionAdapterImpl::SetZOrder(void* transaction, void* surfaceControl, int32_t zOrder)
{
    real_.SetZOrder(transaction, surfaceControl, zOrder);
}

void ArkSurfaceTransactionAdapterImpl::SetBuffer(void* transaction, void* surfaceControl, void* buffer,
    int acquireFenceFd, void* context, void* func)
{
    real_.SetBuffer(transaction, surfaceControl, buffer, acquireFenceFd, context,
        reinterpret_cast<NWeb::SurfaceControlSetBufferCallback>(func));
}

void ArkSurfaceTransactionAdapterImpl::SetBounds(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    real_.SetBounds(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterImpl::SetPivot(void* transaction, void* surfaceControl, float x, float y)
{
    real_.SetPivot(transaction, surfaceControl, x, y);
}

void ArkSurfaceTransactionAdapterImpl::SetBufferTransform(void* transaction, void* surfaceControl, int32_t transform)
{
    real_.SetBufferTransform(transaction, surfaceControl, transform);
}

void ArkSurfaceTransactionAdapterImpl::SetTranslate(
    void* transaction, void* surfaceControl, float translateX, float translateY, float translateZ)
{
    real_.SetTranslate(transaction, surfaceControl, translateX, translateY, translateZ);
}

void ArkSurfaceTransactionAdapterImpl::SetDamageRegion(
    void* transaction, void* surfaceControl, const void* rects, uint32_t count)
{
    real_.SetDamageRegion(transaction, surfaceControl, rects, count);
}

void ArkSurfaceTransactionAdapterImpl::SetBufferAlpha(void* transaction, void* surfaceControl, float alpha)
{
    real_.SetBufferAlpha(transaction, surfaceControl, alpha);
}

void ArkSurfaceTransactionAdapterImpl::SetForegroundColor(
    void* transaction, void* surfaceControl, float red, float green, float blue, float alpha)
{
    real_.SetForegroundColor(transaction, surfaceControl, red, green, blue, alpha);
}

void ArkSurfaceTransactionAdapterImpl::SetBackgroundColor(
    void* transaction, void* surfaceControl, float red, float green, float blue, float alpha)
{
    real_.SetBackgroundColor(transaction, surfaceControl, red, green, blue, alpha);
}

void ArkSurfaceTransactionAdapterImpl::SetBorderColor(
    void* transaction, void* surfaceControl, float red, float green, float blue, float alpha)
{
    real_.SetBorderColor(transaction, surfaceControl, red, green, blue, alpha);
}

void ArkSurfaceTransactionAdapterImpl::SetBorderWidth(
    void* transaction, void* surfaceControl, float left, float top, float right, float bottom)
{
    real_.SetBorderWidth(transaction, surfaceControl, left, top, right, bottom);
}

void ArkSurfaceTransactionAdapterImpl::SetBorderStyle(
    void* transaction, void* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    real_.SetBorderStyle(transaction, surfaceControl, left, top, right, bottom);
}

void ArkSurfaceTransactionAdapterImpl::SetHardwareEnableHint(void* transaction, void* surfaceControl, bool enable)
{
    real_.SetHardwareEnableHint(transaction, surfaceControl, enable);
}

void ArkSurfaceTransactionAdapterImpl::SetName(void* transaction, void* surfaceControl, const char* name)
{
    real_.SetName(transaction, surfaceControl, name);
}

void ArkSurfaceTransactionAdapterImpl::SetFrameGravity(void* transaction, void* surfaceControl, int32_t gravity)
{
    real_.SetFrameGravity(transaction, surfaceControl, gravity);
}

void ArkSurfaceTransactionAdapterImpl::SetSrcRect(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    real_.SetSrcRect(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterImpl::SetDisplayRect(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    real_.SetDisplayRect(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterImpl::ClearBufferQueueCache(void* transaction, void* surfaceControl, bool cleanAll)
{
    real_.ClearBufferQueueCache(transaction, surfaceControl, cleanAll);
}
} // namespace OHOS::ArkWeb