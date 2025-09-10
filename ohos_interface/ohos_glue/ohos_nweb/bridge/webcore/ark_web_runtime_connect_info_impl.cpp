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

#include "ohos_nweb/bridge/ark_web_runtime_connect_info_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebRuntimeConnectInfoImpl::ArkWebRuntimeConnectInfoImpl(
    std::shared_ptr<OHOS::NWeb::NWebRuntimeConnectInfo> nweb_runtime_connect_info)
    : nweb_runtime_connect_info_(nweb_runtime_connect_info)
{}

ArkWebString ArkWebRuntimeConnectInfoImpl::GetBundleName()
{
    return ArkWebStringClassToStruct(nweb_runtime_connect_info_->GetBundleName());
}

ArkWebString ArkWebRuntimeConnectInfoImpl::GetExtensionOrigin()
{
    return ArkWebStringClassToStruct(nweb_runtime_connect_info_->GetExtensionOrigin());
}

int ArkWebRuntimeConnectInfoImpl::GetMessageReadPipe()
{
    return nweb_runtime_connect_info_->GetMessageReadPipe();
}

int ArkWebRuntimeConnectInfoImpl::GetMessageWritePipe()
{
    return nweb_runtime_connect_info_->GetMessageWritePipe();
}
} // namespace OHOS::ArkWeb
