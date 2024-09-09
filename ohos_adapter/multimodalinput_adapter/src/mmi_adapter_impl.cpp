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

void OnDeviceAdded(int32_t deviceId)
{
    std::shared_ptr<MMIListenerAdapter> deviceListener = MMIAdapterImpl::GetInstance().GetDeviceAdapterListener();
    std::string type = MMIAdapterImpl::GetInstance().GetType();
    if (deviceListener) {
        deviceListener->OnDeviceAdded(deviceId, type);
    }
}

void OnDeviceRemoved(int32_t deviceId)
{
    std::shared_ptr<MMIListenerAdapter> deviceListener = MMIAdapterImpl::GetInstance().GetDeviceAdapterListener();
    std::string type = MMIAdapterImpl::GetInstance().GetType();
    if (deviceListener) {
        deviceListener->OnDeviceRemoved(deviceId, type);
    }
}

MMIAdapterImpl& MMIAdapterImpl::GetInstance()
{
    static MMIAdapterImpl instance;
    return instance;
}

std::shared_ptr<MMIListenerAdapter> MMIAdapterImpl::GetDeviceAdapterListener()
{
    return inputDeviceAdapterListener_;
}

std::string MMIAdapterImpl::GetType()
{
    return type_;
}

char* MMIAdapterImpl::KeyCodeToString(int32_t keyCode)
{
    WVLOG_W("deprecated key code to string");
    return NULL;
}

int32_t MMIAdapterImpl::RegisterMMIInputListener(std::shared_ptr<MMIInputListenerAdapter> eventCallback)
{
    WVLOG_W("deprecated register MMI input listener");
    return 0;
}

void MMIAdapterImpl::UnregisterMMIInputListener(int32_t monitorId)
{
    WVLOG_W("deprecated unregister MMI input listener");
}

int32_t MMIAdapterImpl::RegisterDevListener(std::string type, std::shared_ptr<MMIListenerAdapter> listener)
{
    if (!listener) {
        WVLOG_E("register device listener is nullptr");
        return -1;
    }
    inputDeviceAdapterListener_ = listener;
    type_ = type;
    inputDeviceListener_ = {
        .deviceAddedCallback = OnDeviceAdded,
        .deviceRemovedCallback = OnDeviceRemoved,
    };

    return OH_Input_RegisterDeviceListener(&inputDeviceListener_);
}

int32_t MMIAdapterImpl::UnregisterDevListener(std::string type)
{
    if (type == type_) {
        return OH_Input_UnregisterDeviceListener(&inputDeviceListener_);
    }
    return -1;
}

int32_t MMIAdapterImpl::GetKeyboardType(int32_t deviceId, int32_t& type)
{
    return OH_Input_GetKeyboardType(deviceId, &type);
}

int32_t MMIAdapterImpl::GetDeviceIds(std::vector<int32_t>& ids)
{
    const size_t size = ids.size();
    if (size == 0) {
        return -1;
    }
    int32_t idsArr[size];
    for (size_t i = 0; i < size; ++i) {
        idsArr[i] = ids[i];
    }
    int32_t outSize = 0;
    Input_Result result = OH_Input_GetDeviceIds(idsArr, size, &outSize);
    if (result != INPUT_SUCCESS) {
        return -1;
    }
    if (outSize <= 0) {
        return outSize;
    }
    ids.clear();
    for (int32_t i = 0; i < outSize; ++i) {
        ids.push_back(idsArr[i]);
    }
    return 0;
}

int32_t MMIAdapterImpl::GetDeviceInfo(int32_t deviceId, std::shared_ptr<MMIDeviceInfoAdapter> info)
{
    if (!info) {
        WVLOG_E("GetDeviceInfo info is nullptr");
        return -1;
    }
    Input_DeviceInfo *deviceInfo = OH_Input_CreateDeviceInfo();
    if (deviceInfo == nullptr) {
        WVLOG_E("create device info error");
        return -1;
    }
    OH_Input_GetDevice(deviceId, &deviceInfo);
    int32_t id = -1;
    int32_t version = -1;
    int32_t product = -1;
    int32_t vendor = -1;
    char *name = nullptr;
    char *address = nullptr;
    int32_t capabilities = -1;

    OH_Input_GetDeviceId(deviceInfo, &id);
    OH_Input_GetDeviceVersion(deviceInfo, &version);
    OH_Input_GetDeviceProduct(deviceInfo, &product);
    OH_Input_GetDeviceVendor(deviceInfo, &vendor);
    OH_Input_GetDeviceName(deviceInfo, &name);
    OH_Input_GetDeviceAddress(deviceInfo, &address);
    OH_Input_GetCapabilities(deviceInfo, &capabilities);

    info->SetId(id);
    info->SetType(capabilities);
    info->SetVersion(version);
    info->SetProduct(product);
    info->SetVendor(vendor);
    info->SetName(name);
    info->SetPhys(address);

    OH_Input_DestroyDeviceInfo(&deviceInfo);
    return 0;
}
} // namespace OHOS::NWeb
