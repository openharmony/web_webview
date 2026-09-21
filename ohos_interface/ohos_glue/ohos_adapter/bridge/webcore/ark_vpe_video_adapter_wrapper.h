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

#ifndef ARK_VPE_VIDEO_ADAPTER_WRAPPER_H
#define ARK_VPE_VIDEO_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_graphic_adapter.h"
#include "vpe_video_adapter.h"

namespace OHOS::ArkWeb {
class ArkVpeVideoAdapterWrapper : public OHOS::NWeb::VpeVideoAdapter {
public:
    explicit ArkVpeVideoAdapterWrapper(ArkWebRefPtr<ArkVpeVideoAdapter>);

    void* CreateVpeSurface(uint64_t surfaceId, void* window) override;

    void ReleaseVpeSurface(uint64_t surfaceId) override;

private:
    ArkWebRefPtr<ArkVpeVideoAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb
#endif // ARK_VPE_VIDEO_ADAPTER_WRAPPER_H
