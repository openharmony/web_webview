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

#ifndef ARK_WEB_DRAG_EVENT_WRAPPER_H_
#define ARK_WEB_DRAG_EVENT_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_drag_event.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

using ArkWebDragAction = OHOS::NWeb::DragAction;

class ArkWebDragEventWrapper : public OHOS::NWeb::NWebDragEvent {
public:
    ArkWebDragEventWrapper(ArkWebRefPtr<ArkWebDragEvent> ark_web_drag_event);
    ~ArkWebDragEventWrapper() = default;

    double GetX() override;

    double GetY() override;

    ArkWebDragAction GetAction() override;

private:
    ArkWebRefPtr<ArkWebDragEvent> ark_web_drag_event_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DRAG_EVENT_WRAPPER_H_
