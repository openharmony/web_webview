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

#include <cstdarg>
#include "surface_control_adapter_impl.h"

#include "surface/window.h"
#include "native_window.h"
#include "nweb_log.h"
#include "surface_control/interface/oh_surface_control.h"

namespace OHOS::NWeb {
using namespace OHOS::Rosen;
SurfaceControlAdapter& SurfaceControlAdapterImpl::GetInstance()
{
    static SurfaceControlAdapterImpl instance;
    return instance;
}

void* SurfaceControlAdapterImpl::CreateSurfaceControlFromNativeWindow(void* parent, const char* name)
{
    auto* parentSurface = reinterpret_cast<OHNativeWindow*>(parent);
    return SurfaceControlUtils::CreateSurfaceControlFromNativeWindow(parentSurface, name);
}

void* SurfaceControlAdapterImpl::Create(const char* name)
{
    return SurfaceControlUtils::Create(name);
}

void SurfaceControlAdapterImpl::Release(void* surfaceControl)
{
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Release(surface);
}

// transaction
SurfaceTransactionAdapter& SurfaceTransactionAdapterImpl::GetInstance()
{
    static SurfaceTransactionAdapterImpl instance;
    return instance;
}

void* SurfaceTransactionAdapterImpl::CreateSurfaceTransaction(void* nativeWindow)
{
    return SurfaceControlUtils::Transaction::CreateSurfaceTransaction(reinterpret_cast<OHNativeWindow*>(nativeWindow));
}

void SurfaceTransactionAdapterImpl::DeleteTransaction(void* transaction)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    SurfaceControlUtils::Transaction::Delete(txn);
}

void SurfaceTransactionAdapterImpl::Commit(void* transaction)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    SurfaceControlUtils::Transaction::Commit(txn);
}

void SurfaceTransactionAdapterImpl::SetOnComplete(void* transaction, void* context,
    SurfaceControlOnCompleteCallback func)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* callback = reinterpret_cast<OH_SurfaceTransaction_OnComplete>(func);
    SurfaceControlUtils::Transaction::SetOnComplete(txn, context, callback);
}

void SurfaceTransactionAdapterImpl::Reparent(void* transaction, void* surfaceControl, void* newParent)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    auto* parent = reinterpret_cast<OH_SurfaceControl*>(newParent);
    SurfaceControlUtils::Transaction::Reparent(txn, surface, parent);
}

void SurfaceTransactionAdapterImpl::SetVisibility(void* transaction, void* surfaceControl, int8_t visibility)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    auto surfaceVisibility = static_cast<SurfaceControlUtils::Transaction::Visibility>(visibility);
    SurfaceControlUtils::Transaction::SetVisibility(txn, surface, surfaceVisibility);
}

void SurfaceTransactionAdapterImpl::SetZOrder(void* transaction, void* surfaceControl, int32_t zOrder)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetZOrder(txn, surface, zOrder);
}

void SurfaceTransactionAdapterImpl::SetBuffer(void* transaction, void* surfaceControl, void* buffer,
    int acquireFenceFd, void* context, SurfaceControlSetBufferCallback func)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    auto* nativeBuffer = reinterpret_cast<OH_NativeBuffer*>(buffer);
    auto* callback = reinterpret_cast<OH_SurfaceTransaction_OnBufferRelease>(func);
    SurfaceControlUtils::Transaction::SetBuffer(txn, surface, nativeBuffer, acquireFenceFd, context, callback);
}

void SurfaceTransactionAdapterImpl::SetBounds(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetBounds(txn, surface, x, y, w, h);
}

void SurfaceTransactionAdapterImpl::SetPivot(void* transaction, void* surfaceControl, float x, float y)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetPivot(txn, surface, x, y);
}

void SurfaceTransactionAdapterImpl::SetBufferTransform(void* transaction, void* surfaceControl, int32_t transform)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    auto surfaceTransform = static_cast<SurfaceControlUtils::Transaction::Transform>(transform);
    SurfaceControlUtils::Transaction::SetBufferTransform(txn, surface, surfaceTransform);
}

void SurfaceTransactionAdapterImpl::SetTranslate(
    void* transaction, void* surfaceControl, float translateX, float translateY, float translateZ)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetTranslate(txn, surface, translateX, translateY, translateZ);
}

void SurfaceTransactionAdapterImpl::SetDamageRegion(
    void* transaction, void* surfaceControl, const void* rects, uint32_t count)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    const OH_Rect* surfaceRects = reinterpret_cast<const OH_Rect*>(rects);
    SurfaceControlUtils::Transaction::SetDamageRegion(txn, surface, surfaceRects, count);
}

void SurfaceTransactionAdapterImpl::SetBufferAlpha(void* transaction, void* surfaceControl, float alpha)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetBufferAlpha(txn, surface, alpha);
}

void SurfaceTransactionAdapterImpl::SetForegroundColor(
    void* transaction, void* surfaceControl, float red, float green, float blue, float alpha)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetForegroundColor(txn, surface, red, green, blue, alpha);
}

void SurfaceTransactionAdapterImpl::SetBackgroundColor(
    void* transaction, void* surfaceControl, float red, float green, float blue, float alpha)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetBackgroundColor(txn, surface, red, green, blue, alpha);
}

void SurfaceTransactionAdapterImpl::SetBorderColor(
    void* transaction, void* surfaceControl, float red, float green, float blue, float alpha)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetBorderColor(txn, surface, red, green, blue, alpha);
}

void SurfaceTransactionAdapterImpl::SetBorderWidth(
    void* transaction, void* surfaceControl, float left, float top, float right, float bottom)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetBorderWidth(txn, surface, left, top, right, bottom);
}

void SurfaceTransactionAdapterImpl::SetBorderStyle(
    void* transaction, void* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetBorderStyle(txn, surface, left, top, right, bottom);
}

void SurfaceTransactionAdapterImpl::SetHardwareEnableHint(void* transaction,
    void* surfaceControl, bool enable)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetHardwareEnableHint(txn, surface, enable);
}

void SurfaceTransactionAdapterImpl::SetName(
    void* transaction, void* surfaceControl, const char* name)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetName(txn, surface, name);
}

void SurfaceTransactionAdapterImpl::SetFrameGravity(
    void* transaction, void* surfaceControl, int32_t gravity)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetFrameGravity(txn, surface, gravity);
}

void SurfaceTransactionAdapterImpl::SetSrcRect(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetSrcRect(txn, surface, x, y, w, h);
}

void SurfaceTransactionAdapterImpl::SetDisplayRect(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::SetDisplayRect(txn, surface, x, y, w, h);
}

void SurfaceTransactionAdapterImpl::ClearBufferQueueCache(void* transaction, void* surfaceControl, bool cleanAll)
{
    auto* txn = reinterpret_cast<OH_SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<OH_SurfaceControl*>(surfaceControl);
    SurfaceControlUtils::Transaction::ClearBufferQueueCache(txn, surface, cleanAll);
}

} // namespace OHOS::NWeb