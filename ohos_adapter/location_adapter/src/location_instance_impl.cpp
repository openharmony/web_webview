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

#include "location_instance_impl.h"
#include "location_proxy_adapter_impl.h"

namespace OHOS::NWeb {
LocationInstance& LocationInstance::GetInstance()
{
    static LocationInstanceImpl instance;
    return instance;
}

std::shared_ptr<LocationProxyAdapter> LocationInstanceImpl::CreateLocationProxyAdapter()
{
    return std::make_shared<LocationProxyAdapterImpl>();
}

std::shared_ptr<LocationRequestConfig> LocationInstanceImpl::CreateLocationRequestConfig()
{
    return std::make_shared<LocationRequestConfigImpl>();
}

}
