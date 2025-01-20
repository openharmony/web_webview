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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_adapter/bridge/ark_ohos_image_decoder_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkOhosImageDecoderAdapterWrapper::ArkOhosImageDecoderAdapterWrapper(ArkWebRefPtr<ArkOhosImageDecoderAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkOhosImageDecoderAdapterWrapper::ParseImageInfo(const uint8_t* data, uint32_t size)
{
    return ctocpp_->ParseImageInfo(data, size);
}

std::string ArkOhosImageDecoderAdapterWrapper::GetEncodedFormat()
{
    ArkWebString str = ctocpp_->GetEncodedFormat();
    std::string result = ArkWebStringStructToClass(ctocpp_->GetEncodedFormat());
    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkOhosImageDecoderAdapterWrapper::GetImageWidth()
{
    return ctocpp_->GetImageWidth();
}

int32_t ArkOhosImageDecoderAdapterWrapper::GetImageHeight()
{
    return ctocpp_->GetImageHeight();
}

bool ArkOhosImageDecoderAdapterWrapper::DecodeToPixelMap(const uint8_t* data, uint32_t size)
{
    return ctocpp_->DecodeToPixelMap(data, size);
}


bool ArkOhosImageDecoderAdapterWrapper::Decode(const uint8_t* data,
                                               uint32_t size,
                                               OHOS::NWeb::AllocatorType type,
                                               bool useYuv)
{
    return ctocpp_->Decode(data, size, (uint32_t)type, useYuv);
}

int32_t ArkOhosImageDecoderAdapterWrapper::GetFd()
{
    return ctocpp_->GetFd();
}

int32_t ArkOhosImageDecoderAdapterWrapper::GetStride()
{
    return ctocpp_->GetStride();
}

int32_t ArkOhosImageDecoderAdapterWrapper::GetOffset()
{
    return ctocpp_->GetOffset();
}

uint64_t ArkOhosImageDecoderAdapterWrapper::GetSize()
{
    return ctocpp_->GetSize();
}

void* ArkOhosImageDecoderAdapterWrapper::GetNativeWindowBuffer()
{
    return ctocpp_->GetNativeWindowBuffer();
}

int32_t ArkOhosImageDecoderAdapterWrapper::GetPlanesCount()
{
    return ctocpp_->GetPlanesCount();
}

void ArkOhosImageDecoderAdapterWrapper::ReleasePixelMap()
{
    return ctocpp_->ReleasePixelMap();
}

void* ArkOhosImageDecoderAdapterWrapper::GetDecodeData()
{
    return ctocpp_->GetDecodeData();
}

} // namespace OHOS::ArkWeb
