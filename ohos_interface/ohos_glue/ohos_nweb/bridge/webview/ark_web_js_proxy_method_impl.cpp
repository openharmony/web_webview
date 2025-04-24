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

 #include "ohos_nweb/bridge/ark_web_js_proxy_method_impl.h"

 #include "base/bridge/ark_web_bridge_macros.h"
 
 namespace OHOS::ArkWeb {
 
 ArkWebJsProxyMethodImpl::ArkWebJsProxyMethodImpl(std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data)
     : nweb_data_(data) {}
 
 
 int32_t ArkWebJsProxyMethodImpl::GetSize()
 {
     return nweb_data_->GetSize();
 }
 
 void ArkWebJsProxyMethodImpl::OnHandle(int32_t number, const ArkWebStringVector& param)
 {
     return nweb_data_->OnHandle(number, ArkWebStringVectorStructToClass(param));
 }
 
 } // namespace OHOS::ArkWeb
 