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

#include "nweb_javascript_result_callback_test.h"

#include <iostream>
#include "nweb_test_log.h"

namespace OHOS::NWeb {
std::shared_ptr<NWebValue> NWebJavaScriptResultCallBackTest::GetJavaScriptResult(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string &method, const std::string &object_name)
{
    TESTLOG_I("GetJavaScriptResult=%{public}s", object_name.c_str());
    TESTLOG_I("GetJavaScriptResult=%{public}s", method.c_str());
    std::shared_ptr<NWebValue> value = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    return value;
}
void NWebJavaScriptResultCallBackTest::RegisterArkJSfunction(const std::string &objectname)
{
    std::unique_lock<std::mutex> lk(object_mtx_);
    if (objector_map_.find(objectname) != objector_map_.end()) {
        return;
    }
    objector_map_[objectname] = "method";
}
void NWebJavaScriptResultCallBackTest::UnregisterArkJSfunction(const std::string &objectname)
{
    std::unique_lock<std::mutex> lk(object_mtx_);
    if (objector_map_.find(objectname) == objector_map_.end()) {
        return;
    }
    objector_map_.erase(objectname);
}
}
