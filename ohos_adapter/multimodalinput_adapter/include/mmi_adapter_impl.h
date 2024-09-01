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

#ifndef MMI_ADAPTER_IMPL_H
#define MMI_ADAPTER_IMPL_H

#include "mmi_adapter.h"

#include "nweb_log.h"

namespace OHOS::NWeb {

class MMIAdapterImpl : public MMIAdapter {
public:
    MMIAdapterImpl() = default;

    ~MMIAdapterImpl() override = default;

    char* KeyCodeToString(int32_t keyCode) override
    {
        char* empty = new char('\0');
        return empty;
    }

    int32_t RegisterMMIInputListener(std::shared_ptr<MMIInputListenerAdapter> eventCallback) override
    {
        return 0;
    }

    void UnregisterMMIInputListener(int32_t monitorId) override {}

    int32_t RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener) override
    {
        return 0;
    }

    int32_t UnregisterDevListener(std::string type) override { return 0; }

    int32_t GetKeyboardType(int32_t deviceId, int32_t& type) override { return 0; }

    int32_t GetDeviceIds(std::vector<int32_t>& ids) override { return 0; }

    int32_t GetDeviceInfo(int32_t deviceId, std::shared_ptr<MMIDeviceInfoAdapter> info) override { return 0; }
};
}  // namespace OHOS::NWeb

#endif // MMI_ADAPTER_IMPL_H
