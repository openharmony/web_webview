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

#ifndef ARK_IMF_ADAPTER_IMPL_H
#define ARK_IMF_ADAPTER_IMPL_H
#pragma once

#include "imf_adapter.h"
#include "ohos_adapter/include/ark_imf_adapter.h"

namespace OHOS::ArkWeb {

class ArkIMFAdapterImpl : public ArkIMFAdapter {
public:
    explicit ArkIMFAdapterImpl(std::shared_ptr<OHOS::NWeb::IMFAdapter>);

    bool Attach(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard) override;

    bool Attach(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard,
        ArkWebRefPtr<ArkIMFTextConfigAdapter> config, bool isResetListener) override;

    bool AttachWithRequestKeyboardReason(ArkWebRefPtr<ArkIMFTextListenerAdapter> listener, bool isShowKeyboard,
        ArkWebRefPtr<ArkIMFTextConfigAdapter> config, bool isResetListener, int32_t requestKeyboardReason) override;

    void ShowCurrentInput(const int32_t& inputType) override;

    void HideTextInput() override;

    void Close() override;

    void OnCursorUpdate(ArkWebRefPtr<ArkIMFCursorInfoAdapter> cursorInfo) override;

    void OnSelectionChange(ArkWebU16String& text, int start, int end) override;

    bool SendPrivateCommand(const ArkWebString& commandKey, const ArkWebString& commandValue) override;

private:
    std::shared_ptr<OHOS::NWeb::IMFAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkIMFAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_IMF_ADAPTER_IMPL_H
