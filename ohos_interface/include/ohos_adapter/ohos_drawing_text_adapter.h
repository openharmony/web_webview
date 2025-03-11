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

#ifndef OHOS_DRAWING_TEXT_ADAPTER_H
#define OHOS_DRAWING_TEXT_ADAPTER_H

#include <string>

namespace OHOS::NWeb {

typedef struct OH_Drawing_FontFallbackInfo {
    /**The type of language supported by the font set. The language format is bcp47 */
    char* language;
    /** FontFamily Name */
    char* familyName;
} ArkWeb_Drawing_FontFallbackInfo;

typedef struct OH_Drawing_FontAliasInfo {
    /** Font Family Name */
    char* familyName;
    /**
     * Font weight value. When the weight value is greater than 0.
     * the font set contains only fonts with the sepcified weight
     * When the weight values is equal to 0, the font set contain all fonts
     */
    int weight;
} ArkWeb_Drawing_FontAliasInfo;

typedef struct OH_Drawing_FontAdjustInfo {
    /**The font's original weight info */
    int weight;
    /** The font weight displayed in the application. */
    int to;
} ArkWeb_Drawing_FontAdjustInfo;

typedef struct OH_Drawing_FontGenericInfo {
    /** Font family name */
    char* familyName;
    /** The size of alias font lists */
    size_t aliasInfoSize;
    /** The Size of font weight mapping information lists */
    size_t adjustInfoSize;
    /** List of alias fonts */
    ArkWeb_Drawing_FontAliasInfo* aliasInfoSet;
    /** Font weight mapping information lists */
    ArkWeb_Drawing_FontAdjustInfo* adjustInfoSet;
} ArkWeb_Drawing_FontGenericInfo;

typedef struct OH_Drawing_FontFallbackGroup {
    /**
     * The name of the font set corresponding to the fallback font set. If the value is null,
     * all fonts can be set using the fallback font set list.
     */
    char* groupName;
    /** Fallback font Info Size */
    size_t fallbackInfoSize;
    /** A list of font sets for fallback fonts */
    ArkWeb_Drawing_FontFallbackInfo* fallbackInfoSet;
} ArkWeb_Drawing_FontFallbackGroup;

typedef struct ArkWeb_Drawing_FontConfigInfo {
    /** Count of system font file paths*/
    size_t fontDirSize;
    /** List size of generic font sets */
    size_t fontGenericInfoSize;
    /**Count of fallback font set lists */
    size_t fallbackGroupSize;
    /** List of system font file paths */
    char** fontDirSet;
    /** List of generic font sets */
    ArkWeb_Drawing_FontGenericInfo* fontGenericInfoSet;
    /**List of fallback font sets */
    ArkWeb_Drawing_FontFallbackGroup* fallbackGroupSet;
} ArkWeb_Drawing_FontConfigInfo;

typedef enum {
    /** All font types */
    ALL = 1 << 0,
    /** System generic font type */
    GENERIC = 1 << 1,
    /**Stylist font type */
    STYLISH = 1 << 2,
    /**Installed font types */
    INSTALLED = 1 << 3,
} ArkWeb_Drawing_SystemFontType;

typedef struct {
    /** A pointer to a byte string containing UTF-16 encoded entities */
    uint8_t* strData;
    /** The length of 'strData' in bytes */
    uint32_t strLen;
} ArkWeb_Drawing_String;

typedef struct ArkWeb_Drawing_FontDescriptor {
    /**The file path of System font */
    char* path;
    /** A name that uniquely identifies the font */
    char* postScriptName;
    /** The name of system font */
    char* fullName;
    /** The family of System font */
    char* fontFamily;
    /** The subfont family of the system font */
    char* fontSubfamily;
    /** The weight of system font */
    int weight;
    /** The width of system font */
    int width;
    /** whether the system font is titled */
    int italic;
    /** Whether the system font is compat */
    bool monoSpace;
    /** whether symbolic fonts are suppoted */
    bool symbolic;
} ArkWeb_Drawing_FontDescriptor;

class OhosDrawingTextFontAdapter {
public:
    OhosDrawingTextFontAdapter() = default;

    virtual ~OhosDrawingTextFontAdapter() = default;

    virtual int GetSystemFontFullNamesByType(int32_t systemFontType, void** drawingArray) = 0;

    virtual int GetFontDescriptorByFullName(void* drawingString,
                                            int32_t systemFontType, void** drawingFontDescriptor) = 0;
    
    virtual int GetSystemFontFullNameByIndex(void* drawingArray,
                                             int32_t indexOfFullName, const void** drawingString) = 0;

    virtual void DestroySystemFontFullNames(void* drawingArray) = 0;

    virtual void DestroyFontDescriptor(void* descriptor) = 0;
};

class OhosDrawingTextTypographyAdapter {
public:
    OhosDrawingTextTypographyAdapter() = default;

    virtual ~OhosDrawingTextTypographyAdapter() = default;

    virtual int GetSystemFontConfigInfo(void* fontConfigInfoErrorCode, void** fontConfigInfo) = 0;

    virtual int GetDrawingArraySize(void* drawingArray, int32_t& sizeOfArray) = 0;

    virtual void DestroySystemFontConfigInfo(void* fontConfigInfo) = 0;
};
} // namespace OHOS::NWeb

#endif // OHOS_DRAWING_TEXT_ADAPTER_H