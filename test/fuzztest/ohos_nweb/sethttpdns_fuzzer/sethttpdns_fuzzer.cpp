/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "sethttpdns_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "nweb_handler.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool SetHttpDnsFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int))) {
        return false;
    }
    std::string dohConfig((const char*)data, size);
    int mode;
    if (memcpy_s(&mode, sizeof(int), data, sizeof(int)) != 0) {
        return false;
    }

    std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
    config->SetMode(mode);
    config->SetConfig(dohConfig);
    NWebHelper::Instance().SetHttpDns(config);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SetHttpDnsFuzzTest(data, size);
    return 0;
}
