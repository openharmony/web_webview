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

#include "ohos_adapter/bridge/ark_imftext_listener_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_imf_adapter_function_key_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkIMFTextListenerAdapterWrapper::ArkIMFTextListenerAdapterWrapper(ArkWebRefPtr<ArkIMFTextListenerAdapter> ref)
    : ctocpp_(ref)
{}

void ArkIMFTextListenerAdapterWrapper::InsertText(const std::u16string& text)
{
    ArkWebU16String str = ArkWebU16StringClassToStruct(text);
    ctocpp_->InsertText(str);
    ArkWebU16StringStructRelease(str);
}

void ArkIMFTextListenerAdapterWrapper::DeleteForward(int32_t length)
{
    ctocpp_->DeleteForward(length);
}

void ArkIMFTextListenerAdapterWrapper::DeleteBackward(int32_t length)
{
    ctocpp_->DeleteBackward(length);
}

void ArkIMFTextListenerAdapterWrapper::SendKeyEventFromInputMethod()
{
    ctocpp_->SendKeyEventFromInputMethod();
}

void ArkIMFTextListenerAdapterWrapper::SendKeyboardStatus(const OHOS::NWeb::IMFAdapterKeyboardStatus& keyboardStatus)
{
    ctocpp_->SendKeyboardStatus((int32_t)keyboardStatus);
}

void ArkIMFTextListenerAdapterWrapper::SendFunctionKey(
    std::shared_ptr<OHOS::NWeb::IMFAdapterFunctionKeyAdapter> functionKey)
{
    if (CHECK_SHARED_PTR_IS_NULL(functionKey)) {
        return ctocpp_->SendFunctionKey(nullptr);
    }

    ctocpp_->SendFunctionKey(new ArkIMFAdapterFunctionKeyAdapterImpl(functionKey));
}

void ArkIMFTextListenerAdapterWrapper::SetKeyboardStatus(bool status)
{
    ctocpp_->SetKeyboardStatus(status);
}

void ArkIMFTextListenerAdapterWrapper::MoveCursor(const OHOS::NWeb::IMFAdapterDirection direction)
{
    ctocpp_->MoveCursor((uint32_t)direction);
}

void ArkIMFTextListenerAdapterWrapper::HandleSetSelection(int32_t start, int32_t end)
{
    ctocpp_->HandleSetSelection(start, end);
}

void ArkIMFTextListenerAdapterWrapper::HandleExtendAction(int32_t action)
{
    ctocpp_->HandleExtendAction(action);
}

void ArkIMFTextListenerAdapterWrapper::HandleSelect(int32_t keyCode, int32_t cursorMoveSkip)
{
    ctocpp_->HandleSelect(keyCode, cursorMoveSkip);
}

int32_t ArkIMFTextListenerAdapterWrapper::GetTextIndexAtCursor()
{
    return ctocpp_->GetTextIndexAtCursor();
}

std::u16string ArkIMFTextListenerAdapterWrapper::GetLeftTextOfCursor(int32_t number)
{
    ArkWebU16String str = ctocpp_->GetLeftTextOfCursor(number);
    std::u16string result = ArkWebU16StringStructToClass(str);

    ArkWebU16StringStructRelease(str);
    return result;
}

std::u16string ArkIMFTextListenerAdapterWrapper::GetRightTextOfCursor(int32_t number)
{
    ArkWebU16String str = ctocpp_->GetRightTextOfCursor(number);
    std::u16string result = ArkWebU16StringStructToClass(str);

    ArkWebU16StringStructRelease(str);
    return result;
}

int32_t ArkIMFTextListenerAdapterWrapper::SetPreviewText(const std::u16string& text, int32_t start, int32_t end)
{
    ArkWebU16String str = ArkWebU16StringClassToStruct(text);
    int32_t result = ctocpp_->SetPreviewText(str, start, end);

    ArkWebU16StringStructRelease(str);
    return result;
}

void ArkIMFTextListenerAdapterWrapper::FinishTextPreview()
{
    ctocpp_->FinishTextPreview();
}

void ArkIMFTextListenerAdapterWrapper::SetNeedUnderLine(bool isNeedUnderline)
{
    ctocpp_->SetNeedUnderLine(isNeedUnderline);
}

void ArkIMFTextListenerAdapterWrapper::AutoFillWithIMFEvent(bool isUsername, bool isOtherAccount, bool isNewPassword,
    const std::string& content)
{
    ArkWebString str = ArkWebStringClassToStruct(content);
    ctocpp_->AutoFillWithIMFEvent(isUsername, isOtherAccount, isNewPassword, str);

    ArkWebStringStructRelease(str);
}

void ArkIMFTextListenerAdapterWrapper::KeyboardUpperRightCornerHide()
{
    ctocpp_->KeyboardUpperRightCornerHide();
}

} // namespace OHOS::ArkWeb
