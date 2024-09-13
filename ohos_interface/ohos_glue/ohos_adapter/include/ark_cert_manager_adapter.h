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

#ifndef ARK_CERT_MANAGER_ADAPTER_H
#define ARK_CERT_MANAGER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkCertManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint32_t GetCertMaxSize() = 0;

    /*--ark web()--*/
    virtual uint32_t GetAppCertMaxSize() = 0;

    /*--ark web()--*/
    virtual int32_t GetSytemRootCertData(uint32_t certCount, uint8_t* certData) = 0;

    /*--ark web()--*/
    virtual uint32_t GetSytemRootCertSum() = 0;

    /*--ark web()--*/
    virtual int32_t GetUserRootCertData(uint32_t certCount, uint8_t* certData) = 0;

    /*--ark web()--*/
    virtual uint32_t GetUserRootCertSum() = 0;

    /*--ark web()--*/
    virtual int32_t GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len) = 0;

    /*--ark web()--*/
    virtual int32_t Sign(
        const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen, uint8_t* signData, uint32_t signDataLen) = 0;

    /*--ark web()--*/
    virtual int32_t GetCertDataBySubject(const char* subjectName, uint8_t* certData, int32_t certType) = 0;

    /*--ark web()--*/
    virtual int VerifyCertFromNetSsl(uint8_t* certData, uint32_t certSize) = 0;

    /*--ark web()--*/
    virtual bool GetTrustAnchorsForHostName(const ArkWebString& hostname, ArkWebStringVector& certs) = 0;

    /*--ark web()--*/
    virtual bool GetPinSetForHostName(const ArkWebString& hostname, ArkWebStringVector& pins) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_CERT_MANAGER_ADAPTER_H
