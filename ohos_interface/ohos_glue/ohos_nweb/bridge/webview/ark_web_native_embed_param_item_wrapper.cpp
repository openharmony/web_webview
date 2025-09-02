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

#include "ohos_nweb/bridge/ark_web_native_embed_param_item_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNativeEmbedParamItemWrapper::ArkWebNativeEmbedParamItemWrapper(
    ArkWebRefPtr<ArkWebNativeEmbedParamItem> ark_web_native_embed_param_item)
    : ark_web_native_embed_param_item_(ark_web_native_embed_param_item)
{}

ArkWebNativeEmbedParamStatus ArkWebNativeEmbedParamItemWrapper::GetStatus()
{
    return static_cast<ArkWebNativeEmbedParamStatus>(ark_web_native_embed_param_item_->GetStatus());
}

std::string ArkWebNativeEmbedParamItemWrapper::GetId()
{
    ArkWebString stId = ark_web_native_embed_param_item_->GetId();

    std::string objId = ArkWebStringStructToClass(stId);
    ArkWebStringStructRelease(stId);
    return objId;
}

std::string ArkWebNativeEmbedParamItemWrapper::GetName()
{
    ArkWebString stName = ark_web_native_embed_param_item_->GetName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

std::string ArkWebNativeEmbedParamItemWrapper::GetValue()
{
    ArkWebString stValue = ark_web_native_embed_param_item_->GetValue();

    std::string objValue = ArkWebStringStructToClass(stValue);
    ArkWebStringStructRelease(stValue);
    return objValue;
}

} // namespace OHOS::ArkWeb