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

#include "ohos_nweb/bridge/ark_web_image_info_callback_wrapper.h"
#include "ohos_nweb/cpptoc/ark_web_image_info_vector_cpptoc.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebImageInfoCallbackWrapper::ArkWebImageInfoCallbackWrapper(
    ArkWebRefPtr<ArkWebImageInfoCallback> ark_web_image_info_callback)
    : ark_web_image_info_callback_(ark_web_image_info_callback)
{}
 
void ArkWebImageInfoCallbackWrapper::OnReceivedValue(
    int32_t errorCode,
    const std::vector<std::shared_ptr<OHOS::NWeb::NWebImageInfo>>& imageInfos)
{
    ArkWebImageInfoVector stImageInfos = ArkWebImageInfoVectorClassToStruct(imageInfos);
 
    ark_web_image_info_callback_->OnReceivedValue(errorCode, stImageInfos);
 
    ArkWebImageInfoVectorStructRelease(stImageInfos);
}
 
} // namespace OHOS::ArkWeb
