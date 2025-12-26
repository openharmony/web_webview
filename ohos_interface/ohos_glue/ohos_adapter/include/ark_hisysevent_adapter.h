/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_HISYSEVENT_ADAPTER_H
#define ARK_HISYSEVENT_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkHiSysEventAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int Write(
        const ArkWebString& eventName, uint32_t type, const ArkWebString key0, const ArkWebString value0) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3,
        const ArkWebString key4, const ArkWebString value4) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3,
        const ArkWebString key4, const ArkWebString value4, const ArkWebString key5, const ArkWebString value5) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3,
        const ArkWebString key4, const ArkWebString value4, const ArkWebString key5, const ArkWebString value5,
        const ArkWebString key6, const ArkWebString value6) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const int64_t value1,
        const ArkWebString key2, const int value2, const ArkWebString key3, const ArkWebUint16Vector value3,
        const ArkWebString key4, const int value4) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const int64_t value1,
        const ArkWebString key2, const int64_t value2, const ArkWebString key3, const int value3,
        const ArkWebString key4, const int value4, const ArkWebString key5, const int64_t value5,
        const ArkWebString key6, const int value6) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const int64_t value1,
        const ArkWebString key2, const int64_t value2, const ArkWebString key3, const uint32_t value3,
        const ArkWebString key4, const int64_t value4, const ArkWebString key5, const int64_t value5,
        const ArkWebString key6, const int64_t value6, const ArkWebString key7, const int64_t value7,
        const ArkWebString key8, const int64_t value8, const ArkWebString key9, const int64_t value9,
        const ArkWebString key10, const int64_t value10, const ArkWebString key11, const int64_t value11,
        const ArkWebString key12, const int64_t value12, const ArkWebString key13, const int64_t value13,
        const ArkWebString key14, const int64_t value14, const ArkWebString key15, const int64_t value15,
        const ArkWebString key16, const int64_t value16, const ArkWebString key17, const int64_t value17,
        const ArkWebString key18, const int64_t value18, const ArkWebString key19, const int64_t value19,
        const ArkWebString key20, const int64_t value20, const ArkWebString key21, const int64_t value21,
        const ArkWebString key22, const int64_t value22, const ArkWebString key23, const int64_t value23) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3,
        const ArkWebString key4, const ArkWebString value4, const ArkWebString key5, const ArkWebString value5,
        const ArkWebString key6, const ArkWebString value6, const ArkWebString key7, const ArkWebString value7) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const ArkWebString value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3,
        const ArkWebString key4, const ArkWebString value4, const ArkWebString key5, const ArkWebString value5,
        const ArkWebString key6, const ArkWebString value6, const ArkWebString key7, const ArkWebString value7,
        const ArkWebString key8, const ArkWebString value8) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const uint32_t value1,
        const ArkWebString key2, const uint64_t value2) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1, const int32_t value1,
        const ArkWebString key2, const ArkWebString value2, const ArkWebString key3, const ArkWebString value3,
        const ArkWebString key4, const ArkWebString value4, const ArkWebString key5, const int32_t value5) = 0;

    /*--ark web()--*/
    virtual int Write(const ArkWebString& eventName, uint32_t type, const ArkWebString key1,
        const int32_t value1) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_HISYSEVENT_ADAPTER_H
