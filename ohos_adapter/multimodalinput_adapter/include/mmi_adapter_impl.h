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

#ifndef MMI_ADAPTER_IMPL_H
#define MMI_ADAPTER_IMPL_H

#include "mmi_adapter.h"

#include "input_manager.h"
#include "key_event.h"

namespace OHOS::NWeb {
class MMIListenerAdapterImpl : public MMI::IInputDeviceListener {
public:
    explicit MMIListenerAdapterImpl(std::shared_ptr<MMIListenerAdapter> listener);

    ~MMIListenerAdapterImpl() override;

    void OnDeviceAdded(int32_t deviceId, const std::string &type) override;

    void OnDeviceRemoved(int32_t deviceId, const std::string &type) override;

private:
    std::shared_ptr<MMIListenerAdapter> listener_ = nullptr;
};

class MMIInputListenerAdapterImpl : public MMI::IInputEventConsumer {
public:
    explicit MMIInputListenerAdapterImpl(const InputEventCallback& listener);

    ~MMIInputListenerAdapterImpl() override;

    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override;

    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override;

    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override;

private:
    InputEventCallback listener_ = nullptr;
};

class MMIAdapterImpl : public MMIAdapter {
public:
    MMIAdapterImpl() = default;

    ~MMIAdapterImpl() override = default;

    const char* KeyCodeToString(int32_t keyCode) override;

    int32_t RegisterMMIInputListener(const InputEventCallback&& eventCallback) override;

    void UnregisterMMIInputListener(int32_t monitorId) override;

    int32_t RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener) override;

    int32_t UnregisterDevListener(std::string type) override;

    int32_t GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback) override;

    int32_t GetDeviceIds(std::function<void(std::vector<int32_t>&)> callback) override;

private:
    std::shared_ptr<MMI::IInputDeviceListener> devListener_ = nullptr;
    std::shared_ptr<MMI::IInputEventConsumer> inputListener_ = nullptr;
};
}  // namespace OHOS::NWeb

#endif // MMI_ADAPTER_IMPL_H