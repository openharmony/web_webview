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

#ifndef ARK_WEB_RUNTIME_CONNECT_INFO_WRAPPER_H_
#define ARK_WEB_RUNTIME_CONNECT_INFO_WRAPPER_H_
#pragma once

#include "include/nweb_runtime_connect_info.h"
#include "ohos_nweb/include/ark_web_runtime_connect_info.h"

namespace OHOS::ArkWeb {

class ArkWebRuntimeConnectInfoWrapper : public OHOS::NWeb::NWebRuntimeConnectInfo {
public:
    ArkWebRuntimeConnectInfoWrapper(ArkWebRefPtr<ArkWebRuntimeConnectInfo> ark_web_runtime_connect_info);
    ~ArkWebRuntimeConnectInfoWrapper() = default;

    std::string GetBundleName() override;
    std::string GetExtensionOrigin() override;
    int GetMessageReadPipe() override;
    int GetMessageWritePipe() override;

private:
    ArkWebRefPtr<ArkWebRuntimeConnectInfo> ark_web_runtime_connect_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_RUNTIME_CONNECTION_INFO_WRAPPER_H_
