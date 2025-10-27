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

#ifndef CERT_MGR_ADAPTER_H
#define CERT_MGR_ADAPTER_H

#include <functional>
#include <memory>

namespace OHOS::NWeb {

class CertManagerAdapter {
public:
    CertManagerAdapter() = default;
    virtual ~CertManagerAdapter() = default;

    virtual uint32_t GetCertMaxSize() = 0;

    virtual uint32_t GetAppCertMaxSize() = 0;

    virtual int32_t GetSytemRootCertData(uint32_t certCount, uint8_t* certData) = 0;

    virtual uint32_t GetSytemRootCertSum() = 0;

    virtual int32_t GetUserRootCertData(uint32_t certCount, uint8_t* certData) = 0;

    virtual uint32_t GetUserRootCertSum() = 0;

    virtual int32_t GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len) = 0;

    virtual int32_t Sign(
        const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen, uint8_t* signData, uint32_t signDataLen) = 0;

    virtual int32_t GetCertDataBySubject(const char* subjectName, uint8_t* certData, int32_t certType) = 0;

    virtual bool GetTrustAnchorsForHostName(const std::string& hostname, std::vector<std::string>& certs) = 0;

    virtual bool GetPinSetForHostName(const std::string& hostname, std::vector<std::string>& pins) = 0;

    virtual int32_t SignV2(const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen,
        uint8_t* signData, uint32_t* signDataLen, uint16_t algorithm) {return 0;};

    virtual int32_t GetUkeyCert(const std::string& identity, uint8_t* certData, uint32_t* certDataLen) {return -1;}
 
    virtual int32_t OpenUKeyRemoteHandle(const std::string& identity) {return -1;}
 
    virtual int32_t CloseUKeyRemoteHandle(const std::string& identity) {return -1;}
 
    virtual int32_t GetUkeyPinAuthState(const std::string& uri, bool* state) {return -1;}
 
    virtual int32_t SignUsingHuks(const std::string& identity, const uint8_t* certData, uint32_t certDataLen,
        uint8_t* signData, uint32_t* signDataLen, uint16_t algorithm, uint32_t keySize) {return -1;}
};

} // namespace OHOS::NWeb

#endif // CERT_MGR_ADAPTER_H