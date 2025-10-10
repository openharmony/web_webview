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

#ifndef ARK_WEB_JS_SSL_ERROR_RESULT_IMPL_H_
#define ARK_WEB_JS_SSL_ERROR_RESULT_IMPL_H_
#pragma once

#include "include/nweb_js_ssl_error_result.h"
#include "ohos_nweb/include/ark_web_js_all_ssl_error_result.h"
#include "ohos_nweb/include/ark_web_js_ssl_error_result.h"

namespace OHOS::ArkWeb {

class ArkWebJsSslErrorResultImpl : public ArkWebJsSslErrorResult {
    IMPLEMENT_REFCOUNTING(ArkWebJsSslErrorResultImpl);

public:
    ArkWebJsSslErrorResultImpl(std::shared_ptr<OHOS::NWeb::NWebJSSslErrorResult> nweb_js_ssl_error_result);
    ~ArkWebJsSslErrorResultImpl() = default;

    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    void HandleCancel() override;

    /**
     * @brief Handle a confirmation response from the user, the url request will
     *        be continued.
     */
    void HandleConfirm() override;

    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    void HandleCancelV2(bool abortLoading) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebJSSslErrorResult> nweb_js_ssl_error_result_;
};

class ArkWebJsAllSslErrorResultImpl : public ArkWebJsAllSslErrorResult {
    IMPLEMENT_REFCOUNTING(ArkWebJsAllSslErrorResultImpl);

public:
    ArkWebJsAllSslErrorResultImpl(std::shared_ptr<OHOS::NWeb::NWebJSAllSslErrorResult> nweb_js_all_ssl_error_result);
    ~ArkWebJsAllSslErrorResultImpl() = default;

    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    void HandleCancel() override;

    /**
     * @brief Handle a confirmation response from the user, the url request will
     *        be continued.
     */
    void HandleConfirm() override;

    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    void HandleCancelV2(bool abortLoading) override;

private:
    std::shared_ptr<OHOS::NWeb::NWebJSAllSslErrorResult> nweb_js_all_ssl_error_result_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_SSL_ERROR_RESULT_IMPL_H_
