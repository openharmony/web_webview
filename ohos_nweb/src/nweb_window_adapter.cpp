/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_window_adapter.h"

#include <display_type.h>
#include <refbase.h>
#include <securec.h>
#include <surface.h>
#include <ui/rs_surface_node.h>

#include "graphic_common.h"
#include "graphic_common_c.h"
#include "nweb_input_event_consumer.h"
#include "nweb_log.h"
#include "surface_buffer.h"
#include "surface_type.h"
#include "transaction/rs_interfaces.h"
#include "wm_common.h"

namespace OHOS::NWeb {
// static
NWebWindowAdapter &NWebWindowAdapter::Instance()
{
    static NWebWindowAdapter windowAdapter;
    return windowAdapter;
}

NWebCreateInfo NWebWindowAdapter::GetCreateInfo(Rosen::Window *window, const NWebInitArgs &initArgs)
{
    NWebCreateInfo createInfo = {
        .init_args = initArgs,
    };
    if (window == nullptr) {
        return createInfo;
    }
    GetSize(window, createInfo);
    GetRenderInterface(window, createInfo);
    return createInfo;
}

void NWebWindowAdapter::GetSize(Rosen::Window *window, NWebCreateInfo &createInfo)
{
    createInfo.width = window->GetRect().width_;
    createInfo.height = window->GetRect().height_;
    windowInfoMap_[window].width = createInfo.width;
    windowInfoMap_[window].height = createInfo.height;
}

void NWebWindowAdapter::GetRenderInterface(Rosen::Window *window, NWebCreateInfo &createInfo)
{
    createInfo.output_render_frame = [window, this] (const char *buffer, uint32_t width, uint32_t height) -> bool {
        if (windowInfoMap_[window].cachedSurfaceBuffer != nullptr) {
            WVLOG_I("last cached frame is not consumed, skip this frame output");
            return false;
        }

        sptr<Surface> surface = window->GetSurfaceNode()->GetSurface();
        if (surface == nullptr) {
            WVLOG_E("surface get from window is nullptr");
            return false;
        }

        BufferRequestConfig config = {
            .width = window->GetRect().width_,
            .height = window->GetRect().height_,
            .strideAlignment = sizeof(void *),
            .format = PIXEL_FMT_RGBA_8888,
            .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
            .timeout = 0,
        };

        sptr<SurfaceBuffer> surfaceBuffer;
        int32_t releaseFence = -1;
        SurfaceError ret = surface->RequestBuffer(surfaceBuffer, releaseFence, config);
        if (ret != SURFACE_ERROR_OK) {
            WVLOG_E("fail to request buffer from surface, errorcode=%{public}d", ret);
            return false;
        }

        char *addr = reinterpret_cast<char *>(surfaceBuffer->GetVirAddr());
        if (addr == nullptr) {
            WVLOG_E("fail to get buffer addr");
            surface->CancelBuffer(surfaceBuffer);
            return false;
        }

        constexpr uint32_t BITS_PER_PIXEL = 4;
        uint32_t frameSize = windowInfoMap_[window].width * windowInfoMap_[window].height * BITS_PER_PIXEL;
        if (windowInfoMap_[window].width != window->GetRect().width_ ||
            windowInfoMap_[window].height != window->GetRect().height_) {
            WVLOG_W("nweb output frame size (%{public}u*%{public}u) is different from "
                    "window surface buffer size (%{public}u*%{public}u)",
                    windowInfoMap_[window].width, windowInfoMap_[window].height,
                    window->GetRect().width_, window->GetRect().height_);
            uint32_t surfaceBufferSize = window->GetRect().width_ * window->GetRect().height_ * BITS_PER_PIXEL;
            if (surfaceBufferSize < frameSize) {
                frameSize = surfaceBufferSize;
            }
        }
        if (memcpy_s(addr, frameSize, buffer, frameSize) != EOK) {
            WVLOG_E("memcpy_s fail, errorcode=%{public}d", ret);
            return false;
        }

        windowInfoMap_[window].cachedSurfaceBuffer = surfaceBuffer;

        return true;
    };
}

void NWebWindowAdapter::RequestVsync(Rosen::Window *window, std::shared_ptr<NWeb> nweb)
{
    if (window == nullptr) {
        return;
    }

    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    if (receiver_ == nullptr) {
        receiver_ = rsClient.CreateVSyncReceiver("NWeb_" + std::to_string(::getpid()));
        if (receiver_ == nullptr) {
            WVLOG_E("FAIL to CreateVSyncReceiver");
            return;
        }
    }
    receiver_->Init();

    std::weak_ptr<NWeb> nwebWeak(nweb);
    VsyncCbInfo *info = new(std::nothrow) VsyncCbInfo { nwebWeak };
    if (info == nullptr) {
        return;
    }
    Rosen::VSyncReceiver::FrameCallback frameCb = {
        .userData_ = reinterpret_cast<void *>(info),
        .callback_ = [window, this] (int64_t time, void *userdata) -> void {
            VsyncCbInfo *info = reinterpret_cast<VsyncCbInfo *>(userdata);
            std::weak_ptr<NWeb> nwebWeak = info->nwebWeak;
            delete info;
            info = nullptr;
            if (nwebWeak.expired()) {
                WVLOG_I("nweb instance has expired, stop request vsync");
                return;
            }
            this->VsyncCb(window, nwebWeak.lock());
        },
    };

    int ret = receiver_->RequestNextVSync(frameCb);
    if (ret != 0) {
        WVLOG_E("NWebWindowAdapter RequestVsync RequestNextVSync fail, ret=%{public}d", ret);
    }
}

void NWebWindowAdapter::VsyncCb(Rosen::Window *window, std::shared_ptr<NWeb> nweb)
{
    if (window == nullptr) {
        return;
    }

    if (windowInfoMap_[window].cachedSurfaceBuffer == nullptr) {
        RequestVsync(window, nweb);
        return;
    }

    sptr<Surface> surface = window->GetSurfaceNode()->GetSurface();
    if (surface == nullptr) {
        WVLOG_E("surface get from window is nullptr");
        RequestVsync(window, nweb);
        return;
    }

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = window->GetRect().width_,
            .h = window->GetRect().height_,
        },
        .timestamp = 0,
    };

    SurfaceError ret = surface->FlushBuffer(windowInfoMap_[window].cachedSurfaceBuffer, -1, flushConfig);
    if (ret == SURFACE_ERROR_OK) {
        windowInfoMap_[window].cachedSurfaceBuffer = nullptr;
    } else {
        WVLOG_E("FAIL flush nweb render frame to window surface, ret=%{public}d", ret);
    }

    RequestVsync(window, nweb);
}

void NWebWindowAdapter::RegistEventCb(Rosen::Window *window, std::shared_ptr<NWeb> nweb)
{
    if (window == nullptr) {
        return;
    }

    std::shared_ptr<Rosen::IInputEventConsumer> inputEventListener =
        std::make_shared<NWebInputEventConsumer>(nweb);

    window->SetInputEventConsumer(inputEventListener);
}
} // namespace OHOS::NWeb
