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

#include "ark_surface_control_adapter_wrapper.h"
#include "graphic_adapter.h"
#include "ohos_adapter/include/ark_graphic_adapter.h"

namespace OHOS::ArkWeb {

ArkSurfaceControlAdapterWrapper::ArkSurfaceControlAdapterWrapper(
    ArkWebRefPtr<ArkSurfaceControlAdapter> ref) : ctocpp_(ref) {}

void* ArkSurfaceControlAdapterWrapper::FromNativeWindow(void* parent, const char* name)
{
    return ctocpp_->FromNativeWindow(parent, name);
}

void* ArkSurfaceControlAdapterWrapper::Create(const char* name)
{
    return ctocpp_->Create(name);
}

void ArkSurfaceControlAdapterWrapper::Release(void* surfaceControl)
{
    ctocpp_->Release(surfaceControl);
}

void ArkSurfaceControlAdapterWrapper::SetWindowNodeIdAndRSHandle(
    void* nativeWindow, uint64_t nodeId, uintptr_t handle)
{
    ctocpp_->SetWindowNodeIdAndRSHandle(nativeWindow, nodeId, handle);
}

// transaction
ArkSurfaceTransactionAdapterWrapper::ArkSurfaceTransactionAdapterWrapper(
    ArkWebRefPtr<ArkSurfaceTransactionAdapter> ref) : ctocpp_(ref) {}

void* ArkSurfaceTransactionAdapterWrapper::Create()
{
    return ctocpp_->Create();
}

void ArkSurfaceTransactionAdapterWrapper::DeleteTransaction(void* transaction)
{
    ctocpp_->DeleteTransaction(transaction);
}

void ArkSurfaceTransactionAdapterWrapper::Commit(void* transaction)
{
    ctocpp_->Commit(transaction);
}

void ArkSurfaceTransactionAdapterWrapper::SetOnComplete(
    void* transaction, void* context, OHOS::NWeb::SurfaceControlOnCompleteCallback func)
{
    ctocpp_->SetOnComplete(transaction, context, reinterpret_cast<void*>(func));
}

void ArkSurfaceTransactionAdapterWrapper::Reparent(void* transaction, void* surfaceControl, void* newParent)
{
    ctocpp_->Reparent(transaction, surfaceControl, newParent);
}

void ArkSurfaceTransactionAdapterWrapper::SetVisibility(void* transaction, void* surfaceControl, int8_t visibility)
{
    ctocpp_->SetVisibility(transaction, surfaceControl, visibility);
}

void ArkSurfaceTransactionAdapterWrapper::SetZOrder(void* transaction, void* surfaceControl, int32_t zOrder)
{
    ctocpp_->SetZOrder(transaction, surfaceControl, zOrder);
}

void ArkSurfaceTransactionAdapterWrapper::SetBuffer(void* transaction, void* surfaceControl, void* buffer,
    int acquireFenceFd, void* context, OHOS::NWeb::SurfaceControlSetBufferCallback func)
{
    ctocpp_->SetBuffer(transaction, surfaceControl, buffer, acquireFenceFd, context, reinterpret_cast<void*>(func));
}

void ArkSurfaceTransactionAdapterWrapper::SetBounds(void* transaction, void* surfaceControl,
    float x, float y, float w, float h)
{
    ctocpp_->SetBounds(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterWrapper::SetFrame(void* transaction, void* surfaceControl,
    float x, float y, float w, float h)
{
    ctocpp_->SetFrame(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterWrapper::SetPivot(void* transaction, void* surfaceControl, float x, float y)
{
    ctocpp_->SetPivot(transaction, surfaceControl, x, y);
}

void ArkSurfaceTransactionAdapterWrapper::SetBufferTransform(void* transaction, void* surfaceControl, int32_t transform)
{
    ctocpp_->SetBufferTransform(transaction, surfaceControl, transform);
}

void ArkSurfaceTransactionAdapterWrapper::SetScale(
    void* transaction, void* surfaceControl, float scaleX, float scaleY, float scaleZ)
{
    ctocpp_->SetScale(transaction, surfaceControl, scaleX, scaleY, scaleZ);
}

void ArkSurfaceTransactionAdapterWrapper::SetTranslate(void* transaction, void* surfaceControl,
    float translateX, float translateY, float translateZ)
{
    ctocpp_->SetTranslate(transaction, surfaceControl, translateX, translateY, translateZ);
}

void ArkSurfaceTransactionAdapterWrapper::SetDamageRegion(
    void* transaction, void* surfaceControl, const void* rects, uint32_t count)
{
    ctocpp_->SetDamageRegion(transaction, surfaceControl, rects, count);
}

void ArkSurfaceTransactionAdapterWrapper::SetBufferAlpha(void* transaction, void* surfaceControl, float alpha)
{
    ctocpp_->SetBufferAlpha(transaction, surfaceControl, alpha);
}

void ArkSurfaceTransactionAdapterWrapper::SetForegroundColor(void* transaction, void* surfaceControl,
    float red, float green, float blue, float alpha)
{
    ctocpp_->SetForegroundColor(transaction, surfaceControl, red, green, blue, alpha);
}

void ArkSurfaceTransactionAdapterWrapper::SetBackgroundColor(void* transaction, void* surfaceControl,
    float red, float green, float blue, float alpha)
{
    ctocpp_->SetBackgroundColor(transaction, surfaceControl, red, green, blue, alpha);
}

void ArkSurfaceTransactionAdapterWrapper::SetBorderColor(void* transaction, void* surfaceControl,
    float red, float green, float blue, float alpha)
{
    ctocpp_->SetBorderColor(transaction, surfaceControl, red, green, blue, alpha);
}

void ArkSurfaceTransactionAdapterWrapper::SetBorderWidth(void* transaction, void* surfaceControl,
    float left, float top, float right, float bottom)
{
    ctocpp_->SetBorderWidth(transaction, surfaceControl, left, top, right, bottom);
}

void ArkSurfaceTransactionAdapterWrapper::SetBorderStyle(void* transaction, void* surfaceControl,
    uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    ctocpp_->SetBorderStyle(transaction, surfaceControl, left, top, right, bottom);
}

void ArkSurfaceTransactionAdapterWrapper::SetHardwareEnableHint(void* transaction, void* surfaceControl, bool enable)
{
    ctocpp_->SetHardwareEnableHint(transaction, surfaceControl, enable);
}

void ArkSurfaceTransactionAdapterWrapper::SetName(void* transaction, void* surfaceControl, const char* name)
{
    ctocpp_->SetName(transaction, surfaceControl, name);
}

void ArkSurfaceTransactionAdapterWrapper::SetFrameGravity(void* transaction, void* surfaceControl, int32_t gravity)
{
    ctocpp_->SetFrameGravity(transaction, surfaceControl, gravity);
}

void ArkSurfaceTransactionAdapterWrapper::SetSrcRect(
    void* transaction, void* surfaceControl, float x, float y, float w, float h)
{
    ctocpp_->SetSrcRect(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterWrapper::SetDisplayRect(void* transaction, void* surfaceControl,
    float x, float y, float w, float h)
{
    ctocpp_->SetDisplayRect(transaction, surfaceControl, x, y, w, h);
}

void ArkSurfaceTransactionAdapterWrapper::ClearBufferQueueCache(void* transaction, void* surfaceControl, bool cleanAll)
{
    ctocpp_->ClearBufferQueueCache(transaction, surfaceControl, cleanAll);
}
} // namespace OHOS::ArkWeb