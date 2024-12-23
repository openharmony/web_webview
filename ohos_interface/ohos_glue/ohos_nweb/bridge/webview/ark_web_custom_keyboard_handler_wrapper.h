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

#ifndef ARK_WEB_CUSTOM_KEYBOARD_HANDLER_WRAPPER_H_
#define ARK_WEB_CUSTOM_KEYBOARD_HANDLER_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_custom_keyboard_handler.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebCustomKeyboardHandlerWrapper : public OHOS::NWeb::NWebCustomKeyboardHandler {
public:
    ArkWebCustomKeyboardHandlerWrapper(ArkWebRefPtr<ArkWebCustomKeyboardHandler> ark_web_custom_keyboard_handler) :
        ark_web_custom_keyboard_handler_(ark_web_custom_keyboard_handler) {}
    ~ArkWebCustomKeyboardHandlerWrapper() = default;

    void InsertText(const std::string &text) override;

    void DeleteForward(int32_t length) override;

    void DeleteBackward(int32_t length) override;

    void SendFunctionKey(int32_t key) override;

    void Close() override;

private:
    ArkWebRefPtr<ArkWebCustomKeyboardHandler> ark_web_custom_keyboard_handler_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CUSTOM_KEYBOARD_HANDLER_WRAPPER_H_