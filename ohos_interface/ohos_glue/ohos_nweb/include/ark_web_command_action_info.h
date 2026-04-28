/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_COMMAND_ACTION_INFO_H_
#define ARK_WEB_COMMAND_ACTION_INFO_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebCommandActionInfo : public virtual ArkWebBaseRefCounted {
public:
    virtual ~ArkWebCommandActionInfo() = default;

    /**
     * @brief Get the event type.
     * @return The event type string.
     */
    /*--ark web()--*/
    virtual ArkWebString GetEventType() = 0;

    // Input getters

    /**
     * @brief Get the input value to inject.
     * @return The input value string.
     */
    /*--ark web()--*/
    virtual ArkWebString GetInputValue() = 0;

    /**
     * @brief Get the XPath of the target element.
     * @return The XPath string.
     */
    /*--ark web()--*/
    virtual ArkWebString GetXPath() = 0;

    // Select getters

    /**
     * @brief Get the select option values to match.
     * @return The select option values vector.
     */
    /*--ark web()--*/
    virtual ArkWebStringVector GetOptionValues() = 0;

    /**
     * @brief Get the select option indexes.
     * @return The select option indexes vector.
     */
    /*--ark web()--*/
    virtual ArkWebInt32Vector GetOptionIndexes() = 0;

    /**
     * @brief Get X coordinate for gesture events.
     * @return X coordinate value.
     */
    /*--ark web()--*/
    virtual double GetX() = 0;

    /**
     * @brief Get Y coordinate for gesture events.
     * @return Y coordinate value.
     */
    /*--ark web()--*/
    virtual double GetY() = 0;

    /**
     * @brief Get horizontal distance for scroll gesture.
     * @return Horizontal distance value in pixels.
     */
    /*--ark web()--*/
    virtual double GetDistanceX() = 0;

    /**
     * @brief Get vertical distance for scroll gesture.
     * @return Vertical distance value in pixels.
     */
    /*--ark web()--*/
    virtual double GetDistanceY() = 0;

    /**
     * @brief Get scale factor for pinch gesture.
     * @return Scale factor value.
     */
    /*--ark web()--*/
    virtual float GetScale() = 0;

    /**
     * @brief Get tap duration for tap gesture.
     * @return Duration in milliseconds.
     */
    /*--ark web()--*/
    virtual int32_t GetDuration() = 0;

    /**
     * @brief Get tap count for tap gesture.
     * @return Number of taps.
     */
    /*--ark web()--*/
    virtual int32_t GetTapCount() = 0;

    /**
     * @brief Get speed for scroll or pinch gesture.
     * @return Speed in pixels per second.
     */
    /*--ark web()--*/
    virtual int32_t GetSpeed() = 0;
};

}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_COMMAND_ACTION_INFO_H_
