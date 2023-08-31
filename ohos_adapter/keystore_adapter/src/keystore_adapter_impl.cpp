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

#include "keystore_adapter_impl.h"

#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <climits>

#include "nweb_log.h"
#include <algorithm>

namespace {
const uint32_t AES_COMMON_SIZE = 1024;
static const uint32_t IV_SIZE = 16;
static const uint8_t IV[IV_SIZE] = { 0 };
}

namespace OHOS::NWeb {
// static
KeystoreAdapterImpl& KeystoreAdapterImpl::GetInstance()
{
    static KeystoreAdapterImpl instance;
    return instance;
}

int32_t KeystoreAdapterImpl::InitParamSet(
    struct HksParamSet **paramSet,
    const struct HksParam *params,
    uint32_t paramCount)
{
    int32_t ret = HksInitParamSet(paramSet);
    if (ret != HKS_SUCCESS) {
        return ret;
    }

    ret = HksAddParams(*paramSet, params, paramCount);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(paramSet);
        return ret;
    }

    ret = HksBuildParamSet(paramSet);
    if (ret != HKS_SUCCESS) {
        HksFreeParamSet(paramSet);
        return ret;
    }

    return ret;
}

struct HksParam g_genEncDecParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }
};

struct HksParam g_encryptParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_ENCRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }, {
        .tag = HKS_TAG_IV,
        .blob = {
            .size = IV_SIZE,
            .data = (uint8_t *)IV
        }
    }
};

struct HksParam g_decryptParams[] = {
    {
        .tag = HKS_TAG_ALGORITHM,
        .uint32Param = HKS_ALG_AES
    }, {
        .tag = HKS_TAG_PURPOSE,
        .uint32Param = HKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = HKS_TAG_KEY_SIZE,
        .uint32Param = HKS_AES_KEY_SIZE_256
    }, {
        .tag = HKS_TAG_PADDING,
        .uint32Param = HKS_PADDING_NONE
    }, {
        .tag = HKS_TAG_BLOCK_MODE,
        .uint32Param = HKS_MODE_CBC
    }, {
        .tag = HKS_TAG_IV,
        .blob = {
            .size = IV_SIZE,
            .data = (uint8_t *)IV
        }
    }
};

std::string KeystoreAdapterImpl::EncryptKey(const std::string alias, const std::string plainData)
{
    struct HksBlob keyAlias = { alias.length(), (uint8_t *)alias.c_str() };
    struct HksBlob inData =  { plainData.length(), (uint8_t *)plainData.c_str() };
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *encryptParamSet = nullptr;
    uint8_t cipher[AES_COMMON_SIZE] = {0};
    struct HksBlob cipherText = {AES_COMMON_SIZE, cipher};
    int32_t ohResult = InitParamSet(&genParamSet, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(HksParam));
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("init gen param set failed, error code: %d", ohResult);
        HksFreeParamSet(&genParamSet);
        return std::string();
    }
    ohResult = HksKeyExist(&keyAlias, genParamSet);
    if (ohResult != HKS_SUCCESS) {
        ohResult = HksGenerateKey(&keyAlias, genParamSet, nullptr);
        if (ohResult != HKS_SUCCESS) {
            WVLOG_E("generate key failed, error code: %d", ohResult);
            HksFreeParamSet(&genParamSet);
            return std::string();
        }
    }
    ohResult = InitParamSet(&encryptParamSet, g_encryptParams, sizeof(g_encryptParams) / sizeof(HksParam));
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("init encrypt param set failed, error code: %d", ohResult);
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&encryptParamSet);
        return std::string();
    }
    uint8_t handleE[sizeof(uint64_t)] = {0};
    struct HksBlob handleEncrypt = {sizeof(uint64_t), handleE};
    ohResult = HksInit(&keyAlias, encryptParamSet, &handleEncrypt, nullptr);
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("hks init invoke failed, error code: %d", ohResult);
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&encryptParamSet);
        return std::string();
    }
    ohResult = HksFinish(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("hks finish invoke failed, error code: %d", ohResult);
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&encryptParamSet);
        return std::string();
    }

    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    return std::string(reinterpret_cast<char*>(cipherText.data), cipherText.size);
}

std::string KeystoreAdapterImpl::DecryptKey(const std::string alias, const std::string encryptedData)
{
    struct HksBlob keyAlias = { alias.length(), (uint8_t *)alias.c_str() };
    struct HksBlob cipherText =  { encryptedData.length(), (uint8_t *)encryptedData.c_str() };
    struct HksParamSet *genParamSet = nullptr;
    struct HksParamSet *decryptParamSet = nullptr;
    uint8_t plain[AES_COMMON_SIZE] = {0};
    struct HksBlob plainText = {AES_COMMON_SIZE, plain};
    int32_t ohResult = InitParamSet(&genParamSet, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(HksParam));
    if (ohResult != HKS_SUCCESS) {
        HksFreeParamSet(&genParamSet);
        WVLOG_E("init gen param set failed, error code: %d", ohResult);
        return std::string();
    }
    ohResult = InitParamSet(&decryptParamSet, g_decryptParams, sizeof(g_decryptParams) / sizeof(HksParam));
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("init decrypt param set failed, error code: %d", ohResult);
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&decryptParamSet);
        return std::string();
    }
    ohResult = HksKeyExist(&keyAlias, genParamSet);
    if (ohResult != HKS_SUCCESS) {
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&decryptParamSet);
        WVLOG_E("hks key is not exist, error code: %d", ohResult);
        return std::string();
    }
    uint8_t handleD[sizeof(uint64_t)] = {0};
    struct HksBlob handleDecrypt = {sizeof(uint64_t), handleD};
    ohResult = HksInit(&keyAlias, decryptParamSet, &handleDecrypt, nullptr);
    if (ohResult != HKS_SUCCESS) {
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&decryptParamSet);
        WVLOG_E("hks init invoke failed, error code: %d", ohResult);
        return std::string();
    }
    ohResult = HksFinish(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    if (ohResult != HKS_SUCCESS) {
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&decryptParamSet);
        WVLOG_E("hks finish invoke failed, error code: %d", ohResult);
        return std::string();
    }
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&decryptParamSet);
    return std::string(reinterpret_cast<char*>(plainText.data), plainText.size);
}
} // namespace OHOS::NWeb
