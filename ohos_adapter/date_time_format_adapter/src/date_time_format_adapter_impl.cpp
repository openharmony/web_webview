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

#include "date_time_format_adapter_impl.h"

#include <cstddef>
#include <string>

#include <BasicServicesKit/oh_commonevent.h>
#include <BasicServicesKit/oh_commonevent_support.h>
#include <BasicServicesKit/time_service.h>

#include "nweb_log.h"

namespace OHOS::NWeb {

constexpr int MAX_TIMEZONE_LEN = 32;

std::string WebTimezoneInfoImpl::GetTzId()
{
    return tzId_;
}

bool WebTimezoneInfoImpl::GetIsValid()
{
    return isValid_;
}

std::map<const std::shared_ptr<TimezoneEventCallbackAdapter>, EventCallbackState> DateTimeFormatAdapterImpl::cbMap_;
CommonEvent_SubscribeInfo *DateTimeFormatAdapterImpl::commonEventSubscribeInfo_ = NULL;
CommonEvent_Subscriber *DateTimeFormatAdapterImpl::commonEventSubscriber_ = NULL;
std::mutex DateTimeFormatAdapterImpl::mutex_;

void DateTimeFormatAdapterImpl::RegTimezoneEvent(std::shared_ptr<TimezoneEventCallbackAdapter> eventCallback)
{
    WVLOG_I("Reg Timezone Event.");
    cb_ = std::move(eventCallback);
    std::lock_guard<std::mutex> lock(mutex_);
    if (cbMap_.find(cb_) == cbMap_.end()) {
        cbMap_[cb_] = INACTIVE;
    }
}

void DateTimeFormatAdapterImpl::DateTimeFormatReceiveCallback(const CommonEvent_RcvData *data)
{
    if (data == nullptr || strcmp(OH_CommonEvent_GetEventFromRcvData(data), COMMON_EVENT_TIMEZONE_CHANGED)) {
        return;
    }

    WVLOG_I("receive timezone action: %{public}s", OH_CommonEvent_GetEventFromRcvData(data));
    char timeZone[MAX_TIMEZONE_LEN] = {0};
    TimeService_ErrCode ret = OH_TimeService_GetTimeZone(timeZone, sizeof(timeZone));
    if (ret != TIMESERVICE_ERR_OK) {
        WVLOG_E("get timezone error: %{public}d", ret);
        return;
    }

    std::shared_ptr<WebTimezoneInfoImpl> timeZoneInfo = 
        std::make_shared<WebTimezoneInfoImpl>(std::string(timeZone), true);

    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &i : cbMap_) {
        if (i.second == ACTIVE) {
            i.first->TimezoneChanged(timeZoneInfo);
        }
    }
}

bool DateTimeFormatAdapterImpl::StartListen()
{
    WVLOG_I("start time_zone listen.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (commonEventSubscriber_ == NULL) {
        const char *events[] = {
            COMMON_EVENT_TIMEZONE_CHANGED,
        };

        int count = sizeof(events) / sizeof(events[0]);
        commonEventSubscribeInfo_ = OH_CommonEvent_CreateSubscribeInfo(events, count);
        if (commonEventSubscribeInfo_ == NULL) {
            WVLOG_E("Create SubscribeInfo failed.");
            return false;
        }

        commonEventSubscriber_ = OH_CommonEvent_CreateSubscriber(commonEventSubscribeInfo_,
            &DateTimeFormatAdapterImpl::DateTimeFormatReceiveCallback);
        if (commonEventSubscriber_ == NULL) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
            commonEventSubscribeInfo_ = NULL;
            WVLOG_E("Create Subscriber failed.");
            return false;
        }

        CommonEvent_ErrCode ret = OH_CommonEvent_Subscribe(commonEventSubscriber_);
        if (ret != COMMONEVENT_ERR_OK) {
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
            OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
            commonEventSubscribeInfo_ = NULL;
            commonEventSubscriber_ = NULL;
            WVLOG_E("Subscribe failed. ret = %{public}d", ret);
            return false;
        }
    }

    if (cbMap_.find(cb_) == cbMap_.end()) {
        WVLOG_E("start time_zone listen failed. Please RegTimezoneEvent first.");
        return false;
    }

    cbMap_[cb_] = ACTIVE;
    return true;
}

void DateTimeFormatAdapterImpl::StopListen()
{
    WVLOG_I("stop time_zone listen.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (cbMap_.find(cb_) == cbMap_.end()) {
        WVLOG_E("stop listen failed. Please RegTimezoneEvent first.");
        return;
    }

    cbMap_.erase(cb_);
}

std::string DateTimeFormatAdapterImpl::GetTimezone()
{
    char timeZone[MAX_TIMEZONE_LEN] = {0};

    TimeService_ErrCode ret = OH_TimeService_GetTimeZone(timeZone, sizeof(timeZone));
    if (ret != TIMESERVICE_ERR_OK) {
        WVLOG_I("get timezone error: %{public}d", ret);
    }

    return std::string(timeZone);
}
} // namespace OHOS::NWeb
