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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_SYSTEM_CONFIGURATION_WRAPPER_H_
#define ARK_WEB_SYSTEM_CONFIGURATION_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_system_configuration.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

class ArkWebSystemConfigurationWrapper : public OHOS::NWeb::NWebSystemConfiguration {
public:
    ArkWebSystemConfigurationWrapper(ArkWebRefPtr<ArkWebSystemConfiguration> ark_web_system_configuration);
    ~ArkWebSystemConfigurationWrapper() = default;

    uint8_t GetThemeFlags() override;

private:
    ArkWebRefPtr<ArkWebSystemConfiguration> ark_web_system_configuration_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_SYSTEM_CONFIGURATION_WRAPPER_H_
