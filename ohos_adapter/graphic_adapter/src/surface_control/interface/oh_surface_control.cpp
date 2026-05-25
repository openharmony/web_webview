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

#include "oh_surface_control.h"
#include <memory>
#include "surface_buffer.h"
#include "surface_type.h"
#include "../surface_transaction.h"
#include "nweb_log.h"

namespace OHOS {
namespace NWeb {
#define CHECK_NULL_POINTER(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            WVLOG_E(#ptr " is null pointer"); \
            return; \
        } \
    } while (0)

OH_SurfaceControl* SurfaceControlUtils::FromNativeWindow(OHNativeWindow* parent, const char* name)
{
    auto surface = SurfaceControl::CreateFromWindow(parent, name);
    if (surface) {
        surface->IncStrongRef(nullptr);
    }
    return reinterpret_cast<OH_SurfaceControl*>(surface.GetRefPtr());
}

OH_SurfaceControl* SurfaceControlUtils::Create(const char* name)
{
    auto surface = SurfaceControl::Create(name);
    if (surface) {
        surface->IncStrongRef(nullptr);
    }
    return reinterpret_cast<OH_SurfaceControl*>(surface.GetRefPtr());
}

void SurfaceControlUtils::Acquire(OH_SurfaceControl* surfaceControl)
{
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(surface);
    surface->IncStrongRef(nullptr);
}

void SurfaceControlUtils::Release(OH_SurfaceControl* surfaceControl)
{
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(surface);
    surface->DecStrongRef(nullptr);
}

OH_SurfaceTransaction* SurfaceControlUtils::Transaction::Create()
{
    auto* txn = new SurfaceTransaction();
    return reinterpret_cast<OH_SurfaceTransaction*>(txn);
}

int64_t SurfaceControlUtils::TransactionStats::GetLatchTime(OH_SurfaceTransactionStats* surfaceTransactionStats)
{
    return 0;
}

int SurfaceControlUtils::TransactionStats::GetPresentFenceFd(OH_SurfaceTransactionStats* surfaceTransactionStats)
{
    return -1;
}

void SurfaceControlUtils::TransactionStats::GetSurfaceControls(OH_SurfaceTransactionStats* surfaceTransactionStats,
    OH_SurfaceControl*** outSurfaceControls, size_t* outSurfaceControlsSize)
{}

void SurfaceControlUtils::TransactionStats::ReleaseSurfaceControls(OH_SurfaceControl** surfaceControls) {}

void SurfaceControlUtils::Transaction::Delete(OH_SurfaceTransaction* transaction)
{
    delete reinterpret_cast<SurfaceTransaction*>(transaction);
}

void SurfaceControlUtils::Transaction::Commit(OH_SurfaceTransaction* transaction)
{
    auto txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    CHECK_NULL_POINTER(txn);
    txn->Commit();
}

void SurfaceControlUtils::Transaction::SetOnComplete(
    OH_SurfaceTransaction* transaction, void* context, OH_SurfaceTransaction_OnComplete func)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    CHECK_NULL_POINTER(txn);
    if (func) {
        txn->SetOnComplete([context, func](uint64_t timestamp) { func(context, timestamp); });
    } else {
        txn->SetOnComplete({});
    }
}

void SurfaceControlUtils::Transaction::SetOnCommit(
    OH_SurfaceTransaction* transaction, void* context, OH_SurfaceTransaction_OnCommit func)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    CHECK_NULL_POINTER(txn);
    if (func) {
        txn->SetOnCommit([context, func](SurfaceTransactionStats* state) {
            func(context, reinterpret_cast<OH_SurfaceTransactionStats*>(state));
        });
    } else {
        txn->SetOnCommit({});
    }
}

void SurfaceControlUtils::Transaction::Reparent(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, OH_SurfaceControl* newParent)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    auto* parent = reinterpret_cast<SurfaceControl*>(newParent);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    CHECK_NULL_POINTER(parent);
    txn->Reparent(surface, parent);
}

void SurfaceControlUtils::Transaction::SetVisibility(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, Visibility visibility)
{
    if (visibility != Visibility::HIDE && visibility != Visibility::SHOW) {
        return;
    }
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetVisibility(surface, visibility == Visibility::SHOW);
}

void SurfaceControlUtils::Transaction::SetZOrder(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int32_t zOrder)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetZOrder(surface, zOrder);
}

void SurfaceControlUtils::Transaction::SetBuffer(OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl,
    OH_NativeBuffer* buffer, int acquireFenceFd, void* context, OH_SurfaceTransaction_OnBufferRelease func)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    auto* surfaceBuffer = OHOS::SurfaceBuffer::NativeBufferToSurfaceBuffer(buffer);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    CHECK_NULL_POINTER(surfaceBuffer);
    txn->SetBuffer(surface, surfaceBuffer, acquireFenceFd,
        [context, func](int releaseFenceFd) { func(context, releaseFenceFd); });
}

void SurfaceControlUtils::Transaction::SetCrop(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, const OH_Rect* crop)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetCrop(surface, crop);
}

void SurfaceControlUtils::Transaction::SetBounds(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBounds(surface, x, y, w, h);
}

void SurfaceControlUtils::Transaction::SetFrame(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetFrame(surface, x, y, w, h);
}

void SurfaceControlUtils::Transaction::SetPosition(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetPosition(surface, x, y);
}

void SurfaceControlUtils::Transaction::SetPivot(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetPivot(surface, x, y);
}

void SurfaceControlUtils::Transaction::SetBufferTransform(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int32_t transform)
{
#define STATIC_ASSERT_TRANSFORM(name) \
    static_assert(static_cast<int>(OHOS::GraphicTransformType::GRAPHIC_##name) == OH_TRANSFORM_##name)
    STATIC_ASSERT_TRANSFORM(ROTATE_NONE);
    STATIC_ASSERT_TRANSFORM(ROTATE_90);
    STATIC_ASSERT_TRANSFORM(ROTATE_180);
    STATIC_ASSERT_TRANSFORM(ROTATE_270);
    STATIC_ASSERT_TRANSFORM(FLIP_H);
    STATIC_ASSERT_TRANSFORM(FLIP_V);
    STATIC_ASSERT_TRANSFORM(FLIP_H_ROT90);
    STATIC_ASSERT_TRANSFORM(FLIP_V_ROT90);
    STATIC_ASSERT_TRANSFORM(FLIP_H_ROT180);
    STATIC_ASSERT_TRANSFORM(FLIP_V_ROT180);
    STATIC_ASSERT_TRANSFORM(FLIP_H_ROT270);
    STATIC_ASSERT_TRANSFORM(FLIP_V_ROT270);
#undef STATIC_ASSERT_TRANSFORM
    if (transform < OH_TRANSFORM_ROTATE_NONE || transform > OH_TRANSFORM_ROTATE_LAST) {
        WVLOG_E("Invalid transform value: %{public}d", transform);
        return;
    }
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBufferTransform(surface, static_cast<OHOS::GraphicTransformType>(transform));
}

void SurfaceControlUtils::Transaction::SetScale(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float scaleX, float scaleY, float scaleZ)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetScale(surface, scaleX, scaleY, scaleZ);
}

void SurfaceControlUtils::Transaction::SetRotation(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float degreeX, float degreeY, float degreeZ)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetRotation(surface, degreeX, degreeY, degreeZ);
}

void SurfaceControlUtils::Transaction::SetTranslate(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float translateX, float translateY, float translateZ)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetTranslate(surface, translateX, translateY, translateZ);
}

void SurfaceControlUtils::Transaction::SetSkew(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float skewX, float skewY, float skewZ)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetSkew(surface, skewX, skewY, skewZ);
}

void SurfaceControlUtils::Transaction::SetPerspective(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float perspX, float perspY, float perspZ, float perspW)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetPerspective(surface, perspX, perspY, perspZ, perspW);
}

void SurfaceControlUtils::Transaction::SetBufferTransparency(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int8_t transparency)
{
    if (transparency != Transparency::TRANSPARENT && transparency != Transparency::TRANSLUCENT &&
        transparency != Transparency::OPAQUE) {
        WVLOG_E("Invalid transparency value: %{public}d", transparency);
        return;
    }
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBufferTransparency(surface, transparency);
}

void SurfaceControlUtils::Transaction::SetDamageRegion(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, const OH_Rect* rects, uint32_t count)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetDamageRegion(surface, rects, count);
}

void SurfaceControlUtils::Transaction::SetDesiredPresentTime(
    OH_SurfaceTransaction* transaction, int64_t desiredPresentTime)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    CHECK_NULL_POINTER(txn);
    txn->SetDesiredPresentTime(desiredPresentTime);
}

void SurfaceControlUtils::Transaction::SetBufferAlpha(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float alpha)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBufferAlpha(surface, alpha);
}

void SurfaceControlUtils::Transaction::SetFrameRateWithChangeStrategy(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float frameRate, int8_t compatibility, int8_t changeFrameRateStrategy)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetFrameRateWithChangeStrategy(surface, frameRate, compatibility, changeFrameRateStrategy);
}

void SurfaceControlUtils::Transaction::ClearFrameRate(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->ClearFrameRate(surface);
}

void SurfaceControlUtils::Transaction::SetEnableBackPressure(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, bool enableBackPressure)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetEnableBackPressure(surface, enableBackPressure);
}

void SurfaceControlUtils::Transaction::SetForegroundColor(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float red, float green, float blue, float alpha)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetForegroundColor(surface, red, green, blue, alpha);
}

void SurfaceControlUtils::Transaction::SetBackgroundColor(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float red, float green, float blue, float alpha)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBackgroundColor(surface, red, green, blue, alpha);
}

void SurfaceControlUtils::Transaction::SetBorderColor(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float red, float green, float blue, float alpha)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBorderColor(surface, red, green, blue, alpha);
}

void SurfaceControlUtils::Transaction::SetBorderWidth(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, float left, float top, float right, float bottom)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBorderWidth(surface, left, top, right, bottom);
}

void SurfaceControlUtils::Transaction::SetBorderStyle(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetBorderStyle(surface, left, top, right, bottom);
}

void SurfaceControlUtils::Transaction::SetHardwareEnableHint(OH_SurfaceTransaction* transaction,
    OH_SurfaceControl* surfaceControl, bool enable)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetHardwareEnableHint(surface, enable);
}

void SurfaceControlUtils::Transaction::SetName(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, const char* name)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetName(surface, name);
}

void SurfaceControlUtils::Transaction::SetFrameGravity(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, int32_t gravity)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetFrameGravity(surface, gravity);
}

void SurfaceControlUtils::Transaction::SetSrcRect(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetSrcRect(surface, x, y, w, h);
}

void SurfaceControlUtils::Transaction::SetDisplayRect(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->SetDisplayRect(surface, x, y, w, h);
}

void SurfaceControlUtils::Transaction::ClearBufferQueueCache(
    OH_SurfaceTransaction* transaction, OH_SurfaceControl* surfaceControl, bool cleanAll)
{
    auto* txn = reinterpret_cast<SurfaceTransaction*>(transaction);
    auto* surface = reinterpret_cast<SurfaceControl*>(surfaceControl);
    CHECK_NULL_POINTER(txn);
    CHECK_NULL_POINTER(surface);
    txn->ClearBufferQueueCache(surface, cleanAll);
}
} // namespace NWeb
} // namespace OHOS