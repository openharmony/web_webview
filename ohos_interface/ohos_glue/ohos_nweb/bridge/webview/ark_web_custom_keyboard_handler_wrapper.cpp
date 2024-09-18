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

#include "ohos_nweb/bridge/ark_web_custom_keyboard_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

void ArkWebCustomKeyboardHandlerWrapper::InsertText(const std::string& text)
{
    ark_web_custom_keyboard_handler_->InsertText(ArkWebStringClassToStruct(text));
}

void ArkWebCustomKeyboardHandlerWrapper::DeleteForward(int32_t length)
{
    ark_web_custom_keyboard_handler_->DeleteForward(length);
}

void ArkWebCustomKeyboardHandlerWrapper::DeleteBackward(int32_t length)
{
    ark_web_custom_keyboard_handler_->DeleteBackward(length);
}

void ArkWebCustomKeyboardHandlerWrapper::SendFunctionKey(int32_t key)
{
    ark_web_custom_keyboard_handler_->SendFunctionKey(key);
}

void ArkWebCustomKeyboardHandlerWrapper::Close()
{
    ark_web_custom_keyboard_handler_->Close();
}

} // namespace OHOS::ArkWeb