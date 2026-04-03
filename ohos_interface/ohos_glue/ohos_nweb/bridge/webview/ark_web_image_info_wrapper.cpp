/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_image_info_wrapper.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
 
namespace OHOS::ArkWeb {
 
ArkWebImageInfoWrapper::ArkWebImageInfoWrapper(ArkWebRefPtr<ArkWebImageInfo> ref)
    : ctocpp_(ref)
{}
 
int32_t ArkWebImageInfoWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}
 
int32_t ArkWebImageInfoWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}
 
int32_t ArkWebImageInfoWrapper::GetAlphaType()
{
    return ctocpp_->GetAlphaType();
}
 
int32_t ArkWebImageInfoWrapper::GetColorType()
{
    return ctocpp_->GetColorType();
}
 
uint32_t* ArkWebImageInfoWrapper::GetData()
{
    return ctocpp_->GetData();
}
 
uint32_t ArkWebImageInfoWrapper::GetDataSize()
{
    return ctocpp_->GetDataSize();
}
 
void ArkWebImageInfoWrapper::SetWidth(int32_t width)
{
    ctocpp_->SetWidth(width);
}
 
void ArkWebImageInfoWrapper::SetHeight(int32_t height)
{
    ctocpp_->SetHeight(height);
}
 
void ArkWebImageInfoWrapper::SetAlphaType(int32_t alphaType)
{
    ctocpp_->SetAlphaType(alphaType);
}
 
void ArkWebImageInfoWrapper::SetColorType(int32_t colorType)
{
    ctocpp_->SetColorType(colorType);
}
 
void ArkWebImageInfoWrapper::SetData(uint32_t *data)
{
    ctocpp_->SetData(data);
}
 
void ArkWebImageInfoWrapper::SetDataSize(uint32_t dataSize)
{
    ctocpp_->SetDataSize(dataSize);
}
 
} // namespace OHOS::ArkWeb
