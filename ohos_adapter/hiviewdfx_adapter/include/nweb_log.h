/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef NWEB_HILOG_H
#define NWEB_HILOG_H

#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <hilog_adapter.h>

#define BROWSER_UID_BASE  20000000
#define LOG_APP_DOMAIN    0xD004500
#define LOG_RENDER_DOMAIN 0xD004501
#ifndef HILOG_TAG
#define HILOG_TAG         "webview"
#endif
#define FILE_NAME (__builtin_strrchr("/" __FILE__, '/') + 1)
#define FUNC_LINE_FMT "[%{public}s:%{public}d] "

#define PRINT_MOCK_LOG() do {                                                   \
    HiLogAdapter::PrintLog(LogLevelAdapter::ERROR, HILOG_TAG,                   \
                           "mock_function_called %{public}s : %{public}s",      \
                           FILE_NAME, __FUNCTION__);                            \
} while (0)

#define WVLOG_D(fmt, ...)                                                        \
    HiLogAdapter::PrintLog(LogLevelAdapter::DEBUG, HILOG_TAG, FUNC_LINE_FMT fmt, \
                           FILE_NAME, __LINE__, ##__VA_ARGS__)

#define WVLOG_I(fmt, ...)                                                        \
    HiLogAdapter::PrintLog(LogLevelAdapter::INFO, HILOG_TAG, FUNC_LINE_FMT fmt, \
                           FILE_NAME, __LINE__, ##__VA_ARGS__)

#define WVLOG_W(fmt, ...)                                                        \
    HiLogAdapter::PrintLog(LogLevelAdapter::WARN, HILOG_TAG, FUNC_LINE_FMT fmt, \
                           FILE_NAME, __LINE__, ##__VA_ARGS__)

#define WVLOG_E(fmt, ...)                                                        \
    HiLogAdapter::PrintLog(LogLevelAdapter::ERROR, HILOG_TAG, FUNC_LINE_FMT fmt, \
                           FILE_NAME, __LINE__, ##__VA_ARGS__)

#define WVLOG_F(fmt, ...)                                                        \
    HiLogAdapter::PrintLog(LogLevelAdapter::FATAL, HILOG_TAG, FUNC_LINE_FMT fmt, \
                           FILE_NAME, __LINE__, ##__VA_ARGS__)

#endif // NWEB_HILOG_H
