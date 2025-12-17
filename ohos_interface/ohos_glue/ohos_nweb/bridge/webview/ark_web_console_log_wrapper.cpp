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

#include "ohos_nweb/bridge/ark_web_console_log_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebConsoleLogWrapper::ArkWebConsoleLogWrapper(ArkWebRefPtr<ArkWebConsoleLog> ark_web_console_log)
    : ark_web_console_log_(ark_web_console_log)
{}

std::string ArkWebConsoleLogWrapper::Log()
{
    ArkWebString stLog = ark_web_console_log_->Log();

    std::string objLog = ArkWebStringStructToClass(stLog);
    ArkWebStringStructRelease(stLog);
    return objLog;
}

std::string ArkWebConsoleLogWrapper::SourceId()
{
    ArkWebString stSourceId = ark_web_console_log_->SourceId();

    std::string objSourceId = ArkWebStringStructToClass(stSourceId);
    ArkWebStringStructRelease(stSourceId);
    return objSourceId;
}

ArkWebLogLevel ArkWebConsoleLogWrapper::LogLevel()
{
    return static_cast<ArkWebLogLevel>(ark_web_console_log_->LogLevel());
}

int ArkWebConsoleLogWrapper::LineNumer()
{
    return ark_web_console_log_->LineNumer();
}

ArkWebLogSource ArkWebConsoleLogWrapper::Source()
{
    return static_cast<ArkWebLogSource>(ark_web_console_log_->Source());
}

} // namespace OHOS::ArkWeb
