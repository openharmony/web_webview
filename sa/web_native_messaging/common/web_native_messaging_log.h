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

#ifndef WEB_NATIVE_MESSAGING_LOG_H
#define WEB_NATIVE_MESSAGING_LOG_H

#include <cstdio>
#include <sys/types.h>
#include <hilog/log_c.h>

#define LOG_WEB_NATIVE_MESSAGING_DOMAIN 0xD004500
#define WNM_HILOG_TAG  "webNativeMessaging"
#define WNM_FILE_NAME (__builtin_strrchr("/" __FILE__, '/') + 1)
#define WNM_FUNC_LINE_FMT "[%{public}s:%{public}d] "

#define WNMLOG_D(fmt, ...) do {                                                   \
    uint32_t domain = LOG_WEB_NATIVE_MESSAGING_DOMAIN;                            \
    HILOG_IMPL(LOG_CORE, LOG_DEBUG, domain, WNM_HILOG_TAG, WNM_FUNC_LINE_FMT fmt, \
               WNM_FILE_NAME, __LINE__, ##__VA_ARGS__);                           \
} while (0)

#define WNMLOG_I(fmt, ...) do {                                                   \
    uint32_t domain = LOG_WEB_NATIVE_MESSAGING_DOMAIN;                            \
    HILOG_IMPL(LOG_CORE, LOG_INFO, domain, WNM_HILOG_TAG, WNM_FUNC_LINE_FMT fmt,  \
               WNM_FILE_NAME, __LINE__, ##__VA_ARGS__);                           \
} while (0)

#define WNMLOG_W(fmt, ...) do {                                                   \
    uint32_t domain = LOG_WEB_NATIVE_MESSAGING_DOMAIN;                            \
    HILOG_IMPL(LOG_CORE, LOG_WARN, domain, WNM_HILOG_TAG, WNM_FUNC_LINE_FMT fmt,  \
               WNM_FILE_NAME, __LINE__, ##__VA_ARGS__);                           \
} while (0)

#define WNMLOG_E(fmt, ...) do {                                                   \
    uint32_t domain = LOG_WEB_NATIVE_MESSAGING_DOMAIN;                            \
    HILOG_IMPL(LOG_CORE, LOG_ERROR, domain, WNM_HILOG_TAG, WNM_FUNC_LINE_FMT fmt, \
               WNM_FILE_NAME, __LINE__, ##__VA_ARGS__);                           \
} while (0)

#define WNMLOG_F(fmt, ...) do {                                                   \
    uint32_t domain = LOG_WEB_NATIVE_MESSAGING_DOMAIN;                            \
    HILOG_IMPL(LOG_CORE, LOG_FATAL, domain, WNM_HILOG_TAG, WNM_FUNC_LINE_FMT fmt, \
               WNM_FILE_NAME, __LINE__, ##__VA_ARGS__);                           \
} while (0)

#endif // WEB_NATIVE_MESSAGING_LOG_H

