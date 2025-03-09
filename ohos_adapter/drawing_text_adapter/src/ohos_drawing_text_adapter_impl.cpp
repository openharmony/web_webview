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

#include "ohos_drawing_text_adapter_impl.h"

#include "drawing_text_font_descriptor.h"
#include "drawing_text_typography.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

OhosDrawingTextFontAdapter& OhosDrawingTextFontAdapterImpl::GetInstance()
{
    WVLOG_D("drawing text font adapter impl get instance.");
    static OhosDrawingTextFontAdapterImpl instance;
    return instance;
}

OhosDrawingTextFontAdapterImpl::~OhosDrawingTextFontAdapterImpl()
{
    WVLOG_D("Drawing text font adapter impl destructor.");
}

int OhosDrawingTextFontAdapterImpl::GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray)
{
    *drawingArray = OH_Drawing_GetSystemFontFullNamesByType(static_cast<OH_Drawing_SystemFontType>(systemFontType));
    if (*drawingArray == nullptr) {
        WVLOG_E("drawingArray GetSystemFontFullNamesByType, drawingArray is null.");
        return -1;
    }
    WVLOG_D("drawingArray GetSystemFontFullNamesByType %{public}d", systemFontType);
    return 0;
}

int OhosDrawingTextFontAdapterImpl::GetFontDescriptorByFullName(void* drawingString,
    int32_t systemFontType, void** drawingFontDescriptor)
{
    *drawingFontDescriptor = OH_Drawing_GetFontDescriptorByFullName(
        static_cast<OH_Drawing_String*>(drawingString),
        static_cast<OH_Drawing_SystemFontType>(systemFontType));
    if (*drawingFontDescriptor == nullptr) {
      WVLOG_E("drawingArray GetFontDescriptorByFullName, drawingArray is null.");
      return -1;
    }
    WVLOG_D("drawingArray GetFontDescriptorByFullName success!!!");
    return 0;
}

int OhosDrawingTextFontAdapterImpl::GetSystemFontFullNameByIndex(void* drawingArray,
    int32_t indexOfFullName, const void** drawingString)
{
    *drawingString = OH_Drawing_GetSystemFontFullNameByIndex(
        static_cast<OH_Drawing_Array*>(drawingArray),
        static_cast<size_t>(indexOfFullName));
    if (*drawingString == nullptr) {
        WVLOG_E("drawingArray GetSystemFontFullNameByIndex, drawingArray is null.");
        return -1;
    }
    WVLOG_D("drawingArray GetSystemFontFullNameByIndex sucess!!!");
    return 0;
}

void OhosDrawingTextFontAdapterImpl::DestroySystemFontFullNames(void* drawingArray)
{
    OH_Drawing_DestroySystemFontFullNames(static_cast<OH_Drawing_Array*>(drawingArray));
    WVLOG_D("DestroySystemFontFullNames");
}

void OhosDrawingTextFontAdapterImpl::DestroyFontDescriptor(void* descriptor)
{
    OH_Drawing_DestroyFontDescriptor(static_cast<OH_Drawing_FontDescriptor*>(descriptor));
    WVLOG_D("DestroySystemFontDescriptor");
}

OhosDrawingTextTypographyAdapter& OhosDrawingTextTypographyAdapterImpl::GetInstance()
{
    WVLOG_D("drawing text typography adapter impl get instance.");
    static OhosDrawingTextTypographyAdapterImpl instance;
    return instance;
}

OhosDrawingTextTypographyAdapterImpl::~OhosDrawingTextTypographyAdapterImpl()
{
    WVLOG_D("Drawing text typography adapter impl desctrutor");
}

int OhosDrawingTextTypographyAdapterImpl::GetSystemFontConfigInfo(void* fontConfigInfoErrorCode, void** fontConfigInfo)
{
    *fontConfigInfo =
      OH_Drawing_GetSystemFontConfigInfo(static_cast<OH_Drawing_FontConfigInfoErrorCode*>(fontConfigInfoErrorCode));
    if (*fontConfigInfo == nullptr) {
        WVLOG_E("drawingArray GetSystemFontConfigInfo, drawingArray is null.");
        return -1;
    }
    WVLOG_D("drawingArray GetSystemFontConfigInfo success!!!");
    return 0;
}

int OhosDrawingTextTypographyAdapterImpl::GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray)
{
    sizeOfArray = OH_Drawing_GetDrawingArraySize(static_cast<OH_Drawing_Array*>(drawingArray));
    if (sizeOfArray < 0) {
        WVLOG_E("drawingArray GetDrawingArraySize, drawingArray is null.");
        return -1;
    }
    WVLOG_D("drawingArray GetDrawingArraySize success!!!");
    return 0;
}

void OhosDrawingTextTypographyAdapterImpl::DestroySystemFontConfigInfo(void* fontConfigInfo)
{
    OH_Drawing_DestroySystemFontConfigInfo(static_cast<OH_Drawing_FontConfigInfo*>(fontConfigInfo));
    WVLOG_D("DestroySystemFontConfigInfo");
}
} //namespace OHOS::NWeb
 