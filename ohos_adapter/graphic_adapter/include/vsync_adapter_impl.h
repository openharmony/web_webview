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

#ifndef VSYNC_ADAPTER_IMPL_H
#define VSYNC_ADAPTER_IMPL_H

#include <functional>
#include <memory>
#include <mutex>
#include <native_vsync/native_vsync.h>

#include "graphic_adapter.h"

namespace OHOS::NWeb {
class VSyncAdapterImpl : public VSyncAdapter {
public:
    VSyncAdapterImpl() = default;
    ~VSyncAdapterImpl() override;
    VSyncAdapterImpl(const VSyncAdapterImpl&) = delete;
    VSyncAdapterImpl& operator=(const VSyncAdapterImpl&) = delete;

    static VSyncAdapterImpl& GetInstance();
    VSyncErrorCode RequestVsync(void* data, NWebVSyncCb cb) override;
    int64_t GetVSyncPeriod() override;
    void SetFrameRateLinkerEnable(bool enabled) override;
    void SetFramePreferredRate(int32_t preferredRate) override;

    void SetOnVsyncCallback(void (*callback)()) override;
    void SetIsGPUProcess(bool isGPU);
    void SetOnVsyncEndCallback(void (*onVsyncEndCallback)()) override;
    void SetDVSyncSwitch(bool dvsyncSwitch) override;
private:
    static void OnVsync(long long timestamp, void* data);
    void VsyncCallbackInner(long long nanoTimestamp);
    VSyncErrorCode Init();

    std::mutex mtx_;
    bool hasRequestedVsync_ = false;
    OH_NativeVSync* vsyncReceiver_ = nullptr;
    std::unordered_map<void*, NWebVSyncCb> vsyncCallbacks_;
    OH_NativeVSync_FrameCallback frameCallback_ = OnVsync;
    static void (*callback_)();
    static void (*onVsyncEndCallback_)();
    bool isGPUProcess_ = false;
};
} // namespace OHOS::NWeb

#endif // VSYNC_ADAPTER_IMPL_H
