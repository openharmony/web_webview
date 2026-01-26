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

#include "ability_manager_client.h"
#include "application_context.h"
#include "hisysevent.h"
#include "nweb_hisysevent.h"
#include "arkweb_utils.h"
#include "parameters.h"

namespace OHOS::NWeb {
namespace {
const HiviewDFX::HiSysEvent::EventType EVENT_TYPES[] = {
    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
    OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
    OHOS::HiviewDFX::HiSysEvent::EventType::SECURITY,
    OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
};

enum EventType {
    FAULT = 0,
    STATISTIC,
    SECURITY,
    BEHAVIOR,
};
}

constexpr char INSTANCE_INITIALIZE_TIME[] = "INSTANCE_INITIALIZE_TIME";
constexpr char INSTANCE_ID[] = "INSTANCE_ID";
constexpr char USED_TIME[] = "USED_TIME";

static std::string g_webEngineType = "";
static std::string g_defaultWebEngineType = "";
template<typename... Args>
static int ForwardToHiSysEvent(const std::string& eventName, EventType type, const std::tuple<Args...>& tp)
{
    if (g_webEngineType.empty()) {
        g_webEngineType = std::to_string(static_cast<int>(OHOS::ArkWeb::getActiveWebEngineType()));
    }

    if (g_defaultWebEngineType.empty()) {
        g_defaultWebEngineType = std::to_string(OHOS::system::GetIntParameter("web.engine.default",
            static_cast<int>(OHOS::ArkWeb::ArkWebEngineType::EVERGREEN)));
    }

    auto sysData = std::make_tuple("BUNDLE_NAME", OHOS::ArkWeb::GetBundleName(),
                                   "API_COMPATIBLE_VERSION", OHOS::ArkWeb::GetApiVersion(),
                                   "WEB_ENGINE_TYPE", g_webEngineType,
                                   "DEFAULT_WEB_ENGINE_TYPE", g_defaultWebEngineType);
    auto mergeData = std::tuple_cat(sysData, tp);

    return std::apply(
        [&](auto&&... args) {
            return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::WEBVIEW, eventName, EVENT_TYPES[type], args...);
        },
        mergeData);
}

int EventReport::ReportCreateWebInstanceTime(uint32_t nwebId, int64_t usedTime)
{
    auto data = std::make_tuple(INSTANCE_ID, nwebId, USED_TIME, usedTime);
    return ForwardToHiSysEvent(INSTANCE_INITIALIZE_TIME, STATISTIC, data);
}
} // namespace OHOS::NWeb