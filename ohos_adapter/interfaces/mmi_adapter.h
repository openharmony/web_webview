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

#ifndef MMI_ADAPTER_H
#define MMI_ADAPTER_H

#include <functional>

namespace OHOS::NWeb {
enum MMIAdapterKeyboardType : int32_t {
    NONE = 0,
    UNKNOWN_TYPE = 1,
    ALPHABETIC_KEYBOARD = 2,
    DIGITAL_KEYBOARD = 3,
    HANDWRITING_PEN = 4,
    REMOTE_CONTROL = 5,
};

class MMIListenerAdapter {
public:
    MMIListenerAdapter() = default;
    virtual ~MMIListenerAdapter() = default;
    virtual void OnDeviceAdded(int32_t deviceId, const std::string &type) = 0;
    virtual void OnDeviceRemoved(int32_t deviceId, const std::string &type) = 0;
};

using InputEventCallback = std::function<void(int32_t, int32_t)>;

class MMIAdapter {
public:
    MMIAdapter() = default;

    virtual ~MMIAdapter() = default;

    virtual const char* KeyCodeToString(int32_t keyCode) = 0;

    virtual int32_t RegisterMMIInputListener(const InputEventCallback&& eventCallback) = 0;

    virtual void UnregisterMMIInputListener(int32_t monitorId) = 0;

    virtual int32_t RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener) = 0;

    virtual int32_t UnregisterDevListener(std::string type) = 0;

    virtual int32_t GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback) = 0;

    virtual int32_t GetDeviceIds(std::function<void(std::vector<int32_t>&)> callback) = 0;
};
} // namespace OHOS::NWeb

#endif // MMI_ADAPTER_H