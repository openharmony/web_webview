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

#ifndef WEB_JAVASCRIPT_RESULT_CALLBACK_TEST_H
#define WEB_JAVASCRIPT_RESULT_CALLBACK_TEST_H

#include "nweb_javascript_result_callback.h"

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <condition_variable>
#include "nweb_value.h"

namespace OHOS::NWeb {
class NWebJavaScriptResultCallBackTest : public OHOS::NWeb::NWebJavaScriptResultCallBack {
public:
    NWebJavaScriptResultCallBackTest() {}
    ~NWebJavaScriptResultCallBackTest() = default;
    std::shared_ptr<OHOS::NWeb::NWebValue>
    GetJavaScriptResult(std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>> args,
                        const std::string& method,
                        const std::string& objectname) override;
    void RegisterArkJSfunction(const std::string& objectname);
    void UnregisterArkJSfunction(const std::string& objectname);
private:
    std::mutex int_mtx_;
    using ObjectClassMap  = std::map<std::string, std::string>;
    ObjectClassMap objector_map_;
    std::mutex object_mtx_;
};
}
#endif // WEB_JAVASCRIPT_RESULT_CALLBACK_TEST_H
