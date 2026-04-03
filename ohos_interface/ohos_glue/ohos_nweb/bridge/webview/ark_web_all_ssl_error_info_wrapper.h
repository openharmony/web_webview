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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_ALL_SSL_ERROR_INFO_WRAPPER_H_
#define ARK_WEB_ALL_SSL_ERROR_INFO_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_all_ssl_error_info.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebAllSslErrorInfoWrapper : public OHOS::NWeb::NWebAllSslErrorInfo {
public:
    ArkWebAllSslErrorInfoWrapper(ArkWebRefPtr<ArkWebAllSslErrorInfo> ark_web_all_ssl_error_info);
    ~ArkWebAllSslErrorInfoWrapper() = default;

    OHOS::NWeb::SslError GetError() override;

    std::string GetUrl() override;

    std::string GetOriginalUrl() override;

    std::string GetReferrer() override;

    bool GetIsFatalError() override;

    bool GetIsMainFrame() override;

    std::vector<std::string> GetCertChainData() override;

private:
    ArkWebRefPtr<ArkWebAllSslErrorInfo> ark_web_all_ssl_error_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ALL_SSL_ERROR_INFO_WRAPPER_H_
