/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "setwebdebug_fuzzer.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool SetWebDebugFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int))) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    bool enableDebug = dataProvider.ConsumeBool();
    int32_t port = dataProvider.ConsumeIntegral<int32_t>();
    std::string name = dataProvider.ConsumeRandomLengthString();
    int32_t option = dataProvider.ConsumeIntegral<int32_t>();
    if (NWebHelper::Instance().LoadNWebSDK()) {
        NWebHelper::Instance().SetWebDebuggingAccess(enableDebug);
        NWebHelper::Instance().SetWebDebuggingAccessAndPort(enableDebug, port);
        NWebHelper::Instance().SaveSchemeVector(name.c_str(), option);
        NWebHelper::Instance().HasLoadWebEngine();
        NWebHelper::Instance().RegisterCustomSchemes();
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SetWebDebugFuzzTest(data, size);
    return 0;
}
