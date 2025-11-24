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

#ifndef ARK_WEB_TYPES_H_
#define ARK_WEB_TYPES_H_
#pragma once

#include <list>
#include <map>
#include <math.h>
#include <string>
#include <vector>

#include "base/include/ark_web_macros.h"
#include "base/include/ark_web_memory.h"
#include "base/include/ark_web_scoped_refptr.h"

template<class T>
using ArkWebRefPtr = ark_web_scoped_refptr<T>;

typedef struct {
    int size;
    char* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebCharVector;

const ArkWebCharVector ark_web_char_vector_default = {
    0,
};

typedef struct {
    int size;
    uint8_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebUint8Vector;

const ArkWebUint8Vector ark_web_uint8_vector_default = {
    0,
};

typedef struct {
    int size;
    uint16_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebUint16Vector;

const ArkWebUint16Vector ark_web_uint16_vector_default = {
    0,
};

typedef struct {
    int size;
    int32_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebInt32Vector;

const ArkWebInt32Vector ark_web_int32_vector_default = {
    0,
};

typedef struct {
    int size;
    int64_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebInt64Vector;

const ArkWebInt64Vector ark_web_int64_vector_default = {
    0,
};

typedef struct {
    int size;
    uint32_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebUint32Vector;

const ArkWebUint32Vector ark_web_uint32_vector_default = {
    0,
};

typedef struct {
    int size;
    double* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebDoubleVector;

const ArkWebDoubleVector ark_web_double_vector_default = {
    0,
};

typedef struct {
    int size;
    bool* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebBooleanVector;

const ArkWebBooleanVector ark_web_boolean_vector_default = {
    0,
};

typedef struct {
    int size;
    int32_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebInt32List;

const ArkWebInt32List ark_web_int32_list_default = {
    0,
};

typedef struct {
    int size;
    char* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebString;

const ArkWebString ark_web_string_default = {
    0,
};

typedef struct {
    int size;
    char16_t* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebU16String;

const ArkWebU16String ark_web_u16string_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* key;
    ArkWebString* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebStringMap;

const ArkWebStringMap ark_web_string_map_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebStringList;

const ArkWebStringList ark_web_string_list_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebStringVector;

const ArkWebStringVector ark_web_string_vector_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* key;
    ArkWebStringVector* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebStringVectorMap;

const ArkWebStringVectorMap ark_web_string_vector_map_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* key;
    ArkWebUint8Vector* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebUInt8VectorMap;

const ArkWebUInt8VectorMap ark_web_uint8_vector_map_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* first;
    ArkWebString* second;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebPairStringVector;

const ArkWebPairStringVector ark_web_pair_string_vector_default = {
    0,
};

typedef struct {
    int size;
    ArkWebString* key;
    ArkWebPairStringVector* value;

    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebPairStringVectorMap;

const ArkWebPairStringVectorMap ark_web_pair_string_vector_map_default = {
    0,
};

template<typename T1, typename T2, typename R>
R ArkWebBasicMapClassToStruct(const std::map<T1, T2>& class_value)
{
    R struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.key = (T1*)ArkWebMemMalloc(sizeof(T1) * struct_value.size);
        struct_value.value = (T2*)ArkWebMemMalloc(sizeof(T2) * struct_value.size);
        if (struct_value.key == nullptr || struct_value.value == nullptr) {
            ArkWebMemFree(struct_value.key);
            ArkWebMemFree(struct_value.value);
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.key[count] = it->first;
            struct_value.value[count] = it->second;
            count++;
        }
    }

    return struct_value;
}

template<typename T1, typename T2, typename P>
std::map<T1, T2> ArkWebBasicMapStructToClass(const P& struct_value)
{
    std::map<T1, T2> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            class_value.insert(std::map<T1, T2>::value_type(struct_value.key[count], struct_value.value[count]));
        }
    }

    return class_value;
}

template<typename P>
ARK_WEB_NO_SANITIZE void ArkWebBasicMapStructRelease(P& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.key, struct_value.ark_web_mem_free_func);
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

template<typename T, typename R>
R ArkWebBasicListClassToStruct(const std::list<T>& class_value)
{
    R struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (T*)ArkWebMemMalloc(sizeof(T) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.value[count] = *it;
            count++;
        }
    }

    return struct_value;
}

template<typename T, typename P>
std::list<T> ArkWebBasicListStructToClass(const P& struct_value)
{
    std::list<T> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            class_value.push_back(struct_value.value[count]);
        }
    }

    return class_value;
}

template<typename P>
ARK_WEB_NO_SANITIZE void ArkWebBasicListStructRelease(P& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

template<typename T, typename R>
R ArkWebBasicVectorClassToStruct(const std::vector<T>& class_value)
{
    R struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (T*)ArkWebMemMalloc(sizeof(T) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            struct_value.value[count] = *it;
            count++;
        }
    }

    return struct_value;
}

template<typename T, typename P>
std::vector<T> ArkWebBasicVectorStructToClass(const P& struct_value)
{
    std::vector<T> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            class_value.push_back(struct_value.value[count]);
        }
    }

    return class_value;
}

template<typename P>
ARK_WEB_NO_SANITIZE void ArkWebBasicVectorStructRelease(P& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

ArkWebString ArkWebStringClassToStruct(const std::string& class_value);

std::string ArkWebStringStructToClass(const ArkWebString& struct_value);

ArkWebU16String ArkWebU16StringClassToStruct(const std::u16string& class_value);

std::u16string ArkWebU16StringStructToClass(const ArkWebU16String& struct_value);

void ArkWebStringStructRelease(ArkWebString& struct_value);

void ArkWebU16StringStructRelease(ArkWebU16String& struct_value);

ArkWebStringMap ArkWebStringMapClassToStruct(const std::map<std::string, std::string>& class_value);

std::map<std::string, std::string> ArkWebStringMapStructToClass(const ArkWebStringMap& struct_value);

void ArkWebStringMapStructRelease(ArkWebStringMap& struct_value);

ArkWebStringList ArkWebStringListClassToStruct(const std::list<std::string>& class_value);

std::list<std::string> ArkWebStringListStructToClass(const ArkWebStringList& struct_value);

void ArkWebStringListStructRelease(ArkWebStringList& struct_value);

ArkWebStringVector ArkWebStringVectorClassToStruct(const std::vector<std::string>& class_value);

std::vector<std::string> ArkWebStringVectorStructToClass(const ArkWebStringVector& struct_value);

void ArkWebStringVectorStructRelease(ArkWebStringVector& struct_value);

ArkWebStringVectorMap ArkWebStringVectorMapClassToStruct(
    const std::map<std::string, std::vector<std::string>>& class_value);

std::map<std::string, std::vector<std::string>> ArkWebStringVectorMapStructToClass(
    const ArkWebStringVectorMap& struct_value);

void ArkWebStringVectorMapStructRelease(ArkWebStringVectorMap& struct_value);

ArkWebUInt8VectorMap ArkWebUInt8VectorMapClassToStruct(
    const std::map<std::string, std::vector<uint8_t>>& class_value);

std::map<std::string, std::vector<uint8_t>> ArkWebUInt8VectorMapStructToClass(
    const ArkWebUInt8VectorMap& struct_value);

void ArkWebUInt8VectorMapStructRelease(ArkWebUInt8VectorMap& struct_value);

#endif // ARK_WEB_TYPES_H_