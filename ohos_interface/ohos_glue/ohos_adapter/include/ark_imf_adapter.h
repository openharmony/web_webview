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

#ifndef ARK_IMF_ADAPTER_H
#define ARK_IMF_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkIMFCursorInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual double GetLeft() = 0;

    /*--ark web()--*/
    virtual double GetTop() = 0;

    /*--ark web()--*/
    virtual double GetWidth() = 0;

    /*--ark web()--*/
    virtual double GetHeight() = 0;
};

/*--ark web(source=webcore)--*/
class ArkIMFInputAttributeAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetInputPattern() = 0;

    /*--ark web()--*/
    virtual int32_t GetEnterKeyType() = 0;
};

/*--ark web(source=webcore)--*/
class ArkIMFSelectionRangeAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetStart() = 0;

    /*--ark web()--*/
    virtual int32_t GetEnd() = 0;
};

/*--ark web(source=webcore)--*/
class ArkIMFTextConfigAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkIMFInputAttributeAdapter> GetInputAttribute() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkIMFCursorInfoAdapter> GetCursorInfo() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkIMFSelectionRangeAdapter> GetSelectionRange() = 0;

    /*--ark web()--*/
    virtual uint32_t GetWindowId() = 0;

    /*--ark web()--*/
    virtual double GetPositionY() = 0;

    /*--ark web()--*/
    virtual double GetHeight() = 0;
};

/*--ark web(source=webview)--*/
class ArkIMFAdapterFunctionKeyAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetEnterKeyType() = 0;
};

/*--ark web(source=webcore)--*/
class ArkIMFTextListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void InsertText(const ArkWebU16String& text) = 0;

    /*--ark web()--*/
    virtual void DeleteForward(int32_t length) = 0;

    /*--ark web()--*/
    virtual void DeleteBackward(int32_t length) = 0;

    /*--ark web()--*/
    virtual void SendKeyEventFromInputMethod() = 0;

    /*--ark web()--*/
    virtual void SendKeyboardStatus(const int32_t& keyboardStatus) = 0;

    /*--ark web()--*/
    virtual void SendFunctionKey(ArkWebRefPtr<ArkIMFAdapterFunctionKeyAdapter> functionKey) = 0;

    /*--ark web()--*/
    virtual void SetKeyboardStatus(bool status) = 0;

    /*--ark web()--*/
    virtual void MoveCursor(const uint32_t direction) = 0;

    /*--ark web()--*/
    virtual void HandleSetSelection(int32_t start, int32_t end) = 0;

    /*--ark web()--*/
    virtual void HandleExtendAction(int32_t action) = 0;

    /*--ark web()--*/
    virtual void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) = 0;

    /*--ark web()--*/
    virtual int32_t GetTextIndexAtCursor() = 0;

    /*--ark web()--*/
    virtual ArkWebU16String GetLeftTextOfCursor(int32_t number) = 0;

    /*--ark web()--*/
    virtual ArkWebU16String GetRightTextOfCursor(int32_t number) = 0;

    /*--ark web()--*/
    virtual int32_t SetPreviewText(const ArkWebU16String& text, int32_t start, int32_t end) = 0;

    /*--ark web()--*/
    virtual void FinishTextPreview() = 0;

    /*--ark web()--*/
    virtual void SetNeedUnderLine(bool isNeedUnderline) = 0;

    /*--ark web()--*/
    virtual void AutoFillWithIMFEvent(bool isUsername, bool isOtherAccount, bool isNewPassword,
        const ArkWebString& content) = 0;
        
    /*--ark web()--*/
    virtual void KeyboardUpperRightCornerHide() = 0;
};

/*--ark web(source=webview)--*/
class ArkIMFAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool Attach(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard) = 0;

    /*--ark web()--*/
    virtual bool Attach(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard,
        ArkWebRefPtr<ArkIMFTextConfigAdapter> config, bool isResetListener) = 0;

    /*--ark web()--*/
    virtual bool AttachWithRequestKeyboardReason(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard,
        ArkWebRefPtr<ArkIMFTextConfigAdapter> config, bool isResetListener, int32_t requestKeyboardReason) = 0;

    /*--ark web()--*/
    virtual void ShowCurrentInput(const int32_t& inputType) = 0;

    /*--ark web()--*/
    virtual void HideTextInput() = 0;

    /*--ark web()--*/
    virtual void Close() = 0;

    /*--ark web()--*/
    virtual void OnCursorUpdate(ArkWebRefPtr<ArkIMFCursorInfoAdapter> cursorInfo) = 0;

    /*--ark web()--*/
    virtual void OnSelectionChange(ArkWebU16String& text, int start, int end) = 0;

    /*--ark web()--*/
    virtual bool SendPrivateCommand(const ArkWebString& commandKey, const ArkWebString& commandValue) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_IMF_ADAPTER_H
