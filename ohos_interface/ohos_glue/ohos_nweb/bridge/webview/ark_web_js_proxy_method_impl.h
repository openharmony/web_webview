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

 #ifndef ARK_WEB_JS_PROXY_METHOD_IMPL_H_
 #define ARK_WEB_JS_PROXY_METHOD_IMPL_H_
 #pragma once
 
 #include "include/nweb.h"
 #include "ohos_nweb/include/ark_web_js_proxy_method.h"
 
 namespace OHOS::ArkWeb {
 
 class ArkWebJsProxyMethodImpl : public ArkWebJsProxyMethod {
     IMPLEMENT_REFCOUNTING(ArkWebJsProxyMethodImpl);
 
 public:
     ArkWebJsProxyMethodImpl(std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data);
     ~ArkWebJsProxyMethodImpl() = default;
 
     int32_t GetSize() override;
 
     void OnHandle(int32_t number, const ArkWebStringVector& param) override;
 
 private:
     std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> nweb_data_;
 };
 
 } // namespace OHOS::ArkWeb
 
 #endif // ARK_WEB_JS_PROXY_METHOD_IMPL_H_
 