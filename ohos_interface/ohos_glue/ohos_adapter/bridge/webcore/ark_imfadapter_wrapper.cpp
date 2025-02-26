/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_imfadapter_wrapper.h"

#include "ohos_adapter/bridge/ark_imf_cursor_info_adapter_impl.h"
#include "ohos_adapter/bridge/ark_imf_text_config_adapter_impl.h"
#include "ohos_adapter/bridge/ark_imf_text_listener_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIMFAdapterWrapper::ArkIMFAdapterWrapper(ArkWebRefPtr<ArkIMFAdapter> ref) : ctocpp_(ref) {}

bool ArkIMFAdapterWrapper::Attach(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> listener, bool isShowKeyboard)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->Attach(nullptr, isShowKeyboard);
    }

    return ctocpp_->Attach(new ArkIMFTextListenerAdapterImpl(listener), isShowKeyboard);
}

bool ArkIMFAdapterWrapper::Attach(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> listener, bool isShowKeyboard,
    const std::shared_ptr<OHOS::NWeb::IMFTextConfigAdapter> config, bool isResetListener)
{
    if (!CHECK_SHARED_PTR_IS_NULL(listener) && !CHECK_SHARED_PTR_IS_NULL(config)) {
        return ctocpp_->Attach(new ArkIMFTextListenerAdapterImpl(listener), isShowKeyboard,
            new ArkIMFTextConfigAdapterImpl(config), isResetListener);
    } else if (CHECK_SHARED_PTR_IS_NULL(listener) && CHECK_SHARED_PTR_IS_NULL(config)) {
        return ctocpp_->Attach(nullptr, isShowKeyboard, nullptr, isResetListener);
    } else if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->Attach(nullptr, isShowKeyboard, new ArkIMFTextConfigAdapterImpl(config), isResetListener);
    } else {
        return ctocpp_->Attach(new ArkIMFTextListenerAdapterImpl(listener), isShowKeyboard, nullptr, isResetListener);
    }
}

bool ArkIMFAdapterWrapper::AttachWithRequestKeyboardReason(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> listener,
    bool isShowKeyboard, const std::shared_ptr<OHOS::NWeb::IMFTextConfigAdapter> config, bool isResetListener,
    int32_t requestKeyboardReason)
{
    if (!CHECK_SHARED_PTR_IS_NULL(listener) && !CHECK_SHARED_PTR_IS_NULL(config)) {
        return ctocpp_->AttachWithRequestKeyboardReason(new ArkIMFTextListenerAdapterImpl(listener), isShowKeyboard,
            new ArkIMFTextConfigAdapterImpl(config), isResetListener, requestKeyboardReason);
    } else if (CHECK_SHARED_PTR_IS_NULL(listener) && CHECK_SHARED_PTR_IS_NULL(config)) {
        return ctocpp_->AttachWithRequestKeyboardReason(
            nullptr, isShowKeyboard, nullptr, isResetListener, requestKeyboardReason);
    } else if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->AttachWithRequestKeyboardReason(
            nullptr, isShowKeyboard, new ArkIMFTextConfigAdapterImpl(config), isResetListener, requestKeyboardReason);
    } else {
        return ctocpp_->AttachWithRequestKeyboardReason(new ArkIMFTextListenerAdapterImpl(listener), isShowKeyboard,
            nullptr, isResetListener, requestKeyboardReason);
    }
}

void ArkIMFAdapterWrapper::ShowCurrentInput(const OHOS::NWeb::IMFAdapterTextInputType& inputType)
{
    return ctocpp_->ShowCurrentInput((int32_t)inputType);
}

void ArkIMFAdapterWrapper::HideTextInput()
{
    return ctocpp_->HideTextInput();
}

void ArkIMFAdapterWrapper::Close()
{
    return ctocpp_->Close();
}

void ArkIMFAdapterWrapper::OnCursorUpdate(const std::shared_ptr<OHOS::NWeb::IMFCursorInfoAdapter> cursorInfo)
{
    if (CHECK_SHARED_PTR_IS_NULL(cursorInfo)) {
        return ctocpp_->OnCursorUpdate(nullptr);
    }
    return ctocpp_->OnCursorUpdate(new ArkIMFCursorInfoAdapterImpl(cursorInfo));
}

void ArkIMFAdapterWrapper::OnSelectionChange(std::u16string text, int start, int end)
{
    ArkWebU16String str = ArkWebU16StringClassToStruct(text);
    ctocpp_->OnSelectionChange(str, start, end);

    ArkWebU16StringStructRelease(str);
}

bool ArkIMFAdapterWrapper::SendPrivateCommand(const std::string& commandKey, const std::string& commandValue)
{
    ArkWebString keyStr = ArkWebStringClassToStruct(commandKey);
    ArkWebString valueStr = ArkWebStringClassToStruct(commandValue);
    auto result = ctocpp_->SendPrivateCommand(keyStr, valueStr);

    ArkWebStringStructRelease(keyStr);
    ArkWebStringStructRelease(valueStr);

    return result;
}

} // namespace OHOS::ArkWeb
