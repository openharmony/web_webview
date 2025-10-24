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

#include "ohos_nweb/bridge/ark_web_js_ssl_select_cert_result_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebJsSslSelectCertResultImpl::ArkWebJsSslSelectCertResultImpl(
    std::shared_ptr<OHOS::NWeb::NWebJSSslSelectCertResult> nweb_js_ssl_select_cert_result)
    : nweb_js_ssl_select_cert_result_(nweb_js_ssl_select_cert_result)
{}

void ArkWebJsSslSelectCertResultImpl::Cancel()
{
    nweb_js_ssl_select_cert_result_->Cancel();
}

void ArkWebJsSslSelectCertResultImpl::Ignore()
{
    nweb_js_ssl_select_cert_result_->Ignore();
}

void ArkWebJsSslSelectCertResultImpl::Confirm(const ArkWebString& private_key_file, const ArkWebString& cert_chain_file)
{
    nweb_js_ssl_select_cert_result_->Confirm(
        ArkWebStringStructToClass(private_key_file), ArkWebStringStructToClass(cert_chain_file));
}

void ArkWebJsSslSelectCertResultImpl::Confirm(const ArkWebString& identity, int32_t type)
{
    nweb_js_ssl_select_cert_result_->Confirm(ArkWebStringStructToClass(identity), type);
}

} // namespace OHOS::ArkWeb
