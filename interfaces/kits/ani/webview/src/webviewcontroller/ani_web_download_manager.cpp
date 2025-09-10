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

#include <array>
#include <iostream>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_download_delegate.h"
#include "web_download_manager.h"
#include "web_errors.h"
#include "webview_controller.h"


namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_DOWNLOAD_MANAGER_CLASS_NAME = "L@ohos/web/webview/webview/WebDownloadManager;";
}

static void ResumeDownload(ani_env* env, ani_object object, ani_object webDownloadItem)
{
    WVLOG_D("[DOWNLOAD] WebDownloadManager native ResumeDownload");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!WebDownloadManager::HasValidDelegate()) {
        AniBusinessError::ThrowErrorByErrCode(env, NWebError::NO_DOWNLOAD_DELEGATE_SET);
        return;
    }
    WebDownloadItem* delegate = nullptr;
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(webDownloadItem, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    delegate = reinterpret_cast<WebDownloadItem*>(thisVar);
    if (delegate == nullptr) {
        WVLOG_E("WebDownloadDelegate is null.");
        return;
    }
    WebDownloadManager::ResumeDownload(delegate);
    return;
}

static void SetDownloadDelegate(ani_env* env, ani_object object, ani_object delegate)
{
    WVLOG_D("[DOWNLOAD] SetDownloadDelegate");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WebDownloadDelegate* delegateObj = nullptr;
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(delegate, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    delegateObj = reinterpret_cast<WebDownloadDelegate*>(thisVar);
    if (delegateObj == nullptr) {
        WVLOG_E("WebDownloadDelegate is null.");
        return;
    }
    WebDownloadManager::SetDownloadDelegate(delegateObj);
    return;
}

ani_status StsWebDownloadManagerInit(ani_env* env)
{
    WVLOG_D("[DOWNLOAD] StsWebDownloadManagerInit");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webDownloadManagerCls = nullptr;
    ani_status status = env->FindClass(WEB_DOWNLOAD_MANAGER_CLASS_NAME, &webDownloadManagerCls);
    if (status != ANI_OK || !webDownloadManagerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_DOWNLOAD_MANAGER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "resumeDownload", nullptr, reinterpret_cast<void*>(ResumeDownload) },
        ani_native_function { "setDownloadDelegate", nullptr, reinterpret_cast<void*>(SetDownloadDelegate) },
    };

    status = env->Class_BindNativeMethods(webDownloadManagerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS