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

#ifndef ARK_WEB_TOUCH_HANDLE_STATE_IMPL_H_
#define ARK_WEB_TOUCH_HANDLE_STATE_IMPL_H_
#pragma once

#include "include/nweb_touch_handle_state.h"
#include "ohos_nweb/include/ark_web_touch_handle_state.h"

namespace OHOS::ArkWeb {

class ArkWebTouchHandleStateImpl : public ArkWebTouchHandleState {
    IMPLEMENT_REFCOUNTING(ArkWebTouchHandleStateImpl);

public:
    ArkWebTouchHandleStateImpl(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_touch_handle_state);
    ~ArkWebTouchHandleStateImpl() = default;

    int32_t GetX() override;

    int32_t GetY() override;

    bool IsEnable() override;

    float GetAlpha() override;

    float GetEdgeHeight() override;

    int32_t GetViewPortX() override;

    int32_t GetViewPortY() override;

    int32_t GetTouchHandleId() override;

    int GetTouchHandleType() override;

    bool IsDragging() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> nweb_touch_handle_state_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_TOUCH_HANDLE_STATE_IMPL_H_
