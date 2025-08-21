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

#ifndef ARK_IMFTEXT_LISTENER_ADAPTER_WRAPPER_H
#define ARK_IMFTEXT_LISTENER_ADAPTER_WRAPPER_H
#pragma once

#include "imf_adapter.h"
#include "ohos_adapter/include/ark_imf_adapter.h"

namespace OHOS::ArkWeb {

class ArkIMFTextListenerAdapterWrapper : public OHOS::NWeb::IMFTextListenerAdapter {
public:
    explicit ArkIMFTextListenerAdapterWrapper(ArkWebRefPtr<ArkIMFTextListenerAdapter>);

    void InsertText(const std::u16string& text) override;

    void DeleteForward(int32_t length) override;

    void DeleteBackward(int32_t length) override;

    void SendKeyEventFromInputMethod() override;

    void SendKeyboardStatus(const OHOS::NWeb::IMFAdapterKeyboardStatus& keyboardStatus) override;

    void SendFunctionKey(std::shared_ptr<OHOS::NWeb::IMFAdapterFunctionKeyAdapter> functionKey) override;

    void SetKeyboardStatus(bool status) override;

    void MoveCursor(const OHOS::NWeb::IMFAdapterDirection direction) override;

    void HandleSetSelection(int32_t start, int32_t end) override;

    void HandleExtendAction(int32_t action) override;

    void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) override;

    int32_t GetTextIndexAtCursor() override;

    std::u16string GetLeftTextOfCursor(int32_t number) override;

    std::u16string GetRightTextOfCursor(int32_t number) override;

    int32_t SetPreviewText(const std::u16string& text, int32_t start, int32_t end) override;

    void FinishTextPreview() override;

    void SetNeedUnderLine(bool isNeedUnderline) override;

    void AutoFillWithIMFEvent(bool isUsername, bool isOtherAccount, bool isNewPassword,
        const std::string& content) override;

    void KeyboardUpperRightCornerHide() override;

    void WebSetImeShow(bool visible) override;
private:
    ArkWebRefPtr<ArkIMFTextListenerAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_IMFTEXT_LISTENER_ADAPTER_WRAPPER_H
