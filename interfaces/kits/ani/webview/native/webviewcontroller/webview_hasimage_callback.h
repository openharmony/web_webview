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
#ifndef NWEB_WEBVIEW_HAS_IMAGE_CALLBACK_H
#define NWEB_WEBVIEW_HAS_IMAGE_CALLBACK_H

#include <uv.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nweb_value_callback.h"
#include "event_handler.h"
#include "ani.h"

namespace OHOS::NWeb {
class WebviewHasImageCallback : public NWebBoolValueCallback {
public:
    explicit WebviewHasImageCallback(ani_vm* vm, ani_ref callback, ani_resolver deferred)
        : vm_(vm), callback_(callback), deferred_(deferred) {}
    ~WebviewHasImageCallback();
    void OnReceiveValue(bool result) override;

private:
    ani_vm* vm_ = nullptr;
    ani_ref callback_ = nullptr;
    ani_resolver deferred_ = nullptr;
    std::mutex mainHandlerMutex;

    void AfterWorkCb(bool result);
    void AfterWorkCbAsync(ani_env* env, bool result);
    void AfterWorkCbPromise(ani_env* env, bool result);
};
}
#endif