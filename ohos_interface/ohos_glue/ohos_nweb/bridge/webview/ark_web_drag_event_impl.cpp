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

#include "ohos_nweb/bridge/ark_web_drag_event_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDragEventImpl::ArkWebDragEventImpl(std::shared_ptr<OHOS::NWeb::NWebDragEvent> nweb_drag_event)
    : nweb_drag_event_(nweb_drag_event)
{}

double ArkWebDragEventImpl::GetX()
{
    return nweb_drag_event_->GetX();
}

double ArkWebDragEventImpl::GetY()
{
    return nweb_drag_event_->GetY();
}

int ArkWebDragEventImpl::GetAction()
{
    return static_cast<int>(nweb_drag_event_->GetAction());
}

} // namespace OHOS::ArkWeb
