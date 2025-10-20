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

#include "nweb_surface_adapter.h"

#include <securec.h>
#include <sync_fence.h>

#include "graphic_common.h"
#include "graphic_common_c.h"
#include "nweb_log.h"
#include "surface_type.h"

namespace {
constexpr int BITS_PER_PIXEL = 4;
}

namespace OHOS::NWeb {
//LCOV_EXCL_START
class NWebOutputFrameCallbackImpl : public NWebOutputFrameCallback {
public:
    NWebOutputFrameCallbackImpl(wptr<Surface> surface, NWebSurfaceAdapter *adapter) : surface_(surface),
                                                                                      adapter_(adapter) {}
    ~NWebOutputFrameCallbackImpl() = default;

    bool Handle(const char* buffer, uint32_t width, uint32_t height) override
    {
        return adapter_->OutputFrameCallback(buffer, width, height, surface_);
    }

private:
    wptr<Surface> surface_ = nullptr;
    NWebSurfaceAdapter *adapter_ = nullptr;
};

NWebSurfaceAdapter &NWebSurfaceAdapter::Instance()
{
    static NWebSurfaceAdapter surfaceAdapter;
    return surfaceAdapter;
}

std::shared_ptr<NWebCreateInfoImpl> NWebSurfaceAdapter::GetCreateInfo(sptr<Surface> surface,
    std::shared_ptr<NWebEngineInitArgs> initArgs, uint32_t width, uint32_t height, bool incognitoMode)
{
    std::shared_ptr<NWebCreateInfoImpl> createInfo = std::make_shared<NWebCreateInfoImpl>();
    createInfo->SetEngineInitArgs(initArgs);
    createInfo->SetProducerSurface(reinterpret_cast<void *>(&surface));

    if (surface == nullptr) {
        return createInfo;
    }

    createInfo->SetIsIncognitoMode(incognitoMode);
    createInfo->SetWidth((width == 0) ? (uint32_t)surface->GetDefaultWidth() : width);
    createInfo->SetHeight((height == 0) ? (uint32_t)surface->GetDefaultHeight() : height);

    wptr<Surface> surfaceWeak(surface);
    createInfo->SetOutputFrameCallback(std::make_shared<NWebOutputFrameCallbackImpl>(surfaceWeak, this));
    return createInfo;
}

bool NWebSurfaceAdapter::OutputFrameCallback(const char *buffer, uint32_t width, uint32_t height,
                                             wptr<Surface> surfaceWeak)
{
    sptr<Surface> surface = surfaceWeak.promote();
    if (surface == nullptr) {
        WVLOG_E("surface is nullptr or has expired");
        return false;
    }

    sptr<SurfaceBuffer> surfaceBuffer = this->RequestBuffer(surface, width, height);
    if (surfaceBuffer == nullptr) {
        return false;
    }

    if (!this->CopyFrame(surfaceBuffer, buffer, width, height)) {
        surface->CancelBuffer(surfaceBuffer);
        return false;
    }

    return this->FlushBuffer(surface, surfaceBuffer, width, height);
}

sptr<SurfaceBuffer> NWebSurfaceAdapter::RequestBuffer(sptr<Surface> surface, uint32_t width, uint32_t height)
{
    if (surface == nullptr) {
        return nullptr;
    }

    BufferRequestConfig config = {
        .width = width,
        .height = height,
        .strideAlignment = sizeof(void *),
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };

    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    SurfaceError ret = surface->RequestBuffer(surfaceBuffer, releaseFence, config);
    if (ret != SURFACE_ERROR_OK) {
        WVLOG_E("fail to request buffer from surface, errorcode=%{public}d", ret);
        return nullptr;
    }

    sptr<SyncFence> tempFence = new (std::nothrow) SyncFence(releaseFence);
    if (tempFence == nullptr) {
        WVLOG_E("new tempFence failed");
        return nullptr;
    }
    tempFence->Wait(100); // 100 ms

    return surfaceBuffer;
}

bool NWebSurfaceAdapter::CopyFrame(
    sptr<SurfaceBuffer> surfaceBuffer, const char *src, uint32_t width, uint32_t height)
{
    if (surfaceBuffer == nullptr) {
        return false;
    }

    char *dst = reinterpret_cast<char *>(surfaceBuffer->GetVirAddr());
    if (dst == nullptr) {
        WVLOG_E("fail to get buffer addr");
        return false;
    }

    uint32_t srcStride = width * BITS_PER_PIXEL;
    uint32_t dstStride = (uint32_t)surfaceBuffer->GetStride();
    uint32_t copiedSize = 0;

    for (uint32_t currHeight = 0; currHeight < height; ++currHeight) {
        if (copiedSize + dstStride > surfaceBuffer->GetSize()) {
            WVLOG_E("copy size overflow, drop this frame(%{public}u*%{public}u)", width, height);
            return false;
        }
        errno_t ret = memcpy_s(dst, static_cast<size_t>(dstStride), src, static_cast<size_t>(srcStride));
        if (ret != EOK) {
            WVLOG_E("memcpy_s failed");
            return false;
        }
        src += srcStride;
        dst += dstStride;
        copiedSize += dstStride;
    }

    return true;
}

bool NWebSurfaceAdapter::FlushBuffer(
    sptr<Surface> surface, sptr<SurfaceBuffer> surfaceBuffer, uint32_t width, uint32_t height)
{
    if (surface == nullptr) {
        return false;
    }

    BufferFlushConfig flushConfig = {
        .damage = {
            .w = width,
            .h = height,
        },
        .timestamp = 0,
    };

    SurfaceError ret = surface->FlushBuffer(surfaceBuffer, -1, flushConfig);
    if (ret != SURFACE_ERROR_OK) {
        WVLOG_E("FAIL flush nweb render frame, ret=%{public}d", ret);
        return false;
    }

    return true;
}
//LCOV_EXCL_STOP
} // namespace OHOS::NWeb
