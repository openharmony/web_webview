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

#include "web_download_manager.h"

#include <cstring>
#include <mutex>
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
static std::mutex g_defaultDelegateMutex;
static std::mutex g_delegateMapMutex;
WebDownloadDelegate *GetWebDownloadDelegate(int32_t nwebId)
{
    std::lock_guard<std::mutex> lock(g_delegateMapMutex);
    auto it = g_web_download_delegate_map.find(nwebId);
    if (it != g_web_download_delegate_map.end()) {
        return it->second;
    }
    return nullptr;
}

void DownloadBeforeStart(NWebDownloadItem *downloadItem, WebBeforeDownloadCallbackWrapper *wrapper)
{
    WVLOG_D("[DOWNLOAD] DownloadBeforeStart.");
    if (downloadItem == nullptr) {
        WVLOG_E("[DOWNLOAD] NWebDownloadItem is null");
        return;
    }
    if (wrapper == nullptr) {
        WVLOG_E("[DOWNLOAD] WebBeforeDownloadCallbackWrapper is null");
        return;
    }
    int32_t nwebId = WebDownloadItem_NWebId(downloadItem);
    WebDownloadDelegate *webDownloadDelegate = GetWebDownloadDelegate(nwebId);
    if (!webDownloadDelegate) {
        WVLOG_D("[DOWNLOAD] donn't found delegate for nweb.");
        std::lock_guard<std::mutex> lock(g_defaultDelegateMutex);
        webDownloadDelegate = g_default_delegate.get();
    }

    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }

    WebDownloadItem *webDownloadItem = new WebDownloadItem(webDownloadDelegate->GetEnv(), downloadItem);
    if (webDownloadItem == nullptr) {
        WVLOG_E("[DOWNLOAD] webDownloadItem is null");
        return;
    }
    // destroy download_item since copied content from download_item.
    WebDownloadItem_Destroy(downloadItem);
    webDownloadItem->before_download_callback = wrapper;
    webDownloadDelegate->DownloadBeforeStart(webDownloadItem);
}

void DownloadDidUpdate(NWebDownloadItem *downloadItem, WebDownloadItemCallbackWrapper *wrapper)
{
    WVLOG_D("[DOWNLOAD] DownloadDidUpdate.");
    if (wrapper == nullptr) {
        WVLOG_E("[DOWNLOAD] WebBeforeDownloadCallbackWrapper is null");
        return;
    }
    if (downloadItem == nullptr) {
        WVLOG_E("[DOWNLOAD] downloadItem is null");
        return;
    }

    int32_t nwebId = WebDownloadItem_NWebId(downloadItem);

    WebDownloadDelegate *webDownloadDelegate = GetWebDownloadDelegate(nwebId);
    if (!webDownloadDelegate) {
        WVLOG_D("[DOWNLOAD] didn't find delegate for nweb.");
        std::lock_guard<std::mutex> lock(g_defaultDelegateMutex);
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
            delete webDownloadItem;
            webDownloadItem = nullptr;
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
        default:
            delete webDownloadItem;
            webDownloadItem = nullptr;
            break;
    }
}
} // namespace

// static
void WebDownloadManager::RegisterDownloadCallback()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        WVLOG_I("RegisterDownloadCallback.");
        WebDownloader_CreateDownloadDelegateCallback(&g_download_callback);
        WebDownloader_SetDownloadBeforeStart(g_download_callback, &DownloadBeforeStart);
        WebDownloader_SetDownloadDidUpdate(g_download_callback, &DownloadDidUpdate);
        WebDownloadManager_PutDownloadCallback(g_download_callback);
    });
}

// static
void WebDownloadManager::RemoveDownloadDelegate(WebDownloadDelegate *delegate)
{
    std::lock_guard<std::mutex> lock(g_delegateMapMutex);
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
    std::lock_guard<std::mutex> lock(g_delegateMapMutex);
    g_web_download_delegate_map.insert_or_assign(nwebId, delegate);
    RegisterDownloadCallback();
}

// static
void WebDownloadManager::RemoveDownloadDelegateRef(int32_t nwebId)
{
    WebDownloadDelegate *delegate = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_delegateMapMutex);
        auto iter = g_web_download_delegate_map.find(nwebId);
        if (iter != g_web_download_delegate_map.end()) {
            delegate = iter->second;
        }
    }
    if (delegate) {
        delegate->RemoveSelfRef();
    }
}

// static
void WebDownloadManager::SetDownloadDelegate(WebDownloadDelegate *delegate)
{
    NWebHelper::Instance().LoadNWebSDK();
    bool needRegister = false;
    {
        std::lock_guard<std::mutex> lock(g_defaultDelegateMutex);
        if (!g_default_delegate) {
            g_default_delegate = std::make_unique<WebDownloadDelegate>(*delegate);
            needRegister = true;
        }
    }
    if (needRegister) {
        RegisterDownloadCallback();
    }
}

// static
bool WebDownloadManager::HasValidDelegate()
{
    std::lock_guard<std::mutex> lock(g_defaultDelegateMutex);
    return g_default_delegate != nullptr;
}

// static
void WebDownloadManager::ResumeDownload(const WebDownloadItem *webDownload)
{
    WVLOG_D("[DOWNLOAD] WebDownloadManager::ResumeDownload");
    if (!webDownload) {
        WVLOG_E("webDownload is nullptr");
        return;
    }
    NWebHelper::Instance().LoadNWebSDK();
    NWebDownloadItem *downloadItem = nullptr;
    WebDownloadItem_CreateWebDownloadItem(&downloadItem);
    WebDownloadItem_SetGuid(downloadItem, webDownload->guid.c_str());
    WebDownloadItem_SetUrl(downloadItem, webDownload->url.c_str());
    WebDownloadItem_SetFullPath(downloadItem, webDownload->fullPath.c_str());
    WebDownloadItem_SetETag(downloadItem, webDownload->etag.c_str());
    WebDownloadItem_SetMimeType(downloadItem, webDownload->mimeType.c_str());
    WebDownloadItem_SetReceivedBytes(downloadItem, webDownload->receivedBytes);
    WebDownloadItem_SetTotalBytes(downloadItem, webDownload->totalBytes);
    WebDownloadItem_SetReceivedSlices(downloadItem, webDownload->receivedSlices.c_str());
    WebDownloadItem_SetLastModified(downloadItem, webDownload->lastModified.c_str());
    WebDownloadItem_SetOriginalUrl(downloadItem, webDownload->originalUrl.c_str());
    WebDownloadItem_SetReferrerUrl(downloadItem, webDownload->referrerUrl.c_str());
    if (!webDownload->urlChain.empty()) {
        std::vector<char*> urlChainArray;
        for (const auto& url : webDownload->urlChain) {
            urlChainArray.push_back(const_cast<char*>(url.c_str()));
        }
        WebDownloadItem_SetUrlChain(downloadItem, urlChainArray.data(), urlChainArray.size());
    }
    WebDownloader_ResumeDownloadStatic(downloadItem);
    return;
}
} // namespace NWeb
} // namespace OHOS
