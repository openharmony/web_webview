/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_SAVE_COOKIE_CALLBACK_H
#define NWEB_SAVE_COOKIE_CALLBACK_H

#include "nweb_value_callback.h"

namespace OHOS::NWeb {
class NWebSaveCookieCallback : public OHOS::NWeb::NWebValueCallback<bool> {
public:
    NWebSaveCookieCallback() = default;
    ~NWebSaveCookieCallback() = default;

    void OnReceiveValue(bool result) override
    {
        if (callback_) {
            callback_(result);
        }
    }
    void SetCallBack(const std::function<void(bool)> &&callback)
    {
        callback_ = callback;
    }

private:
    std::function<void(bool)> callback_;
};
} // namespace OHOS::NWeb
#endif // NWEB_SAVE_COOKIE_CALLBACK_H
