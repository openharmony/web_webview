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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef ARK_WEB_JS_VERIFY_PIN_RESULT_IMPL_H_
#define ARK_WEB_JS_VERIFY_PIN_RESULT_IMPL_H_
#pragma once
 
#include "include/nweb_js_verify_pin_result.h"
#include "ohos_nweb/include/ark_web_js_verify_pin_result.h"
 
namespace OHOS::ArkWeb {
 
class ArkWebJsVerifyPinResultImpl : public ArkWebJsVerifyPinResult {
    IMPLEMENT_REFCOUNTING(ArkWebJsVerifyPinResultImpl);
 
public:
    ArkWebJsVerifyPinResultImpl(
        std::shared_ptr<OHOS::NWeb::NWebJSVerifyPinResult> nweb_js_ssl_select_cert_result);
    ~ArkWebJsVerifyPinResultImpl() = default;
 
    /**
     * @brief Confirm to use the specified private key and client certificate
     *        chain.
     */
    void Confirm(int32_t verify_result) override;
private:
    std::shared_ptr<OHOS::NWeb::NWebJSVerifyPinResult> nweb_js_verify_pin_result_;
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_WEB_JS_VERIFY_PIN_RESULT_IMPL_H_