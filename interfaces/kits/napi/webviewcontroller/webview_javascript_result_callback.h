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

#ifndef NWEB_WEBVIEW_JAVASCRIPT_RESULT_CALLBACK_IMPL_H
#define NWEB_WEBVIEW_JAVASCRIPT_RESULT_CALLBACK_IMPL_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_value.h"
#include "uv.h"

namespace OHOS::NWeb {
struct JavaScriptObj {
    napi_env env = nullptr;
    std::unordered_map<std::string, napi_ref> methodMap;
};

class WebviewJavaScriptResultCallBack : public NWebJavaScriptResultCallBack {
public:
    WebviewJavaScriptResultCallBack() {}

    ~WebviewJavaScriptResultCallBack() override;

    std::shared_ptr<NWebValue> GetJavaScriptResult(std::vector<std::shared_ptr<NWebValue>> args,
        const std::string& method, const std::string& objName) override;

    void RegisterJavaScriptProxy(napi_env env, napi_value obj, const std::string& objName,
        const std::vector<std::string>& methodList);

    bool DeleteJavaScriptRegister(const std::string& objName);

private:
    std::unordered_map<std::string, JavaScriptObj> objectMap_;
    static void ParseNwebValue2NapiValue(napi_env env, std::shared_ptr<OHOS::NWeb::NWebValue> value,
        std::vector<napi_value>& argv);

    static void ParseNapiValue2NwebValue(napi_env env, napi_value value, std::shared_ptr<NWebValue> nwebValue);
    static void UvJsCallbackThreadWoker(uv_work_t *work, int status);
    struct NapiJsCallBackParm {
        napi_env env_;
        napi_ref callback_;
        std::vector<std::shared_ptr<NWebValue>> args_;
        std::shared_ptr<NWebValue> value_;
        std::mutex mutex_;
        std::condition_variable condition_;
        bool ready_ = false;
    };
};
}
#endif
