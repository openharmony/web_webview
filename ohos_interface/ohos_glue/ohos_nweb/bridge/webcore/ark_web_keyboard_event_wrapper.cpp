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

#include "ohos_nweb/bridge/ark_web_keyboard_event_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebKeyboardEventWrapper::ArkWebKeyboardEventWrapper(ArkWebRefPtr<ArkWebKeyboardEvent> keyboardEvent)
    : keyboardEvent_(keyboardEvent)
{}

int32_t ArkWebKeyboardEventWrapper::GetKeyCode()
{
    return keyboardEvent_->GetKeyCode();
}

int32_t ArkWebKeyboardEventWrapper::GetAction() 
{
    return keyboardEvent_->GetAction();
}
int32_t ArkWebKeyboardEventWrapper::GetUnicode() 
{
    return keyboardEvent_->GetUnicode();
}

bool ArkWebKeyboardEventWrapper::IsEnableCapsLock() 
{
    return keyboardEvent_->IsEnableCapsLock();
}

std::vector<int32_t> ArkWebKeyboardEventWrapper::GetPressKeyCodes()
{
    ArkWebInt32Vector pressKeyCodes = keyboardEvent_->GetPressKeyCodes();

    std::vector<int32_t> result = ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(pressKeyCodes);

    ArkWebBasicVectorStructRelease(pressKeyCodes);
    
    return result;
}
} // namespace OHOS::ArkWeb