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

#include "ark_video_capture_params_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkVideoCaptureParamsAdapterWrapper::ArkVideoCaptureParamsAdapterWrapper(ArkWebRefPtr<ArkVideoCaptureParamsAdapter> ref)
    : ctocpp_(ref)
{}

uint32_t ArkVideoCaptureParamsAdapterWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}

uint32_t ArkVideoCaptureParamsAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}

float ArkVideoCaptureParamsAdapterWrapper::GetFrameRate()
{
    return ctocpp_->GetFrameRate();
}

NWeb::VideoPixelFormatAdapter ArkVideoCaptureParamsAdapterWrapper::GetPixelFormat()
{
    return (NWeb::VideoPixelFormatAdapter)ctocpp_->GetPixelFormat();
}

bool ArkVideoCaptureParamsAdapterWrapper::GetEnableFaceDetection()
{
    return ctocpp_->GetEnableFaceDetection();
}

} // namespace OHOS::ArkWeb