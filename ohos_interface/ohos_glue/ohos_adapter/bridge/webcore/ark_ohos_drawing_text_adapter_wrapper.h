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

#ifndef ARK_OHOS_DRAWING_TEXT_ADAPTER_WRAPPER_H
#define ARK_OHOS_DRAWING_TEXT_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_ohos_drawing_text_adapter.h"
#include "ohos_drawing_text_adapter.h"

namespace OHOS::ArkWeb {
using namespace OHOS::NWeb;

class ArkOhosDrawingTextFontAdapterWrapper : public OHOS::NWeb::OhosDrawingTextFontAdapter {
public:
    ArkOhosDrawingTextFontAdapterWrapper(ArkWebRefPtr<ArkOhosDrawingTextFontAdapter>);

    int GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray) override;

    int GetFontDescriptorByFullName(void* drawingString, int32_t systemFontType, void** drawingFontDescriptor) override;

    int GetSystemFontFullNameByIndex(void* drawingArray, int32_t indexOfFullName, const void** drawingString) override;

    void DestroySystemFontFullNames(void* drawingArray) override;

    void DestroyFontDescriptor(void* descriptor) override;

private:
    ArkWebRefPtr<ArkOhosDrawingTextFontAdapter> ctocpp_;
};

class ArkOhosDrawingTextTypographyAdapterWrapper : public OHOS::NWeb::OhosDrawingTextTypographyAdapter {
public:
    ArkOhosDrawingTextTypographyAdapterWrapper(ArkWebRefPtr<ArkOhosDrawingTextTypographyAdapter>);

    int GetSystemFontConfigInfo(void* fontConfigInfoErrorCode, void** fontConfigInfo) override;

    int GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray) override;

    void DestroySystemFontConfigInfo(void* fontConfigInfo) override;

private:
    ArkWebRefPtr<ArkOhosDrawingTextTypographyAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_DRAWING_TEXT_ADAPTER_WRAPPER_H