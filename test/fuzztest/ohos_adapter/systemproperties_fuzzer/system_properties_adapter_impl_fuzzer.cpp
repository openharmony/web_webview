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

#include "system_properties_adapter_impl_fuzzer.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include "system_properties_adapter_impl.h"

namespace OHOS {

bool SystemPropertiesAdapterFuzzTest(const uint8_t* data, size_t size)
{
    size_t keySize = std::min(size, static_cast<size_t>(32));
    std::string key(reinterpret_cast<const char*>(data), keySize);

    size_t valueSize = size > keySize ? size - keySize : 0;
    std::string value(reinterpret_cast<const char*>(data + keySize), valueSize);

    bool boolResult = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetBoolParameter(key, false);
    std::string productModel = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetDeviceInfoProductModel();
    std::string brand = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetDeviceInfoBrand();
    int32_t majorVersion = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetSoftwareMajorVersion();
    int32_t seniorVersion = NWeb::SystemPropertiesAdapterImpl::GetInstance().GetSoftwareSeniorVersion();

    (void)boolResult;
    (void)productModel;
    (void)brand;
    (void)majorVersion;
    (void)seniorVersion;

    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    return OHOS::SystemPropertiesAdapterFuzzTest(data, size) ? 0 : 1;
}

} // namespace OHOS