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

#ifndef ARK_MEDIA_SOURCE_DATA_HANDLER_IMPL_H
#define ARK_MEDIA_SOURCE_DATA_HANDLER_IMPL_H
#pragma once

#include "media_adapter.h"
#include "ohos_adapter/include/ark_media_adapter.h"

namespace OHOS::ArkWeb {

// HLS proxy download data handler, webcore side (mirror of
// ArkPlayerCallbackAdapterImpl). Wraps a NWeb::MediaSourceDataHandler (the
// Chromium downloader) to present it as the ArkWeb ArkMediaSourceDataHandler
// interface across the bridge.
class ArkMediaSourceDataHandlerImpl : public ArkMediaSourceDataHandler {
public:
    explicit ArkMediaSourceDataHandlerImpl(std::shared_ptr<OHOS::NWeb::MediaSourceDataHandler>);

    int64_t HandleDataOpen(const ArkWebString& url, const ArkWebStringMap& header) override;

    void HandleDataRead(int64_t uuid, int64_t offset, int64_t length) override;

    void HandleDataClose(int64_t uuid) override;

private:
    std::shared_ptr<OHOS::NWeb::MediaSourceDataHandler> real_;

    IMPLEMENT_REFCOUNTING(ArkMediaSourceDataHandlerImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_SOURCE_DATA_HANDLER_IMPL_H
