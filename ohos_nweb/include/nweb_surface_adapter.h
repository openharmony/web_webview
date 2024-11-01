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

#ifndef NWEB_SURFACE_ADAPTER_H
#define NWEB_SURFACE_ADAPTER_H

#include <cstdint>
#include <refbase.h>
#include <surface.h>

#include "nweb.h"
#include "nweb_init_params.h"
#include "surface_buffer.h"

namespace OHOS {
class Surface;
}

namespace OHOS::NWeb {
class NWebSurfaceAdapter {
public:
    static NWebSurfaceAdapter &Instance();
    std::shared_ptr<NWebCreateInfoImpl> GetCreateInfo(sptr<Surface> surface,
                                 std::shared_ptr<NWebEngineInitArgs> initArgs,
                                 uint32_t width,
                                 uint32_t height,
                                 bool incognitoMode = false);
    bool OutputFrameCallback(const char *buffer, uint32_t width, uint32_t height, wptr<Surface> surfaceWeak);

private:
    sptr<SurfaceBuffer> RequestBuffer(sptr<Surface> surface, uint32_t width, uint32_t height);
    bool CopyFrame(sptr<SurfaceBuffer> surfaceBuffer, const char *src, uint32_t width, uint32_t height);
    bool FlushBuffer(sptr<Surface> surface, sptr<SurfaceBuffer> surfaceBuffer, uint32_t width, uint32_t height);
};
} // namespace OHOS::NWeb

#endif // NWEB_SURFACE_ADAPTER_H