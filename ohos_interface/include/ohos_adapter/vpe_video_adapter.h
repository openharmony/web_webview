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

#ifndef VPE_VIDEO_ADAPTER_H
#define VPE_VIDEO_ADAPTER_H

#include <cstdint>

namespace OHOS::NWeb {

// Wraps OHOS VideoProcessingEngine (VPE) to provide detail-enhancement / AI-HDR
// for a surface.
class VpeVideoAdapter {
public:
    VpeVideoAdapter() = default;

    virtual ~VpeVideoAdapter() = default;

    // Wrap the output `window` with a VPE pipeline and return its input surface for the
    // producer to draw into. Returns `window` unchanged (passthrough) when VPE is
    // unsupported or creation fails.
    virtual void* CreateVpeSurface(uint64_t surfaceId, void* window) = 0;

    // Stop and release the VPE pipeline bound to `surfaceId`. No-op if absent.
    virtual void ReleaseVpeSurface(uint64_t surfaceId) = 0;
};
} // namespace OHOS::NWeb

#endif // VPE_VIDEO_ADAPTER_H
