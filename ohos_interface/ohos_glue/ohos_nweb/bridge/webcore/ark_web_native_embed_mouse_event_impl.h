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

#ifndef ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_IMPL_H_
#define ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_IMPL_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_native_embed_mouse_event.h"

namespace OHOS::ArkWeb {

class ArkWebNativeEmbedMouseEventImpl : public virtual ArkWebNativeEmbedMouseEvent {
    IMPLEMENT_REFCOUNTING(ArkWebNativeEmbedMouseEventImpl);

public:
    ArkWebNativeEmbedMouseEventImpl(
        std::shared_ptr<OHOS::NWeb::NWebNativeEmbedMouseEvent> nweb_native_embed_mouse_event);
    ~ArkWebNativeEmbedMouseEventImpl() = default;

    float GetX() override;

    float GetY() override;

    bool IsHitNativeArea() override;

    size_t GetType() override;

    size_t GetButton() override;

    float GetOffsetX() override;

    float GetOffsetY() override;

    float GetScreenX() override;

    float GetScreenY() override;

    ArkWebString GetEmbedId() override;

    ArkWebRefPtr<ArkWebMouseEventResult> GetResult() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedMouseEvent> nweb_native_embed_mouse_event_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_MOUSE_EVENT_IMPL_H_
