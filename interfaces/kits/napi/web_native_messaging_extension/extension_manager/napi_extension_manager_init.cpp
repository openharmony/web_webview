/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_web_native_messaging_extension_manager.h"

namespace OHOS {
namespace NWeb {
namespace {
napi_value NapiExtensionManagerInit(napi_env env, napi_value exports)
{
    napi_property_descriptor resultFuncs[] = {
        DECLARE_NAPI_FUNCTION("connectNative", NapiWebNativeMessagingExtensionManager::ConnectNative),
        DECLARE_NAPI_FUNCTION("disconnectNative", NapiWebNativeMessagingExtensionManager::DisconnectNative),
    };
    napi_define_properties(env, exports, sizeof(resultFuncs) / sizeof(resultFuncs[0]), resultFuncs);
    NapiWebNativeMessagingExtensionManager::Init(env, exports);
    return exports;
}

napi_module *GetNapiExtensionManagerModule(void)
{
    static ::napi_module napiModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = NapiExtensionManagerInit,
        .nm_modname = "web.webNativeMessagingExtensionManager",
        .nm_priv = ((void *)0),
        .reserved = {0}
    };
    return &napiModule;
}
} // namespace

/*
 * module register
 */
extern "C" __attribute__((constructor)) void Register(void)
{
    napi_module_register(GetNapiExtensionManagerModule());
}
} // namespace NWeb
} // namesapce OHOS
