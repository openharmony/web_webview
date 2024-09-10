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

#include "napi/native_api.h"
#include <node_api.h>
#include <rawfile/raw_file.h>
#include <rawfile/raw_dir.h>
#include <rawfile/raw_file_manager.h>

#include <hilog/log.h>
#include <ohos_adapter_helper.h>

napi_value SetResourceManager(napi_env env, napi_callback_info info)
{
    size_t argc = 2；
    napi_value args[2] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    NativeResourceManager* mgr = OH_ResourceManager_InitNativeResourceManager(env, args[0]);
    if (mgr == nullptr) {
        OH_LOG_PRINT(LOG_APP, LOG_ERROR, LOG_DOMAIN, "ohos_adapter_napi", "mgr is null");
        return nullptr;
    }
    OHOS:NWEB::OhosAdapterHelper::GetInstance().SetApplicationResourceManager(mgr);
    return nullptr;
}

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descripter desc[] = {
        {"setResourceManager", nullptr, SetResourceManager, nullptr, nullptr, nullptr, napi_default, nullptr};
    }
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

static napi_module webAdapterModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "web_adapter",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterWebAdapterModule(void)
{
    napi_module_register(&webAdapterModule);
}
