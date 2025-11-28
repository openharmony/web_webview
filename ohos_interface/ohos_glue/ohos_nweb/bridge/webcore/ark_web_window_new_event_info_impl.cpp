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

#include "ohos_nweb/bridge/ark_web_window_new_event_info_impl.h"

#include "ohos_nweb/bridge/ark_web_controller_handler_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebWindowNewEventInfoImpl::ArkWebNewWindowInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebWindowNewEventInfo> nweb_window_new_event_info)
    : nweb_window_new_event_info_(nweb_window_new_event_info)
{}

ArkWebString ArkWebWindowNewEventInfoImpl::GetUrl()
{
    return ArkWebStringClassToStruct(nweb_window_new_event_info_->GetUrl());
}

bool ArkWebWindowNewEventInfoImpl::IsAlert()
{
    return nweb_window_new_event_info_->IsAlert();
}

bool ArkWebWindowNewEventInfoImpl::IsUserTrigger()
{
    return nweb_window_new_event_info_->IsUserTrigger();
}

ArkWebRefPtr<ArkWebControllerHandler> ArkWebWindowNewEventInfoImpl::GetHandler()
{
    std::shared_ptr<OHOS::NWeb::NWebControllerHandler> handler = nweb_window_new_event_info_->GetHandler();
    if (CHECK_SHARED_PTR_IS_NULL(handler)) {
        return nullptr;
    }
    return new ArkWebControllerHandlerImpl(handler);
}

int32_t ArkWebWindowNewEventInfoImpl::GetX()
{
    return nweb_window_new_event_info_->GetX();
}

int32_t ArkWebWindowNewEventInfoImpl::GetY()
{
    return nweb_window_new_event_info_->GetY();
}

int32_t ArkWebWindowNewEventInfoImpl::GetWidth()
{
    return nweb_window_new_event_info_->GetWidth();
}

int32_t ArkWebWindowNewEventInfoImpl::GetHeight()
{
    return nweb_window_new_event_info_->GetHeight();
}

int32_t ArkWebWindowNewEventInfoImpl::GetNavigationPolicy()
{
    return static_cast<size_t>(nweb_window_new_event_info_->GetNavigationPolicy());
}

} // namespace OHOS::ArkWeb