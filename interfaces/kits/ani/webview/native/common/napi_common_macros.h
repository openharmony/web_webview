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

#ifndef NAPI_COMMON_NACROS_H
#define NAPI_COMMON_NACROS_H

#define NAPI_GET_CALLBACK_RETURN_VOID(env, value, name, callback) \
    do {                                                          \
        napi_get_named_property(env, value, name, &(callback));   \
        napi_valuetype valueType = napi_undefined;                \
        napi_typeof(env, callback, &valueType);                   \
        if (valueType != napi_function) {                         \
            WVLOG_I("failed to get callback %{public}s", name);   \
            return;                                               \
        }                                                         \
    } while (0)

#endif // NAPI_COMMON_NACROS_H
