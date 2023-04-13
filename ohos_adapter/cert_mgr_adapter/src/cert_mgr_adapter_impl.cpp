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

#include <vector>

#include "cert_mgr_adapter_impl.h"
#include "nweb_log.h"
#include "securec.h"

using namespace OHOS::NWeb;

namespace OHOS::NWeb {

int32_t CertManagerAdapterImpl::InitCertList(struct CertList **cList)
{
    *cList = static_cast<struct CertList *>(malloc(sizeof(struct CertList)));
    if (*cList == nullptr) {
        return CMR_ERROR_MALLOC_FAIL;
    }

    uint32_t buffSize = MAX_COUNT_CERTIFICATE * sizeof(struct CertAbstract);
    (*cList)->certAbstract = static_cast<struct CertAbstract *>(malloc(buffSize));
    if ((*cList)->certAbstract == nullptr) {
        free(*cList);
        *cList = nullptr;
        return CMR_ERROR_MALLOC_FAIL;
    }
    (void)memset_s((*cList)->certAbstract, buffSize, 0, buffSize);
    (*cList)->certsCount = MAX_COUNT_CERTIFICATE;

    return CM_SUCCESS;
}

int32_t CertManagerAdapterImpl::InitCertInfo(struct CertInfo *certInfo)
{
    if (certInfo == nullptr) {
        return CMR_ERROR_MALLOC_FAIL;
    }

    certInfo->certInfo.data = static_cast<uint8_t *>(malloc(MAX_LEN_CERTIFICATE));
    if (certInfo->certInfo.data == nullptr) {
        return CMR_ERROR_MALLOC_FAIL;
    }
    certInfo->certInfo.size = MAX_LEN_CERTIFICATE;

    return CM_SUCCESS;
}

void CertManagerAdapterImpl::FreeCMBlobData(struct CmBlob *blob)
{
    if (blob == nullptr) {
        return;
    }

    if (blob->data != nullptr) {
        free(blob->data);
        blob->data = nullptr;
    }
    blob->size = 0;
}

void CertManagerAdapterImpl::FreeCertList(CertList *certList)
{
    if (certList == nullptr) {
        return;
    }

    if (certList->certAbstract != nullptr) {
        free(certList->certAbstract);
        certList->certAbstract = nullptr;
    }

    free(certList);
    certList = nullptr;
}

uint32_t CertManagerAdapterImpl::GetCertMaxSize()
{
    return MAX_LEN_CERTIFICATE;
}

uint32_t CertManagerAdapterImpl::GetAppCertMaxSize()
{
    return MAX_LEN_CERTIFICATE_CHAIN;
}

uint32_t CertManagerAdapterImpl::GetSytemRootCertSum()
{
    uint32_t certSum = 0;
    struct CertList *certList = nullptr;
    int32_t ret = InitCertList(&certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetSytemRootCertSum, init cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return certSum;
    }

    ret = CmGetCertList(CM_SYSTEM_TRUSTED_STORE, certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetSytemRootCertSum, get cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return certSum;
    }

    if (certList != nullptr) {
        certSum = certList->certsCount;
    }

    FreeCertList(certList);
    return certSum;
}

int32_t CertManagerAdapterImpl::GetSytemRootCertData(uint32_t certCount, uint8_t* certData)
{
    struct CertList *certList = nullptr;
    int32_t ret = InitCertList(&certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetSytemRootCertData, init cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return CM_FAILURE;
    }

    ret = CmGetCertList(CM_SYSTEM_TRUSTED_STORE, certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetSytemRootCertData, get cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return CM_FAILURE;
    }

    struct CertInfo certInfo;
    unsigned int len = sizeof(struct CertInfo);
    (void)memset_s(&certInfo, len, 0, len);
    ret = InitCertInfo(&certInfo);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetSytemRootCertData, init cert failed, ret = %{public}d ", ret);
        FreeCMBlobData(&(certInfo.certInfo));
        FreeCertList(certList);
        return CM_FAILURE;
    }

    if (certCount > certList->certsCount) {
        WVLOG_E("GetSytemRootCertData, cert count invailed, cert count = %{public}d ", certCount);
        return CM_FAILURE;
    }

    char *uri = certList->certAbstract[certCount].uri;
    struct CmBlob uriBlob = {strlen(uri) + 1, (uint8_t *)(uri)};

    ret = CmGetCertInfo(&uriBlob, CM_SYSTEM_TRUSTED_STORE, &certInfo);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetSytemRootCertData, get cert info failed, ret = %{public}d ", ret);
        FreeCMBlobData(&(certInfo.certInfo));
        FreeCertList(certList);
        return CM_FAILURE;
    }

    if (memcpy_s(certData, MAX_LEN_CERTIFICATE, certInfo.certInfo.data, certInfo.certInfo.size) != EOK) {
        WVLOG_E("GetSytemRootCertData, memory copy failed");
        FreeCMBlobData(&(certInfo.certInfo));
        FreeCertList(certList);
        return CM_FAILURE;
    }

    FreeCMBlobData(&(certInfo.certInfo));
    FreeCertList(certList);
    return CM_SUCCESS;
}

uint32_t CertManagerAdapterImpl::GetUserRootCertSum()
{
    uint32_t certSum = 0;
    struct CertList *certList = nullptr;
    int32_t ret = InitCertList(&certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetUserRootCertSum, init cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return certSum;
    }

    ret = CmGetUserCertList(CM_USER_TRUSTED_STORE, certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetUserRootCertSum, get user cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return certSum;
    }

    if (certList != nullptr) {
        certSum = certList->certsCount;
    }

    FreeCertList(certList);
    return certSum;
}

int32_t CertManagerAdapterImpl::GetUserRootCertData(uint32_t certCount, uint8_t* certData)
{
    struct CertList *certList = nullptr;
    int32_t ret = InitCertList(&certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetUserRootCertData, init cert list, ret = %{public}d ", ret);
        FreeCertList(certList);
        return CM_FAILURE;
    }

    ret = CmGetUserCertList(CM_USER_TRUSTED_STORE, certList);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetUserRootCertData, get user cert list failed, ret = %{public}d ", ret);
        FreeCertList(certList);
        return CM_FAILURE;
    }

    struct CertInfo certInfo;
    unsigned int len = sizeof(struct CertInfo);
    (void)memset_s(&certInfo, len, 0, len);
    ret = InitCertInfo(&certInfo);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetUserRootCertData, init cert info failed, ret = %{public}d ", ret);
        FreeCMBlobData(&(certInfo.certInfo));
        FreeCertList(certList);
        return CM_FAILURE;
    }

    if (certCount > certList->certsCount) {
        WVLOG_E("GetUserRootCertData, cert count invailed, cert count = %{public}d ", certCount);
        return CM_FAILURE;
    }

    char *uri = certList->certAbstract[certCount].uri;
    struct CmBlob uriBlob = {strlen(uri) + 1, (uint8_t *)(uri)};

    ret = CmGetUserCertInfo(&uriBlob, CM_USER_TRUSTED_STORE, &certInfo);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetUserRootCertData, get user cert info failed, ret = %{public}d ", ret);
        FreeCMBlobData(&(certInfo.certInfo));
        FreeCertList(certList);
        return CM_FAILURE;
    }

    if (memcpy_s(certData, MAX_LEN_CERTIFICATE, certInfo.certInfo.data, certInfo.certInfo.size) != EOK) {
        WVLOG_E("GetUserRootCertData, memory copy failed");
        FreeCMBlobData(&(certInfo.certInfo));
        FreeCertList(certList);
        return CM_FAILURE;
    }

    FreeCMBlobData(&(certInfo.certInfo));
    FreeCertList(certList);
    return CM_SUCCESS;
}

int32_t CertManagerAdapterImpl::GetAppCert(uint8_t* uriData, uint8_t* certData, uint32_t* len)
{
    struct Credential cred;
    (void)memset_s(&cred, sizeof(struct Credential), 0, sizeof(struct Credential));
    cred.credData.size = MAX_LEN_CERTIFICATE_CHAIN;
    cred.credData.data = static_cast<uint8_t *>(malloc(MAX_LEN_CERTIFICATE_CHAIN));
    if (cred.credData.data == nullptr) {
        WVLOG_E("GetAppCert, malloc failed");
        return CM_FAILURE;
    }

    struct CmBlob uri = { strlen((char*)uriData) + 1, uriData };
    int32_t ret = CmGetAppCert(&uri, CM_CREDENTIAL_STORE, &cred);
    if (ret != CM_SUCCESS) {
        WVLOG_E("GetAppCert, get app cert failed, ret = %{public}d ", ret);
        free(cred.credData.data);
        cred.credData.data = nullptr;
        return CM_FAILURE;
    }

    *len = cred.credData.size;
    if (memcpy_s(certData, MAX_LEN_CERTIFICATE_CHAIN, cred.credData.data, cred.credData.size) != EOK) {
        WVLOG_E("GetAppCert, memory copy failed");
        free(cred.credData.data);
        cred.credData.data = nullptr;
        return CM_FAILURE;
    }

    free(cred.credData.data);
    cred.credData.data = nullptr;
    return CM_SUCCESS;
}

int32_t CertManagerAdapterImpl::Sign(const uint8_t* uri, const uint8_t* certData, uint32_t certDataLen,
    uint8_t* signData, uint32_t signDataLen)
{
    uint64_t handleValue = 0;
    struct CmBlob handle = { sizeof(uint64_t), (uint8_t *)&handleValue };
    const struct CmBlob keyUri = { strlen((char*)uri) + 1, (uint8_t*)uri };
    struct CmSignatureSpec spec = { CM_KEY_PURPOSE_SIGN, CM_PADDING_PSS, CM_DIGEST_SHA256 };

    if (signData == nullptr) {
        WVLOG_E("Sign, sign data is nullptr");
        return CM_FAILURE;
    }

    int32_t ret = CmInit(&keyUri, &spec, &handle);
    if (ret != CM_SUCCESS) {
        WVLOG_E("Sign, init failed, ret = %{public}d ", ret);
        return CM_FAILURE;
    }

    const struct CmBlob message = {certDataLen, const_cast<uint8_t *>(certData)};
    ret = CmUpdate(&handle, &message);
    if (ret != CM_SUCCESS) {
        WVLOG_E("Sign, update failed, ret = %{public}d ", ret);
        return CM_FAILURE;
    }

    struct CmBlob signature = { signDataLen, signData };
    struct CmBlob inDataFinish = { 0, nullptr };
    ret = CmFinish(&handle, &inDataFinish, &signature);
    if (ret != CM_SUCCESS) {
        WVLOG_E("Sign, finish failed, ret = %{public}d ", ret);
        return CM_FAILURE;
    }

    ret = CmAbort(&handle);
    if (ret != CM_SUCCESS) {
        WVLOG_E("Sign, abort failed, ret = %{public}d ", ret);
        return CM_FAILURE;
    }

    return CM_SUCCESS;
}
}