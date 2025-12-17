/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "flowbuffer_adapter_impl.h"

#include <cstring>
#include <sys/mman.h>
#include <sys/prctl.h>

#include "ashmem.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "res_sched_client_adapter.h"
#include "securec.h"

namespace OHOS::NWeb {
namespace {
    const int64_t PERFORMANCE_PERIOD_MS = 300;
}

int64_t FlowbufferAdapterImpl::timeStamp_ = 0;
int64_t FlowbufferAdapterImpl::preTimeStamp_ = 0;

FlowbufferAdapterImpl::~FlowbufferAdapterImpl()
{
    if (data_ != nullptr) {
        if (::munmap(data_, size_) != 0) {
            WVLOG_E("Failed to unmap ashmem region");
        }
        data_ = nullptr;
        size_ = 0;
    }
}

bool FlowbufferAdapterImpl::NeedReportScene()
{
    auto currentTime = std::chrono::system_clock::now().time_since_epoch();
    timeStamp_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime).count();
    if (timeStamp_ - preTimeStamp_ > PERFORMANCE_PERIOD_MS) {
        preTimeStamp_ = timeStamp_;
        return true;
    }
    return false;
}

void FlowbufferAdapterImpl::StartPerformanceBoost()
{
    if (!NeedReportScene()) {
        return;
    }
    OHOS::NWeb::ResSchedClientAdapter::ReportScene(
        OHOS::NWeb::ResSchedStatusAdapter::WEB_SCENE_ENTER, OHOS::NWeb::ResSchedSceneAdapter::KEY_TASK);
}

void* FlowbufferAdapterImpl::CreateAshmem(size_t size, int mapType, int& fd)
{
    fd = AshmemCreate("WebFlowBuffer", size);
    if (fd < 0) {
        WVLOG_E("Create ashmem failed, fd: %{public}d", fd);
        return nullptr;
    }

    int result = AshmemSetProt(fd, PROT_READ | PROT_WRITE);
    if (result < 0) {
        close(fd);
        fd = -1;
        WVLOG_E("Ashmem set port failed, result: %{public}d", result);
        return nullptr;
    }

    void *startAddr = ::mmap(nullptr, size, mapType, MAP_SHARED, fd, 0);
    if (startAddr == MAP_FAILED) {
        close(fd);
        fd = -1;
        WVLOG_E("Map ashmem failed");
        return nullptr;
    }
    prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, startAddr, size, "web_flowbuffer");
    data_ = startAddr;
    size_ = size;
    return startAddr;
}

void* FlowbufferAdapterImpl::CreateAshmemWithFd(const int fd, size_t size, int mapType)
{
    if (fd < 0) {
        WVLOG_E("CreateAshmemWithFd failed, fd: %{public}d", fd);
        return nullptr;
    }

    int ashmemSize = AshmemGetSize(fd);
    if (ashmemSize < 0 || size_t(ashmemSize) < size) {
        WVLOG_E("CreateAshmemWithFd failed, ashmemSize: %{public}d, size: %{public}zu", ashmemSize, size);
        return nullptr;
    }

    void *startAddr = ::mmap(nullptr, size, mapType, MAP_SHARED, fd, 0);
    if (startAddr == MAP_FAILED) {
        close(fd);
        WVLOG_E("Map ashmem failed");
        return nullptr;
    }
    prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, startAddr, size, "web_flowbuffer");
    data_ = startAddr;
    size_ = size;
    return startAddr;
}
} // namespace OHOS::NWeb
