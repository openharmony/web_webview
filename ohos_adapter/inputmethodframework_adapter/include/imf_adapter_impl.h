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

#ifndef IMF_ADAPTER_IMPL_H
#define IMF_ADAPTER_IMPL_H

#include "imf_adapter.h"
#include <inputmethod/inputmethod_controller_capi.h>

namespace OHOS::NWeb {

enum class IMFAdapterExtendAction : int32_t {
    SELECT_ALL,
    CUT,
    COPY,
    PASTE,
};

class IMFAdapterFunctionKeyAdapterImpl : public IMFAdapterFunctionKeyAdapter {
public:
    IMFAdapterFunctionKeyAdapterImpl() = default;

    ~IMFAdapterFunctionKeyAdapterImpl() = default;

    IMFAdapterEnterKeyType GetEnterKeyType() override
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

class IMFTextListenerAdapterImpl {
public:
    explicit IMFTextListenerAdapterImpl(const std::shared_ptr<IMFTextListenerAdapter>& listener);

    ~IMFTextListenerAdapterImpl();

    void InsertText(const std::u16string& text);

    void DeleteForward(int32_t length);

    void DeleteBackward(int32_t length);

    void SendKeyEventFromInputMethod(void);

    void SendKeyboardStatus(const IMFAdapterKeyboardStatus& keyboardStatus);

    void SendFunctionKey(std::shared_ptr<IMFAdapterFunctionKeyAdapterImpl> functionKey);

    void SetKeyboardStatus(bool status);

    void MoveCursor(const IMFAdapterDirection direction);

    void HandleSetSelection(int32_t start, int32_t end);

    void HandleExtendAction(int32_t action);

    void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip);

    int32_t GetTextIndexAtCursor();

    std::u16string GetLeftTextOfCursor(int32_t number);

    std::u16string GetRightTextOfCursor(int32_t number);

    int32_t SetPreviewText(const std::u16string& text, int32_t start, int32_t end);

    void FinishTextPreview();

    int32_t ReceivePrivateCommand(
        InputMethod_PrivateCommand *privateCommand[], size_t size);

private:
    std::shared_ptr<IMFTextListenerAdapter> listener_ = nullptr;
    const std::string PREVIEW_TEXT_STYLE_KEY = "previewTextStyle";
    const std::string PREVIEW_TEXT_STYLE_UNDERLINE = "underline";
    const std::string AUTO_FILL_PARAMS_USERNAME = "com.autofill.params.userName";
    const std::string AUTO_FILL_PARAMS_OTHERACCOUNT = "com.autofill.params.otherAccount";
};

class IMFTextEditorProxyImpl {
public:
    explicit IMFTextEditorProxyImpl();

    ~IMFTextEditorProxyImpl();

    static void GetTextConfigFunc(InputMethod_TextEditorProxy *proxy, InputMethod_TextConfig *config);
    static void InsertTextFunc(InputMethod_TextEditorProxy *proxy, const char16_t *text, size_t length);
    static void DeleteForwardFunc(InputMethod_TextEditorProxy *proxy, int32_t length);
    static void DeleteBackwardFunc(InputMethod_TextEditorProxy *proxy, int32_t length);
    static void SendKeyboardStatusFunc(InputMethod_TextEditorProxy *proxy, InputMethod_KeyboardStatus status);
    static void SendEnterKeyFunc(InputMethod_TextEditorProxy *proxy, InputMethod_EnterKeyType type);
    static void MoveCursorFunc(InputMethod_TextEditorProxy *proxy, InputMethod_Direction direction);
    static void HandleSetSelectionFunc(InputMethod_TextEditorProxy *proxy, int32_t start, int32_t end);
    static void HandleExtendActionFunc(InputMethod_TextEditorProxy *proxy, InputMethod_ExtendAction action);
    static void GetLeftTextOfCursorFunc(
        InputMethod_TextEditorProxy *proxy, int32_t number, char16_t text[], size_t *length);
    static void GetRightTextOfCursorFunc(
        InputMethod_TextEditorProxy *proxy, int32_t number, char16_t text[], size_t *length);
    static int32_t GetTextIndexAtCursorFunc(InputMethod_TextEditorProxy *proxy);
    static int32_t ReceivePrivateCommandFunc(
        InputMethod_TextEditorProxy *proxy, InputMethod_PrivateCommand *privateCommand[], size_t size);
    static int32_t SetPreviewTextFunc(
        InputMethod_TextEditorProxy *proxy, const char16_t *text, size_t length, int32_t start, int32_t end);
    static void FinishTextPreviewFunc(InputMethod_TextEditorProxy *proxy);
    static InputMethod_TextEditorProxy *TextEditorProxyCreate(
        std::shared_ptr<IMFTextListenerAdapter>listener, const std::shared_ptr<IMFTextConfigAdapter>config);
    static void TextEditorProxyDestroy(InputMethod_TextEditorProxy *textEditorProxy);
    static InputMethod_ErrorCode ConstructTextConfig(const std::shared_ptr<IMFTextConfigAdapter>config);
    static void DestroyTextConfig(void);
    static std::shared_ptr<IMFTextListenerAdapterImpl> textListener_;
    static InputMethod_TextConfig *textConfig_;
private:
    static InputMethod_ErrorCode TextEditorProxy_SetTextFunc(InputMethod_TextEditorProxy* textEditorProxy);
    static InputMethod_ErrorCode TextEditorProxy_SetFunc(InputMethod_TextEditorProxy* textEditorProxy);
};

class IMFAdapterImpl : public IMFAdapter {
public:
    IMFAdapterImpl() = default;

    ~IMFAdapterImpl() override
    {
        OH_TextEditorProxy_Destroy(textEditorProxy_);
        Close();
    }

    bool Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard) override;

    bool Attach(std::shared_ptr<IMFTextListenerAdapter> listener, bool isShowKeyboard,
        const std::shared_ptr<IMFTextConfigAdapter> config, bool isResetListener) override;

    void ShowCurrentInput(const IMFAdapterTextInputType& inputType) override;

    void HideTextInput() override;

    void Close() override;

    void OnCursorUpdate(const std::shared_ptr<IMFCursorInfoAdapter> cursorInfo) override;

    void OnSelectionChange(std::u16string text, int start, int end) override;

    bool SendPrivateCommand(const std::string& commandKey, const std::string& commandValue) override;

private:
    InputMethod_TextEditorProxy *textEditorProxy_ = nullptr;
    InputMethod_InputMethodProxy *inputMethodProxy_ = nullptr;
    bool ParseFillContentJsonValue(const std::string& jsonStr,
        std::vector<InputMethod_PrivateCommand *> privateCommands);
};
} // namespace OHOS::NWeb

#endif // IMF_ADAPTER_IMPL_H
