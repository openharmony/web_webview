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

#ifndef ARK_VIDEO_DEVICE_DESCRIPTOR_ADAPTER_VECTOR_CPPTOC_H
#define ARK_VIDEO_DEVICE_DESCRIPTOR_ADAPTER_VECTOR_CPPTOC_H

#pragma once

#include "camera_manager_adapter.h"
#include "ohos_adapter/include/ark_video_device_descriptor_adapter_vector.h"

namespace OHOS::ArkWeb {

ArkVideoDeviceDescriptorAdapterVector ArkVideoDeviceDescriptorAdapterVectorClassToStruct(
    const std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>>& class_value);

} // namespace OHOS::ArkWeb

#endif // ARK_VIDEO_DEVICE_DESCRIPTOR_ADAPTER_VECTOR_CPPTOC_H