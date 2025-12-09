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

#include "registerasyncjsproxy_fuzzer.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include <securec.h>

#include "native_interface_arkweb.h"
#include "nweb.h"

namespace OHOS {
constexpr uint32_t MAX_KEYS_COUNT = 150;
constexpr size_t MAX_KEY_LENGTH = 2500;
bool RegisterAsyncJSProxyFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string webTag = dataProvider.ConsumeRandomLengthString();
    ArkWeb_ProxyMethodWithResult method;
    std::string methodName = dataProvider.ConsumeRandomLengthString();
    method.methodName = methodName.c_str();
    method.callback = nullptr;
    method.userData = nullptr;
    size_t objSize = dataProvider.ConsumeIntegral<size_t>();
    ArkWeb_ProxyObjectWithResult proxyObject;
    std::string objName = dataProvider.ConsumeRandomLengthString();
    proxyObject.objName = objName.c_str();
    proxyObject.size = objSize;
    proxyObject.methodList = &method;
    std::string permission = dataProvider.ConsumeRandomLengthString();
    OH_NativeArkWeb_RegisterAsyncThreadJavaScriptProxy(webTag.c_str(), &proxyObject, permission.c_str());
    OH_NativeArkWeb_UnregisterJavaScriptProxy(webTag.c_str(), objName.c_str());
    OH_NativeArkWeb_GetJavaScriptProxyValidCallback(webTag.c_str());
    OH_NativeArkWeb_GetDestroyCallback(webTag.c_str());
    std::string dataTest = dataProvider.ConsumeRandomLengthString();
    std::string mimeType = dataProvider.ConsumeRandomLengthString();
    std::string encoding = dataProvider.ConsumeRandomLengthString();
    std::string baseUrl = dataProvider.ConsumeRandomLengthString();
    std::string historyUrl = dataProvider.ConsumeRandomLengthString();
    OH_NativeArkWeb_LoadData(
        webTag.c_str(), dataTest.c_str(), mimeType.c_str(), encoding.c_str(), baseUrl.c_str(), historyUrl.c_str());
    std::string key = dataProvider.ConsumeRandomLengthString();
    OH_NativeArkWeb_GetBlanklessInfoWithKey(webTag.c_str(), key.c_str());
    bool isStarted = dataProvider.ConsumeBool();
    OH_NativeArkWeb_SetBlanklessLoadingWithKey(webTag.c_str(), key.c_str(), isStarted);
    uint32_t capacity = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_KEYS_COUNT);
    OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(capacity);
    OH_ArkWebCookieManager_SaveCookieSync();
    OH_NativeArkWeb_GetActiveWebEngineVersion();
    OH_NativeArkWeb_IsActiveWebEngineEvergreen();
    return true;
}

bool RegisterAsyncJSProxyFuzzTest_002(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return true;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint32_t arraySize = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_KEYS_COUNT);
    std::vector<std::string> keyStorage;
    std::vector<const char*> keyArray;
    keyStorage.reserve(arraySize);
    keyArray.reserve(arraySize);
    for (uint32_t i = 0; i < arraySize; ++i) {
        size_t maxLen = dataProvider.ConsumeIntegralInRange<size_t>(0, MAX_KEY_LENGTH);
        std::string key = dataProvider.ConsumeRandomLengthString(maxLen);
        keyStorage.push_back(key);
        keyArray.push_back(keyStorage.back().c_str());
    }
    OH_NativeArkWeb_ClearBlanklessLoadingCache(keyArray.data(), arraySize);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::RegisterAsyncJSProxyFuzzTest(data, size);
    OHOS::RegisterAsyncJSProxyFuzzTest_002(data, size);
    return 0;
}
