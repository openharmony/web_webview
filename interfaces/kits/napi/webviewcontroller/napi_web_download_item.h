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

#ifndef NWEB_NAPI_WEB_DOWNLOAD_ITEM_H
#define NWEB_NAPI_WEB_DOWNLOAD_ITEM_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NWeb {
const std::string WEB_DOWNLOAD_ITEMT = "WebDownloadItem";
const std::string WEB_DOWNLOAD_STATE_ENUM_NAME = "WebDownloadState";
const std::string WEB_DOWNLOAD_ERROR_CODE_ENUM_NAME = "WebDownloadErrorCode";

class NapiWebDownloadItem {
public:
    NapiWebDownloadItem() = default;
    ~NapiWebDownloadItem() = default;

    static napi_value Init(napi_env env, napi_value exports);
    static napi_status DefineProperties(napi_env, napi_value *object);
    static void ExportWebDownloadItemClass(napi_env, napi_value* exportsPointer);
    static void ExportWebDownloadStateEnum(napi_env, napi_value* exportsPointer);
    static void ExportWebDownloadErrorCodeEnum(napi_env, napi_value* exportsPointer);

    static napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetGuid(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetCurrentSpeed(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetPercentComplete(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetTotalBytes(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetReceivedBytes(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetState(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetMimeType(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetLastErrorCode(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetMethod(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetUrl(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetOriginalUrl(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetReferrerUrl(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetSuggestedFileName(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_GetFullPath(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Start(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Continue(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Cancel(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Pause(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Resume(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Serialize(napi_env env, napi_callback_info cbinfo);
    static napi_value JS_Deserialize(napi_env env, napi_callback_info cbinfo);
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEB_DOWNLOAD_ITEM_H
