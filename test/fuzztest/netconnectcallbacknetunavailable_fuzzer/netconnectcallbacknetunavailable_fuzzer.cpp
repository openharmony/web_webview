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

#include "netconnectcallbacknetunavailable_fuzzer.h"

#define private public
#include "core_service_client.h"
#include "cellular_data_client.h"
#include "net_connect_callback_impl.h"

using namespace OHOS::NetManagerStandard;
using namespace OHOS::Telephony;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        std::shared_ptr<NWeb::NetConnectCallbackImpl> netConnectCallbackImpl =
            std::make_shared<NWeb::NetConnectCallbackImpl>(nullptr);
        sptr<NetHandle> netHandle(new NetHandle);
        sptr<NetLinkInfo> info(new NetLinkInfo);
        netConnectCallbackImpl->NetUnavailable();
        netConnectCallbackImpl->NetConnectionPropertiesChange(netHandle, info);
        netConnectCallbackImpl->NetBlockStatusChange(netHandle, false);
        sptr<NetAllCapabilities> netAllCap(new NetAllCapabilities);
        netAllCap->bearerTypes_.insert(NetBearType::BEARER_WIFI);
        netConnectCallbackImpl->NetCapabilitiesChange(netHandle, netAllCap);
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
