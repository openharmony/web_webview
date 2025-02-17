/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_NAPI_PROXY_CONTROLLER_H
#define NWEB_NAPI_PROXY_CONTROLLER_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nweb_proxy_changed_callback.h"

#include "proxy_config.h"

namespace OHOS {
namespace NWeb {

const std::string PROXY_CONTROLLER_CLASS_NAME = "ProxyController";

class NapiProxyController {
public:
    NapiProxyController() {}
    ~NapiProxyController() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JS_Constructor(napi_env env, napi_callback_info info);
    static napi_value JS_ApplyProxyOverride(napi_env env, napi_callback_info info);
    static napi_value JS_RemoveProxyOverride(napi_env env, napi_callback_info info);
    static void InnerApplyProxyOverride(ProxyConfig *proxyConfig, napi_env env, napi_ref jsCallback);
};

class ProxyChangedCallbackImpl : public NWebProxyChangedCallback {
public:
    ProxyChangedCallbackImpl(napi_env env, napi_ref callback)
        : env_(env), callback_(callback) {}
    ~ProxyChangedCallbackImpl() = default;

    void OnChanged() override;

private:
    napi_env env_;
    napi_ref callback_;
};

} // namespace NWeb
} // namespace OHOS

#endif  // NWEB_NAPI_PROXY_CONTROLLER_H