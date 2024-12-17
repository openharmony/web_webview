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

#ifndef ARK_WEB_GESTURE_EVENT_RESULT_H
#define ARK_WEB_GESTURE_EVENT_RESULT_H

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebGestureEventResult : public virtual ArkWebBaseRefCounted {
public:
    virtual ~ArkWebGestureEventResult() = default;

    /*--ark web()--*/
    virtual void SetGestureEventResult(bool result) = 0;

    /**
     * @Description: Set event consumption results.
     * @Input result: True if the event is consumed.
     * @Input stopPropagation: Stops the propagation of events farther along.
     * @Since 14001
     */
    /*--ark web()--*/
    virtual void SetGestureEventResultV2(bool result, bool stopPropagation) = 0;
};
} // namespace OHOS::ArkWeb

#endif // ARK_WEB_GESTURE_EVENT_RESULT_H
