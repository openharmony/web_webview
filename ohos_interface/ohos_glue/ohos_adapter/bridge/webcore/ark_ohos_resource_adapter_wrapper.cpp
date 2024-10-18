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

#include "ohos_adapter/bridge/ark_ohos_resource_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_ohos_file_mapper_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkOhosResourceAdapterWrapper::ArkOhosResourceAdapterWrapper(ArkWebRefPtr<ArkOhosResourceAdapter> ref) : ctocpp_(ref) {}

bool ArkOhosResourceAdapterWrapper::GetRawFileData(const std::string& rawFile, size_t& len, uint8_t** dest, bool isSys)
{
    ArkWebString ark_raw_file = ArkWebStringClassToStruct(rawFile);
    bool result = ctocpp_->GetRawFileData(ark_raw_file, len, dest, isSys);
    ArkWebStringStructRelease(ark_raw_file);
    return result;
}

std::shared_ptr<NWeb::OhosFileMapper> ArkOhosResourceAdapterWrapper::GetRawFileMapper(
    const std::string& rawFile, bool isSys)
{
    ArkWebString ark_raw_file = ArkWebStringClassToStruct(rawFile);
    ArkWebRefPtr<ArkOhosFileMapper> mapper = ctocpp_->GetRawFileMapper(ark_raw_file, isSys);
    ArkWebStringStructRelease(ark_raw_file);
    if (CHECK_REF_PTR_IS_NULL(mapper)) {
        return nullptr;
    }
    return std::make_shared<ArkOhosFileMapperWrapper>(mapper);
}

bool ArkOhosResourceAdapterWrapper::IsRawFileExist(const std::string& rawFile, bool isSys)
{
    ArkWebString ark_raw_file = ArkWebStringClassToStruct(rawFile);
    bool result = ctocpp_->IsRawFileExist(ark_raw_file, isSys);
    ArkWebStringStructRelease(ark_raw_file);
    return result;
}

bool ArkOhosResourceAdapterWrapper::GetRawFileLastModTime(
    const std::string& rawFile, uint16_t& date, uint16_t& time, bool isSys)
{
    ArkWebString ark_raw_file = ArkWebStringClassToStruct(rawFile);
    bool result = ctocpp_->GetRawFileLastModTime(ark_raw_file, date, time, isSys);
    ArkWebStringStructRelease(ark_raw_file);
    return result;
}

bool ArkOhosResourceAdapterWrapper::GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys)
{
    ArkWebString ark_raw_file = ArkWebStringClassToStruct(rawFile);
    bool result = ctocpp_->GetRawFileLastModTime(ark_raw_file, time, isSys);
    ArkWebStringStructRelease(ark_raw_file);
    return result;
}

std::string ArkOhosResourceAdapterWrapper::GetSystemLanguage()
{
    if (!ctocpp_) {
        return "";
    }

    ArkWebString ark_result = ctocpp_->GetSystemLanguage();
    std::string result = ArkWebStringStructToClass(ark_result);
    ArkWebStringStructRelease(ark_result);
    return result;
}
} // namespace OHOS::ArkWeb
