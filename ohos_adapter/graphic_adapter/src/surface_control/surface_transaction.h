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
#include "ipc_callbacks/rs_delegate_composite_callback.h"
#include "feature/delegate_composite/rs_delegate_composite_listener.h"
#include "common/rs_common_def.h"
#include "ui/rs_ui_context_manager.h"

namespace OHOS {
namespace NWeb {
struct OH_Rect;

class RSC_EXPORT SurfaceTransactionStats {
public:
    SurfaceTransactionStats() = default;
    ~SurfaceTransactionStats() = default;

private:
    SurfaceTransactionStats(const SurfaceTransactionStats&) = delete;
    SurfaceTransactionStats& operator=(const SurfaceTransactionStats&) = delete;
    SurfaceTransactionStats(SurfaceTransactionStats&&) = delete;
    SurfaceTransactionStats& operator=(SurfaceTransactionStats&&) = delete;
};

class ScopedTransaction {
public:
    ScopedTransaction()
    {
        std::shared_ptr<OHOS::Rosen::RSUIContext> uiContext = SurfaceControl::GetRSUIContext();
        if (uiContext) {
            transaction_ = uiContext->GetRSTransaction();
        }
        if (transaction_) {
            transaction_->Begin();
        } else {
            RS_TRACE_NAME("ScopedTransaction init fail, transaction is nullptr");
        }
    }

    ~ScopedTransaction()
    {
        if (transaction_) {
            transaction_->Commit();
            RS_TRACE_NAME("~ScopedTransaction commit success");
        } else {
            RS_TRACE_NAME("~ScopedTransaction commit fail, transaction is nullptr");
        }
    }

    void AddCommand(std::unique_ptr<OHOS::Rosen::RSCommand> cmd)
    {
        if (transaction_) {
            transaction_->AddCommand(cmd, true);
        } else {
            RS_TRACE_NAME("ScopedTransaction AddCommand, transaction is nullptr");
        }
    }

    ScopedTransaction(const ScopedTransaction&) = delete;
    ScopedTransaction& operator=(const ScopedTransaction&) = delete;
private:
    std::shared_ptr<OHOS::Rosen::RSTransactionHandler> transaction_ = nullptr;
};

class RSC_EXPORT SurfaceTransaction {
public:
    using OnCompleteCallback = std::function<void(uint64_t timestamp)>;
    using BufferReleaseCallback = SurfaceControl::BufferReleaseCallback;

    SurfaceTransaction(OHNativeWindow* nativeWindow);
    ~SurfaceTransaction();

    void Commit();
    void SetOnComplete(const OnCompleteCallback& callback);
    void Reparent(SurfaceControl* surfaceControl, SurfaceControl* newParent);
    void SetVisibility(SurfaceControl* surfaceControl, bool visibility);
    void SetZOrder(SurfaceControl* surfaceControl, int32_t zOrder);
    void SetBuffer(SurfaceControl* surfaceControl, SurfaceBuffer* buffer, int acquireFenceFd,
        const BufferReleaseCallback& callback);
    void SetBounds(SurfaceControl* surfaceControl, float x, float y, float w, float h);
    void SetFrameGravity(SurfaceControl* surfaceControl, int32_t gravity);
    void SetPivot(SurfaceControl* surfaceControl, float x, float y);
    void SetBufferTransform(SurfaceControl* surfaceControl, GraphicTransformType transform);
    void SetTranslate(SurfaceControl* surfaceControl, float translateX, float translateY, float translateZ);
    void SetDamageRegion(SurfaceControl* surfaceControl, const OH_Rect* rects, uint32_t count);
    void SetBufferAlpha(SurfaceControl* surfaceControl, float alpha);
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
    std::vector<std::function<void()>> transactionCommands_;
    std::vector<std::function<void()>> bufferCommands_;
    std::set<sptr<SurfaceControl>> surfaceControls_;
    sptr<Rosen::SurfaceTransactionListener> listener_ = nullptr;
};
} // namespace NWeb
} // namespace OHOS
#endif // SURFACE_CONTROL_SURFACE_TRANSACTION_H_