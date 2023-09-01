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

#include "system_properties_adapter_impl.h"

#include "parameter.h"
#include "parameters.h"
#include "sysversion.h"

namespace OHOS::NWeb {

// static
SystemPropertiesAdapterImpl& SystemPropertiesAdapterImpl::GetInstance()
{
    static SystemPropertiesAdapterImpl instance;
    return instance;
}

bool SystemPropertiesAdapterImpl::GetResourceUseHapPathEnable() const
{
    return OHOS::system::GetBoolParameter("compress", false);
}

std::string SystemPropertiesAdapterImpl::GetDeviceInfoProductModel() const
{
    return GetProductModel();
}

std::string SystemPropertiesAdapterImpl::GetDeviceInfoBrand() const
{
    return GetBrand();
}

int32_t SystemPropertiesAdapterImpl::GetDeviceInfoMajorVersion() const
{
    return GetMajorVersion();
}

ProductDeviceType SystemPropertiesAdapterImpl::GetProductDeviceType() const
{
    std::string deviceType = OHOS::system::GetDeviceType();
    if (deviceType == "phone" || deviceType == "default") {
        return ProductDeviceType::DEVICE_TYPE_MOBILE;
    } else if (deviceType == "tablet") {
        return ProductDeviceType::DEVICE_TYPE_TABLET;
    } else if (deviceType == "2in1") {
        return ProductDeviceType::DEVICE_TYPE_2IN1;
    }

    return ProductDeviceType::DEVICE_TYPE_UNKNOWN;
}
} // namespace OHOS::NWeb