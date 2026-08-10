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
// Order matters: native_window.h's `struct NativeWindow` (surface/include) has
// a `sptr<Surface>` member and pulls in window.h's NativeObjectMagic enum, so
// surface.h / surface/window.h must be included first. Keep native_window.h
// after the surface headers.
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

void VpeVideoAdapterImpl::DestroyPipeline(VpePipeline& pipeline)
{
    if (pipeline.vpeWindow != nullptr) {
        OH_NativeWindow_DestroyNativeWindow(pipeline.vpeWindow);
        pipeline.vpeWindow = nullptr;
    }
    // Tear down stages in reverse build order: input-side stages last so each
    // stage's output surface consumer (the next stage) has already stopped.
    for (auto it = pipeline.videos.rbegin(); it != pipeline.videos.rend(); ++it) {
        if (*it != nullptr) {
            auto stopRet = (*it)->Stop();
            auto releaseRet = (*it)->Release();
            WVLOG_I("DestroyPipeline Stop:%{public}d, Release:%{public}d",
                static_cast<int32_t>(stopRet), static_cast<int32_t>(releaseRet));
        }
    }
    pipeline.videos.clear();
    if (pipeline.rawSurface != nullptr) {
        pipeline.rawSurface->SetDefaultUsage(pipeline.originUsage);
        pipeline.rawSurface = nullptr;
    }
}
#endif

VpeVideoAdapterImpl::~VpeVideoAdapterImpl()
{
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    std::unordered_map<uint64_t, VpePipeline> maps;
    {
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        maps.swap(pipelines_);
    }
    for (auto& item : maps) {
        DestroyPipeline(item.second);
    }
#endif
}

void VpeVideoAdapterImpl::ReleaseVpeSurface(uint64_t surfaceId)
{
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    VpePipeline pipeline;
    bool found = false;
    {
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        auto it = pipelines_.find(surfaceId);
        if (it != pipelines_.end()) {
            pipeline = std::move(it->second);
            pipelines_.erase(it);
            found = true;
        }
    }
    if (!found) {
        WVLOG_W("ReleaseVpeSurface surfaceId not found:%{public}" PRIu64, surfaceId);
        return;
    }
    DestroyPipeline(pipeline);
    WVLOG_I("ReleaseVpeSurface released, surfaceId:%{public}" PRIu64, surfaceId);
#endif
}

#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
// Build a single VPE stage of `type` whose output is `outputSurface`.
// On success appends the new video stage to `pipeline.videos` and returns the
// stage's input surface (which becomes the next stage's consumer); on any
// failure returns `outputSurface` unchanged and leaves `pipeline` untouched.
sptr<Surface> VpeVideoAdapterImpl::BuildVpeStage(
    uint32_t type, uint64_t surfaceId, const sptr<Surface>& outputSurface, VpePipeline& pipeline)
{
    if (!VpeVideo::IsSurfaceSupported(type, outputSurface)) {
        WVLOG_D("BuildVpeStage surface not supported, type:%{public}u", type);
        return outputSurface;
    }
    std::shared_ptr<VpeVideo> video = VpeVideo::Create(type);
    if (video == nullptr) {
        WVLOG_E("BuildVpeStage Create failed, type:%{public}u", type);
        return outputSurface;
    }
    auto callback = std::make_shared<VpeVideoCallbackImpl>(video);
    if (video->RegisterCallback(callback) != 0) {
        WVLOG_E("BuildVpeStage RegisterCallback failed, type:%{public}u", type);
        return outputSurface;
    }
    if (!SetVpeParameter(video, type)) {
        return outputSurface;
    }
    if (video->SetOutputSurface(outputSurface) != 0) {
        WVLOG_E("BuildVpeStage SetOutputSurface failed, type:%{public}u", type);
        return outputSurface;
    }
    sptr<Surface> inputSurface = video->GetInputSurface();
    if (inputSurface == nullptr) {
        WVLOG_E("BuildVpeStage GetInputSurface failed, type:%{public}u", type);
        return outputSurface;
    }
    if (video->Start() != 0) {
        WVLOG_E("BuildVpeStage Start failed, type:%{public}u", type);
        return outputSurface;
    }
    pipeline.videos.push_back(video);
    WVLOG_I("BuildVpeStage vpe stage created, type:%{public}u, surfaceId:%{public}" PRIu64, type, surfaceId);
    return inputSurface;
}

// Build a fresh pipeline for `window` from scratch. Pure: touches neither
// pipelines_ nor the lock. On success returns a pipeline with vpeWindow set;
// on any failure returns a pipeline with vpeWindow == nullptr and any surface
// usage it changed restored.
VpeVideoAdapterImpl::VpePipeline VpeVideoAdapterImpl::BuildPipeline(uint64_t surfaceId, void* window)
{
    VpePipeline pipeline;
    if (!VpeVideo::IsSupported()) {
        WVLOG_I("BuildPipeline VPE not supported, surfaceId:%{public}" PRIu64, surfaceId);
        return pipeline; // vpeWindow == nullptr
    }
    OHNativeWindow* nativeWindow = reinterpret_cast<OHNativeWindow*>(window);
    sptr<Surface> rawSurface = nativeWindow->surface;
    if (rawSurface == nullptr) {
        WVLOG_E("BuildPipeline invalid native window, surfaceId:%{public}" PRIu64, surfaceId);
        return pipeline; // vpeWindow == nullptr
    }

    pipeline.rawWindow = window;
    pipeline.rawSurface = rawSurface;
    pipeline.originUsage = rawSurface->GetDefaultUsage();
    // Hardware-composer usage is required for the VPE output path; restored by
    // DestroyPipeline on teardown.
    rawSurface->SetDefaultUsage(pipeline.originUsage | BUFFER_USAGE_HW_COMPOSER);

    // Build stages in chain order. The output surface of stage[i] is the input
    // surface of stage[i-1]; the first stage's output is the original raw
    // surface, and the last stage's input surface becomes the producer-facing
    // surface returned to the caller. Types are tried in priority order; a
    // chain with zero successful stages falls back to the raw window.
    Media::Format parameter {};
    std::vector<uint32_t> supportTypes = { VIDEO_TYPE_DETAIL_ENHANCER, VIDEO_TYPE_AIHDR_ENHANCER };
    sptr<Surface> outputSurface = rawSurface;
    for (uint32_t type : supportTypes) {
        if (!VpeVideo::IsSupported(type, parameter)) {
            WVLOG_D("BuildPipeline type not supported:%{public}u", type);
            continue;
        }
        outputSurface = BuildVpeStage(type, surfaceId, outputSurface, pipeline);
    }

    if (pipeline.videos.empty()) {
        // Nothing was built; restore the usage we changed and reset to a clean
        // empty state so the returned pipeline is uniform (vpeWindow == nullptr
        // implies rawSurface == nullptr / no resources held).
        rawSurface->SetDefaultUsage(pipeline.originUsage);
        pipeline.rawSurface = nullptr;
        return pipeline; // vpeWindow == nullptr
    }

    OHNativeWindow* vpeWindow = OH_NativeWindow_CreateNativeWindow(&outputSurface);
    if (vpeWindow == nullptr) {
        WVLOG_E("BuildPipeline CreateNativeWindow failed, surfaceId:%{public}" PRIu64, surfaceId);
        DestroyPipeline(pipeline);
        return pipeline; // vpeWindow == nullptr
    }
    pipeline.vpeWindow = vpeWindow;
    return pipeline;
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
    // Idempotency: a pipeline already bound to surfaceId is reused as-is and
    // the same borrowed window is returned. Create(Create(x)) == Create(x): no
    // teardown, no rebuild, no pipeline interruption. The caller is expected to
    // keep surfaceId mapped to the same window; a mismatch is logged but never
    // triggers a rebuild (that would break idempotency).
    {
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        auto it = pipelines_.find(surfaceId);
        if (it != pipelines_.end() && it->second.vpeWindow != nullptr) {
            if (window != it->second.rawWindow) {
                WVLOG_W("CreateVpeSurface surfaceId reused with different window:%{public}" PRIu64, surfaceId);
            }
            WVLOG_D("CreateVpeSurface reuse existing pipeline, surfaceId:%{public}" PRIu64, surfaceId);
            return it->second.vpeWindow;
        }
    }

    // Build a fresh pipeline off the lock; BuildPipeline is pure (touches
    // neither pipelines_ nor the lock). vpeWindow == nullptr means any failure
    // (VPE unsupported / all stages failed / native-window creation failed);
    // in that case passthrough the original window.
    VpePipeline pipeline = BuildPipeline(surfaceId, window);
    if (pipeline.vpeWindow == nullptr) {
        return window;
    }
    // Capture the borrowed window before moving the pipeline into the map.
    OHNativeWindow* vpeWindow = pipeline.vpeWindow;

    {
        // Double-check under the lock: another thread may have raced ahead and
        // inserted a pipeline for the same surfaceId while we built this one
        // unlocked. If so, drop the one we just built (its window/videos would
        // otherwise leak forever, since map insertion would overwrite it) and
        // return the winner. DestroyPipeline operates only on the local
        // `pipeline` — it touches neither `pipelines_` nor the lock, so holding
        // the lock here cannot deadlock.
        std::lock_guard<std::mutex> lock(vpeVideoLock_);
        auto it = pipelines_.find(surfaceId);
        if (it != pipelines_.end() && it->second.vpeWindow != nullptr) {
            WVLOG_W("CreateVpeSurface concurrent build discarded, surfaceId:%{public}" PRIu64, surfaceId);
            DestroyPipeline(pipeline);
            return it->second.vpeWindow;
        }
        pipelines_[surfaceId] = std::move(pipeline);
    }
    // Ownership note: the adapter owns vpeWindow. The caller borrows it and
    // must NOT destroy it; it is destroyed on Release/next Create/destruction.
    return vpeWindow;
#else
    return window;
#endif
}
} // namespace OHOS::NWeb
