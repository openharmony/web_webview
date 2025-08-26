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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_drag_event_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDragEventWrapper::ArkWebDragEventWrapper(ArkWebRefPtr<ArkWebDragEvent> ark_web_drag_event)
    : ark_web_drag_event_(ark_web_drag_event)
{}

double ArkWebDragEventWrapper::GetX()
{
    return ark_web_drag_event_->GetX();
}

double ArkWebDragEventWrapper::GetY()
{
    return ark_web_drag_event_->GetY();
}

ArkWebDragAction ArkWebDragEventWrapper::GetAction()
{
    return static_cast<ArkWebDragAction>(ark_web_drag_event_->GetAction());
}

ArkWebDragOperation ArkWebDragEventWrapper::GetDragOperation() const
{
    return static_cast<ArkWebDragOperation>(ark_web_drag_event_->GetDragOperation());
}

ArkWebDragOperationsMask ArkWebDragEventWrapper::GetAllowedDragOperation() const
{
    return static_cast<ArkWebDragOperationsMask>(ark_web_drag_event_->GetAllowedDragOperation());
}

bool ArkWebDragEventWrapper::IsDragOpValid() const
{
    return ark_web_drag_event_->IsDragOpValid();
}

} // namespace OHOS::ArkWeb
