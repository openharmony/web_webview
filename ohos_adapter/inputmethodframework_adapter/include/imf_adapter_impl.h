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
#include "input_method_controller.h"

namespace OHOS::NWeb {

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

class IMFTextListenerAdapterImpl : public MiscServices::OnTextChangedListener {
public:
    explicit IMFTextListenerAdapterImpl(const std::shared_ptr<IMFTextListenerAdapter>& listener);

    ~IMFTextListenerAdapterImpl();

    void InsertText(const std::u16string& text) override;

    void DeleteForward(int32_t length) override;

    void DeleteBackward(int32_t length) override;

    void SendKeyEventFromInputMethod(const MiscServices::KeyEvent& event) override;

    void SendKeyboardStatus(const MiscServices::KeyboardStatus& keyboardStatus) override;

    void SendFunctionKey(const MiscServices::FunctionKey& functionKey) override;

    void SetKeyboardStatus(bool status) override;

    void MoveCursor(const MiscServices::Direction direction) override;

    void HandleSetSelection(int32_t start, int32_t end) override;

    void HandleExtendAction(int32_t action) override;

    void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) override;

    int32_t GetTextIndexAtCursor() override;

    std::u16string GetLeftTextOfCursor(int32_t number) override;

    std::u16string GetRightTextOfCursor(int32_t number) override;

    int32_t SetPreviewText(const std::u16string& text, const MiscServices::Range& range) override;

    void FinishTextPreview() override;

    int32_t ReceivePrivateCommand(
        const std::unordered_map<std::string, MiscServices::PrivateDataValue>& privateCommand) override;

    void NotifyPanelStatusInfo(const MiscServices::PanelStatusInfo& info) override;

private:
    std::shared_ptr<IMFTextListenerAdapter> listener_ = nullptr;
    const std::string PREVIEW_TEXT_STYLE_KEY = "previewTextStyle";
    const std::string PREVIEW_TEXT_STYLE_UNDERLINE = "underline";
    const std::string AUTO_FILL_PARAMS_USERNAME = "com.autofill.params.userName";
    const std::string AUTO_FILL_PARAMS_OTHERACCOUNT = "com.autofill.params.otherAccount";
};

class IMFAdapterImpl : public IMFAdapter {
public:
    IMFAdapterImpl() = default;

    ~IMFAdapterImpl() override = default;

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
    sptr<MiscServices::OnTextChangedListener> textListener_ = nullptr;

    bool ParseFillContentJsonValue(const std::string& jsonStr,
        std::unordered_map<std::string, std::variant<std::string, bool, int32_t>>& map);
};
} // namespace OHOS::NWeb

#endif // IMF_ADAPTER_IMPL_H
