/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_NWEB_WEBVIEW_BRIDGE_HELPER_H_
#define ARK_WEB_NWEB_WEBVIEW_BRIDGE_HELPER_H_
#pragma once

#include "base/bridge/ark_web_bridge_helper.h"

namespace OHOS::ArkWeb {

class ArkWebNWebWebviewBridgeHelper : public ArkWebBridgeHelper {
public:
    ~ArkWebNWebWebviewBridgeHelper() = default;

    static ArkWebNWebWebviewBridgeHelper& GetInstance();

    bool Init(bool runMode, const std::string& bundlePath);

private:
    ArkWebNWebWebviewBridgeHelper();
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NWEB_WEBVIEW_BRIDGE_HELPER_H_
