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

#include "ohos_nweb/bridge/ark_web_string_vector_value_callback_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebStringVectorValueCallbackImpl::ArkWebStringVectorValueCallbackImpl(
    std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> nweb_string_vector_value_callback)
    : nweb_string_vector_value_callback_(nweb_string_vector_value_callback)
{}

void ArkWebStringVectorValueCallbackImpl::OnReceiveValue(const ArkWebStringVector& value)
{
    nweb_string_vector_value_callback_->OnReceiveValue(ArkWebStringVectorStructToClass(value));
}

} // namespace OHOS::ArkWeb
