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

#include "ohos_nweb/bridge/ark_web_js_ssl_select_cert_result_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebJsSslSelectCertResultWrapper::ArkWebJsSslSelectCertResultWrapper(
    ArkWebRefPtr<ArkWebJsSslSelectCertResult> ark_web_js_ssl_select_cert_result)
    : ark_web_js_ssl_select_cert_result_(ark_web_js_ssl_select_cert_result)
{}

void ArkWebJsSslSelectCertResultWrapper::Cancel()
{
    ark_web_js_ssl_select_cert_result_->Cancel();
}

void ArkWebJsSslSelectCertResultWrapper::Ignore()
{
    ark_web_js_ssl_select_cert_result_->Ignore();
}

void ArkWebJsSslSelectCertResultWrapper::Confirm(
    const std::string& private_key_file, const std::string& cert_chain_file)
{
    ArkWebString stCertChainFile = ArkWebStringClassToStruct(cert_chain_file);
    ArkWebString stPrivateKeyFile = ArkWebStringClassToStruct(private_key_file);

    ark_web_js_ssl_select_cert_result_->Confirm(stPrivateKeyFile, stCertChainFile);

    ArkWebStringStructRelease(stCertChainFile);
    ArkWebStringStructRelease(stPrivateKeyFile);
}

void ArkWebJsSslSelectCertResultWrapper::Confirm(
    const std::string& identity, int32_t type)
{
    ArkWebString stIdentity = ArkWebStringClassToStruct(identity);
 
    ark_web_js_ssl_select_cert_result_->Confirm(stIdentity, type);
 
    ArkWebStringStructRelease(stIdentity);
}

} // namespace OHOS::ArkWeb
