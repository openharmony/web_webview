/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef BACKGROUND_TASK_ADAPTER_H
#define BACKGROUND_TASK_ADAPTER_H

#include <cstdint>

namespace OHOS::NWeb {

enum class BackgroundModeAdapter : int32_t {
    DATA_TRANSFER = 1,
    AUDIO_PLAYBACK,
    AUDIO_RECORDING,
    LOCATION,
    BLUETOOTH_INTERACTION,
    MULTI_DEVICE_CONNECTION,
    WIFI_INTERACTION,
    VOIP,
    TASK_KEEPING,
};

class BackgroundTaskAdapter {
public:
    BackgroundTaskAdapter() = default;

    virtual ~BackgroundTaskAdapter() = default;

    static bool RequestBackgroundRunning(bool running, BackgroundModeAdapter bgMode);
};

} // namespace OHOS::NWeb

#endif // BACKGROUND_TASK_ADAPTER_H
