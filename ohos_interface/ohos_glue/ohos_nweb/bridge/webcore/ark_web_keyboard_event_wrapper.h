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

#ifndef ARK_WEB_KEYBOARD_EVENT_IMPL_H_
#define ARK_WEB_KEYBOARD_EVENT_IMPL_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_keyboard_event.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebKeyboardEventWrapper : public OHOS::NWeb::NWebKeyboardEvent {
public:
    ArkWebKeyboardEventWrapper(ArkWebRefPtr<ArkWebKeyboardEvent> keyboardEvent);
    ~ArkWebKeyboardEventWrapper() = default;

    int32_t GetKeyCode() override;

    int32_t GetAction() override;

    int32_t GetUnicode() override;

    bool IsEnableCapsLock() override;

    std::vector<int32_t> GetPressKeyCodes() override;

private:
    ArkWebRefPtr<ArkWebKeyboardEvent> keyboardEvent_;
};

} //namespace OHOS::ArkWeb

#endif // ARK_WEB_KEYBOARD_EVENT_IMPL_H