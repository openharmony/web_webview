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

#ifndef ARK_WEB_WINDOW_EVENT_IMPL_H_
#define ARK_WEB_WINDOW_EVENT_IMPL_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_window_new_event_info.h"

namespace OHOS::ArkWeb {

class ArkWebWindowNewEventInfoImpl : public virtual ArkWebWindowNewEventInfo {
    IMPLEMENT_REFCOUNTING(ArkWebWindowNewEventInfo);

public:
    ArkWebWindowNewEventInfoImpl(std::shared_ptr<OHOS::NWeb::NWebWindowNewEventInfo> nweb_window_new_event_info);
    ~ArkWebWindowNewEventInfoImpl() = default;

    /**
     * @brief Gets the url of the new window.
     *
     * @return The url of the new window.
     */
    ArkWebString GetUrl() override;

    /**
     * @brief Check is_alert dialog window or not.
     * properties.
     *
     * @return Is alert.
     */
    bool IsAlert() override;

    /**
     * @brief Check is_user_trigger triggered by User.
     * properties.
     *
     * @return Is user trigger.
     */
    bool IsUserTrigger() override;

    /**
     * @brief Get handler set the new web object.
     * properties.
     *
     * @return The handler of the new web object.
     */
    ArkWebRefPtr<ArkWebControllerHandler> GetHandler() override;

    /**
     * @brief Get the requested x-coordinate of the containing window.
     * properties.
     *
     * @return The requested x-coordinate of the containing window.
     */
    int32_t GetX() override;

    /**
     * @brief Get the requested y-coordinate of the containing window.
     * properties.
     *
     * @return The requested y-coordinate of the containing window.
     */
    int32_t GetY() override;

    /**
     * @brief Get the requested width of the containing window.
     * properties.
     *
     * @return The requested width of the containing window.
     */
    int32_t GetWidth() override;

    /**
     * @brief Get the requested height of the containing window.
     * properties.
     *
     * @return The requested height of the containing window.
     */
    int32_t GetHeight() override;

    /**
     * @brief Get the value of the navigationPolicy type.
     *
     * @return The value of the navigationPolicy type.
     */
    int32_t GetNavigationPolicy() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebWindowNewEventInfo> nweb_window_new_event_info_;
};
} // namespace OHOS::ArkWeb

#endif // ARK_WEB_WINDOW_EVENT_IMPL_H_