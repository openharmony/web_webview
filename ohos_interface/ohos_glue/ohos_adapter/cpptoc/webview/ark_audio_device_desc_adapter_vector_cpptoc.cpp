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

#include "ohos_adapter/cpptoc/ark_audio_device_desc_adapter_vector_cpptoc.h"

#include "ohos_adapter/bridge/ark_audio_device_desc_adapter_impl.h"
#include "ohos_adapter/cpptoc/ark_audio_device_desc_adapter_cpptoc.h"

namespace OHOS::ArkWeb {

ArkAudioDeviceDescAdapterVector ArkAudioDeviceDescAdapterVectorClassToStruct(
    const std::vector<std::shared_ptr<NWeb::AudioDeviceDescAdapter>>& class_value)
{
    ArkAudioDeviceDescAdapterVector struct_value = { .size = class_value.size(),
        .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (_ark_audio_device_desc_adapter_t**)ArkWebMemMalloc(
            sizeof(_ark_audio_device_desc_adapter_t*) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }

        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            if (!(*it)) {
                continue;
            }
            ArkWebRefPtr<ArkAudioDeviceDescAdapter> adapter = new ArkAudioDeviceDescAdapterImpl(*it);
            struct_value.value[count] = ArkAudioDeviceDescAdapterCppToC::Invert(adapter);
            count++;
        }
    }

    return struct_value;
}

} // namespace OHOS::ArkWeb
