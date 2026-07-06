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

#ifndef ARK_MEDIA_SOURCE_DATA_HANDLER_WRAPPER_H
#define ARK_MEDIA_SOURCE_DATA_HANDLER_WRAPPER_H
#pragma once

#include "media_adapter.h"
#include "ohos_adapter/include/ark_media_adapter.h"

namespace OHOS::ArkWeb {

// HLS proxy download data handler, webview side (mirror of
// ArkPlayerCallbackAdapterWrapper). Implements the NWeb::MediaSourceDataHandler
// interface the player framework holds, forwarding each call across the bridge
// to the ArkWeb ArkMediaSourceDataHandler (the Chromium downloader).
class ArkMediaSourceDataHandlerWrapper : public OHOS::NWeb::MediaSourceDataHandler {
public:
    explicit ArkMediaSourceDataHandlerWrapper(ArkWebRefPtr<ArkMediaSourceDataHandler>);

    int64_t HandleDataOpen(const std::string& url,
        const std::map<std::string, std::string>& header) override;

    void HandleDataRead(int64_t uuid, int64_t offset, int64_t length) override;

    void HandleDataClose(int64_t uuid) override;

private:
    ArkWebRefPtr<ArkMediaSourceDataHandler> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_SOURCE_DATA_HANDLER_WRAPPER_H
