/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_JS_SSL_SELECT_CERT_RESULT_H
#define NWEB_JS_SSL_SELECT_CERT_RESULT_H

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebJSSslSelectCertResult {
public:
    virtual ~NWebJSSslSelectCertResult() = default;

    /**
     * @brief Confirm to use the specified private key and client certificate chain.
     */
    virtual void Confirm(const std::string& privateKeyFile, const std::string& certChainFile) = 0;

    /**
     * @brief Cancel this certificate request.
     */
    virtual void Cancel() = 0;

    /**
     * @brief Ignore this certificate request temporarily.
     */
    virtual void Ignore() = 0;

    /**
     * @brief Confirm to use the specified identity and certificate type.
     */
    virtual void Confirm(const std::string& identity, int32_t type) {}
};

} // namespace OHOS::NWeb

#endif
