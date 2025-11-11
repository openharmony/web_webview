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

#include "ohos_nweb/bridge/ark_web_hap_value_impl.h"

#include "ohos_nweb/cpptoc/ark_web_hap_value_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

using ArkWebHapValueType = OHOS::NWeb::NWebHapValue::Type;

ArkWebHapValueImpl::ArkWebHapValueImpl(std::shared_ptr<OHOS::NWeb::NWebHapValue> nweb_hap_value)
    : nweb_hap_value_(nweb_hap_value)
{}

unsigned char ArkWebHapValueImpl::GetType()
{
    return static_cast<unsigned char>(nweb_hap_value_->GetType());
}

void ArkWebHapValueImpl::SetType(unsigned char type)
{
    nweb_hap_value_->SetType(static_cast<ArkWebHapValueType>(type));
}

int ArkWebHapValueImpl::GetInt()
{
    return nweb_hap_value_->GetInt();
}

void ArkWebHapValueImpl::SetInt(int value)
{
    nweb_hap_value_->SetInt(value);
}

bool ArkWebHapValueImpl::GetBool()
{
    return nweb_hap_value_->GetBool();
}

void ArkWebHapValueImpl::SetBool(bool value)
{
    nweb_hap_value_->SetBool(value);
}

double ArkWebHapValueImpl::GetDouble()
{
    return nweb_hap_value_->GetDouble();
}

void ArkWebHapValueImpl::SetDouble(double value)
{
    nweb_hap_value_->SetDouble(value);
}

ArkWebString ArkWebHapValueImpl::GetString()
{
    return ArkWebStringClassToStruct(nweb_hap_value_->GetString());
}

void ArkWebHapValueImpl::SetString(const ArkWebString& value)
{
    nweb_hap_value_->SetString(ArkWebStringStructToClass(value));
}

const char* ArkWebHapValueImpl::GetBinary(int& length)
{
    return nweb_hap_value_->GetBinary(length);
}

void ArkWebHapValueImpl::SetBinary(int length, const char* value)
{
    nweb_hap_value_->SetBinary(length, value);
}

ArkWebHapValueMap ArkWebHapValueImpl::GetDictValue()
{
    return ArkWebHapValueMapClassToStruct(nweb_hap_value_->GetDictValue());
}

ArkWebHapValueVector ArkWebHapValueImpl::GetListValue()
{
    return ArkWebHapValueVectorClassToStruct(nweb_hap_value_->GetListValue());
}

ArkWebRefPtr<ArkWebHapValue> ArkWebHapValueImpl::NewChildValue()
{
    std::shared_ptr<OHOS::NWeb::NWebHapValue> nweb_hap_value = nweb_hap_value_->NewChildValue();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_hap_value)) {
        return nullptr;
    }

    return new ArkWebHapValueImpl(nweb_hap_value);
}

void ArkWebHapValueImpl::SaveDictChildValue(const ArkWebString& key)
{
    nweb_hap_value_->SaveDictChildValue(ArkWebStringStructToClass(key));
}

void ArkWebHapValueImpl::SaveListChildValue()
{
    nweb_hap_value_->SaveListChildValue();
}

int64_t ArkWebHapValueImpl::GetInt64()
{
    return nweb_hap_value_->GetInt64();
}

void ArkWebHapValueImpl::SetInt64(int64_t value)
{
    nweb_hap_value_->SetInt64(value);
}

ArkWebUint8Vector ArkWebHapValueImpl::GetBinary()
{
    return ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(nweb_hap_value_->GetBinary());
}

void ArkWebHapValueImpl::SetBinary(const ArkWebUint8Vector& value)
{
    nweb_hap_value_->SetBinary(ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(value));
}

ArkWebBooleanVector ArkWebHapValueImpl::GetBoolArray()
{
    return ArkWebBasicVectorClassToStruct<bool, ArkWebBooleanVector>(nweb_hap_value_->GetBoolArray());
}

void ArkWebHapValueImpl::SetBoolArray(const ArkWebBooleanVector& value)
{
    nweb_hap_value_->SetBoolArray(ArkWebBasicVectorStructToClass<bool, ArkWebBooleanVector>(value));
}

ArkWebInt64Vector ArkWebHapValueImpl::GetInt64Array()
{
    return ArkWebBasicVectorClassToStruct<int64_t, ArkWebInt64Vector>(nweb_hap_value_->GetInt64Array());
}

void ArkWebHapValueImpl::SetInt64Array(const ArkWebInt64Vector& value)
{
    nweb_hap_value_->SetInt64Array(ArkWebBasicVectorStructToClass<int64_t, ArkWebInt64Vector>(value));
}

ArkWebDoubleVector ArkWebHapValueImpl::GetDoubleArray()
{
    return ArkWebBasicVectorClassToStruct<double, ArkWebDoubleVector>(nweb_hap_value_->GetDoubleArray());
}

void ArkWebHapValueImpl::SetDoubleArray(const ArkWebDoubleVector& value)
{
    nweb_hap_value_->SetDoubleArray(ArkWebBasicVectorStructToClass<double, ArkWebDoubleVector>(value));
}

ArkWebStringVector ArkWebHapValueImpl::GetStringArray()
{
    return ArkWebStringVectorClassToStruct(nweb_hap_value_->GetStringArray());
}

void ArkWebHapValueImpl::SetStringArray(const ArkWebStringVector& value)
{
    nweb_hap_value_->SetStringArray(ArkWebStringVectorStructToClass(value));
}

ArkWebString ArkWebHapValueImpl::GetErrMsg()
{
    return ArkWebStringClassToStruct(nweb_hap_value_->GetErrMsg());
}

void ArkWebHapValueImpl::SetErrMsg(const ArkWebString& msg)
{
    nweb_hap_value_->SetErrMsg(ArkWebStringStructToClass(msg));
}

ArkWebString ArkWebHapValueImpl::GetErrName()
{
    return ArkWebStringClassToStruct(nweb_hap_value_->GetErrName());
}

void ArkWebHapValueImpl::SetErrName(const ArkWebString& name)
{
    nweb_hap_value_->SetErrName(ArkWebStringStructToClass(name));
}

ArkWebString ArkWebHapValueImpl::GetErrorDescription()
{
    return ArkWebStringClassToStruct(nweb_hap_value_->GetErrorDescription());
}

void ArkWebHapValueImpl::SetErrorDescription(const ArkWebString& description)
{
    nweb_hap_value_->SetErrorDescription(ArkWebStringStructToClass(description));
}

} // namespace OHOS::ArkWeb
