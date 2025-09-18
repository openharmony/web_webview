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

#include "web_extension_connection_callback.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
void WebExtensionConnectionCallback::OnConnect(ConnectionNativeInfo& info)
{
    if (callback_) {
        callback_->OnExtensionConnect(info);
    }
}

void WebExtensionConnectionCallback::OnDisconnect(ConnectionNativeInfo& info)
{
    if (callback_) {
        callback_->OnExtensionDisconnect(info);
    }
}

void WebExtensionConnectionCallback::OnFailed(int32_t errorNum)
{
    if (callback_) {
        callback_->OnExtensionFailed(connectionId_, errorNum);
    }
}
} // namespace OHOS::NWeb
