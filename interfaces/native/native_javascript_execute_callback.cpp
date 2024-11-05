/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "native_javascript_execute_callback.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
void NativeJavaScriptExecuteCallback::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    if (callbackNative_) {
        if (result && result->IsString()) {
            callbackNative_(result->GetString().c_str());
        } else {
            WVLOG_E("native RunJavaScript result is not string");
            callbackNative_("null");
        }
    } else {
        WVLOG_E("native RunJavaScript callback is null");
    }
}
} // namespace OHOS::NWeb