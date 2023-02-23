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

#include "mmi_adapter_impl.h"

#include "nweb_log.h"

namespace OHOS::NWeb {
using namespace MMI;

const int32_t KEY_DOWN = 0;
const int32_t KEY_UP = 1;

MMIListenerAdapterImpl::MMIListenerAdapterImpl(std::shared_ptr<MMIListenerAdapter> listener) : listener_(listener) {};

MMIListenerAdapterImpl::~MMIListenerAdapterImpl()
{
    listener_ = nullptr;
};

void MMIListenerAdapterImpl::OnDeviceAdded(int32_t deviceId, const std::string &type)
{
    if (listener_) {
        listener_->OnDeviceAdded(deviceId, type);
    }
};

void MMIListenerAdapterImpl::OnDeviceRemoved(int32_t deviceId, const std::string &type)
{
    if (listener_) {
        listener_->OnDeviceRemoved(deviceId, type);
    }
};

MMIInputListenerAdapterImpl::MMIInputListenerAdapterImpl(const InputEventCallback& listener) : listener_(listener) {};

MMIInputListenerAdapterImpl::~MMIInputListenerAdapterImpl()
{
    listener_ = nullptr;
};

void MMIInputListenerAdapterImpl::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    if (!listener_) {
        return;
    }
    if (keyEvent->GetKeyAction() != MMI::KeyEvent::KEY_ACTION_DOWN &&
        keyEvent->GetKeyAction() != MMI::KeyEvent::KEY_ACTION_UP) {
        return;
    }
    int32_t keyAction = (keyEvent->GetKeyAction() == MMI::KeyEvent::KEY_ACTION_DOWN) ? KEY_DOWN : KEY_UP;
    listener_(keyEvent->GetKeyCode(), keyAction);
};

void MMIInputListenerAdapterImpl::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
};

void MMIInputListenerAdapterImpl::OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const
{
};

const char* MMIAdapterImpl::KeyCodeToString(int32_t keyCode)
{
    return MMI::KeyEvent::KeyCodeToString(keyCode);
}

int32_t MMIAdapterImpl::RegisterMMIInputListener(const InputEventCallback&& eventCallback)
{
    if (!eventCallback) {
        WVLOG_E("register input listener is nullptr");
        return -1;
    }
    inputListener_ = std::make_shared<MMIInputListenerAdapterImpl>(eventCallback);
    int32_t id = InputManager::GetInstance()->AddMonitor(inputListener_);
    WVLOG_D("RegisterMMIInputListener id = %{public}d", id);
    return id;
};

void MMIAdapterImpl::UnregisterMMIInputListener(int32_t monitorId)
{
    InputManager::GetInstance()->RemoveMonitor(monitorId);
};

int32_t MMIAdapterImpl::RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener)
{
    if (!listener) {
        WVLOG_E("register device listener is nullptr");
        return -1;
    }
    devListener_ = std::make_shared<MMIListenerAdapterImpl>(listener);
    return InputManager::GetInstance()->RegisterDevListener(type, devListener_);
};

int32_t MMIAdapterImpl::UnregisterDevListener(std::string type)
{
    return InputManager::GetInstance()->UnregisterDevListener(type, devListener_);
};

int32_t MMIAdapterImpl::GetKeyboardType(int32_t deviceId, std::function<void(int32_t)> callback)
{
    return InputManager::GetInstance()->GetKeyboardType(deviceId, callback);
};

int32_t MMIAdapterImpl::GetDeviceIds(std::function<void(std::vector<int32_t>&)> callback)
{
    return InputManager::GetInstance()->GetDeviceIds(callback);
};
}  // namespace OHOS::NWeb