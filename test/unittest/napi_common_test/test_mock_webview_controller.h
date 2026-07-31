/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef TEST_MOCK_WEBVIEW_CONTROLLER_H
#define TEST_MOCK_WEBVIEW_CONTROLLER_H

#include <atomic>
#include <cstdint>

namespace OHOS {
namespace NWeb {

class WebviewController {
public:
    explicit WebviewController() : controllerId_(GetNextControllerId()++) {}

    explicit WebviewController(int32_t nwebId) : controllerId_(GetNextControllerId()++) {}

    explicit WebviewController(const std::string& webTag) : controllerId_(GetNextControllerId()++) {}

    ~WebviewController() = default;

    uint64_t GetControllerId() const { return controllerId_; }

private:
    static std::atomic<uint64_t>& GetNextControllerId()
    {
        static std::atomic<uint64_t> nextControllerId{1};
        return nextControllerId;
    }

    uint64_t controllerId_ = 0;
};

} // namespace NWeb
} // namespace OHOS

#endif // TEST_MOCK_WEBVIEW_CONTROLLER_H
