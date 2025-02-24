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

#ifndef ARK_WEB_KEYBOARD_EVENT_H_
#define ARK_WEB_KEYBOARD_EVENT_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {
/*--ark web(source=webview)--*/
class ArkWebKeyboardEvent : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @Description: Get key code.
     * @Return: return key code.
     */
    /*--ark web()--*/
    virtual int32_t GetKeyCode() = 0;

    /**
     * @Description: Get key action.
     * @Return: return key action.
     */
    /*--ark web()--*/
    virtual int32_t GetAction() = 0;

    /**
     * @Description: Get key unicode.
     * @Return: return key unicode.
     */
    /*--ark web()--*/
    virtual int32_t GetUnicode() = 0;

    /**
     * @Description: Get enable CapsLock.
     * @Return: return enable CapsLock.
     */
    /*--ark web()--*/
    virtual bool IsEnableCapsLock() = 0;

    /**
     * @Description: Get key unicode.
     * @Return: return key unicode.
     */
    /*--ark web()--*/
    virtual ArkWebInt32Vector GetPressKeyCodes() = 0;
};

} //namesapca OHOS::ArkWeb

#endif //  ARK_WEB_KEYBOARD_EVENT_H_
