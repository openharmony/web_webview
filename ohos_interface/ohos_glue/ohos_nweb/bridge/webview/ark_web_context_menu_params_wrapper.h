/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_CONTEXT_MENU_PARAMS_WRAPPER_H_
#define ARK_WEB_CONTEXT_MENU_PARAMS_WRAPPER_H_
#pragma once

#include "include/nweb_context_menu_params.h"
#include "ohos_nweb/include/ark_web_context_menu_params.h"

namespace OHOS::ArkWeb {

using ArkWebMediaType = OHOS::NWeb::NWebContextMenuParams::ContextMenuMediaType;
using ArkWebSourceType = OHOS::NWeb::NWebContextMenuParams::ContextMenuSourceType;
using ArkWebInputFieldType = OHOS::NWeb::NWebContextMenuParams::ContextMenuInputFieldType;

class ArkWebContextMenuParamsWrapper : public OHOS::NWeb::NWebContextMenuParams {
public:
    ArkWebContextMenuParamsWrapper(ArkWebRefPtr<ArkWebContextMenuParams> ark_web_context_menu_params);
    ~ArkWebContextMenuParamsWrapper() = default;

    int32_t GetXCoord() override;

    int32_t GetYCoord() override;

    bool IsEditable() override;

    std::string GetLinkUrl() override;

    std::string GetPageUrl() override;

    std::string GetSourceUrl() override;

    std::string GetTitleText() override;

    std::string GetSelectionText() override;

    ArkWebMediaType GetMediaType() override;

    ArkWebSourceType GetSourceType() override;

    ArkWebInputFieldType GetInputFieldType() override;

    bool HasImageContents() override;

    int32_t GetEditStateFlags() override;

    std::string GetUnfilteredLinkUrl() override;

    int32_t GetContextMenuTypeFlags() override;

    void GetImageRect(int& x, int& y, int& w, int& h) override;

private:
    ArkWebRefPtr<ArkWebContextMenuParams> ark_web_context_menu_params_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CONTEXT_MENU_PARAMS_WRAPPER_H_
