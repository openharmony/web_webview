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

#include "ohos_nweb/bridge/ark_web_native_embed_param_data_info_impl.h"

#include "ohos_nweb/cpptoc/ark_web_native_embed_param_item_vector_cpptoc.h"

#include "ohos_nweb/bridge/ark_web_native_embed_param_item_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedParamDataInfoImpl::ArkWebNativeEmbedParamDataInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedParamDataInfo> nweb_native_embed_param_data_info)
    : nweb_native_embed_param_data_info_(nweb_native_embed_param_data_info)
{}

ArkWebString ArkWebNativeEmbedParamDataInfoImpl::GetEmbedId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_param_data_info_->GetEmbedId());
}

ArkWebString ArkWebNativeEmbedParamDataInfoImpl::GetObjectAttributeId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_param_data_info_->GetObjectAttributeId());
}

ArkWebNativeEmbedParamItemVector ArkWebNativeEmbedParamDataInfoImpl::GetParamItems()
{
    return ArkWebNativeEmbedParamItemVectorClassToStruct(nweb_native_embed_param_data_info_->GetParamItems());
}

} // namespace OHOS::ArkWeb