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
 
#ifndef ARK_WEB_COOKIE_WRAPPER_H_
#define ARK_WEB_COOKIE_WRAPPER_H_
#pragma once
 
#include "include/nweb_cookie_manager.h"
#include "ohos_nweb/include/ark_web_cookie.h"
 
namespace OHOS::ArkWeb {
 
class ArkWebCookieWrapper : public OHOS::NWeb::NWebCookie {
public:
    ArkWebCookieWrapper(ArkWebRefPtr<ArkWebCookie> ark_web_cookie);
    ~ArkWebCookieWrapper() = default;
 
    int GetSamesitePolicy() override;
    std::string GetExpiresDate() override;
    std::string GetName() override;
    bool GetIsSessionCookie() override;
    std::string GetValue() override;
    std::string GetPath() override;
    bool GetIsHttpOnly() override;
    bool GetIsSecure() override;
    std::string GetDomain() override;
 
private:
    ArkWebRefPtr<ArkWebCookie> ark_web_cookie_;
};
 
} // namespace OHOS::ArkWeb
 
#endif // ARK_WEB_COOKIE_WRAPPER_H_
