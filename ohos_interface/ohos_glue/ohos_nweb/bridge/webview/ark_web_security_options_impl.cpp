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

#include "ohos_nweb/bridge/ark_web_security_options_impl.h"

namespace OHOS::ArkWeb {

ArkWebSecurityOptionsImpl::ArkWebSecurityOptionsImpl(
    std::shared_ptr<OHOS::NWeb::NWebSecurityOptions> nweb_security_options)
    : nweb_security_options_(nweb_security_options)
{}

bool ArkWebSecurityOptionsImpl::GetDisableJITCompilation()
{
    return nweb_security_options_->GetDisableJITCompilation();
}

bool ArkWebSecurityOptionsImpl::GetDisableWebAssembly()
{
    return nweb_security_options_->GetDisableWebAssembly();
}

bool ArkWebSecurityOptionsImpl::GetDisableWebGL()
{
    return nweb_security_options_->GetDisableWebGL();
}

bool ArkWebSecurityOptionsImpl::GetDisablePDFViewer()
{
    return nweb_security_options_->GetDisablePDFViewer();
}

bool ArkWebSecurityOptionsImpl::GetDisableMathML()
{
    return nweb_security_options_->GetDisableMathML();
}

bool ArkWebSecurityOptionsImpl::GetDisableServiceWorker()
{
    return nweb_security_options_->GetDisableServiceWorker();
}

bool ArkWebSecurityOptionsImpl::GetDisableNonProxyUDP()
{
    return nweb_security_options_->GetDisableNonProxyUDP();
}

} // namespace OHOS::ArkWeb
