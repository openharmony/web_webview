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
#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_geolocation_permission.h"
#include "napi_native_media_player.h"
#include "napi_user_agent_brand_version.h"
#include "napi_user_agent_metadata.h"
#include "napi_web_adsblock_manager.h"
#include "napi_web_async_controller.h"
#include "napi_webview_controller.h"
#include "napi_webview_function.h"
#include "napi_web_cookie_manager.h"
#include "napi_web_data_base.h"
#include "napi_web_storage.h"

#include "napi_web_download_manager.h"
#include "napi_web_download_item.h"
#include "napi_web_download_delegate.h"
#include "napi_web_scheme_handler_request.h"
#include "napi_back_forward_cache_options.h"
#include "napi_prefetch_options.h"

#include "napi_proxy_controller.h"
#include "napi_proxy_config.h"
#include "napi_proxy_rule.h"

namespace OHOS {
namespace NWeb {
EXTERN_C_START
static napi_value WebViewExport(napi_env env, napi_value exports)
{
    NapiWebDataBase::Init(env, exports);
    NapiWebStorage::Init(env, exports);
    NapiWebAsyncController::Init(env, exports);
    NapiWebviewController::Init(env, exports);
    NapiWebCookieManager::Init(env, exports);
    NapiGeolocationPermission::Init(env, exports);
    NapiWebDownloadItem::Init(env, exports);
    NapiWebDownloadManager::Init(env, exports);
    NapiWebDownloadDelegate::Init(env, exports);
    NapiWebSchemeHandlerRequest::Init(env, exports);
    NapiWebSchemeHandlerResponse::Init(env, exports);
    NapiWebSchemeHandler::Init(env, exports);
    NapiWebAdsBlockManager::Init(env, exports);
    WebFunctionInit(env, exports);
    NapiNativeMediaPlayerHandler::Init(env, exports);
    NapiBackForwardCacheOptions::Init(env, exports);
    NapiWebPrefetchOptions::Init(env, exports);
    NapiBackForwardCacheSupportedFeatures::Init(env, exports);
    NapiProxyController::Init(env, exports);
    NapiProxyConfig::Init(env, exports);
    NapiProxyRule::Init(env, exports);
    NapiUserAgentBrandVersion::Init(env, exports);
    NapiUserAgentMetadata::Init(env, exports);
    return exports;
}
EXTERN_C_END

/*
 * module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = WebViewExport,
    .nm_modname = "web.webview",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * module register
 */
extern "C" __attribute__((constructor)) void Register()
{
    napi_module_register(&_module);
}
} // namespace NWeb
} // namesapce OHOS
