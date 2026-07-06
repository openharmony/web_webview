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

#include "ohos_adapter/bridge/ark_media_source_data_handler_impl.h"

namespace OHOS::ArkWeb {

ArkMediaSourceDataHandlerImpl::ArkMediaSourceDataHandlerImpl(
    std::shared_ptr<OHOS::NWeb::MediaSourceDataHandler> ref) : real_(ref) {}

int64_t ArkMediaSourceDataHandlerImpl::HandleDataOpen(const ArkWebString& url, const ArkWebStringMap& header)
{
    std::string surl = ArkWebStringStructToClass(url);
    std::map<std::string, std::string> sheader = ArkWebStringMapStructToClass(header);
    return real_->HandleDataOpen(surl, sheader);
}

void ArkMediaSourceDataHandlerImpl::HandleDataRead(int64_t uuid, int64_t offset, int64_t length)
{
    real_->HandleDataRead(uuid, offset, length);
}

void ArkMediaSourceDataHandlerImpl::HandleDataClose(int64_t uuid)
{
    real_->HandleDataClose(uuid);
}

} // namespace OHOS::ArkWeb
