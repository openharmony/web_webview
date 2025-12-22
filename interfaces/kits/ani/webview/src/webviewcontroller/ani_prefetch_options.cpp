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
 
#include "ani_prefetch_options.h"
#include "prefetch_options.h"

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <securec.h>
 
#include "ani_parse_utils.h"
#include "business_error.h"
#include "nweb_log.h"
#include "web_errors.h"
 
namespace OHOS {
namespace NWeb {

using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* PREFETCH_OPTIONS_INNER_CLASS_NAME = "@ohos.web.webview.webview.PrefetchOptions";
}

static void Constructor(ani_env *env, ani_object object)
{
    WVLOG_D("PrefetchOption native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
}

ani_status StsPrefetchOptioninnerInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }

    ani_class prefetchOptionsInnerCls = nullptr;
    auto status = env->FindClass(PREFETCH_OPTIONS_INNER_CLASS_NAME, &prefetchOptionsInnerCls);
    if (status != ANI_OK || !prefetchOptionsInnerCls) {
        WVLOG_E(
            "find %{public}s class failed, status: %{public}d", PREFETCH_OPTIONS_INNER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
    };

    status = env->Class_BindNativeMethods(prefetchOptionsInnerCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}

} // namespace NWeb
} // namespace OHOS