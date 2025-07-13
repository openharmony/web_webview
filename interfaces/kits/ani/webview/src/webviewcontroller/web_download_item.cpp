/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "web_download_item.h"

#include <string>

#include "nweb_log.h"

namespace OHOS {
namespace NWeb {
WebDownloadItem::WebDownloadItem(ani_env* env)
    : guid(""),
      fullPath(""),
      url(""),
      etag(""),
      originalUrl(""),
      suggestedFileName(""),
      contentDisposition(""),
      mimeType(""),
      lastModified(""),
      method(""),
      receivedSlices(""),
      downloadPath(""),
      before_download_callback(nullptr),
      download_item_callback(nullptr)
{
    WVLOG_D("[DOWNLOAD] WebDownloadItem::constructor");
    this->env_ = env;
    this->currentSpeed = 0;
    this->percentComplete = 0;
    this->totalBytes = 0;
    this->receivedBytes = 0;
    this->lastErrorCode = 0;
    this->webDownloadId = 0;
    this->nwebId = 0;
}

WebDownloadItem::WebDownloadItem(ani_env* env, NWebDownloadItem *downloadItem)
    : guid(""),
      fullPath(""),
      url(""),
      etag(""),
      originalUrl(""),
      suggestedFileName(""),
      contentDisposition(""),
      mimeType(""),
      lastModified(""),
      method(""),
      receivedSlices(""),
      downloadPath(""),
      before_download_callback(nullptr),
      download_item_callback(nullptr)
{
    WVLOG_D("[DOWNLOAD] WebDownloadItem constructor");
    this->webDownloadId = WebDownloadItem_GetDownloadItemId(downloadItem);
    this->state = WebDownloadItem_GetState(downloadItem);
    this->currentSpeed = WebDownloadItem_CurrentSpeed(downloadItem);
    this->percentComplete = WebDownloadItem_PercentComplete(downloadItem);
    this->totalBytes = WebDownloadItem_TotalBytes(downloadItem);
    this->receivedBytes = WebDownloadItem_ReceivedBytes(downloadItem);
    this->guid = std::string(WebDownloadItem_Guid(downloadItem));
    this->fullPath = std::string(WebDownloadItem_FullPath(downloadItem));
    this->url = std::string(WebDownloadItem_Url(downloadItem));
    this->originalUrl = std::string(WebDownloadItem_OriginalUrl(downloadItem));
    this->suggestedFileName = std::string(WebDownloadItem_SuggestedFileName(downloadItem));
    this->contentDisposition = std::string(WebDownloadItem_ContentDisposition(downloadItem));
    this->method = std::string(WebDownloadItem_Method(downloadItem));
    this->lastModified = std::string(WebDownloadItem_LastModified(downloadItem));
    this->lastErrorCode = WebDownloadItem_LastErrorCode(downloadItem);
    this->receivedSlices = std::string(WebDownloadItem_ReceivedSlices(downloadItem));
    this->etag = std::string(WebDownloadItem_ETag(downloadItem));
    this->mimeType = std::string(WebDownloadItem_MimeType(downloadItem));
    this->nwebId = WebDownloadItem_NWebId(downloadItem);
    env_ = env;
}

WebDownloadItem::~WebDownloadItem()
{
    WVLOG_D("[DOWNLOAD] WebDownloadItem::~WebDownloadItem()");
    if (before_download_callback) {
        DestroyBeforeDownloadCallbackWrapper(before_download_callback);
        before_download_callback = nullptr;
    }
    if (download_item_callback) {
        DestroyDownloadItemCallbackWrapper(download_item_callback);
        download_item_callback = nullptr;
    }
}
} // namespace NWeb
} // namespace OHOS
