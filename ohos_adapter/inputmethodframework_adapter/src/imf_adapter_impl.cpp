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

#include "imf_adapter_impl.h"

#include "nweb_log.h"

namespace OHOS::NWeb {
IMFTextListenerAdapterImpl::IMFTextListenerAdapterImpl(const std::shared_ptr<IMFTextListenerAdapter>& listener)
    : listener_(listener) {};

IMFTextListenerAdapterImpl::~IMFTextListenerAdapterImpl()
{
    listener_ = nullptr;
}

void IMFTextListenerAdapterImpl::InsertText(const std::u16string& text)
{
    if (listener_) {
        listener_->InsertText(text);
    }
}

void IMFTextListenerAdapterImpl::DeleteForward(int32_t length)
{
    if (listener_) {
        listener_->DeleteForward(length);
    }
}

void IMFTextListenerAdapterImpl::DeleteBackward(int32_t length)
{
    if (listener_) {
        listener_->DeleteBackward(length);
    }
}

void IMFTextListenerAdapterImpl::SendKeyEventFromInputMethod(const MiscServices::KeyEvent& event)
{
    (void)event;
    if (listener_) {
        listener_->SendKeyEventFromInputMethod();
    }
}

void IMFTextListenerAdapterImpl::SendKeyboardStatus(const MiscServices::KeyboardStatus& keyboardStatus)
{
    if (listener_) {
        auto status = IMFAdapterKeyboardStatus::NONE;
        if (keyboardStatus == MiscServices::KeyboardStatus::SHOW) {
            status = IMFAdapterKeyboardStatus::SHOW;
        } else if (keyboardStatus == MiscServices::KeyboardStatus::HIDE) {
            status = IMFAdapterKeyboardStatus::HIDE;
        }
        listener_->SendKeyboardStatus(status);
    }
}

void IMFTextListenerAdapterImpl::SendFunctionKey(const MiscServices::FunctionKey& functionKey)
{
    if (listener_) {
        IMFAdapterFunctionKey adapterFunction;
        switch (functionKey.GetEnterKeyType()) {
            case MiscServices::EnterKeyType::UNSPECIFIED:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::UNSPECIFIED);
                break;
            case MiscServices::EnterKeyType::NONE:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::NONE);
                break;
            case MiscServices::EnterKeyType::GO:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::GO);
                break;
            case MiscServices::EnterKeyType::SEARCH:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::SEARCH);
                break;
            case MiscServices::EnterKeyType::SEND:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::SEND);
                break;
            case MiscServices::EnterKeyType::NEXT:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::NEXT);
                break;
            case MiscServices::EnterKeyType::DONE:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::DONE);
                break;
            case MiscServices::EnterKeyType::PREVIOUS:
                adapterFunction.SetEnterKeyType(IMFAdapterEnterKeyType::PREVIOUS);
                break;
            default:
                WVLOG_E("unknown functionKey");
                break;
        }
        listener_->SendFunctionKey(adapterFunction);
    }
}

void IMFTextListenerAdapterImpl::SetKeyboardStatus(bool status)
{
    if (listener_) {
        listener_->SetKeyboardStatus(status);
    }
}

void IMFTextListenerAdapterImpl::MoveCursor(const MiscServices::Direction direction)
{
    if (listener_) {
        IMFAdapterDirection adapterDirection;
        switch (direction) {
            case MiscServices::Direction::UP: {
                adapterDirection = IMFAdapterDirection::UP;
                break;
            }
            case MiscServices::Direction::DOWN: {
                adapterDirection = IMFAdapterDirection::DOWN;
                break;
            }
            case MiscServices::Direction::LEFT: {
                adapterDirection = IMFAdapterDirection::LEFT;
                break;
            }
            case MiscServices::Direction::RIGHT: {
                adapterDirection = IMFAdapterDirection::RIGHT;
                break;
            }
            default: {
                adapterDirection = IMFAdapterDirection::NONE;
            }
        }
        listener_->MoveCursor(adapterDirection);
    }
}

void IMFTextListenerAdapterImpl::HandleSetSelection(int32_t start, int32_t end)
{
    if (listener_) {
        listener_->HandleSetSelection(start, end);
    }
}

void IMFTextListenerAdapterImpl::HandleExtendAction(int32_t action)
{
    if (listener_) {
        listener_->HandleExtendAction(action);
    }
}

void IMFTextListenerAdapterImpl::HandleSelect(int32_t keyCode, int32_t cursorMoveSkip)
{
    if (listener_) {
        listener_->HandleSelect(keyCode, cursorMoveSkip);
    }
}

int32_t IMFTextListenerAdapterImpl::GetTextIndexAtCursor()
{
    if (listener_) {
        return listener_->GetTextIndexAtCursor();
    }
    return -1;
}

std::u16string IMFTextListenerAdapterImpl::GetLeftTextOfCursor(int32_t number)
{
    if (listener_) {
        return listener_->GetLeftTextOfCursor(number);
    }
    return u"";
}

std::u16string IMFTextListenerAdapterImpl::GetRightTextOfCursor(int32_t number)
{
    if (listener_) {
        return listener_->GetRightTextOfCursor(number);
    }
    return u"";
}

bool IMFAdapterImpl::Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard)
{
    if (!listener) {
        WVLOG_E("the listener is nullptr");
        return false;
    }
    if (!textListener_) {
        textListener_ = new (std::nothrow) IMFTextListenerAdapterImpl(listener);
        if (!textListener_) {
            WVLOG_E("new textListener failed");
            return false;
        }
    }
    int32_t ret = MiscServices::InputMethodController::GetInstance()->Attach(textListener_, isShowKeyboard);
    if (ret != 0) {
        WVLOG_E("inputmethod attach failed, errcode=%{public}d", ret);
        return false;
    }
    return true;
}

bool IMFAdapterImpl::Attach(
    std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard, const IMFAdapterTextConfig &config)
{
    if (!listener) {
        WVLOG_E("the listener is nullptr");
        return false;
    }
    if (!textListener_) {
        textListener_ = new (std::nothrow) IMFTextListenerAdapterImpl(listener);
        if (!textListener_) {
            WVLOG_E("new textListener failed");
            return false;
        }
    }

    MiscServices::InputAttribute inputAttribute = { .inputPattern = config.inputAttribute.inputPattern,
        .enterKeyType = config.inputAttribute.enterKeyType };

    MiscServices::CursorInfo imfInfo = {
        .left = config.cursorInfo.left,
        .top = config.cursorInfo.top,
        .width = config.cursorInfo.width,
        .height = config.cursorInfo.height
    };

    MiscServices::TextConfig textConfig = { .inputAttribute = inputAttribute, .cursorInfo = imfInfo };
    int32_t ret = MiscServices::InputMethodController::GetInstance()->Attach(textListener_, isShowKeyboard, textConfig);
    if (ret != 0) {
        WVLOG_E("inputmethod attach failed, errcode=%{public}d", ret);
        return false;
    }
    return true;
}

void IMFAdapterImpl::ShowCurrentInput(const IMFAdapterTextInputType& inputType)
{
    MiscServices::Configuration config;
    if (inputType == IMFAdapterTextInputType::NUMBER) {
        config.SetTextInputType(MiscServices::TextInputType::NUMBER);
    } else {
        config.SetTextInputType(MiscServices::TextInputType::TEXT);
    }
    MiscServices::InputMethodController::GetInstance()->OnConfigurationChange(config);
    MiscServices::InputMethodController::GetInstance()->ShowCurrentInput();
}

void IMFAdapterImpl::HideTextInput()
{
    MiscServices::InputMethodController::GetInstance()->HideTextInput();
}

void IMFAdapterImpl::Close()
{
    MiscServices::InputMethodController::GetInstance()->Close();
}

void IMFAdapterImpl::OnCursorUpdate(IMFAdapterCursorInfo cursorInfo)
{
    MiscServices::CursorInfo imfInfo = {
        .left = cursorInfo.left,
        .top = cursorInfo.top,
        .width = cursorInfo.width,
        .height = cursorInfo.height
    };
    WVLOG_D("CursorInfo left = %{public}f, top = %{public}f, width = %{public}f, height = %{public}f",
        cursorInfo.left, cursorInfo.top, cursorInfo.width, cursorInfo.height);
    MiscServices::InputMethodController::GetInstance()->OnCursorUpdate(imfInfo);
}

void IMFAdapterImpl::OnSelectionChange(std::u16string text, int start, int end)
{
    MiscServices::InputMethodController::GetInstance()->OnSelectionChange(text, start, end);
}
} // namespace OHOS::NWeb
