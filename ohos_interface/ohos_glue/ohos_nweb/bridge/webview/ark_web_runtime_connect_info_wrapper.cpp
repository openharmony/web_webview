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

#include "ohos_nweb/bridge/ark_web_runtime_connect_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebRuntimeConnectInfoWrapper::ArkWebRuntimeConnectInfoWrapper(
    ArkWebRefPtr<ArkWebRuntimeConnectInfo> ark_web_runtime_connect_info)
    : ark_web_runtime_connect_info_(ark_web_runtime_connect_info)
{}
std::string ArkWebRuntimeConnectInfoWrapper::GetBundleName()
{
    ArkWebString bundleName = ark_web_runtime_connect_info_->GetBundleName();

    std::string strBundleName = ArkWebStringStructToClass(bundleName);
    ArkWebStringStructRelease(bundleName);
    return strBundleName;
}
std::string ArkWebRuntimeConnectInfoWrapper::GetExtensionOrigin()
{
    ArkWebString extensionOrigin = ark_web_runtime_connect_info_->GetExtensionOrigin();

    std::string strExtensionOrigin = ArkWebStringStructToClass(extensionOrigin);
    ArkWebStringStructRelease(extensionOrigin);
    return strExtensionOrigin;
}

int ArkWebRuntimeConnectInfoWrapper::GetMessageReadPipe()
{
    return ark_web_runtime_connect_info_->GetMessageReadPipe();
}
int ArkWebRuntimeConnectInfoWrapper::GetMessageWritePipe()
{
    return ark_web_runtime_connect_info_->GetMessageWritePipe();
}
} // namespace OHOS::ArkWeb
