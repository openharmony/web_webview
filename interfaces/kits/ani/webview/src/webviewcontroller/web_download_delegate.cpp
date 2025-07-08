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

#include "web_download_delegate.h"

#include <cstring>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_c_api.h"
#include "nweb_log.h"
#include "web_download_manager.h"
#include "web_errors.h"


namespace OHOS {
namespace NWeb {
namespace {
const char* WEB_DOWNLOAD_ITEM_CLASS_NAME = "L@ohos/web/webview/webview/WebDownloadItem;";
}

WebDownloadDelegate::WebDownloadDelegate(ani_env* env)
    : delegate_(nullptr), download_before_start_callback_(nullptr), download_did_update_callback_(nullptr),
      download_did_finish_callback_(nullptr), download_did_fail_callback_(nullptr), env_(env)
{
    WVLOG_D("WebDownloadDelegate::WebDownloadDelegate");
}

WebDownloadDelegate::~WebDownloadDelegate()
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::~WebDownloadDelegate");
    if (download_before_start_callback_) {
        env_->GlobalReference_Delete(download_before_start_callback_);
    }
    if (download_did_update_callback_) {
        env_->GlobalReference_Delete(download_did_update_callback_);
    }
    if (download_did_finish_callback_) {
        env_->GlobalReference_Delete(download_did_finish_callback_);
    }
    if (download_did_fail_callback_) {
        env_->GlobalReference_Delete(download_did_fail_callback_);
    }
    WebDownloadManager::RemoveDownloadDelegate(this);
}

void WebDownloadDelegate::RemoveSelfRef()
{
    if (delegate_) {
        env_->GlobalReference_Delete(delegate_);
        delegate_ = nullptr;
    }
}

void WebDownloadDelegate::DownloadBeforeStart(WebDownloadItem* webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::DownloadBeforeStart");
    bool success = false;
    do {
        if (env_ == nullptr) {
            WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadBeforeStart nil env");
            break;
        }
        if (!download_before_start_callback_ || !webDownloadItem) {
            WVLOG_E("[DOWNLOAD] download_before_start_callback_ or webDownloadItem is null.");
            break;
        }
        ani_object webItemObj = {};
        if (!AniParseUtils::CreateObjectVoid(env_, WEB_DOWNLOAD_ITEM_CLASS_NAME, webItemObj)) {
            WVLOG_E("[DOWNLOAD] aniDownloadItem is null");
            break;
        }
        if (!AniParseUtils::Wrap(
                env_, webItemObj, WEB_DOWNLOAD_ITEM_CLASS_NAME, reinterpret_cast<ani_long>(webDownloadItem))) {
            WVLOG_E("[DOWNLOAD] WebDownloadItem wrap failed");
            break;
        }
        success = true;
        std::vector<ani_ref> vec;
        vec.push_back(webItemObj);
        ani_ref result = nullptr;
        if (env_->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(download_before_start_callback_), vec.size(),
                vec.data(), &result) != ANI_OK) {
            WVLOG_E("FunctionalObject_Call failed");
        }
    } while (false);
    if (!success && webDownloadItem) {
        delete webDownloadItem;
        webDownloadItem = nullptr;
    }
}

void WebDownloadDelegate::DownloadDidUpdate(WebDownloadItem* webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::DownloadDidUpdate");
    bool success = false;
    do {
        if (env_ == nullptr) {
            WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadDidUpdate nil env.");
            break;
        }
        if (!download_did_update_callback_ || !webDownloadItem) {
            WVLOG_E("[DOWNLOAD] download_did_update_callback_ or webDownloadItem is null.");
            break;
        }
        ani_object webItemObj = {};
        if (!AniParseUtils::CreateObjectVoid(env_, WEB_DOWNLOAD_ITEM_CLASS_NAME, webItemObj)) {
            WVLOG_E("[DOWNLOAD] aniDownloadItem is null");
            break;
        }
        if (!AniParseUtils::Wrap(
                env_, webItemObj, WEB_DOWNLOAD_ITEM_CLASS_NAME, reinterpret_cast<ani_long>(webDownloadItem))) {
            WVLOG_E("[DOWNLOAD] WebDownloadItem wrap failed");
            break;
        }
        success = true;
        std::vector<ani_ref> vec;
        vec.push_back(webItemObj);
        ani_ref result = nullptr;
        if (env_->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(download_did_update_callback_), vec.size(),
                vec.data(), &result) != ANI_OK) {
            WVLOG_E("FunctionalObject_Call failed");
        }
    } while (false);
    if (!success && webDownloadItem) {
        delete webDownloadItem;
        webDownloadItem = nullptr;
    }
}

void WebDownloadDelegate::DownloadDidFail(WebDownloadItem* webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::DownloadDidFail");
    bool success = false;
    do {
        if (env_ == nullptr) {
            WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadDidFail nil env");
            break;
        }
        if (!download_did_fail_callback_ || !webDownloadItem) {
            WVLOG_E("[DOWNLOAD] download_did_fail_callback_ or webDownloadItem is null.");
            break;
        }
        ani_object webItemObj = {};
        if (!AniParseUtils::CreateObjectVoid(env_, WEB_DOWNLOAD_ITEM_CLASS_NAME, webItemObj)) {
            WVLOG_E("[DOWNLOAD] aniDownloadItem is null");
            break;
        }
        if (!AniParseUtils::Wrap(
                env_, webItemObj, WEB_DOWNLOAD_ITEM_CLASS_NAME, reinterpret_cast<ani_long>(webDownloadItem))) {
            WVLOG_E("[DOWNLOAD] WebDownloadItem wrap failed");
            break;
        }
        success = true;
        std::vector<ani_ref> vec;
        vec.push_back(webItemObj);
        ani_ref result = nullptr;
        if (env_->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(download_did_fail_callback_), vec.size(),
                vec.data(), &result) != ANI_OK) {
            WVLOG_E("FunctionalObject_Call failed");
        }
    } while (false);
    if (!success && webDownloadItem) {
        delete webDownloadItem;
        webDownloadItem = nullptr;
    }
}

void WebDownloadDelegate::DownloadDidFinish(WebDownloadItem* webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate::DownloadDidFinish");
    bool success = false;
    do {
        if (env_ == nullptr) {
            WVLOG_E("[DOWNLOAD] WebDownloadDelegate::DownloadDidFinish nil env");
            break;
        }
        if (!download_did_finish_callback_ || !webDownloadItem) {
            WVLOG_E("[DOWNLOAD] download_did_finish_callback_ or webDownloadItem is null.");
            break;
        }
        ani_object webItemObj = {};
        if (!AniParseUtils::CreateObjectVoid(env_, WEB_DOWNLOAD_ITEM_CLASS_NAME, webItemObj)) {
            WVLOG_E("[DOWNLOAD] aniDownloadItem is null");
            break;
        }
        if (!AniParseUtils::Wrap(
                env_, webItemObj, WEB_DOWNLOAD_ITEM_CLASS_NAME, reinterpret_cast<ani_long>(webDownloadItem))) {
            WVLOG_E("[DOWNLOAD] WebDownloadItem wrap failed");
            break;
        }
        success = true;
        std::vector<ani_ref> vec;
        vec.push_back(webItemObj);
        ani_ref result = nullptr;
        if (env_->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(download_did_finish_callback_), vec.size(),
                vec.data(), &result) != ANI_OK) {
            WVLOG_E("FunctionalObject_Call failed");
        }
    } while (false);
    if (!success && webDownloadItem) {
        delete webDownloadItem;
        webDownloadItem = nullptr;
    }
}

void WebDownloadDelegate::PutDownloadBeforeStart(ani_fn_object callback)
{
    WVLOG_I("[DOWNLOAD] WebDownloadDelegate::PutDownloadBeforeStart");
    env_->GlobalReference_Create(reinterpret_cast<ani_ref>(callback), &download_before_start_callback_);
}

void WebDownloadDelegate::PutDownloadDidUpdate(ani_fn_object callback)
{
    WVLOG_I("[DOWNLOAD] WebDownloadDelegate::PutDownloadDidUpdate");
    env_->GlobalReference_Create(reinterpret_cast<ani_ref>(callback), &download_did_update_callback_);
}

void WebDownloadDelegate::PutDownloadDidFinish(ani_fn_object callback)
{
    WVLOG_I("[DOWNLOAD] WebDownloadDelegate::PutDownloadDidFinish");
    env_->GlobalReference_Create(reinterpret_cast<ani_ref>(callback), &download_did_finish_callback_);
}

void WebDownloadDelegate::PutDownloadDidFail(ani_fn_object callback)
{
    WVLOG_I("[DOWNLOAD] WebDownloadDelegate::PutDownloadDidFail");
    env_->GlobalReference_Create(reinterpret_cast<ani_ref>(callback), &download_did_fail_callback_);
}

int32_t WebDownloadDelegate::GetNWebId() const
{
    return nwebId_;
}

void WebDownloadDelegate::SetNWebId(int32_t nwebId)
{
    nwebId_ = nwebId;
}

ani_env* WebDownloadDelegate::GetEnv()
{
    return env_;
}
} // namespace NWeb
} // namespace OHOS
