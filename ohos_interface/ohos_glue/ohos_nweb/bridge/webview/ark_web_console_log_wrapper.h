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

#ifndef ARK_WEB_CONSOLE_LOG_WRAPPER_H_
#define ARK_WEB_CONSOLE_LOG_WRAPPER_H_
#pragma once

#include "include/nweb_console_log.h"
#include "ohos_nweb/include/ark_web_console_log.h"

namespace OHOS::ArkWeb {

using ArkWebLogLevel = OHOS::NWeb::NWebConsoleLog::NWebConsoleLogLevel;
using ArkWebLogSource = OHOS::NWeb::NWebConsoleLog::NWebConsoleLogSource;

class ArkWebConsoleLogWrapper : public OHOS::NWeb::NWebConsoleLog {
public:
    ArkWebConsoleLogWrapper(ArkWebRefPtr<ArkWebConsoleLog> ark_web_console_log);
    ~ArkWebConsoleLogWrapper() = default;

    /**
     * @brief Get console log message
     *
     * @return message
     */
    std::string Log() override;

    /**
     * @brief Get console log source id
     *
     * @return source id
     */
    std::string SourceId() override;

    /**
     * @brief Get console log message level
     *
     * @return message level
     */
    ArkWebLogLevel LogLevel() override;

    /**
     * @brief Get console log line number
     *
     * @return line number
     */
    int LineNumer() override;

    /**
     * @brief Get console log source type
     *
     * @return source type
     */
    ArkWebLogSource Source() override;

private:
    ArkWebRefPtr<ArkWebConsoleLog> ark_web_console_log_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CONSOLE_LOG_WRAPPER_H_
