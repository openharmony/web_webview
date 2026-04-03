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

#ifndef ARK_WEB_IMAGE_INFO_CALLBACK_IMPL_H_
#define ARK_WEB_IMAGE_INFO_CALLBACK_IMPL_H_
 
#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_image_info_callback.h"
 
namespace OHOS::ArkWeb {
 
class ArkWebImageInfoCallbackImpl : public ArkWebImageInfoCallback {
    IMPLEMENT_REFCOUNTING(ArkWebImageInfoCallbackImpl);
 
public:
    ArkWebImageInfoCallbackImpl(std::shared_ptr<OHOS::NWeb::NWebImageInfoCallback>
            nweb_web_storage_origin_vector_value_callback);
    ~ArkWebImageInfoCallbackImpl() = default;
 
    void OnReceivedValue(int32_t errorCode, const ArkWebImageInfoVector& imageInfos) override;
 
private:
    std::shared_ptr<OHOS::NWeb::NWebImageInfoCallback> nweb_image_info_callback_;
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_WEB_IMAGE_INFO_CALLBACK_IMPL_H_
