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

#include "ohos_adapter/bridge/ark_media_source_data_handler_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaSourceDataHandlerWrapper::ArkMediaSourceDataHandlerWrapper(
    ArkWebRefPtr<ArkMediaSourceDataHandler> ref) : ctocpp_(ref) {}

int64_t ArkMediaSourceDataHandlerWrapper::HandleDataOpen(const std::string& url,
    const std::map<std::string, std::string>& header)
{
    ArkWebString surl = ArkWebStringClassToStruct(url);
    ArkWebStringMap sheader = ArkWebStringMapClassToStruct(header);
    int64_t result = ctocpp_->HandleDataOpen(surl, sheader);
    ArkWebStringStructRelease(surl);
    ArkWebStringMapStructRelease(sheader);
    return result;
}

void ArkMediaSourceDataHandlerWrapper::HandleDataRead(int64_t uuid, int64_t offset, int64_t length)
{
    ctocpp_->HandleDataRead(uuid, offset, length);
}

void ArkMediaSourceDataHandlerWrapper::HandleDataClose(int64_t uuid)
{
    ctocpp_->HandleDataClose(uuid);
}

} // namespace OHOS::ArkWeb
