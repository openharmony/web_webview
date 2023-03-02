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
IMFTextListenerAdapterImpl::IMFTextListenerAdapterImpl(const std::shared_ptr<IMFTextListenerAdapter> &listener)
    : listener_(listener) {};

IMFTextListenerAdapterImpl::~IMFTextListenerAdapterImpl()
{
    listener_ = nullptr;
}

void IMFTextListenerAdapterImpl::InsertText(const std::u16string &text)
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

void IMFTextListenerAdapterImpl::SendKeyEventFromInputMethod(const MiscServices::KeyEvent &event)
{
    (void)event;
    if (listener_) {
        listener_->SendKeyEventFromInputMethod();
    }
}

void IMFTextListenerAdapterImpl::SendKeyboardInfo(const MiscServices::KeyboardInfo &info)
{
    if (listener_) {
        IMFAdapterKeyboardInfo adapterInfo;
        if (info.GetKeyboardStatus() == MiscServices::KeyboardStatus::SHOW) {
            adapterInfo.keyboardStatus = IMFAdapterKeyboardStatus::SHOW;
        } else if (info.GetKeyboardStatus() == MiscServices::KeyboardStatus::HIDE) {
            adapterInfo.keyboardStatus = IMFAdapterKeyboardStatus::HIDE;
        }
        if (info.GetFunctionKey() == MiscServices::FunctionKey::CONFIRM) {
            adapterInfo.functionKey = IMFAdapterFunctionKey::CONFIRM;
        }
        listener_->SendKeyboardInfo(adapterInfo);
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
    MiscServices::InputMethodController::GetInstance()->Attach(textListener_, isShowKeyboard);
    return true;
}

void IMFAdapterImpl::ShowCurrentInput(const IMFAdapterTextInputType &inputType)
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
    MiscServices::InputMethodController::GetInstance()->OnCursorUpdate(imfInfo);
}

void IMFAdapterImpl::OnSelectionChange(std::u16string text, int start, int end)
{
    MiscServices::InputMethodController::GetInstance()->OnSelectionChange(text, start, end);
}
} // namespace OHOS::NWeb
