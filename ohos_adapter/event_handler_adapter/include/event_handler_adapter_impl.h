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

#ifndef EVENT_HANDLER_ADAPTER_IMPL_H
#define EVENT_HANDLER_ADAPTER_IMPL_H

#include "event_handler_adapter.h"
#include <ffrt/loop.h>

namespace OHOS::NWeb {

static constexpr int32_t INVALID_FD = -1;

class EventHandlerFDListenerAdapterImpl {
public:
    explicit EventHandlerFDListenerAdapterImpl(const std::shared_ptr<EventHandlerFDListenerAdapter>& listener,
                                               int32_t fileDescriptor);

    ~EventHandlerFDListenerAdapterImpl() {}

    std::shared_ptr<EventHandlerFDListenerAdapter> GetFDListener() {return listener_;}
    int32_t GetFileDescriptor() {return fileDescriptor_;}

private:
    std::shared_ptr<EventHandlerFDListenerAdapter> listener_ = nullptr;
    int32_t fileDescriptor_ = INVALID_FD;
};

class EventHandlerAdapterImpl : public EventHandlerAdapter {
public:
    EventHandlerAdapterImpl();

    ~EventHandlerAdapterImpl() override;

    bool AddFileDescriptorListener(int32_t fileDescriptor, uint32_t events,
        const std::shared_ptr<EventHandlerFDListenerAdapter> listener) override;

    void RemoveFileDescriptorListener(int32_t fileDescriptor) override;

private:
    ffrt_loop_t loop_ = nullptr;
    std::unordered_map<int32_t, EventHandlerFDListenerAdapterImpl *> fdListenerMap_;
};
} // namespace OHOS::NWeb

#endif // EVENT_HANDLER_ADAPTER_IMPL_H
