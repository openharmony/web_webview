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

#include "netconnectadaptergettype_fuzzer.h"

#include <cstring>
#include <securec.h>

#define private public
#include "net_connect_adapter_impl.h"
#include "net_connect_utils.h"

using namespace OHOS::NetManagerStandard;
using namespace OHOS::Telephony;

namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }
        std::shared_ptr<NWeb::NetConnectAdapterImpl> netConnectAdapterImpl =
            std::make_shared<NWeb::NetConnectAdapterImpl>();
        NWeb::NetConnectType type;
        NWeb::NetConnectSubtype subtype;
        if (memcpy_s(&type, sizeof(NWeb::NetConnectType), data, sizeof(NWeb::NetConnectType)) != 0) {
            return false;
        }
        if (memcpy_s(&subtype, sizeof(NWeb::NetConnectSubtype), data, sizeof(NWeb::NetConnectSubtype)) != 0) {
            return false;
        }
        netConnectAdapterImpl->GetDefaultNetConnect(type, subtype);
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
