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

#ifndef OHOS_DRAWING_TEXT_ADAPTER_IMPL_H
#define OHOS_DRAWING_TEXT_ADAPTER_IMPL_H

#include "ohos_drawing_text_adapter.h"

namespace OHOS::NWeb {
class OhosDrawingTextFontAdapterImpl : public OhosDrawingTextFontAdapter {
public:
   static OhosDrawingTextFontAdapter& GetInstance();
   /**
    * @brief Construct a new OhosDrawingTextFontAdapterImpl object.
    */
   OhosDrawingTextFontAdapterImpl() = default;

   /**
    * @brief Destory the OhosDrawingTextFontAdapterImpl object
    */
   ~OhosDrawingTextFontAdapterImpl() override;

   /**
    * @brief Get OH_Drawing_Array by OH_Drawing_GetSystemFontFullNamesByType
    * 
    * @param systemFontType Indicates enumberates of system font type object <b>OH_Drawing_SystemFontType</b>
    * @param drawingArray the pointer to full name array object <b>OH_Drawing_Array</b>
    * @return ErrCode, -1 mean fail and other mean success
    */
    int GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray) override;

    /**
     * @brief Get OH_Drawing_FontDescriptor by OH_Drawing_GetFontDescriptorByFullName
     * 
     * @param drawingString Indicates the full name object <b>OH_Drawing_String</b>
     * @param systemFontType Indicates enumerates of system font type object <b>OH_Drawing_SystemFontType</b>
     * @param drawingFontDescriptor the pointer to a font descriptor object <b>OH_Drawing_FontDescriptor</b>
     * @return ErrCode, -1 mean fail and the other mean sucess
     */
    int GetFontDescriptorByFullName(void* drawingString, int32_t systemFontType, void** drawingFontDescriptor) override;

    /**
     * @brief Get OH_Drawing_String by OH_Drawing_GetSystemFontFullNameByIndex
     * 
     * @param drawingArray Indicates an array of full name object <b>OH_Drawing_Array</b>
     * @param indexOfFullName The index of full name
     * @param drawingString The full name object <b>OH_Drawing_String</b>
     * @return ErrCode, -1 mean fail and other mean success
     */
    int GetSystemFontFullNameByIndex(void* drawingArray, int32_t indexOfFullName, const void** drawingString) override;

    /**
     * @brief Releases the memory occupied by an array of font full names
     * 
     * @param drawingArray Indicates an array of full name object <b>OH_Drawing_Array</b>
     */
    void DestroySystemFontFullNames(void* drawingArray) override;

    /**
     * @brief Releases the memory occupied by descriptor
     * 
     * @param descriptor Indicates the pointer to an <b>OH_Drawing_FontDescriptor</b> object
     */
    void DestroyFontDescriptor(void* descriptor) override;
};

class OhosDrawingTextTypographyAdapterImpl : public OhosDrawingTextTypographyAdapter {
public:
    static OhosDrawingTextTypographyAdapter& GetInstance();
    /**
     * @brief Construct a new OhosDrawingTextFontAdapterImpl object.
     */
    OhosDrawingTextTypographyAdapterImpl() = default;

    /**
     * @brief Destory the OhosDrawingTextTypographyAdapterImpl object.
     */
    ~OhosDrawingTextTypographyAdapterImpl() override;

    /**
     * @brief get OH_Drawing_FontConfigInfo by OH_Drawing_GetSystemFontConfigInfo.
     * 
     * @param fontConfigInfoErrorCode Indicates error code returned, based on the error code to
     * release the memory of system font configuration information.
     * @param fontConfigInfo The pointer to system font configuration information.
     * Indicates the pointer to an <b>OH_Drawing_FontConfigInfo<b> object.
     * @return ErrCode, -1 mean fail and the other mean success
     */
    int GetSystemFontConfigInfo(void* fontConfigInfoErrorCode, void** fontConfigInfo) override;

    /**
     * @brief get size of Array by OH_Drawing_GetDrawingArraySize
     * 
     * @param drawingArray Indicates the pointer to the array object <b>OH_Drawing_Array</b>
     * @param sizeOfArray Size of array., the result of OH_Drawing_GetDrawingArraySize.
     * @return ErrCode, -1 mean fail and other mean success
     */
    int GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray) override;

    /**
     * @brief Releases the memory occupied by system font configuration information.
     * 
     * @param fontConfigInfo Indicates the pointer to an <b>OH_Drawing_FontConfigInfo</b> object.
     */
    void DestroySystemFontConfigInfo(void* fontConfigInfo) override;
};
} // namespace OHOS::NWeb

#endif //OHOS_DRAWING_TEXT_FONT_ADAPTER_IMPL_H