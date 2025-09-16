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

#ifndef ARK_BACKGROUND_TASK_ADAPTER_IMPL_H
#define ARK_BACKGROUND_TASK_ADAPTER_IMPL_H
#pragma once

#include "background_task_adapter.h"
#include "ohos_adapter/include/ark_background_task_adapter.h"

namespace OHOS::ArkWeb {

class ArkBackgroundTaskAdapterImpl : public ArkBackgroundTaskAdapter {
public:
    explicit ArkBackgroundTaskAdapterImpl(
        std::shared_ptr<OHOS::NWeb::BackgroundTaskAdapter> ref);

    bool RequestBackgroundTaskRunning(bool running, int32_t bgMode) override;
    void RegisterBackgroundTaskPolicyCallback(
        const ArkWebRefPtr<ArkBackgroundStateChangeCallbackAdapter> callback) override;

private:
    std::shared_ptr<OHOS::NWeb::BackgroundTaskAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkBackgroundTaskAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_BACKGROUND_TASK_ADAPTER_IMPL_H
