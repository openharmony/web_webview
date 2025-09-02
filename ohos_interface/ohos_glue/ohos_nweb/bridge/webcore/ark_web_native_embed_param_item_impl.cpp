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

#include "ohos_nweb/bridge/ark_web_native_embed_param_item_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedParamItemImpl::ArkWebNativeEmbedParamItemImpl(
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedParamItem> nweb_native_embed_param_item)
    : nweb_native_embed_param_item_(nweb_native_embed_param_item)
{}

int ArkWebNativeEmbedParamItemImpl::GetStatus()
{
    return static_cast<int>(nweb_native_embed_param_item_->GetStatus());
}

ArkWebString ArkWebNativeEmbedParamItemImpl::GetId()
{
    return ArkWebStringClassToStruct(nweb_native_embed_param_item_->GetId());
}

ArkWebString ArkWebNativeEmbedParamItemImpl::GetName()
{
    return ArkWebStringClassToStruct(nweb_native_embed_param_item_->GetName());
}

ArkWebString ArkWebNativeEmbedParamItemImpl::GetValue()
{
    return ArkWebStringClassToStruct(nweb_native_embed_param_item_->GetValue());
}

} // namespace OHOS::ArkWeb