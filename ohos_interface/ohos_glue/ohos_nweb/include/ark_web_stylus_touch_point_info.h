/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_STYLUS_TOUCH_POINT_INFO_H_
#define ARK_WEB_STYLUS_TOUCH_POINT_INFO_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebStylusTouchPointInfo : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetId() = 0;

    /*--ark web()--*/
    virtual double GetX() = 0;

    /*--ark web()--*/
    virtual double GetY() = 0;

    /*--ark web()--*/
    virtual int32_t GetSourceTool() = 0;

    /*--ark web()--*/
    virtual float GetForce() = 0;

    /*--ark web()--*/
    virtual float GetTiltX() = 0;

    /*--ark web()--*/
    virtual float GetTiltY() = 0;

    /*--ark web()--*/
    virtual float GetRollAngle() = 0;

    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;
};

} // namespace OHOS::ArkWeb
#endif // ARK_WEB_STYLUS_TOUCH_POINT_INFO_H_
