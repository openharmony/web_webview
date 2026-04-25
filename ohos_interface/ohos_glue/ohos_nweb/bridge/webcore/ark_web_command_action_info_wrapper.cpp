/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "ohos_nweb/bridge/ark_web_command_action_info_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebCommandActionInfoWrapper::ArkWebCommandActionInfoWrapper(ArkWebRefPtr<ArkWebCommandActionInfo> info) : info_(info)
{}

std::string ArkWebCommandActionInfoWrapper::GetEventType() const
{
    ArkWebString str = info_->GetEventType();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkWebCommandActionInfoWrapper::GetXPath() const
{
    ArkWebString str = info_->GetXPath();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::string ArkWebCommandActionInfoWrapper::GetInputValue() const
{
    ArkWebString str = info_->GetInputValue();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

std::vector<std::string> ArkWebCommandActionInfoWrapper::GetOptionValues() const
{
    return ArkWebStringVectorStructToClass(info_->GetOptionValues());
}

std::vector<int32_t> ArkWebCommandActionInfoWrapper::GetOptionIndexes() const
{
    return ArkWebBasicVectorStructToClass<int32_t, ArkWebInt32Vector>(info_->GetOptionIndexes());
}

} // namespace OHOS::ArkWeb
