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

#ifndef BACKGROUND_TASK_IMPL_H
#define BACKGROUND_TASK_IMPL_H
#include "background_task_adapter.h"
#include "application_state_change_callback.h"

namespace OHOS::NWeb {

class ApplicationStateChangeCallbackImpl : public AbilityRuntime::ApplicationStateChangeCallback {
public:
    ApplicationStateChangeCallbackImpl(std::shared_ptr<BackgroundStateChangeCallbackAdapter> cb);
    ~ApplicationStateChangeCallbackImpl() = default;
    void NotifyApplicationForeground() override;
    void NotifyApplicationBackground() override;

private:
    std::shared_ptr<BackgroundStateChangeCallbackAdapter> cb_ = nullptr;
};

class BackgroundTaskAdapterImpl : public BackgroundTaskAdapter {
public:
    BackgroundTaskAdapterImpl();
    ~BackgroundTaskAdapterImpl() override = default;
    bool RequestBackgroundTaskRunning(bool running, BackgroundModeAdapter bgMode) override;
    void RegisterBackgroundTaskPolicyCallback(std::shared_ptr<BackgroundStateChangeCallbackAdapter> callback) override;
private:
    std::shared_ptr<ApplicationStateChangeCallbackImpl> applicationStateChangeCallback_;
};

} // namespace OHOS::NWeb
#endif // BACKGROUND_TASK_IMPL_H