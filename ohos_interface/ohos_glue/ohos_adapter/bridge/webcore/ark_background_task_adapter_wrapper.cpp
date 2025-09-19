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

#include "ohos_adapter/bridge/ark_background_task_adapter_wrapper.h"
#include "ohos_adapter/bridge/ark_background_state_change_callback_adapter_impl.h"
#include "background_task_adapter.h"
#include "ohos_adapter/include/ark_background_task_adapter.h"
#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::NWeb {

bool BackgroundTaskAdapter::RequestBackgroundRunning(bool running, BackgroundModeAdapter bgMode)
{
    return ArkWeb::ArkBackgroundTaskAdapter::RequestBackgroundRunning(running, (int32_t)bgMode);
}

} // namespace OHOS::NWeb

namespace OHOS::ArkWeb {
ArkBackgroundTaskAdapterWrapper::ArkBackgroundTaskAdapterWrapper(
    ArkWebRefPtr<ArkBackgroundTaskAdapter> ref) : ctocpp_(ref)
{}

bool ArkBackgroundTaskAdapterWrapper::RequestBackgroundTaskRunning(
    bool running, NWeb::BackgroundModeAdapter bgMode)
{
    return ctocpp_->RequestBackgroundTaskRunning(running, (int32_t)bgMode);
}

void ArkBackgroundTaskAdapterWrapper::RegisterBackgroundTaskPolicyCallback(
    std::shared_ptr<NWeb::BackgroundStateChangeCallbackAdapter> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return;
    }

    ctocpp_->RegisterBackgroundTaskPolicyCallback(new ArkBackgroundStateChangeCallbackAdapterImpl(callback));
}
} // namespace OHOS::ArkWeb