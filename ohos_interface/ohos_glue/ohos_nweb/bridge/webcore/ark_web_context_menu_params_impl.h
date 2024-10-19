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

#ifndef ARK_WEB_CONTEXT_MENU_PARAMS_IMPL_H_
#define ARK_WEB_CONTEXT_MENU_PARAMS_IMPL_H_
#pragma once

#include "include/nweb_context_menu_params.h"
#include "ohos_nweb/include/ark_web_context_menu_params.h"

namespace OHOS::ArkWeb {

class ArkWebContextMenuParamsImpl : public ArkWebContextMenuParams {
    IMPLEMENT_REFCOUNTING(ArkWebContextMenuParamsImpl);

public:
    ArkWebContextMenuParamsImpl(std::shared_ptr<OHOS::NWeb::NWebContextMenuParams> nweb_context_menu_params);
    ~ArkWebContextMenuParamsImpl() = default;

    int32_t GetXCoord() override;

    int32_t GetYCoord() override;

    bool IsEditable() override;

    ArkWebString GetLinkUrl() override;

    ArkWebString GetPageUrl() override;

    ArkWebString GetSourceUrl() override;

    ArkWebString GetTitleText() override;

    ArkWebString GetSelectionText() override;

    int GetMediaType() override;

    int GetSourceType() override;

    int GetInputFieldType() override;

    bool HasImageContents() override;

    int32_t GetEditStateFlags() override;

    ArkWebString GetUnfilteredLinkUrl() override;

    int32_t GetContextMenuTypeFlags() override;

    void GetImageRect(int& x, int& y, int& w, int& h) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebContextMenuParams> nweb_context_menu_params_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CONTEXT_MENU_PARAMS_IMPL_H_
