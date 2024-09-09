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

#include "ohos_adapter/bridge/ark_cert_manager_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkCertManagerAdapterWrapper::ArkCertManagerAdapterWrapper(ArkWebRefPtr<ArkCertManagerAdapter> ref) : ctocpp_(ref) {}

uint32_t ArkCertManagerAdapterWrapper::GetCertMaxSize()
{
    return ctocpp_->GetCertMaxSize();
}

uint32_t ArkCertManagerAdapterWrapper::GetAppCertMaxSize()
{
    return ctocpp_->GetAppCertMaxSize();
}

int32_t ArkCertManagerAdapterWrapper::GetSytemRootCertData(uint32_t certCount, uint8_t* certData)
{
    return ctocpp_->GetSytemRootCertData(certCount, certData);
}

uint32_t ArkCertManagerAdapterWrapper::GetSytemRootCertSum()
{
    return ctocpp_->GetSytemRootCertSum();
}

int32_t ArkCertManagerAdapterWrapper::GetUserRootCertData(uint32_t certCount, uint8_t* certData)
{
    return ctocpp_->GetUserRootCertData(certCount, certData);
}

uint32_t ArkCertManagerAdapterWrapper::GetUserRootCertSum()
{
    return ctocpp_->GetUserRootCertSum();
}

int32_t ArkCertManagerAdapterWrapper::GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len)
{
    return ctocpp_->GetAppCert(uriData, certData, len);
}

int32_t ArkCertManagerAdapterWrapper::Sign(
    const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen, uint8_t* signData, uint32_t signDataLen)
{
    return ctocpp_->Sign(uri, certData, certDataLen, signData, signDataLen);
}

int32_t ArkCertManagerAdapterWrapper::GetCertDataBySubject(const char* subjectName, uint8_t* certData, int32_t certType)
{
    return ctocpp_->GetCertDataBySubject(subjectName, certData, certType);
}

int ArkCertManagerAdapterWrapper::VerifyCertFromNetSsl(uint8_t* certData, uint32_t certSize)
{
    return ctocpp_->VerifyCertFromNetSsl(certData, certSize);
}

bool ArkCertManagerAdapterWrapper::GetTrustAnchorsForHostName(
    const std::string& hostname, std::vector<std::string>& certs)
{
    ArkWebString ark_hostname = ArkWebStringClassToStruct(hostname);
    ArkWebStringVector ark_certs = ArkWebStringVectorClassToStruct(certs);
    bool result = ctocpp_->GetTrustAnchorsForHostName(ark_hostname, ark_certs);
    certs = ArkWebStringVectorStructToClass(ark_certs);
    ArkWebStringStructRelease(ark_hostname);
    ArkWebStringVectorStructRelease(ark_certs);
    return result;
}

bool ArkCertManagerAdapterWrapper::GetPinSetForHostName(const std::string& hostname, std::vector<std::string>& pins)
{
    ArkWebString ark_hostname = ArkWebStringClassToStruct(hostname);
    ArkWebStringVector ark_pins = ArkWebStringVectorClassToStruct(pins);
    bool result = ctocpp_->GetPinSetForHostName(ark_hostname, ark_pins);
    pins = ArkWebStringVectorStructToClass(ark_pins);
    ArkWebStringStructRelease(ark_hostname);
    ArkWebStringVectorStructRelease(ark_pins);
    return result;
}

} // namespace OHOS::ArkWeb
