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

#include "videocontrolsupportadapter_fuzzer.h"

#define private public
#include "camera_device.h"
#include "camera_manager.h"
#include "camera_manager_adapter_impl.h"
#include "nweb_surface_adapter.h"
#include "video_control_support_adapter_impl.cpp"
#include "video_control_support_adapter_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {

bool VideoControlSupportAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    OHOS::NWeb::VideoControlSupportAdapterImpl adapter;
    adapter.SetPan(true);
    adapter.SetTilt(true);
    adapter.SetZoom(true);

    // 获取adapter的值并忽略它们
    adapter.GetPan();
    adapter.GetTilt();
    adapter.GetZoom();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::VideoControlSupportAdapterFuzzTest(data, size);
    return 0;
}
