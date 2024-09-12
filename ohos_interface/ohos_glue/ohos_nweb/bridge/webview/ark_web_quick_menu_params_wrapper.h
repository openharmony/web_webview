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

#ifndef ARK_WEB_QUICK_MENU_PARAMS_WRAPPER_H_
#define ARK_WEB_QUICK_MENU_PARAMS_WRAPPER_H_
#pragma once

#include "include/nweb_context_menu_params.h"
#include "ohos_nweb/bridge/ark_web_touch_handle_state_wrapper.h"
#include "ohos_nweb/include/ark_web_quick_menu_params.h"

namespace OHOS::ArkWeb {

class ArkWebQuickMenuParamsWrapper : public OHOS::NWeb::NWebQuickMenuParams {
public:
    ArkWebQuickMenuParamsWrapper(ArkWebRefPtr<ArkWebQuickMenuParams> ark_web_quick_menu_params);
    ~ArkWebQuickMenuParamsWrapper() = default;

    int32_t GetXCoord() override;

    int32_t GetYCoord() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetEditStateFlags() override;

    int32_t GetSelectX() override;

    int32_t GetSelectY() override;

    int32_t GetSelectWidth() override;

    int32_t GetSelectXHeight() override;

    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> GetTouchHandleState(ArkWebTouchHandleType type) override;

    bool GetIsLongPressActived() override;

private:
    ArkWebRefPtr<ArkWebQuickMenuParams> ark_web_quick_menu_params_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_QUICK_MENU_PARAMS_WRAPPER_H_
