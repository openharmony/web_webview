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

void MMIListenerAdapterImpl::OnDeviceAdded(int32_t deviceId, const std::string& type)
{
    if (listener_) {
        listener_->OnDeviceAdded(deviceId, type);
    }
};

void MMIListenerAdapterImpl::OnDeviceRemoved(int32_t deviceId, const std::string& type)
{
    if (listener_) {
        listener_->OnDeviceRemoved(deviceId, type);
    }
};

MMIInputListenerAdapterImpl::MMIInputListenerAdapterImpl(std::shared_ptr<MMIInputListenerAdapter> listener)
    : listener_(listener) {};

MMIInputListenerAdapterImpl::~MMIInputListenerAdapterImpl()
{
    listener_ = nullptr;
};

void MMIInputListenerAdapterImpl::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    if (!listener_ || !keyEvent) {
        return;
    }
    if (keyEvent->GetKeyAction() != MMI::KeyEvent::KEY_ACTION_DOWN &&
        keyEvent->GetKeyAction() != MMI::KeyEvent::KEY_ACTION_UP) {
        return;
    }
    int32_t keyAction = (keyEvent->GetKeyAction() == MMI::KeyEvent::KEY_ACTION_DOWN) ? KEY_DOWN : KEY_UP;
    listener_->OnInputEvent(keyEvent->GetKeyCode(), keyAction);
};

void MMIInputListenerAdapterImpl::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const {};

void MMIInputListenerAdapterImpl::OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const {};

char* MMIAdapterImpl::KeyCodeToString(int32_t keyCode)
{
    return const_cast<char*>(MMI::KeyEvent::KeyCodeToString(keyCode));
}

int32_t MMIAdapterImpl::RegisterMMIInputListener(std::shared_ptr<MMIInputListenerAdapter> eventCallback)
{
    if (!eventCallback) {
        WVLOG_E("register input listener is nullptr");
        return -1;
    }
    inputListener_ = std::make_shared<MMIInputListenerAdapterImpl>(eventCallback);
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    int32_t id = manager->AddMonitor(inputListener_);
    WVLOG_D("RegisterMMIInputListener id = %{public}d", id);
    return id;
};

void MMIAdapterImpl::UnregisterMMIInputListener(int32_t monitorId)
{
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return;
    }
    manager->RemoveMonitor(monitorId);
};

int32_t MMIAdapterImpl::RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener)
{
    if (!listener) {
        WVLOG_E("register device listener is nullptr");
        return -1;
    }
    devListener_ = std::make_shared<MMIListenerAdapterImpl>(listener);
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    return manager->RegisterDevListener(type, devListener_);
};

int32_t MMIAdapterImpl::UnregisterDevListener(std::string type)
{
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    return manager->UnregisterDevListener(type, devListener_);
};

int32_t MMIAdapterImpl::GetKeyboardType(int32_t deviceId, int32_t& type)
{
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    std::function<void(int32_t)> callback = [&type](int32_t param) { type = param; };
    return manager->GetKeyboardType(deviceId, callback);
};

int32_t MMIAdapterImpl::GetDeviceIds(std::vector<int32_t>& ids)
{
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    std::function<void(std::vector<int32_t>&)> callback = [&ids](std::vector<int32_t>& param) { ids = param; };
    return manager->GetDeviceIds(callback);
};

int32_t MMIAdapterImpl::GetDeviceInfo(int32_t deviceId, std::shared_ptr<MMIDeviceInfoAdapter> info)
{
    if (!info) {
        WVLOG_E("GetDeviceInfo info is nullptr");
        return -1;
    }

    std::function<void(std::shared_ptr<MMI::InputDevice>)> callback = [&info](
                                                                          std::shared_ptr<MMI::InputDevice> device) {
        if (device) {
            info->SetId(device->GetId());
            info->SetType(device->GetType());
            info->SetBus(device->GetBus());
            info->SetVersion(device->GetVersion());
            info->SetProduct(device->GetProduct());
            info->SetVendor(device->GetVendor());
            info->SetName(device->GetName());
            info->SetPhys(device->GetPhys());
            info->SetUniq(device->GetUniq());
        }
    };
    auto manager = InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    int32_t ret = manager->GetDevice(
        deviceId, [&callback](std::shared_ptr<MMI::InputDevice> device) { callback(device); });
    if (ret != 0) {
        WVLOG_E("InputManager GetDevice failed, ret: %{public}d", ret);
    }
    return ret;
}

int32_t MMIAdapterImpl::GetMaxTouchPoints()
{
    int32_t pointNum { 0 };
    auto manager = MMI::InputManager::GetInstance();
    if (!manager) {
        WVLOG_E("InputManager::GetInstance failed");
        return -1;
    }
    int32_t ret = manager->GetMaxMultiTouchPointNum(pointNum);
    WVLOG_D("MMIAdapterImpl::GetMaxTouchPoints, pointNum: %{public}d", pointNum);
    if (ret != 0) {
        WVLOG_E("InputManager GetMaxTouchPoints failed, ret: %{public}d", ret);
        return 0;
    }
    return pointNum;
}
} // namespace OHOS::NWeb
