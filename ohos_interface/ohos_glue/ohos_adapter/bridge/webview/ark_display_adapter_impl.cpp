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

#include "ohos_adapter/bridge/ark_display_adapter_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkDisplayAdapterImpl::ArkDisplayAdapterImpl(std::shared_ptr<OHOS::NWeb::DisplayAdapter> ref) : real_(ref) {}

uint64_t ArkDisplayAdapterImpl::GetId()
{
    return real_->GetId();
}

int32_t ArkDisplayAdapterImpl::GetWidth()
{
    return real_->GetWidth();
}

int32_t ArkDisplayAdapterImpl::GetHeight()
{
    return real_->GetHeight();
}

float ArkDisplayAdapterImpl::GetVirtualPixelRatio()
{
    return real_->GetVirtualPixelRatio();
}

uint32_t ArkDisplayAdapterImpl::GetRotation()
{
    return (uint32_t)real_->GetRotation();
}

uint32_t ArkDisplayAdapterImpl::GetOrientation()
{
    return (uint32_t)real_->GetOrientation();
}

int32_t ArkDisplayAdapterImpl::GetDpi()
{
    return (uint32_t)real_->GetDpi();
}

uint32_t ArkDisplayAdapterImpl::GetDisplayOrientation()
{
    return (uint32_t)real_->GetDisplayOrientation();
}

uint32_t OHOS::ArkWeb::ArkDisplayAdapterImpl::GetFoldStatus()
{
    return static_cast<uint32_t>(real_->GetFoldStatus());
}

bool ArkDisplayAdapterImpl::IsFoldable()
{
    return real_->IsFoldable();
}

ArkWebString ArkDisplayAdapterImpl::GetName()
{
    std::string str = real_->GetName();
    return ArkWebStringClassToStruct(str);
}

int32_t ArkDisplayAdapterImpl::GetAvailableWidth()
{
    return real_->GetAvailableWidth();
}

int32_t ArkDisplayAdapterImpl::GetAvailableHeight()
{
    return real_->GetAvailableHeight();
}

bool ArkDisplayAdapterImpl::GetAliveStatus()
{
    return real_->GetAliveStatus();
}

uint32_t ArkDisplayAdapterImpl::GetDisplayState()
{
    return static_cast<uint32_t>(real_->GetDisplayState());
}

int32_t ArkDisplayAdapterImpl::GetDensityDpi()
{
    return real_->GetDensityDpi();
}

int32_t ArkDisplayAdapterImpl::GetX()
{
    return real_->GetX();
}

int32_t ArkDisplayAdapterImpl::GetY()
{
    return real_->GetY();
}

uint32_t ArkDisplayAdapterImpl::GetDisplaySourceMode()
{
    return static_cast<uint32_t>(real_->GetDisplaySourceMode());
}

int32_t ArkDisplayAdapterImpl::GetPhysicalWidth()
{
    return real_->GetPhysicalWidth();
}

int32_t ArkDisplayAdapterImpl::GetPhysicalHeight()
{
    return real_->GetPhysicalHeight();
}

float ArkDisplayAdapterImpl::GetDefaultVirtualPixelRatio()
{
    return real_->GetDefaultVirtualPixelRatio();
}
} // namespace OHOS::ArkWeb
