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

#ifndef ARK_WEB_BASE_REF_COUNTED_CAPI_H_
#define ARK_WEB_BASE_REF_COUNTED_CAPI_H_
#pragma once

#include <mutex>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include "base/include/ark_web_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

///
// All ref-counted framework structures must include this structure first.
///
typedef struct _ark_web_base_ref_counted_t {
    /**
     * @brief Size of the data structure.
     */
    size_t size;

    /**
     * @brief The reference count increment 1. Should be called for every new copy
     *        of a pointer to a given object.
     */
    void(ARK_WEB_CALLBACK* incre_ref)(struct _ark_web_base_ref_counted_t* self);

    /**
     * @brief The reference count decrement 1 and delete the object when the
     *        reference count is 0
     */
    void(ARK_WEB_CALLBACK* decre_ref)(struct _ark_web_base_ref_counted_t* self);
} ark_web_base_ref_counted_t;

/**
 * @brief Check that the structure |s|, which is defined with a size_t member at
 *        the top, is large enough to contain the specified member |m|.
 */
#define ARK_WEB_MEMBER_EXISTS(s, m) \
    ((intptr_t) & ((s)->m) - (intptr_t)(s) + sizeof((s)->m) <= *reinterpret_cast<size_t*>(s))

#define ARK_WEB_FUNC_MEMBER_MISSING(s, f) (!ARK_WEB_MEMBER_EXISTS(s, f) || !((s)->f))

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_BASE_REF_COUNTED_CAPI_H_
