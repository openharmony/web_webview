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

#ifndef ARK_WEB_NATIVE_MESSAGE_CALLBACK_WRAPPER_H_
#define ARK_WEB_NATIVE_MESSAGE_CALLBACK_WRAPPER_H_
#pragma once

#include "include/nweb_native_message_callback.h"
#include "ohos_nweb/include/ark_web_native_message_callback.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebNativeMessageCallbackWrapper : public OHOS::NWeb::NWebNativeMessageCallback {
public:
    ArkWebNativeMessageCallbackWrapper(ArkWebRefPtr<ArkWebNativeMessageCallback> ark_web_native_message_callback);
    ~ArkWebNativeMessageCallbackWrapper() = default;

    virtual void OnConnect(int nativeInfo) override;
    virtual void OnDisconnect(int nativeInfo) override;
    virtual void OnFailed(int code) override;

private:
    ArkWebRefPtr<ArkWebNativeMessageCallback> ark_web_native_message_callback_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_MESSAGE_CALLBACK_WRAPPER_H_
