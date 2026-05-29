/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_display_manager_adapter_impl.h"

#include "ohos_adapter/bridge/ark_display_adapter_impl.h"
#include "ohos_adapter/bridge/ark_display_listener_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_fold_status_listener_adapter_wrapper.h"
#include "ohos_adapter/cpptoc/ark_display_adapter_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDisplayManagerAdapterImpl::ArkDisplayManagerAdapterImpl(std::shared_ptr<OHOS::NWeb::DisplayManagerAdapter> ref)
    : real_(ref)
{}

uint64_t ArkDisplayManagerAdapterImpl::GetDefaultDisplayId()
{
    return real_->GetDefaultDisplayId();
}

ArkWebRefPtr<ArkDisplayAdapter> ArkDisplayManagerAdapterImpl::GetDefaultDisplay()
{
    std::shared_ptr<NWeb::DisplayAdapter> ref = real_->GetDefaultDisplay();
    if (CHECK_SHARED_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return new ArkDisplayAdapterImpl(ref);
}

uint32_t ArkDisplayManagerAdapterImpl::RegisterDisplayListener(ArkWebRefPtr<ArkDisplayListenerAdapter> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->RegisterDisplayListener(nullptr);
    }

    return real_->RegisterDisplayListener(std::make_shared<ArkDisplayListenerAdapterWrapper>(listener));
}

bool ArkDisplayManagerAdapterImpl::UnregisterDisplayListener(uint32_t id)
{
    return real_->UnregisterDisplayListener(id);
}

bool ArkDisplayManagerAdapterImpl::IsDefaultPortrait()
{
    return real_->IsDefaultPortrait();
}

uint32_t ArkDisplayManagerAdapterImpl::RegisterFoldStatusListener(ArkWebRefPtr<ArkFoldStatusListenerAdapter> listener)
{
    if (CHECK_REF_PTR_IS_NULL(listener)) {
        return real_->RegisterFoldStatusListener(nullptr);
    }

    return real_->RegisterFoldStatusListener(std::make_shared<ArkFoldStatusListenerAdapterWrapper>(listener));
}

bool ArkDisplayManagerAdapterImpl::UnregisterFoldStatusListener(uint32_t id)
{
    return real_->UnregisterDisplayListener(id);
}

ArkWebRefPtr<ArkDisplayAdapter> ArkDisplayManagerAdapterImpl::GetPrimaryDisplay()
{
    std::shared_ptr<NWeb::DisplayAdapter> ref = real_->GetPrimaryDisplay();
    if (CHECK_SHARED_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return new ArkDisplayAdapterImpl(ref);
}

ArkDisplayAdapterVector ArkDisplayManagerAdapterImpl::GetAllDisplays()
{
    std::vector<std::shared_ptr<NWeb::DisplayAdapter>> displays = real_->GetAllDisplays();
    ArkDisplayAdapterVector result = ArkDisplayAdapterVectorClassToStruct(displays);
    return result;
}

} // namespace OHOS::ArkWeb
