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

#ifndef ARK_WEB_CTOCPP_MACROS_H_
#define ARK_WEB_CTOCPP_MACROS_H_
#pragma once

#include "base/include/ark_web_errno.h"
#include "base/include/ark_web_log_utils.h"

#define ARK_WEB_CTOCPP_DV_LOG(fmt, ...) ARK_WEB_BASE_DV_LOG(fmt, ##__VA_ARGS__)

#define ARK_WEB_CTOCPP_REF_LOG(fmt, ...) ARK_WEB_BASE_REF_LOG(fmt, ##__VA_ARGS__)

#define ARK_WEB_CTOCPP_DEBUG_LOG(fmt, ...) ARK_WEB_BASE_DEBUG_LOG(fmt, ##__VA_ARGS__)

#define ARK_WEB_CTOCPP_INFO_LOG(fmt, ...) ARK_WEB_BASE_INFO_LOG(fmt, ##__VA_ARGS__)

#define ARK_WEB_CTOCPP_WARN_LOG(fmt, ...) ARK_WEB_BASE_WARN_LOG(fmt, ##__VA_ARGS__)

#define ARK_WEB_CTOCPP_ERROR_LOG(fmt, ...) ARK_WEB_BASE_ERROR_LOG(fmt, ##__VA_ARGS__)

#define ARK_WEB_CTOCPP_CHECK_PARAM(param, result)     \
    {                                                 \
        ArkWebSetErrno(RESULT_OK);                    \
        if (!(param)) {                               \
            ArkWebSetErrno(RESULT_CTOCPP_ERROR);      \
            ARK_WEB_CTOCPP_WARN_LOG("param is null"); \
            return result;                            \
        }                                             \
    }

#define ARK_WEB_CTOCPP_CHECK_FUNC_POINTER(pointer, result)                               \
    {                                                                                    \
        ArkWebSetErrno(RESULT_OK);                                                       \
        if (!(pointer)) {                                                                \
            ArkWebSetErrno(RESULT_NOT_IMPL);                                             \
            ARK_WEB_CTOCPP_INFO_LOG("function %{public}s isn't existing", __FUNCTION__); \
            return result;                                                               \
        }                                                                                \
    }

#endif // ARK_WEB_CTOCPP_MACROS_H_
