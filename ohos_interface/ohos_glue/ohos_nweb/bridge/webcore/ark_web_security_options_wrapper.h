/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_SECURITY_OPTIONS_WRAPPER_H_
#define ARK_WEB_SECURITY_OPTIONS_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_security_options.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebSecurityOptionsWrapper : public OHOS::NWeb::NWebSecurityOptions {
public:
    ArkWebSecurityOptionsWrapper(ArkWebRefPtr<ArkWebSecurityOptions> ark_web_security_options);
    ~ArkWebSecurityOptionsWrapper() = default;

    bool GetDisableJITCompilation() override;
    bool GetDisableWebAssembly() override;
    bool GetDisableWebGL() override;
    bool GetDisablePDFViewer() override;
    bool GetDisableMathML() override;
    bool GetDisableServiceWorker() override;
    bool GetDisableNonProxyUDP() override;

private:
    ArkWebRefPtr<ArkWebSecurityOptions> ark_web_security_options_;
};

} // namespace OHOS::ArkWeb
#endif // ARK_WEB_SECURITY_OPTIONS_WRAPPER_H_
