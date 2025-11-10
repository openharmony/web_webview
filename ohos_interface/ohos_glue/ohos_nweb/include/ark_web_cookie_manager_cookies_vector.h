/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#ifndef ARK_WEB_COOKIE_MANAGER_COOKIES_VECTOR_H_
#define ARK_WEB_COOKIE_MANAGER_COOKIES_VECTOR_H_
#pragma once
 
#include "ohos_nweb/capi/ark_web_cookie_manager_cookies_capi.h"
 
#include "base/include/ark_web_memory.h"
 
typedef struct {
    int size;
    ark_web_cookie_manager_cookies_t** value;
 
    ArkWebMemFreeFunc ark_web_mem_free_func;
} ArkWebCookieManagerCookiesVector;
 
const ArkWebCookieManagerCookiesVector ark_web_cookie_manager_cookies_vector_default = {
    0,
};
 
#endif // ARK_WEB_COOKIE_MANAGER_COOKIES_VECTOR_H_
