/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_NAPI_SCOPE_H
#define NWEB_NAPI_SCOPE_H

#include <js_native_api.h>
#include "napi/native_api.h"

namespace OHOS {
struct NApiScope {
    NApiScope(napi_env env) : env_(env)
    {
        // Enable the napi_handle_scope to manage the life cycle of the napi_value.
        // Otherwise, memory leakage occurs.
        napi_open_handle_scope(env_, &scope_);
    }

    ~NApiScope()
    {
        if (scope_) {
            napi_close_handle_scope(env_, scope_);
        }
    }

    napi_env env_;
    napi_handle_scope scope_ = nullptr;
};
}

#endif // NWEB_NAPI_SCOPE_H
