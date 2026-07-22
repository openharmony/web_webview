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

#include "vpe_video_adapter.h"

#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
#include "surface.h"
namespace OHOS::Media::VideoProcessingEngine {
class VpeVideo;
}
#endif

namespace OHOS::NWeb {
// Always compiled. When NWEB_VIDEO_PROCESSING_ENGINE_ENABLE is undefined it is a
// passthrough stub (CreateVpeSurface returns the original window); the real VPE
// pipeline is built only when the feature is enabled.
class VpeVideoAdapterImpl : public VpeVideoAdapter {
public:
    static VpeVideoAdapter& GetInstance();

    VpeVideoAdapterImpl() = default;

    ~VpeVideoAdapterImpl() override;

    void* CreateVpeSurface(uint64_t surfaceId, void* window) override;

    void ReleaseVpeSurface(uint64_t surfaceId) override;

private:
#if defined(NWEB_VIDEO_PROCESSING_ENGINE_ENABLE)
    bool SetVpeParameter(const std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo>& video,
        uint32_t type);

    sptr<Surface> BuildVpeForType(uint32_t type, uint64_t surfaceId, const sptr<Surface>& surface,
        std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo>& outVideo);

    std::mutex vpeVideoLock_;
    // Map from rawSurfaceId to VpeVideo instance
    std::unordered_map<uint64_t,
        std::shared_ptr<OHOS::Media::VideoProcessingEngine::VpeVideo>> allVpeVideo_;
#endif
};
} // namespace OHOS::NWeb
#endif // VPE_VIDEO_ADAPTER_IMPL_H
