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

#ifndef OHOS_NWEB_HISYSEVENT_H
#define OHOS_NWEB_HISYSEVENT_H

#include <cstdint>
#include <string>

namespace OHOS::NWeb {
class EventReport {
public:
    ~EventReport() = default;

    static int ReportCreateWebInstanceTime(uint32_t nwebId, int64_t usedTime);
    
    static int ReportHighlightSpecifiedContentEvent(
    const std::string& eventType,
    const std::string& errorType,
    const std::string& content,
    const std::string& errorNode);

    static int ReportMSDPError(std::string eventType, std::string errorType, std::string errorInfo,
        std::string nodePath = "");
};
} // namespace OHOS::NWeb
#endif  // OHOS_NWEB_HISYSEVENT_H