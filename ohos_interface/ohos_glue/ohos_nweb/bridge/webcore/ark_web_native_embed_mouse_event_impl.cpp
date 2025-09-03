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

#include "ohos_nweb/bridge/ark_web_native_embed_mouse_event_impl.h"

#include "ohos_nweb/bridge/ark_web_mouse_event_result_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedMouseEventImpl::ArkWebNativeEmbedMouseEventImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedMouseEvent> nweb_native_embed_mouse_event)
    : nweb_native_embed_mouse_event_(nweb_native_embed_mouse_event)
{}

float ArkWebNativeEmbedMouseEventImpl::GetX()
{
    return nweb_native_embed_mouse_event_->GetX();
}

float ArkWebNativeEmbedMouseEventImpl::GetY()
{
    return nweb_native_embed_mouse_event_->GetY();
}

bool ArkWebNativeEmbedMouseEventImpl::IsHitNativeArea()
{
    return nweb_native_embed_mouse_event_->IsHitNativeArea();
}

size_t ArkWebNativeEmbedMouseEventImpl::GetType()
{
    return static_cast<size_t>(nweb_native_embed_mouse_event_->GetType());
}

size_t ArkWebNativeEmbedMouseEventImpl::GetButton()
{
    return static_cast<size_t>(nweb_native_embed_mouse_event_->GetButton());
}

float ArkWebNativeEmbedMouseEventImpl::GetOffsetX()
{
    return nweb_native_embed_mouse_event_->GetOffsetX();
}

float ArkWebNativeEmbedMouseEventImpl::GetOffsetY()
{
    return nweb_native_embed_mouse_event_->GetOffsetY();
}

float ArkWebNativeEmbedMouseEventImpl::GetScreenX()
{
    return nweb_native_embed_mouse_event_->GetScreenX();
}

float ArkWebNativeEmbedMouseEventImpl::GetScreenY()
{
    return nweb_native_embed_mouse_event_->GetScreenY();
}

ArkWebString ArkWebNativeEmbedMouseEventImpl::GetEmbedId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_mouse_event_->GetEmbedId());
}

ArkWebRefPtr<ArkWebMouseEventResult> ArkWebNativeEmbedMouseEventImpl::GetResult()
{
    std::shared_ptr<OHOS::NWeb::NWebMouseEventResult> result = nweb_native_embed_mouse_event_->GetResult();
    if (CHECK_SHARED_PTR_IS_NULL(result)) {
        return nullptr;
    }
    return new ArkWebMouseEventResultImpl(result);
}

} // namespace OHOS::ArkWeb
