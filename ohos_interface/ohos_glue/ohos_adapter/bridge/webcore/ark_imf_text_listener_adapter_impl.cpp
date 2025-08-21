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

#include "ohos_adapter/bridge/ark_imf_text_listener_adapter_impl.h"

#include "ohos_adapter/bridge/ark_imf_adapter_function_key_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIMFTextListenerAdapterImpl::ArkIMFTextListenerAdapterImpl(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> ref)
    : real_(ref)
{}

void ArkIMFTextListenerAdapterImpl::InsertText(const ArkWebU16String& text)
{
    real_->InsertText(ArkWebU16StringStructToClass(text));
}

void ArkIMFTextListenerAdapterImpl::DeleteForward(int32_t length)
{
    real_->DeleteForward(length);
}

void ArkIMFTextListenerAdapterImpl::DeleteBackward(int32_t length)
{
    real_->DeleteBackward(length);
}

void ArkIMFTextListenerAdapterImpl::SendKeyEventFromInputMethod()
{
    real_->SendKeyEventFromInputMethod();
}

void ArkIMFTextListenerAdapterImpl::SendKeyboardStatus(const int32_t& keyboardStatus)
{
    real_->SendKeyboardStatus((OHOS::NWeb::IMFAdapterKeyboardStatus)keyboardStatus);
}

void ArkIMFTextListenerAdapterImpl::SendFunctionKey(ArkWebRefPtr<ArkIMFAdapterFunctionKeyAdapter> functionKey)
{
    if (CHECK_REF_PTR_IS_NULL(functionKey)) {
        real_->SendFunctionKey(nullptr);
    }

    real_->SendFunctionKey(std::make_shared<ArkIMFAdapterFunctionKeyAdapterWrapper>(functionKey));
}

void ArkIMFTextListenerAdapterImpl::SetKeyboardStatus(bool status)
{
    real_->SetKeyboardStatus(status);
}

void ArkIMFTextListenerAdapterImpl::MoveCursor(const uint32_t direction)
{
    real_->MoveCursor((OHOS::NWeb::IMFAdapterDirection)direction);
}

void ArkIMFTextListenerAdapterImpl::HandleSetSelection(int32_t start, int32_t end)
{
    real_->HandleSetSelection(start, end);
}

void ArkIMFTextListenerAdapterImpl::HandleExtendAction(int32_t action)
{
    real_->HandleExtendAction(action);
}

void ArkIMFTextListenerAdapterImpl::HandleSelect(int32_t keyCode, int32_t cursorMoveSkip)
{
    real_->HandleSelect(keyCode, cursorMoveSkip);
}

int32_t ArkIMFTextListenerAdapterImpl::GetTextIndexAtCursor()
{
    return real_->GetTextIndexAtCursor();
}

ArkWebU16String ArkIMFTextListenerAdapterImpl::GetLeftTextOfCursor(int32_t number)
{
    std::u16string str = real_->GetLeftTextOfCursor(number);

    return ArkWebU16StringClassToStruct(str);
}

ArkWebU16String ArkIMFTextListenerAdapterImpl::GetRightTextOfCursor(int32_t number)
{
    std::u16string str = real_->GetRightTextOfCursor(number);

    return ArkWebU16StringClassToStruct(str);
}

int32_t ArkIMFTextListenerAdapterImpl::SetPreviewText(const ArkWebU16String& text, int32_t start, int32_t end)
{
    return real_->SetPreviewText(ArkWebU16StringStructToClass(text), start, end);
}

void ArkIMFTextListenerAdapterImpl::FinishTextPreview()
{
    real_->FinishTextPreview();
}

void ArkIMFTextListenerAdapterImpl::SetNeedUnderLine(bool isNeedUnderline)
{
    real_->SetNeedUnderLine(isNeedUnderline);
}

void ArkIMFTextListenerAdapterImpl::AutoFillWithIMFEvent(bool isUsername, bool isOtherAccount, bool isNewPassword,
    const ArkWebString& content)
{
    real_->AutoFillWithIMFEvent(isUsername, isOtherAccount, isNewPassword, ArkWebStringStructToClass(content));
}

void ArkIMFTextListenerAdapterImpl::KeyboardUpperRightCornerHide()
{
    real_->KeyboardUpperRightCornerHide();
}

void ArkIMFTextListenerAdapterImpl::WebSetImeShow(bool visible)
{
    real_->WebSetImeShow(visible);
}
} // namespace OHOS::ArkWeb
