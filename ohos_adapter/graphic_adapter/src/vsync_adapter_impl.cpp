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

#include "vsync_adapter_impl.h"

#include "nweb_log.h"
#include <unistd.h>
#include <native_vsync/graphic_error_code.h>
#include "system_properties_adapter_impl.h"

namespace OHOS::NWeb {

namespace {
const std::string THREAD_NAME = "VSync-webview";
}

void (*VSyncAdapterImpl::callback_)() = nullptr;
void (*VSyncAdapterImpl::onVsyncEndCallback_)() = nullptr;

VSyncAdapterImpl::~VSyncAdapterImpl()
{
    OH_NativeVSync_Destroy(vsyncReceiver_);
}

VSyncAdapterImpl& VSyncAdapterImpl::GetInstance()
{
    static VSyncAdapterImpl instance;
    return instance;
}

VSyncErrorCode VSyncAdapterImpl::Init()
{
    if (!vsyncReceiver_) {
        const std::string vsyncName = "WebCore_" + std::to_string(getpid());
        vsyncReceiver_ = OH_NativeVSync_Create(vsyncName.c_str(), vsyncName.length());
        if (!vsyncReceiver_) {
            WVLOG_E("CreateVSyncReceiver failed");
            return VSyncErrorCode::ERROR;
        }
    }
    return VSyncErrorCode::SUCCESS;
}

VSyncErrorCode VSyncAdapterImpl::RequestVsync(void* data, NWebVSyncCb cb)
{
    if (Init() != VSyncErrorCode::SUCCESS) {
        WVLOG_E("NWebWindowAdapter init fail");
        return VSyncErrorCode::ERROR;
    }

    std::lock_guard<std::mutex> lock(mtx_);
    vsyncCallbacks_.insert({data, cb});

    if (hasRequestedVsync_) {
        return VSyncErrorCode::SUCCESS;
    }

    auto ret = OH_NativeVSync_RequestFrame(vsyncReceiver_, frameCallback_, this);
    if (ret != NATIVE_ERROR_OK) {
        WVLOG_E("NWebWindowAdapter RequestNextVSync fail, ret=%{public}d", ret);
        return VSyncErrorCode::ERROR;
    }
    hasRequestedVsync_ = true;
    return VSyncErrorCode::SUCCESS;
}

void VSyncAdapterImpl::OnVsync(long long timestamp, void* client)
{
    auto vsyncClient = static_cast<VSyncAdapterImpl*>(client);
    if (vsyncClient) {
        if (callback_) {
            callback_();
        }
        vsyncClient->VsyncCallbackInner(timestamp);
        if (onVsyncEndCallback_) {
            onVsyncEndCallback_();
        }
    } else {
        WVLOG_E("VsyncClient is null");
    }
}

void VSyncAdapterImpl::VsyncCallbackInner(long long timestamp)
{
    std::unordered_map<void*, NWebVSyncCb> vsyncCallbacks;
    std::lock_guard<std::mutex> lock(mtx_);
    vsyncCallbacks = vsyncCallbacks_;
    vsyncCallbacks_.clear();

    for (const auto& callback : vsyncCallbacks) {
        auto func = callback.second;
        if (func) {
            func(timestamp, callback.first);
        }
    }
    hasRequestedVsync_ = false;
}

int64_t VSyncAdapterImpl::GetVSyncPeriod()
{
    int64_t period = 0;
    if (Init() != VSyncErrorCode::SUCCESS) {
        WVLOG_E("NWebWindowAdapter init fail");
        return period;
    }

    auto ret = OH_NativeVSync_GetPeriod(vsyncReceiver_, reinterpret_cast<long long *>(&period));
    if (ret != NATIVE_ERROR_OK) {
        WVLOG_E("NWebWindowAdapter GetVSyncPeriod fail, ret=%{public}d", ret);
    }
    return period;
}

void VSyncAdapterImpl::SetFrameRateLinkerEnable(bool enabled)
{
    WVLOG_D("[adapter mock] SetFrameRateLinkerEnable");
}

void VSyncAdapterImpl::SetFramePreferredRate(int32_t preferredRate)
{
    WVLOG_D("[adapter mock] SetFrameRateLinkerEnable");
}

void VSyncAdapterImpl::SetOnVsyncCallback(void (*callback)())
{
    WVLOG_D("callback function: %{public}ld", (long)callback);
    callback_ = callback;
}

void VSyncAdapterImpl::SetIsGPUProcess(bool isGPU)
{
    isGPUProcess_ = isGPU;
}

void VSyncAdapterImpl::SetOnVsyncEndCallback(void (*onVsyncEndCallback)())
{
    WVLOG_D("callback function: %{public}ld", (long)onVsyncEndCallback);
    onVsyncEndCallback_ = onVsyncEndCallback;
}

void VSyncAdapterImpl::SetDVSyncSwitch(bool dvsyncSwitch)
{
    if (Init() != VSyncErrorCode::SUCCESS) {
        WVLOG_E("NWebWindowAdatrper init fail!");
        return;
    }

    if (!vsyncReceiver_) {
        WVLOG_E("NWebWindowAdatrper SetDVSyncSwitch: receiver_ is nullptr!");
        return;
    }

    WVLOG_D("NWebWindowAdatrper SetDVSyncSwitch: dvsyncSwitch = %{public}d", dvsyncSwitch);
    int ret = OH_NativeVSync_DVSyncSwitch(vsyncReceiver_, dvsyncSwitch);
    if (ret != 0) {
        WVLOG_E("SetNativeDVSyncSwitch failed, ret = %{public}d", ret);
        return;
    }
}
} // namespace OHOS::NWeb
