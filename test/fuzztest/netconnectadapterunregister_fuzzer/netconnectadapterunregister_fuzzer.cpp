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

#include "netconnectadapterunregister_fuzzer.h"

#include <cstring>
#include <securec.h>

#define private public
#include "net_connect_adapter_impl.h"
#include "net_connect_utils.h"
#include "net_connect_adapter.h"

using namespace OHOS::NetManagerStandard;
using namespace OHOS::Telephony;
using namespace OHOS::NWeb;

namespace OHOS {
class NetConnCallbackTest : public NetConnCallback {
public:
    NetConnCallbackTest() = default;
    virtual ~NetConnCallbackTest() = default;
    int32_t NetAvailable() override
    {
        return 0;
    }
    int32_t NetCapabilitiesChange(const NetConnectType &netConnectType,
        const NetConnectSubtype &netConnectSubtype) override
    {
        return 0;
    }
    int32_t NetConnectionPropertiesChange() override
    {
        return 0;
    }
    int32_t NetUnavailable() override
    {
        return 0;
    }
};

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    uint32_t width;
    if (memcpy_s(&width, sizeof(uint32_t), data, sizeof(uint32_t)) != 0) {
        return true;
    }
    std::shared_ptr<NetConnCallback> cb = std::make_shared<NetConnCallbackTest>();
    std::shared_ptr<NWeb::NetConnectAdapterImpl> netConnectAdapterImpl =
        std::make_shared<NWeb::NetConnectAdapterImpl>();
    netConnectAdapterImpl->RegisterNetConnCallback(cb);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
