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

#ifndef ARK_WEB_LOG_UTILS_H_
#define ARK_WEB_LOG_UTILS_H_
#pragma once

#include "include/nweb_log.h"

#define LOG_FMT_PREFIX "[webview]: "

#if defined(OHOS_GLUE_DEBUG)
#define ARK_WEB_BASE_DV_LOG(fmt, ...)                                          \
  OHOS::WVLOG_I(LOG_FMT_PREFIX fmt, ##__VA_ARGS__)
#else
#define ARK_WEB_BASE_DV_LOG(fmt, ...)
#endif

#define ARK_WEB_BASE_REF_LOG(fmt, ...)

#define ARK_WEB_BASE_DEBUG_LOG(fmt, ...)                                       \
  OHOS::WVLOG_D(LOG_FMT_PREFIX fmt, ##__VA_ARGS__)

#define ARK_WEB_BASE_INFO_LOG(fmt, ...)                                        \
  OHOS::WVLOG_I(LOG_FMT_PREFIX fmt, ##__VA_ARGS__)

#define ARK_WEB_BASE_WARN_LOG(fmt, ...)                                        \
  OHOS::WVLOG_W(LOG_FMT_PREFIX fmt, ##__VA_ARGS__)

#define ARK_WEB_BASE_ERROR_LOG(fmt, ...)                                       \
  OHOS::WVLOG_E(LOG_FMT_PREFIX fmt, ##__VA_ARGS__)

#endif // ARK_WEB_LOG_UTILS_H_