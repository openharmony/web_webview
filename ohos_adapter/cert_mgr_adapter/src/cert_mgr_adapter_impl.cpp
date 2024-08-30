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

#include <string>
#include <vector>

#include "nweb_log.h"
#include "cert_mgr_adapter_impl.h"
#include <network/netstack/net_ssl/net_ssl_c.h>

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

uint32_t CertManagerAdapterImpl::GetCertMaxSize()
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetCertMaxSize");
    return 0;
}

uint32_t CertManagerAdapterImpl::GetAppCertMaxSize()
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetAppCertMaxSize");
    return 0;
}

uint32_t CertManagerAdapterImpl::GetSytemRootCertSum()
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetSytemRootCertSum");
    return 0;
}

int32_t CertManagerAdapterImpl::GetCertDataBySubject(const char *subjectName, uint8_t* certData, int32_t certType)
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetCertDataBySubject");
    return 0;
}

int32_t CertManagerAdapterImpl::GetSytemRootCertData(uint32_t certCount, uint8_t* certData)
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetSytemRootCertData");
    return 0;
}

uint32_t CertManagerAdapterImpl::GetUserRootCertSum()
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetUserRootCertSum");
    return 0;
}

int32_t CertManagerAdapterImpl::GetUserRootCertData(uint32_t certCount, uint8_t* certData)
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetUserRootCertData");
    return 0;
}

int32_t CertManagerAdapterImpl::GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len)
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::GetAppCert");
    return 0;
}

int32_t CertManagerAdapterImpl::Sign(const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen,
    uint8_t* signData, uint32_t signDataLen)
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::Sign");
    return 0;
}

int CertManagerAdapterImpl::VerifyCertFromNetSsl(uint8_t* certData, uint32_t certSize)
{
    WVLOG_D("[adapter mock] CertManagerAdapterImpl::VerifyCertFromNetSsl");
    return 0;
}

bool CertManagerAdapterImpl::GetTrustAnchorsForHostName(
    const std::string& hostname, std::vector<std::string>& certs)
{
    NetStack_Certificates certificates;
    int32_t ret = OH_NetStack_GetCertificatesForHostName(hostname.c_str(), &certificates);
    if (ret != 0) {
        WVLOG_E("GetTrustAnchorsForHostName for hostname:%{public}s failed",
            hostname.c_str());
        OH_Netstack_DestroyCertificatesContent(&certificates);
        return false;
    }
    for (size_t i = 0; i < certificates.length; i++) {
        certs.push_back(certificates.content[i]);
    }
    OH_Netstack_DestroyCertificatesContent(&certificates);
    return true;
}

bool CertManagerAdapterImpl::GetPinSetForHostName(
    const std::string& hostname, std::vector<std::string>& pins)
{
    NetStack_CertificatePinning certificatePinning;
    int32_t ret = OH_NetStack_GetPinSetForHostName(hostname.c_str(), &certificatePinning);
    if (ret != 0) {
        WVLOG_E("GetPinSetForHostName for hostname:%{public}s failed, ret:%{public}d",
            hostname.c_str(), ret);
        return false;
    }
    std::string pinsString;
    if (certificatePinning.publicKeyHash == nullptr) {
        WVLOG_D("GetPinSetForHostName for hostname:%{public}s is empty", hostname.c_str());
        return true;
    } else {
        pinsString = certificatePinning.publicKeyHash;
        free(certificatePinning.publicKeyHash);
    }
    size_t start = 0;
    size_t pos = pinsString.find(";", start);
    while (pos != std::string::npos) {
        pins.emplace_back(pinsString.substr(start, pos - start));
        start = pos + 1;
        pos = pinsString.find(";", start);
    }
    pins.emplace_back(pinsString.substr(start));
    return true;
}
}