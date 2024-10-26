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
#include "ohos_adapter/bridge/ark_fold_status_listener_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkFoldStatusListenerAdapterImpl ::ArkFoldStatusListenerAdapterImpl(
    std::shared_ptr<OHOS::NWeb::FoldStatusListenerAdapter> ref)
    : real_(ref)
{}

void ArkFoldStatusListenerAdapterImpl::OnFoldStatusChanged(uint32_t foldstatus)
{
    real_->OnFoldStatusChanged(static_cast<OHOS::NWeb::FoldStatus>(foldstatus));
}

} // namespace OHOS::ArkWeb