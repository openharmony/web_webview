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

#ifndef SURFACE_CONTROL_SURFACE_TRANSACTION_H_
#define SURFACE_CONTROL_SURFACE_TRANSACTION_H_

#include <functional>
#include <mutex>
#include <set>
#include <vector>

#include "surface_control.h"
#include "common/rs_common_def.h"

namespace OHOS {
namespace NWeb {
struct OH_Rect;

class RSC_EXPORT SurfaceTransactionStats {
public:
    SurfaceTransactionStats() = default;
    ~SurfaceTransactionStats() = default;
    int64_t GetLatchTime() const;
    int GetPresentFenceFd() const;
    void GetSurfaceControls(SurfaceControl*** out_surface_controls, size_t* out_surface_controls_size) const;
    void ReleaseSurfaceControls(SurfaceControl** surface_controls) const;

private:
    SurfaceTransactionStats(const SurfaceTransactionStats&) = delete;
    SurfaceTransactionStats& operator=(const SurfaceTransactionStats&) = delete;
    SurfaceTransactionStats(SurfaceTransactionStats&&) = delete;
    SurfaceTransactionStats& operator=(SurfaceTransactionStats&&) = delete;
};

class RSC_EXPORT SurfaceTransaction {
public:
    using OnCompleteCallback = std::function<void(uint64_t timestamp)>;
    using OnCommitCallback = std::function<void(SurfaceTransactionStats* state)>;
    using BufferReleaseCallback = SurfaceControl::BufferReleaseCallback;

    SurfaceTransaction();
    ~SurfaceTransaction();

    void Commit();
    void SetOnComplete(const OnCompleteCallback& callback);
    void SetOnCommit(const OnCommitCallback& callback);
    void SetDesiredPresentTime(int64_t desiredPresentTime);
    void Reparent(SurfaceControl* surfaceControl, SurfaceControl* newParent);
    void SetVisibility(SurfaceControl* surfaceControl, bool visibility);
    void SetZOrder(SurfaceControl* surfaceControl, int32_t zOrder);
    void SetBuffer(SurfaceControl* surfaceControl, SurfaceBuffer* buffer, int acquireFenceFd,
        const BufferReleaseCallback& callback);
    void SetCrop(SurfaceControl* surfaceControl, const OH_Rect* crop);
    void SetBounds(SurfaceControl* surfaceControl, float x, float y, float w, float h);
    void SetFrame(SurfaceControl* surfaceControl, float x, float y, float w, float h);
    void SetFrameGravity(SurfaceControl* surfaceControl, int32_t gravity);
    void SetPosition(SurfaceControl* surfaceControl, float x, float y);
    void SetPivot(SurfaceControl* surfaceControl, float x, float y);
    void SetBufferTransform(SurfaceControl* surfaceControl, GraphicTransformType transform);
    void SetScale(SurfaceControl* surfaceControl, float xScale, float yScale, float zScale);
    void SetRotation(SurfaceControl* surfaceControl, float degreeX, float degreeY, float degreeZ);
    void SetTranslate(SurfaceControl* surfaceControl, float translateX, float translateY, float translateZ);
    void SetSkew(SurfaceControl* surfaceControl, float skewX, float skewY, float skewZ);
    void SetPerspective(SurfaceControl* surfaceControl, float perspX, float perspY, float perspZ, float perspW);

    void SetBufferTransparency(SurfaceControl* surfaceControl, int32_t transparency);
    void SetDamageRegion(SurfaceControl* surfaceControl, const OH_Rect* rects, uint32_t count);
    void SetBufferAlpha(SurfaceControl* surfaceControl, float alpha);
    void SetFrameRateWithChangeStrategy(
        SurfaceControl* surfaceControl, float frameRate, int8_t compatibility, int32_t strategy);
    void ClearFrameRate(SurfaceControl* surfaceControl);
    void SetEnableBackPressure(SurfaceControl* surfaceControl, bool enableBackPressure);
    void SetForegroundColor(SurfaceControl* surfaceControl, float red, float green, float blue, float alpha);
    void SetBackgroundColor(SurfaceControl* surfaceControl, float red, float green, float blue, float alpha);
    void SetBorderWidth(SurfaceControl* surfaceControl, float left, float top, float right, float bottom);
    void SetBorderColor(SurfaceControl* surfaceControl, float red, float green, float blue, float alpha);
    void SetBorderStyle(SurfaceControl* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
    void SetName(SurfaceControl* surfaceControl, const char* name);
    void SetHardwareEnableHint(SurfaceControl* surfaceControl, bool enable);
    void SetSrcRect(SurfaceControl* surfaceControl, float x, float y, float w, float h);
    void SetDisplayRect(SurfaceControl* surfaceControl, float x, float y, float w, float h);
    void ClearBufferQueueCache(SurfaceControl* surfaceControl, bool cleanAll);

private:
    SurfaceTransaction(const SurfaceTransaction&) = delete;
    SurfaceTransaction& operator=(const SurfaceTransaction&) = delete;
    SurfaceTransaction(SurfaceTransaction&&) = delete;
    SurfaceTransaction& operator=(SurfaceTransaction&&) = delete;
    static void OnCompleteCallBack(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t> seqNums);
    OnCompleteCallback onCompleteCallback_;
    OnCommitCallback onCommitCallback_;
    int64_t desiredPresentTime_ = 0;
    std::vector<std::function<void()>> transactionCommands_;
    std::vector<std::function<void()>> bufferCommands_;
    std::set<sptr<SurfaceControl>> surfaceControls_;
    uint64_t seqNum_ = 0;
    std::atomic<uint64_t> commandSeqNum_ = 0;
    std::map<uint64_t, SurfaceTransaction::OnCompleteCallback> onCompleteCallbackMap_;
};
} // namespace NWeb
} // namespace OHOS
#endif // SURFACE_CONTROL_SURFACE_TRANSACTION_H_