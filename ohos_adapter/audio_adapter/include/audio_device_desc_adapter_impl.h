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

#ifndef AUDIO_DEVICE_DESC_ADAPTER_IMPL_H
#define AUDIO_DEVICE_DESC_ADAPTER_IMPL_H

#include "audio_system_manager_adapter.h"

namespace OHOS::NWeb {

extern const int32_t ADAPTER_AUDIO_UNDEFINED_DEVICE_ID;
extern const char* ADAPTER_AUDIO_UNDEFINED_DEVICE_NAME;

class AudioDeviceDescAdapterImpl : public AudioDeviceDescAdapter {
public:
    AudioDeviceDescAdapterImpl() = default;

    int32_t GetDeviceId() override;

    std::string GetDeviceName() override;

    void SetDeviceId(int32_t id);
  
    void SetDeviceName(std::string name);

private:
    int32_t device_id_ = ADAPTER_AUDIO_UNDEFINED_DEVICE_ID;
    std::string device_name_ = ADAPTER_AUDIO_UNDEFINED_DEVICE_NAME;
};

} // namespace OHOS::NWeb

#endif // AUDIO_DEVICE_DESC_ADAPTER_IMPL_H
