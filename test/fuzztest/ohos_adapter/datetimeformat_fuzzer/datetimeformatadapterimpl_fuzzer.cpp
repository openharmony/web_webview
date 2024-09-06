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

#include "datetimeformatadapterimpl_fuzzer.h"

#define private public
#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "date_time_format_adapter_impl.h"
#include "time_service_client.h"

using namespace OHOS::NWeb;

namespace OHOS {
namespace {
bool g_commonEvent = false;
bool g_unCommonEvent = false;
using Want = OHOS::AAFwk::Want;
} // namespace
namespace EventFwk {
bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber>& subscriber)
{
    return g_commonEvent;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber>& subscriber)
{
    return g_unCommonEvent;
}
} // namespace EventFwk
namespace NWeb {
class MockTimezoneEventCallbackAdapter : public TimezoneEventCallbackAdapter {
public:
    MockTimezoneEventCallbackAdapter() = default;
    void TimezoneChanged(std::shared_ptr<WebTimezoneInfo> info) {}
};

bool DateTimeFormatAdapterFuzzTest(const uint8_t* data, size_t size)
{
    EventFwk::CommonEventSubscribeInfo in;
    std::shared_ptr<TimezoneEventCallbackAdapter> cb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    auto adapter = std::make_shared<NWebTimeZoneEventSubscriber>(in, cb);
    Want want;
    want.SetAction("test");
    EventFwk::CommonEventData newdata(want);
    adapter->OnReceiveEvent(newdata);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_TIMEZONE_CHANGED);
    newdata.SetWant(want);
    adapter->OnReceiveEvent(newdata);
    adapter->eventCallback_ = nullptr;
    adapter->OnReceiveEvent(newdata);

    auto newadapter = std::make_shared<DateTimeFormatAdapterImpl>();
    std::shared_ptr<TimezoneEventCallbackAdapter> newcb = std::make_shared<MockTimezoneEventCallbackAdapter>();
    newadapter->RegTimezoneEvent(std::move(newcb));
    bool newresult = newadapter->StartListen();
    g_commonEvent = true;
    newresult = newadapter->StartListen();
    newadapter->StopListen();
    g_unCommonEvent = true;
    newadapter->StopListen();
    newadapter->StopListen();
    std::string timeZoneNicosia("");
    OHOS::MiscServices::TimeServiceClient::GetInstance()->SetTimeZone(timeZoneNicosia);
    auto timeStr = newadapter->GetTimezone();
    return true;
}
} // namespace NWeb
/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NWeb::DateTimeFormatAdapterFuzzTest(data, size);
    return 0;
}
} // namespace OHOS