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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_HAP_VALUE_WRAPPER_H_
#define ARK_WEB_HAP_VALUE_WRAPPER_H_
#pragma once

#include "include/nweb_hap_value.h"
#include "ohos_nweb/include/ark_web_hap_value.h"

namespace OHOS::ArkWeb {

using ArkWebHapValueType = OHOS::NWeb::NWebHapValue::Type;

class ArkWebHapValueWrapper : public OHOS::NWeb::NWebHapValue {
public:
    ArkWebHapValueWrapper(ArkWebRefPtr<ArkWebHapValue> ark_web_hap_value);
    ~ArkWebHapValueWrapper() = default;

    ArkWebHapValueType GetType() override;

    void SetType(ArkWebHapValueType type) override;

    int GetInt() override;

    void SetInt(int value) override;

    bool GetBool() override;

    void SetBool(bool value) override;

    double GetDouble() override;

    void SetDouble(double value) override;

    std::string GetString() override;

    void SetString(const std::string& value) override;

    const char* GetBinary(int& length) override;

    void SetBinary(int length, const char* value) override;

    std::map<std::string, std::shared_ptr<OHOS::NWeb::NWebHapValue>> GetDictValue() override;

    std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>> GetListValue() override;

    std::shared_ptr<OHOS::NWeb::NWebHapValue> NewChildValue() override;

    void SaveDictChildValue(const std::string& key) override;

    void SaveListChildValue() override;

    int64_t GetInt64() override;

    void SetInt64(int64_t value) override;

    std::vector<uint8_t> GetBinary() override;

    void SetBinary(const std::vector<uint8_t>& value) override;

    std::vector<bool> GetBoolArray() override;

    void SetBoolArray(const std::vector<bool>& value) override;

    std::vector<int64_t> GetInt64Array() override;

    void SetInt64Array(const std::vector<int64_t>& value) override;

    std::vector<double> GetDoubleArray() override;

    void SetDoubleArray(const std::vector<double>& value) override;

    std::vector<std::string> GetStringArray() override;

    void SetStringArray(const std::vector<std::string>& value) override;

    std::string GetErrMsg() override;

    void SetErrMsg(const std::string& msg) override;

    std::string GetErrName() override;

    void SetErrName(const std::string& name) override;

    std::string GetErrorDescription() override;

    void SetErrorDescription(const std::string& description) override;

private:
    ArkWebRefPtr<ArkWebHapValue> ark_web_hap_value_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HAP_VALUE_WRAPPER_H_
