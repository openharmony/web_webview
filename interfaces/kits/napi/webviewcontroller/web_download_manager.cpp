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

#include "web_download_manager.h"

#include <cstring>

#include "nweb_c_api.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "web_download_delegate.h"

namespace OHOS {
namespace NWeb {
namespace {
static std::unique_ptr<OHOS::NWeb::WebDownloadDelegate> g_default_delegate;
static std::unordered_map<int32_t, WebDownloadDelegate *> g_web_download_delegate_map;
static WebDownloadDelegateCallback *g_download_callback;

WebDownloadDelegate *GetWebDownloadDelegate(int32_t nwebId)
{
    auto it = g_web_download_delegate_map.find(nwebId);
    if (it != g_web_download_delegate_map.end()) {
        return it->second;
    }
    return nullptr;
}

void DownloadBeforeStart(NWebDownloadItem *downloadItem, WebBeforeDownloadCallbackWrapper *wrapper)
{
    WVLOG_D("[DOWNLOAD] DownloadBeforeStart.");
    if (wrapper == nullptr) {
        WVLOG_E("[DOWNLOAD] WebBeforeDownloadCallbackWrapper is null");
        return;
    }
    int32_t nwebId = WebDownloadItem_NWebId(downloadItem);
    WebDownloadDelegate *webDownloadDelegate = GetWebDownloadDelegate(nwebId);
    if (!webDownloadDelegate) {
        WVLOG_D("[DOWNLOAD] donn't found delegate for nweb.");
        webDownloadDelegate = g_default_delegate.get();
    }

    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }

    WebDownloadItem *webDownloadItem = new WebDownloadItem(webDownloadDelegate->GetEnv(), downloadItem);
    // destroy download_item since copied content from download_item.
    WebDownloadItem_Destroy(downloadItem);
    webDownloadItem->before_download_callback = wrapper;
    webDownloadDelegate->DownloadBeforeStart(webDownloadItem);
}

void DownloadDidUpdate(NWebDownloadItem *downloadItem, WebDownloadItemCallbackWrapper *wrapper)
{
    WVLOG_I("DownloadDidUpdate.");
    if (wrapper == nullptr) {
        WVLOG_E("[DOWNLOAD] WebBeforeDownloadCallbackWrapper is null");
        return;
    }

    int32_t nwebId = WebDownloadItem_NWebId(downloadItem);

    WebDownloadDelegate *webDownloadDelegate = GetWebDownloadDelegate(nwebId);
    if (!webDownloadDelegate) {
        WVLOG_D("[DOWNLOAD] donn't found delegate for nweb.");
        webDownloadDelegate = g_default_delegate.get();
    }

    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    WebDownloadItem *webDownloadItem = new WebDownloadItem(webDownloadDelegate->GetEnv(), downloadItem);
    // destroy download_item since copied content from download_item.
    WebDownloadItem_Destroy(downloadItem);
    webDownloadItem->download_item_callback = wrapper;
    switch (webDownloadItem->state) {
        case NWebDownloadItemState::PENDING:
            //  When in PENDING state, chromium call downloadDidUpdate
            //  while file path is temporary file, just stop calling ui.
            break;
        case NWebDownloadItemState::IN_PROGRESS:
        case NWebDownloadItemState::PAUSED:
            webDownloadDelegate->DownloadDidUpdate(webDownloadItem);
            break;
        case NWebDownloadItemState::INTERRUPTED:
        case NWebDownloadItemState::CANCELED:
            webDownloadDelegate->DownloadDidFail(webDownloadItem);
            break;
        case NWebDownloadItemState::COMPLETE:
            webDownloadDelegate->DownloadDidFinish(webDownloadItem);
            break;
        case NWebDownloadItemState::MAX_DOWNLOAD_STATE:
            break;
    }
}
}

// static
void WebDownloadManager::RegisterDownloadCallback()
{
    // Only regist once.
    if (g_download_callback == nullptr) {
        WVLOG_I("RegisterDownloadCallback.");
        WebDownloader_CreateDownloadDelegateCallback(&g_download_callback);
        WebDownloader_SetDownloadBeforeStart(g_download_callback, &DownloadBeforeStart);
        WebDownloader_SetDownloadDidUpdate(g_download_callback, &DownloadDidUpdate);
        WebDownloadManager_PutDownloadCallback(g_download_callback);
    } else {
        WVLOG_E("[DOWNLOAD] had RegisterDownloadCallback.");
    }
}

// static
void WebDownloadManager::RemoveDownloadDelegate(WebDownloadDelegate *delegate)
{
    auto iterator = g_web_download_delegate_map.begin();
    while (iterator != g_web_download_delegate_map.end()) {
        if (iterator->second == delegate) {
            g_web_download_delegate_map.erase(iterator++);
        } else {
            iterator++;
        }
    }
}

// static
void WebDownloadManager::AddDownloadDelegateForWeb(int32_t nwebId, WebDownloadDelegate *delegate)
{
    NWebHelper::Instance().LoadNWebSDK();
    g_web_download_delegate_map.insert_or_assign(nwebId, delegate);
    RegisterDownloadCallback();
}

// static
void WebDownloadManager::SetDownloadDelegate(WebDownloadDelegate *delegate)
{
    NWebHelper::Instance().LoadNWebSDK();
    if (!g_default_delegate) {
        g_default_delegate = std::make_unique<WebDownloadDelegate>(*delegate);
        RegisterDownloadCallback();
    }
}

// static
bool WebDownloadManager::HasValidDelegate()
{
    if (!g_default_delegate) {
        return false;
    }

    return true;
}

// static
void WebDownloadManager::ResumeDownload(const WebDownloadItem *webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadManager::ResumeDownload");
    NWebHelper::Instance().LoadNWebSDK();
    NWebDownloadItem *download_item = nullptr;
    WebDownloadItem_CreateWebDownloadItem(&download_item);
    WebDownloadItem_SetGuid(download_item, webDownloadItem->guid.c_str());
    WebDownloadItem_SetUrl(download_item, webDownloadItem->url.c_str());
    WebDownloadItem_SetFullPath(download_item, webDownloadItem->full_path.c_str());
    WebDownloadItem_SetETag(download_item, webDownloadItem->etag.c_str());
    WebDownloadItem_SetMimeType(download_item, webDownloadItem->mime_type.c_str());
    WebDownloadItem_SetReceivedBytes(download_item, webDownloadItem->received_bytes);
    WebDownloadItem_SetTotalBytes(download_item, webDownloadItem->total_bytes);
    WebDownloadItem_SetReceivedSlices(download_item, webDownloadItem->received_slices.c_str());
    WebDownloadItem_SetLastModified(download_item, webDownloadItem->last_modified.c_str());
    WebDownloader_ResumeDownloadStatic(download_item);
    return;
}
} // namespace NWeb
} // namespace OHOS