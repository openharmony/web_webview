/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_CONSOLE_LOG_H
#define NWEB_CONSOLE_LOG_H

#include <memory>
#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebConsoleLog {
public:
    enum NWebConsoleLogLevel { DEBUG = 1, INFO, WARNING, ERROR, UNKNOWN };

    enum NWebConsoleLogSource {
        XML,
        JAVASCRIPT,
        NETWORK,
        CONSOLEAPI,
        STORAGE,
        RENDERING,
        SECURITY,
        OTHER,
        DEPRECATION,
        WORKER,
        VIOLATION,
        INTERVENTION,
        RECOMMENDATION
    };

    virtual ~NWebConsoleLog() = default;

    /**
     * @brief Get console log line number
     *
     * @retval line number
     */
    virtual int LineNumer() = 0;

    /**
     * @brief Get console log message
     *
     * @retval message
     */
    virtual std::string Log() = 0;

    /**
     * @brief Get console log message level
     *
     * @retval message level
     */
    virtual NWebConsoleLogLevel LogLevel() = 0;

    /**
     * @brief Get console log source id
     *
     * @retval source id
     */
    virtual std::string SourceId() = 0;

    /**
     * @brief Get console log source id
     *
     * @retval source id
     */
    virtual NWebConsoleLogSource Source()
    {
        return OTHER;
    }
};

} // namespace OHOS::NWeb

#endif // NWEB_CONSOLE_LOG_H