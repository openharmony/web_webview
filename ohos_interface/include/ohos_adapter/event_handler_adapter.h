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

#ifndef EVENT_HANDLER_ADAPTER_H
#define EVENT_HANDLER_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {

class EventHandlerFDListenerAdapter {
public:
    EventHandlerFDListenerAdapter() = default;

    virtual ~EventHandlerFDListenerAdapter() = default;

    virtual void OnReadable(int32_t fileDescriptor) = 0;
};

class OnceCallbackAdapter {
public:
    OnceCallbackAdapter() = default;

    virtual ~OnceCallbackAdapter() = default;

    virtual void OnRunnable() = 0;
};

class EventHandlerAdapter {
public:
    EventHandlerAdapter() = default;

    virtual ~EventHandlerAdapter() = default;

    virtual bool AddFileDescriptorListener(
        int32_t fileDescriptor, uint32_t events, const std::shared_ptr<EventHandlerFDListenerAdapter> listener) = 0;

    virtual void RemoveFileDescriptorListener(int32_t fileDescriptor) = 0;

    virtual void PostTask(const std::shared_ptr<OnceCallbackAdapter> callback) {};

    // be consistent with
    // rom/base/notification/eventhandler/interfaces/inner_api/file_descriptor_listener.h
    // FILE_DESCRIPTOR_INPUT_EVENT
    enum {
        INPUT_EVENT = 1,
    };
};

} // namespace OHOS::NWeb

#endif // EVENT_HANDLER_ADAPTER_H
