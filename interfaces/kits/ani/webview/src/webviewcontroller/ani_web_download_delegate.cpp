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

#include <array>
#include <iostream>

#include "webview_controller.h"
#include "web_download_delegate.h"

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* WEB_DOWNLOAD_DELEGATE_CLASS_NAME = "L@ohos/web/webview/webview/WebDownloadDelegate;";
}

static void JsDownloadBeforeStart(ani_env *env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("[DOWNLOAD] JsDownloadBeforeStart");
    if (env == nullptr) {
        WVLOG_E("[DOWNLOAD] env is nullptr");
        return;
    }

    auto* webDownloadDelegate = reinterpret_cast<WebDownloadDelegate *>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return;
    }
    webDownloadDelegate->PutDownloadBeforeStart(callback);
}

static void Constructor(ani_env *env, ani_object object)
{
    WVLOG_D("[DOWNLOAD] WebDownloadDelegate native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    WebDownloadDelegate* webDownloadDelegate = new (std::nothrow) WebDownloadDelegate(env);
    if (webDownloadDelegate == nullptr) {
        WVLOG_E("new WebDownloadDelegate failed");
        return;
    }
    if (!AniParseUtils::Wrap(env, object, WEB_DOWNLOAD_DELEGATE_CLASS_NAME,
                             reinterpret_cast<ani_long>(webDownloadDelegate))) {
        WVLOG_E("WebDownloadDelegate wrap failed");
        delete webDownloadDelegate;
        webDownloadDelegate = nullptr;
    }
}

ani_status StsWebDownloadDelegateInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webDownloadDelagateCls = nullptr;
    ani_status status = env->FindClass(WEB_DOWNLOAD_DELEGATE_CLASS_NAME, &webDownloadDelagateCls);
    if (status != ANI_OK || !webDownloadDelagateCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", WEB_DOWNLOAD_DELEGATE_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
        ani_native_function { "onBeforeDownload", "Lstd/core/Function1;:V",
                              reinterpret_cast<void *>(JsDownloadBeforeStart) },
    };

    status = env->Class_BindNativeMethods(webDownloadDelagateCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS