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

 #ifndef ARK_WEB_JS_PROXY_METHOD_WRAPPER_H_
 #define ARK_WEB_JS_PROXY_METHOD_WRAPPER_H_
 #pragma once
 
 #include "include/nweb.h"
 #include "ohos_nweb/include/ark_web_js_proxy_method.h"
 
 namespace OHOS::ArkWeb {
 
 class ArkWebJsProxyMethodWrapper : public OHOS::NWeb::NWebJsProxyMethod {
 public:
     ArkWebJsProxyMethodWrapper(ArkWebRefPtr<ArkWebJsProxyMethod> data);
 
     ~ArkWebJsProxyMethodWrapper() = default;
 
     int32_t GetSize() override;
 
     void OnHandle(int32_t number, const std::vector<std::string>& param) override;
 
 private:
     ArkWebRefPtr<ArkWebJsProxyMethod> data_;
 };
 
 } // namespace OHOS::ArkWeb
 
 #endif // ARK_WEB_JS_PROXY_METHOD_WRAPPER_H_
 