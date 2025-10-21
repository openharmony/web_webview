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

#ifndef ARK_WEB_JS_SSL_SELECT_CERT_RESULT_WRAPPER_H_
#define ARK_WEB_JS_SSL_SELECT_CERT_RESULT_WRAPPER_H_
#pragma once

#include "include/nweb_js_ssl_select_cert_result.h"
#include "ohos_nweb/include/ark_web_js_ssl_select_cert_result.h"

namespace OHOS::ArkWeb {

class ArkWebJsSslSelectCertResultWrapper : public OHOS::NWeb::NWebJSSslSelectCertResult {
public:
    ArkWebJsSslSelectCertResultWrapper(ArkWebRefPtr<ArkWebJsSslSelectCertResult> ark_web_js_ssl_select_cert_result);
    ~ArkWebJsSslSelectCertResultWrapper() = default;

    /**
     * @brief Cancel this certificate request.
     */
    void Cancel() override;

    /**
     * @brief Ignore this certificate request temporarily.
     */
    void Ignore() override;

    /**
     * @brief Confirm to use the specified private key and client certificate
     *        chain.
     */
    void Confirm(const std::string& private_key_file, const std::string& cert_chain_file) override;

    /**
     * @brief Confirm to use the specified private key and client certificate
     *        chain.
     */
    void Confirm(const std::string& identity, int32_t type) override;

private:
    ArkWebRefPtr<ArkWebJsSslSelectCertResult> ark_web_js_ssl_select_cert_result_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_SSL_SELECT_CERT_RESULT_WRAPPER_H_
