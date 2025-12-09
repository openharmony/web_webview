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

#ifndef ARK_WEB_CONSOLE_LOG_H_
#define ARK_WEB_CONSOLE_LOG_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebConsoleLog : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Get console log message
     *
     * @return message
     */
    /*--ark web()--*/
    virtual ArkWebString Log() = 0;

    /**
     * @brief Get console log source id
     *
     * @return source id
     */
    /*--ark web()--*/
    virtual ArkWebString SourceId() = 0;

    /**
     * @brief Get console log message level
     *
     * @return message level
     */
    /*--ark web()--*/
    virtual int LogLevel() = 0;

    /**
     * @brief Get console log line number
     *
     * @return line number
     */
    /*--ark web()--*/
    virtual int LineNumer() = 0;

    /**
     * @brief Get console log source type
     *
     * @return source type
     */
    /*--ark web()--*/
    virtual int Source() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CONSOLE_LOG_H_
