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
 
#ifndef WEBVIEW_BLANKLESS_CALLBACK_H
#define WEBVIEW_BLANKLESS_CALLBACK_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nweb_blankless_callback.h"
 
namespace OHOS::NWeb {
 
class WebviewBlanklessCallback : public NWebBlanklessCallback {
public:
    explicit WebviewBlanklessCallback(napi_env env, napi_ref callbackRef) : env_(env),
        callbackRef_(callbackRef) {}

    ~WebviewBlanklessCallback() override = default;

    void OnReceiveValue(const std::string& key, int32_t state,
        int64_t timestamp, const std::string& reason) override;

private:
    napi_env env_ = nullptr;
    napi_ref callbackRef_ = nullptr;
};
} // namespace OHOS::NWeb
#endif  // #define WEBVIEW_BLANKLESS_CALLBACK_H
