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

#include <securec.h>

#include "init_param.h"
#include "nweb_log.h"
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

SystemPropertiesAdapterImpl::SystemPropertiesAdapterImpl()
{
    std::string osFullName =
        OHOS::system::GetParameter("const.ohos.fullname", "");
    if (osFullName.empty()) {
        WVLOG_E("get os full name failed");
        return;
    }
    int versionPartOne;
    int versionPartTwo;
    int versionPartThree;
    int versionPartFour;
    const char *tmp = strstr(osFullName.c_str(), "-");
    if (tmp == NULL) {
        return;
    }
    tmp++;
    int ret = sscanf_s(tmp, "%d.%d.%d.%d",
        &versionPartOne, &versionPartTwo, &versionPartThree, &versionPartFour);
    if (ret <= 0) {
        WVLOG_E("paser os full name failed");
        return;
    }
    softwareMajorVersion_ = versionPartOne;
    softwareSeniorVersion_ = versionPartTwo;
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

bool SystemPropertiesAdapterImpl::GetWebOptimizationValue() const
{
    return OHOS::system::GetBoolParameter("web.optimization", true);
}

bool SystemPropertiesAdapterImpl::GetLockdownModeStatus() const
{
    char buffer[32] = { 0 };
    uint32_t buffSize = sizeof(buffer);

    if (SystemGetParameter("ohos.boot.advsecmode.state", buffer, &buffSize) == 0 && strcmp(buffer, "0") != 0) {
        return true;
    }
    return false;
}

std::string SystemPropertiesAdapterImpl::GetUserAgentOSName() const
{
    return OHOS::system::GetParameter("const.product.os.dist.name", "");
}

int32_t SystemPropertiesAdapterImpl::GetSoftwareMajorVersion() const
{
    return softwareMajorVersion_;
}

int32_t SystemPropertiesAdapterImpl::GetSoftwareSeniorVersion() const
{
    return softwareSeniorVersion_;
}

std::string SystemPropertiesAdapterImpl::GetNetlogMode() const
{
    return OHOS::system::GetParameter("web.debug.netlog", "");
}

bool SystemPropertiesAdapterImpl::GetTraceDebugEnable() const
{
    return OHOS::system::GetBoolParameter("web.debug.trace", false);
}
} // namespace OHOS::NWeb