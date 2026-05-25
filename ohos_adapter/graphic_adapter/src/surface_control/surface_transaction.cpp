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

#include "surface_transaction.h"
#include <optional>
#include <unique_fd.h>
#include "transaction/rs_transaction.h"
#include "ui/rs_ui_context_manager.h"
#include "surface_buffer.h"
#include "interface/oh_surface_control.h"
#include "rs_trace.h"
#include "nweb_log.h"
#include "surface_transaction_helper.h"
#include "common/rs_optional_trace.h"

using namespace OHOS::Rosen;
using OHOS::Rosen::RSUIContextManager;

namespace OHOS {
namespace NWeb {
namespace {
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
            RS_TRACE_NAME("ScopedTransaction init fail, transaction is invalid");
        }
    }

    ~ScopedTransaction()
    {
        if (transaction_) {
            transaction_->Commit();
            RS_TRACE_NAME("ScopedTransaction commit ok");
        } else {
            RS_TRACE_NAME("ScopedTransaction commit fail, transaction is invalid");
        }
    }

    void AddCommand(std::unique_ptr<OHOS::Rosen::RSCommand> cmd)
    {
        if (transaction_) {
            transaction_->AddCommand(cmd, true);
        } else {
            RS_TRACE_NAME("ScopedTransaction AddCommand, transaction is invalid");
        }
    }

private:
    std::shared_ptr<Rosen::RSTransactionHandler> transaction_ = nullptr;
};
} // namespace

void SurfaceTransaction::OnCompleteCallBack(uint64_t timestamp, uint64_t srcId, std::queue<uint64_t> seqNums)
{
    RS_TRACE_NAME_FMT("SurfaceTransaction::OnCompleteCallBack, timestamp=%llu, srcId=%llu, seqNums size=%u",
        timestamp, srcId, seqNums.size());

    while (!seqNums.empty()) {
        uint64_t seqNum = seqNums.front();
        seqNums.pop();
        auto cb = SurfaceTransactionCallBackHelper::GetInstance().GetCallBack(srcId, seqNum);
        if (!cb) {
            RS_OPTIONAL_TRACE_NAME_FMT("error: not find callback:srcId=%llu, seqNum=%llu", srcId, seqNum);
            WVLOG_E("OnCompleteCallback fail, not find callback, srcId=%" PRIu64
                ", seqNum=%" PRIu64, srcId, seqNum);
            continue;
        }
        cb(timestamp);
        RS_OPTIONAL_TRACE_NAME_FMT("find callback srcId=%llu, seqNum=%llu", srcId, seqNum);
    }

    SurfaceTransactionCallBackHelper::GetInstance().DfxInfo();
}

SurfaceTransaction::SurfaceTransaction()
{
};

SurfaceTransaction::~SurfaceTransaction()
{
    RS_TRACE_NAME_FMT("SurfaceTransaction::~SurfaceTransaction(), seqNum_=%llu", seqNum_);
}

void SurfaceTransaction::Commit()
{
    auto uiContext = SurfaceControl::GetRSUIContext();
    if (!uiContext) {
        WVLOG_E("SurfaceTransaction::Commit fail, uiContext is invalid");
        return;
    }
    auto transaction = uiContext->GetRSTransaction();

    RS_TRACE_NAME_FMT("webview:commit: transaction_commandSize:%u buffer_commandSize:%u surface_controlSize:%u, "
        "transaction=%d", transactionCommands_.size(), bufferCommands_.size(), surfaceControls_.size(),
        transaction != nullptr);
    
    ScopedTransaction scopedTransaction;

    for (auto& command : transactionCommands_) {
        command();
    }
    transactionCommands_.clear();

    for (auto& command : bufferCommands_) {
        command();
    }
    bufferCommands_.clear();
    surfaceControls_.clear();
}

void SurfaceTransaction::SetOnComplete(const OnCompleteCallback& callback)
{
    onCompleteCallback_ = callback;
}

void SurfaceTransaction::SetOnCommit(const OnCommitCallback& callback)
{
    onCommitCallback_ = callback;
}

void SurfaceTransaction::Reparent(SurfaceControl* surfaceControl, SurfaceControl* newParent)
{
    if (!surfaceControl) {
        return;
    }

    if (surfaceControl->IsRootSurface()) {
        return;
    }

    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), parent = sptr<SurfaceControl>(newParent)] {
            surface->SetParent(parent.GetRefPtr());
        });
}

void SurfaceTransaction::SetVisibility(SurfaceControl* surfaceControl, bool visibility)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), visibility] { surface->SetVisibility(visibility); });
}

void SurfaceTransaction::SetZOrder(SurfaceControl* surfaceControl, int32_t zOrder)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), zOrder] { surface->SetZOrder(zOrder); });
}

void SurfaceTransaction::SetBuffer(
    SurfaceControl* surfaceControl, SurfaceBuffer* buffer, int acquireFenceFd, const BufferReleaseCallback& callback)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    bufferCommands_.push_back(
        [surface = surfaceControl, surfaceBuffer = sptr<SurfaceBuffer>(buffer), acquireFenceFd, callback] {
            surface->SetBuffer(std::move(surfaceBuffer), UniqueFd(acquireFenceFd), callback);
        });
    surfaceControls_.insert(sptr<SurfaceControl>(surfaceControl));
}

void SurfaceTransaction::SetCrop(SurfaceControl* surfaceControl, const OH_Rect* crop)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }

    static_assert(sizeof(Rect) == sizeof(OH_Rect));
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl),
        crop = *reinterpret_cast<const Rect*>(crop)] { surface->SetCrop(&crop); });
}

void SurfaceTransaction::SetBounds(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetBounds, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] { surface->SetBounds(x, y, w, h); });
}

void SurfaceTransaction::SetFrame(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetFrame, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] { surface->SetFrame(x, y, w, h); });
}

void SurfaceTransaction::SetFrameGravity(SurfaceControl* surfaceControl, int32_t gravity)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), gravity] { surface->SetFrameGravity(gravity); });
}

void SurfaceTransaction::SetPosition(SurfaceControl* surfaceControl, float x, float y)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), x, y] { surface->SetPosition(x, y); });
}

void SurfaceTransaction::SetPivot(SurfaceControl* surfaceControl, float x, float y)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), x, y] { surface->SetPivot(x, y); });
}

void SurfaceTransaction::SetBufferTransform(SurfaceControl* surfaceControl, GraphicTransformType transform)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = surfaceControl, transform] { surface->SetBufferTransform(transform); });
    // Buffer transform change may need to set buffer again.
    surfaceControls_.insert(sptr<SurfaceControl>(surfaceControl));
}

void SurfaceTransaction::SetScale(SurfaceControl* surfaceControl, float xScale, float yScale, float zScale)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), xScale, yScale, zScale] {
        surface->SetScale(xScale, yScale, zScale);
    });
}

void SurfaceTransaction::SetRotation(SurfaceControl* surfaceControl, float degreeX, float degreeY, float degreeZ)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), degreeX, degreeY, degreeZ] {
        surface->SetRotation(degreeX, degreeY, degreeZ);
    });
}

void SurfaceTransaction::SetTranslate(
    SurfaceControl* surfaceControl, float translateX, float translateY, float translateZ)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), translateX, translateY,
                                        translateZ] { surface->SetTranslate(translateX, translateY, translateZ); });
}

void SurfaceTransaction::SetSkew(SurfaceControl* surfaceControl, float skewX, float skewY, float skewZ)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), skewX, skewY, skewZ] {
        surface->SetSkew(skewX, skewY, skewZ);
    });
}

void SurfaceTransaction::SetPerspective(
    SurfaceControl* surfaceControl, float perspX, float perspY, float perspZ, float perspW)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), perspX, perspY, perspZ,
                                        perspW] { surface->SetPerspective(perspX, perspY, perspZ, perspW); });
}

void SurfaceTransaction::SetBufferTransparency(SurfaceControl* surfaceControl, int32_t transparency)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }

    std::string name(reinterpret_cast<const char*>(transparency));
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), transparency] {
        surface->SetBufferTransparency(transparency);
    });
}

void SurfaceTransaction::SetDamageRegion(SurfaceControl* surfaceControl, const OH_Rect* rects, uint32_t count)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    std::shared_ptr<std::vector<Rect>> damageRects;
    if (count != 0) {
        damageRects = std::make_shared<std::vector<Rect>>(count);
        static_assert(sizeof(Rect) == sizeof(OH_Rect));
        // Make sure OH_Rect and OHOS::Rect are sychronized.
        if (memcpy_s(damageRects->data(), sizeof(Rect) * count, rects, sizeof(Rect) * count) != EOK) {
            WVLOG_E("memcpy_s fail");
            return;
        }
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), damageRects, count] {
        (void)count;
        surface->SetDamageRegion(damageRects ? std::move(*damageRects) : std::vector<Rect>());
    });
}

void SurfaceTransaction::SetDesiredPresentTime(int64_t desiredPresentTime)
{
    desiredPresentTime_ = desiredPresentTime;
}

void SurfaceTransaction::SetBufferAlpha(SurfaceControl* surfaceControl, float alpha)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), alpha] { surface->SetBufferAlpha(alpha); });
}

void SurfaceTransaction::SetFrameRateWithChangeStrategy(
    SurfaceControl* surfaceControl, float frameRate, int8_t compatibility, int32_t strategy)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), frameRate, compatibility, strategy] {
            surface->SetFrameRateWithChangeStrategy(frameRate, compatibility, strategy);
        });
}

void SurfaceTransaction::ClearFrameRate(SurfaceControl* surfaceControl)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl)] { surface->ClearFrameRate(); });
}

void SurfaceTransaction::SetEnableBackPressure(SurfaceControl* surfaceControl, bool enableBackPressure)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), enableBackPressure] {
        surface->SetEnableBackPressure(enableBackPressure);
    });
}

void SurfaceTransaction::SetForegroundColor(
    SurfaceControl* surfaceControl, float red, float green, float blue, float alpha)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), red, green, blue, alpha] {
        surface->SetForegroundColor(red, green, blue, alpha);
    });
}

void SurfaceTransaction::SetBackgroundColor(
    SurfaceControl* surfaceControl, float red, float green, float blue, float alpha)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), red, green, blue, alpha] {
        surface->SetBackgroundColor(red, green, blue, alpha);
    });
}

void SurfaceTransaction::SetBorderWidth(
    SurfaceControl* surfaceControl, float left, float top, float right, float bottom)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), left, top, right, bottom] {
        surface->SetBorderWidth(left, top, right, bottom);
    });
}

void SurfaceTransaction::SetBorderColor(
    SurfaceControl* surfaceControl, float red, float green, float blue, float alpha)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), red, green, blue, alpha] {
        surface->SetBorderColor(red, green, blue, alpha);
    });
}

void SurfaceTransaction::SetBorderStyle(
    SurfaceControl* surfaceControl, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }

    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), left, top, right, bottom] {
        surface->SetBorderStyle(left, top, right, bottom);
    });
}

void SurfaceTransaction::SetName(SurfaceControl* surfaceControl, const char* name)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), nameStr = std::string(name)] {
        surface->SetName(std::move(nameStr));
    });
}

void SurfaceTransaction::SetHardwareEnableHint(SurfaceControl* surfaceControl, bool enable)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), enable] { surface->SetHardwareEnableHint(enable); });
}

void SurfaceTransaction::SetSrcRect(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetSrcRect, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] { surface->SetSrcRect(x, y, w, h); });
}

void SurfaceTransaction::SetDisplayRect(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetDisplayRect, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] { surface->SetDisplayRect(x, y, w, h); });
}

void SurfaceTransaction::ClearBufferQueueCache(SurfaceControl* surfaceControl, bool cleanAll)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::ClearBufferQueueCache");
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), cleanAll] { surface->ClearBufferQueueCache(cleanAll); });
}
} // namespace NWeb
} // namespace OHOS