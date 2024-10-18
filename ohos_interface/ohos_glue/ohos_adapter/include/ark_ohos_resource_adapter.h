/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_OHOS_RESOURCE_ADAPTER_H
#define ARK_OHOS_RESOURCE_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkOhosFileMapper : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetFd() = 0;

    /*--ark web()--*/
    virtual int32_t GetOffset() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetFileName() = 0;

    /*--ark web()--*/
    virtual bool IsCompressed() = 0;

    /*--ark web()--*/
    virtual void* GetDataPtr() = 0;

    /*--ark web()--*/
    virtual size_t GetDataLen() = 0;

    /*--ark web()--*/
    virtual bool UnzipData(uint8_t** dest, size_t& len) = 0;
};

/*--ark web(source=webview)--*/
class ArkOhosResourceAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool GetRawFileData(const ArkWebString& rawFile, size_t& len, uint8_t** dest, bool isSys) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkOhosFileMapper> GetRawFileMapper(const ArkWebString& rawFile, bool isSys) = 0;

    /*--ark web()--*/
    virtual bool IsRawFileExist(const ArkWebString& rawFile, bool isSys) = 0;

    /*--ark web()--*/
    virtual bool GetRawFileLastModTime(const ArkWebString& rawFile, uint16_t& date, uint16_t& time, bool isSys) = 0;

    /*--ark web()--*/
    virtual bool GetRawFileLastModTime(const ArkWebString& rawFile, time_t& time, bool isSys) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetSystemLanguage() = 0;
};

} // namespace OHOS::ArkWeb

#endif // Ark_OHOS_RESOURCE_ADAPTER_H
