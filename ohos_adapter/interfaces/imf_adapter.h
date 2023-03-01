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

#ifndef IMF_ADAPTER_H
#define IMF_ADAPTER_H

#include <string>

namespace OHOS::NWeb {
enum class IMFAdapterTextInputType {
    NONE = -1,
    TEXT = 0,
    MULTILINE,
    NUMBER,
    PHONE,
    DATETIME,
    EMAIL_ADDRESS,
    URL,
    VISIBLE_PASSWORD,
};

enum class IMFAdapterDirection {
    NONE = 0,
    UP = 1,
    DOWN,
    LEFT,
    RIGHT,
};

struct IMFAdapterCursorInfo {
    double left = 0.0;
    double top = 0.0;
    double width = 0.0;
    double height = 0.0;
};

enum class IMFAdapterKeyboardStatus : int32_t { NONE = 0, HIDE, SHOW };

enum class IMFAdapterFunctionKey : int32_t { NONE = 0, CONFIRM };

struct IMFAdapterKeyboardInfo {
    IMFAdapterKeyboardStatus keyboardStatus = IMFAdapterKeyboardStatus::NONE;
    IMFAdapterFunctionKey functionKey = IMFAdapterFunctionKey::NONE;
};

class IMFTextListenerAdapter {
public:
    IMFTextListenerAdapter() = default;

    virtual ~IMFTextListenerAdapter() = default;

    virtual void InsertText(const std::u16string &text) = 0;

    virtual void DeleteForward(int32_t length) = 0;

    virtual void DeleteBackward(int32_t length) = 0;

    virtual void SendKeyEventFromInputMethod() = 0;

    virtual void SendKeyboardInfo(const IMFAdapterKeyboardInfo &info) = 0;

    virtual void SetKeyboardStatus(bool status) = 0;

    virtual void MoveCursor(const IMFAdapterDirection direction) = 0;

    virtual void HandleSetSelection(int32_t start, int32_t end) = 0;

    virtual void HandleExtendAction(int32_t action) = 0;

    virtual void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) = 0;
};

class IMFAdapter {
public:
    IMFAdapter() = default;

    virtual ~IMFAdapter() = default;

    virtual bool Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard) = 0;

    virtual void ShowCurrentInput(const IMFAdapterTextInputType &inputType) = 0;

    virtual void HideTextInput() = 0;

    virtual void Close() = 0;

    virtual void OnCursorUpdate(IMFAdapterCursorInfo cursorInfo) = 0;

    virtual void OnSelectionChange(std::u16string text, int start, int end) = 0;
};
} // namespace OHOS::NWeb

#endif // IMF_ADAPTER_H
