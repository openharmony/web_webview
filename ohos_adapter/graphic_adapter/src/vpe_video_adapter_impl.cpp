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

#include "vpe_video_adapter_impl.h"

#include <cinttypes>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
#include "detail_enhancer_video.h"
#include "surface.h"
#include "surface/window.h"
#include "native_window.h"
#include "external_window.h"
#include "meta/format.h"
#include "nweb_log.h"
#endif

namespace OHOS::NWeb {
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
using namespace OHOS::Media::VideoProcessingEngine;

// Internal callback: release each processed output buffer back to the surface.
class VpeVideoCallbackImpl : public VpeVideoCallback {
public:
    VpeVideoCallbackImpl() = default;

    ~VpeVideoCallbackImpl() override = default;

    explicit VpeVideoCallbackImpl(const std::shared_ptr<VpeVideo>& video) : video_(video) {}

    void OnOutputBufferAvailable(uint32_t index, VpeBufferFlag flag) override
    {
        if (video_.expired()) {
            WVLOG_E("OnOutputBufferAvailable video is expired");
            return;
        }
        std::shared_ptr<VpeVideo> video = video_.lock();
        if (video == nullptr) {
            WVLOG_E("OnOutputBufferAvailable video is null");
            return;
        }
        video->ReleaseOutputBuffer(index, true);
    }

private:
    std::weak_ptr<VpeVideo> video_;
};
#endif

VpeVideoAdapter& VpeVideoAdapterImpl::GetInstance()
{
    static VpeVideoAdapterImpl instance;
    return instance;
}

VpeVideoAdapterImpl::~VpeVideoAdapterImpl()
{
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    std::unordered_map<uint64_t, std::shared_ptr<VpeVideo>> videos;
    {
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        videos.swap(allVpeVideo_);
    }
    for (auto& item : videos) {
        if (item.second != nullptr) {
            item.second->Stop();
            item.second->Release();
        }
    }
#endif
}

void VpeVideoAdapterImpl::ReleaseVpeSurface(uint64_t surfaceId)
{
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    std::shared_ptr<VpeVideo> video;
    {
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        auto it = allVpeVideo_.find(surfaceId);
        if (it == allVpeVideo_.end()) {
            return;
        }
        video = it->second;
        allVpeVideo_.erase(it);
    }
    if (video != nullptr) {
        VPEAlgoErrCode stopRet = video->Stop();
        VPEAlgoErrCode releaseRet = video->Release();
        WVLOG_I("ReleaseVpeSurface Stop:%{public}d, Release:%{public}d, surfaceId:%{public}" PRIu64,
            static_cast<int32_t>(stopRet), static_cast<int32_t>(releaseRet), surfaceId);
    }
#endif
}

#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
bool VpeVideoAdapterImpl::SetVpeParameter(const std::shared_ptr<VpeVideo>& video, uint32_t type)
{
    if (type == VIDEO_TYPE_DETAIL_ENHANCER) {
        Media::Format qualityParam {};
        qualityParam.PutIntValue(ParameterKey::DETAIL_ENHANCER_QUALITY_LEVEL, DETAIL_ENHANCER_LEVEL_HIGH);
        if (video->SetParameter(qualityParam) != 0) {
            WVLOG_E("SetVpeParameter quality failed, type:%{public}u", type);
            return false;
        }
    }
    return true;
}

sptr<Surface> VpeVideoAdapterImpl::BuildVpeForType(uint32_t type, uint64_t surfaceId,
    const sptr<Surface>& surface, std::shared_ptr<VpeVideo>& outVideo)
{
    if (!VpeVideo::IsSurfaceSupported(type, surface)) {
        WVLOG_D("BuildVpeForType surface not supported, type:%{public}u", type);
        return surface;
    }
    ReleaseVpeSurface(surfaceId);
    std::shared_ptr<VpeVideo> video = VpeVideo::Create(type);
    if (video == nullptr) {
        WVLOG_E("BuildVpeForType Create failed, type:%{public}u", type);
        return surface;
    }
    auto callback = std::make_shared<VpeVideoCallbackImpl>(video);
    if (video->RegisterCallback(callback) != 0) {
        WVLOG_E("BuildVpeForType RegisterCallback failed, type:%{public}u", type);
        return surface;
    }
    if (!SetVpeParameter(video, type)) {
        return surface;
    }
    if (video->SetOutputSurface(surface) != 0) {
        WVLOG_E("BuildVpeForType SetOutputSurface failed, type:%{public}u", type);
        return surface;
    }
    sptr<Surface> inputSurface = video->GetInputSurface();
    if (inputSurface == nullptr) {
        WVLOG_E("BuildVpeForType GetInputSurface failed, type:%{public}u", type);
        return surface;
    }
    if (video->Start() != 0) {
        WVLOG_E("BuildVpeForType Start failed, type:%{public}u", type);
        return surface;
    }
    outVideo = video;
    WVLOG_I("BuildVpeForType vpe created, type:%{public}u, surfaceId:%{public}" PRIu64, type, surfaceId);
    return inputSurface;
}
#endif

void* VpeVideoAdapterImpl::CreateVpeSurface(uint64_t surfaceId, void* window)
{
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    WVLOG_I("CreateVpeSurface surfaceId:%{public}" PRIu64, surfaceId);
    if (window == nullptr) {
        WVLOG_E("CreateVpeSurface window is null, surfaceId:%{public}" PRIu64, surfaceId);
        return nullptr;
    }
    if (!VpeVideo::IsSupported()) {
        WVLOG_I("CreateVpeSurface VPE not supported, passthrough window");
        return window;
    }
    OHNativeWindow* nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    sptr<Surface> rawSurface = nativeWindow->surface;
    if (rawSurface == nullptr) {
        WVLOG_E("CreateVpeSurface invalid native window, surfaceId:%{public}" PRIu64, surfaceId);
        return window;
    }
    uint64_t originUsage = rawSurface->GetDefaultUsage();
    rawSurface->SetDefaultUsage(originUsage | BUFFER_USAGE_HW_COMPOSER);
    sptr<Surface> vpeSurface = rawSurface;
    Media::Format parameter {};
    std::vector<uint32_t> supportTypes = { VIDEO_TYPE_DETAIL_ENHANCER, VIDEO_TYPE_AIHDR_ENHANCER };
    for (uint32_t type : supportTypes) {
        if (!VpeVideo::IsSupported(type, parameter)) {
            WVLOG_D("CreateVpeSurface type not supported:%{public}u", type);
            continue;
        }
        std::shared_ptr<VpeVideo> video;
        vpeSurface = BuildVpeForType(type, surfaceId, vpeSurface, video);
        if (video != nullptr) {
            std::lock_guard<std::mutex> lock(vpeVideoLock_);
            allVpeVideo_[surfaceId] = video;
        }
    }
    // If the VPE surface build failed, vpeSurface stays equal to rawSurface; restore the original usage.
    if (vpeSurface.GetRefPtr() == rawSurface.GetRefPtr()) {
        rawSurface->SetDefaultUsage(originUsage);
        return window;
    }
    OHNativeWindow* vpeWindow = OH_NativeWindow_CreateNativeWindow(&vpeSurface);
    if (vpeWindow == nullptr) {
        WVLOG_E("CreateVpeSurface CreateNativeWindow failed, surfaceId:%{public}" PRIu64, surfaceId);
        ReleaseVpeSurface(surfaceId);
        rawSurface->SetDefaultUsage(originUsage);
        return window;
    }
    // Ownership of vpeWindow is transferred to the caller; the caller must OH_NativeWindow_DestroyNativeWindow it.
    return vpeWindow;
#else
    return window;
#endif
}
} // namespace OHOS::NWeb
