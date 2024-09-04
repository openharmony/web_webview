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

#include <huks/native_huks_api.h>
#include <huks/native_huks_param.h>
#include <string>

#include "nweb_log.h"

namespace {
const uint32_t AES_COMMON_SIZE = 1024;
static const uint32_t IV_SIZE = 16;
static const uint8_t IV[IV_SIZE] = { 0 };
} // namespace

namespace OHOS::NWeb {
// static
KeystoreAdapterImpl& KeystoreAdapterImpl::GetInstance()
{
    static KeystoreAdapterImpl instance;
    return instance;
}

int32_t KeystoreAdapterImpl::InitParamSet(
    struct OH_Huks_ParamSet** paramSet, const struct OH_Huks_Param* params, uint32_t paramCount)
{
    OH_Huks_Result ohResult = OH_Huks_InitParamSet(paramSet);
    if (ohResult.errorCode != OH_HUKS_SUCCESS) {
        return ohResult.errorCode;
    }

    ohResult = OH_Huks_AddParams(*paramSet, params, paramCount);
    if (ohResult.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(paramSet);
        return ohResult.errorCode;
    }

    ohResult = OH_Huks_BuildParamSet(paramSet);
    if (ohResult.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(paramSet);
        return ohResult.errorCode;
    }

    return ohResult.errorCode;
}

struct OH_Huks_Param g_genEncDecParams[] = {
    {
        .tag = OH_HUKS_TAG_ALGORITHM,
        .uint32Param = OH_HUKS_ALG_AES
    }, {
        .tag = OH_HUKS_TAG_PURPOSE,
        .uint32Param = OH_HUKS_KEY_PURPOSE_ENCRYPT | OH_HUKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = OH_HUKS_TAG_KEY_SIZE,
        .uint32Param = OH_HUKS_AES_KEY_SIZE_256
    }, {
        .tag = OH_HUKS_TAG_PADDING,
        .uint32Param = OH_HUKS_PADDING_NONE
    }, {
        .tag = OH_HUKS_TAG_BLOCK_MODE,
        .uint32Param = OH_HUKS_MODE_CBC
    }
};

struct OH_Huks_Param g_encryptParams[] = {
    {
        .tag = OH_HUKS_TAG_ALGORITHM,
        .uint32Param = OH_HUKS_ALG_AES
    }, {
        .tag = OH_HUKS_TAG_PURPOSE,
        .uint32Param = OH_HUKS_KEY_PURPOSE_ENCRYPT
    }, {
        .tag = OH_HUKS_TAG_KEY_SIZE,
        .uint32Param = OH_HUKS_AES_KEY_SIZE_256
    }, {
        .tag = OH_HUKS_TAG_PADDING,
        .uint32Param = OH_HUKS_PADDING_NONE
    }, {
        .tag = OH_HUKS_TAG_BLOCK_MODE,
        .uint32Param = OH_HUKS_MODE_CBC
    }, {
        .tag = OH_HUKS_TAG_IV,
        .blob = {
            .size = IV_SIZE,
            .data = (uint8_t*)IV
        }
    }
};

struct OH_Huks_Param g_decryptParams[] = {
    {
        .tag = OH_HUKS_TAG_ALGORITHM,
        .uint32Param = OH_HUKS_ALG_AES
    }, {
        .tag = OH_HUKS_TAG_PURPOSE,
        .uint32Param = OH_HUKS_KEY_PURPOSE_DECRYPT
    }, {
        .tag = OH_HUKS_TAG_KEY_SIZE,
        .uint32Param = OH_HUKS_AES_KEY_SIZE_256
    }, {
        .tag = OH_HUKS_TAG_PADDING,
        .uint32Param = OH_HUKS_PADDING_NONE
    }, {
        .tag = OH_HUKS_TAG_BLOCK_MODE,
        .uint32Param = OH_HUKS_MODE_CBC
    }, {
        .tag = OH_HUKS_TAG_IV,
        .blob = {
            .size = IV_SIZE,
            .data = (uint8_t*)IV
        }
    }
};

std::string KeystoreAdapterImpl::EncryptKey(const std::string alias, const std::string plainData)
{
    struct OH_Huks_Blob keyAlias = { alias.length(), (uint8_t*)alias.c_str() };
    struct OH_Huks_Blob inData = { plainData.length(), (uint8_t*)plainData.c_str() };
    struct OH_Huks_ParamSet* genParamSet = nullptr;
    struct OH_Huks_ParamSet* encryptParamSet = nullptr;
    uint8_t cipher[AES_COMMON_SIZE] = { 0 };
    struct OH_Huks_Blob cipherText = { AES_COMMON_SIZE, cipher };
    int32_t ohResult =
        InitParamSet(&genParamSet, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(OH_Huks_Param));
    if (ohResult != OH_HUKS_SUCCESS) {
        WVLOG_E("init gen param set failed, error code: %d", ohResult);
        OH_Huks_FreeParamSet(&genParamSet);
        return std::string();
    }
    struct OH_Huks_Result ohHuksResult = OH_Huks_IsKeyItemExist(&keyAlias, genParamSet);
    if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
        ohHuksResult = OH_Huks_GenerateKeyItem(&keyAlias, genParamSet, nullptr);
        if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
            WVLOG_E("generate key failed, error code: %d", ohHuksResult.errorCode);
            OH_Huks_FreeParamSet(&genParamSet);
            return std::string();
        }
    }
    ohResult = InitParamSet(&encryptParamSet, g_encryptParams, sizeof(g_encryptParams) / sizeof(OH_Huks_Param));
    if (ohResult != OH_HUKS_SUCCESS) {
        WVLOG_E("init encrypt param set failed, error code: %d", ohResult);
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&encryptParamSet);
        return std::string();
    }
    uint8_t handleE[sizeof(uint64_t)] = { 0 };
    struct OH_Huks_Blob handleEncrypt = { sizeof(uint64_t), handleE };
    ohHuksResult = OH_Huks_InitSession(&keyAlias, encryptParamSet, &handleEncrypt, nullptr);
    if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("hks init invoke failed, error code: %d", ohHuksResult.errorCode);
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&encryptParamSet);
        return std::string();
    }
    ohHuksResult = OH_Huks_FinishSession(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
        WVLOG_E("hks finish invoke failed, error code: %d", ohHuksResult.errorCode);
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&encryptParamSet);
        return std::string();
    }

    OH_Huks_FreeParamSet(&genParamSet);
    OH_Huks_FreeParamSet(&encryptParamSet);
    return std::string(reinterpret_cast<char*>(cipherText.data), cipherText.size);
}

std::string KeystoreAdapterImpl::DecryptKey(const std::string alias, const std::string encryptedData)
{
    struct OH_Huks_Blob keyAlias = { alias.length(), (uint8_t*)alias.c_str() };
    struct OH_Huks_Blob cipherText = { encryptedData.length(), (uint8_t*)encryptedData.c_str() };
    struct OH_Huks_ParamSet* genParamSet = nullptr;
    struct OH_Huks_ParamSet* decryptParamSet = nullptr;
    uint8_t plain[AES_COMMON_SIZE] = { 0 };
    struct OH_Huks_Blob plainText = { AES_COMMON_SIZE, plain };
    int32_t ohResult =
        InitParamSet(&genParamSet, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(OH_Huks_Param));
    if (ohResult != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(&genParamSet);
        WVLOG_E("init gen param set failed, error code: %d", ohResult);
        return std::string();
    }
    ohResult = InitParamSet(&decryptParamSet, g_decryptParams, sizeof(g_decryptParams) / sizeof(OH_Huks_Param));
    if (ohResult != OH_HUKS_SUCCESS) {
        WVLOG_E("init decrypt param set failed, error code: %d", ohResult);
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&decryptParamSet);
        return std::string();
    }
    struct OH_Huks_Result ohHuksResult = OH_Huks_IsKeyItemExist(&keyAlias, genParamSet);
    if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&decryptParamSet);
        WVLOG_E("hks key is not exist, error code: %d", ohHuksResult.errorCode);
        return std::string();
    }
    uint8_t handleD[sizeof(uint64_t)] = { 0 };
    struct OH_Huks_Blob handleDecrypt = { sizeof(uint64_t), handleD };
    ohHuksResult = OH_Huks_InitSession(&keyAlias, decryptParamSet, &handleDecrypt, nullptr);
    if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&decryptParamSet);
        WVLOG_E("hks init invoke failed, error code: %d", ohHuksResult.errorCode);
        return std::string();
    }
    ohHuksResult = OH_Huks_FinishSession(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    if (ohHuksResult.errorCode != OH_HUKS_SUCCESS) {
        OH_Huks_FreeParamSet(&genParamSet);
        OH_Huks_FreeParamSet(&decryptParamSet);
        WVLOG_E("hks finish invoke failed, error code: %d", ohHuksResult.errorCode);
        return std::string();
    }
    OH_Huks_FreeParamSet(&genParamSet);
    OH_Huks_FreeParamSet(&decryptParamSet);
    return std::string(reinterpret_cast<char*>(plainText.data), plainText.size);
}
} // namespace OHOS::NWeb
