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

#include "ohos_adapter/bridge/ark_media_avsession_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_media_avsession_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_media_avsession_metadata_adapter_impl.h"
#include "ohos_adapter/bridge/ark_media_avsession_position_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaAVSessionAdapterWrapper::ArkMediaAVSessionAdapterWrapper(ArkWebRefPtr<ArkMediaAVSessionAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkMediaAVSessionAdapterWrapper::CreateAVSession(NWeb::MediaAVSessionType type)
{
    return ctocpp_->CreateAVSession((int32_t)type);
}

void ArkMediaAVSessionAdapterWrapper::DestroyAVSession()
{
    ctocpp_->DestroyAVSession();
}

bool ArkMediaAVSessionAdapterWrapper::RegistCallback(
    std::shared_ptr<NWeb::MediaAVSessionCallbackAdapter> callbackAdapter)
{
    if (CHECK_SHARED_PTR_IS_NULL(callbackAdapter)) {
        return ctocpp_->RegistCallback(nullptr);
    }

    return ctocpp_->RegistCallback(new ArkMediaAVSessionCallbackAdapterImpl(callbackAdapter));
}

bool ArkMediaAVSessionAdapterWrapper::IsActivated()
{
    return ctocpp_->IsActivated();
}

bool ArkMediaAVSessionAdapterWrapper::Activate()
{
    return ctocpp_->Activate();
}

void ArkMediaAVSessionAdapterWrapper::DeActivate()
{
    ctocpp_->DeActivate();
}

void ArkMediaAVSessionAdapterWrapper::SetMetadata(const std::shared_ptr<NWeb::MediaAVSessionMetadataAdapter> metadata)
{
    if (CHECK_SHARED_PTR_IS_NULL(metadata)) {
        ctocpp_->SetMetadata(nullptr);
    } else {
        return ctocpp_->SetMetadata(new ArkMediaAVSessionMetadataAdapterImpl(metadata));
    }
}

void ArkMediaAVSessionAdapterWrapper::SetPlaybackState(OHOS::NWeb::MediaAVSessionPlayState state)
{
    ctocpp_->SetPlaybackState((int32_t)state);
}

void ArkMediaAVSessionAdapterWrapper::SetPlaybackPosition(
    const std::shared_ptr<NWeb::MediaAVSessionPositionAdapter> position)
{
    if (CHECK_SHARED_PTR_IS_NULL(position)) {
        ctocpp_->SetPlaybackPosition(nullptr);
    } else {
        return ctocpp_->SetPlaybackPosition(new ArkMediaAVSessionPositionAdapterImpl(position));
    }
}
} // namespace OHOS::ArkWeb