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

#ifndef ARK_WEB_COMMAND_ACTION_H_
#define ARK_WEB_COMMAND_ACTION_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebCommandAction : public virtual ArkWebBaseRefCounted {
public:
    virtual ~ArkWebCommandAction() = default;

    /**
     * @brief Retrieves the type of the simulated event (e.g., "click", "scroll").
     * @return A string representing the event type.
     * @Since: 16001
     */
    /*--ark web()--*/
    virtual ArkWebString GetEventType() = 0;

    /**
     * @brief Retrieves the XPath path of the target element.
     * @return A string representing the XPath path.
     * @Since: 16001
     */
    /*--ark web()--*/
    virtual ArkWebString GetXPath() = 0;

    /**
     * @brief Retrieves the duration of the action (e.g., duration for a scroll).
     * @return The duration in milliseconds (ms).
     * @Since: 16001
     */
    /*--ark web()--*/
    virtual int32_t GetDuration() = 0;

    /**
     * @brief Retrieves the alignment mode (e.g., alignment for scroll top/mid/bottom).
     * @return A string representing the alignment mode.
     * @Since: 16001
     */
    /*--ark web()--*/
    virtual ArkWebString GetAlign() = 0;

    /**
     * @brief Retrieves the offset value for the action.
     * @return The offset value (typically in vp).
     * @Since: 16001
     */
    /*--ark web()--*/
    virtual int32_t GetOffset() = 0;
};

}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_COMMAND_ACTION_H_
