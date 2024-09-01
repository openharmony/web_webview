/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HILOG_ADAPTER_H
#define HILOG_ADAPTER_H

#include <cstdint>

namespace OHOS::NWeb {

enum class LogLevelAdapter : uint32_t {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    LEVEL_MAX,
};

class __attribute__((visibility("default"))) HiLogAdapter {
public:
    static int PrintLog(LogLevelAdapter level, const char* tag, const char* fmt, ...);

    static int PrintConsoleLog(LogLevelAdapter level, const char* tag, const char* fmt, ...);
};

} // namespace OHOS::NWeb

#endif // HILOG_ADAPTER_H
