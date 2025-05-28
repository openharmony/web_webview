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

#include "napi_webview_controller_fuzzer.h"

#include <cstring>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "webview_controller.h"
#undef private

using namespace OHOS::NWeb;

namespace OHOS {

bool NapiWebViewControllerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t)))) {
        return false;
    }
    FuzzedDataProvider fdp(data, size);
    std::string type = fdp.ConsumeRandomLengthString();
    int32_t timeout = fdp.ConsumeIntegral<int32_t>();

    WebviewController webviewController;
    napi_env env = nullptr;
    napi_deferred deferred = nullptr;
    napi_value handler = nullptr;

    webviewController.UnregisterStateChangeCallback(env, type, handler);
    webviewController.RegisterStateChangeCallback(env, type, handler);
    webviewController.WaitForAttachedPromise(env, timeout, deferred);

    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NapiWebViewControllerFuzzTest(data, size);
    return 0;
}