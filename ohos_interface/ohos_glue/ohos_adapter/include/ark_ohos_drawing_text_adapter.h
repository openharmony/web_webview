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

#ifndef ARK_OHOS_DRAWING_TEXT_ADAPTER_H
#define ARK_OHOS_DRAWING_TEXT_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkOhosDrawingTextFontAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkOhosDrawingTextFontAdapter() = default;

    virtual ~ArkOhosDrawingTextFontAdapter() = default;

    /*--ark web()--*/
    virtual int GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray) = 0;

    /*--ark web()--*/
    virtual int GetFontDescriptorByFullName(void* drawingString,
                                            int32_t systemFontType, void** drawingFontDescriptor) = 0;
    
    /*--ark web()--*/
    virtual int GetSystemFontFullNameByIndex(void* drawingArray,
                                             int32_t indexOfFullName, const void** drawingString) = 0;
    
    /*--ark web()--*/
    virtual void DestroySystemFontFullNames(void* drawingArray) = 0;

    /*--ark web()--*/
    virtual void DestroyFontDescriptor(void* descriptor) = 0;
};

/*--ark web(source=webview)--*/
class ArkOhosDrawingTextTypographyAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkOhosDrawingTextTypographyAdapter() = default;

    virtual ~ArkOhosDrawingTextTypographyAdapter() = default;

    /*--ark web()--*/
    virtual int GetSystemFontConfigInfo(void* fontConfigInfoErrorCode, void** fontConfigInfo) = 0;

    /*--ark web()--*/
    virtual int GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray) = 0;

    /*--ark web()--*/
    virtual void DestroySystemFontConfigInfo(void* fontConfigInfo) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_DRAWING_TEXT_ADAPTER_H