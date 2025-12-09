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

#ifndef ARK_HISYSEVENT_ADAPTER_WRAPPER_H
#define ARK_HISYSEVENT_ADAPTER_WRAPPER_H
#pragma once

#include "hisysevent_adapter.h"
#include "ohos_adapter/include/ark_hisysevent_adapter.h"

namespace OHOS::ArkWeb {

using ArkEventType = OHOS::NWeb::HiSysEventAdapter::EventType;

class ArkHiSysEventAdapterWrapper : public OHOS::NWeb::HiSysEventAdapter {
public:
    explicit ArkHiSysEventAdapterWrapper(ArkWebRefPtr<ArkHiSysEventAdapter>);

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string>& data)
        override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const int64_t, const std::string, const int, const std::string,
            const std::vector<uint16_t>, const std::string, const int>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int, const std::string, const int, const std::string, const int64_t, const std::string, const int>&
            data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const uint32_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t, const std::string,
            const int64_t, const std::string, const int64_t, const std::string, const int64_t>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string, const std::string, const std::string, const std::string, const std::string,
            const std::string>& data) override;

    int Write(const std::string& eventName, ArkEventType type,
        const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>& data) override;

    int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int32_t, const std::string, const std::string,
                         const std::string, const std::string, const std::string, const std::string,
                         const std::string, const int32_t>& data) override;
private:
    ArkWebRefPtr<ArkHiSysEventAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb

#endif // HISYSEVENT_ADAPTER_H
