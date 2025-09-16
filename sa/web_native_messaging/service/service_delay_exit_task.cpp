/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "service_delay_exit_task.h"

#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
namespace {
static const std::string DELAY_EXIT_TASK = "DelayExitTask";
static const int32_t DELAY_EXIT_MILLISECONDS = 30 * 1000; // 30s
}

ServiceDelayExitTask::ServiceDelayExitTask(const std::shared_ptr<ServiceEventHandler>& handler,
    std::function<void ()> exitTask)
{
    serviceHandler_ = handler;
    exitTask_ = exitTask;
}

void ServiceDelayExitTask::Start()
{
    if (serviceHandler_ == nullptr) {
        WNMLOG_E("serviceHandler_ is null");
        return;
    }
    WNMLOG_I("Delay exit service after %{public}d ms", DELAY_EXIT_MILLISECONDS);
    serviceHandler_->ProxyPostTask([exitTask = exitTask_] () {
        if (exitTask) {
            exitTask();
        }
    }, DELAY_EXIT_TASK, DELAY_EXIT_MILLISECONDS);
}

void ServiceDelayExitTask::Stop()
{
    if (serviceHandler_ == nullptr) {
        WNMLOG_E("serviceHandler_ is null");
        return;
    }
    WNMLOG_I("service delay exit handler stop");
    serviceHandler_->ProxyRemoveTask(DELAY_EXIT_TASK);
}
} // namespace OHOS::NWeb
