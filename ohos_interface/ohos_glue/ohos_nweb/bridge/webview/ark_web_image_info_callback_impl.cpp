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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_nweb/bridge/ark_web_image_info_callback_impl.h"
 
#include "ohos_nweb/ctocpp/ark_web_image_info_vector_ctocpp.h"
 
#include "base/bridge/ark_web_bridge_macros.h"
 
namespace OHOS::ArkWeb {
 
ArkWebImageInfoCallbackImpl::ArkWebImageInfoCallbackImpl(
    std::shared_ptr<OHOS::NWeb::NWebImageInfoCallback> nweb_image_info_callback)
    : nweb_image_info_callback_(nweb_image_info_callback)
{}
 
void ArkWebImageInfoCallbackImpl::OnReceivedValue(int32_t errorCode, const ArkWebImageInfoVector& imageInfos)
{
    nweb_image_info_callback_->OnReceivedValue(errorCode, ArkWebImageInfoVectorStructToClass(imageInfos));
}
 
} // namespace OHOS::ArkWeb