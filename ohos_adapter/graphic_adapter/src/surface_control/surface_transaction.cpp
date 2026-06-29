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
#include "surface/window.h"
#include "native_window.h"

using namespace OHOS::Rosen;
using OHOS::Rosen::RSUIContextManager;

namespace OHOS {
namespace NWeb {
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
            WVLOG_E("DelegateModeDebugTag: OnCompleteCallback fail, not find callback, srcId=%" PRIu64
                ", seqNum=%" PRIu64, srcId, seqNum);
            continue;
        }
        cb(timestamp);
        RS_OPTIONAL_TRACE_NAME_FMT("find callback srcId=%llu, seqNum=%llu", srcId, seqNum);
    }

    SurfaceTransactionCallBackHelper::GetInstance().DfxInfo();
}

SurfaceTransaction::SurfaceTransaction(OHNativeWindow* nativeWindow)
{
    if (nativeWindow == nullptr) {
        WVLOG_E("DelegateModeDebugTag: window is nullptr");
        return;
    }

    OHOS::sptr<OHOS::Surface> surface = nativeWindow->surface;
    if (surface == nullptr) {
        WVLOG_E("DelegateModeDebugTag: surface is nullptr");
        return;
    }

    std::string fetchedRSHandle = surface->GetUserData("delegate_connect_to_render");
    sptr<IRemoteObject> rsHandle = nullptr;
    if (!fetchedRSHandle.empty()) {
        uint64_t handle = static_cast<uint64_t>(std::stoull(fetchedRSHandle));
        if (handle != 0) {
            IRemoteObject* rawPtr = reinterpret_cast<IRemoteObject*>(handle);
            rsHandle = OHOS::sptr<IRemoteObject>(rawPtr);
            rawPtr->DecStrongRef(nullptr);
        } else {
            WVLOG_E("DelegateModeDebugTag: handle is invalid");
            return;
        }
    } else {
        WVLOG_E("DelegateModeDebugTag: get fetchedRSHandle fail");
        return;
    }
    if (rsHandle == nullptr) {
        WVLOG_E("DelegateModeDebugTag: rsHandle is nullptr");
        return;
    }
    SurfaceControl::SetConnectToRenderObject(rsHandle);
    WVLOG_I("SurfaceTransaction set rsHandle success");

    listener_ = sptr<SurfaceTransactionListener>::MakeSptr(rsHandle);
    if (listener_) {
        listener_->RegisterCommandCompleteCallBack(SurfaceTransaction::OnCompleteCallBack);
    } else {
        WVLOG_E("DelegateModeDebugTag: create SurfaceTransactionListener fail");
    }
}

SurfaceTransaction::~SurfaceTransaction()
{
    RS_TRACE_NAME_FMT("SurfaceTransaction::~SurfaceTransaction()");
    if (listener_) {
        SurfaceTransactionCallBackHelper::GetInstance().UnRegisterCallBack(listener_->GetUniqueId());
        listener_->UnRegisterCommandCompleteCallBack();
        listener_ = nullptr;
    }
}

void SurfaceTransaction::Commit()
{
    auto uiContext = SurfaceControl::GetRSUIContext();
    if (!uiContext) {
        WVLOG_E("DelegateModeDebugTag: SurfaceTransaction::Commit fail, uiContext is invalid");
        return;
    }
    auto transaction = uiContext->GetRSTransaction();

    RS_TRACE_NAME_FMT("transactionCommands_ size:%u bufferCommands_ size:%u surfaceControls_ size:%u, transaction=%d",
        transactionCommands_.size(), bufferCommands_.size(), surfaceControls_.size(), transaction != nullptr);

    ScopedTransaction scopedTransaction;
    if (listener_) {
        uint64_t seqNum = 0;
        std::unique_ptr<OHOS::Rosen::RSCommand> cmd = listener_->GetCommand(seqNum);
        if (cmd) {
            scopedTransaction.AddCommand(std::move(cmd));
            SurfaceTransactionCallBackHelper::GetInstance().RegisterCallBack(
                listener_->GetUniqueId(), seqNum, onCompleteCallback_);
        } else {
            WVLOG_E("DelegateModeDebugTag: GetCommand fail");
        }
    } else {
        WVLOG_E("DelegateModeDebugTag: SurfaceTransaction:: Commit err, listener is nullptr");
    }

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
            if (surface) {
                surface->SetParent(parent.GetRefPtr());
            }
        });
}

void SurfaceTransaction::SetVisibility(SurfaceControl* surfaceControl, bool visibility)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), visibility] {
        if (surface) {
            surface->SetVisibility(visibility);
        }
    });
}

void SurfaceTransaction::SetZOrder(SurfaceControl* surfaceControl, int32_t zOrder)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), zOrder] {
        if (surface) {
            surface->SetZOrder(zOrder);
        }
    });
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
            if (surface) {
                surface->SetBuffer(std::move(surfaceBuffer), UniqueFd(acquireFenceFd), callback);
            }
        });
    surfaceControls_.insert(sptr<SurfaceControl>(surfaceControl));
}

void SurfaceTransaction::SetBounds(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetBounds, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] {
        if (surface) {
            surface->SetBounds(x, y, w, h);
        }
    });
}

void SurfaceTransaction::SetFrameGravity(SurfaceControl* surfaceControl, int32_t gravity)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), gravity] {
        if (surface) {
            surface->SetFrameGravity(gravity);
        }
    });
}

void SurfaceTransaction::SetPivot(SurfaceControl* surfaceControl, float x, float y)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), x, y] {
        if (surface) {
            surface->SetPivot(x, y);
        }
    });
}

void SurfaceTransaction::SetBufferTransform(SurfaceControl* surfaceControl, GraphicTransformType transform)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = surfaceControl, transform] {
        if (surface) {
            surface->SetBufferTransform(transform);
        }
    });
    // Buffer transform change may need to set buffer again.
    surfaceControls_.insert(sptr<SurfaceControl>(surfaceControl));
}

void SurfaceTransaction::SetTranslate(
    SurfaceControl* surfaceControl, float translateX, float translateY, float translateZ)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back(
        [surface = sptr<SurfaceControl>(surfaceControl), translateX, translateY, translateZ] {
            if (surface) {
                surface->SetTranslate(translateX, translateY, translateZ);
            }
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
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), damageRects] {
        if (surface) {
            surface->SetDamageRegion(damageRects ? std::move(*damageRects) : std::vector<Rect>());
        }
    });
}

void SurfaceTransaction::SetBufferAlpha(SurfaceControl* surfaceControl, float alpha)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), alpha] {
        if (surface) {
            surface->SetBufferAlpha(alpha);
        }
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
        if (surface) {
            surface->SetForegroundColor(red, green, blue, alpha);
        }
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
        if (surface) {
            surface->SetBackgroundColor(red, green, blue, alpha);
        }
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
        if (surface) {
            surface->SetBorderWidth(left, top, right, bottom);
        }
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
        if (surface) {
            surface->SetBorderColor(red, green, blue, alpha);
        }
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
        if (surface) {
            surface->SetBorderStyle(left, top, right, bottom);
        }
    });
}

void SurfaceTransaction::SetName(SurfaceControl* surfaceControl, const char* name)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), nameStr = std::string(name)] {
        if (surface) {
            surface->SetName(std::move(nameStr));
        }
    });
}

void SurfaceTransaction::SetHardwareEnableHint(SurfaceControl* surfaceControl, bool enable)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), enable] {
        if (surface) {
            surface->SetHardwareEnableHint(enable);
        }
    });
}

void SurfaceTransaction::SetSrcRect(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetSrcRect, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] {
        if (surface) {
            surface->SetSrcRect(x, y, w, h);
        }
    });
}

void SurfaceTransaction::SetDisplayRect(SurfaceControl* surfaceControl, float x, float y, float w, float h)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::SetDisplayRect, [x:%.2f, y:%.2f, w:%.2f, h:%.2f]", x, y, w, h);
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), x, y, w, h] {
        if (surface) {
            surface->SetDisplayRect(x, y, w, h);
        }
    });
}

void SurfaceTransaction::ClearBufferQueueCache(SurfaceControl* surfaceControl, bool cleanAll)
{
    if (!surfaceControl) {
        WVLOG_E("surfaceControl is nullptr");
        return;
    }
    RS_TRACE_NAME_FMT("SurfaceTransaction::ClearBufferQueueCache");
    transactionCommands_.push_back([surface = sptr<SurfaceControl>(surfaceControl), cleanAll] {
        if (surface) {
            surface->ClearBufferQueueCache(cleanAll);
        }
    });
}
} // namespace NWeb
} // namespace OHOS