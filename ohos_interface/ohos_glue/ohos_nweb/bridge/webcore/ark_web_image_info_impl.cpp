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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_image_info_impl.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebImageInfoImpl::ArkWebImageInfoImpl(std::shared_ptr<OHOS::NWeb::NWebImageInfo> ref)
    : real_(ref)
{}
 
int32_t ArkWebImageInfoImpl::GetColorType()
{
    return real_->GetColorType();
}
 
int32_t ArkWebImageInfoImpl::GetAlphaType()
{
    return real_->GetAlphaType();
}
 
uint32_t* ArkWebImageInfoImpl::GetData()
{
    return real_->GetData();
}
 
uint32_t ArkWebImageInfoImpl::GetDataSize()
{
    return real_->GetDataSize();
}
 
int32_t ArkWebImageInfoImpl::GetWidth()
{
    return real_->GetWidth();
}
 
int32_t ArkWebImageInfoImpl::GetHeight()
{
    return real_->GetHeight();
}
 
void ArkWebImageInfoImpl::SetColorType(int32_t colorType)
{
    real_->SetColorType(colorType);
}
 
void ArkWebImageInfoImpl::SetAlphaType(int32_t alphaType)
{
    real_->SetAlphaType(alphaType);
}
 
void ArkWebImageInfoImpl::SetData(uint32_t* data)
{
    real_->SetData(data);
}
 
void ArkWebImageInfoImpl::SetDataSize(uint32_t dataSize)
{
    real_->SetDataSize(dataSize);
}
 
void ArkWebImageInfoImpl::SetWidth(int32_t width)
{
    real_->SetWidth(width);
}
 
void ArkWebImageInfoImpl::SetHeight(int32_t height)
{
    real_->SetHeight(height);
}
 
} // namespace OHOS::ArkWeb
