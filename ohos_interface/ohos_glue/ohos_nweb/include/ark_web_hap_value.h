/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_HAP_VALUE_H_
#define ARK_WEB_HAP_VALUE_H_
#pragma once

#include "ohos_nweb/include/ark_web_hap_value_vector.h"

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebHapValue : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual unsigned char GetType() = 0;

    /*--ark web()--*/
    virtual void SetType(unsigned char type) = 0;

    /*--ark web()--*/
    virtual int GetInt() = 0;

    /*--ark web()--*/
    virtual void SetInt(int value) = 0;

    /*--ark web()--*/
    virtual bool GetBool() = 0;

    /*--ark web()--*/
    virtual void SetBool(bool value) = 0;

    /*--ark web()--*/
    virtual double GetDouble() = 0;

    /*--ark web()--*/
    virtual void SetDouble(double value) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetString() = 0;

    /*--ark web()--*/
    virtual void SetString(const ArkWebString& value) = 0;

    /*--ark web()--*/
    virtual const char* GetBinary(int& length) = 0;

    /*--ark web()--*/
    virtual void SetBinary(int length, const char* value) = 0;

    /*--ark web()--*/
    virtual ArkWebHapValueMap GetDictValue() = 0;

    /*--ark web()--*/
    virtual ArkWebHapValueVector GetListValue() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebHapValue> NewChildValue() = 0;

    /*--ark web()--*/
    virtual void SaveDictChildValue(const ArkWebString& key) = 0;

    /*--ark web()--*/
    virtual void SaveListChildValue() = 0;

    /*--ark web()--*/
    virtual int64_t GetInt64() = 0;

    /*--ark web()--*/
    virtual void SetInt64(int64_t value) = 0;

    /*--ark web()--*/
    virtual ArkWebUint8Vector GetBinary() = 0;

    /*--ark web()--*/
    virtual void SetBinary(const ArkWebUint8Vector& value) = 0;

    /*--ark web()--*/
    virtual ArkWebBooleanVector GetBoolArray() = 0;

    /*--ark web()--*/
    virtual void SetBoolArray(const ArkWebBooleanVector& value) = 0;

    /*--ark web()--*/
    virtual ArkWebInt64Vector GetInt64Array() = 0;

    /*--ark web()--*/
    virtual void SetInt64Array(const ArkWebInt64Vector& value) = 0;

    /*--ark web()--*/
    virtual ArkWebDoubleVector GetDoubleArray() = 0;

    /*--ark web()--*/
    virtual void SetDoubleArray(const ArkWebDoubleVector& value) = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetStringArray() = 0;

    /*--ark web()--*/
    virtual void SetStringArray(const ArkWebStringVector& value) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetErrMsg() = 0;

    /*--ark web()--*/
    virtual void SetErrMsg(const ArkWebString& msg) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetErrName() = 0;

    /*--ark web()--*/
    virtual void SetErrName(const ArkWebString& name) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetErrorDescription() = 0;

    /*--ark web()--*/
    virtual void SetErrorDescription(const ArkWebString& description) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HAP_VALUE_H_
