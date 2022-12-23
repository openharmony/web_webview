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

#include "nweb_enhance_surface_adapter.h"

#include <display_type.h>
#include <securec.h>
#include <sync_fence.h>

#include "graphic_common.h"
#include "graphic_common_c.h"
#include "include/core/SkTypes.h"
#include "nweb_log.h"
#include "surface_type.h"

namespace OHOS::NWeb {
NWebEnhanceSurfaceAdapter &NWebEnhanceSurfaceAdapter::Instance()
{
    static NWebEnhanceSurfaceAdapter surfaceAdapter;
    return surfaceAdapter;
}

NWebCreateInfo NWebEnhanceSurfaceAdapter::GetCreateInfo(void *enhanceSurfaceInfo,
                                                        const NWebInitArgs &initArgs,
                                                        uint32_t width,
                                                        uint32_t height)
{
    NWebCreateInfo createInfo = {
        .init_args = initArgs,
        .enhance_surface_info = enhanceSurfaceInfo,
    };
    GetSize(createInfo, width, height);
    GetRenderInterface(createInfo);
    return createInfo;
}

void NWebEnhanceSurfaceAdapter::GetSize(NWebCreateInfo &createInfo,
                                        uint32_t width,
                                        uint32_t height) const
{
    createInfo.width = width;
    createInfo.height = height;
}

void NWebEnhanceSurfaceAdapter::GetRenderInterface(NWebCreateInfo &createInfo)
{
    createInfo.output_render_frame = [] (const char *buffer, uint32_t width, uint32_t height) -> bool {
        return true;
    };
}
} // namespace OHOS::NWeb
