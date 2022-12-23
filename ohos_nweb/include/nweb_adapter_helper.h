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

#ifndef NWEB_ADAPTER_HELPER_H
#define NWEB_ADAPTER_HELPER_H

#include "window.h"
#include "nweb_helper.h"

namespace OHOS {
class Surface;
}

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebAdapterHelper {
public:
    static NWebAdapterHelper &Instance();
    ~NWebAdapterHelper() = default;
    bool Init(bool from_ark = true);
    std::shared_ptr<NWeb> CreateNWeb(Rosen::Window *window, const NWebInitArgs &initArgs);
    std::shared_ptr<NWeb> CreateNWeb(sptr<Surface> surface,
                                     const NWebInitArgs &initArgs,
                                     uint32_t width = 0,
                                     uint32_t height = 0);
    std::shared_ptr<NWeb> CreateNWeb(void *enhanceSurfaceInfo,
                                     const NWebInitArgs &initArgs,
                                     uint32_t width = 0,
                                     uint32_t height = 0);
private:
    NWebAdapterHelper() = default;
};
} // namespace OHOS

#endif // NWEB_ADAPTER_HELPER_H