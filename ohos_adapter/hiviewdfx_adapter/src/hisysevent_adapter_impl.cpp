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
#include "hisysevent_adapter_impl.h"
#include "hisysevent.h"
#include "ohos_resource_adapter_impl.h"
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
}

const int kBase10 = 10;
bool ConvertToLongLong(const std::string& str, long long& value)
{
    char* end;
    errno = 0;
    value = std::strtoll(str.c_str(), &end, kBase10);
    if (end == str.c_str()) {
        return false;
    }
    if (errno == ERANGE && (value == LLONG_MAX || value == LLONG_MIN)) {
        return false;
    }
    if (*end != '\0') {
        return false;
    }
    return true;
}

int64_t GetValueInt64(const std::string& input, const std::string& key1, const std::string& key2)
{
    long long result = 0;
    std::string key = "";
    if(key2 == key) {
        std::string::size_type keyPosition1 = input.find(key1, 0);
        if (keyPosition1 != std::string::npos) {
            std::string waitConvertString = input.substr(keyPosition1 + key1.size());
            if (ConvertToLongLong(waitConvertString, result) == true) {
                return result;
            }
        }
        return -1;
    }
    std::string::size_type keyPosition1 = input.find(key1, 0);
    std::string::size_type keyPosition2 = input.find(key2, 0);
    if (keyPosition1 != std::string::npos && keyPosition2 != std::string::npos &&
        keyPosition1 + key1.size() < keyPosition2) {
        std::string waitConvertString = input.substr(keyPosition1 + key1.size(),
            keyPosition2 - keyPosition1 - key1.size());
        if (ConvertToLongLong(waitConvertString, result) == true) {
            return result;
        }
    }
    return -1;
}

const static std::string PAGE_LOAD_KEY_LISTS[] = {
    "NAVIGATION_ID",
    "NAVIGATION_START",
    "REDIRECT_COUNT",
    "REDIRECT_START",
    "REDIRECT_END",
    "FETCH_START",
    "WORKER_START",
    "DOMAIN_LOOKUP_START",
    "DOMAIN_LOOKUP_END",
    "CONNECT_START",
    "SECURE_CONNECT_START",
    "CONNECT_END",
    "REQUEST_START",
    "RESPONSE_START",
    "RESPONSE_END",
    "DOM_INTERACTIVE",
    "DOM_CONTENT_LOADED_EVENT_START",
    "DOM_CONTENT_LOADED_EVENT_END",
    "LOAD_EVENT_START",
    "LOAD_EVENT_END",
    "FIRST_PAINT",
    "FIRST_CONTENTFUL_PAINT",
    "LARGEST_CONTENTFUL_PAINT",
    "RENDER_INIT_BLOCK",
    "INPUT_TIME",
    "IS_PAINT_DONE",
    "FIRST_MEANINGFUL_PAINT"
};
static std::string g_currentBundleName = "";
static std::string g_versionCode = "";
static std::string g_apiCompatibleVersion = "";
static std::string g_appVersion = "";
static std::string g_webEngineType = "";
static std::string g_defaultWebEngineType = "";
HiSysEventAdapterImpl& HiSysEventAdapterImpl::GetInstance()
{
    static HiSysEventAdapterImpl instance;
    return instance;
}

template<typename... Args>
static int ForwardToHiSysEvent(const std::string& eventName, HiSysEventAdapter::EventType type,
    const std::tuple<Args...>& tp)
{
    if (g_currentBundleName.empty()) {
        g_currentBundleName = OHOS::ArkWeb::GetBundleName();
    }

    if (g_apiCompatibleVersion.empty()) {
        g_apiCompatibleVersion = OHOS::ArkWeb::GetApiVersion();
    }

    if (g_appVersion.empty()) {
        g_appVersion = OHOS::ArkWeb::GetAppVersion();
    }

    if (g_versionCode.empty()) {
        g_versionCode = OhosResourceAdapterImpl::GetArkWebVersion();
    }

    if (g_webEngineType.empty()) {
        g_webEngineType = std::to_string(static_cast<int>(OHOS::ArkWeb::getActiveWebEngineType()));
    }

    if (g_defaultWebEngineType.empty()) {
        g_defaultWebEngineType = std::to_string(OHOS::system::GetIntParameter("web.engine.default",
            static_cast<int>(OHOS::ArkWeb::ArkWebEngineType::EVERGREEN)));
    }

    auto sysData = std::make_tuple("BUNDLE_NAME", g_currentBundleName,
                                   "VERSION_CODE", g_versionCode,
                                   "API_COMPATIBLE_VERSION", g_apiCompatibleVersion,
                                   "WEB_ENGINE_TYPE", g_webEngineType,
                                   "DEFAULT_WEB_ENGINE_TYPE", g_defaultWebEngineType);
    auto mergeData = std::tuple_cat(sysData, tp);

    if (type == HiSysEventAdapter::EventType::BEHAVIOR) {
        auto ueData = std::make_tuple("PNAMEID", g_currentBundleName, "PVERSIONID", g_appVersion);
        return std::apply(
            [&](auto&&... args) {
                return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::ARKWEB_UE,
                                       eventName, EVENT_TYPES[type], args...);
            },
            std::tuple_cat(ueData, mergeData));
    } else {
        return std::apply(
            [&](auto&&... args) {
                return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::WEBVIEW,
                                       eventName, EVENT_TYPES[type], args...);
            },
            mergeData);
    }
}

int ProcessEventPageLoadTime(const std::string& eventName, HiSysEventAdapter::EventType type,
    const std::tuple<const std::string, const std::string>& data)
{
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    if (appInfo == nullptr) {
        return -1;
    }
    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    std::tuple<const std::string, const std::string> sysData = {
        "ABILITY_NAME", elementName.GetAbilityName(),
    };

    const std::string input = std::get<0>(data);

    const std::int64_t value1 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[0], PAGE_LOAD_KEY_LISTS[1]);
    const std::int64_t value2 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[1], PAGE_LOAD_KEY_LISTS[2]);
    const std::uint32_t value3 = (std::uint32_t)GetValueInt64(input, PAGE_LOAD_KEY_LISTS[2], PAGE_LOAD_KEY_LISTS[3]);
    const std::int64_t value4 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[3], PAGE_LOAD_KEY_LISTS[4]);
    const std::int64_t value5 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[4], PAGE_LOAD_KEY_LISTS[5]);
    const std::int64_t value6 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[5], PAGE_LOAD_KEY_LISTS[6]);
    const std::int64_t value7 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[6], PAGE_LOAD_KEY_LISTS[7]);
    const std::int64_t value8 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[7], PAGE_LOAD_KEY_LISTS[8]);
    const std::int64_t value9 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[8], PAGE_LOAD_KEY_LISTS[9]);
    const std::int64_t value10 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[9], PAGE_LOAD_KEY_LISTS[10]);
    const std::int64_t value11 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[10], PAGE_LOAD_KEY_LISTS[11]);
    const std::int64_t value12 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[11], PAGE_LOAD_KEY_LISTS[12]);
    const std::int64_t value13 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[12], PAGE_LOAD_KEY_LISTS[13]);
    const std::int64_t value14 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[13], PAGE_LOAD_KEY_LISTS[14]);
    const std::int64_t value15 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[14], PAGE_LOAD_KEY_LISTS[15]);
    const std::int64_t value16 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[15], PAGE_LOAD_KEY_LISTS[16]);
    const std::int64_t value17 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[16], PAGE_LOAD_KEY_LISTS[17]);
    const std::int64_t value18 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[17], PAGE_LOAD_KEY_LISTS[18]);
    const std::int64_t value19 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[18], PAGE_LOAD_KEY_LISTS[19]);
    const std::int64_t value20 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[19], PAGE_LOAD_KEY_LISTS[20]);
    const std::int64_t value21 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[20], PAGE_LOAD_KEY_LISTS[21]);
    const std::int64_t value22 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[21], PAGE_LOAD_KEY_LISTS[22]);
    const std::int64_t value23 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[22], PAGE_LOAD_KEY_LISTS[23]);
    const bool value24 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[23], "");

    auto newData = std::make_tuple(
        PAGE_LOAD_KEY_LISTS[0], value1, PAGE_LOAD_KEY_LISTS[1], value2,
        PAGE_LOAD_KEY_LISTS[2], value3, PAGE_LOAD_KEY_LISTS[3], value4,
        PAGE_LOAD_KEY_LISTS[4], value5, PAGE_LOAD_KEY_LISTS[5], value6,
        PAGE_LOAD_KEY_LISTS[6], value7, PAGE_LOAD_KEY_LISTS[7], value8,
        PAGE_LOAD_KEY_LISTS[8], value9, PAGE_LOAD_KEY_LISTS[9], value10,
        PAGE_LOAD_KEY_LISTS[10], value11, PAGE_LOAD_KEY_LISTS[11], value12,
        PAGE_LOAD_KEY_LISTS[12], value13, PAGE_LOAD_KEY_LISTS[13], value14,
        PAGE_LOAD_KEY_LISTS[14], value15, PAGE_LOAD_KEY_LISTS[15], value16,
        PAGE_LOAD_KEY_LISTS[16], value17, PAGE_LOAD_KEY_LISTS[17], value18,
        PAGE_LOAD_KEY_LISTS[18], value19, PAGE_LOAD_KEY_LISTS[19], value20,
        PAGE_LOAD_KEY_LISTS[20], value21, PAGE_LOAD_KEY_LISTS[21], value22,
        PAGE_LOAD_KEY_LISTS[22], value23, PAGE_LOAD_KEY_LISTS[23], value24);
        
    auto mergeData = std::tuple_cat(newData, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
}

int ProcessEventFirstMeaningfulPaintDone(const std::string& eventName, HiSysEventAdapter::EventType type,
    const std::tuple<const std::string, const std::string>& data)
{
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    if (appInfo == nullptr) {
        return -1;
    }
    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    std::tuple<const std::string, const std::string> sysData = {
        "ABILITY_NAME", elementName.GetAbilityName(),
    };

    const std::string input = std::get<0>(data);

    const std::int64_t value1 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[0], PAGE_LOAD_KEY_LISTS[1]);
    const std::int64_t value2 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[1], PAGE_LOAD_KEY_LISTS[2]);
    const std::uint32_t value3 = (std::uint32_t)GetValueInt64(input, PAGE_LOAD_KEY_LISTS[2], PAGE_LOAD_KEY_LISTS[24]);
    const std::int64_t value4 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[24], PAGE_LOAD_KEY_LISTS[20]);
    const std::int64_t value5 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[20], PAGE_LOAD_KEY_LISTS[21]);
    const std::int64_t value6 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[21], PAGE_LOAD_KEY_LISTS[26]);
    const std::int64_t value7 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[26], PAGE_LOAD_KEY_LISTS[25]);
    const std::int64_t value8 = GetValueInt64(input, PAGE_LOAD_KEY_LISTS[25], "");

    auto newData = std::make_tuple(
        PAGE_LOAD_KEY_LISTS[0], value1, PAGE_LOAD_KEY_LISTS[1], value2,
        PAGE_LOAD_KEY_LISTS[2], value3, PAGE_LOAD_KEY_LISTS[24], value4,
        PAGE_LOAD_KEY_LISTS[20], value5, PAGE_LOAD_KEY_LISTS[21], value6,
        PAGE_LOAD_KEY_LISTS[26], value7, PAGE_LOAD_KEY_LISTS[25], value8);
        
    auto mergeData = std::tuple_cat(newData, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string>& data)
{
    const std::string eventNamePageLoadTime = "PAGE_LOAD_TIME";
    const std::string eventNameFirstMeaningfulPaintDone = "FIRST_MEANINGFUL_PAINT_DONE";
    if (eventName == eventNamePageLoadTime) {
        return ProcessEventPageLoadTime(eventName, type, data);
    }
    if (eventName == eventNameFirstMeaningfulPaintDone) {
        int result = ProcessEventFirstMeaningfulPaintDone(eventName, type, data);
        return result;
    }
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

using systemData = std::tuple<const std::string, const int, const std::string, const std::string,
                              const std::string, const std::string>;

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int,
    const std::string, const std::vector<uint16_t>, const std::string, const int>& data)
{
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    if (appInfo == nullptr) {
        return -1;
    }
    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    systemData sysData = {
        "VERSION_CODE", appInfo->versionCode,
        "VERSION_NAME", appInfo->versionName.c_str(),
        "ABILITY_NAME", elementName.GetAbilityName()
    };

    auto mergeData = std::tuple_cat(data, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int, const std::string, const int,
    const std::string, const int64_t, const std::string, const int>& data)
{
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    if (appInfo == nullptr) {
        return -1;
    }
    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    std::tuple<const std::string, const std::string, const std::string, const std::string,
        const std::string, const std::string> sysData = {
        "SCENE_ID", "WEB_LIST_FLING",
        "ABILITY_NAME", elementName.GetAbilityName(),
        "PAGE_URL", ""
    };

    auto mergeData = std::tuple_cat(data, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const uint32_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int64_t, const std::string, const int64_t>& data)
{
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
    if (appInfo == nullptr) {
        return -1;
    }
    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    std::tuple<const std::string, const std::string> sysData = {
        "ABILITY_NAME", elementName.GetAbilityName(),
    };

    auto mergeData = std::tuple_cat(data, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string>& data)
{
   return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int32_t, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const int32_t>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int32_t>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}
} // namespace OHOS::NWeb
