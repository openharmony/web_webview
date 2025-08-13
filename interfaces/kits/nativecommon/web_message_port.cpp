/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "web_message_port.h"
#include "../../../ohos_interface/ohos_glue/base/include/ark_web_errno.h"

namespace OHOS {
namespace NWeb {
namespace {
constexpr int INIT_ERROR = -2;
}
WebMessagePort::WebMessagePort(int32_t nwebId, std::string& port, bool isExtentionType)
    : nwebId_(nwebId), portHandle_(port), isExtentionType_(isExtentionType)
{}

ErrCode WebMessagePort::ClosePort()
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    nweb_ptr->ClosePort(portHandle_);
    portHandle_.clear();
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::PostPortMessage(std::shared_ptr<NWebMessage> data)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    if (portHandle_.empty()) {
        WVLOG_E("can't post message, message port already closed");
        return NWebError::CAN_NOT_POST_MESSAGE;
    }
    nweb_ptr->PostPortMessage(portHandle_, data);
    return NWebError::NO_ERROR;
}

ErrCode WebMessagePort::SetPortMessageCallback(
    std::shared_ptr<NWebMessageValueCallback> callback)
{
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        return INIT_ERROR;
    }

    if (portHandle_.empty()) {
        WVLOG_E("can't register message port callback event, message port already closed");
        return NWebError::CAN_NOT_REGISTER_MESSAGE_EVENT;
    }
    nweb_ptr->SetPortMessageCallback(portHandle_, callback);
    return NWebError::NO_ERROR;
}

std::string WebMessagePort::GetPortHandle() const
{
    return portHandle_;
}

} // namespace NWeb
} // namespace OHOS