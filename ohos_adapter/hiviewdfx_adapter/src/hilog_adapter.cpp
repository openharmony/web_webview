/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "hilog_adapter.h"
#include <unistd.h>
#include <sys/types.h>
#include <hilog/log.h>
#include <string>
#include <cstdio>

namespace OHOS::NWeb {
namespace {
constexpr uint32_t BROWSER_UID_BASE = 20000000;
constexpr uint32_t LOG_APP_DOMAIN = 0xD004500;
constexpr uint32_t LOG_RENDER_DOMAIN = 0xD004501;
constexpr uint32_t LOG_CONSOLE_DOMAIN = 0x001194;
constexpr uint32_t MAX_LENGTH = 1024;
constexpr uint32_t FAIL = -1;
const std::string PUBLIC_STR = "{public}";
const std::string PRIVATE_STR = "{private}";
const std::string STD_FORMAT = "%{public}s";

const ::LogLevel LOG_LEVELS[] = {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};
} // namespace

extern "C" {
int HiLogPrintArgs(LogType type, LogLevel level, unsigned int domain, const char* tag, const char* fmt, va_list ap);

void Format(std::string& fmtStr)
{
    size_t pos = 0;
    while (std::string::npos != (pos = fmtStr.find(PUBLIC_STR))) {
        fmtStr.erase(pos, PUBLIC_STR.length());
    }
    pos = 0;
    while (std::string::npos != (pos = fmtStr.find(PRIVATE_STR))) {
        fmtStr.erase(pos, PRIVATE_STR.length());
    }
    if (fmtStr.length() > MAX_LENGTH) {
        fmtStr.erase(MAX_LENGTH);
    }
}

int HiLogAdapterPrintLog(uint32_t level, const char* tag, const char* fmt, va_list ap)
{
    uint32_t domain = LOG_RENDER_DOMAIN;
    if ((getuid() / BROWSER_UID_BASE) != 0) {
        domain = LOG_APP_DOMAIN;
    }
    std::string fmtStr(fmt);
    Format(fmtStr);
    char buffer[MAX_LENGTH];
    if (FAIL == vsnprintf(buffer, sizeof(buffer), fmtStr.c_str(), ap)) {
        return FAIL;
    }
    return OH_LOG_Print(LOG_APP, LOG_LEVELS[level], domain, tag, STD_FORMAT.c_str(), buffer);
}

int HiLogAdapterConsoleLog(uint32_t level, const char* tag, const char* fmt, va_list ap)
{
    std::string fmtStr(fmt);
    Format(fmtStr);
    char buffer[MAX_LENGTH];
    if (FAIL == vsnprintf(buffer, sizeof(buffer), fmtStr.c_str(), ap)) {
        return FAIL;
    }
    return OH_LOG_Print(LOG_APP, LOG_LEVELS[level], LOG_CONSOLE_DOMAIN, tag, STD_FORMAT.c_str(), buffer);
}
}

int HiLogAdapter::PrintLog(LogLevelAdapter level, const char* tag, const char* fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = HiLogAdapterPrintLog(static_cast<uint32_t>(level), tag, fmt, ap);
    va_end(ap);
    return ret;
}

int HiLogAdapter::PrintConsoleLog(LogLevelAdapter level, const char* tag, const char* fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = HiLogAdapterConsoleLog(static_cast<uint32_t>(level), tag, fmt, ap);
    va_end(ap);
    return ret;
}
} // namespace OHOS::NWeb
