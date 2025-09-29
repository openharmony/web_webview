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

#ifndef ARK_WEB_HAP_VALUE_IMPL_H_
#define ARK_WEB_HAP_VALUE_IMPL_H_
#pragma once

#include "include/nweb_hap_value.h"
#include "ohos_nweb/include/ark_web_hap_value.h"

namespace OHOS::ArkWeb {

class ArkWebHapValueImpl : public ArkWebHapValue {
    IMPLEMENT_REFCOUNTING(ArkWebHapValueImpl);

public:
    ArkWebHapValueImpl(std::shared_ptr<OHOS::NWeb::NWebHapValue> nweb_hap_value);
    ~ArkWebHapValueImpl() = default;

    unsigned char GetType() override;

    void SetType(unsigned char type) override;

    int GetInt() override;

    void SetInt(int value) override;

    bool GetBool() override;

    void SetBool(bool value) override;

    double GetDouble() override;

    void SetDouble(double value) override;

    ArkWebString GetString() override;

    void SetString(const ArkWebString& value) override;

    const char* GetBinary(int& length) override;

    void SetBinary(int length, const char* value) override;

    ArkWebHapValueMap GetDictValue() override;

    ArkWebHapValueVector GetListValue() override;

    ArkWebRefPtr<ArkWebHapValue> NewChildValue() override;

    void SaveDictChildValue(const ArkWebString& key) override;

    void SaveListChildValue() override;

    int64_t GetInt64() override;

    void SetInt64(int64_t value) override;

    ArkWebUint8Vector GetBinary() override;

    void SetBinary(const ArkWebUint8Vector& value) override;

    ArkWebBooleanVector GetBoolArray() override;

    void SetBoolArray(const ArkWebBooleanVector& value) override;

    ArkWebInt64Vector GetInt64Array() override;

    void SetInt64Array(const ArkWebInt64Vector& value) override;

    ArkWebDoubleVector GetDoubleArray() override;

    void SetDoubleArray(const ArkWebDoubleVector& value) override;

    ArkWebStringVector GetStringArray() override;

    void SetStringArray(const ArkWebStringVector& value) override;

    ArkWebString GetErrMsg() override;

    void SetErrMsg(const ArkWebString& msg) override;

    ArkWebString GetErrName() override;

    void SetErrName(const ArkWebString& name) override;

    ArkWebString GetErrorDescription() override;

    void SetErrorDescription(const ArkWebString& description) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebHapValue> nweb_hap_value_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HAP_VALUE_IMPL_H_
