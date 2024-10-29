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

#include "ohos_adapter/bridge/ark_web_adapter_bridge_helper.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "base/check.h"

namespace OHOS::ArkWeb {

const std::string LIB_ARKWEB_FILE_NAME = "libarkweb_os_adapter_glue.z.so";
const std::string LIB_FILE_NAME = "libohos_adapter_glue_source.z.so";

ArkWebAdapterBridgeHelper& ArkWebAdapterBridgeHelper::GetInstance(bool isPrintLog)
{
    static ArkWebAdapterBridgeHelper helper;
    CHECK(helper.Init(isPrintLog));

    return helper;
}

bool ArkWebAdapterBridgeHelper::Init(bool isPrintLog)
{
    bool success = LoadLibFile(RTLD_LAZY, LIB_ARKWEB_FILE_NAME, isPrintLog);
    if (!success) {
        success = LoadLibFile(RTLD_LAZY, LIB_FILE_NAME, isPrintLog);
    }
    return success;
}

} // namespace OHOS::ArkWeb
