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

#ifndef ARK_WEB_CUSTOM_KEYBOARD_HANDLER_H_
#define ARK_WEB_CUSTOM_KEYBOARD_HANDLER_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebCustomKeyboardHandler : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void InsertText(const ArkWebString &text) = 0;

    /*--ark web()--*/
    virtual void DeleteForward(int32_t length) = 0;

    /*--ark web()--*/
    virtual void DeleteBackward(int32_t length) = 0;

    /*--ark web()--*/
    virtual void SendFunctionKey(int32_t key) = 0;

    /*--ark web()--*/
    virtual void Close() = 0;
};
} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CUSTOM_KEYBOARD_HANDLER_H_
