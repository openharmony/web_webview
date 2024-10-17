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

#include "ohos_adapter/bridge/ark_vsync_adapter_wrapper.h"

namespace OHOS::ArkWeb {

ArkVSyncAdapterWrapper::ArkVSyncAdapterWrapper(ArkWebRefPtr<ArkVSyncAdapter> ref) : ctocpp_(ref) {}

OHOS::NWeb::VSyncErrorCode ArkVSyncAdapterWrapper::RequestVsync(void* data, OHOS::NWeb::NWebVSyncCb cb)
{
    if (!ctocpp_) {
        return OHOS::NWeb::VSyncErrorCode::ERROR;
    }
    uint32_t result = ctocpp_->RequestVsync(data, reinterpret_cast<void*>(cb));
    return (OHOS::NWeb::VSyncErrorCode)result;
}

int64_t ArkVSyncAdapterWrapper::GetVSyncPeriod()
{
    if (!ctocpp_) {
        return 0;
    }
    return ctocpp_->GetVSyncPeriod();
}

void ArkVSyncAdapterWrapper::SetFrameRateLinkerEnable(bool enabled)
{
    if (!ctocpp_) {
        return;
    }
    return ctocpp_->SetFrameRateLinkerEnable(enabled);
}

void ArkVSyncAdapterWrapper::SetFramePreferredRate(int32_t preferredRate)
{
    if (!ctocpp_) {
        return;
    }
    return ctocpp_->SetFramePreferredRate(preferredRate);
}

void ArkVSyncAdapterWrapper::SetOnVsyncCallback(OnVsyncCallback callback)
{
    if (!ctocpp_) {
        return;
    }
    return ctocpp_->SetOnVsyncCallback(callback);
}

void ArkVSyncAdapterWrapper::SetOnVsyncEndCallback(OnVsyncCallback onVsyncEndCallback)
{
    if (!ctocpp_) {
        return;
    }
    return ctocpp_->SetOnVsyncEndCallback(onVsyncEndCallback);
}

} // namespace OHOS::ArkWeb
