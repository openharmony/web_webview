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

#include "ohos_adapter/bridge/ark_cert_manager_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCertManagerAdapterImpl::ArkCertManagerAdapterImpl(std::shared_ptr<OHOS::NWeb::CertManagerAdapter> ref) : real_(ref)
{}

uint32_t ArkCertManagerAdapterImpl::GetCertMaxSize()
{
    return real_->GetCertMaxSize();
}

uint32_t ArkCertManagerAdapterImpl::GetAppCertMaxSize()
{
    return real_->GetAppCertMaxSize();
}

int32_t ArkCertManagerAdapterImpl::GetSytemRootCertData(uint32_t certCount, uint8_t* certData)
{
    return real_->GetSytemRootCertData(certCount, certData);
}

uint32_t ArkCertManagerAdapterImpl::GetSytemRootCertSum()
{
    return real_->GetSytemRootCertSum();
}

int32_t ArkCertManagerAdapterImpl::GetUserRootCertData(uint32_t certCount, uint8_t* certData)
{
    return real_->GetUserRootCertData(certCount, certData);
}

uint32_t ArkCertManagerAdapterImpl::GetUserRootCertSum()
{
    return real_->GetUserRootCertSum();
}

int32_t ArkCertManagerAdapterImpl::GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len)
{
    return real_->GetAppCert(uriData, certData, len);
}

int32_t ArkCertManagerAdapterImpl::Sign(
    const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen, uint8_t* signData, uint32_t signDataLen)
{
    return real_->Sign(uri, certData, certDataLen, signData, signDataLen);
}

int32_t ArkCertManagerAdapterImpl::GetCertDataBySubject(const char* subjectName, uint8_t* certData, int32_t certType)
{
    return real_->GetCertDataBySubject(subjectName, certData, certType);
}

int ArkCertManagerAdapterImpl::VerifyCertFromNetSsl(uint8_t* certData, uint32_t certSize)
{
    return 0;
}

bool ArkCertManagerAdapterImpl::GetTrustAnchorsForHostName(const ArkWebString& hostname, ArkWebStringVector& certs)
{
    std::vector<std::string> vec;
    bool result = real_->GetTrustAnchorsForHostName(ArkWebStringStructToClass(hostname), vec);
    certs = ArkWebStringVectorClassToStruct(vec);
    return result;
}

bool ArkCertManagerAdapterImpl::GetPinSetForHostName(const ArkWebString& hostname, ArkWebStringVector& pins)
{
    std::vector<std::string> vec;
    bool result = real_->GetPinSetForHostName(ArkWebStringStructToClass(hostname), vec);
    pins = ArkWebStringVectorClassToStruct(vec);
    return result;
}

} // namespace OHOS::ArkWeb
