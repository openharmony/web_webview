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

#include "ohos_adapter/bridge/ark_ohos_resource_adapter_impl.h"

#include "ohos_adapter/bridge/ark_ohos_file_mapper_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkOhosResourceAdapterImpl::ArkOhosResourceAdapterImpl(std::shared_ptr<OHOS::NWeb::OhosResourceAdapter> ref)
    : real_(ref)
{}

bool ArkOhosResourceAdapterImpl::GetRawFileData(const ArkWebString& rawFile, size_t& len, uint8_t** dest, bool isSys)
{
    return real_->GetRawFileData(ArkWebStringStructToClass(rawFile), len, dest, isSys);
}

ArkWebRefPtr<ArkOhosFileMapper> ArkOhosResourceAdapterImpl::GetRawFileMapper(const ArkWebString& rawFile, bool isSys)
{
    std::shared_ptr<NWeb::OhosFileMapper> fileMapper =
        real_->GetRawFileMapper(ArkWebStringStructToClass(rawFile), isSys);
    if (CHECK_SHARED_PTR_IS_NULL(fileMapper)) {
        return nullptr;
    }

    return new ArkOhosFileMapperImpl(fileMapper);
}

bool ArkOhosResourceAdapterImpl::IsRawFileExist(const ArkWebString& rawFile, bool isSys)
{
    return real_->IsRawFileExist(ArkWebStringStructToClass(rawFile), isSys);
}

bool ArkOhosResourceAdapterImpl::GetRawFileLastModTime(
    const ArkWebString& rawFile, uint16_t& date, uint16_t& time, bool isSys)
{
    return real_->GetRawFileLastModTime(ArkWebStringStructToClass(rawFile), date, time, isSys);
}

bool ArkOhosResourceAdapterImpl::GetRawFileLastModTime(const ArkWebString& rawFile, time_t& time, bool isSys)
{
    return real_->GetRawFileLastModTime(ArkWebStringStructToClass(rawFile), time, isSys);
}

ArkWebString ArkOhosResourceAdapterImpl::GetSystemLanguage()
{
    std::string result = real_->GetSystemLanguage();
    return ArkWebStringClassToStruct(result);
}

} // namespace OHOS::ArkWeb
