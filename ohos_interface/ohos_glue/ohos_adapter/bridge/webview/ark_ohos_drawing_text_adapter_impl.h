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

#ifndef ARK_OHOS_DRAWING_TEXT_ADAPTER_IMPL_H
#define ARK_OHOS_DRAWING_TEXT_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_drawing_text_adapter.h"
#include "ohos_drawing_text_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosDrawingTextFontAdapterImpl : public ArkOhosDrawingTextFontAdapter {
public:
   ArkOhosDrawingTextFontAdapterImpl(NWeb::OhosDrawingTextFontAdapter&);

   int GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray) override;

   int GetFontDescriptorByFullName(void* drawingString, int32_t systemFontType, void** drawingFontDescriptor) override;

   int GetSystemFontFullNameByIndex(void* drawingArray, int32_t indexOfFullName, const void** drawingString) override;

   void DestroySystemFontFullNames(void* drawingArray) override;

   void DestroyFontDescriptor(void* descriptor) override;

private:
   OHOS::NWeb::OhosDrawingTextFontAdapter& real_;

   IMPLEMENT_REFCOUNTING(ArkOhosDrawingTextFontAdapterImpl);
};

class ArkOhosDrawingTextTypographyAdapterImpl : public ArkOhosDrawingTextTypographyAdapter {
public:
    ArkOhosDrawingTextTypographyAdapterImpl(NWeb::OhosDrawingTextTypographyAdapter&);

    int GetSystemFontConfigInfo(void* fontConfigInfoErrorCode, void** fontConfigInfo) override;

    int GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray) override;

    void DestroySystemFontConfigInfo(void* fontConfigInfo) override;

private:
    OHOS::NWeb::OhosDrawingTextTypographyAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkOhosDrawingTextTypographyAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_DRAWING_TEXT_ADAPTER_IMPL_H