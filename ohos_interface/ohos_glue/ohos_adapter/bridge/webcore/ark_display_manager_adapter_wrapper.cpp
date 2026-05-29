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

#include "ohos_adapter/bridge/ark_display_manager_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_display_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_display_listener_adapter_impl.h"
#include "ohos_adapter/bridge/ark_fold_status_listener_adapter_impl.h"
#include "ohos_adapter/ctocpp/ark_display_adapter_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDisplayManagerAdapterWrapper::ArkDisplayManagerAdapterWrapper(ArkWebRefPtr<ArkDisplayManagerAdapter> ref)
    : ctocpp_(ref)
{}

OHOS::NWeb::DisplayId ArkDisplayManagerAdapterWrapper::GetDefaultDisplayId()
{
    return ctocpp_->GetDefaultDisplayId();
}

std::shared_ptr<OHOS::NWeb::DisplayAdapter> ArkDisplayManagerAdapterWrapper::GetDefaultDisplay()
{
    ArkWebRefPtr<ArkDisplayAdapter> ref = ctocpp_->GetDefaultDisplay();
    if (CHECK_REF_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return std::make_shared<ArkDisplayAdapterWrapper>(ref);
}

uint32_t ArkDisplayManagerAdapterWrapper::RegisterDisplayListener(
    std::shared_ptr<OHOS::NWeb::DisplayListenerAdapter> listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->RegisterDisplayListener(nullptr);
    }

    return ctocpp_->RegisterDisplayListener(new ArkDisplayListenerAdapterImpl(listener));
}

bool ArkDisplayManagerAdapterWrapper::UnregisterDisplayListener(uint32_t id)
{
    return ctocpp_->UnregisterDisplayListener(id);
}

bool ArkDisplayManagerAdapterWrapper::IsDefaultPortrait()
{
    return ctocpp_->IsDefaultPortrait();
}

uint32_t ArkDisplayManagerAdapterWrapper::RegisterFoldStatusListener(
    std::shared_ptr<OHOS::NWeb::FoldStatusListenerAdapter> listener)
{
    if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        return ctocpp_->RegisterFoldStatusListener(nullptr);
    }

    return ctocpp_->RegisterFoldStatusListener(new ArkFoldStatusListenerAdapterImpl(listener));
}

bool ArkDisplayManagerAdapterWrapper::UnregisterFoldStatusListener(uint32_t id)
{
    return ctocpp_->UnregisterFoldStatusListener(id);
}

std::shared_ptr<OHOS::NWeb::DisplayAdapter> ArkDisplayManagerAdapterWrapper::GetPrimaryDisplay()
{
    ArkWebRefPtr<ArkDisplayAdapter> ref = ctocpp_->GetPrimaryDisplay();
    if (CHECK_REF_PTR_IS_NULL(ref)) {
        return nullptr;
    }

    return std::make_shared<ArkDisplayAdapterWrapper>(ref);
}

std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>> ArkDisplayManagerAdapterWrapper::GetAllDisplays()
{
    if (!ctocpp_) {
        return std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>>();
    }

    ArkDisplayAdapterVector ark_vector = ctocpp_->GetAllDisplays();

    std::vector<std::shared_ptr<OHOS::NWeb::DisplayAdapter>> result = ArkDisplayAdapterVectorStructToClass(ark_vector);

    ArkDisplayAdapterVectorStructRelease(ark_vector);

    return result;
}

} // namespace OHOS::ArkWeb
