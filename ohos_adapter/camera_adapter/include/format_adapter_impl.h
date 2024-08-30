/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FORMAT_ADAPTER_IMPL_H
#define FORMAT_ADAPTER_IMPL_H

#include "camera_manager_adapter.h"

namespace OHOS::NWeb {

class FormatAdapterImpl : public FormatAdapter {
public:
    FormatAdapterImpl() = default;

    uint32_t GetWidth() override;

    uint32_t GetHeight() override;

    float GetFrameRate() override;

    VideoPixelFormatAdapter GetPixelFormat() override;

    void SetWidth(uint32_t width);

    void SetHeight(uint32_t height);

    void SetFrameRate(float rate);

    void SetPixelFormat(VideoPixelFormatAdapter format);

private:
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    float frame_rate_ = 0.0;
    VideoPixelFormatAdapter pixel_format_ = VideoPixelFormatAdapter::FORMAT_UNKNOWN;
};

} // namespace OHOS::NWeb

#endif // FORMAT_ADAPTER_IMPL_H
