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

#include "ohos_adapter/bridge/ark_ohos_file_mapper_wrapper.h"

namespace OHOS::ArkWeb {

ArkOhosFileMapperWrapper::ArkOhosFileMapperWrapper(ArkWebRefPtr<ArkOhosFileMapper> ref) : ctocpp_(ref) {}

int32_t ArkOhosFileMapperWrapper::GetFd()
{
    return ctocpp_->GetFd();
}

int32_t ArkOhosFileMapperWrapper::GetOffset()
{
    return ctocpp_->GetOffset();
}

std::string ArkOhosFileMapperWrapper::GetFileName()
{
    ArkWebString str = ctocpp_->GetFileName();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

bool ArkOhosFileMapperWrapper::IsCompressed()
{
    return ctocpp_->IsCompressed();
}

void* ArkOhosFileMapperWrapper::GetDataPtr()
{
    return ctocpp_->GetDataPtr();
}

size_t ArkOhosFileMapperWrapper::GetDataLen()
{
    return ctocpp_->GetDataLen();
}

bool ArkOhosFileMapperWrapper::UnzipData(uint8_t** dest, size_t& len)
{
    return ctocpp_->UnzipData(dest, len);
}

} // namespace OHOS::ArkWeb
