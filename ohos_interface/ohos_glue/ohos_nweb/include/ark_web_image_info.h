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

#ifndef ARK_WEB_IMAGE_INFO_H_
#define ARK_WEB_IMAGE_INFO_H_
#pragma once
 
#include "base/include/ark_web_base_ref_counted.h"
 
namespace OHOS::ArkWeb {
 
/*--ark web(source=webcore)--*/
class ArkWebImageInfo : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;
 
    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;
 
    /*--ark web()--*/
    virtual int32_t GetAlphaType() = 0;
 
    /*--ark web()--*/
    virtual int32_t GetColorType() = 0;
 
    /*--ark web()--*/
    virtual uint32_t *GetData() = 0;
 
    /*--ark web()--*/
    virtual uint32_t GetDataSize() = 0;
 
    /*--ark web()--*/
    virtual void SetWidth(int32_t width) = 0;
 
    /*--ark web()--*/
    virtual void SetHeight(int32_t height) = 0;
 
    /*--ark web()--*/
    virtual void SetAlphaType(int32_t alphaType) = 0;
 
    /*--ark web()--*/
    virtual void SetColorType(int32_t colorType) = 0;
 
    /*--ark web()--*/
    virtual void SetData(uint32_t *data) = 0;
 
    /*--ark web()--*/
    virtual void SetDataSize(uint32_t dataSize) = 0;
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_WEB_IMAGE_INFO_H_
