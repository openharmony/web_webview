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

#include "event_handler_adapter_impl.h"

#include <sys/epoll.h>

namespace OHOS::NWeb {

#define WVLOG_E(...)
#define WVLOG_D(...)

void FfrtCallback(void* data, uint32_t events)
{
    EventHandlerFDListenerAdapterImpl *fileDescriptorListener = (EventHandlerFDListenerAdapterImpl*)data;
    int32_t fileDescriptor = fileDescriptorListener->GetFileDescriptor();
    auto listener = fileDescriptorListener->GetFDListener();
    listener->OnReadable(fileDescriptor);
}

EventHandlerFDListenerAdapterImpl::EventHandlerFDListenerAdapterImpl(
    const std::shared_ptr<EventHandlerFDListenerAdapter>& listener, int32_t fileDescriptor)
    : listener_(listener), fileDescriptor_(fileDescriptor)
{}

EventHandlerAdapterImpl::~EventHandlerAdapterImpl()
{
    if (!fdListenerMap_.empty()) {
        for (auto it = fdListenerMap_.begin(); it != fdListenerMap_.end(); ++it) {
            delete it->second;
            it = fdListenerMap_.erase(it);
        }
    }
    if (ffrt_loop_destroy(loop_) != ffrt_success) {
        WVLOG_E("loop destroy failed");
    }
}

EventHandlerAdapterImpl::EventHandlerAdapterImpl()
{
    auto mainQueue = ffrt_get_main_queue();
    loop_ = ffrt_loop_create(mainQueue);
    if (loop_ == nullptr) {
        WVLOG_E("create loop failed");
    }
}

bool EventHandlerAdapterImpl::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const std::shared_ptr<EventHandlerFDListenerAdapter> listener)
{
    WVLOG_D("[event handler] add fileDescriptorListener");
    
    if (!listener) {
        WVLOG_E("the listener is nullptr");
        return false;
    }
    auto it = fdListenerMap_.find(fileDescriptor);
    if (it != fdListenerMap_.end()) {
        WVLOG_E("fileDescriptorListener already exit which fileDescriptor=%{public}d", fileDescriptor);
        return false;
    }
    uint32_t ffrtEvents = events | EPOLLERR | EPOLLHUP;
    EventHandlerFDListenerAdapterImpl *fileDescriptorListener =
                                      new EventHandlerFDListenerAdapterImpl(listener, fileDescriptor);
    fdListenerMap_[fileDescriptor] = fileDescriptorListener;
    auto ret = ffrt_loop_epoll_ctl(loop_, EPOLL_CTL_ADD, fileDescriptor, ffrtEvents,
                                   fileDescriptorListener, FfrtCallback);
    if (ret != ffrt_success) {
        WVLOG_E("add file descriptor listener failed");
    }

    return ret == ffrt_success;
}

void EventHandlerAdapterImpl::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    WVLOG_D("[event handler] remove fileDescriptorListener");

    auto ret = ffrt_loop_epoll_ctl(loop_, EPOLL_CTL_DEL, fileDescriptor, 0, nullptr, nullptr);
    if (ret != ffrt_success) {
        WVLOG_E("remove file descriptor listener failed");
    }

    auto it = fdListenerMap_.find(fileDescriptor);
    if (it != fdListenerMap_.end()) {
        delete it->second;
        fdListenerMap_.erase(it);
    }
}
} // namespace OHOS::NWeb
