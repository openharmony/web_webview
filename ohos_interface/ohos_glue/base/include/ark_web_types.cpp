/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "base/include/ark_web_types.h"

#include "base/include/ark_web_log_utils.h"

ArkWebU16String ArkWebU16StringClassToStruct(const std::u16string& class_value)
{
    ArkWebU16String struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (char16_t*)ArkWebMemMalloc((struct_value.size + 1) * sizeof(char16_t));
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        memcpy((char*)struct_value.value, (char*)class_value.c_str(), struct_value.size * sizeof(char16_t));
        struct_value.value[struct_value.size] = 0;
    }

    ARK_WEB_BASE_DV_LOG("length is %{public}d", struct_value.size);

    return struct_value;
}

std::u16string ArkWebU16StringStructToClass(const ArkWebU16String& struct_value)
{
    std::u16string class_value;
    if (struct_value.size > 0) {
        class_value = struct_value.value;
    }

    ARK_WEB_BASE_DV_LOG("length is %{public}d", struct_value.size);

    return class_value;
}

ArkWebString ArkWebStringClassToStruct(const std::string& class_value)
{
    ArkWebString struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (char*)ArkWebMemMalloc(struct_value.size + 1);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        memcpy(struct_value.value, class_value.c_str(), struct_value.size);
        struct_value.value[struct_value.size] = 0;
    }

    ARK_WEB_BASE_DV_LOG("string is %{public}s,length is %{public}d", class_value.c_str(), struct_value.size);

    return struct_value;
}

std::string ArkWebStringStructToClass(const ArkWebString& struct_value)
{
    std::string class_value;
    if (struct_value.size > 0) {
        class_value.assign(struct_value.value, struct_value.size);
    }

    ARK_WEB_BASE_DV_LOG("string is %{public}s,length is %{public}d", class_value.c_str(), struct_value.size);

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebStringStructRelease(ArkWebString& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ARK_WEB_NO_SANITIZE void ArkWebU16StringStructRelease(ArkWebU16String& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebStringMap ArkWebStringMapClassToStruct(const std::map<std::string, std::string>& class_value)
{
    ArkWebStringMap struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.key = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        struct_value.value = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        if (struct_value.key == nullptr || struct_value.value == nullptr) {
            ArkWebMemFree(struct_value.key);
            ArkWebMemFree(struct_value.value);
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.key[count] = ArkWebStringClassToStruct(it->first);
            struct_value.value[count] = ArkWebStringClassToStruct(it->second);
            count++;
        }
    }

    return struct_value;
}

std::map<std::string, std::string> ArkWebStringMapStructToClass(const ArkWebStringMap& struct_value)
{
    std::map<std::string, std::string> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.insert(std::map<std::string, std::string>::value_type(
            ArkWebStringStructToClass(struct_value.key[count]), ArkWebStringStructToClass(struct_value.value[count])));
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebStringMapStructRelease(ArkWebStringMap& struct_value)
{
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.key[count]);
        ArkWebStringStructRelease(struct_value.value[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebStringList ArkWebStringListClassToStruct(const std::list<std::string>& class_value)
{
    ArkWebStringList struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.value[count] = ArkWebStringClassToStruct(*it);
            count++;
        }
    }

    return struct_value;
}

ARK_WEB_NO_SANITIZE std::list<std::string> ArkWebStringListStructToClass(const ArkWebStringList& struct_value)
{
    std::list<std::string> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.push_back(ArkWebStringStructToClass(struct_value.value[count]));
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebStringListStructRelease(ArkWebStringList& struct_value)
{
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.value[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebStringVector ArkWebStringVectorClassToStruct(const std::vector<std::string>& class_value)
{
    ArkWebStringVector struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.value[count] = ArkWebStringClassToStruct(*it);
            count++;
        }
    }

    return struct_value;
}

std::vector<std::string> ArkWebStringVectorStructToClass(const ArkWebStringVector& struct_value)
{
    std::vector<std::string> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.push_back(ArkWebStringStructToClass(struct_value.value[count]));
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebStringVectorStructRelease(ArkWebStringVector& struct_value)
{
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.value[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebStringVectorMap ArkWebStringVectorMapClassToStruct(
    const std::map<std::string, std::vector<std::string>>& class_value)
{
    ArkWebStringVectorMap struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.key = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        struct_value.value = (ArkWebStringVector*)ArkWebMemMalloc(sizeof(ArkWebStringVector) * struct_value.size);
        if (struct_value.key == nullptr || struct_value.value == nullptr) {
            ArkWebMemFree(struct_value.key);
            ArkWebMemFree(struct_value.value);
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.key[count] = ArkWebStringClassToStruct(it->first);
            struct_value.value[count] = ArkWebStringVectorClassToStruct(it->second);
            count++;
        }
    }

    return struct_value;
}

std::map<std::string, std::vector<std::string>> ArkWebStringVectorMapStructToClass(
    const ArkWebStringVectorMap& struct_value)
{
    std::map<std::string, std::vector<std::string>> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.insert(std::map<std::string, std::vector<std::string>>::value_type(
            ArkWebStringStructToClass(struct_value.key[count]),
            ArkWebStringVectorStructToClass(struct_value.value[count])));
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebStringVectorMapStructRelease(ArkWebStringVectorMap& struct_value)
{
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.key[count]);
        ArkWebStringVectorStructRelease(struct_value.value[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebUInt8VectorMap ArkWebUInt8VectorMapClassToStruct(
    const std::map<std::string, std::vector<uint8_t>>& class_value)
{
    ArkWebUInt8VectorMap struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.key = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        struct_value.value = (ArkWebUint8Vector*)ArkWebMemMalloc(sizeof(ArkWebUint8Vector) * struct_value.size);
        if (struct_value.key == nullptr || struct_value.value == nullptr) {
            ArkWebMemFree(struct_value.key);
            ArkWebMemFree(struct_value.value);
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.key[count] = ArkWebStringClassToStruct(it->first);
            struct_value.value[count] = ArkWebBasicVectorClassToStruct<uint8_t, ArkWebUint8Vector>(it->second);
            count++;
        }
    }

    return struct_value;
}

std::map<std::string, std::vector<uint8_t>> ArkWebUInt8VectorMapStructToClass(const ArkWebUInt8VectorMap& struct_value)
{
    std::map<std::string, std::vector<uint8_t>> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.insert(std::map<std::string, std::vector<uint8_t>>::value_type(
            ArkWebStringStructToClass(struct_value.key[count]),
            ArkWebBasicVectorStructToClass<uint8_t, ArkWebUint8Vector>(struct_value.value[count])));
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebUInt8VectorMapStructRelease(ArkWebUInt8VectorMap& struct_value)
{
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.key[count]);
        ArkWebBasicVectorStructRelease(struct_value.value[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebPairStringVector ArkWebPairStringVectorClassToStruct(
    const std::vector<std::pair<std::string, std::string>>& class_value)
{
    ArkWebPairStringVector struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.first = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        struct_value.second = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        if (struct_value.first == nullptr || struct_value.second == nullptr) {
            ArkWebMemFree(struct_value.first);
            ArkWebMemFree(struct_value.second);
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (const auto& pair : class_value) {
            struct_value.first[count] = ArkWebStringClassToStruct(pair.first);
            struct_value.second[count] = ArkWebStringClassToStruct(pair.second);
            count++;
        }
    }

    return struct_value;
}

std::vector<std::pair<std::string, std::string>> ArkWebPairStringVectorStructToClass(
    const ArkWebPairStringVector& struct_value) 
{
    std::vector<std::pair<std::string, std::string>> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.push_back(std::make_pair(
            ArkWebStringStructToClass(struct_value.first[count]),
            ArkWebStringStructToClass(struct_value.second[count])));
    }

    return class_value;
}

ArkWebPairStringVectorMap ArkWebPairStringVectorMapClassToStruct(
    const std::map<std::string, std::vector<std::pair<std::string, std::string>>>& class_value)
{
    ArkWebPairStringVectorMap struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.key = (ArkWebString*)ArkWebMemMalloc(sizeof(ArkWebString) * struct_value.size);
        struct_value.value = (ArkWebPairStringVector*)ArkWebMemMalloc(
            sizeof(ArkWebPairStringVector) * struct_value.size);
        if (struct_value.key == nullptr || struct_value.value == nullptr) {
            ArkWebMemFree(struct_value.key);
            ArkWebMemFree(struct_value.value);
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (const auto& [key, value] : class_value) {
            struct_value.key[count] = ArkWebStringClassToStruct(key);
            struct_value.value[count] = ArkWebPairStringVectorClassToStruct(value);
            count++;
        }
    }

    return struct_value;
}

std::map<std::string, std::vector<std::pair<std::string, std::string>>> ArkWebPairStringVectorMapStructToClass(
    const ArkWebPairStringVectorMap& struct_value)
{
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> class_value;
    for (int count = 0; count < struct_value.size; count++) {
        class_value.insert(std::map<std::string, std::vector<std::pair<std::string, std::string>>>::value_type(
            ArkWebStringStructToClass(struct_value.key[count]),
            ArkWebPairStringVectorStructToClass(struct_value.value[count])));
    }

    return class_value;
}

ARK_WEB_NO_SANITIZE void ArkWebPairStringVectorStructRelease(ArkWebPairStringVector& struct_value) {
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.first[count]);
        ArkWebStringStructRelease(struct_value.second[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.first, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.second, struct_value.ark_web_mem_free_func);
}

ARK_WEB_NO_SANITIZE void ArkWebPairStringVectorMapStructRelease(ArkWebPairStringVectorMap& struct_value) {
    for (int count = 0; count < struct_value.size; count++) {
        ArkWebStringStructRelease(struct_value.key[count]);
        ArkWebPairStringVectorStructRelease(struct_value.value[count]);
    }

    struct_value.size = 0;
    SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}
