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

#include "ohos_adapter/bridge/ark_ohos_drawing_text_adapter_impl.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using namespace OHOS::NWeb;

ArkOhosDrawingTextFontAdapterImpl::ArkOhosDrawingTextFontAdapterImpl(NWeb::OhosDrawingTextFontAdapter& ref)
    : real_(ref) {}

int ArkOhosDrawingTextFontAdapterImpl::GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray)
{
    return real_.GetSystemFontFullNamesByType(systemFontType, drawingArray);
}

int ArkOhosDrawingTextFontAdapterImpl::GetFontDescriptorByFullName(void* drawingString,
    int32_t systemFontType, void** drawingFontDescriptor)
{
    return real_.GetFontDescriptorByFullName(drawingString, systemFontType, drawingFontDescriptor);
}

int ArkOhosDrawingTextFontAdapterImpl::GetSystemFontFullNameByIndex(void* drawingArray,
                                                                    int32_t indexOfFullName, const void** drawingString)
{
    return real_.GetSystemFontFullNameByIndex(drawingArray, indexOfFullName, drawingString);
}

void ArkOhosDrawingTextFontAdapterImpl::DestroySystemFontFullNames(void* drawingArray)
{
    real_.DestroySystemFontFullNames(drawingArray);
}

void ArkOhosDrawingTextFontAdapterImpl::DestroyFontDescriptor(void* descriptor)
{
    real_.DestroyFontDescriptor(descriptor);
}

ArkOhosDrawingTextTypographyAdapterImpl::ArkOhosDrawingTextTypographyAdapterImpl(
    NWeb::OhosDrawingTextTypographyAdapter& ref) : real_(ref) {}

int ArkOhosDrawingTextTypographyAdapterImpl::GetSystemFontConfigInfo(void* fontConfigInfoErrorCode,
                                                                     void** fontConfigInfo)
{
    return real_.GetSystemFontConfigInfo(fontConfigInfoErrorCode, fontConfigInfo);
}

int ArkOhosDrawingTextTypographyAdapterImpl::GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray)
{
    return real_.GetDrawingArraySize(drawingArray, sizeOfArray);
}

void ArkOhosDrawingTextTypographyAdapterImpl::DestroySystemFontConfigInfo(void* fontConfigInfo)
{
    real_.DestroySystemFontConfigInfo(fontConfigInfo);
}
} // namespace OHOS::ArkWeb