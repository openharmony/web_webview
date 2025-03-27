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

#include "ohos_adapter/bridge/ark_display_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkDisplayAdapterWrapper::ArkDisplayAdapterWrapper(ArkWebRefPtr<ArkDisplayAdapter> ref) : ctocpp_(ref) {}

OHOS::NWeb::DisplayId ArkDisplayAdapterWrapper::GetId()
{
    return ctocpp_->GetId();
}

int32_t ArkDisplayAdapterWrapper::GetWidth()
{
    return ctocpp_->GetWidth();
}

int32_t ArkDisplayAdapterWrapper::GetHeight()
{
    return ctocpp_->GetHeight();
}

float ArkDisplayAdapterWrapper::GetVirtualPixelRatio()
{
    return ctocpp_->GetVirtualPixelRatio();
}

OHOS::NWeb::RotationType ArkDisplayAdapterWrapper::GetRotation()
{
    uint32_t type = ctocpp_->GetRotation();
    return (OHOS::NWeb::RotationType)type;
}

OHOS::NWeb::OrientationType ArkDisplayAdapterWrapper::GetOrientation()
{
    uint32_t type = ctocpp_->GetOrientation();
    return (OHOS::NWeb::OrientationType)type;
}

int32_t ArkDisplayAdapterWrapper::GetDpi()
{
    return ctocpp_->GetDpi();
}

OHOS::NWeb::DisplayOrientation ArkDisplayAdapterWrapper::GetDisplayOrientation()
{
    uint32_t type = ctocpp_->GetDisplayOrientation();
    return (OHOS::NWeb::DisplayOrientation)type;
}

OHOS::NWeb::FoldStatus ArkDisplayAdapterWrapper::GetFoldStatus()
{
    uint32_t foldstatus = ctocpp_->GetFoldStatus();
    return static_cast<OHOS::NWeb::FoldStatus>(foldstatus);
}

bool ArkDisplayAdapterWrapper::IsFoldable()
{
    return ctocpp_->IsFoldable();
}

std::string ArkDisplayAdapterWrapper::GetName()
{
    ArkWebString str = ctocpp_->GetName();
    std::string result = ArkWebStringStructToClass(str);

    ArkWebStringStructRelease(str);
    return result;
}

int32_t ArkDisplayAdapterWrapper::GetAvailableWidth()
{
    return ctocpp_->GetAvailableWidth();
}

int32_t ArkDisplayAdapterWrapper::GetAvailableHeight()
{
    return ctocpp_->GetAvailableHeight();
}

bool ArkDisplayAdapterWrapper::GetAliveStatus()
{
    return ctocpp_->GetAliveStatus();
}

OHOS::NWeb::DisplayState ArkDisplayAdapterWrapper::GetDisplayState()
{
    uint32_t state = ctocpp_->GetDisplayState();
    return (OHOS::NWeb::DisplayState)state;
}

int32_t ArkDisplayAdapterWrapper::GetDensityDpi()
{
    return ctocpp_->GetDensityDpi();
}

int32_t ArkDisplayAdapterWrapper::GetX()
{
    return ctocpp_->GetX();
}

int32_t ArkDisplayAdapterWrapper::GetY()
{
    return ctocpp_->GetY();
}

OHOS::NWeb::DisplaySourceMode ArkDisplayAdapterWrapper::GetDisplaySourceMode()
{
    uint32_t mode = ctocpp_->GetDisplaySourceMode();
    return (OHOS::NWeb::DisplaySourceMode)mode;
}

int32_t ArkDisplayAdapterWrapper::GetPhysicalWidth()
{
    return ctocpp_->GetPhysicalWidth();
}

int32_t ArkDisplayAdapterWrapper::GetPhysicalHeight()
{
    return ctocpp_->GetPhysicalHeight();
}

float ArkDisplayAdapterWrapper::GetDefaultVirtualPixelRatio()
{
    return ctocpp_->GetDefaultVirtualPixelRatio();
}
} // namespace OHOS::ArkWeb
