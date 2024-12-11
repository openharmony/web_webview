/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_WEB_DOWNLOAD_ITEM_H
#define NWEB_WEB_DOWNLOAD_ITEM_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "nweb_c_api.h"

namespace OHOS {
namespace NWeb {
class WebDownloadItem {
public:
    explicit WebDownloadItem(napi_env env);
    WebDownloadItem(napi_env env, NWebDownloadItem *downloadItem);
    ~WebDownloadItem();

    long webDownloadId;
    int32_t currentSpeed;
    int32_t percentComplete;
    int64_t totalBytes;
    int64_t receivedBytes;
    int32_t lastErrorCode;

    std::string guid;
    std::string fullPath;
    std::string url;
    std::string etag;
    std::string originalUrl;
    std::string suggestedFileName;
    std::string contentDisposition;
    std::string mimeType;
    std::string lastModified;

    NWebDownloadItemState state = NWebDownloadItemState::MAX_DOWNLOAD_STATE;
    std::string method;
    std::string receivedSlices;

    std::string downloadPath;
    WebBeforeDownloadCallbackWrapper *before_download_callback;
    WebDownloadItemCallbackWrapper *download_item_callback;

    int32_t nwebId;
    bool hasStarted{false};

private:
    napi_env env_;
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEB_DOWNLOAD_ITEM_H
