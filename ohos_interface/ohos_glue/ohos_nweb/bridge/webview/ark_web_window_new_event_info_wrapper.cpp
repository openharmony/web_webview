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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_window_new_event_info_wrapper.h"

#include "ohos_nweb/bridge/ark_web_controller_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebWindowNewEventInfoWrapper::ArkWebWindowNewEventInfoWrapper(
    ArkWebRefPtr<ArkWebWindowNewEventInfo> ark_web_window_new_event_info)
    : ark_web_window_new_event_info_(ark_web_window_new_event_info)
{}

std::string ArkWebWindowNewEventInfoWrapper::GetUrl()
{
    ArkWebString stUrl = ark_web_window_new_event_info_->GetUrl();
    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

bool ArkWebWindowNewEventInfoWrapper::IsAlert()
{
    return ark_web_window_new_event_info_->IsAlert();
}

bool ArkWebWindowNewEventInfoWrapper::IsUserTrigger()
{
    return ark_web_window_new_event_info_->IsUserTrigger();
}

std::shared_ptr<OHOS::NWeb::NWebControllerHandler> ArkWebWindowNewEventInfoWrapper::GetHandler()
{
    ArkWebRefPtr<ArkWebControllerHandler> handler = ark_web_window_new_event_info_->GetHandler();
    if (CHECK_REF_PTR_IS_NULL(handler)) {
        return nullptr;
    }

    return std::make_shared<ArkWebControllerHandlerWrapper>(handler);
}

int32_t ArkWebWindowNewEventInfoWrapper::GetX()
{
    return ark_web_window_new_event_info_->GetX();
}

int32_t ArkWebWindowNewEventInfoWrapper::GetY()
{
    return ark_web_window_new_event_info_->GetY();
}

int32_t ArkWebWindowNewEventInfoWrapper::GetWidth()
{
    return ark_web_window_new_event_info_->GetWidth();
}

int32_t ArkWebWindowNewEventInfoWrapper::GetHeight()
{
    return ark_web_window_new_event_info_->GetHeight();
}

ArkWebNavigationPolicy ArkWebWindowNewEventInfoWrapper::GetNavigationPolicy()
{
    return static_cast<ArkWebNavigationPolicy>(ark_web_window_new_event_info_->GetNavigationPolicy());
}

} // namespace OHOS::ArkWeb