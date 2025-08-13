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

#include <ani.h>
#include <iostream>
#include <js_native_api.h>
#include <js_native_api_types.h>

#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"
#include "nweb_c_api.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "nweb_log.h"
#include "nweb_precompile_callback.h"
#include "web_download.pb.h"
#include "web_download_delegate.h"
#include "web_download_item.h"
#include "web_errors.h"
#include "webview_controller.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
const char* ANI_CLASS_WEB_DOWNLOAD_ITEM = "L@ohos/web/webview/webview/WebDownloadItem;";
const ani_double ERROR_NUM = -1;
} // namespace
static ani_string GetGuid(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD] WebDownloadItem::GetGuid");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return nullptr;
    }

    std::string guidStr = webDownloadItem->guid;
    ani_string result;
    if (env->String_NewUTF8(guidStr.c_str(), guidStr.size(), &result) != ANI_OK) {
        WVLOG_E("creat stringObj error");
        return nullptr;
    }
    return result;
}

static ani_double GetPercentComplete(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD] WebDownloadItem::GetPercentComplete");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ERROR_NUM;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return ERROR_NUM;
    }
    ani_double value = static_cast<ani_double>(webDownloadItem->percentComplete);
    return value;
}

static ani_string GetSuggestedFileName(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD] WebDownloadItem::GetSuggestedFileName");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return nullptr;
    }

    std::string fileName = webDownloadItem->suggestedFileName;
    ani_string result;
    if (env->String_NewUTF8(fileName.c_str(), fileName.size(), &result) != ANI_OK) {
        WVLOG_E("creat stringObj error");
        return nullptr;
    }
    return result;
}

static ani_string GetMethod(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetMethod webDownloadItem is null");
        return nullptr;
    }

    ani_string methodValue;
    if (env->String_NewUTF8(webDownloadItem->method.c_str(), webDownloadItem->method.size(), &methodValue) != ANI_OK) {
        WVLOG_E("GetMethod error");
        return nullptr;
    }
    return methodValue;
}

static ani_double GetCurrentSpeed(ani_env* env, ani_object object)
{
    int32_t currentSpeed = 0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_double>(currentSpeed);
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetCurrentSpeed webDownloadItem is null");
        return static_cast<ani_double>(currentSpeed);
    }

    currentSpeed = webDownloadItem->currentSpeed;
    return static_cast<ani_double>(currentSpeed);
}

static ani_double GetReceivedBytes(ani_env* env, ani_object object)
{
    int64_t receivedBytes = 0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_double>(receivedBytes);
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetReceivedBytes webDownloadItem is null");
        return static_cast<ani_double>(receivedBytes);
    }

    receivedBytes = webDownloadItem->receivedBytes;
    return static_cast<ani_double>(receivedBytes);
}

static ani_string GetFullPath(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetFullPath webDownloadItem is null");
        return nullptr;
    }

    ani_string fullPath;
    if (env->String_NewUTF8(webDownloadItem->fullPath.c_str(), webDownloadItem->fullPath.size(), &fullPath) != ANI_OK) {
        WVLOG_E("GetFullPath error");
        return nullptr;
    }
    return fullPath;
}

static ani_double GetTotalBytes(ani_env* env, ani_object object)
{
    int64_t totalBytes = -1;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_double>(totalBytes);
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetTotalBytes webDownloadItem is null");
        return static_cast<ani_double>(totalBytes);
    }

    totalBytes = static_cast<int64_t>(webDownloadItem->totalBytes);
    return static_cast<ani_double>(totalBytes);
}

static ani_enum_item GetLastErrorCode(ani_env* env, ani_object object)
{
    ani_enum_item errorCode = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return errorCode;
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetLastErrorCode webDownloadItem is null");
        return errorCode;
    }

    ani_enum enumType;
    ani_status status = env->FindEnum("L@ohos/web/webview/webview/WebDownloadErrorCode;", &enumType);
    if (status != ANI_OK) {
        return errorCode;
    }

    ani_int errorCodeTemp = static_cast<ani_int>(webDownloadItem->lastErrorCode);
    if (env->Enum_GetEnumItemByIndex(enumType, errorCodeTemp, &errorCode) != ANI_OK) {
        WVLOG_E("GetEnum error");
        return errorCode;
    }
    return errorCode;
}

static ani_enum_item GetState(ani_env* env, ani_object object)
{
    ani_enum_item state = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return state;
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("GetState webDownloadItem is null");
        return state;
    }

    ani_enum enumType;
    ani_status status = env->FindEnum("L@ohos/web/webview/webview/WebDownloadState;", &enumType);
    if (status != ANI_OK) {
        return state;
    }

    ani_int stateTemp = static_cast<ani_int>(webDownloadItem->state);
    if (env->Enum_GetEnumItemByIndex(enumType, stateTemp, &state) != ANI_OK) {
        WVLOG_E("GetEnum error");
        return state;
    }
    return state;
}

static void Start(ani_env* env, ani_object object, ani_object pathObj)
{
    WVLOG_D("[DOWNLOAD]AniWebDownloadItem::Start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (!AniParseUtils::IsString(env, pathObj)) {
        WVLOG_E("[DOWNLOAD]pathObj is not string");
        return;
    }
    std::string path;
    if (!AniParseUtils::ParseString(env, pathObj, path)) {
        WVLOG_E("[DOWNLOAD]ParseString fail");
        return;
    }

    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return;
    }
    webDownloadItem->hasStarted = true;
    webDownloadItem->downloadPath = path;
    WVLOG_D("AniWebDownloadItem::Start, download_path: %s", webDownloadItem->downloadPath.c_str());
    WebDownload_Continue(webDownloadItem->before_download_callback, webDownloadItem->downloadPath.c_str());
    return;
}

static void Cancel(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD]AniWebDownloadItem::Cancel");
    if (env == nullptr) {
        WVLOG_E("[DOWNLOAD]env is nullptr");
        return;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return;
    }

    if (webDownloadItem->download_item_callback) {
        WebDownload_Cancel(webDownloadItem->download_item_callback);
    } else if (webDownloadItem->before_download_callback) {
        WebDownload_CancelBeforeDownload(webDownloadItem->before_download_callback);
    } else {
        WVLOG_E("[DOWNLOAD]Cancel failed for callback nullptr");
    }
    return;
}

static void Resume(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD]AniWebDownloadItem::Resume");
    if (env == nullptr) {
        WVLOG_E("[DOWNLOAD]env is nullptr");
        return;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return;
    }

    NWebDownloadItemState state = WebDownload_GetItemStateByGuid(webDownloadItem->guid);
    WVLOG_D("[DOWNLOAD] resume state %{public}d", static_cast<int>(state));
    if (state != NWebDownloadItemState::PAUSED) {
        AniBusinessError::ThrowErrorByErrCode(env, DOWNLOAD_NOT_PAUSED);
        return;
    }
    if (webDownloadItem->download_item_callback) {
        WebDownload_Resume(webDownloadItem->download_item_callback);
    } else if (webDownloadItem->before_download_callback) {
        WebDownload_ResumeBeforeDownload(webDownloadItem->before_download_callback);
    } else {
        WVLOG_E("[DOWNLOAD] NapiWebDownloadItem::JS_Resume failed for callback nullptr");
    }
    return;
}

static void Pause(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD]AniWebDownloadItem::Pause");
    if (env == nullptr) {
        WVLOG_E("[DOWNLOAD]env is nullptr");
        return;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return;
    }

    NWebDownloadItemState state = WebDownload_GetItemStateByGuid(webDownloadItem->guid);
    WVLOG_D("[DOWNLOAD] Pause state %{public}d", static_cast<int>(state));
    if (state != NWebDownloadItemState::IN_PROGRESS && state != NWebDownloadItemState::PENDING) {
        AniBusinessError::ThrowErrorByErrCode(env, DOWNLOAD_NOT_START);
        return;
    }
    if (webDownloadItem->download_item_callback) {
        WebDownload_Pause(webDownloadItem->download_item_callback);
    } else if (webDownloadItem->before_download_callback) {
        WebDownload_PauseBeforeDownload(webDownloadItem->before_download_callback);
    } else {
        WVLOG_E("[DOWNLOAD] NapiWebDownloadItem::JS_Pause failed for callback nullptr");
    }
    return;
}

void SetWebDownloadPb(browser_service::WebDownload& webDownloadPb, const WebDownloadItem* webDownloadItem)
{
    webDownloadPb.set_web_download_id(webDownloadItem->webDownloadId);
    webDownloadPb.set_current_speed(webDownloadItem->currentSpeed);
    webDownloadPb.set_percent_complete(webDownloadItem->percentComplete);
    webDownloadPb.set_total_bytes(webDownloadItem->totalBytes);
    webDownloadPb.set_received_bytes(webDownloadItem->receivedBytes);
    webDownloadPb.set_guid(webDownloadItem->guid);
    webDownloadPb.set_full_path(webDownloadItem->fullPath);
    webDownloadPb.set_url(webDownloadItem->url);
    webDownloadPb.set_etag(webDownloadItem->etag);
    webDownloadPb.set_original_url(webDownloadItem->originalUrl);
    webDownloadPb.set_suggested_file_name(webDownloadItem->suggestedFileName);
    webDownloadPb.set_content_disposition(webDownloadItem->contentDisposition);
    webDownloadPb.set_mime_type(webDownloadItem->mimeType);
    webDownloadPb.set_last_modified(webDownloadItem->lastModified);
    webDownloadPb.set_state(static_cast<browser_service::WebDownload::WebDownloadState>(webDownloadItem->state));
    webDownloadPb.set_method(webDownloadItem->method);
    webDownloadPb.set_last_error_code(webDownloadItem->lastErrorCode);
    webDownloadPb.set_received_slices(webDownloadItem->receivedSlices);
    webDownloadPb.set_download_path(webDownloadItem->downloadPath);
}

static ani_object SerializeInternal(ani_env* env, ani_object object)
{
    WVLOG_D("[DOWNLOAD]AniWebDownloadItem::Serialize");
    if (env == nullptr) {
        WVLOG_E("[DOWNLOAD]env is nullptr");
        return nullptr;
    }
    auto* webDownloadItem = reinterpret_cast<WebDownloadItem*>(AniParseUtils::Unwrap(env, object));
    if (!webDownloadItem) {
        WVLOG_E("[DOWNLOAD]unwrap webDownloadItem failed");
        return nullptr;
    }
    browser_service::WebDownload webDownloadPb;
    SetWebDownloadPb(webDownloadPb, webDownloadItem);
    std::string webDownloadValue;
    webDownloadPb.SerializeToString(&webDownloadValue);
    WVLOG_D("[DOWNLOAD] webDownloadValue.c_str() is %{public}s", webDownloadValue.c_str());
    size_t length = webDownloadValue.length();

    ani_class cls;
    ani_status status = env->FindClass("Lescompat/ArrayBuffer;", &cls);
    if (status != ANI_OK) {
        WVLOG_E("[DOWNLOAD] Find class %{public}s failed, status is %{public}d.", "Lescompat/ArrayBuffer", status);
        return nullptr;
    }

    ani_method ctor;
    status = env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor);
    if (status != ANI_OK) {
        WVLOG_E("[DOWNLOAD] Find function %{public}s failed, status is %{public}d.", "<ctor>", status);
        return nullptr;
    }

    ani_object arrayBufferObj = nullptr;
    status = env->Object_New(cls, ctor, &arrayBufferObj, length);
    if (status != ANI_OK) {
        WVLOG_E("[DOWNLOAD] Object_New failed, status is %{public}d.", status);
        return nullptr;
    }

    for (size_t i = 0; i < length; i++) {
        ani_int value = webDownloadValue[i];
        status = env->Object_CallMethodByName_Void(
            arrayBufferObj, "set", "IB:V", static_cast<ani_int>(i), static_cast<ani_byte>(value));
        if (status != ANI_OK) {
            WVLOG_E("[DOWNLOAD]arrayBufferObj set() failed, status is %{public}d.", status);
            break;
        }
    }
    return arrayBufferObj;
}

void GetWebDownloadPb(const browser_service::WebDownload& webDownloadPb, WebDownloadItem* webDownloadItem)
{
    webDownloadItem->webDownloadId = webDownloadPb.web_download_id();
    webDownloadItem->currentSpeed = webDownloadPb.current_speed();
    webDownloadItem->percentComplete = webDownloadPb.percent_complete();
    webDownloadItem->totalBytes = webDownloadPb.total_bytes();
    webDownloadItem->receivedBytes = webDownloadPb.received_bytes();
    webDownloadItem->guid = webDownloadPb.guid();
    webDownloadItem->fullPath = webDownloadPb.full_path();
    webDownloadItem->url = webDownloadPb.url();
    webDownloadItem->etag = webDownloadPb.etag();
    webDownloadItem->originalUrl = webDownloadPb.original_url();
    webDownloadItem->suggestedFileName = webDownloadPb.suggested_file_name();
    webDownloadItem->contentDisposition = webDownloadPb.content_disposition();
    webDownloadItem->mimeType = webDownloadPb.mime_type();
    webDownloadItem->lastModified = webDownloadPb.last_modified();
    webDownloadItem->state = static_cast<NWebDownloadItemState>(webDownloadPb.state());
    webDownloadItem->method = webDownloadPb.method();
    webDownloadItem->lastErrorCode = webDownloadPb.last_error_code();
    webDownloadItem->receivedSlices = webDownloadPb.received_slices();
    webDownloadItem->downloadPath = webDownloadPb.download_path();
}

static ani_object DeserializeInternal(ani_env* env, ani_object object, ani_object arrayObject)
{
    WVLOG_D("[DOWNLOAD]AniWebDownloadItem::Deserialize");
    if (env == nullptr) {
        WVLOG_E("[DOWNLOAD]env is nullptr");
        return nullptr;
    }

    ani_int length;
    ani_status status = env->Object_CallMethodByName_Int(arrayObject, "getByteLength", ":I", &length);
    if (status != ANI_OK || length <= 0) {
        WVLOG_E("[DOWNLOAD]Object_CallMethodByName_Int getByteLength() failed, status is %{public}d.", status);
        return nullptr;
    }

    std::string dataStr(length, '\0');
    for (ani_size i = 0; i < length; i++) {
        ani_byte value;
        status = env->Object_CallMethodByName_Byte(arrayObject, "at", "I:B", &value, static_cast<ani_int>(i));
        if (status != ANI_OK) {
            WVLOG_E("[DOWNLOAD]arrayBufferObj at() failed, status is %{public}d.", status);
            return nullptr;
        }
        dataStr[i] = static_cast<char>(value);
    }

    browser_service::WebDownload webDownloadPb;
    bool result = webDownloadPb.ParseFromArray(dataStr.c_str(), dataStr.size());
    if (!result) {
        WVLOG_E("[DOWNLOAD] Unserialize webDownloadItem failed");
        return nullptr;
    }

    WebDownloadItem* webDownloadItem = new WebDownloadItem(env);
    GetWebDownloadPb(webDownloadPb, webDownloadItem);

    ani_object webDownloadUnserialized = {};
    if (AniParseUtils::CreateObjectVoid(env, ANI_CLASS_WEB_DOWNLOAD_ITEM, webDownloadUnserialized) == false) {
        WVLOG_E("CreateObjectVoid failed");
        return nullptr;
    }

    if (!AniParseUtils::Wrap(
            env, webDownloadUnserialized, ANI_CLASS_WEB_DOWNLOAD_ITEM, reinterpret_cast<ani_long>(webDownloadItem))) {
        WVLOG_E("[DOWNLOAD] WebDownloadDelegate wrap failed");
        delete webDownloadItem;
        webDownloadItem = nullptr;
        return nullptr;
    }
    return webDownloadUnserialized;
}

static void Constructor(ani_env* env, ani_object object)
{
    WVLOG_D("AniWebDownloadItem native Constructor");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
}

ani_status StsWebDownLoadItemInit(ani_env* env)
{
    WVLOG_D("[DOWNLOAD] StsWebDownLoadItemInit");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class aniCls = nullptr;
    ani_status status = env->FindClass(ANI_CLASS_WEB_DOWNLOAD_ITEM, &aniCls);
    if (status != ANI_OK || !aniCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_CLASS_WEB_DOWNLOAD_ITEM, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(Constructor) },
        ani_native_function { "getGuid", nullptr, reinterpret_cast<void*>(GetGuid) },
        ani_native_function { "getPercentComplete", nullptr, reinterpret_cast<void*>(GetPercentComplete) },
        ani_native_function { "getSuggestedFileName", nullptr, reinterpret_cast<void*>(GetSuggestedFileName) },
        ani_native_function { "getMethod", nullptr, reinterpret_cast<void*>(GetMethod) },
        ani_native_function { "getCurrentSpeed", nullptr, reinterpret_cast<void*>(GetCurrentSpeed) },
        ani_native_function { "getReceivedBytes", nullptr, reinterpret_cast<void*>(GetReceivedBytes) },
        ani_native_function { "getFullPath", nullptr, reinterpret_cast<void*>(GetFullPath) },
        ani_native_function { "getTotalBytes", nullptr, reinterpret_cast<void*>(GetTotalBytes) },
        ani_native_function { "getLastErrorCode", nullptr, reinterpret_cast<void*>(GetLastErrorCode) },
        ani_native_function { "getState", nullptr, reinterpret_cast<void*>(GetState) },
        ani_native_function { "start", nullptr, reinterpret_cast<void*>(Start) },
        ani_native_function { "cancel", nullptr, reinterpret_cast<void*>(Cancel) },
        ani_native_function { "resume", nullptr, reinterpret_cast<void*>(Resume) },
        ani_native_function { "pause", nullptr, reinterpret_cast<void*>(Pause) },
        ani_native_function { "serializeInternal", nullptr, reinterpret_cast<void*>(SerializeInternal) },
        ani_native_function { "deserializeInternal", nullptr, reinterpret_cast<void*>(DeserializeInternal) },
    };
    status = env->Class_BindNativeMethods(aniCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
