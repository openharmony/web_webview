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

#include "ohos_adapter/ctocpp/ark_video_device_descriptor_adapter_vector_ctocpp.h"

#include "ohos_adapter/bridge/ark_video_device_descriptor_adapter_wrapper.h"
#include "ohos_adapter/ctocpp/ark_video_device_descriptor_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>> ArkVideoDeviceDescriptorAdapterVectorStructToClass(
    const ArkVideoDeviceDescriptorAdapterVector& struct_value)
{
    std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>> class_value;
    if (struct_value.size > 0) {
        for (int count = 0; count < struct_value.size; count++) {
            std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter> adapter =
                std::make_shared<ArkVideoDeviceDescriptorAdapterWrapper>(
                    ArkVideoDeviceDescriptorAdapterCToCpp::Invert(struct_value.value[count]));
            class_value.push_back(adapter);
        }
    }

    return class_value;
}

void ArkVideoDeviceDescriptorAdapterVectorStructRelease(ArkVideoDeviceDescriptorAdapterVector& struct_value)
{
    struct_value.size = 0;
    SAFE_FREE(struct_value.value, struct_value.ark_web_mem_free_func);
}

} // namespace OHOS::ArkWeb