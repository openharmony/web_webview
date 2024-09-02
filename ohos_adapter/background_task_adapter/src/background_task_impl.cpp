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

#include "background_task_adapter.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

bool BackgroundTaskAdapter::RequestBackgroundRunning(bool running, BackgroundModeAdapter bgMode)
{
    PRINT_MOCK_LOG();
    return true;
}
} // namespace OHOS::NWeb
