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

#ifndef ARK_IMFADAPTER_WRAPPER_H
#define ARK_IMFADAPTER_WRAPPER_H
#pragma once

#include "imf_adapter.h"
#include "ohos_adapter/include/ark_imf_adapter.h"

namespace OHOS::ArkWeb {

class ArkIMFAdapterWrapper : public OHOS::NWeb::IMFAdapter {
public:
    explicit ArkIMFAdapterWrapper(ArkWebRefPtr<ArkIMFAdapter>);

    bool Attach(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> listener, bool isShowKeyboard) override;

    bool Attach(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> listener, bool isShowKeyboard,
        const std::shared_ptr<OHOS::NWeb::IMFTextConfigAdapter> config, bool isResetListener) override;

    bool AttachWithRequestKeyboardReason(std::shared_ptr<OHOS::NWeb::IMFTextListenerAdapter> listener,
        bool isShowKeyboard, const std::shared_ptr<OHOS::NWeb::IMFTextConfigAdapter> config, bool isResetListener,
        int32_t requestKeyboardReason) override;

    void ShowCurrentInput(const OHOS::NWeb::IMFAdapterTextInputType& inputType) override;

    void HideTextInput() override;

    void Close() override;

    void OnCursorUpdate(const std::shared_ptr<OHOS::NWeb::IMFCursorInfoAdapter> cursorInfo) override;

    void OnSelectionChange(std::u16string text, int start, int end) override;

    bool SendPrivateCommand(const std::string& commandKey, const std::string& commandValue) override;

private:
    ArkWebRefPtr<ArkIMFAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_IMFADAPTER_WRAPPER_H
