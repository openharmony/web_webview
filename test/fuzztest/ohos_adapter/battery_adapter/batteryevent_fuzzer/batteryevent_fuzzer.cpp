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

#include "batteryevent_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "battery_mgr_client_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
namespace {
class EmptyCallback : public WebBatteryEventCallback {
public:
    EmptyCallback() = default;

private:
    void BatteryInfoChanged(std::shared_ptr<WebBatteryInfo>) {}
};
} // namespace
bool BatteryEventFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    BatteryMgrClientAdapterImpl battery;
    std::shared_ptr<WebBatteryEventCallback> callback = std::make_shared<EmptyCallback>();
    battery.RegBatteryEvent(callback);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::BatteryEventFuzzTest(data, size);
    return 0;
}
