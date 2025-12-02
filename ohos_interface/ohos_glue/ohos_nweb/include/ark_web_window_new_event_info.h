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

#ifndef ARK_WEB_WINDOW_NEW_EVENT_INFO_H_
#define ARK_WEB_WINDOW_NEW_EVENT_INFO_H_
#pragma once

#include "ark_web_controller_handler.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebWindowNewEventInfo : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Gets the url of the new window.
     *
     * @return The url of the new window.
     */
    /*--ark web()--*/
    virtual ArkWebString GetUrl() = 0;

    /**
     * @brief Check is_alert dialog window or not.
     * properties.
     *
     * @return Is alert.
     */
    /*--ark web()--*/
    virtual bool IsAlert() = 0;

    /**
     * @brief Check is_user_trigger triggered by User.
     * properties.
     *
     * @return Is user trigger.
     */
    /*--ark web()--*/
    virtual bool IsUserTrigger() = 0;

    /**
     * @brief Get handler set the new web object.
     * properties.
     *
     * @return The handler of the new web object.
     */
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebControllerHandler> GetHandler() = 0;

    /**
     * @brief Get the requested x-coordinate of the containing window.
     * properties.
     *
     * @return The requested x-coordinate of the containing window.
     */
    /*--ark web()--*/
    virtual int32_t GetX() = 0;

    /**
     * @brief Get the requested y-coordinate of the containing window.
     * properties.
     *
     * @return The requested y-coordinate of the containing window.
     */
    /*--ark web()--*/
    virtual int32_t GetY() = 0;

    /**
     * @brief Get the requested width of the containing window.
     * properties.
     *
     * @return The requested width of the containing window.
     */
    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /**
     * @brief Get the requested height of the containing window.
     * properties.
     *
     * @return The requested height of the containing window.
     */
    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /**
     * @brief Get the value of the navigationPolicy type.
     *
     * @return The value of the navigationPolicy type.
     */
    /*--ark web()--*/
    virtual int32_t GetNavigationPolicy() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_WINDOW_EVENT_INFO_H_
