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

#include "web_download_delegate.h"

#include <cstring>

#include "nweb_c_api.h"
#include "nweb_log.h"
#include "napi_web_download_item.h"
#include "web_download_manager.h"
#include "nweb_napi_scope.h"

namespace OHOS {
namespace NWeb {
WebDownloadDelegate::WebDownloadDelegate(napi_env env)
    : delegate_(nullptr),
      download_before_start_callback_(nullptr),
      download_did_update_callback_(nullptr),
      download_did_finish_callback_(nullptr),
      download_did_fail_callback_(nullptr),
      env_(env)
{
    WVLOG_D("WebDownloadDelegate::WebDownloadDelegate");
}

WebDownloadDelegate::~WebDownloadDelegate()
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::~WebDownloadDelegate");
    if (download_before_start_callback_) {
        napi_delete_reference(env_, download_before_start_callback_);
    }
    if (download_did_update_callback_) {
        napi_delete_reference(env_, download_did_update_callback_);
    }
    if (download_did_finish_callback_) {
        napi_delete_reference(env_, download_did_finish_callback_);
    }
    if (download_did_fail_callback_) {
        napi_delete_reference(env_, download_did_fail_callback_);
    }
    WebDownloadManager::RemoveDownloadDelegate(this);
}

void WebDownloadDelegate::RemoveSelfRef()
{
    if (delegate_) {
        napi_delete_reference(env_, delegate_);
        delegate_ = nullptr;
    }
}

void WebDownloadDelegate::DownloadBeforeStart(WebDownloadItem *webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::DownloadBeforeStart");
    if (!env_) {
        WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadBeforeStart nil env");
        return;
    }
    size_t paramCount = 1;

    OHOS::NApiScope scope(env_);

    napi_value callbackFunc = nullptr;
    napi_status status;

    if (!download_before_start_callback_) {
        WVLOG_E("[DOWNLOAD] downloadBeforeStart nil env");
        return;
    }
    status = napi_get_reference_value(env_, download_before_start_callback_, &callbackFunc);
    if (status != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("[DOWNLOAD] get downloadBeforeStart func failed.");
        return;
    }

    napi_value webDownloadItemValue = nullptr;
    napi_create_object(env_, &webDownloadItemValue);
    napi_wrap(
        env_, webDownloadItemValue, webDownloadItem,
        [](napi_env                             /* env */, void *data, void * /* hint */) {
            if (data) {
                WebDownloadItem *downloadItem = (WebDownloadItem *)data;
                if (!downloadItem->download_item_callback &&
                    !downloadItem->hasStarted &&
                    downloadItem->before_download_callback) {
                        WVLOG_I("[DOWNLOAD] downloadBeforeStart cancel guid: %s.",
                                downloadItem->guid.c_str());
                        WebDownload_CancelBeforeDownload(downloadItem->before_download_callback);
                    }
                delete downloadItem;
            }
        },
        nullptr, nullptr);
    NapiWebDownloadItem::DefineProperties(env_, &webDownloadItemValue);

    napi_value result = nullptr;
    status = napi_call_function(env_, nullptr, callbackFunc, paramCount, &webDownloadItemValue, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] call downloadBeforeStart failed.");
    }
}
void WebDownloadDelegate::DownloadDidUpdate(WebDownloadItem *webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::DownloadDidUpdate");
    if (!env_) {
        WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadDidUpdate nil env.");
        return;
    }
    size_t paramCount = 1;

    OHOS::NApiScope scope(env_);

    napi_value callbackFunc = nullptr;
    napi_status status;

    if (!download_did_update_callback_) {
        WVLOG_E("[DOWNLOAD] downloadDidUpdate not exists.");
        return;
    }
    status = napi_get_reference_value(env_, download_did_update_callback_, &callbackFunc);
    if (status != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("[DOWNLOAD] get downloadDidUpdate func failed.");
        return;
    }

    napi_value webDownloadItemValue = nullptr;
    napi_create_object(env_, &webDownloadItemValue);
    napi_wrap(
        env_, webDownloadItemValue, webDownloadItem,
        [](napi_env                             /* env */, void *data, void * /* hint */) {
            if (data) {
                WebDownloadItem *downloadItem = (WebDownloadItem *)data;
                delete downloadItem;
            }
        },
        nullptr, nullptr);
    NapiWebDownloadItem::DefineProperties(env_, &webDownloadItemValue);

    napi_value result = nullptr;
    status = napi_call_function(env_, nullptr, callbackFunc, paramCount, &webDownloadItemValue, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] call downloadDidUpdate failed.");
    }
}

void WebDownloadDelegate::DownloadDidFail(WebDownloadItem *webDownloadItem)
{
    WVLOG_D("WebDownloadDelegate::DownloadDidFail");
    if (!env_) {
        WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadDidFail nil env.");
        return;
    }
    size_t paramCount = 1;

    OHOS::NApiScope scope(env_);

    napi_value callbackFunc = nullptr;
    napi_status status;

    if (!download_did_fail_callback_) {
        WVLOG_E("[DOWNLOAD] DownloadDidFail not exists.");
        return;
    }
    status = napi_get_reference_value(env_, download_did_fail_callback_, &callbackFunc);
    if (status != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("[DOWNLOAD] get downloadDidFail func failed.");
        return;
    }

    napi_value webDownloadItemValue = nullptr;
    napi_create_object(env_, &webDownloadItemValue);
    napi_wrap(
        env_, webDownloadItemValue, webDownloadItem,
        [](napi_env                             /* env */, void *data, void * /* hint */) {
            if (data) {
                WebDownloadItem *downloadItem = (WebDownloadItem *)data;
                delete downloadItem;
            }
        },
        nullptr, nullptr);
    NapiWebDownloadItem::DefineProperties(env_, &webDownloadItemValue);

    napi_value result = nullptr;
    status = napi_call_function(env_, nullptr, callbackFunc, paramCount, &webDownloadItemValue, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] call downloadDidFail failed.");
    }
}

void WebDownloadDelegate::DownloadDidFinish(WebDownloadItem *webDownloadItem)
{
    WVLOG_D("WebDownloadDelegate::DownloadDidFinish");
    if (!env_) {
        WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadDidFinish nil env.");
        return;
    }
    size_t paramCount = 1;

    OHOS::NApiScope scope(env_);

    napi_value callbackFunc = nullptr;
    napi_status status;

    if (!download_did_finish_callback_) {
        WVLOG_E("[DOWNLOAD] downloadDidFinish not exists.");
        return;
    }
    status = napi_get_reference_value(env_, download_did_finish_callback_, &callbackFunc);
    if (status != napi_ok || callbackFunc == nullptr) {
        WVLOG_E("[DOWNLOAD] get downloadDidFinish func failed.");
        return;
    }

    napi_value webDownloadItemValue = nullptr;
    napi_create_object(env_, &webDownloadItemValue);
    napi_wrap(
        env_, webDownloadItemValue, webDownloadItem,
        [](napi_env                             /* env */, void *data, void * /* hint */) {
            if (data) {
                WebDownloadItem *downloadItem = (WebDownloadItem *)data;
                delete downloadItem;
            }
        },
        nullptr, nullptr);
    NapiWebDownloadItem::DefineProperties(env_, &webDownloadItemValue);

    napi_value result = nullptr;
    status = napi_call_function(env_, nullptr, callbackFunc, paramCount, &webDownloadItemValue, &result);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] call downloadDidFinish failed.");
    }
}

void WebDownloadDelegate::PutDownloadBeforeStart(napi_env, napi_value callback)
{
    WVLOG_D("WebDownloadDelegate::PutDownloadBeforeStart");
    napi_status status = napi_create_reference(env_, callback, 1, &download_before_start_callback_);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] PutDownloadBeforeStart create reference failed.");
    }
}

void WebDownloadDelegate::PutDownloadDidUpdate(napi_env, napi_value callback)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::PutDownloadDidUpdate");
    napi_status status = napi_create_reference(env_, callback, 1, &download_did_update_callback_);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] PutDownloadDidUpdate create reference failed.");
    }
}

void WebDownloadDelegate::PutDownloadDidFinish(napi_env, napi_value callback)
{
    WVLOG_D("WebDownloadDelegate::PutDownloadDidFinish");
    napi_status status = napi_create_reference(env_, callback, 1, &download_did_finish_callback_);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] PutDownloadDidFinish create reference failed.");
    }
}

void WebDownloadDelegate::PutDownloadDidFail(napi_env, napi_value callback)
{
    WVLOG_D("WebDownloadDelegate::PutDownloadDidFail");
    napi_status status = napi_create_reference(env_, callback, 1, &download_did_fail_callback_);
    if (status != napi_status::napi_ok) {
        WVLOG_E("[DOWNLOAD] PutDownloadDidFail create reference failed.");
    }
}

int32_t WebDownloadDelegate::GetNWebId() const
{
    return nwebId_;
}

void WebDownloadDelegate::SetNWebId(int32_t nwebId)
{
    nwebId_ = nwebId;
}

napi_env WebDownloadDelegate::GetEnv()
{
    return env_;
}
} // namespace NWeb
} // namespace OHOS
