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
#ifndef NWEB_WEB_NATIVE_MESSAGING_SERVICE_DELAY_EXIT_TASK_H
#define NWEB_WEB_NATIVE_MESSAGING_SERVICE_DELAY_EXIT_TASK_H

#include <functional>
#include <memory>
#include <string>
#include "nocopyable.h"
#include "service_event_handler.h"

namespace OHOS::NWeb {
class ServiceDelayExitTask {
public:
    ServiceDelayExitTask(const std::shared_ptr<ServiceEventHandler>& handler, std::function<void ()> exitTask);
    virtual ~ServiceDelayExitTask() = default;

    void Start();
    void Stop();
private:
    std::shared_ptr<ServiceEventHandler> serviceHandler_;
    std::function<void ()> exitTask_ = [] () { return; };

    DISALLOW_COPY_AND_MOVE(ServiceDelayExitTask);
};
} // namespace OHOS::NWeb
#endif // NWEB_WEB_NATIVE_MESSAGING_SERVICE_DELAY_EXIT_TASK_H
