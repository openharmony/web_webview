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
enum class IMFAdapterTextInputType : int32_t {
    NONE = -1,
    TEXT = 0,
    MULTILINE,
    NUMBER,
    PHONE,
    DATETIME,
    EMAIL_ADDRESS,
    URL,
    VISIBLE_PASSWORD,
    NUMBER_PASSWORD,
    SCREEN_LOCK_PASSWORD,
    USER_NAME,
    NEW_PASSWORD,
    NUMBER_DECIMAL,
};

enum class IMFAdapterEnterKeyType : int32_t {
    UNSPECIFIED = 0,
    NONE,
    GO,
    SEARCH,
    SEND,
    NEXT,
    DONE,
    PREVIOUS,
    NEW_LINE,
};

enum class IMFAdapterDirection : int32_t {
    NONE = 0,
    UP = 1,
    DOWN,
    LEFT,
    RIGHT,
};

class IMFCursorInfoAdapter {
public:
    IMFCursorInfoAdapter() = default;

    virtual ~IMFCursorInfoAdapter() = default;

    virtual double GetLeft() = 0;

    virtual double GetTop() = 0;

    virtual double GetWidth() = 0;

    virtual double GetHeight() = 0;
};

class IMFInputAttributeAdapter {
public:
    IMFInputAttributeAdapter() = default;

    virtual ~IMFInputAttributeAdapter() = default;

    virtual int32_t GetInputPattern() = 0;

    virtual int32_t GetEnterKeyType() = 0;
};

class IMFSelectionRangeAdapter {
public:
    IMFSelectionRangeAdapter() = default;

    virtual ~IMFSelectionRangeAdapter() = default;

    virtual int32_t GetStart() = 0;

    virtual int32_t GetEnd() = 0;
};

class IMFTextConfigAdapter {
public:
    IMFTextConfigAdapter() = default;

    virtual ~IMFTextConfigAdapter() = default;

    virtual std::shared_ptr<IMFInputAttributeAdapter> GetInputAttribute() = 0;

    virtual std::shared_ptr<IMFCursorInfoAdapter> GetCursorInfo() = 0;

    virtual std::shared_ptr<IMFSelectionRangeAdapter> GetSelectionRange() = 0;

    virtual uint32_t GetWindowId() = 0;

    virtual double GetPositionY()
    {
        return 0.0;
    }

    virtual double GetHeight()
    {
        return 0.0;
    }
};

enum class IMFAdapterKeyboardStatus : int32_t { NONE = 0, HIDE, SHOW };

class IMFAdapterFunctionKeyAdapter {
public:
    IMFAdapterFunctionKeyAdapter() = default;

    virtual ~IMFAdapterFunctionKeyAdapter() = default;

    virtual IMFAdapterEnterKeyType GetEnterKeyType() = 0;
};

class IMFTextListenerAdapter {
public:
    IMFTextListenerAdapter() = default;

    virtual ~IMFTextListenerAdapter() = default;

    virtual void InsertText(const std::u16string& text) = 0;

    virtual void DeleteForward(int32_t length) = 0;

    virtual void DeleteBackward(int32_t length) = 0;

    virtual void SendKeyEventFromInputMethod() = 0;

    virtual void SendKeyboardStatus(const IMFAdapterKeyboardStatus& keyboardStatus) = 0;

    virtual void SendFunctionKey(std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey) = 0;

    virtual void SetKeyboardStatus(bool status) = 0;

    virtual void MoveCursor(const IMFAdapterDirection direction) = 0;

    virtual void HandleSetSelection(int32_t start, int32_t end) = 0;

    virtual void HandleExtendAction(int32_t action) = 0;

    virtual void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) = 0;

    virtual int32_t GetTextIndexAtCursor() = 0;

    virtual std::u16string GetLeftTextOfCursor(int32_t number) = 0;

    virtual std::u16string GetRightTextOfCursor(int32_t number) = 0;

    virtual int32_t SetPreviewText(const std::u16string& text, int32_t start, int32_t end) = 0;

    virtual void FinishTextPreview() = 0;

    virtual void SetNeedUnderLine(bool isNeedUnderline) = 0;

    virtual void AutoFillWithIMFEvent(bool isUsername, bool isOtherAccount, bool isNewPassword,
        const std::string& content) {}

    virtual void KeyboardUpperRightCornerHide() {};
};

class IMFAdapter {
public:
    IMFAdapter() = default;

    virtual ~IMFAdapter() = default;

    virtual bool Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard) = 0;

    virtual bool Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard,
        const std::shared_ptr<IMFTextConfigAdapter> config, bool isResetListener) = 0;

    virtual bool AttachWithRequestKeyboardReason(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard,
        const std::shared_ptr<IMFTextConfigAdapter> config, bool isResetListener, int32_t requestKeyboardReason) = 0;

    virtual void ShowCurrentInput(const IMFAdapterTextInputType& inputType) = 0;

    virtual void HideTextInput() = 0;

    virtual void Close() = 0;

    virtual void OnCursorUpdate(const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo) = 0;

    virtual void OnSelectionChange(std::u16string text, int start, int end) = 0;

    virtual bool SendPrivateCommand(const std::string& commandKey, const std::string& commandValue)
    {
        return false;
    }
};

} // namespace OHOS::NWeb

#endif // IMF_ADAPTER_H
