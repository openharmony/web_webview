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

enum class IMFAdapterEnterKeyType { UNSPECIFIED = 0, NONE, GO, SEARCH, SEND, NEXT, DONE, PREVIOUS };

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

struct IMFAdapterInputAttribute {
    int32_t inputPattern = 0;
    int32_t enterKeyType = 0;
};

struct IMFAdapterSelectionRange {
    int32_t start = -1;
    int32_t end = -1;
};

struct IMFAdapterTextConfig {
    IMFAdapterInputAttribute inputAttribute = {};
    IMFAdapterCursorInfo cursorInfo = {};
    IMFAdapterSelectionRange range = {};
    uint32_t windowId = -1;
};

enum class IMFAdapterKeyboardStatus : int32_t { NONE = 0, HIDE, SHOW };

class IMFAdapterFunctionKey {
public:
    IMFAdapterEnterKeyType GetEnterKeyType() const
    {
        return enterKeyType;
    }

    void SetEnterKeyType(IMFAdapterEnterKeyType keyType)
    {
        enterKeyType = keyType;
    }

private:
    IMFAdapterEnterKeyType enterKeyType = IMFAdapterEnterKeyType::UNSPECIFIED;
};

class IMFTextListenerAdapter {
public:
    IMFTextListenerAdapter() = default;

    virtual ~IMFTextListenerAdapter() = default;

    virtual void InsertText(const std::u16string &text) = 0;

    virtual void DeleteForward(int32_t length) = 0;

    virtual void DeleteBackward(int32_t length) = 0;

    virtual void SendKeyEventFromInputMethod() = 0;

    virtual void SendKeyboardStatus(const IMFAdapterKeyboardStatus& keyboardStatus) = 0;

    virtual void SendFunctionKey(const IMFAdapterFunctionKey& functionKey) = 0;

    virtual void SetKeyboardStatus(bool status) = 0;

    virtual void MoveCursor(const IMFAdapterDirection direction) = 0;

    virtual void HandleSetSelection(int32_t start, int32_t end) = 0;

    virtual void HandleExtendAction(int32_t action) = 0;

    virtual void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) = 0;

    virtual int32_t GetTextIndexAtCursor() = 0;

    virtual std::u16string GetLeftTextOfCursor(int32_t number) = 0;

    virtual std::u16string GetRightTextOfCursor(int32_t number) = 0;
};

class IMFAdapter {
public:
    IMFAdapter() = default;

    virtual ~IMFAdapter() = default;

    virtual bool Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard) = 0;

    virtual bool Attach(
        std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard, const IMFAdapterTextConfig& config) = 0;

    virtual void ShowCurrentInput(const IMFAdapterTextInputType &inputType) = 0;

    virtual void HideTextInput() = 0;

    virtual void Close() = 0;

    virtual void OnCursorUpdate(IMFAdapterCursorInfo cursorInfo) = 0;

    virtual void OnSelectionChange(std::u16string text, int start, int end) = 0;
};
} // namespace OHOS::NWeb

#endif // IMF_ADAPTER_H
