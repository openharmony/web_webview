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


namespace OHOS::NWeb {
namespace {
const HiviewDFX::HiSysEvent::EventType EVENT_TYPES[] = {
    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
    OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
    OHOS::HiviewDFX::HiSysEvent::EventType::SECURITY,
    OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
};
}

static std::string g_currentBundleName = "";
static std::string g_versionCode = "";
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
        auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();
        if (appInfo != nullptr) {
            g_currentBundleName = appInfo->bundleName.c_str();
        }
    }

    if (g_versionCode.empty()) {
        g_versionCode = OhosResourceAdapterImpl::GetArkWebVersion();
    }
    std::tuple<const std::string, const std::string, const std::string, const std::string> sysData(
        "BUNDLE_NAME", g_currentBundleName.c_str(),
        "VERSION_CODE", g_versionCode.c_str()
    );
    auto mergeData = std::tuple_cat(sysData, tp);

    return std::apply(
        [&](auto&&... args) {
            return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::WEBVIEW, eventName, EVENT_TYPES[type], args...);
        },
        mergeData);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string>& data)
{
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
    std::string versionCode = OhosResourceAdapterImpl::GetArkWebVersion();
    auto extendedData = std::tuple_cat(
        std::make_tuple("VERSION_CODE", versionCode),
        data);

    return std::apply(
        [&](auto&&... args) {
            return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::WEBVIEW, eventName, EVENT_TYPES[type], args...);
        },
        extendedData);
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const uint32_t, const std::string, const uint64_t>& data)
{
    return ForwardToHiSysEvent(eventName, type, data);
}
} // namespace OHOS::NWeb
