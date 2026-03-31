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

#ifndef OHOS_NWEB_NWEB_C_API_H
#define OHOS_NWEB_NWEB_C_API_H

#include <stddef.h>
#include <stdint.h>
#include <memory>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define NWEB_EXPORT __attribute__((visibility("default")))

enum NWebDownloadItemState {
    IN_PROGRESS = 0,
    COMPLETE,
    CANCELED,
    INTERRUPTED,
    PENDING,
    PAUSED,
    MAX_DOWNLOAD_STATE,
};


typedef struct NWebDownloadDelegateCallback WebDownloadDelegateCallback;
typedef struct NWebBeforeDownloadCallbackWrapper WebBeforeDownloadCallbackWrapper;
typedef struct NWebDownloadItemCallbackWrapper WebDownloadItemCallbackWrapper;
typedef struct NWebDownloadItem NWebDownloadItem;

typedef void (*OnDownloadBeforeStart)(NWebDownloadItem *downloadItem, WebBeforeDownloadCallbackWrapper *wrapper);
typedef void (*OnDownloadDidUpdate)(NWebDownloadItem *downloadItem, WebDownloadItemCallbackWrapper *wrapper);

// / WebDownloader functions.
NWEB_EXPORT void WebDownloadManager_PutDownloadCallback(WebDownloadDelegateCallback *callback);

NWEB_EXPORT void WebDownloader_StartDownload(int32_t nwebId, const char* url);

NWEB_EXPORT void WebDownloader_SetDownloadBeforeStart(WebDownloadDelegateCallback *callback, OnDownloadBeforeStart fun);

NWEB_EXPORT void WebDownloader_SetDownloadDidUpdate(WebDownloadDelegateCallback *callback, OnDownloadDidUpdate fun);

NWEB_EXPORT void WebDownloader_ResumeDownloadStatic(const NWebDownloadItem *downloadItem);

NWEB_EXPORT void WebDownloader_CreateDownloadDelegateCallback(WebDownloadDelegateCallback **callback);

NWEB_EXPORT void WebDownload_Continue(const WebBeforeDownloadCallbackWrapper *wrapper, const char *downloadPath);

NWEB_EXPORT void WebDownload_CancelBeforeDownload(const WebBeforeDownloadCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_PauseBeforeDownload(const WebBeforeDownloadCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_ResumeBeforeDownload(const WebBeforeDownloadCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_Cancel(const WebDownloadItemCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_Pause(const WebDownloadItemCallbackWrapper *wrapper);

NWEB_EXPORT void WebDownload_Resume(const WebDownloadItemCallbackWrapper *wrapper);

NWEB_EXPORT NWebDownloadItemState WebDownload_GetItemState(int32_t nwebId, long downloadItemId);

NWEB_EXPORT NWebDownloadItemState WebDownload_GetItemStateByGuid(const std::string& guid);

NWEB_EXPORT NWebDownloadItemState WebDownload_GetItemStateByGuidV2(const char* guid);

NWEB_EXPORT void WebDownloadItem_CreateWebDownloadItem(NWebDownloadItem **downloadItem);

NWEB_EXPORT void WebDownloadItem_Destroy(NWebDownloadItem *downloadItem);

NWEB_EXPORT void WebDownloadItem_SetFullPath(NWebDownloadItem *downloadItem, const char *fullPath);

NWEB_EXPORT void WebDownloadItem_SetReceivedBytes(NWebDownloadItem *downloadItem, int64_t receivedBytes);

NWEB_EXPORT void WebDownloadItem_SetTotalBytes(NWebDownloadItem *downloadItem, int64_t totalBytes);

NWEB_EXPORT void WebDownloadItem_SetETag(NWebDownloadItem *downloadItem, const char *etag);

NWEB_EXPORT void WebDownloadItem_SetLastModified(NWebDownloadItem *downloadItem, const char *lastModified);

NWEB_EXPORT void WebDownloadItem_SetMimeType(NWebDownloadItem *downloadItem, const char *mimeType);

NWEB_EXPORT void WebDownloadItem_SetUrl(NWebDownloadItem *downloadItem, const char *url);

NWEB_EXPORT void WebDownloadItem_SetOriginalUrl(NWebDownloadItem *downloadItem, const char *originalUrl);

NWEB_EXPORT void WebDownloadItem_SetReferrerUrl(NWebDownloadItem *downloadItem, const char *referrerUrl);

NWEB_EXPORT void WebDownloadItem_SetUrlChain(NWebDownloadItem *downloadItem, char** urlChain, int64_t size);

NWEB_EXPORT void WebDownloadItem_SetReceivedSlices(NWebDownloadItem *downloadItem, const char *receivedSlices);

NWEB_EXPORT void WebDownloadItem_SetGuid(NWebDownloadItem *downloadItem, const char *guid);

NWEB_EXPORT char *WebDownloadItem_Guid(const NWebDownloadItem *downloadItem);

NWEB_EXPORT long WebDownloadItem_GetDownloadItemId(const NWebDownloadItem *downloadItem);

NWEB_EXPORT NWebDownloadItemState WebDownloadItem_GetState(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int WebDownloadItem_CurrentSpeed(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int WebDownloadItem_PercentComplete(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int64_t WebDownloadItem_TotalBytes(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int64_t WebDownloadItem_ReceivedBytes(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_FullPath(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_Url(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_OriginalUrl(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_ReferrerUrl(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char** WebDownloadItem_UrlChain(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int64_t WebDownloadItem_UrlChainSize(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_SuggestedFileName(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_ContentDisposition(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_ETag(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_MimeType(const NWebDownloadItem *downloadItem);

NWEB_EXPORT bool WebDownloadItem_IsPaused(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_Method(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int WebDownloadItem_LastErrorCode(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_ReceivedSlices(const NWebDownloadItem *downloadItem);

NWEB_EXPORT char *WebDownloadItem_LastModified(const NWebDownloadItem *downloadItem);

NWEB_EXPORT int WebDownloadItem_NWebId(const NWebDownloadItem *downloadItem);

NWEB_EXPORT void DestroyBeforeDownloadCallbackWrapper(WebBeforeDownloadCallbackWrapper *wrapper);

NWEB_EXPORT void DestroyDownloadItemCallbackWrapper(WebDownloadItemCallbackWrapper *wrapper);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // OHOS_NWEB_NWEB_C_API_H
