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

#include "napi_web_download_delegate.h"

#include <cstring>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <napi/native_api.h>
#include <securec.h>

#include "nweb_log.h"
#include "web_download_delegate.h"

namespace OHOS {
namespace NWeb {
napi_value NapiWebDownloadDelegate::JS_DownloadBeforeStart(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadDelegate::JS_DownloadBeforeStart");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    WebDownloadDelegate *webDownloadDelegate = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, &data);

    napi_unwrap(env, thisVar, (void **)&webDownloadDelegate);
    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return thisVar;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);

    webDownloadDelegate->PutDownloadBeforeStart(env, argv[0]);
    return thisVar;
}

napi_value NapiWebDownloadDelegate::JS_DownloadDidUpdate(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadDelegate::JS_DownloadDidUpdate");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    WebDownloadDelegate *webDownloadDelegate = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, &data);

    napi_unwrap(env, thisVar, (void **)&webDownloadDelegate);
    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return thisVar;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);

    webDownloadDelegate->PutDownloadDidUpdate(env, argv[0]);
    return thisVar;
}

napi_value NapiWebDownloadDelegate::JS_DownloadDidFinish(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadDelegate::JS_DownloadDidFinish");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    WebDownloadDelegate *webDownloadDelegate = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, &data);

    napi_unwrap(env, thisVar, (void **)&webDownloadDelegate);
    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return thisVar;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);

    webDownloadDelegate->PutDownloadDidFinish(env, argv[0]);
    return thisVar;
}

napi_value NapiWebDownloadDelegate::JS_DownloadDidFail(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadDelegate::JS_DownloadDidFail");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    WebDownloadDelegate *webDownloadDelegate = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, &data);

    napi_unwrap(env, thisVar, (void **)&webDownloadDelegate);
    if (!webDownloadDelegate) {
        WVLOG_E("[DOWNLOAD] webDownloadDelegate is null");
        return thisVar;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);

    webDownloadDelegate->PutDownloadDidFail(env, argv[0]);
    return thisVar;
}

napi_value NapiWebDownloadDelegate::JS_Constructor(napi_env env, napi_callback_info cbinfo)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadDelegate::JS_Constructor");
    WebDownloadDelegate *delegate = new WebDownloadDelegate(env);
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr);
    napi_wrap(
        env, thisVar, delegate,
        [](napi_env /* env */, void *data, void * /* hint */) {
            WebDownloadDelegate *delegate = static_cast<WebDownloadDelegate *>(data);
            delete delegate;
        },
        nullptr, nullptr);

    return thisVar;
}

napi_value NapiWebDownloadDelegate::Init(napi_env env, napi_value exports)
{
    WVLOG_D("[DOWNLOAD] NapiWebDownloadDelegate::Init");
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("onBeforeDownload", JS_DownloadBeforeStart),
        DECLARE_NAPI_FUNCTION("onDownloadUpdated", JS_DownloadDidUpdate),
        DECLARE_NAPI_FUNCTION("onDownloadFinish", JS_DownloadDidFinish),
        DECLARE_NAPI_FUNCTION("onDownloadFailed", JS_DownloadDidFail),
    };
    const std::string WEB_DOWNLOAD_DELEGATE = "WebDownloadDelegate";
    napi_value webDownloadDelegateClass = nullptr;
    napi_define_class(env, WEB_DOWNLOAD_DELEGATE.c_str(), WEB_DOWNLOAD_DELEGATE.length(), JS_Constructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &webDownloadDelegateClass);
    napi_set_named_property(env, exports, WEB_DOWNLOAD_DELEGATE.c_str(), webDownloadDelegateClass);

    return exports;
}
} // namespace NWeb
} // namespace OHOS