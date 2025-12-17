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

#ifndef ARK_WEB_CONSOLE_LOG_IMPL_H_
#define ARK_WEB_CONSOLE_LOG_IMPL_H_
#pragma once

#include "include/nweb_console_log.h"
#include "ohos_nweb/include/ark_web_console_log.h"

namespace OHOS::ArkWeb {

class ArkWebConsoleLogImpl : public ArkWebConsoleLog {
    IMPLEMENT_REFCOUNTING(ArkWebConsoleLogImpl);

public:
    ArkWebConsoleLogImpl(std::shared_ptr<OHOS::NWeb::NWebConsoleLog> nweb_console_log);
    ~ArkWebConsoleLogImpl() = default;

    /**
     * @brief Get console log message
     *
     * @return message
     */
    ArkWebString Log() override;

    /**
     * @brief Get console log source id
     *
     * @return source id
     */
    ArkWebString SourceId() override;

    /**
     * @brief Get console log message level
     *
     * @return message level
     */
    int LogLevel() override;

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
    int Source() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebConsoleLog> nweb_console_log_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CONSOLE_LOG_IMPL_H_
