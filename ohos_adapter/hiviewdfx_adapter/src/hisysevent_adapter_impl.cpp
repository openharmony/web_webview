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

#include "hisysevent_adapter_impl.h"

#ifdef WEBVIWE_ONLY
#include "ability_manager_client.h"
#include "application_context.h"
#include "hisysevent.h"
#else
#include "nweb_log.h"
#endif

namespace OHOS::NWeb {
#ifdef WEBVIWE_ONLY
namespace {
const HiviewDFX::HiSysEvent::EventType EVENT_TYPES[] = {
    OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
    OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
    OHOS::HiviewDFX::HiSysEvent::EventType::SECURITY,
    OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
};
}

static std::string g_currentBundleName = "";
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
    std::tuple<const std::string, const std::string> sysData("BUNDLE_NAME", g_currentBundleName.c_str());
    auto mergeData = std::tuple_cat(sysData, tp);

    return std::apply(
        [&](auto&&... args) {
            return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::WEBVIEW, eventName, EVENT_TYPES[type], args...);
        },
        mergeData);
}
#endif

HiSysEventAdapterImpl& HiSysEventAdapterImpl::GetInstance()
{
    static HiSysEventAdapterImpl instance;
    return instance;
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

using systemData = std::tuple<const std::string, const int, const std::string, const std::string,
                              const std::string, const std::string>;

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int,
    const std::string, const std::vector<uint16_t>, const std::string, const int>& data)
{
#ifdef WEBVIWE_ONLY
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();

    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    systemData sysData = {
        "VERSION_CODE", appInfo->versionCode,
        "VERSION_NAME", appInfo->versionName.c_str(),
        "ABILITY_NAME", elementName.GetAbilityName()
    };

    auto mergeData = std::tuple_cat(data, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const int64_t, const std::string, const int64_t,
    const std::string, const int, const std::string, const int,
    const std::string, const int64_t, const std::string, const int>& data)
{
#ifdef WEBVIWE_ONLY
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
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
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
#ifdef WEBVIWE_ONLY
    auto appInfo = AbilityRuntime::ApplicationContext::GetInstance()->GetApplicationInfo();

    AppExecFwk::ElementName elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();

    std::tuple<const std::string, const std::string> sysData = {
        "ABILITY_NAME", elementName.GetAbilityName(),
    };

    auto mergeData = std::tuple_cat(data, sysData);
    return ForwardToHiSysEvent(eventName, type, mergeData);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return ForwardToHiSysEvent(eventName, type, data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}

int HiSysEventAdapterImpl::Write(const std::string& eventName, EventType type,
    const std::tuple<const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string,
                     const std::string, const std::string, const std::string, const std::string>& data)
{
#ifdef WEBVIWE_ONLY
    return std::apply(
        [&](auto&&... args) {
            return HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::WEBVIEW, eventName, EVENT_TYPES[type], args...);
        },
        data);
#else
    PRINT_NOT_IMPL_FUNC_LOG();
    return 0;
#endif
}
} // namespace OHOS::NWeb
