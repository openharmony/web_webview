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

#include "ani_webview_controller.h"

#include "ani_business_error.h"
#include "ani_class_name.h"
#include "ani_parse_utils.h"
#include "nweb_init_params.h"

#include <cstdlib>
#include <ctime>
#include <memory>
#include <unordered_map>
#include <securec.h>
#include <regex>

#include "application_context.h"
#include "ohos_resource_adapter_impl.h"

#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"

#include "nweb_log.h"
#include "web_errors.h"
#include "webview_createpdf_execute_callback.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"

#include "nweb_precompile_callback.h"
#include "nweb_cache_options_impl.h"

#include "bundle_mgr_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "parameters.h"
#include "system_ability_definition.h"
#include "nweb_c_api.h"
#include "web_download_delegate.h"
#include "web_download_manager.h"
#include "pixel_map.h"
#include "pixel_map_ani.h"
#include "pixel_map_taihe_ani.h"
#include "proxy_config.h"
#include "web_scheme_handler_response.h"
#include "web_download_item.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
namespace {
constexpr int32_t RESULT_COUNT = 2;
ani_vm *g_vm = nullptr;
constexpr size_t MAX_URL_TRUST_LIST_STR_LEN = 10 * 1024 * 1024; // 10M
constexpr uint32_t SOCKET_MAXIMUM = 6;
constexpr size_t MAX_RESOURCES_COUNT = 30;
constexpr uint32_t URL_MAXIMUM = 2048;
constexpr char URL_REGEXPR[] = "^http(s)?:\\/\\/.+";
struct SnapshotOptions {
    std::string id;
    int32_t width = 0;
    int32_t height = 0;
    PixelUnit widthType = PixelUnit::NONE;
    PixelUnit heightType = PixelUnit::NONE;
    PixelUnit sizeType = PixelUnit::NONE;
};

bool ParsePrepareUrl(ani_env* env, ani_ref urlObj, std::string& url)
{
    if (AniParseUtils::ParseString(env, urlObj, url)) {
        if (url.size() > URL_MAXIMUM) {
            WVLOG_E("The URL exceeds the maximum length of %{public}d", URL_MAXIMUM);
            return false;
        }

        if (!regex_match(url, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            WVLOG_E("ParsePrepareUrl error");
            return false;
        }
        return true;
    }

    WVLOG_E("Unable to parse type from url object.");
    return false;
}

bool ParsePrepareRequestMethod(ani_env* env, ani_ref methodObj, std::string& method)
{
    if (AniParseUtils::ParseString(env, methodObj, method)) {
        if (method != "POST") {
            WVLOG_E("The method %{public}s is not supported.", method.c_str());
            return false;
        }
        return true;
    }

    WVLOG_E("Unable to parse type from method object.");
    return false;
}

bool CheckCacheKey(ani_env* env, const std::string& cacheKey)
{
    for (char c : cacheKey) {
        if (!isalnum(c)) {
            WVLOG_E("AniBusinessError: 401. The character of 'cacheKey' must be number or letters.");
            AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
            return false;
        }
    }
    return true;
}

bool ParseResourceRawfileUrl(ani_env *env, const ani_object& object, std::string& fileName)
{
    ani_ref paramsRef;
    if (env->Object_GetPropertyByName_Ref(object, "params", &paramsRef) != ANI_OK) {
        return false;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(paramsRef, &isUndefined) != ANI_OK || isUndefined) {
        return false;
    }
    ani_object paramsObj = reinterpret_cast<ani_object>(paramsRef);
    ani_double paramsLength;
    if (env->Object_GetPropertyByName_Double(paramsObj, "length", &paramsLength) != ANI_OK) {
        return false;
    }
    ani_ref fileNameRef;
    if (env->Object_CallMethodByName_Ref(paramsObj, "$_get", "I:Lstd/core/Object;", &fileNameRef, 0) != ANI_OK) {
        return false;
    }
    if (env->Reference_IsUndefined(fileNameRef, &isUndefined) != ANI_OK || isUndefined) {
        return false;
    }
    if (!AniParseUtils::ParseString(env, fileNameRef, fileName) || fileName.empty()) {
        return false;
    }
    return true;
}

bool ParseResourceUrl(ani_env *env, ani_object urlObject, std::string& url, WebviewController *controller)
{
    ani_ref bundleNameRef;
    if (env->Object_GetPropertyByName_Ref(urlObject, "bundleName", &bundleNameRef) != ANI_OK) {
        return false;
    }
    ani_ref moduleNameRef;
    if (env->Object_GetPropertyByName_Ref(urlObject, "moduleName", &moduleNameRef) != ANI_OK) {
        return false;
    }
    ani_ref typeRef;
    if (env->Object_GetPropertyByName_Ref(urlObject, "type", &typeRef) != ANI_OK) {
        return false;
    }
    double typeDouble;
    if (env->Object_CallMethodByName_Double(static_cast<ani_object>(typeRef), "unboxed", ":D",
                                            &typeDouble) != ANI_OK) {
        return false;
    }
    std::string bundleName;
    std::string moduleName;
    if (!AniParseUtils::ParseString(env, bundleNameRef, bundleName) ||
        !AniParseUtils::ParseString(env, moduleNameRef, moduleName)) {
        return false;
    }
    int type = static_cast<int>(std::round(typeDouble));
    if (type == static_cast<int>(ResourceType::RAWFILE)) {
        std::string fileName;
        if (!ParseResourceRawfileUrl(env, urlObject, fileName)) {
            return false;
        }
        if (!controller->GetRawFileUrl(fileName, bundleName, moduleName, url)) {
            return false;
        }
    } else if (type == static_cast<int>(ResourceType::STRING)) {
        ani_double idProperty;
        if (env->Object_GetPropertyByName_Double(urlObject, "id", &idProperty) != ANI_OK) {
            return false;
        }
        int id = static_cast<int>(std::round(idProperty));
        if (!OhosResourceAdapterImpl::GetResourceString(bundleName, moduleName, id, url)) {
            return false;
        }
    } else {
        return false;
    }
    WVLOG_I("ParseResourceUrl url: %{public}s", url.c_str());
    return true;
}
}

static void InitializeWebEngine(ani_env *env, ani_object aniClass)
{
    WVLOG_D("InitializeWebEngine invoked.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    // obtain bundle path
    std::shared_ptr<AbilityRuntime::ApplicationContext> ctx =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!ctx) {
        WVLOG_E("Failed to init web engine due to nil application context.");
        return;
    }

    // load so
    const std::string& bundlePath = ctx->GetBundleCodeDir();
    NWebHelper::Instance().SetBundlePath(bundlePath);
    if (!NWebHelper::Instance().InitAndRun(true)) {
        WVLOG_E("Failed to init web engine due to NWebHelper failure.");
    }
}

static void SetNWebId(ani_env *env, ani_object object, ani_double nwebId)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("webviewController is nullptr.");
        return;
    }
    auto id = static_cast<int>(std::round(nwebId));
    WVLOG_I("SetNWebId nwebId: %{public}d", id);
    controller->SetWebId(id);
}

static void SetHapPath(ani_env *env, ani_object object, ani_string hapPath)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined;
    if (env->Reference_IsUndefined(hapPath, &isUndefined) != ANI_OK || isUndefined) {
        WVLOG_E("is undefined");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("Wrap webviewController failed. WebviewController must be associated with a Web component.");
        return;
    }
    std::string hapPathStr;
    if (!AniParseUtils::ParseString(env, hapPath, hapPathStr)) {
        WVLOG_E("Parse hap path failed.");
        return;
    }
    WVLOG_I("SetHapPath hapPath: %{public}s", hapPathStr.c_str());
    controller->InnerSetHapPath(hapPathStr);
}

static bool GetUrl(ani_env *env, ani_object urlObject, std::string& url, WebviewController *controller)
{
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(urlObject, &isUndefined) != ANI_OK || isUndefined) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_NUMBERS_ERROR_TWO, "one", "two"));
        return false;
    }
    if (AniParseUtils::IsString(env, urlObject)) {
        if (!AniParseUtils::ParseString(env, urlObject, url)) {
            return false;
        }
    } else if (AniParseUtils::IsResource(env, urlObject)) {
        if (!ParseResourceUrl(env, urlObject, url, controller)) {
            AniBusinessError::ThrowErrorByErrCode(env, INVALID_URL);
            return false;
        }
    } else {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "url"));
        return false;
    }
    return true;
}

static bool GetWebHeaders(ani_env *env, ani_object headersArrayObj, std::map<std::string, std::string>& webHeaders)
{
    ani_double headersLength;
    if (env->Object_GetPropertyByName_Double(headersArrayObj, "length", &headersLength) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return false;
    }
    ani_class webHeaderClass;
    if (env->FindClass(ANI_CLASS_WEB_HEADER, &webHeaderClass) != ANI_OK) {
        WVLOG_E("Find WebHeader Class failed");
        return false;
    }
    for (int i = 0; i < int(headersLength); i++) {
        ani_ref webHeaderRef;
        if (env->Object_CallMethodByName_Ref(headersArrayObj, "$_get", "I:Lstd/core/Object;",
                                             &webHeaderRef, (ani_int)i) != ANI_OK) {
            return false;
        }
        ani_object webHeaderObj = reinterpret_cast<ani_object>(webHeaderRef);
        ani_boolean isWebHeader = false;
        env->Object_InstanceOf(webHeaderObj, webHeaderClass, &isWebHeader);
        if (!isWebHeader) {
            WVLOG_E("not WebHeader");
            return false;
        }
        ani_ref keyPropertyRef;
        if (env->Object_GetPropertyByName_Ref(webHeaderObj, "headerKey", &keyPropertyRef) != ANI_OK) {
            continue;
        }
        ani_ref valuePropertyRef;
        if (env->Object_GetPropertyByName_Ref(webHeaderObj, "headerValue", &valuePropertyRef) != ANI_OK) {
            continue;
        }
        std::string keyString;
        std::string valueString;
        if (!AniParseUtils::ParseString(env, keyPropertyRef, keyString) ||
            !AniParseUtils::ParseString(env, valuePropertyRef, valueString)) {
            continue;
        }
        webHeaders[keyString] = valueString;
    }
    return true;
}

static void LoadUrl(ani_env *env, ani_object object, ani_object urlObject, ani_object headersArrayObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    std::string url;
    if (!GetUrl(env, urlObject, url, controller)) {
        return;
    }
    ErrCode ret;
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(headersArrayObj, &isUndefined) != ANI_OK || isUndefined) {
        ret = controller->LoadUrl(url);
    } else {
        std::map<std::string, std::string> webHeaders;
        if (!GetWebHeaders(env, headersArrayObj, webHeaders)) {
            return;
        }
        ret = controller->LoadUrl(url, webHeaders);
    }
    WVLOG_I("loadUrl ret: %{public}d", ret);
    if (ret != NO_ERROR && ret != NWEB_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}

static void Clean(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_long ptr;
    ani_status status = env->Object_GetFieldByName_Long(object, "ptr", &ptr);
    if (status != ANI_OK) {
        WVLOG_E("Clean Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }

    ani_ref nameObj = nullptr;
    if ((status = env->Object_GetFieldByName_Ref(object, "name", &nameObj)) != ANI_OK) {
        WVLOG_E("Clean Object_GetFieldByName_Ref status: %{public}d", status);
        return;
    }
    std::string clsName = "";
    if (!AniParseUtils::ParseString(env, nameObj, clsName)) {
        WVLOG_E("Clean ParseString failed.");
        return;
    }
    if (clsName == "WebviewController") {
        delete reinterpret_cast<WebviewController *>(ptr);
    } else if (clsName == "WebHistoryList") {
        delete reinterpret_cast<WebHistoryList *>(ptr);
    } else if (clsName == "ProxyConfig") {
        delete reinterpret_cast<ProxyConfig *>(ptr);
    } else if (clsName == "WebSchemeHandlerResponse") {
        delete reinterpret_cast<WebSchemeHandlerResponse *>(ptr);
    } else if (clsName == "WebDownloadDelegate") {
        delete reinterpret_cast<WebDownloadDelegate *>(ptr);
    } else {
        WVLOG_E("Clean unsupport className: %{public}s", clsName.c_str());
    }
    return;
}

static void Constructor(ani_env *env, ani_object object, ani_string webTagObject)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = true;
    env->Reference_IsUndefined(webTagObject, &isUndefined);
    std::string webTag;
    if (isUndefined) {
        webTag = WebviewController::GenerateWebTag();
    } else {
        if (!AniParseUtils::ParseString(env, webTagObject, webTag)) {
            return;
        }
        WVLOG_I("new webview controller webname:%{public}s", webTag.c_str());
    }
    WebviewController* controller = new (std::nothrow) WebviewController(webTag);
    if (controller == nullptr) {
        WVLOG_E("new webview controller failed");
        return;
    }
    WebviewController::webTagSet_.insert(webTag);
    if (!AniParseUtils::Wrap(env, object, ANI_CLASS_WEBVIEW_CONTROLLER, reinterpret_cast<ani_long>(controller))) {
        WVLOG_E("webview controller wrap failed");
        delete controller;
        controller = nullptr;
    }
}

static void OnActive(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->OnActive();
}

static void OnInactive(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->OnInactive();
}

static ani_double GetWebId(ani_env *env, ani_object object)
{
    int32_t webId = -1;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_double>(webId);
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return static_cast<ani_double>(webId);
    }

    webId = controller->GetWebId();
    return static_cast<ani_double>(webId);
}

static ani_boolean GetScrollable(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_TRUE;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_TRUE;
    }

    return static_cast<ani_boolean>(controller->GetScrollable());
}

static void RequestFocus(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->RequestFocus();
}

static void clearClientAuthenticationCache(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->ClearClientAuthenticationCache();   
}

static ani_boolean ScrollByWithResult(ani_env *env, ani_object object, ani_double deltaX, ani_double deltaY)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }
    return static_cast<ani_boolean>(
        controller->ScrollByWithResult(static_cast<float>(deltaX), static_cast<float>(deltaY)));
}
static void SetScrollable(ani_env *env, ani_object object, ani_boolean enable, ani_enum_item type)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    int32_t scrollType = -1;
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(type, &isUndefined);
    if (isUndefined == ANI_FALSE) {
        ani_int iType;
        if (env->EnumItem_GetValue_Int(type, &iType) != ANI_OK) {
            WVLOG_E("BusinessError: 401. The character of 'scrollType' must be int32.");
            AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
            return;
        }
        scrollType = static_cast<int32_t>(iType);
    }
    controller->SetScrollable(static_cast<bool>(enable), scrollType);
}
static void ScrollTo(ani_env *env, ani_object object, ani_double x, ani_double y, ani_object durationObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(durationObj, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        controller->ScrollTo(static_cast<float>(x), static_cast<float>(y));
    } else {
        ani_double duration;
        if (env->Object_CallMethodByName_Double(durationObj, "doubleValue", nullptr, &duration) != ANI_OK) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "duration", "number"));
            return;
        }
        controller->ScrollToWithAnime(
            static_cast<float>(x), static_cast<float>(y), static_cast<int32_t>(duration));
    }
}
static void ScrollBy(ani_env *env, ani_object object, ani_double deltaX, ani_double deltaY, ani_object durationObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(durationObj, &isUndefined);
    if (isUndefined == ANI_TRUE) {
        controller->ScrollBy(static_cast<float>(deltaX), static_cast<float>(deltaY));
    } else {
        ani_double duration;
        if (env->Object_CallMethodByName_Double(durationObj, "doubleValue", nullptr, &duration) != ANI_OK) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "duration", "number"));
            return;
        }
        controller->ScrollByWithAnime(
            static_cast<float>(deltaX), static_cast<float>(deltaY), static_cast<int32_t>(duration));
    }
}
static ani_object GetScrollOffset(ani_env *env, ani_object object)
{
    ani_object offset = {};
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return offset;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return offset;
    }
    float offsetX = 0;
    float offsetY = 0;
    controller->GetScrollOffset(&offsetX, &offsetY);

    if (AniParseUtils::CreateObjectVoid(env, ANI_CLASS_SCROLL_OFFSET_INNER, offset)) {
        env->Object_SetPropertyByName_Double(offset, "x", static_cast<ani_double>(offsetX));
        env->Object_SetPropertyByName_Double(offset, "y", static_cast<ani_double>(offsetY));
    }
    return offset;
}
static void SlideScroll(ani_env *env, ani_object object, ani_double vx, ani_double vy)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->SlideScroll(static_cast<float>(vx), static_cast<float>(vy));
}

static void PageDown(ani_env *env, ani_object object, ani_boolean bottom)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->ScrollPageDown(static_cast<bool>(bottom));
}
static void PageUp(ani_env *env, ani_object object, ani_boolean top)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->ScrollPageUp(static_cast<bool>(top));
}

static ani_boolean IsAdsBlockEnabledForCurPage(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        return ANI_FALSE;
    }
    if(!controller->IsAdsBlockEnabledForCurPage()) {
        WVLOG_E("IsAdsBlockEnabledForCurPage failed.");
        return ANI_FALSE;
    }
    return ANI_TRUE;
}

static ani_boolean IsIntelligentTrackingPreventionEnabled(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        return ANI_FALSE;
    }
    if(!controller->IsIntelligentTrackingPreventionEnabled()) {
        WVLOG_E("IsIntelligentTrackingPreventionEnabled failed.");
        return ANI_FALSE;
    }
    return ANI_TRUE;
}

static void AddIntelligentTrackingPreventionBypassingList(ani_env *env, ani_object object, ani_object stringArrayObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::vector<std::string> hosts;
    if(!AniParseUtils::GetStringList(env, stringArrayObj, hosts)) {
        WVLOG_E("GetStringList failed.");
        return;
    }
    NWebHelper::Instance().AddIntelligentTrackingPreventionBypassingList(hosts);
    return;
}

static void RemoveIntelligentTrackingPreventionBypassingList(ani_env *env, ani_object object, 
    ani_object stringArrayObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::vector<std::string> hosts;
    if(!AniParseUtils::GetStringList(env, stringArrayObj, hosts)) {
        WVLOG_E("GetStringList failed.");
        return;
    }
    NWebHelper::Instance().RemoveIntelligentTrackingPreventionBypassingList(hosts);
    return;
}

static void ClearIntelligentTrackingPreventionBypassingList(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_I("Clear intelligent tracking prevention bypassing list.");
    NWebHelper::Instance().ClearIntelligentTrackingPreventionBypassingList();
    return;
}
static void SetHostIP(ani_env *env, ani_object object, ani_object hostNameObj, ani_object addressObj, 
    ani_int aliveTime)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string hostName;
    if (!AniParseUtils::ParseString(env, hostNameObj, hostName)) {
        WVLOG_E("Parse host name failed.");
        return;
    }
    std::string address;
    if (!AniParseUtils::ParseIP(env, addressObj, address)) {
        WVLOG_E("Parse address failed.");
        return;
    }
    int aliveTimeInt = static_cast<int32_t>(std::round(aliveTime));
    if (aliveTime <= 0) {
        WVLOG_E("aliveTime must be greater than 0, aliveTime: %{public}d", aliveTimeInt);
        return;
    }
    WVLOG_I("Set host ip: %{public}s, %{public}s, aliveTime: %{public}d", hostName.c_str(), address.c_str(), 
    aliveTimeInt);
    NWebHelper::Instance().SetHostIP(hostName, address, aliveTimeInt);
    return;
}
static void ClearHostIP(ani_env *env, ani_object object, ani_object hostNameObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string hostName;
    if (!AniParseUtils::ParseString(env, hostNameObj, hostName)) {
        WVLOG_E("Parse host name failed.");
        return;
    }
    WVLOG_I("Clear host ip: %{public}s", hostName.c_str());
    NWebHelper::Instance().ClearHostIP(hostName);
    return;
}

static void WarmupServiceWorker(ani_env *env, ani_object object, ani_object urlObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string url;
    if (!AniParseUtils::ParseString(env, urlObj, url)) {
        WVLOG_E("Parse url failed.");
        return;
    }
    WVLOG_I("Warm up Service Worker: %{public}s", url.c_str());
    NWebHelper::Instance().WarmupServiceWorker(url);
    return;
}

static void SetUrlTrustList(ani_env *env, ani_object object, ani_object urlTrustListObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("webview controller is null or not init");
        return;
    }

    std::string urlTrustList;
    if (!AniParseUtils::ParseString(env, urlTrustListObj, urlTrustList)) {
        return;
    }
    if (urlTrustList.size() > MAX_URL_TRUST_LIST_STR_LEN) {
        WVLOG_E("EnableAdsBlock: url trust list len is too large.");
        return;
    }
    std::string detailMsg;
    ErrCode ret = controller->SetUrlTrustList(urlTrustList, detailMsg);
    if (ret != NO_ERROR) {
        WVLOG_E("Set Url Trust List failed, detailMsg: %{public}s", detailMsg.c_str());
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

static void EnableIntelligentTrackingPrevention(ani_env *env, ani_object object, ani_boolean enable)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("controller is nullptr");
        return;
    }
    WVLOG_I("enable/disable intelligent tracking prevention.");
    controller->EnableIntelligentTrackingPrevention(static_cast<bool>(enable));
    return;
}

static void SearchNext(ani_env *env, ani_object object, ani_boolean forward)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->SearchNext(static_cast<bool>(forward));
    return;
}

static ani_string GetDefaultUserAgent(ani_env *env, ani_object object)
{
    ani_string userAgent = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return userAgent;
    }
    WVLOG_I("Get the default user agent.");
    std::string result = NWebHelper::Instance().GetDefaultUserAgent();
    env->String_NewUTF8(result.c_str(), result.size(), &userAgent);
    return userAgent;
}

static ani_boolean IsSafeBrowsingEnabled(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }
    if (!controller->IsSafeBrowsingEnabled()) {
        return ANI_FALSE;
    }
    return ANI_TRUE;
}

static void PrepareForPageLoad(
    ani_env* env, ani_object object, ani_string aniUrl, ani_boolean preconnectable, ani_double aniNumSockets)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string url;
    if (!AniParseUtils::ParseString(env, aniUrl, url)) {
        WVLOG_E("parse url failed");
        return;
    }
    int32_t numSockets = static_cast<int32_t>(std::round(aniNumSockets));
    if (numSockets <= 0 || static_cast<uint32_t>(numSockets) > SOCKET_MAXIMUM) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    NWebHelper::Instance().PrepareForPageLoad(url, preconnectable, numSockets);
    return;
}

static void Zoom(ani_env *env, ani_object object, ani_double factor)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->Zoom(static_cast<float>(factor));
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("Zoom failed.");
            return;
        }
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}
static void ZoomOut(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->ZoomOut();
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("ZoomOut failed.");
            return;
        }
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}
static void ZoomIn(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->ZoomIn();
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("ZoomIn failed.");
            return;
        }
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}

static ani_object GetLastHitTest(ani_env *env, ani_object object)
{
    ani_object hitTestValue = {};
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return hitTestValue;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return hitTestValue;
    }
    std::shared_ptr<HitTestResult> nwebResult = controller->GetLastHitTest();
    if (!AniParseUtils::CreateObjectVoid(env, ANI_CLASS_HIT_TEST_VALUE_INNER, hitTestValue) || !nwebResult) {
        return hitTestValue;
    }
    ani_enum_item eType;
    if (AniParseUtils::GetEnumItemByIndex(env, ANI_ENUM_WEB_HIT_TEST_TYPE, nwebResult->GetType(), eType)) {
        env->Object_SetPropertyByName_Ref(hitTestValue, "type", eType);
    }
    auto extra = nwebResult->GetExtra();
    ani_string aniExtra;
    if (env->String_NewUTF8(extra.c_str(), extra.size(), &aniExtra) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(hitTestValue, "extra", aniExtra);
    }
    return hitTestValue;
}

static void Stop(ani_env *env, ani_object object)
{
    WVLOG_D("[WebviewCotr] Stop");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->Stop();
}

static void clearSslCache(ani_env *env, ani_object object)
{
    WVLOG_D("[WebviewCotr] clearSslCache");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->ClearSslCache();
}

static void clearMatches(ani_env *env, ani_object object)
{
    WVLOG_D("[WebviewCotr] clearMatches");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->ClearMatches();
}

static void Refresh(ani_env *env, ani_object object)
{
    WVLOG_D("[WebviewCotr] Refresh");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->Refresh();
}

static void StartDownload(ani_env *env, ani_object object, ani_string aniUrl)
{
    WVLOG_D("[WebviewCotr] StartDownload");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    std::string urlStr;
    if (!AniParseUtils::ParseString(env, aniUrl, urlStr)) {
        WVLOG_E("Parse url failed.");
        return;
    }
    WVLOG_I("StartDownload url: %{public}s", urlStr.c_str());
    int32_t nwebId = controller->GetWebId();
    NWebHelper::Instance().LoadNWebSDK();
    WebDownloader_StartDownload(nwebId, urlStr.c_str());
}

static void SetDownloadDelegate(ani_env *env, ani_object object, ani_object aniDelegate)
{
    WVLOG_D("[WebviewCotr] SetDownloadDelegate");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    NWebHelper::Instance().LoadNWebSDK();

    WebDownloadDelegate* delegate = nullptr;
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(aniDelegate, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    delegate = reinterpret_cast<WebDownloadDelegate *>(thisVar);
    if (delegate == nullptr) {
        WVLOG_E("WebDownloadDelegate is null.");
        return;
    }
    int32_t nwebId = controller->GetWebId();
    WebDownloadManager::AddDownloadDelegateForWeb(nwebId, delegate);
}

Media::PixelFormat getColorType(ImageColorType colorType)
{
    Media::PixelFormat pixelFormat_;
    switch (colorType) {
        case ImageColorType::COLOR_TYPE_UNKNOWN:
            pixelFormat_ = Media::PixelFormat::UNKNOWN;
            break;
        case ImageColorType::COLOR_TYPE_RGBA_8888:
            pixelFormat_ = Media::PixelFormat::RGBA_8888;
            break;
        case ImageColorType::COLOR_TYPE_BGRA_8888:
            pixelFormat_ = Media::PixelFormat::BGRA_8888;
            break;
        default:
            pixelFormat_ = Media::PixelFormat::UNKNOWN;
            break;
    }
    return pixelFormat_;
}

Media::AlphaType getAlphaType(ImageAlphaType alphaType)
{
    Media::AlphaType alphaType_;
    switch (alphaType) {
        case ImageAlphaType::ALPHA_TYPE_UNKNOWN:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
            break;
        case ImageAlphaType::ALPHA_TYPE_OPAQUE:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
            break;
        case ImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
            break;
        case ImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
            break;
        default:
            alphaType_ = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
            break;
    }
    return alphaType_;
}

static ani_ref GetFaviconByHistoryItem(ani_env *env, std::shared_ptr<NWebHistoryItem> item)
{
    if (env == nullptr || !item) {
        WVLOG_E("[PIXELMAP] env or NWebHistoryItem is nullptr");
        return nullptr;
    }

    ani_ref result = nullptr;
    void *data = nullptr;
    int32_t width = 0;
    int32_t height = 0;
    ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
    ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
    bool isGetFavicon = item->GetFavicon(&data, width, height, colorType, alphaType);
    if (env->GetNull(&result) != ANI_OK) {
        WVLOG_E("[PIXELMAP] GetNull fail");
        return nullptr;
    }
    if (!isGetFavicon) {
        WVLOG_E("[PIXELMAP] isGetFavicon is false");
        return result;
    }

    Media::InitializationOptions opt;
    opt.size.width = width;
    opt.size.height = height;
    opt.pixelFormat = getColorType(colorType);
    opt.alphaType = getAlphaType(alphaType);
    opt.editable = true;
    auto pixelMap = Media::PixelMap::Create(opt);
    if (pixelMap == nullptr) {
         WVLOG_E("[PIXELMAP] pixelMap is null");
        return result;
    }
    uint64_t stride = static_cast<uint64_t>(width) << 2;
    uint64_t bufferSize = stride * static_cast<uint64_t>(height);
    pixelMap->WritePixels(static_cast<const uint8_t *>(data), bufferSize);
    std::shared_ptr<Media::PixelMap> pixelMapToJs(pixelMap.release());
    ani_object jsPixelMap = OHOS::Media::PixelMapAni::CreatePixelMap(env, pixelMapToJs);
    return jsPixelMap;
}

static ani_ref GetFavicon(ani_env* env, ani_object object)
{
    ani_ref result = nullptr;
    if (env == nullptr) {
        WVLOG_E("[PIXELMAP] env is nullptr");
        return result;
    }
    auto *controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }
    
    const void *data = nullptr;
    size_t width = 0;
    size_t height = 0;
    ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
    ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
    bool isGetFavicon = controller->GetFavicon(&data, width, height, colorType, alphaType);
    if (!isGetFavicon) {
        return result;
    }

    Media::InitializationOptions opt;
    opt.size.width = width;
    opt.size.height = height;
    opt.pixelFormat = getColorType(colorType);
    opt.alphaType = getAlphaType(alphaType);
    opt.editable = true;
    auto pixelMap = Media::PixelMap::Create(opt);
    if (pixelMap == nullptr) {
        return result;
    }
    uint64_t stride = static_cast<uint64_t>(width) << 2;
    uint64_t bufferSize = stride * static_cast<uint64_t>(height);
    pixelMap->WritePixels(static_cast<const uint8_t *>(data), bufferSize);
    std::shared_ptr<Media::PixelMap> pixelMapToJs(pixelMap.release());
    ani_object jsPixelMap = OHOS::Media::PixelMapAni::CreatePixelMap(env, pixelMapToJs);
    return jsPixelMap;
}

static ani_ref GetBackForwardEntries(ani_env *env, ani_object object)
{
    WVLOG_D("[BACKFORWARD] GetBackForwardEntries");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    std::shared_ptr<NWebHistoryList> list = controller->GetHistoryList();
    if (!list) {
        return nullptr;
    }

    int32_t currentIndex = list->GetCurrentIndex();
    int32_t size = list->GetListSize();
    WebHistoryList *webHistoryList = new (std::nothrow) WebHistoryList(list);
    if (webHistoryList == nullptr) {
        WVLOG_E("[BACKFORWARD] new WebHistoryList failed");
        return nullptr;
    }

    ani_object backForwardObj = {};
    if (AniParseUtils::CreateObjectVoid(env, ANI_BACK_FORWARD_LIST_INNER_CLASS_NAME, backForwardObj) == false) {
        WVLOG_E("[BACKFORWARD] CreateObjectVoid failed");
        delete webHistoryList;
        webHistoryList = nullptr;
        return nullptr;
    }

    if (!AniParseUtils::Wrap(env, backForwardObj, ANI_BACK_FORWARD_LIST_INNER_CLASS_NAME,
                             reinterpret_cast<ani_long>(webHistoryList))) {
        WVLOG_E("[BACKFORWARD] WebDownloadDelegate wrap failed");
        delete webHistoryList;
        webHistoryList = nullptr;
        return nullptr;
    }

    env->Object_SetPropertyByName_Double(backForwardObj, "currentIndex", static_cast<ani_double>(currentIndex));
    env->Object_SetPropertyByName_Double(backForwardObj, "size", static_cast<ani_double>(size));
    return backForwardObj;
}

static void RemoveAllCache(ani_env *env, ani_object object, ani_boolean clearRom)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    NWebHelper::Instance().RemoveAllCache(static_cast<bool>(clearRom));
    return;
}

static void PostUrl(ani_env *env, ani_object object, ani_object urlObj, ani_object arrayBufferObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    std::string url;
    if (!GetUrl(env, urlObj, url, controller)) {
        return;
    }

    char *arrayBuffer = nullptr;
    size_t byteLength = 0;
    if (env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(arrayBufferObj),
                                 reinterpret_cast<void **>(&arrayBuffer), &byteLength) != ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo failed");
        return;
    }
    std::vector<char> postData(arrayBuffer, arrayBuffer + byteLength);

    ErrCode ret = controller->PostUrl(url, postData);
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("PostUrl failed");
            return;
        }
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

static ani_string GetUrlAni(ani_env *env, ani_object object)
{
    ani_string url = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return url;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return url;
    }

    std::string result = controller->GetUrl();
    env->String_NewUTF8(result.c_str(), result.size(), &url);

    return url;
}

static ani_string GetTitle(ani_env *env, ani_object object)
{
    ani_string title = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return title;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return title;
    }

    std::string result = controller->GetTitle();
    env->String_NewUTF8(result.c_str(), result.size(), &title);

    return title;
}

static ani_string GetOriginalUrl(ani_env *env, ani_object object)
{
    ani_string originUrl = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return originUrl;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return originUrl;
    }

    std::string result = controller->GetOriginalUrl();
    env->String_NewUTF8(result.c_str(), result.size(), &originUrl);

    return originUrl;
}

static ani_string GetUserAgent(ani_env *env, ani_object object)
{
    ani_string userAgent = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return userAgent;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return userAgent;
    }

    std::string result = controller->GetUserAgent();
    env->String_NewUTF8(result.c_str(), result.size(), &userAgent);

    return userAgent;
}

static ani_string GetCustomUserAgent(ani_env *env, ani_object object)
{
    ani_string customUserAgent = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return customUserAgent;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return customUserAgent;
    }

    std::string result = controller->GetCustomUserAgent();
    env->String_NewUTF8(result.c_str(), result.size(), &customUserAgent);

    return customUserAgent;
}

static ani_string GetLastJavascriptProxyCallingFrameUrl(ani_env *env, ani_object object)
{
    ani_string lastCallingFrameUrl = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return lastCallingFrameUrl;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return lastCallingFrameUrl;
    }

    std::string result = controller->GetLastJavascriptProxyCallingFrameUrl();
    env->String_NewUTF8(result.c_str(), result.size(), &lastCallingFrameUrl);

    return lastCallingFrameUrl;
}

static void Forward(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->Forward();
}

static void Backward(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->Backward();
}

static ani_boolean AccessForward(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }

    if (!controller->AccessForward()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

static ani_boolean AccessBackward(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }

    if (!controller->AccessBackward()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

static ani_boolean AccessStep(ani_env *env, ani_object object, ani_int step)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if ((!controller)|| !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }
    if (!controller->AccessStep(static_cast<int32_t>(step))) {
        return ANI_FALSE;
    }
    return ANI_TRUE;
}

static ani_boolean IsAdsBlockEnabled(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("controller is nullptr");
        return ANI_FALSE;
    }

    if (!controller->IsAdsBlockEnabled()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

static void EnableAdsBlock(ani_env *env, ani_object object, ani_boolean enable)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("controller is nullptr");
        return;
    }

    controller->EnableAdsBlock(static_cast<bool>(enable));
    return;
}

static void LoadData(ani_env *env, ani_object object, ani_object urlObj, ani_object mimeTypeObj,
                     ani_object encodingObj, ani_object baseUrlObj, ani_object historyUrlObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::string url;
    if (!GetUrl(env, urlObj, url, controller)) {
        return;
    }
    std::string mimeType;
    if (!AniParseUtils::ParseString(env, mimeTypeObj, mimeType)) {
        return;
    }
    std::string encoding;
    if (!AniParseUtils::ParseString(env, encodingObj, encoding)) {
        return;
    }
    std::string baseUrl;
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(baseUrlObj, &isUndefined) != ANI_OK) {
        return;
    }
    if (!isUndefined && !AniParseUtils::ParseString(env, baseUrlObj, baseUrl)) {
        return;
    }
    std::string historyUrl;
    if (env->Reference_IsUndefined(historyUrlObj, &isUndefined) != ANI_OK) {
        return;
    }
    if (!isUndefined && !AniParseUtils::ParseString(env, historyUrlObj, historyUrl)) {
        return;
    }
    ErrCode ret = controller->LoadData(url, mimeType, encoding, baseUrl, historyUrl);
    if (ret != NO_ERROR) {
        if (ret == NWEB_ERROR) {
            WVLOG_E("LoadData failed");
            return;
        }
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

static void ClearHistory(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->ClearHistory();
}

static void ClearWebSchemeHandler(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    if (controller->ClearWebSchemeHandler() != 0) {
        WVLOG_E("AniWebviewController::ClearWebSchemeHandler failed");
    }
    WVLOG_I("AniWebviewController::ClearWebSchemeHandler successful");
}

static ani_ref GetItemAtIndex(ani_env *env, ani_object object, ani_double aniIndex)
{
    if (env == nullptr) {
        WVLOG_E("[BACKFORWARD] env is nullptr");
        return nullptr;
    }

    int32_t index = static_cast<int32_t>(std::round(aniIndex));
    WebHistoryList *historyList = reinterpret_cast<WebHistoryList *>(AniParseUtils::Unwrap(env, object));
    if (!historyList) {
        WVLOG_E("[BACKFORWARD] Unwrap failed");
        return nullptr;
    }

    if (index >= historyList->GetListSize() || index < 0) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "BusinessError 401: Parameter error. The value of index must be greater than or equal to 0");
        return nullptr;
    }

    std::shared_ptr<NWebHistoryItem> item = historyList->GetItem(index);
    if (!item) {
        return nullptr;
    }

    std::string historyUrl = item->GetHistoryUrl();
    std::string historyRawUrl = item->GetHistoryRawUrl();
    std::string title = item->GetHistoryTitle();

    ani_object historyItemListObj = {};
    if (AniParseUtils::CreateObjectVoid(env, ANI_HISTORY_ITEM_INNER_CLASS_NAME, historyItemListObj) == false) {
        WVLOG_E("[BACKFORWARD] CreateObjectVoid failed");
        return nullptr;
    }

    if (AniParseUtils::SetPropertyByName_String(env, historyItemListObj, "historyUrl", historyUrl) != ANI_OK) {
        WVLOG_E("[BACKFORWARD] SetPropertyByName_String failed! key=historyUrl, value=%{public}s.", historyUrl.c_str());
        return historyItemListObj;
    }

    if (AniParseUtils::SetPropertyByName_String(env, historyItemListObj, "historyRawUrl", historyRawUrl) != ANI_OK) {
        WVLOG_E("[BACKFORWARD] SetPropertyByName_String failed! key=historyRawUrl, value=%{public}s.",
                historyRawUrl.c_str());
        return historyItemListObj;
    }

    if (AniParseUtils::SetPropertyByName_String(env, historyItemListObj, "title", title) != ANI_OK) {
        WVLOG_E("[BACKFORWARD] SetPropertyByName_String failed! key=title, value=%{public}s.", title.c_str());
        return historyItemListObj;
    }

    ani_ref iconRef = GetFaviconByHistoryItem(env, item);
    if (env->Object_SetPropertyByName_Ref(historyItemListObj, "icon", iconRef) != ANI_OK) {
        WVLOG_E("[BACKFORWARD] SetPropertyByName_String failed! key=icon.");
        return historyItemListObj;
    }
    return historyItemListObj;
}

ani_status StsBackForwardListInit(ani_env *env)
{
    ani_class backForwardListCls = nullptr;
    ani_status status = env->FindClass(ANI_BACK_FORWARD_LIST_INNER_CLASS_NAME, &backForwardListCls);
    if (status != ANI_OK || !backForwardListCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_BACK_FORWARD_LIST_INNER_CLASS_NAME, status);
        return ANI_ERROR;
    }

    std::array methodArray = {
        ani_native_function { "getItemAtIndex", nullptr, reinterpret_cast<void *>(GetItemAtIndex) },
    };

    status = env->Class_BindNativeMethods(backForwardListCls, methodArray.data(), methodArray.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }

    return ANI_OK;
}

ani_status StsCleanerInit(ani_env *env)
{
    ani_class cleanerCls = nullptr;
    ani_status status = env->FindClass(ANI_CLASS_CLEANER, &cleanerCls);
    if (status != ANI_OK || !cleanerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_CLASS_CLEANER, status);
        return status;
    }
    std::array cleanerMethods = {
        ani_native_function { "clean", nullptr, reinterpret_cast<void *>(Clean) },
    };
    status = env->Class_BindNativeMethods(cleanerCls, cleanerMethods.data(), cleanerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return status;
}

static void SetConnectionTimeout(ani_env* env, ani_object object, ani_double aniTimeout)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    int32_t timeout = static_cast<int32_t>(std::round(aniTimeout));
    if (timeout <= 0) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    NWebHelper::Instance().SetConnectionTimeout(timeout);
    return;
}

static void BackOrForward(ani_env* env, ani_object object, ani_int step)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    ErrCode ret = controller->BackOrForward(step);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
    return;
}

static void SetWebDebuggingAccess(ani_env* env, ani_object object, ani_boolean webDebuggingAccess)
{
    WVLOG_D("[WebviewCotr] SetWebDebuggingAccess");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WebviewController::webDebuggingAccess_ = webDebuggingAccess;
    return;
}

static void EnableSafeBrowsing(ani_env* env, ani_object object, ani_boolean enable)
{
    WVLOG_D("[WebviewCotr] EnableSafeBrowsing");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->EnableSafeBrowsing(enable);
    return;
}

static ani_double GetPageHeight(ani_env *env, ani_object object)
{
    int32_t pageHeight = 0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_double>(pageHeight);
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return static_cast<ani_double>(pageHeight);
    }

    pageHeight = controller->GetPageHeight();
    return static_cast<ani_double>(pageHeight);
}

static ani_boolean TerminateRenderProcess(ani_env* env, ani_object object)
{
    WVLOG_I("TerminateRenderProcess");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }

    if (!controller->TerminateRenderProcess()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

static void SetRenderProcessMode(ani_env* env, ani_object object, ani_enum_item mode)
{
    WVLOG_D("[WebviewCotr] SetRenderProcessMode");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_FALSE;
    if (env->Reference_IsUndefined(mode, &isUndefined) != ANI_OK || isUndefined) {
        return;
    }

    ani_int iMode;
    if (env->EnumItem_GetValue_Int(mode, &iMode) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }

    int32_t renderProcessMode = static_cast<int32_t>(iMode);
    WVLOG_I("SetRenderProcessMode mode: %{public}d", renderProcessMode);
    NWebHelper::Instance().SetRenderProcessMode(static_cast<RenderProcessMode>(renderProcessMode));
}

static ani_enum_item GetRenderProcessMode(ani_env* env, ani_object object)
{
    ani_int renderProcessMode = 0;
    ani_enum enumType;
    env->FindEnum("Lani_enum/COLORINT;", &enumType);

    renderProcessMode = static_cast<ani_int>(NWebHelper::Instance().GetRenderProcessMode());
    WVLOG_I("getRenderProcessMode mode = %{public}d", static_cast<int32_t>(renderProcessMode));
    ani_enum_item mode;
    env->Enum_GetEnumItemByIndex(enumType, renderProcessMode, &mode);
    return mode;
}

static void PauseAllTimers(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_I("PauseAllTimers");
    NWebHelper::Instance().PauseAllTimers();
    return;
}

static void ResumeAllTimers(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_I("ResumeAllTimers");
    NWebHelper::Instance().ResumeAllTimers();
    return;
}

static void SetCustomUserAgent(ani_env* env, ani_object object, ani_object userAgentObj)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string userAgent;
    if (!AniParseUtils::ParseString(env, userAgentObj, userAgent)) {
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->SetCustomUserAgent(userAgent);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}

static void RemoveCache(ani_env* env, ani_object object, ani_boolean clearRom)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->RemoveCache(clearRom);
    return;
}

static void SetNetworkAvailable(ani_env* env, ani_object object, ani_boolean enable)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->PutNetworkAvailable(enable);
    return;
}

static ani_boolean IsIncognitoMode(ani_env *env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }
    if (!controller->IsIncognitoMode()) {
        return ANI_FALSE;
    }
    return ANI_TRUE;
}

static ani_object SerializeWebState(ani_env* env, ani_object object)
{
    ani_object result = nullptr;

    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }

    void* data = nullptr;
    ani_arraybuffer buffer = nullptr;
    auto webState = controller->SerializeWebState();

    size_t length = webState.size();
    env->CreateArrayBuffer(length, &data, &buffer);

    int retCode = memcpy_s(data, length, webState.data(), length);
    if (retCode != 0) {
        return result;
    }

    ani_class cls;
    ani_method ctor;
    if (env->FindClass("Lstd/core/ArrayBuffer;", &cls) != ANI_OK) {
        return result;
    }
    if (env->Class_FindMethod(cls, "<ctor>", "I:V", &ctor) != ANI_OK) {
        return result;
    }

    ani_object arrayObject;
    env->Object_New(cls, ctor, &arrayObject, buffer);
    return arrayObject;
}

static void TrimMemoryByPressureLevel(ani_env *env, ani_object object, ani_double level)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    int32_t memoryLevel = static_cast<int32_t>(level) == 1 ? 0 : static_cast<int32_t>(level);
    NWebHelper::Instance().TrimMemoryByPressureLevel(memoryLevel);
    return;
}

static void SetPathAllowingUniversalAccess(ani_env *env, ani_object object, ani_object pathList)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_array_ref pathListStr = static_cast<ani_array_ref>(pathList);
    ani_int pathCount;
    env->Object_GetPropertyByName_Int(pathList,"length",&pathCount);
    std::vector<std::string>pathListArr;
    for (ani_int i = 0 ; i < pathCount ; i++) {
        ani_ref pathItem = nullptr;
        env->Array_Get_Ref(pathListStr, i, &pathItem);
        std::string path;
        if (!AniParseUtils::ParseString(env, pathItem, path)) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "pathList", "Array<string>"));
            return;
        }
        if (path.empty()) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString("BusinessError 401: Parameter error. Path: '%s' is invalid", path.c_str()));
            return;
        }
        pathListArr.emplace_back(path);
    }
}

static void EnableWholeWebPageDrawing(ani_env* env, ani_object object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_ref result = nullptr;
    NWebHelper::Instance().EnableWholeWebPageDrawing();
    env->GetUndefined(&result);
    return;
}

static ani_string GetSurfaceId(ani_env* env, ani_object object)
{
    ani_string result = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }
    std::string surfaceId = controller->GetSurfaceId();
    env->String_NewUTF8(surfaceId.c_str(), surfaceId.size(), &result);
    return result;
}

static void SetPrintBackground(ani_env* env, ani_object object, ani_boolean enable)
{
    bool printBackgroundEnabled = static_cast<bool>(enable);
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->SetPrintBackground(printBackgroundEnabled);
    return;
}

static ani_boolean GetPrintBackground(ani_env* env, ani_object object, ani_boolean enable)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }

    if (!controller->GetPrintBackground()) {
        return ANI_FALSE;
    }

    return ANI_TRUE;
}

static void AddResourceToMemoryCache(ani_env* env, ani_object object, OfflineResourceValueAni resourceValue)
{
    WVLOG_I("AddResourceToMemoryCache begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::vector<std::string> urlList;
    ani_size byteLength;
    uint8_t* arrayBufferUint8 = nullptr;
    std::map<std::string, std::string> responseHeaders;
    ani_int type = 0;

    ani_object urlListObject = static_cast<ani_object>(resourceValue.urlListArray);
    if (!AniParseUtils::ParseStringArray(env, urlListObject, urlList)) {
        WVLOG_E("InjectOfflineResources ParseStringArray fail");
        return;
    }
    if (env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(resourceValue.resourceArrayBuffer),
            reinterpret_cast<void**>(&arrayBufferUint8), &byteLength) != ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo failed");
        return;
    }
    std::vector<uint8_t> postData(arrayBufferUint8, arrayBufferUint8 + byteLength);
    if (!AniParseUtils::ParseStringArrayMap(
            env, static_cast<ani_object>(resourceValue.responseHeadersArray), responseHeaders)) {
        WVLOG_E("InjectOfflineResources ParseStringArrayMap fail");
        return;
    }
    env->EnumItem_GetValue_Int(static_cast<ani_enum_item>(resourceValue.typeRef), &type);

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    if (!controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->InjectOfflineResource(urlList, postData, responseHeaders, type);
}

static void AddResourcesToMemoryCache(
    ani_env* env, ani_object object, ani_int resourceMapsCount, ani_object resourceMaps)
{
    ani_array_ref resourceMapsRef;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!object) {
        WVLOG_E("object is nullptr");
        return;
    }
    resourceMapsRef = static_cast<ani_array_ref>(resourceMaps);
    for (ani_int i = 0; i < resourceMapsCount; i++) {
        ani_ref resourceMapItem = nullptr;
        env->Array_Get_Ref(resourceMapsRef, i, &resourceMapItem);
        ani_ref urlListArray = nullptr;
        ani_ref resourceArrayBuffer = nullptr;
        ani_ref responseHeadersArray = nullptr;
        ani_ref typeRef = nullptr;
        if (env->Object_GetPropertyByName_Ref(static_cast<ani_object>(resourceMapItem), "urlList", &urlListArray) !=
            ANI_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
            WVLOG_E("urlList error");
            continue;
        }
        if (env->Object_GetPropertyByName_Ref(
                static_cast<ani_object>(resourceMapItem), "resource", &resourceArrayBuffer) != ANI_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
            WVLOG_E("resource error");
            continue;
        }
        if (env->Object_GetPropertyByName_Ref(
                static_cast<ani_object>(resourceMapItem), "responseHeaders", &responseHeadersArray) != ANI_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
            WVLOG_E("responseHeaders error");
            continue;
        }
        if (env->Object_GetPropertyByName_Ref(static_cast<ani_object>(resourceMapItem), "type", &typeRef) != ANI_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
            WVLOG_E("type error");
            continue;
        }
        OfflineResourceValueAni resourceValue;
        resourceValue.urlListArray = urlListArray;
        resourceValue.resourceArrayBuffer = resourceArrayBuffer;
        resourceValue.responseHeadersArray = responseHeadersArray;
        resourceValue.typeRef = typeRef;
        AddResourceToMemoryCache(env, object, resourceValue);
    }
}

static void InjectOfflineResources(ani_env* env, ani_object object, ani_object resourceMaps)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!object) {
        WVLOG_E("object is nullptr");
        return;
    }
    ani_double resourceMapsCount;
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("controller null");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    env->Object_GetPropertyByName_Double(resourceMaps, "length", &resourceMapsCount);
    size_t resourceMapsCountInt = static_cast<size_t>(resourceMapsCount);
    if (resourceMapsCountInt > MAX_RESOURCES_COUNT || resourceMapsCountInt == 0) {
        WVLOG_E("BusinessError: 401. The size of 'resourceMaps' must less than %{public}zu and not 0",
            MAX_RESOURCES_COUNT);
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    AddResourcesToMemoryCache(env, object, resourceMapsCount, resourceMaps);
}

static void ClearPrefetchedResource(ani_env* env, ani_object aniClass, ani_object cacheKey)
{
    WVLOG_I("ClearPrefetchedResource invoked.");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_array_ref cacheKeyStr = nullptr;
    cacheKeyStr = static_cast<ani_array_ref>(cacheKey);
    ani_double cacheKeyCount;
    env->Object_GetPropertyByName_Double(cacheKey, "length", &cacheKeyCount);
    std::vector<std::string> cacheKeyList;
    for (ani_double i = 0; i < cacheKeyCount; i++) {
        ani_ref pathItem = nullptr;
        env->Array_Get_Ref(cacheKeyStr, i, &pathItem);
        std::string path;
        if (!AniParseUtils::ParseString(env, pathItem, path)) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "cacheKey", "Array<string>"));
            return;
        }
        if (path.empty()) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString("BusinessError 401: Parameter error. Path: '%s' is invalid", path.c_str()));
            return;
        }
        cacheKeyList.emplace_back(path);
    }
    NWebHelper::Instance().ClearPrefetchedResource(cacheKeyList);
}

ani_double PrecompileJavaScriptPromise(ani_env* env, ani_object object, std::string url,
    std::string script, std::shared_ptr<OHOS::NWeb::CacheOptions> cacheOptions)
{
    WVLOG_I("PrecompileJavaScript");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return NWEB_ERROR;
    }
    ani_resolver resolver = nullptr;
    ani_error rejection = nullptr;
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    env->PromiseResolver_Reject(resolver, rejection);
    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebPrecompileCallback>();
    if (url.empty() || script.empty()) {
        WVLOG_E("PrecompileJavaScript args empty");
        return NWebError::PARAM_CHECK_ERROR;
    }
    if (!callbackImpl) {
        WVLOG_E("PrecompileJavaScript !callbackImpl");
        return NWebError::PARAM_CHECK_ERROR;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_I("PrecompileJavaScript controller fail");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return NWEB_ERROR;
    }
    controller->PrecompileJavaScript(url, script, cacheOptions, callbackImpl);
    return NWebError::NO_ERROR;
}

ani_double PrecompileJavaScript(
    ani_env* env, ani_object object, ani_object url, ani_object script, ani_object cacheOptions)
{
    WVLOG_I("PrecompileJavaScript begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return -1.0;
    }

    std::string urlStr;
    if (!AniParseUtils::ParseString(env, url, urlStr)) {
        WVLOG_E("Parse url failed.");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return -1.0;
    }
    ani_class ArrayBufferCls;
    env->FindClass("escompat.ArrayBuffer", &ArrayBufferCls);
    ani_boolean isArrayBuffer;
    std::string scriptStr;
    env->Object_InstanceOf(script, ArrayBufferCls, &isArrayBuffer);
    if (isArrayBuffer) {
        uint8_t* arrayBuffer = nullptr;
        ani_size byteLength;
        if (env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(script), reinterpret_cast<void**>(&arrayBuffer),
                &byteLength) != ANI_OK) {
            WVLOG_E("ArrayBuffer_GetInfo failed");
            return -1.0;
        }
        std::vector<uint8_t> postData(arrayBuffer, arrayBuffer + byteLength);
    } else {
        if (!AniParseUtils::ParseString(env, script, scriptStr)) {
            WVLOG_E("PrecompileJavaScript :script must be string or Uint8Array");
            return -1.0;
        }
    }
    
    auto cacheOptionsPtr = AniParseUtils::ParseCacheOptions(env, cacheOptions);
    PrecompileJavaScriptPromise(env, object, urlStr, scriptStr, cacheOptionsPtr);
    return -1.0;
}

static std::shared_ptr<NWebEnginePrefetchArgs> ParsePrefetchArgs(ani_env* env, ani_object object, ani_object request)
{
    if (env == nullptr) {
        return nullptr;
    }
    ani_ref urlObj = nullptr;
    std::string url;
    if (env->Object_GetPropertyByName_Ref(request, "url", &urlObj) != ANI_OK) {
        return nullptr;
    }
    if (!ParsePrepareUrl(env, urlObj, url)) {
        AniBusinessError::ThrowErrorByErrCode(env, INVALID_URL);
        return nullptr;
    }

    ani_ref methodObj = nullptr;
    std::string method;
    env->Object_GetPropertyByName_Ref(request, "method", &methodObj);
    if (!ParsePrepareRequestMethod(env, methodObj, method)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    ani_ref formDataObj = nullptr;
    std::string formData;
    env->Object_GetPropertyByName_Ref(request, "formData", &formDataObj);
    if (!AniParseUtils::ParseString(env, formDataObj, formData)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    std::shared_ptr<NWebEnginePrefetchArgs> prefetchArgs =
        std::make_shared<NWebEnginePrefetchArgsImpl>(url, method, formData);
    return prefetchArgs;
}

static void PrefetchResource(ani_env* env, ani_object object, ani_object request, ani_object additionalHeaders,
    ani_object cacheKey, ani_object cacheValidTime)
{
    if (env == nullptr) {
        return;
    }
    std::shared_ptr<NWebEnginePrefetchArgs> prefetchArgs = ParsePrefetchArgs(env, object, request);
    if (prefetchArgs == nullptr) {
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    std::map<std::string, std::string> additionalHttpHeadersObj;
    env->Reference_IsUndefined(additionalHeaders, &isUndefined);
    if (!isUndefined && !GetWebHeaders(env, additionalHeaders, additionalHttpHeadersObj)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    std::string cacheKeyObj;
    if (env->Reference_IsUndefined(cacheKey, &isUndefined) != ANI_OK) {
        return;
    }
    if (!isUndefined && !AniParseUtils::ParseString(env, cacheKey, cacheKeyObj)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    if (cacheKeyObj.empty()) {
        cacheKeyObj = prefetchArgs->GetUrl();
    } else {
        if (!CheckCacheKey(env, cacheKeyObj)) {
            return;
        }
    }
    if (env->Reference_IsUndefined(cacheValidTime, &isUndefined) != ANI_OK) {
        return;
    }
    ani_double cacheValidTimeTemp = 300;
    if (!isUndefined &&
        (env->Object_CallMethodByName_Double(cacheValidTime, "doubleValue", nullptr, &cacheValidTimeTemp) != ANI_OK)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "duration", "number"));
        return;
    }
    int32_t cacheValidTimeObj = static_cast<int32_t>(cacheValidTimeTemp);
    if (cacheValidTimeObj <= 0 || cacheValidTimeObj > INT_MAX) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    NWebHelper::Instance().PrefetchResource(prefetchArgs, additionalHttpHeadersObj, cacheKeyObj, cacheValidTimeObj);
    return;
}

static void StartCamera(ani_env* env, ani_object object)
{
    WVLOG_I("StartCamera begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->StartCamera();
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
    return;
}

static void CloseAllMediaPresentations(ani_env* env, ani_object object)
{
    WVLOG_I("CloseAllMediaPresentations begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->CloseAllMediaPresentations();
    return;
}

static void StopAllMedia(ani_env* env, ani_object object)
{
    WVLOG_I("StopAllMedia begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->StopAllMedia();
    return;
}

static void StopCamera(ani_env* env, ani_object object)
{
    WVLOG_I("StopCamera begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->StopCamera();
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
    return;
}

static void CloseCamera(ani_env* env, ani_object object)
{
    WVLOG_I("CloseCamera begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->CloseCamera();
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
    return;
}

static void PauseAllMedia(ani_env* env, ani_object object)
{
    WVLOG_I("PauseAllMedia begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->PauseAllMedia();
    return;
}

static void ResumeAllMedia(ani_env* env, ani_object object)
{
    WVLOG_I("ResumeAllMedia begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->ResumeAllMedia();
    return;
}

static void SetAudioMuted(ani_env* env, ani_object object,ani_boolean mute)
{
    WVLOG_I("SetAudioMuted begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ErrCode ret = controller->SetAudioMuted(mute);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
    return;
}

static ani_enum_item GetMediaPlaybackState(ani_env* env, ani_object object)
{
    WVLOG_I("GetMediaPlaybackState begin");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_int mediaPlaybackState = 0;
    ani_enum enumType;
    env->FindEnum(ANI_ENUM_MEDIA_PLAY_BACK_STATE, &enumType);
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    mediaPlaybackState = static_cast<ani_int>(controller->GetMediaPlaybackState());
    ani_enum_item state;
    env->Enum_GetEnumItemByIndex(enumType, mediaPlaybackState, &state);
    return state;
}

void OnCreateNativeMediaPlayer(ani_env* env, ani_object object, ani_fn_object callback)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    
    ani_vm *vm = nullptr;
    env->GetVM(&vm);
    g_vm = vm;
    WVLOG_I("put on_create_native_media_player callback");

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->OnCreateNativeMediaPlayer(g_vm, callback);
}

bool ParseJsLengthResourceToInt(ani_env* env, ani_object jsLength, PixelUnit& type, int32_t& result)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_ref resIdObj = nullptr;
    int32_t resId;
    if ((env->Object_GetPropertyByName_Ref(jsLength, "id", &resIdObj) != ANI_OK)) {
        return false;
    }
    if (!AniParseUtils::ParseInt32(env, resIdObj, resId)) {
        return false;
    }
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!context) {
        WVLOG_E("WebPageSnapshot Failed to get application context.");
        return false;
    }
    auto resourceManager = context->GetResourceManager();
    if (!resourceManager) {
        WVLOG_E("WebPageSnapshot Failed to get resource manager.");
        return false;
    }
    ani_ref jsResourceType = nullptr;
    env->Object_GetPropertyByName_Ref(jsLength, "type", &jsResourceType);
    if (AniParseUtils::IsDouble(env, static_cast<ani_object>(jsResourceType))) {
        int32_t resourceTypeNum;
        AniParseUtils::ParseInt32(env, jsResourceType, resourceTypeNum);
        std::string resourceString;
        switch (resourceTypeNum) {
            case static_cast<int>(ResourceType::INTEGER):
                if (resourceManager->GetIntegerById(resId, result) == Global::Resource::SUCCESS) {
                    type = PixelUnit::VP;
                    return true;
                }
                break;
            case static_cast<int>(ResourceType::STRING):
                if (resourceManager->GetStringById(resId, resourceString) == Global::Resource::SUCCESS) {
                    return AniParseUtils::ParseJsLengthStringToInt(resourceString, type, result);
                }
                break;
            default:
                WVLOG_E("WebPageSnapshot resource type not support");
                break;
        }
        return false;
    }
    return false;
}

bool ParseJsLengthToInt(ani_env* env, ani_object jsLength, PixelUnit& type, int32_t& result)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if ((!AniParseUtils::IsObject(env, jsLength)) && (!AniParseUtils::IsString(env, jsLength)) &&
        (!AniParseUtils::IsDouble(env, jsLength))) {
        WVLOG_E("WebPageSnapshot Unable to parse js length object.");
        return false;
    }
    if (AniParseUtils::IsDouble(env, jsLength)) {
        AniParseUtils::ParseInt32(env, jsLength, result);
        type = PixelUnit::VP;
        return true;
    }
    if (AniParseUtils::IsString(env, jsLength)) {
        std::string nativeString;
        AniParseUtils::ParseString(env, jsLength, nativeString);
        if (!AniParseUtils::ParseJsLengthStringToInt(nativeString, type, result)) {
            return false;
        }
        return true;
    }
    if (AniParseUtils::IsObject(env, jsLength)) {
        return ParseJsLengthResourceToInt(env, jsLength, type, result);
    }
    return false;
}

static void JsErrorCallback(ani_env* env, ani_ref jsCallback, int32_t err)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_object jsResult = {};
    ani_ref jsError = AniBusinessError::CreateError(env, err);
    ani_fn_object callbackFn = static_cast<ani_fn_object>(jsCallback);
    std::vector<ani_ref> vec;
    vec.push_back(jsError);
    vec.push_back(jsResult);
    ani_ref fnReturnVal;
    env->FunctionalObject_Call(callbackFn, ani_size(RESULT_COUNT), vec.data(), &fnReturnVal);
}

std::atomic<bool> g_inWebPageSnapshot { false };
WebSnapshotCallback CreateWebPageSnapshotResultCallback(
    ani_env* env, ani_ref jsCallback, bool check, int32_t inputWidth, int32_t inputHeight)
{
    return [env, jCallback = std::move(jsCallback), check, inputWidth, inputHeight](const char* returnId,
               bool returnStatus, float radio, void* returnData, int returnWidth, int returnHeight) {
        WVLOG_I("WebPageSnapshot return ani callback");
        ani_object jsResult = {};
        if (AniParseUtils::CreateObjectVoid(env, ANI_SNAPSHOT_RESULT_CLASS_NAME, jsResult) == false) {
            WVLOG_E("jsResult CreateObjectVoid failed");
            return;
        }
        ani_object jsPixelMap = nullptr;
        Media::InitializationOptions opt;
        opt.size.width = static_cast<int32_t>(returnWidth);
        opt.size.height = static_cast<int32_t>(returnHeight);
        opt.pixelFormat = Media::PixelFormat::RGBA_8888;
        opt.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        opt.editable = true;
        auto pixelMap = Media::PixelMap::Create(opt);
        if (pixelMap != nullptr) {
            uint64_t stride = static_cast<uint64_t>(returnWidth) << 2;
            uint64_t bufferSize = stride * static_cast<uint64_t>(returnHeight);
            pixelMap->WritePixels(static_cast<const uint8_t*>(returnData), bufferSize);
            std::shared_ptr<Media::PixelMap> pixelMapToJs(pixelMap.release());
            jsPixelMap = OHOS::Media::PixelMapTaiheAni::CreateEtsPixelMap(env, pixelMapToJs);
        } else {
            WVLOG_E("WebPageSnapshot create pixel map error");
        }
        env->Object_SetPropertyByName_Ref(jsResult, "imagePixelMap", jsPixelMap);
        ani_string jsId = nullptr;
        env->String_NewUTF8(returnId, strlen(returnId), &jsId);
        env->Object_SetPropertyByName_Ref(jsResult, "id", jsId);
        ani_object jsStatus = {};
        if (!AniParseUtils::CreateBoolean(env, returnStatus, jsStatus)) {
            return;
        }
        env->Object_SetPropertyByName_Ref(jsResult, "status", jsStatus);
        ani_object jsError = {};
        ani_ref callbackResult;
        std::vector<ani_ref> vec;
        vec.push_back(jsError);
        vec.push_back(jsResult);
        ani_fn_object callbackFn = static_cast<ani_fn_object>(jCallback);
        env->FunctionalObject_Call(callbackFn, ani_size(RESULT_COUNT), vec.data(), &callbackResult);
        env->GlobalReference_Delete(jCallback);
        g_inWebPageSnapshot = false;
    };
}

bool ParseSnapshotOptions(ani_env* env, ani_object info, SnapshotOptions& options)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_ref snapshotId = nullptr;
    ani_ref snapshotSize = nullptr;
    ani_ref snapshotSizeWidth = nullptr;
    ani_ref snapshotSizeHeight = nullptr;
    if (env->Object_GetPropertyByName_Ref(info, "id", &snapshotId) == ANI_OK) {
        AniParseUtils::ParseString(env, snapshotId, options.id);
    }
    if (env->Object_GetPropertyByName_Ref(info, "size", &snapshotSize) == ANI_OK) {
        ani_object snapshotSizeObj = static_cast<ani_object>(snapshotSize);
        if (env->Object_GetPropertyByName_Ref(snapshotSizeObj, "width", &snapshotSizeWidth) == ANI_OK) {
            ani_object snapshotSizeWidthObj = static_cast<ani_object>(snapshotSizeWidth);
            if (!ParseJsLengthToInt(env, snapshotSizeWidthObj, options.widthType, options.width)) {
                return false;
            }
        }
        if (env->Object_GetPropertyByName_Ref(snapshotSizeObj, "height", &snapshotSizeHeight) == ANI_OK) {
            ani_object snapshotSizeHeightObj = static_cast<ani_object>(snapshotSizeHeight);
            if (!ParseJsLengthToInt(env, snapshotSizeHeightObj, options.heightType, options.height)) {
                return false;
            }
        }
    }
    if (options.widthType != PixelUnit::NONE && options.heightType != PixelUnit::NONE &&
        options.widthType != options.heightType) {
        WVLOG_E("WebPageSnapshot input different pixel unit");
        return false;
    }
    if (options.widthType != PixelUnit::NONE) {
        options.sizeType = options.widthType;
    } else if (options.heightType != PixelUnit::NONE) {
        options.sizeType = options.heightType;
    }
    if (options.width < 0 || options.height < 0) {
        WVLOG_E("WebPageSnapshot input pixel length less than 0");
        return false;
    }
    return true;
}

static void WebPageSnapshot(ani_env* env, ani_object object, ani_object info, ani_object callback)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!AniParseUtils::IsFunction(env, callback)) {
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    if (g_inWebPageSnapshot) {
        JsErrorCallback(env, std::move(callback), FUNCTION_NOT_ENABLE);
        return;
    }
    g_inWebPageSnapshot = true;
    SnapshotOptions options;
    if (!ParseSnapshotOptions(env, info, options)) {
        JsErrorCallback(env, std::move(callback), PARAM_CHECK_ERROR);
        g_inWebPageSnapshot = false;
        return;
    }
    bool pixelCheck = (options.sizeType == PixelUnit::VP);
    WVLOG_I("WebPageSnapshot pixel type :%{public}d", static_cast<int>(options.sizeType));    
    ani_ref callbackRef;
    if (ANI_OK != env->GlobalReference_Create(callback, &callbackRef)) {
        WVLOG_E("WebPageSnapshot failed to create reference for callback");
        return;
    }
    auto resultCallback =
        CreateWebPageSnapshotResultCallback(env, std::move(callbackRef), pixelCheck, options.width, options.height);
    ErrCode ret = controller->WebPageSnapshot(
        options.id.c_str(), options.sizeType, options.width, options.height, std::move(resultCallback));
    if (ret != NO_ERROR) {
        g_inWebPageSnapshot = false;
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
}

ani_status StsWebviewControllerInit(ani_env *env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webviewControllerCls = nullptr;
    ani_status status = env->FindClass(ANI_CLASS_WEBVIEW_CONTROLLER, &webviewControllerCls);
    if (status != ANI_OK || !webviewControllerCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_CLASS_WEBVIEW_CONTROLLER, status);
        return ANI_ERROR;
    }
    std::array controllerMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void *>(Constructor) },
        ani_native_function { "_setNWebId", nullptr, reinterpret_cast<void *>(SetNWebId) },
        ani_native_function { "_setHapPath", nullptr, reinterpret_cast<void *>(SetHapPath) },
        ani_native_function { "initializeWebEngine", nullptr, reinterpret_cast<void *>(InitializeWebEngine) },
        ani_native_function { "loadUrl", nullptr, reinterpret_cast<void *>(LoadUrl) },
        ani_native_function { "onActive", nullptr, reinterpret_cast<void *>(OnActive) },
        ani_native_function { "onInactive", nullptr, reinterpret_cast<void *>(OnInactive) },
        ani_native_function { "getWebId", nullptr, reinterpret_cast<void *>(GetWebId) },
        ani_native_function { "getScrollable", nullptr, reinterpret_cast<void *>(GetScrollable) },
        ani_native_function { "requestFocus", nullptr, reinterpret_cast<void *>(RequestFocus) },
        ani_native_function { "clearClientAuthenticationCache", nullptr, 
            reinterpret_cast<void *>(clearClientAuthenticationCache) },
        ani_native_function { "scrollByWithResult", nullptr, reinterpret_cast<void *>(ScrollByWithResult) },
        ani_native_function { "setScrollable", nullptr, reinterpret_cast<void *>(SetScrollable) },
        ani_native_function { "scrollTo", nullptr, reinterpret_cast<void *>(ScrollTo) },
        ani_native_function { "scrollBy", nullptr, reinterpret_cast<void *>(ScrollBy) },
        ani_native_function { "getScrollOffset", nullptr, reinterpret_cast<void *>(GetScrollOffset) },
        ani_native_function { "slideScroll", nullptr, reinterpret_cast<void *>(SlideScroll) },
        ani_native_function { "zoom", nullptr, reinterpret_cast<void *>(Zoom) },
        ani_native_function { "pageDown", nullptr, reinterpret_cast<void *>(PageDown) },
        ani_native_function { "pageUp", nullptr, reinterpret_cast<void *>(PageUp) },
        ani_native_function { "isAdsBlockEnabledForCurPage", nullptr, 
            reinterpret_cast<void *>(IsAdsBlockEnabledForCurPage) },
        ani_native_function { "isIntelligentTrackingPreventionEnabled", nullptr,
            reinterpret_cast<void *>(IsIntelligentTrackingPreventionEnabled) },
        ani_native_function { "addIntelligentTrackingPreventionBypassingList", nullptr,
            reinterpret_cast<void *>(AddIntelligentTrackingPreventionBypassingList) },
        ani_native_function { "removeIntelligentTrackingPreventionBypassingList", nullptr,
            reinterpret_cast<void *>(RemoveIntelligentTrackingPreventionBypassingList) },
        ani_native_function { "clearIntelligentTrackingPreventionBypassingList", nullptr,
            reinterpret_cast<void *>(ClearIntelligentTrackingPreventionBypassingList) },
        ani_native_function { "setHostIP", nullptr, reinterpret_cast<void *>(SetHostIP) },
        ani_native_function { "clearHostIP", nullptr,reinterpret_cast<void *>(ClearHostIP) },
        ani_native_function { "warmupServiceWorker", nullptr, reinterpret_cast<void *>(WarmupServiceWorker) },
        ani_native_function { "isSafeBrowsingEnabled", nullptr, reinterpret_cast<void *>(IsSafeBrowsingEnabled) },
        ani_native_function { "prepareForPageLoad", nullptr, reinterpret_cast<void *>(PrepareForPageLoad) },
        ani_native_function { "getFavicon", nullptr, reinterpret_cast<void *>(GetFavicon) },
        ani_native_function { "setUrlTrustList", nullptr, reinterpret_cast<void *>(SetUrlTrustList) },
        ani_native_function { "enableIntelligentTrackingPrevention", nullptr,
                              reinterpret_cast<void *>(EnableIntelligentTrackingPrevention) },
        ani_native_function { "searchNext", nullptr, reinterpret_cast<void *>(SearchNext) },
        ani_native_function { "getDefaultUserAgent", nullptr, reinterpret_cast<void *>(GetDefaultUserAgent) },
        ani_native_function { "zoomOut", nullptr, reinterpret_cast<void *>(ZoomOut) },
        ani_native_function { "zoomIn", nullptr, reinterpret_cast<void *>(ZoomIn) },
        ani_native_function { "getLastHitTest", nullptr, reinterpret_cast<void *>(GetLastHitTest) },
        ani_native_function { "getPageHeight", nullptr, reinterpret_cast<void *>(GetPageHeight) },
        ani_native_function { "refresh", nullptr, reinterpret_cast<void *>(Refresh) },
        ani_native_function { "stop", nullptr, reinterpret_cast<void *>(Stop) },
        ani_native_function { "clearSslCache", nullptr, reinterpret_cast<void *>(clearSslCache) },
        ani_native_function { "clearMatches", nullptr, reinterpret_cast<void *>(clearMatches) },
        ani_native_function { "startDownload", nullptr, reinterpret_cast<void *>(StartDownload) },
        ani_native_function { "setDownloadDelegate", nullptr, reinterpret_cast<void *>(SetDownloadDelegate) },
        ani_native_function { "getBackForwardEntries", nullptr, reinterpret_cast<void *>(GetBackForwardEntries) },
        ani_native_function { "accessStep", nullptr, reinterpret_cast<void *>(AccessStep) },
        ani_native_function { "removeAllCache", nullptr, reinterpret_cast<void *>(RemoveAllCache) },
        ani_native_function { "isAdsBlockEnabled", nullptr, reinterpret_cast<void *>(IsAdsBlockEnabled) },
        ani_native_function { "enableAdsBlock", nullptr, reinterpret_cast<void *>(EnableAdsBlock) },
        ani_native_function { "postUrl", nullptr, reinterpret_cast<void *>(PostUrl) },
        ani_native_function { "getUrl", nullptr, reinterpret_cast<void *>(GetUrlAni) },
        ani_native_function { "getTitle", nullptr, reinterpret_cast<void *>(GetTitle) },
        ani_native_function { "getOriginalUrl", nullptr, reinterpret_cast<void *>(GetOriginalUrl) },
        ani_native_function { "getUserAgent", nullptr, reinterpret_cast<void *>(GetUserAgent) },
        ani_native_function { "getCustomUserAgent", nullptr, reinterpret_cast<void *>(GetCustomUserAgent) },
        ani_native_function { "getLastJavascriptProxyCallingFrameUrl", nullptr,
                              reinterpret_cast<void *>(GetLastJavascriptProxyCallingFrameUrl) },
        ani_native_function { "forward", nullptr, reinterpret_cast<void *>(Forward) },
        ani_native_function { "backward", nullptr, reinterpret_cast<void *>(Backward) },
        ani_native_function { "accessForward", nullptr, reinterpret_cast<void *>(AccessForward) },
        ani_native_function { "accessBackward", nullptr, reinterpret_cast<void *>(AccessBackward) },
        ani_native_function { "loadData", nullptr, reinterpret_cast<void *>(LoadData) },
        ani_native_function { "clearHistory", nullptr, reinterpret_cast<void *>(ClearHistory) },
        ani_native_function { "clearWebSchemeHandler", nullptr, reinterpret_cast<void *>(ClearWebSchemeHandler) },
        ani_native_function { "terminateRenderProcess", nullptr, reinterpret_cast<void *>(TerminateRenderProcess) },
        ani_native_function { "setRenderProcessMode", nullptr, reinterpret_cast<void *>(SetRenderProcessMode) },
        ani_native_function { "getRenderProcessMode", nullptr, reinterpret_cast<void *>(GetRenderProcessMode) },
        ani_native_function { "setConnectionTimeout", nullptr, reinterpret_cast<void *>(SetConnectionTimeout) },
        ani_native_function { "backOrForward", nullptr, reinterpret_cast<void *>(BackOrForward) },
        ani_native_function { "setWebDebuggingAccess", nullptr, reinterpret_cast<void *>(SetWebDebuggingAccess) },
        ani_native_function { "enableSafeBrowsing", nullptr, reinterpret_cast<void *>(EnableSafeBrowsing) },
        ani_native_function { "pauseAllTimers", nullptr, reinterpret_cast<void *>(PauseAllTimers) },
        ani_native_function { "resumeAllTimers", nullptr, reinterpret_cast<void *>(ResumeAllTimers) },
        ani_native_function { "setCustomUserAgent", nullptr, reinterpret_cast<void *>(SetCustomUserAgent) },
        ani_native_function { "removeCache", nullptr, reinterpret_cast<void *>(RemoveCache) },
        ani_native_function { "setNetworkAvailable", nullptr, reinterpret_cast<void *>(SetNetworkAvailable) },
        ani_native_function { "isIncognitoMode", nullptr, reinterpret_cast<void *>(IsIncognitoMode) },
        ani_native_function { "serializeWebState", nullptr, reinterpret_cast<void *>(SerializeWebState) },
        ani_native_function { "trimMemoryByPressureLevel", nullptr,
                              reinterpret_cast<void *>(TrimMemoryByPressureLevel) },
        ani_native_function { "setPathAllowingUniversalAccess", nullptr, 
                              reinterpret_cast<void *>(SetPathAllowingUniversalAccess) },
        ani_native_function { "onCreateNativeMediaPlayer", "Lstd/core/Function2;:V", 
                              reinterpret_cast<void *>(OnCreateNativeMediaPlayer) },
        ani_native_function { "injectOfflineResourcesInternal", nullptr,
                              reinterpret_cast<void *>(InjectOfflineResources) },
        ani_native_function { "clearPrefetchedResource", nullptr, reinterpret_cast<void *>(ClearPrefetchedResource) },
        ani_native_function { "precompileJavaScriptInternal", nullptr, reinterpret_cast<void *>(PrecompileJavaScript) },
        ani_native_function { "prefetchResource", nullptr, reinterpret_cast<void *>(PrefetchResource) },
        ani_native_function { "enableWholeWebPageDrawing", nullptr,
                              reinterpret_cast<void *>(EnableWholeWebPageDrawing) },
        ani_native_function { "getSurfaceId", nullptr, reinterpret_cast<void *>(GetSurfaceId) },
        ani_native_function { "setPrintBackground", nullptr, reinterpret_cast<void*>(SetPrintBackground) },
        ani_native_function { "getPrintBackground", nullptr, reinterpret_cast<void*>(GetPrintBackground) },
        ani_native_function { "startCamera", nullptr, reinterpret_cast<void *>(StartCamera) },
        ani_native_function { "closeAllMediaPresentations", nullptr,
                              reinterpret_cast<void *>(CloseAllMediaPresentations) },
        ani_native_function { "stopAllMedia", nullptr, reinterpret_cast<void *>(StopAllMedia) },
        ani_native_function { "stopCamera", nullptr, reinterpret_cast<void *>(StopCamera) },
        ani_native_function { "closeCamera", nullptr, reinterpret_cast<void *>(CloseCamera) },
        ani_native_function { "pauseAllMedia", nullptr, reinterpret_cast<void *>(PauseAllMedia) },
        ani_native_function { "resumeAllMedia", nullptr, reinterpret_cast<void *>(ResumeAllMedia) },
        ani_native_function { "setAudioMuted", nullptr, reinterpret_cast<void *>(SetAudioMuted) },
        ani_native_function { "getMediaPlaybackState", nullptr, reinterpret_cast<void *>(GetMediaPlaybackState) },
        ani_native_function { "webPageSnapshot", nullptr, reinterpret_cast<void *>(WebPageSnapshot) },
    };

    status = env->Class_BindNativeMethods(webviewControllerCls, controllerMethods.data(), controllerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
