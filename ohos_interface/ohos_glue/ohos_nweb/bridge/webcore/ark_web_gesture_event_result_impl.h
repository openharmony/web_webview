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

#ifndef ARK_WEB_GESTURE_EVENT_RESULT_IMPL_H_
#define ARK_WEB_GESTURE_EVENT_RESULT_IMPL_H_
#pragma once

#include "include/nweb_gesture_event_result.h"
#include "ohos_nweb/include/ark_web_gesture_event_result.h"

namespace OHOS::ArkWeb {

class ArkWebGestureEventResultImpl : public ArkWebGestureEventResult {
    IMPLEMENT_REFCOUNTING(ArkWebGestureEventResultImpl);

public:
    ArkWebGestureEventResultImpl(std::shared_ptr<OHOS::NWeb::NWebGestureEventResult> result);
    ~ArkWebGestureEventResultImpl() = default;

    /**
     * @brief Handle the result if the user used.
     */
    void SetGestureEventResult(bool result) override;

    /**
     * @brief Handle the result if the user used.
     */
    void SetGestureEventResultV2(bool result, bool stopPropagation) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebGestureEventResult> result_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_GESTURE_EVENT_RESULT_IMPL_H_
