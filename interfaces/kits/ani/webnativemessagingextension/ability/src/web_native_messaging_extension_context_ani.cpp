/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "web_native_messaging_extension_context_ani.h"

#include "nweb_log.h"

namespace OHOS {
namespace NWeb {
ani_status StsExtensionContextInit(ani_env* env)
{
    if (env == nullptr) {
        WNMLOG_I("env is nullptr");
        return ANI_ERROR;
    }
    return ANI_OK;
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    if (vm == nullptr || result == nullptr) {
        WVLOG_E("null vm or result");
        return ANI_INVALID_ARGS;
    }

    ani_env* env = nullptr;
    ani_status status = ANI_ERROR;
    status = vm->GetEnv(ANI_VERSION_1, &env);
    if (status != ANI_OK) {
        WVLOG_E("GetEnv failed, status=%{public}d", status);
        return ANI_NOT_FOUND;
    }
    StsExtensionContextInit(env);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
} // namespace NWeb
} // namespace OHOS