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

#include "location_callback_adapter_impl.h"

#include "location_proxy_adapter_impl.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
LocationCallbackImpl::LocationCallbackImpl(
    std::shared_ptr<LocationCallbackAdapter> adapter)
    : locationCallbackAdapter_(adapter) {}

int LocationCallbackImpl::OnRemoteRequest(uint32_t code,
    OHOS::MessageParcel& data,
    OHOS::MessageParcel& reply,
    OHOS::MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WVLOG_E("LocationCallbackImpl invalid token.");
        return -1;
    }
    if (locationCallbackAdapter_ == nullptr) {
        WVLOG_E("LocationCallbackImpl adapter is nullptr.");
        return -1;
    }
    switch (code) {
        case RECEIVE_LOCATION_INFO_EVENT: {
            std::unique_ptr<OHOS::Location::Location> location =
                OHOS::Location::Location::UnmarshallingMakeUnique(data);
            OnLocationReport(location);
            break;
        }
        case RECEIVE_ERROR_INFO_EVENT: {
            int32_t errorCode = data.ReadInt32();
            OnErrorReport(errorCode);
            break;
        }
        case RECEIVE_LOCATION_STATUS_EVENT: {
            int32_t status = data.ReadInt32();
            OnLocatingStatusChange(status);
            break;
        }
        default: {
            WVLOG_E("locationCallback receive error code:%{public}u", code);
            return -1;
        }
    }
    return 0;
}

void LocationCallbackImpl::OnLocationReport(
    const std::unique_ptr<OHOS::Location::Location>& location)
{
    if (location == nullptr) {
        WVLOG_E("OnLocationReport location is nullptr");
        return;
    }
    std::unique_ptr<OHOS::Location::Location> tempLocation =
        std::make_unique<OHOS::Location::Location>(*location);
    std::shared_ptr<LocationInfo> locationInfo =
        std::make_shared<LocationInfoImpl>(tempLocation);
    if (locationCallbackAdapter_ != nullptr) {
        locationCallbackAdapter_->OnLocationReport(locationInfo);
    }
}

void LocationCallbackImpl::OnLocatingStatusChange(const int status)
{
    if (locationCallbackAdapter_ != nullptr) {
        locationCallbackAdapter_->OnLocatingStatusChange(status);
    }
}

void LocationCallbackImpl::OnErrorReport(const int errorCode)
{
    if (locationCallbackAdapter_ != nullptr) {
        locationCallbackAdapter_->OnErrorReport(errorCode);
    }
}
}
