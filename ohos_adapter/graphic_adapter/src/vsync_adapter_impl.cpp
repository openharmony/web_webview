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

#include "aafwk_browser_client_adapter_impl.h"
#include "nweb_log.h"
#include "res_sched_client_adapter.h"
#include "system_properties_adapter_impl.h"
#include "transaction/rs_interfaces.h"

namespace OHOS::NWeb {
namespace {
const std::string THREAD_NAME = "VSync-webview";
constexpr int32_t WEBVIEW_FRAME_RATE_TYPE = 4;
}

void (*VSyncAdapterImpl::callback_)() = nullptr;
void (*VSyncAdapterImpl::onVsyncEndCallback_)() = nullptr;

VSyncAdapterImpl::~VSyncAdapterImpl()
{
    if (vsyncHandler_) {
        vsyncHandler_->RemoveAllEvents();
        auto runner = vsyncHandler_->GetEventRunner();
        if (runner) {
            runner->Stop();
            ResSchedClientAdapter::ReportKeyThread(ResSchedStatusAdapter::THREAD_DESTROYED,
                getprocpid(), runner->GetKernelThreadId(), ResSchedRoleAdapter::USER_INTERACT);
        }
        vsyncHandler_ = nullptr;
    }
    hasReportedKeyThread_ = false;
}

VSyncAdapterImpl& VSyncAdapterImpl::GetInstance()
{
    static VSyncAdapterImpl instance;
    return instance;
}

VSyncErrorCode VSyncAdapterImpl::Init()
{
    if (!receiver_) {
        if (!vsyncHandler_) {
            std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create(THREAD_NAME);
            vsyncHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
            runner->Run();
        }
        auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
        frameRateLinker_ = OHOS::Rosen::RSFrameRateLinker::Create();
        receiver_ = rsClient.CreateVSyncReceiver("NWeb_" + std::to_string(::getprocpid()),
            frameRateLinker_->GetId(), vsyncHandler_);
        if (!receiver_) {
            WVLOG_E("CreateVSyncReceiver failed");
            return VSyncErrorCode::ERROR;
        }
        VsyncError ret = receiver_->Init();
        if (ret != VsyncError::GSERROR_OK) {
            receiver_ = nullptr;
            WVLOG_E("vsync receiver init failed, ret=%{public}d", ret);
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

    if (!hasReportedKeyThread_) {
        auto runner = vsyncHandler_->GetEventRunner();
        // At this point, the threadId corresponding to eventrunner may not be available,
        // so need to confirm it several times
        if (runner && runner->GetKernelThreadId() != 0) {
            if (!isGPUProcess_) {
                ResSchedClientAdapter::ReportKeyThread(ResSchedStatusAdapter::THREAD_CREATED,
                    getprocpid(), runner->GetKernelThreadId(), ResSchedRoleAdapter::USER_INTERACT);
            } else {
                AafwkBrowserClientAdapterImpl::GetInstance().ReportThread(ResSchedStatusAdapter::THREAD_CREATED,
                    getprocpid(), runner->GetKernelThreadId(), ResSchedRoleAdapter::USER_INTERACT);
            }
            hasReportedKeyThread_ = true;
        }
    }

    std::lock_guard<std::mutex> lock(mtx_);
    vsyncCallbacks_.insert({data, cb});

    if (hasRequestedVsync_) {
        return VSyncErrorCode::SUCCESS;
    }

    VsyncError ret = receiver_->RequestNextVSync(frameCallback_);
    if (ret != VsyncError::GSERROR_OK) {
        WVLOG_E("NWebWindowAdapter RequestNextVSync fail, ret=%{public}d", ret);
        return VSyncErrorCode::ERROR;
    }
    hasRequestedVsync_ = true;
    return VSyncErrorCode::SUCCESS;
}

void VSyncAdapterImpl::OnVsync(int64_t timestamp, void* client)
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

void VSyncAdapterImpl::VsyncCallbackInner(int64_t timestamp)
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

    VsyncError ret = receiver_->GetVSyncPeriod(period);
    if (ret != VsyncError::GSERROR_OK) {
        WVLOG_E("NWebWindowAdapter GetVSyncPeriod fail, ret=%{public}d", ret);
    }
    return period;
}

void VSyncAdapterImpl::SetFrameRateLinkerEnable(bool enabled)
{
    WVLOG_D("NWebWindowAdapter SetFrameRateLinkerEnable enabled=%{public}d", enabled);
    if (frameRateLinkerEnable_ == enabled) {
        return;
    }

    if (frameRateLinker_) {
        if (!enabled) {
            Rosen::FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, 0, WEBVIEW_FRAME_RATE_TYPE};
            frameRateLinker_->UpdateFrameRateRangeImme(range);
        }
        frameRateLinker_->SetEnable(enabled);
        frameRateLinkerEnable_ = enabled;
    }
}

void VSyncAdapterImpl::SetFramePreferredRate(int32_t preferredRate)
{
    Rosen::FrameRateRange range = {0, RANGE_MAX_REFRESHRATE, preferredRate, WEBVIEW_FRAME_RATE_TYPE};
    if (frameRateLinker_ && frameRateLinker_->IsEnable()) {
        WVLOG_D("NWebWindowAdapter SetFramePreferredRate preferredRate=%{public}d", preferredRate);
        frameRateLinker_->UpdateFrameRateRangeImme(range);
    }
}

void VSyncAdapterImpl::SetOnVsyncCallback(void (*callback)())
{
    WVLOG_D("callback function: %{public}ld", (long)callback);
    callback_ = callback;
}

void VSyncAdapterImpl::SetOnVsyncEndCallback(void (*onVsyncEndCallback)())
{
    WVLOG_D("onVsyncEndCallback function: %{public}ld", (long)onVsyncEndCallback);
    onVsyncEndCallback_ = onVsyncEndCallback;
}

void VSyncAdapterImpl::SetIsGPUProcess(bool isGPU)
{
    isGPUProcess_ = isGPU;
}
} // namespace OHOS::NWeb
