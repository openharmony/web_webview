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

#ifndef ARK_WEB_BOOL_VALUE_CALLBACK_WRAPPER_H_
#define ARK_WEB_BOOL_VALUE_CALLBACK_WRAPPER_H_
#pragma once

#include "include/nweb_value_callback.h"
#include "ohos_nweb/include/ark_web_bool_value_callback.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebBoolValueCallbackWrapper : public OHOS::NWeb::NWebBoolValueCallback {
public:
    ArkWebBoolValueCallbackWrapper(ArkWebRefPtr<ArkWebBoolValueCallback> ark_web_bool_value_callback);
    ~ArkWebBoolValueCallbackWrapper() = default;

    void OnReceiveValue(bool value) override;

private:
    ArkWebRefPtr<ArkWebBoolValueCallback> ark_web_bool_value_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_BOOL_VALUE_CALLBACK_WRAPPER_H_
