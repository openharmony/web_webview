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

#include "ohos_nweb/bridge/ark_web_keyboard_event_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
    
ArkWebKeyboardEventImpl::ArkWebKeyboardEventImpl(std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent> keyboardEvent)
    : keyboardEvent_(keyboardEvent)
{}

int32_t ArkWebKeyboardEventImpl::GetKeyCode()
{
    return keyboardEvent_->GetKeyCode();
}

int32_t ArkWebKeyboardEventImpl::GetAction() 
{
    return keyboardEvent_->GetAction();
}
int32_t ArkWebKeyboardEventImpl::GetUnicode() 
{
    return keyboardEvent_->GetUnicode();
}

bool ArkWebKeyboardEventImpl::IsEnableCapsLock() 
{
    return keyboardEvent_->IsEnableCapsLock();
}

ArkWebInt32Vector ArkWebKeyboardEventImpl::GetPressKeyCodes()
{
    return ArkWebBasicVectorClassToStruct<int32_t,ArkWebInt32Vector>(keyboardEvent_->GetPressKeyCodes());
}

} //namespace OHOS::ArkWeb