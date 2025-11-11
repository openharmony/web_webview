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

#include "ohos_nweb/bridge/ark_web_hap_value_wrapper.h"

#include "ohos_nweb/ctocpp/ark_web_hap_value_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebHapValueType = OHOS::NWeb::NWebHapValue::Type;

ArkWebHapValueWrapper::ArkWebHapValueWrapper(ArkWebRefPtr<ArkWebHapValue> ark_web_hap_value)
    : ark_web_hap_value_(ark_web_hap_value)
{}

ArkWebHapValueType ArkWebHapValueWrapper::GetType()
{
    return static_cast<ArkWebHapValueType>(ark_web_hap_value_->GetType());
}

void ArkWebHapValueWrapper::SetType(ArkWebHapValueType type)
{
    ark_web_hap_value_->SetType(static_cast<unsigned char>(type));
}

int ArkWebHapValueWrapper::GetInt()
{
    return ark_web_hap_value_->GetInt();
}

void ArkWebHapValueWrapper::SetInt(int value)
{
    ark_web_hap_value_->SetInt(value);
}

bool ArkWebHapValueWrapper::GetBool()
{
    return ark_web_hap_value_->GetBool();
}

void ArkWebHapValueWrapper::SetBool(bool value)
{
    ark_web_hap_value_->SetBool(value);
}

double ArkWebHapValueWrapper::GetDouble()
{
    return ark_web_hap_value_->GetDouble();
}

void ArkWebHapValueWrapper::SetDouble(double value)
{
    ark_web_hap_value_->SetDouble(value);
}

std::string ArkWebHapValueWrapper::GetString()
{
    auto stValue = ark_web_hap_value_->GetString();
    auto value = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return value;
}

void ArkWebHapValueWrapper::SetString(const std::string& value)
{
    auto stValue = ArkWebStringClassToStruct(value);
    ark_web_hap_value_->SetString(stValue);
    ArkWebStringStructRelease(stValue);
}

const char* ArkWebHapValueWrapper::GetBinary(int& length)
{
    return ark_web_hap_value_->GetBinary(length);
}

void ArkWebHapValueWrapper::SetBinary(int length, const char* value)
{
    ark_web_hap_value_->SetBinary(length, value);
}

std::map<std::string, std::shared_ptr<OHOS::NWeb::NWebHapValue>> ArkWebHapValueWrapper::GetDictValue()
{
    auto stValue = ark_web_hap_value_->GetDictValue();
    auto value = ArkWebHapValueMapStructToClass(stValue);
    ArkWebHapValueMapStructRelease(stValue);
    return value;
}

std::vector<std::shared_ptr<OHOS::NWeb::NWebHapValue>> ArkWebHapValueWrapper::GetListValue()
{
    auto stValue = ark_web_hap_value_->GetListValue();
    auto value = ArkWebHapValueVectorStructToClass(stValue);
    ArkWebHapValueVectorStructRelease(stValue);
    return value;
}

std::shared_ptr<OHOS::NWeb::NWebHapValue> ArkWebHapValueWrapper::NewChildValue()
{
    ArkWebRefPtr<ArkWebHapValue> ark_web_hap_value = ark_web_hap_value_->NewChildValue();
    if (CHECK_REF_PTR_IS_NULL(ark_web_hap_value)) {
        return nullptr;
    }

    return std::make_shared<ArkWebHapValueWrapper>(ark_web_hap_value);
}

void ArkWebHapValueWrapper::SaveDictChildValue(const std::string& key)
{
    auto stKey = ArkWebStringClassToStruct(key);
    ark_web_hap_value_->SaveDictChildValue(stKey);
    ArkWebStringStructRelease(stKey);
}

void ArkWebHapValueWrapper::SaveListChildValue()
{
    ark_web_hap_value_->SaveListChildValue();
}

int64_t ArkWebHapValueWrapper::GetInt64()
{
    return ark_web_hap_value_->GetInt64();
}

void ArkWebHapValueWrapper::SetInt64(int64_t value)
{
    ark_web_hap_value_->SetInt64(value);
}

std::vector<uint8_t> ArkWebHapValueWrapper::GetBinary()
{
    auto stValue = ark_web_hap_value_->GetBinary();
    auto value = ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stValue);
    return value;
}

void ArkWebHapValueWrapper::SetBinary(const std::vector<uint8_t>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(value);
    ark_web_hap_value_->SetBinary(stValue);
    ArkWebBasicVectorStructRelease<ArkWebUint8Vector>(stValue);
}

std::vector<bool> ArkWebHapValueWrapper::GetBoolArray()
{
    auto stValue = ark_web_hap_value_->GetBoolArray();
    auto value = ArkWebBasicVectorStructToClass<bool, ArkWebBooleanVector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebBooleanVector>(stValue);
    return value;
}

void ArkWebHapValueWrapper::SetBoolArray(const std::vector<bool>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<bool, ArkWebBooleanVector>(value);
    ark_web_hap_value_->SetBoolArray(stValue);
    ArkWebBasicVectorStructRelease<ArkWebBooleanVector>(stValue);
}

std::vector<int64_t> ArkWebHapValueWrapper::GetInt64Array()
{
    auto stValue = ark_web_hap_value_->GetInt64Array();
    auto value = ArkWebBasicVectorStructToClass<int64_t, ArkWebInt64Vector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebInt64Vector>(stValue);
    return value;
}

void ArkWebHapValueWrapper::SetInt64Array(const std::vector<int64_t>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<int64_t, ArkWebInt64Vector>(value);
    ark_web_hap_value_->SetInt64Array(stValue);
    ArkWebBasicVectorStructRelease<ArkWebInt64Vector>(stValue);
}

std::vector<double> ArkWebHapValueWrapper::GetDoubleArray()
{
    auto stValue = ark_web_hap_value_->GetDoubleArray();
    auto value = ArkWebBasicVectorStructToClass<double, ArkWebDoubleVector>(stValue);
    ArkWebBasicVectorStructRelease<ArkWebDoubleVector>(stValue);
    return value;
}

void ArkWebHapValueWrapper::SetDoubleArray(const std::vector<double>& value)
{
    auto stValue = ArkWebBasicVectorClassToStruct<double, ArkWebDoubleVector>(value);
    ark_web_hap_value_->SetDoubleArray(stValue);
    ArkWebBasicVectorStructRelease<ArkWebDoubleVector>(stValue);
}

std::vector<std::string> ArkWebHapValueWrapper::GetStringArray()
{
    auto stValue = ark_web_hap_value_->GetStringArray();
    auto value = ArkWebStringVectorStructToClass(stValue);
    ArkWebStringVectorStructRelease(stValue);
    return value;
}

void ArkWebHapValueWrapper::SetStringArray(const std::vector<std::string>& value)
{
    auto stValue = ArkWebStringVectorClassToStruct(value);
    ark_web_hap_value_->SetStringArray(stValue);
    ArkWebStringVectorStructRelease(stValue);
}

std::string ArkWebHapValueWrapper::GetErrMsg()
{
    auto stMsg = ark_web_hap_value_->GetErrMsg();
    auto msg = ArkWebStringStructToClass(stMsg);
    ArkWebStringStructRelease(stMsg);
    return msg;
}

void ArkWebHapValueWrapper::SetErrMsg(const std::string& msg)
{
    auto stMsg = ArkWebStringClassToStruct(msg);
    ark_web_hap_value_->SetErrMsg(stMsg);
    ArkWebStringStructRelease(stMsg);
}

std::string ArkWebHapValueWrapper::GetErrName()
{
    auto stName = ark_web_hap_value_->GetErrName();
    auto name = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return name;
}

void ArkWebHapValueWrapper::SetErrName(const std::string& name)
{
    auto stName = ArkWebStringClassToStruct(name);
    ark_web_hap_value_->SetErrName(stName);
    ArkWebStringStructRelease(stName);
}

std::string ArkWebHapValueWrapper::GetErrorDescription()
{
    auto stDescription = ark_web_hap_value_->GetErrorDescription();
    auto description = ArkWebStringStructToClass(stDescription);
    ArkWebStringStructRelease(stDescription);
    return description;
}

void ArkWebHapValueWrapper::SetErrorDescription(const std::string& description)
{
    auto stDescription = ArkWebStringClassToStruct(description);
    ark_web_hap_value_->SetErrorDescription(stDescription);
    ArkWebStringStructRelease(stDescription);
}

} // namespace OHOS::ArkWeb
