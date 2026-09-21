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

#ifndef VPE_VIDEO_ADAPTER_IMPL_H
#define VPE_VIDEO_ADAPTER_IMPL_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "vpe_video_adapter.h"

#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
#include "surface.h"
// OHNativeWindow is `typedef struct NativeWindow OHNativeWindow;` (external_window.h).
// VpePipeline only holds a pointer to it, so a forward declaration is enough here and
// avoids pulling native_window.h's heavy transitive deps (NativeWindowMagic ->
// NativeObjectMagic enum -> window.h) into every including TU.
struct NativeWindow;
typedef struct NativeWindow OHNativeWindow;
namespace OHOS::Media::VideoProcessingEngine {
class VpeVideo;
}
#endif

namespace OHOS::NWeb {
// Always compiled. When NWEB_VIDEO_PROCESSING_ENGINE_ENABLE is undefined it is a
// passthrough stub (CreateVpeSurface returns the original window); the real VPE
// pipeline is built only when the feature is enabled.
//
// Ownership contract (unchanged signatures):
// - CreateVpeSurface returns a borrowed pointer. On success it is a new window
//   owned by the adapter; the caller must NOT destroy it. On any fallback path
//   (VPE unsupported / all stages failed / window null) the original input
//   window is returned and the caller keeps its ownership.
// - The returned pointer is valid until the pipeline is torn down, i.e. until
//   the next ReleaseVpeSurface for the same surfaceId or adapter destruction.
// - CreateVpeSurface is idempotent: a second call with the same surfaceId
//   returns the same borrowed window and rebuilds nothing. ReleaseVpeSurface on
//   an unknown surfaceId is a logged no-op.
class VpeVideoAdapterImpl : public VpeVideoAdapter {
public:
    VpeVideoAdapterImpl() = default;

    ~VpeVideoAdapterImpl() override;

    // `window` is an OHNativeWindow* passed as void* to keep the public
    // interface (vpe_video_adapter.h) free of graphic_2d type dependencies;
    // cast to OHNativeWindow* inside the implementation.
    void* CreateVpeSurface(uint64_t surfaceId, void* window) override;

    void ReleaseVpeSurface(uint64_t surfaceId) override;

private:
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    // A fully-built VPE pipeline for one surfaceId. Every field is owned by the
    // adapter and torn down together.
    struct VpePipeline {
        VpePipeline() = default;
        // Move-only: a pipeline owns its borrowed window and surface reference;
        // copies would double-restore the surface usage and double-destroy the
        // native window.
        VpePipeline(const VpePipeline&) = delete;
        VpePipeline& operator=(const VpePipeline&) = delete;
        VpePipeline(VpePipeline&&) = default;
        VpePipeline& operator=(VpePipeline&&) = default;

        // Stages in build order; stage[0]'s input is what the producer writes
        // into, the last stage's output is the original raw surface.
        std::vector<std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo>> videos;
        // Borrowed by the caller, destroyed by the adapter.
        OHNativeWindow* vpeWindow = nullptr;
        // Original input window (void* form) kept only to sanity-check that the
        // caller keeps surfaceId mapped to the same window across repeated calls.
        void* rawWindow = nullptr;
        // Original surface kept to restore its default usage on teardown.
        sptr<Surface> rawSurface;
        uint64_t originUsage = 0;
    };

    static bool SetVpeParameter(
        const std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo>& video, uint32_t type);

    // Build a single VPE stage of `type` whose output is `outputSurface`. On
    // success appends the new video stage to `pipeline.videos` and returns the
    // stage's input surface; on any failure returns `outputSurface` unchanged
    // and leaves `pipeline` untouched.
    sptr<Surface> BuildVpeStage(uint32_t type, uint64_t surfaceId, const sptr<Surface>& outputSurface,
        VpePipeline& pipeline);

    // Build a fresh pipeline for `window` from scratch. Pure: touches neither
    // pipelines_ nor the lock, so it is independently unit-testable. On success
    // returns a pipeline with vpeWindow set; on any failure (VPE unsupported /
    // all stages failed / native-window creation failed) returns a pipeline
    // with vpeWindow == nullptr and any surface usage it changed restored.
    VpePipeline BuildPipeline(uint64_t surfaceId, void* window);

    // Tear a pipeline down (caller already holds no lock): destroy the borrowed
    // native window, stop/release each stage in reverse order, restore the
    // original surface default usage. Idempotent and safe on a pipeline in any
    // state (empty / built-but-not-published / fully published): every field is
    // independently null-checked.
    void DestroyPipeline(VpePipeline& pipeline);

    std::mutex vpeVideoLock_;
    // Map from rawSurfaceId to its VPE pipeline.
    std::unordered_map<uint64_t, VpePipeline> pipelines_;
#endif
};
} // namespace OHOS::NWeb
#endif // VPE_VIDEO_ADAPTER_IMPL_H
