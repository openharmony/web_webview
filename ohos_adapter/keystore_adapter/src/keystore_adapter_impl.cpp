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

#include <algorithm>
#include <climits>
#include <fcntl.h>
#include <string>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "asset_api.h"
#include "nweb_log.h"

namespace {
const uint32_t AES_COMMON_SIZE = 1024;
static const uint32_t IV_SIZE = 16;
static const uint8_t IV[IV_SIZE] = { 0 };
const std::string V10 = "V10";
const uint32_t V10_SIZE = 3;
const uint32_t CIPHER_TEXT_SIZE = 32;

struct HksParam g_genEncDecParams[] = { { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
    { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT | HKS_KEY_PURPOSE_DECRYPT },
    { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
    { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
    { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC } };

std::vector<uint8_t> GetRandom(size_t size)
{
    std::vector<uint8_t> result(size, 0);
    struct HksBlob randBlob = { result.size(), result.data() };
    int ret = HksGenerateRandom(nullptr, &randBlob);
    if (ret != HKS_SUCCESS) {
        WVLOG_E("Failed to generate random bytes, hks error code: %{public}d", ret);
        return {};
    }
    return result;
}
} // namespace

namespace OHOS::NWeb {
// static
KeystoreAdapterImpl& KeystoreAdapterImpl::GetInstance()
{
    static KeystoreAdapterImpl instance;
    return instance;
}

int32_t KeystoreAdapterImpl::InitParamSet(
    struct HksParamSet** paramSet, const struct HksParam* params, uint32_t paramCount)
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

bool KeystoreAdapterImpl::PrepareHuksInternal(struct HksParamSet** genParamSet, const struct HksBlob* keyAlias,
    struct HksParamSet** workParamSet, struct HksParam* workParams, size_t workParamCount)
{
    int32_t result = InitParamSet(genParamSet, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(HksParam));
    if (result != HKS_SUCCESS) {
        WVLOG_E("init gen param set failed, error code: %d", result);
        HksFreeParamSet(genParamSet);
        return false;
    }
    result = InitParamSet(workParamSet, workParams, workParamCount);
    if (result != HKS_SUCCESS) {
        WVLOG_E("init work param set failed, error code: %d", result);
        HksFreeParamSet(genParamSet);
        HksFreeParamSet(workParamSet);
        return false;
    }
    result = HksKeyExist(keyAlias, *genParamSet);
    if (result != HKS_SUCCESS) {
        WVLOG_E("hks key is not exist, error code: %d", result);
        HksFreeParamSet(genParamSet);
        HksFreeParamSet(workParamSet);
        return false;
    }
    return true;
}

std::string KeystoreAdapterImpl::EncryptKey(const std::string alias, const std::string plainData)
{
    struct HksBlob keyAlias = { alias.length(), (uint8_t*)alias.c_str() };
    struct HksBlob inData = { plainData.length(), (uint8_t*)plainData.c_str() };
    struct HksParamSet* genParamSet = nullptr;
    struct HksParamSet* encryptParamSet = nullptr;
    uint8_t cipher[AES_COMMON_SIZE] = { 0 };
    struct HksBlob cipherText = { AES_COMMON_SIZE, cipher };

    std::vector<uint8_t> iv = GetRandom(IV_SIZE);
    if (iv.empty()) {
        WVLOG_E("Failed to get random IV");
        return std::string();
    }
    struct HksParam encryptParams[] = { { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_ENCRYPT },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV, .blob = { .size = IV_SIZE, .data = iv.data() } } };
    if (!PrepareHuksInternal(
            &genParamSet, &keyAlias, &encryptParamSet, encryptParams, sizeof(encryptParams) / sizeof(HksParam))) {
        return std::string();
    }

    uint8_t handleE[sizeof(uint64_t)] = { 0 };
    struct HksBlob handleEncrypt = { sizeof(uint64_t), handleE };
    int32_t ohResult = HksInit(&keyAlias, encryptParamSet, &handleEncrypt, nullptr);
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("hks init invoke failed, error code: %d", ohResult);
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&encryptParamSet);
        return std::string();
    }
    ohResult = HksFinish(&handleEncrypt, encryptParamSet, &inData, &cipherText);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&encryptParamSet);
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("hks finish invoke failed, error code: %d", ohResult);
        return std::string();
    }
    if (cipherText.size == 0) {
        WVLOG_E("hks finish with empty cipher text");
        return std::string();
    }

    return V10 + std::string(reinterpret_cast<char*>(iv.data()), iv.size()) +
           std::string(reinterpret_cast<char*>(cipherText.data), cipherText.size);
}

std::string KeystoreAdapterImpl::DecryptKey(const std::string alias, const std::string encryptedData)
{
    struct HksBlob keyAlias = { alias.length(), (uint8_t*)alias.c_str() };
    struct HksParamSet* genParamSet = nullptr;
    struct HksParamSet* decryptParamSet = nullptr;
    uint8_t plain[AES_COMMON_SIZE] = { 0 };
    struct HksBlob plainText = { AES_COMMON_SIZE, plain };

    std::string ivStr;
    std::string cipherStr = encryptedData;
    if (encryptedData.length() == V10_SIZE + IV_SIZE + CIPHER_TEXT_SIZE &&
        encryptedData.compare(0, V10_SIZE, V10) == 0) {
        size_t prefix_size = V10_SIZE + IV_SIZE;
        ivStr = encryptedData.substr(V10_SIZE, IV_SIZE);
        cipherStr = encryptedData.substr(prefix_size, encryptedData.length() - prefix_size);
    } else if (encryptedData.length() != CIPHER_TEXT_SIZE) {
        WVLOG_W("Invalid cipher text length: %{public}zu", encryptedData.length());
        return std::string();
    }
    struct HksParam decryptParams[] = { { .tag = HKS_TAG_ALGORITHM, .uint32Param = HKS_ALG_AES },
        { .tag = HKS_TAG_PURPOSE, .uint32Param = HKS_KEY_PURPOSE_DECRYPT },
        { .tag = HKS_TAG_KEY_SIZE, .uint32Param = HKS_AES_KEY_SIZE_256 },
        { .tag = HKS_TAG_PADDING, .uint32Param = HKS_PADDING_NONE },
        { .tag = HKS_TAG_BLOCK_MODE, .uint32Param = HKS_MODE_CBC },
        { .tag = HKS_TAG_IV,
            .blob = { .size = IV_SIZE, .data = ivStr.empty() ? (uint8_t*)IV : (uint8_t*)ivStr.c_str() } } };
    struct HksBlob cipherText = { cipherStr.length(), (uint8_t*)cipherStr.c_str() };
    if (!PrepareHuksInternal(
            &genParamSet, &keyAlias, &decryptParamSet, decryptParams, sizeof(decryptParams) / sizeof(HksParam))) {
        return std::string();
    }

    uint8_t handleD[sizeof(uint64_t)] = { 0 };
    struct HksBlob handleDecrypt = { sizeof(uint64_t), handleD };
    int32_t ohResult = HksInit(&keyAlias, decryptParamSet, &handleDecrypt, nullptr);
    if (ohResult != HKS_SUCCESS) {
        HksFreeParamSet(&genParamSet);
        HksFreeParamSet(&decryptParamSet);
        WVLOG_E("hks init invoke failed, error code: %d", ohResult);
        return std::string();
    }
    ohResult = HksFinish(&handleDecrypt, decryptParamSet, &cipherText, &plainText);
    HksFreeParamSet(&genParamSet);
    HksFreeParamSet(&decryptParamSet);
    if (ohResult != HKS_SUCCESS) {
        WVLOG_E("hks finish invoke failed, error code: %d", ohResult);
        return std::string();
    }
    return std::string(reinterpret_cast<char*>(plainText.data), plainText.size);
}

std::string KeystoreAdapterImpl::AssetQuery(const std::string assetHandle)
{
    const char* ALIAS = assetHandle.c_str();
    Asset_Blob alias = { (uint32_t)(strlen(ALIAS)), (uint8_t*)ALIAS };
    Asset_Attr attr[] = {
        { .tag = ASSET_TAG_ALIAS, .value.blob = alias },
        { .tag = ASSET_TAG_RETURN_TYPE, .value.u32 = ASSET_RETURN_ALL },
    };

    Asset_ResultSet resultSet = { 0 };
    int32_t ret = OH_Asset_Query(attr, sizeof(attr) / sizeof(attr[0]), &resultSet);
    if (ret == ASSET_SUCCESS) {
        Asset_Attr* secret = OH_Asset_ParseAttr(resultSet.results, ASSET_TAG_SECRET);
        if (secret) {
            Asset_Blob valueBlob = secret->value.blob;
            std::string localKey(reinterpret_cast<char*>(valueBlob.data), valueBlob.size);
            OH_Asset_FreeResultSet(&resultSet);
            WVLOG_I("get key from asset success.");
            return localKey;
        }
    }
    WVLOG_E("hks finish invoke, query ret: %d", ret);
    return std::string();
}

} // namespace OHOS::NWeb
