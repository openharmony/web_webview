/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef TEST_MOCK_ANI_H
#define TEST_MOCK_ANI_H

#include <cstdarg>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>

using ani_status = int;
using ani_boolean = uint8_t;
using ani_int = int32_t;
using ani_long = long;
using ani_size = size_t;
using ani_double = double;
using ani_char = uint16_t;
using ani_byte = int8_t;
using ani_short = int16_t;
using ani_float = float;

using ani_ref = void*;
using ani_object = void*;
using ani_class = void*;
using ani_method = void*;
using ani_field = void*;
using ani_resolver = void*;
using ani_error = void*;
using ani_string = void*;
using ani_static_field = void*;
using ani_static_method = void*;
using ani_namespace = void*;
using ani_module = void*;
using ani_enum = void*;
using ani_enum_item = void*;
using ani_arraybuffer = void*;
using ani_vm = void*;

#define ANI_OK 0
#define ANI_ERROR (-1)
#define ANI_TRUE 1
#define ANI_FALSE 0
constexpr ani_status ANI_ALREADY_BINDED = 1;

struct AniNativeFunction {
    const char* name;
    const char* signature;
    void* fn;
};

inline ani_status g_findClassRet = ANI_OK;
inline ani_status g_findMethodRet = ANI_OK;
inline ani_status g_findFieldRet = ANI_OK;
inline ani_status g_objectNewRet = ANI_OK;
inline bool g_objectNewValid = true;
inline ani_status g_bindMethodsRet = ANI_OK;
inline std::vector<ani_status> g_bindMethodsSeq;
inline ani_status g_getFieldLongRet = ANI_OK;
inline ani_long g_nativeContextFieldValue = 0;
inline ani_status g_setPropertyRefRet = ANI_OK;
inline ani_status g_callMethodVoidRet = ANI_OK;
inline ani_status g_getPropertyRefRet = ANI_OK;
inline ani_status g_promiseNewRet = ANI_OK;
inline ani_status g_promiseRejectRet = ANI_OK;
inline ani_status g_promiseResolveRet = ANI_OK;
inline ani_status g_globalRefCreateRet = ANI_OK;
inline int g_findClassCount = 0;
inline int g_bindMethodsCount = 0;
inline int g_findMethodCount = 0;
inline int g_objectNewCount = 0;
inline int g_getFieldLongCount = 0;

void ResetAniMockState()
{
    g_findClassRet = ANI_OK;
    g_findMethodRet = ANI_OK;
    g_findFieldRet = ANI_OK;
    g_objectNewRet = ANI_OK;
    g_objectNewValid = true;
    g_bindMethodsRet = ANI_OK;
    g_bindMethodsSeq.clear();
    g_getFieldLongRet = ANI_OK;
    g_nativeContextFieldValue = 0;
    g_setPropertyRefRet = ANI_OK;
    g_callMethodVoidRet = ANI_OK;
    g_getPropertyRefRet = ANI_OK;
    g_promiseNewRet = ANI_OK;
    g_promiseRejectRet = ANI_OK;
    g_promiseResolveRet = ANI_OK;
    g_globalRefCreateRet = ANI_OK;
    g_findClassCount = 0;
    g_bindMethodsCount = 0;
    g_findMethodCount = 0;
    g_objectNewCount = 0;
    g_getFieldLongCount = 0;
}

struct AniEnv {
    ani_status FindClass(const char*, ani_class* r)
    {
        g_findClassCount++;
        if (g_findClassRet == ANI_OK) { *r = reinterpret_cast<ani_class>(this); }
        return g_findClassRet;
    }
    ani_status ClassBindNativeMethods(ani_class, const AniNativeFunction*, ani_size)
    {
        g_bindMethodsCount++;
        if (!g_bindMethodsSeq.empty()) {
            ani_status s = g_bindMethodsSeq.front();
            g_bindMethodsSeq.erase(g_bindMethodsSeq.begin());
            return s;
        }
        return g_bindMethodsRet;
    }
    ani_status ClassFindMethod(ani_class, const char*, const char*, ani_method* r)
    {
        g_findMethodCount++;
        if (g_findMethodRet == ANI_OK) { *r = reinterpret_cast<ani_method>(this); }
        return g_findMethodRet;
    }
    ani_status ClassFindField(ani_class, const char*, ani_field* r)
    {
        if (g_findFieldRet == ANI_OK) { *r = reinterpret_cast<ani_field>(this); }
        return g_findFieldRet;
    }
    ani_status ObjectNew(ani_class, ani_method, ani_object* r, ...)
    {
        g_objectNewCount++;
        va_list ap;
        va_start(ap, r);
        (void)va_arg(ap, ani_long);
        va_end(ap);
        if (g_objectNewRet == ANI_OK && g_objectNewValid) { *r = reinterpret_cast<ani_object>(this); }
        return g_objectNewRet;
    }
    ani_status ObjectGetFieldLong(ani_object, ani_field, ani_long* v)
    {
        g_getFieldLongCount++;
        if (g_getFieldLongRet == ANI_OK) { *v = g_nativeContextFieldValue; }
        return g_getFieldLongRet;
    }
    ani_status ObjectGetFieldByNameLong(ani_object, const char*, ani_long* v)
    {
        g_getFieldLongCount++;
        if (g_getFieldLongRet == ANI_OK) { *v = g_nativeContextFieldValue; }
        return g_getFieldLongRet;
    }
    ani_status ObjectSetPropertyByNameRef(ani_object, const char*, ani_ref) { return g_setPropertyRefRet; }
    ani_status ObjectCallMethodVoid(ani_object, ani_method, ...) { return g_callMethodVoidRet; }
    ani_status ObjectGetPropertyByNameRef(ani_object, const char*, ani_ref* r)
    {
        if (g_getPropertyRefRet == ANI_OK) { *r = reinterpret_cast<ani_ref>(this); }
        return g_getPropertyRefRet;
    }
    ani_status PromiseNew(ani_resolver* res, ani_object* prom)
    {
        if (g_promiseNewRet == ANI_OK) {
            *res = reinterpret_cast<ani_resolver>(this);
            *prom = reinterpret_cast<ani_object>(this);
        }
        return g_promiseNewRet;
    }
    ani_status PromiseResolverReject(ani_resolver, ani_error) { return g_promiseRejectRet; }
    ani_status PromiseResolverResolve(ani_resolver, ani_object) { return g_promiseResolveRet; }
    ani_status GlobalReferenceCreate(ani_ref, ani_ref* r)
    {
        if (g_globalRefCreateRet == ANI_OK) { *r = reinterpret_cast<ani_ref>(this); }
        return g_globalRefCreateRet;
    }
};

#endif // TEST_MOCK_ANI_H
