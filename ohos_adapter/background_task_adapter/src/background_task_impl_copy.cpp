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

#include <cstdint>
#include <unistd.h>

#include "background_mode.h"
#include "background_task_adapter.h"
#include "background_task_mgr_helper.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
using namespace OHOS::BackgroundTaskMgr;

BackgroundMode::Type ConvertBackgroundMode(BackgroundModeAdapter mode)
{
    switch (mode) {
        case BackgroundModeAdapter::DATA_TRANSFER:
            return BackgroundMode::Type::DATA_TRANSFER;
        case BackgroundModeAdapter::AUDIO_PLAYBACK:
            return BackgroundMode::Type::AUDIO_PLAYBACK;
        case BackgroundModeAdapter::AUDIO_RECORDING:
            return BackgroundMode::Type::AUDIO_RECORDING;
        case BackgroundModeAdapter::LOCATION:
            return BackgroundMode::Type::LOCATION;
        case BackgroundModeAdapter::BLUETOOTH_INTERACTION:
            return BackgroundMode::Type::BLUETOOTH_INTERACTION;
        case BackgroundModeAdapter::MULTI_DEVICE_CONNECTION:
            return BackgroundMode::Type::MULTI_DEVICE_CONNECTION;
        case BackgroundModeAdapter::WIFI_INTERACTION:
            return BackgroundMode::Type::WIFI_INTERACTION;
        case BackgroundModeAdapter::VOIP:
            return BackgroundMode::Type::VOIP;
        case BackgroundModeAdapter::TASK_KEEPING:
            return BackgroundMode::Type::TASK_KEEPING;
        default:
            return BackgroundMode::Type::AUDIO_PLAYBACK;
    }
}

bool BackgroundTaskAdapter::RequestBackgroundRunning(bool running, BackgroundModeAdapter bgMode)
{
    auto uid = getuid();
    WVLOG_I("RequestBackgroundRunning uid: %{public}d, running: %{public}d", uid, running);
    ContinuousTaskParamForInner taskParam { uid, ConvertBackgroundMode(bgMode), running };
    ErrCode errCode = BackgroundTaskMgrHelper::RequestBackgroundRunningForInner(taskParam);
    if (errCode != ERR_OK) {
        WVLOG_I("RequestBackgroundRunning failed, error code: %{public}d", errCode);
        return false;
    }
    return true;
}
} // namespace OHOS::NWeb
