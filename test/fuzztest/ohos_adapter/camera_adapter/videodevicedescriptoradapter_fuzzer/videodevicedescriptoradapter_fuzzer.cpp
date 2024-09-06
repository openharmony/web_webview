/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "videodevicedescriptoradapter_fuzzer.h"

#define private public
#include "camera_manager_adapter_impl.h"
#include "nweb_surface_adapter.h"
#include "video_device_descriptor_adapter_impl.cpp"
#include "video_device_descriptor_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {

bool VideoDeviceDescriptorAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    VideoDeviceDescriptorAdapterImpl adapter;
    std::string name((const char*)data, size);
    std::string deviceId((const char*)data, size);
    std::string modelId((const char*)data, size);
    adapter.SetDisplayName(name);
    adapter.SetDeviceId(deviceId);
    adapter.SetModelId(modelId);

    adapter.SetTransportType(VideoTransportType::VIDEO_TRANS_TYPE_USB);

    adapter.SetFacingMode(VideoFacingModeAdapter::FACING_USER);

    adapter.GetDisplayName();
    adapter.GetDeviceId();
    adapter.GetModelId();
    adapter.GetControlSupport();
    adapter.GetTransportType();
    adapter.GetFacingMode();
    adapter.GetSupportCaptureFormats();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::VideoDeviceDescriptorAdapterFuzzTest(data, size);
    return 0;
}
