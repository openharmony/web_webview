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

#include "ohos_nweb/bridge/ark_web_blankless_callback_wrapper.h"

#include "ohos_nweb/bridge/ark_web_hap_value_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebBlanklessCallbackWrapper::ArkWebBlanklessCallbackWrapper(
    ArkWebRefPtr<ArkWebBlanklessCallback> ark_web_blankless_callback)
    : ark_web_blankless_callback_(ark_web_blankless_callback)
{}

void ArkWebBlanklessCallbackWrapper::OnReceiveValue(const std::string& key, int32_t state,
    int64_t timestamp, const std::string& reason)
{
    ArkWebString stKey = ArkWebStringClassToStruct(key);
    ArkWebString stReason = ArkWebStringClassToStruct(reason);

    ark_web_blankless_callback_->OnReceiveValue(stKey, state, timestamp, stReason);

    ArkWebStringStructRelease(stKey);
    ArkWebStringStructRelease(stReason);
}

} // namespace OHOS::ArkWeb
