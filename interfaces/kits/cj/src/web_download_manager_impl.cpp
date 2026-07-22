/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "web_download_manager_impl.h"

#include <cstring>

#include "ffi_remote_data.h"
#include "nweb_c_api.h"
#include "nweb_helper.h"
#include "webview_log.h"
#include "web_download_delegate_impl.h"

using namespace OHOS::FFI;

namespace OHOS::Webview {
namespace {
    static sptr<WebDownloadDelegateImpl> g_default_delegate;
    static std::unordered_map<int32_t, WebDownloadDelegateImpl *> g_web_download_delegate_map;
    static WebDownloadDelegateCallback *g_download_callback;

    WebDownloadDelegateImpl *GetWebDownloadDelegate(int32_t nwebId)
    {
        auto it = g_web_download_delegate_map.find(nwebId);
        if (it != g_web_download_delegate_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    void DownloadBeforeStart(NWebDownloadItem *downloadItem, WebBeforeDownloadCallbackWrapper *wrapper)
    {
        WEBVIEWLOGD("[DOWNLOAD] DownloadBeforeStart.");
        if (wrapper == nullptr) {
            WEBVIEWLOGE("[DOWNLOAD] WebBeforeDownloadCallbackWrapper is null");
            return;
        }
        int32_t nwebId = WebDownloadItem_NWebId(downloadItem);
        WebDownloadDelegateImpl *webDownloadDelegate = GetWebDownloadDelegate(nwebId);
        if (!webDownloadDelegate) {
            WEBVIEWLOGD("[DOWNLOAD] donn't found delegate for nweb.");
            webDownloadDelegate = g_default_delegate;
        }

        if (!webDownloadDelegate) {
            WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
            return;
        }

        WebDownloadItemImpl *webDownloadItem = FFIData::Create<WebDownloadItemImpl>(downloadItem);
        // destroy download_item since copied content from download_item.
        WebDownloadItem_Destroy(downloadItem);
        if (!webDownloadItem) {
            WEBVIEWLOGE("[DOWNLOAD] webDownloadItem is null");
            return;
        }
        webDownloadItem->before_download_callback = wrapper;
        webDownloadDelegate->DownloadBeforeStart(webDownloadItem);
    }

    void DownloadDidUpdate(NWebDownloadItem *downloadItem, WebDownloadItemCallbackWrapper *wrapper)
    {
        WEBVIEWLOGI("DownloadDidUpdate.");
        if (wrapper == nullptr) {
            WEBVIEWLOGE("[DOWNLOAD] WebBeforeDownloadCallbackWrapper is null");
            return;
        }

        int32_t nwebId = WebDownloadItem_NWebId(downloadItem);

        WebDownloadDelegateImpl *webDownloadDelegate = GetWebDownloadDelegate(nwebId);
        if (!webDownloadDelegate) {
            WEBVIEWLOGD("[DOWNLOAD] donn't found delegate for nweb.");
            webDownloadDelegate = g_default_delegate;
        }

        if (!webDownloadDelegate) {
            WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
            return;
        }
        WebDownloadItemImpl *webDownloadItem = FFIData::Create<WebDownloadItemImpl>(downloadItem);
        // destroy download_item since copied content from download_item.
        WebDownloadItem_Destroy(downloadItem);
        if (!webDownloadItem) {
            WEBVIEWLOGE("[DOWNLOAD] webDownloadItem is null");
            return;
        }
        webDownloadItem->download_item_callback = wrapper;
        switch (webDownloadItem->state) {
            case NWebDownloadItemState::PENDING:
                //  When in PENDING state, chromium call downloadDidUpdate
                //  while file path is temporary file, just stop calling ui.
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
                webDownloadItem = nullptr;
                break;
        }
    }
    } // namespace

    // static
    void WebDownloadManagerImpl::RegisterDownloadCallback()
    {
        // Only regist once.
        if (g_download_callback == nullptr) {
            WEBVIEWLOGI("RegisterDownloadCallback.");
            WebDownloader_CreateDownloadDelegateCallback(&g_download_callback);
            WebDownloader_SetDownloadBeforeStart(g_download_callback, &DownloadBeforeStart);
            WebDownloader_SetDownloadDidUpdate(g_download_callback, &DownloadDidUpdate);
            WebDownloadManager_PutDownloadCallback(g_download_callback);
        } else {
            WEBVIEWLOGE("[DOWNLOAD] had RegisterDownloadCallback.");
        }
    }

    // static
    void WebDownloadManagerImpl::RemoveDownloadDelegate(WebDownloadDelegateImpl *delegate)
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
    void WebDownloadManagerImpl::AddDownloadDelegateForWeb(int32_t nwebId, WebDownloadDelegateImpl *delegate)
    {
        OHOS::NWeb::NWebHelper::Instance().LoadNWebSDK();
        g_web_download_delegate_map.insert_or_assign(nwebId, delegate);
        RegisterDownloadCallback();
    }

    // static
    void WebDownloadManagerImpl::SetDownloadDelegate(WebDownloadDelegateImpl *delegate)
    {
        OHOS::NWeb::NWebHelper::Instance().LoadNWebSDK();
        if (!g_default_delegate) {
            g_default_delegate = delegate;
            RegisterDownloadCallback();
        }
    }

    // static
    bool WebDownloadManagerImpl::HasValidDelegate()
    {
        if (!g_default_delegate) {
            return false;
        }

        return true;
    }

    // static
    void WebDownloadManagerImpl::ResumeDownload(const WebDownloadItemImpl *webDownload)
    {
        WEBVIEWLOGD("[DOWNLOAD] WebDownloadManager::ResumeDownload");
        if (!webDownload) {
            WEBVIEWLOGE("webDownload is nullptr");
            return;
        }
        OHOS::NWeb::NWebHelper::Instance().LoadNWebSDK();
        NWebDownloadItem *downloadItem = nullptr;
        WebDownloadItem_CreateWebDownloadItem(&downloadItem);
        if (downloadItem == nullptr) {
            WEBVIEWLOGE("[DOWNLOAD] CreateWebDownloadItem failed.");
            return;
        }
        WebDownloadItem_SetGuid(downloadItem, webDownload->guid.c_str());
        WebDownloadItem_SetUrl(downloadItem, webDownload->url.c_str());
        WebDownloadItem_SetFullPath(downloadItem, webDownload->fullPath.c_str());
        WebDownloadItem_SetETag(downloadItem, webDownload->etag.c_str());
        WebDownloadItem_SetMimeType(downloadItem, webDownload->mimeType.c_str());
        WebDownloadItem_SetReceivedBytes(downloadItem, webDownload->receivedBytes);
        WebDownloadItem_SetTotalBytes(downloadItem, webDownload->totalBytes);
        WebDownloadItem_SetReceivedSlices(downloadItem, webDownload->receivedSlices.c_str());
        WebDownloadItem_SetLastModified(downloadItem, webDownload->lastModified.c_str());
        WebDownloader_ResumeDownloadStatic(downloadItem);
        return;
    }
} // namespace NWeb
