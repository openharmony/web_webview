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

#ifndef ARK_WEB_JS_PROXY_CALLBACK_CAPI_H_
#define ARK_WEB_JS_PROXY_CALLBACK_CAPI_H_
#pragma once

#include "ohos_nweb/include/ark_web_nweb_structs.h"

#include "base/capi/ark_web_base_ref_counted_capi.h"
#include "base/include/ark_web_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ark_web_js_proxy_callback_t {
    /**
     * @brief Base structure.
     */
    ark_web_base_ref_counted_t base;

    ArkWebString(ARK_WEB_CALLBACK* get_method_name)(struct _ark_web_js_proxy_callback_t* self);

    NativeArkWebOnJavaScriptProxyCallback(ARK_WEB_CALLBACK* get_method_callback)(
        struct _ark_web_js_proxy_callback_t* self);
} ark_web_js_proxy_callback_t;

#ifdef __cplusplus
}
#endif

#endif // ARK_WEB_JS_PROXY_CALLBACK_CAPI_H_