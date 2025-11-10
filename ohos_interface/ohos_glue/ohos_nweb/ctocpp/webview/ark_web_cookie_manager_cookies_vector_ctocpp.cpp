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
 
#include "ohos_nweb/ctocpp/ark_web_cookie_manager_cookies_vector_ctocpp.h"
 
#include "ohos_nweb/bridge/ark_web_cookie_manager_cookies_wrapper.h"
#include "ohos_nweb/ctocpp/ark_web_cookie_manager_cookies_ctocpp.h"
 
namespace OHOS::ArkWeb {
 
std::vector<std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies>> ArkWebCookieManagerCookiesVectorStructToClass(
    const ArkWebCookieManagerCookiesVector& struct_value)
{
    std::vector<std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies>> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            std::shared_ptr<OHOS::NWeb::NWebCookieManagerCookies> nweb_storage_origin =
                std::make_shared<ArkWebCookieManagerCookiesWrapper>(
                    ArkWebCookieManagerCookiesCToCpp::Invert(struct_value.value[count]));
            class_value.push_back(nweb_storage_origin);
        }
    }
 
    return class_value;
}
 
} // namespace OHOS::ArkWeb
