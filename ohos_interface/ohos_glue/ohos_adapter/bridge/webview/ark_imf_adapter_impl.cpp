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

#include "ohos_adapter/bridge/ark_imf_adapter_impl.h"

#include "ohos_adapter/bridge/ark_imfcursor_info_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imftext_config_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_imftext_listener_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIMFAdapterImpl::ArkIMFAdapterImpl(std::shared_ptr<OHOS::NWeb::IMFAdapter> ref) : real_(ref) {}

bool ArkIMFAdapterImpl::Attach(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->Attach(nullptr, isShowKeyboard);
    }

    return real_->Attach(std::make_shared<ArkIMFTextListenerAdapterWrapper>(listener), isShowKeyboard);
}

bool ArkIMFAdapterImpl::Attach(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard,
    ArkWebRefPtr<ArkIMFTextConfigAdapter> config, bool isResetListener)
{
    if (CHECK_REF_PTR_IS_NULL(listener) && CHECK_REF_PTR_IS_NULL(config)) {
        return real_->Attach(nullptr, isShowKeyboard, nullptr, isResetListener);
    } else if (!CHECK_REF_PTR_IS_NULL(listener) && !CHECK_REF_PTR_IS_NULL(config)) {
        return real_->Attach(std::make_shared<ArkIMFTextListenerAdapterWrapper>(listener), isShowKeyboard,
            std::make_shared<ArkIMFTextConfigAdapterWrapper>(config), isResetListener);
    } else if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->Attach(
            nullptr, isShowKeyboard, std::make_shared<ArkIMFTextConfigAdapterWrapper>(config), isResetListener);
    } else {
        return real_->Attach(
            std::make_shared<ArkIMFTextListenerAdapterWrapper>(listener), isShowKeyboard, nullptr, isResetListener);
    }
}

bool ArkIMFAdapterImpl::AttachWithRequestKeyboardReason(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener,
    bool isShowKeyboard, ArkWebRefPtr<ArkIMFTextConfigAdapter> config, bool isResetListener,
    int32_t requestKeyboardReason)
{
    if (CHECK_REF_PTR_IS_NULL(listener) && CHECK_REF_PTR_IS_NULL(config)) {
        return real_->AttachWithRequestKeyboardReason(
            nullptr, isShowKeyboard, nullptr, isResetListener, requestKeyboardReason);
    } else if (!CHECK_REF_PTR_IS_NULL(listener) && !CHECK_REF_PTR_IS_NULL(config)) {
        return real_->AttachWithRequestKeyboardReason(std::make_shared<ArkIMFTextListenerAdapterWrapper>(listener),
            isShowKeyboard, std::make_shared<ArkIMFTextConfigAdapterWrapper>(config), isResetListener,
            requestKeyboardReason);
    } else if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->AttachWithRequestKeyboardReason(nullptr, isShowKeyboard,
            std::make_shared<ArkIMFTextConfigAdapterWrapper>(config), isResetListener, requestKeyboardReason);
    } else {
        return real_->AttachWithRequestKeyboardReason(std::make_shared<ArkIMFTextListenerAdapterWrapper>(listener),
            isShowKeyboard, nullptr, isResetListener, requestKeyboardReason);
    }
}

void ArkIMFAdapterImpl::ShowCurrentInput(const int32_t& inputType)
{
    real_->ShowCurrentInput((OHOS::NWeb::IMFAdapterTextInputType)inputType);
}

void ArkIMFAdapterImpl::HideTextInput()
{
    real_->HideTextInput();
}

void ArkIMFAdapterImpl::Close()
{
    real_->Close();
}

void ArkIMFAdapterImpl::OnCursorUpdate(ArkWebRefPtr<ArkIMFCursorInfoAdapter> cursorInfo)
{
    if (CHECK_REF_PTR_IS_NULL(cursorInfo)) {
        return real_->OnCursorUpdate(nullptr);
    }
    real_->OnCursorUpdate(std::make_shared<ArkIMFCursorInfoAdapterWrapper>(cursorInfo));
}

void ArkIMFAdapterImpl::OnSelectionChange(ArkWebU16String& text, int start, int end)
{
    real_->OnSelectionChange(ArkWebU16StringStructToClass(text), start, end);
}

bool ArkIMFAdapterImpl::SendPrivateCommand(const ArkWebString& commandKey, const ArkWebString& commandValue)
{
    std::string key = ArkWebStringStructToClass(commandKey);
    std::string value = ArkWebStringStructToClass(commandValue);

    return real_->SendPrivateCommand(key, value);
}

} // namespace OHOS::ArkWeb
