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

#include "ohos_nweb/bridge/ark_web_security_options_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebSecurityOptionsWrapper::ArkWebSecurityOptionsWrapper(
    ArkWebRefPtr<ArkWebSecurityOptions> ark_web_security_options)
    : ark_web_security_options_(ark_web_security_options)
{}

bool ArkWebSecurityOptionsWrapper::GetDisableJITCompilation()
{
    return ark_web_security_options_->GetDisableJITCompilation();
}

bool ArkWebSecurityOptionsWrapper::GetDisableWebAssembly()
{
    return ark_web_security_options_->GetDisableWebAssembly();
}

bool ArkWebSecurityOptionsWrapper::GetDisableWebGL()
{
    return ark_web_security_options_->GetDisableWebGL();
}

bool ArkWebSecurityOptionsWrapper::GetDisablePDFViewer()
{
    return ark_web_security_options_->GetDisablePDFViewer();
}

bool ArkWebSecurityOptionsWrapper::GetDisableMathML()
{
    return ark_web_security_options_->GetDisableMathML();
}

bool ArkWebSecurityOptionsWrapper::GetDisableServiceWorker()
{
    return ark_web_security_options_->GetDisableServiceWorker();
}

bool ArkWebSecurityOptionsWrapper::GetDisableNonProxyUDP()
{
    return ark_web_security_options_->GetDisableNonProxyUDP();
}

} // namespace OHOS::ArkWeb
