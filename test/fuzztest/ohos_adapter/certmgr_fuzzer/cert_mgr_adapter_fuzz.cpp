
/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * you may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cert_mgr_adapter_fuzz.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "cert_mgr_adapter_impl.h"

using namespace OHOS::NWeb;

void CertManagerAdapterFuzzTest(const uint8_t* data, size_t size)
{
    CertManagerAdapterImpl adapter;

    uint8_t certData[MAX_LEN_CERTIFICATE];
    adapter.GetCertDataBySubject("test_subject", certData, CM_SYSTEM_TRUSTED_STORE);

    uint8_t systemRootCertData[MAX_LEN_CERTIFICATE];
    adapter.GetSytemRootCertData(1, systemRootCertData);

    uint8_t userRootCertData[MAX_LEN_CERTIFICATE];
    adapter.GetUserRootCertData(1, userRootCertData);

    uint8_t certBuffer[MAX_LEN_CERTIFICATE_CHAIN];
    uint32_t certLen = MAX_LEN_CERTIFICATE_CHAIN;
    adapter.GetAppCert(certData, certBuffer, &certLen);

    uint8_t signData[MAX_LEN_CERTIFICATE];
    uint32_t signDataLen = sizeof(signData);
    std::string uri = "test_uri";
    adapter.Sign(reinterpret_cast<const uint8_t*>(uri.c_str()), certData, sizeof(certData), signData, signDataLen);

    std::vector<std::string> certs;
    adapter.GetTrustAnchorsForHostName("example.com", certs);

    std::vector<std::string> pins;
    adapter.GetPinSetForHostName("example.com", pins);

    adapter.GetCertMaxSize();
    adapter.GetAppCertMaxSize();
    adapter.GetSytemRootCertSum();
    adapter.GetUserRootCertSum();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    CertManagerAdapterFuzzTest(data, size);
    return 0;
}