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

#ifndef HISYSEVENT_ADAPTER_H
#define HISYSEVENT_ADAPTER_H

#include <string>
#include <vector>

namespace OHOS::NWeb {

class HiSysEventAdapter {
public:
    enum EventType {
        FAULT = 0,
        STATISTIC,
        SECURITY,
        BEHAVIOR,
    };

    virtual int Write(
        const std::string& eventName, EventType type, const std::tuple<const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int64_t, const std::string, const int, const std::string,
            const std::vector<uint16_t>, const std::string, const int>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int, const std::string, const int, const std::string, const int64_t, const std::string, const int>&
            data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const uint32_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>& data) = 0;

    virtual int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int32_t, const std::string, const int32_t, const std::string,
        const std::string, const std::string, const std::string, const std::string, const std::string>& data) = 0;

protected:
    HiSysEventAdapter() = default;

    virtual ~HiSysEventAdapter() = default;
};

} // namespace OHOS::NWeb

#endif // HISYSEVENT_ADAPTER_H
