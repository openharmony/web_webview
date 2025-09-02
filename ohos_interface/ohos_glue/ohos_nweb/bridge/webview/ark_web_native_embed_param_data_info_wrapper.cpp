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

#include "ohos_nweb/bridge/ark_web_native_embed_param_data_info_wrapper.h"

#include "ohos_nweb/bridge/ark_web_native_embed_param_item_wrapper.h"
#include "ohos_nweb/ctocpp/ark_web_native_embed_param_item_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedParamDataInfoWrapper::ArkWebNativeEmbedParamDataInfoWrapper(
    ArkWebRefPtr<ArkWebNativeEmbedParamDataInfo> ark_web_native_embed_param_data_info)
    : ark_web_native_embed_param_data_info_(ark_web_native_embed_param_data_info)
{}

std::string ArkWebNativeEmbedParamDataInfoWrapper::GetEmbedId()
{
    ArkWebString stEmbedId = ark_web_native_embed_param_data_info_->GetEmbedId();

    std::string objEmbedId = ArkWebStringStructToClass(stEmbedId);
    ArkWebStringStructRelease(stEmbedId);
    return objEmbedId;
}

std::string ArkWebNativeEmbedParamDataInfoWrapper::GetObjectAttributeId()
{
    ArkWebString stObjectAttributeId = ark_web_native_embed_param_data_info_->GetObjectAttributeId();

    std::string objObjectAttributeId = ArkWebStringStructToClass(stObjectAttributeId);
    ArkWebStringStructRelease(stObjectAttributeId);
    return objObjectAttributeId;
}

std::vector<std::shared_ptr<OHOS::NWeb::NWebNativeEmbedParamItem>>
    ArkWebNativeEmbedParamDataInfoWrapper::GetParamItems()
{
    ArkWebNativeEmbedParamItemVector stNativeEmbedParamItemVector =
        ark_web_native_embed_param_data_info_->GetParamItems();

    std::vector<std::shared_ptr<OHOS::NWeb::NWebNativeEmbedParamItem>> objNativeEmbedParamItemVector =
        ArkWebNativeEmbedParamItemVectorStructToClass(stNativeEmbedParamItemVector);
    ArkWebNativeEmbedParamItemVectorStructRelease(stNativeEmbedParamItemVector);
    return objNativeEmbedParamItemVector;
}

} // namespace OHOS::ArkWeb