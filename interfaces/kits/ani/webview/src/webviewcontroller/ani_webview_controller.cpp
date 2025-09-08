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
#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <regex>
#include <vector>

#include "application_context.h"
#include "ohos_resource_adapter_impl.h"

#include "native_arkweb_utils.h"
#include "native_interface_arkweb.h"

#include "nweb_log.h"
#include "web_errors.h"
#include "webview_hasimage_callback.h"
#include "webview_javascript_execute_callback.h"
#include "webview_javascript_result_callback.h"

#include "nweb_precompile_callback.h"
#include "nweb_cache_options_impl.h"
#include "ani_nweb_value_callback_impl.h"
#include "nweb_store_web_archive_callback.h"

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
#include "ani_parse_utils.h"
#include "web_scheme_handler_response.h"
#include "web_download_item.h"
#include "ani_webview_createpdf_execute_callback.h"
#include "ani_web_scheme_handler_request.h"
#include "web_scheme_handler_request.h"
#include "arkweb_scheme_handler.h"
#include "web_history_list.h"
#include "web_message_port.h"
#include "interop_js/arkts_esvalue.h"
#include "interop_js/arkts_interop_js_api.h"
#include "napi_native_mediaplayer_handler_impl.h"
#include "ohos_adapter_helper.h"
#include "nweb_adapter_helper.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using NWebError::NO_ERROR;
ani_boolean g_WebMessagePort = true;
bool g_inWebPageSnapshot = false;
namespace {
constexpr int32_t RESULT_COUNT = 2;
ani_vm *g_vm = nullptr;
ani_vm *g_vmWebMessagePort = nullptr;
constexpr size_t MAX_URL_TRUST_LIST_STR_LEN = 10 * 1024 * 1024; // 10M
constexpr uint32_t SOCKET_MAXIMUM = 6;
constexpr size_t MAX_RESOURCES_COUNT = 30;
constexpr uint32_t URL_MAXIMUM = 2048;
constexpr char URL_REGEXPR[] = "^http(s)?:\\/\\/.+";
const char *INVOKE_METHOD_NAME = "invoke";
const int CALLBACK_PARAM_LENGTH = 2;
int32_t maxFdNum_ = -1;
std::atomic<int32_t> usedFd_ { 0 };
constexpr double A4_WIDTH = 8.27;
constexpr double A4_HEIGHT = 11.69;
constexpr double SCALE_MIN = 0.1;
constexpr double SCALE_MAX = 2.0;
constexpr double HALF = 2.0;
constexpr double TEN_MILLIMETER_TO_INCH = 0.39;
const char* ANI_WEB_CUSTOM_SCHEME_CLASS = "L@ohos/web/webview/webview/WebCustomSchemeClass;";
constexpr size_t BFCACHE_DEFAULT_SIZE = 1;
constexpr size_t BFCACHE_DEFAULT_TIMETOLIVE = 600;
const char* WEB_CONTROLLER_SECURITY_LEVEL_ENUM_NAME = "L@ohos/web/webview/webview/SecurityLevel;";
using WebPrintWriteResultCallback = std::function<void(const std::string&, uint32_t)>;
struct PDFMarginConfig {
    double top = TEN_MILLIMETER_TO_INCH;
    double bottom = TEN_MILLIMETER_TO_INCH;
    double right = TEN_MILLIMETER_TO_INCH;
    double left = TEN_MILLIMETER_TO_INCH;
};
struct SnapshotOptions {
    std::string id;
    int32_t width = 0;
    int32_t height = 0;
    PixelUnit widthType = PixelUnit::NONE;
    PixelUnit heightType = PixelUnit::NONE;
    PixelUnit sizeType = PixelUnit::NONE;
};


struct Scheme {
    std::string name;
    bool isSupportCORS;
    bool isSupportFetch;
    bool isStandard;
    bool isLocal;
    bool isDisplayIsolated;
    bool isSecure;
    bool isCspBypassing;
    bool isCodeCacheSupported;
    int32_t option = 0;
};

bool ParsePrepareUrl(ani_env* env, ani_ref urlObj, std::string& url)
{
    if (AniParseUtils::ParseString(env, urlObj, url)) {
        WVLOG_E("urlObj convert to string failed");
        return false;
    }

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
    ani_int typeInt;
    if (env->Object_CallMethodByName_Int(static_cast<ani_object>(typeRef), "unboxed", ":I",
                                         &typeInt) != ANI_OK) {
        return false;
    }
    std::string bundleName;
    std::string moduleName;
    if (!AniParseUtils::ParseString(env, bundleNameRef, bundleName) ||
        !AniParseUtils::ParseString(env, moduleNameRef, moduleName)) {
        return false;
    }
    int type = static_cast<int>(typeInt);
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
    if (!env) {
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

static void SetNWebId(ani_env *env, ani_object object, ani_int nwebId)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("webviewController is nullptr.");
        return;
    }
    auto id = static_cast<int>(nwebId);
    WVLOG_I("SetNWebId nwebId: %{public}d", id);
    controller->SetWebId(id);
}

static void SetHapPath(ani_env *env, ani_object object, ani_string hapPath)
{
    if (!env) {
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
    ani_int headersLength;
    if (env->Object_GetPropertyByName_Int(headersArrayObj, "length", &headersLength) != ANI_OK) {
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
    if (!env) {
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
    if (!env) {
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
        WebHistoryList *historyList = reinterpret_cast<WebHistoryList *>(ptr);
        if (historyList && historyList->DecRefCount() <= 0) {
            delete historyList;
        }
    } else if (clsName == "NativeMediaPlayerHandlerinner") {
        NapiNativeMediaPlayerHandlerImpl *napiNativeMediaPlayerHandlerImpl
            = reinterpret_cast<NapiNativeMediaPlayerHandlerImpl *>(ptr);
        if (napiNativeMediaPlayerHandlerImpl &&
            napiNativeMediaPlayerHandlerImpl->DecRefCount() <= 0) {
            delete napiNativeMediaPlayerHandlerImpl;
        }
    } else if (clsName == "ProxyConfig") {
        delete reinterpret_cast<ProxyConfig *>(ptr);
    } else if (clsName == "WebSchemeHandlerResponse") {
        delete reinterpret_cast<WebSchemeHandlerResponse *>(ptr);
    } else if (clsName == "WebDownloadDelegate") {
        delete reinterpret_cast<WebDownloadDelegate *>(ptr);
    } else if (clsName == "WebMessageExt"){
        delete reinterpret_cast<WebMessageExt *>(ptr);
    } else if (clsName == "WebDownloadItem") {
        delete reinterpret_cast<WebDownloadItem *>(ptr);
    } else if (clsName == "WebDownloadManager") {
        delete reinterpret_cast<WebDownloadManager *>(ptr);
    } else if (clsName == "PdfData") {
        delete reinterpret_cast<WebJsArrayBufferExt *>(ptr);
    } else if (clsName == "WebMessagePort") {
        WebMessagePort *msgPort = reinterpret_cast<WebMessagePort *>(ptr);
        if (msgPort && msgPort->DecRefCount() <= 0) {
            delete msgPort;
        }
    } else if (clsName == "WebSchemeHandler") {
        delete reinterpret_cast<WebSchemeHandler*>(ptr);
    } else if (clsName == "WebSchemeHandlerRequest") {
        delete reinterpret_cast<WebSchemeHandlerRequest*>(ptr);
    } else if (clsName == "WebResourceHandler") {
        reinterpret_cast<WebResourceHandler*>(ptr)->DecStrongRef(reinterpret_cast<WebResourceHandler*>(ptr));
    } else if (clsName == "WebHttpBodyStream") {
        delete reinterpret_cast<WebHttpBodyStream*>(ptr);
    } else if (clsName == "PrintDocumentAdapterInner") {
        delete reinterpret_cast<WebPrintDocument*>(ptr);
    } else {
        WVLOG_E("Clean unsupport className: %{public}s", clsName.c_str());
    }
    return;
}

static void Constructor(ani_env *env, ani_object object, ani_string webTagObject)
{
    WVLOG_D("[DOWNLOAD] AniWebView native Constructor");
    if (!env) {
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
    if (!controller) {
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
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->OnActive();
}

static void OnInactive(ani_env *env, ani_object object)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->OnInactive();
}

static ani_int GetWebId(ani_env *env, ani_object object)
{
    int32_t webId = -1;
    if (!env) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_int>(webId);
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return static_cast<ani_int>(webId);
    }

    webId = controller->GetWebId();
    return static_cast<ani_int>(webId);
}

static ani_int InnerGetWebId(ani_env *env, ani_object object)
{
    int32_t webId = -1;
    if (!env) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_int>(webId);
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        WVLOG_E("conrtroller is nullptr or not init");
        return static_cast<ani_int>(webId);
    }

    webId = controller->GetWebId();
    return static_cast<ani_int>(webId);
}

static ani_boolean GetScrollable(ani_env *env, ani_object object)
{
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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

PDFMarginConfig ParsePDFMarginConfigArgs(ani_env *env, ani_object preArgs, double width, double height)
{
    ani_double marginTopObj;
    double marginTop = TEN_MILLIMETER_TO_INCH;
    if (env->Object_GetPropertyByName_Double(preArgs, "marginTop", &marginTopObj) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "marginTop", "double"));
        return PDFMarginConfig();
    }
    marginTop = static_cast<double>(marginTopObj);
    marginTop = (marginTop >= height / HALF || marginTop <= 0.0) ? 0.0 : marginTop;

    ani_double marginBottomObj;
    double marginBottom = TEN_MILLIMETER_TO_INCH;
    if (env->Object_GetPropertyByName_Double(preArgs, "marginBottom", &marginBottomObj) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "marginBottom", "double"));
        return PDFMarginConfig();
    }
    marginBottom = static_cast<double>(marginBottomObj);
    marginBottom = (marginBottom >= height / HALF || marginBottom <= 0.0) ? 0.0 : marginBottom;

    ani_double marginRightObj;
    double marginRight = TEN_MILLIMETER_TO_INCH;
    if (env->Object_GetPropertyByName_Double(preArgs, "marginRight", &marginRightObj) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "marginRight", "double"));
        return PDFMarginConfig();
    }
    marginRight = static_cast<double>(marginRightObj);
    marginRight = (marginRight >= width / HALF || marginRight <= 0.0) ? 0.0 : marginRight;

    ani_double marginLeftObj;
    double marginLeft = TEN_MILLIMETER_TO_INCH;
    if (env->Object_GetPropertyByName_Double(preArgs, "marginLeft", &marginLeftObj) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "marginLeft", "double"));
        return PDFMarginConfig();
    }
    marginLeft = static_cast<double>(marginLeftObj);
    marginLeft = (marginLeft >= width / HALF || marginLeft <= 0.0) ? 0.0 : marginLeft;

    return { marginTop, marginBottom, marginRight, marginLeft };
}

bool ParsePDFScale(ani_env *env, ani_object preArgs, double &scale)
{
    ani_ref scale_ref;
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Object_GetPropertyByName_Ref(preArgs, "scale", &scale_ref) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scale", "double"));
        return false;
    }
    if (env->Reference_IsUndefined(scale_ref, &isUndefined) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scale", "double"));
        return false;
    }
    if (!isUndefined && !AniParseUtils::ParseDouble_t(env, scale_ref, scale)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scale", "double"));
        return false;
    }
    scale = scale > SCALE_MAX ? SCALE_MAX : scale < SCALE_MIN ? SCALE_MIN : scale;
    return true;
}

bool ParsePDFShouldPrintBackground(ani_env *env, ani_object preArgs, bool &shouldPrintBackground)
{
    ani_ref shouldPrintBackground_ref;
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Object_GetPropertyByName_Ref(preArgs, "shouldPrintBackground", &shouldPrintBackground_ref) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "shouldPrintBackground", "boolean"));
        return false;
    }
    if (env->Reference_IsUndefined(shouldPrintBackground_ref, &isUndefined) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "shouldPrintBackground", "boolean"));
        return false;
    }
    if (!isUndefined && !AniParseUtils::ParseBoolean_t(env, shouldPrintBackground_ref, shouldPrintBackground)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "shouldPrintBackground", "boolean"));
        return false;
    }
    return true;
}

std::shared_ptr<NWebPDFConfigArgs> ParsePDFConfigArgs(ani_env *env, ani_object preArgs)
{
    if (!env) {
        WVLOG_E("ParsePDFConfigArgs env is nullptr");
        return nullptr;
    }
    ani_double widthObj;
    double width = A4_WIDTH;
    if (env->Object_GetPropertyByName_Double(preArgs, "width", &widthObj) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "width", "double"));
        return nullptr;
    }
    width = static_cast<double>(widthObj);

    ani_double heightObj;
    double height = A4_HEIGHT;
    if (env->Object_GetPropertyByName_Double(preArgs, "height", &heightObj) != ANI_OK) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "height", "double"));
        return nullptr;
    }
    height = static_cast<double>(heightObj);

    auto margin = ParsePDFMarginConfigArgs(env, preArgs, width, height);

    double scale = 1.0;
    if (!ParsePDFScale(env, preArgs, scale)) {
        return nullptr;
    }

    bool shouldPrintBackground = false;
    if (!ParsePDFShouldPrintBackground(env, preArgs, shouldPrintBackground)) {
        return nullptr;
    }

    std::shared_ptr<NWebPDFConfigArgs> pdfConfig = std::make_shared<NWebPDFConfigArgsImpl>(
        width, height, scale, margin.top, margin.bottom, margin.right, margin.left, shouldPrintBackground);
    WVLOG_D("PDFConfig width=%{public}f, height=%{public}f, scale==%{public}f, margin.top==%{public}f, "
        "margin.bottom=%{public}f, margin.right=%{public}f, margin.left=%{public}f, shouldPrintBackground=%{public}s",
        width, height, scale, margin.top, margin.bottom, margin.right, margin.left,
        shouldPrintBackground ? "true" : "false");
    return pdfConfig;
}

ani_object CreatePdfData(ani_env *env, const char* result, const long size)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsArrayBufferExt *webArrayBufferExt = new (std::nothrow) WebJsArrayBufferExt(result, size);
    if (!webArrayBufferExt) {
        WVLOG_E("[PDFDATA] new WebHistoryList failed");
        return nullptr;
    }

    ani_object pdfDataObj = {};
    if (!AniParseUtils::CreateObjectVoid(env, ANI_PDF_DATA_CLASS_NAME, pdfDataObj)) {
        WVLOG_E("[PDFDATA] CreateObjectVoid failed");
        delete webArrayBufferExt;
        webArrayBufferExt = nullptr;
        return nullptr;
    }

    if (!AniParseUtils::Wrap(env, pdfDataObj, ANI_PDF_DATA_CLASS_NAME,
        reinterpret_cast<ani_long>(webArrayBufferExt))) {
        WVLOG_E("[PDFDATA] CreatePdf wrap failed");
        delete webArrayBufferExt;
        webArrayBufferExt = nullptr;
        return nullptr;
    }
    return pdfDataObj;
}

bool AsyncCallback(ani_env *env, ani_ref call, ani_object stsErrCode, ani_object retObj)
{
    ani_status status = ANI_ERROR;
    ani_class clsCall {};
    if ((status = env->FindClass("L@ohos/web/webview/webview/AsyncCallbackWrapper;", &clsCall)) != ANI_OK) {
        WVLOG_E("FindClass fail, status: %{public}d", status);
        return false;
    }
    ani_method method = {};
    if ((status = env->Class_FindMethod(
        clsCall, INVOKE_METHOD_NAME, nullptr, &method)) != ANI_OK) {
        WVLOG_E("Class_FindMethod fail, status: %{public}d", status);
        return false;
    }
    if (!stsErrCode) {
        ani_ref nullRef = nullptr;
        env->GetNull(&nullRef);
        stsErrCode = reinterpret_cast<ani_object>(nullRef);
    }
    if (!retObj) {
        ani_ref undefinedRef = nullptr;
        env->GetUndefined(&undefinedRef);
        retObj = reinterpret_cast<ani_object>(undefinedRef);
    }
    if ((status = env->Object_CallMethod_Void(static_cast<ani_object>(call), method, stsErrCode, retObj)) != ANI_OK) {
        WVLOG_E("Object_CallMethod_Void fail, status: %{public}d", status);
        return false;
    }
    return true;
}

static void CreatePdfNative(ani_env *env, ani_object object, ani_object pdfConfigObject, ani_object callbackObject)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::shared_ptr<NWebPDFConfigArgs> pdfConfig = ParsePDFConfigArgs(env, pdfConfigObject);
    if (!pdfConfig) {
        WVLOG_E("ParsePDFConfigArgs failed");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }

    auto callback = [](ani_env *env, const char* result, const long size, ani_ref callbackRef) -> void {
        if (!env) {
            WVLOG_E("env is nullptr");
            return;
        }
        ani_ref jsError = NWebError::AniBusinessError::CreateError(env, NWebError::NO_ERROR);
        ani_object resolution = CreatePdfData(env, result, size);
        if (!resolution) {
            jsError = NWebError::AniBusinessError::CreateError(env, NWebError::PARAM_CHECK_ERROR);
            AsyncCallback(env, callbackRef, static_cast<ani_object>(jsError), nullptr);
            return;
        }
        AsyncCallback(env, callbackRef, static_cast<ani_object>(jsError), resolution);
    };

    std::shared_ptr<NWebArrayBufferValueCallback> callbackImpl =
        std::make_shared<WebviewCreatePDFExecuteCallback>(env, callback, callbackObject);
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->CreatePDFExt(pdfConfig, callbackImpl);
    return;
}

static ani_boolean ScrollByWithResult(ani_env *env, ani_object object, ani_double deltaX, ani_double deltaY)
{
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
        ani_int duration;
        if (env->Object_CallMethodByName_Int(durationObj, "intValue", nullptr, &duration) != ANI_OK) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "duration", "int"));
            return;
        }
        controller->ScrollToWithAnime(
            static_cast<float>(x), static_cast<float>(y), static_cast<int32_t>(duration));
    }
}
static void ScrollBy(ani_env *env, ani_object object, ani_double deltaX, ani_double deltaY, ani_object durationObj)
{
    if (!env) {
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
        ani_int duration;
        if (env->Object_CallMethodByName_Int(durationObj, "intValue", nullptr, &duration) != ANI_OK) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "duration", "int"));
            return;
        }
        controller->ScrollByWithAnime(
            static_cast<float>(deltaX), static_cast<float>(deltaY), static_cast<int32_t>(duration));
    }
}
static ani_object GetScrollOffset(ani_env *env, ani_object object)
{
    ani_object offset = {};
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    int aliveTimeInt = static_cast<int32_t>(aliveTime);
    if (aliveTimeInt <= 0) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
        WVLOG_E("env is nullptr");
        return userAgent;
    }
    WVLOG_I("Get the default user agent.");
    std::string result = NWebHelper::Instance().GetDefaultUserAgent();
    if (result.empty()) {
        WVLOG_E("Default user agent is empty.");
        return userAgent;
    }
    env->String_NewUTF8(result.c_str(), result.size(), &userAgent);
    return userAgent;
}

static ani_boolean IsSafeBrowsingEnabled(ani_env *env, ani_object object)
{
    if (!env) {
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
    ani_env* env, ani_object object, ani_string aniUrl, ani_boolean preconnectable, ani_int aniNumSockets)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string url;
    if (!AniParseUtils::ParseString(env, aniUrl, url)) {
        WVLOG_E("parse url failed");
        return;
    }
    int32_t numSockets = static_cast<int32_t>(aniNumSockets);
    if (numSockets <= 0 || static_cast<uint32_t>(numSockets) > SOCKET_MAXIMUM) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    NWebHelper::Instance().PrepareForPageLoad(url, preconnectable, numSockets);
    return;
}

static void Zoom(ani_env *env, ani_object object, ani_double factor)
{
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!delegate) {
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
    if (!env || !item) {
        WVLOG_E("[PIXELMAP] env or NWebHistoryItem is nullptr");
        return nullptr;
    }

    ani_ref result = nullptr;
    void *data = nullptr;
    int width = 0;
    int height = 0;
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
    if (!pixelMap) {
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
    if (!env) {
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
    opt.size.width = static_cast<int>(width);
    opt.size.height = static_cast<int>(height);
    opt.pixelFormat = getColorType(colorType);
    opt.alphaType = getAlphaType(alphaType);
    opt.editable = true;
    auto pixelMap = Media::PixelMap::Create(opt);
    if (!pixelMap) {
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
    if (!env) {
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
    if (!webHistoryList) {
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
    webHistoryList->IncRefCount();

    env->Object_SetPropertyByName_Int(backForwardObj, "currentIndex", static_cast<ani_int>(currentIndex));
    env->Object_SetPropertyByName_Int(backForwardObj, "size", static_cast<ani_int>(size));
    return backForwardObj;
}

static void RemoveAllCache(ani_env *env, ani_object object, ani_boolean clearRom)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    NWebHelper::Instance().RemoveAllCache(static_cast<bool>(clearRom));
    return;
}

static void PostUrl(ani_env *env, ani_object object, ani_object urlObj, ani_object arrayBufferObj)
{
    if (!env) {
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

bool GetSendPorts(ani_env* env, ani_object portsArrayObj, std::vector<std::string>& sendPorts)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_double arrayLen = 0;
    if (env->Object_GetPropertyByName_Double(portsArrayObj, "length", &arrayLen) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return false;
    }
    ani_class webMessagePortClass;
    if (env->FindClass(ANI_WEB_MESSAGE_PORT_INNER_CLASS_NAME, &webMessagePortClass) != ANI_OK) {
        WVLOG_E("Find WebMessagePort Class failed");
        return false;
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(arrayLen); i++) {
        ani_ref webMessagePortRef;
        if (env->Object_CallMethodByName_Ref(
                portsArrayObj, "$_get", "I:Lstd/core/Object;", &webMessagePortRef, (ani_int)i) != ANI_OK) {
            return false;
        }
        ani_object portsObj = reinterpret_cast<ani_object>(webMessagePortRef);
        ani_boolean isWebMessagePort = false;
        env->Object_InstanceOf(portsObj, webMessagePortClass, &isWebMessagePort);
        if (!isWebMessagePort) {
            WVLOG_E("not WebMessagePort");
            return false;
        }
        WebMessagePort* msgPort = reinterpret_cast<WebMessagePort*>(AniParseUtils::Unwrap(env, portsObj));
        if ((!msgPort)) {
            WVLOG_E("post port to html failed");
            return false;
        }
        std::string portHandle = msgPort->GetPortHandle();
        sendPorts.emplace_back(portHandle);
    }
    return true;
}

static void PostMessage(
    ani_env* env, ani_object object, ani_object nameObj, ani_object portsArrayObj, ani_object uriObj)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string portName;
    if (!AniParseUtils::ParseString(env, nameObj, portName)) {
        return;
    }
    std::vector<std::string> sendPorts;
    if (!GetSendPorts(env, portsArrayObj, sendPorts)) {
        WVLOG_E("post port to html failed, getSendPorts fail");
        return;
    }
    std::string uriStr;
    if (!AniParseUtils::ParseString(env, uriObj, uriStr)) {
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        WVLOG_E("post port to html failed, napi unwrap webviewController failed");
        return;
    }
    controller->PostWebMessage(portName, sendPorts, uriStr);
    return;
}

static ani_string GetUrlAni(ani_env *env, ani_object object)
{
    ani_string url = nullptr;

    if (!env) {
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

    if (!env) {
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

static ani_int GetProgress(ani_env* env, ani_object object)
{
    ani_int progress = 0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return progress;
    }

    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        WVLOG_E("GetProgress: get controller failed");
        return progress;
    }
    return static_cast<ani_int>(controller->GetProgress());
}

static ani_string GetOriginalUrl(ani_env *env, ani_object object)
{
    ani_string originUrl = nullptr;

    if (!env) {
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

    if (!env) {
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

    if (!env) {
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

    if (!env) {
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

static void SearchAllAsync(ani_env *env, ani_object object, ani_object searchStringObj)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::string searchString;
    if (!AniParseUtils::ParseString(env, searchStringObj, searchString)) {
        return;
    }
    controller->SearchAllAsync(searchString);
    return;
}
static void ClearServiceWorkerWebSchemeHandler(ani_env *env, ani_object object)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    int32_t ret = WebviewController::ClearWebServiceWorkerSchemeHandler();
    if (ret != 0) {
        WVLOG_E("ClearServiceWorkerWebSchemeHandler ret=%{public}d", ret);
        return;
    }
    return;
}

static void Forward(ani_env *env, ani_object object)
{
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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

void SetWebSchemeHandler(ani_env* env, ani_object object, ani_string scheme, ani_object handlerObject)
{
    WVLOG_D("setWebSchemeHandler start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::string schemeStr = "";
    if (!AniParseUtils::ParseString(env, scheme, schemeStr)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scheme", "string"));
        return;
    }

    WebSchemeHandler* handler = nullptr;
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(handlerObject, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    handler = reinterpret_cast<WebSchemeHandler*>(thisVar);
    if (!handler) {
        WVLOG_E("AniWebviewController::SetWebSchemeHandler handler is null");
        return;
    }
    if (!controller->SetWebSchemeHandler(schemeStr.c_str(), handler)) {
        WVLOG_E("AniWebviewController::SetWebSchemeHandler failed");
    }
    return;
}

static void SetServiceWorkerWebSchemeHandler(
    ani_env* env, ani_object object, ani_string scheme, ani_object handlerObject)
{
    WVLOG_D("setServiceWorkerWebSchemeHandler start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::string schemePtr = "";
    if (!AniParseUtils::ParseString(env, scheme, schemePtr)) {
        WVLOG_E("AniWebviewController::SetWebSchemeHandler parse scheme failed");
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "scheme", "string"));
        return;
    }

    WebSchemeHandler* handler = nullptr;
    ani_long thisVar;
    ani_status status = env->Object_GetFieldByName_Long(handlerObject, "nativePtr", &thisVar);
    if (status != ANI_OK) {
        WVLOG_E("AniUtils_Unwrap Object_GetFieldByName_Long status: %{public}d", status);
        return;
    }
    handler = reinterpret_cast<WebSchemeHandler*>(thisVar);
    if (!handler) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        WVLOG_E("Unwrap WebSchemeHandler failed");
        return;
    }
    if (ANI_OK != env->GlobalReference_Create(handlerObject, &handler->delegate_)) {
        WVLOG_E("SetServiceWorkerWebSchemeHandler failed to create reference for callback");
        return;
    }
    if (!WebviewController::SetWebServiveWorkerSchemeHandler(schemePtr.c_str(), handler)) {
        WVLOG_E("AniWebviewController::SetWebSchemeHandler failed");
    }
    return;
}

bool CheckSchemeName(const std::string& schemeName)
{
    if (schemeName.empty() || schemeName.size() > MAX_CUSTOM_SCHEME_NAME_LENGTH) {
        WVLOG_E("Invalid scheme name length");
        return false;
    }

    for (auto it = schemeName.begin(); it != schemeName.end(); it++) {
        char chr = *it;
        if (!((chr >= 'a' && chr <= 'z') || (chr >= '0' && chr <= '9') || (chr == '.') || (chr == '+') ||
                (chr == '-'))) {
            WVLOG_E("invalid character %{public}c", chr);
            return false;
        }
    }
    return true;
}

void SetCustomizeSchemeOption(Scheme& scheme)
{
    std::map<int, std::function<bool(const Scheme&)>> schemeProperties = {
        { 0, [](const Scheme& scheme) { return scheme.isStandard; } },
        { 1, [](const Scheme& scheme) { return scheme.isLocal; } },
        { 2, [](const Scheme& scheme) { return scheme.isDisplayIsolated; } },
        { 3, [](const Scheme& scheme) { return scheme.isSecure; } },
        { 4, [](const Scheme& scheme) { return scheme.isSupportCORS; } },
        { 5, [](const Scheme& scheme) { return scheme.isCspBypassing; } },
        { 6, [](const Scheme& scheme) { return scheme.isSupportFetch; } },
        { 7, [](const Scheme& scheme) { return scheme.isCodeCacheSupported; } }
    };
    for (const auto& property : schemeProperties) {
        if (property.second(scheme)) {
            scheme.option += 1 << property.first;
        }
    }
}

bool SetCustomizeScheme(ani_env* env, ani_ref ref, Scheme& scheme)
{
    std::map<std::string, std::function<void(Scheme&, bool)>> schemeBooleanProperties = {
        { "isSupportCORS", [](Scheme& scheme, bool value) { scheme.isSupportCORS = value; } },
        { "isSupportFetch", [](Scheme& scheme, bool value) { scheme.isSupportFetch = value; } },
        { "isStandard", [](Scheme& scheme, bool value) { scheme.isStandard = value; } },
        { "isLocal", [](Scheme& scheme, bool value) { scheme.isLocal = value; } },
        { "isDisplayIsolated", [](Scheme& scheme, bool value) { scheme.isDisplayIsolated = value; } },
        { "isSecure", [](Scheme& scheme, bool value) { scheme.isSecure = value; } },
        { "isCspBypassing", [](Scheme& scheme, bool value) { scheme.isCspBypassing = value; } },
        { "isCodeCacheSupported", [](Scheme& scheme, bool value) { scheme.isCodeCacheSupported = value; } }
    };

    ani_ref schemePropertyRef = nullptr;
    for (const auto& property : schemeBooleanProperties) {
        ani_status status = ANI_OK;
        WVLOG_D("property.first.c_str() : %{public}s", property.first.c_str());

        status =
            env->Object_GetPropertyByName_Ref(static_cast<ani_object>(ref), property.first.c_str(), &schemePropertyRef);
        if (status != ANI_OK) {
            WVLOG_E("Object_GetPropertyByName_Ref status != ANI_OK : %{public}s", property.first.c_str());
        }
        bool schemeProperty = false;
        if (!AniParseUtils::ParseBoolean(env, schemePropertyRef, schemeProperty)) {
            WVLOG_I("ParseBoolean schemeProperty %{public}d ", schemeProperty);
            if (property.first == "isSupportCORS" || property.first == "isSupportFetch") {
                return false;
            }
        }
        if (!schemeProperty) {
            WVLOG_I("schemeProperty status ");
            if (property.first == "isSupportCORS" || property.first == "isSupportFetch") {
                WVLOG_I("property.first.c_str() : %{public}s", property.first.c_str());
                return false;
            }
        }

        property.second(scheme, schemeProperty);
    }
    ani_ref schemeNameObj = nullptr;
    if (env->Object_GetPropertyByName_Ref(static_cast<ani_object>(ref), "schemeName", &schemeNameObj) != ANI_OK) {
        return false;
    }
    if (!AniParseUtils::ParseString(env, schemeNameObj, scheme.name)) {
        return false;
    }
    if (!CheckSchemeName(scheme.name)) {
        return false;
    }
    SetCustomizeSchemeOption(scheme);
    return true;
}

int32_t CustomizeSchemesArrayDataHandler(ani_env* env, ani_ref array)
{
    ani_size arrayLength = 0;
    env->Array_GetLength(static_cast<ani_array>(array), &arrayLength);
    if (arrayLength > MAX_CUSTOM_SCHEME_SIZE) {
        WVLOG_E("PARAM_CHECK_ERROR");
        return PARAM_CHECK_ERROR;
    }
    std::vector<Scheme> schemeVector;
    ani_object obj = {};
    if (AniParseUtils::CreateObjectVoid(env, ANI_WEB_CUSTOM_SCHEME_CLASS, obj) == false) {
        WVLOG_E("Obj CreateObjectVoid failed");
        return PARAM_CHECK_ERROR;
    }
    ani_ref objRef = static_cast<ani_ref>(obj);
    for (ani_size i = 0; i < arrayLength; ++i) {
        ani_status status = env->Array_Get(static_cast<ani_array>(array), i, &objRef);
        if (status != ANI_OK) {
            WVLOG_E("Array_Get_Ref failed %{public}d", status);
        }

        Scheme scheme;
        bool result = SetCustomizeScheme(env, objRef, scheme);
        if (!result) {
            return PARAM_CHECK_ERROR;
        }
        schemeVector.push_back(scheme);
    }
    int32_t registerResult;
    for (auto it = schemeVector.begin(); it != schemeVector.end(); ++it) {
        registerResult = OH_ArkWeb_RegisterCustomSchemes(it->name.c_str(), it->option);
        if (registerResult != NO_ERROR) {
            return registerResult;
        }
    }
    return NO_ERROR;
}

static void CustomizeSchemes(ani_env* env, ani_object object, ani_object schemes)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (WebviewController::existNweb_) {
        WVLOG_E("There exist web component which has been already created.");
    }
    int32_t registerResult = CustomizeSchemesArrayDataHandler(env, schemes);
    if (registerResult == NO_ERROR) {
        return;
    }
    if (registerResult == PARAM_CHECK_ERROR) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "schemeName", "string"));
        return;
    }
    AniBusinessError::ThrowErrorByErrCode(env, REGISTER_CUSTOM_SCHEME_FAILED);
    return;
}

static ani_enum_item GetSecurityLevel(ani_env* env, ani_object object)
{
    ani_enum_item result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        return result;
    }
    ani_enum enumType;
    env->FindEnum(WEB_CONTROLLER_SECURITY_LEVEL_ENUM_NAME, &enumType);
    ani_int securityLevel = controller->GetSecurityLevel();
    env->Enum_GetEnumItemByIndex(enumType, securityLevel, &result);
    return result;
}

static void PrefetchPage(ani_env *env, ani_object object, ani_string url, ani_object additionalHeadersObj)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::string urlStr;
    if (!AniParseUtils::ParseString(env, url, urlStr)) {
        WVLOG_E("Parse url failed.");
        return;
    }

    ErrCode ret;
    std::map<std::string, std::string> additionalHttpHeaders;
    ani_boolean isUndefined = true;
    if (env->Reference_IsUndefined(additionalHeadersObj, &isUndefined) != ANI_OK || isUndefined) {
        ret = controller->PrefetchPage(urlStr, additionalHttpHeaders);
    } else if (GetWebHeaders(env, additionalHeadersObj, additionalHttpHeaders)){
        ret = controller->PrefetchPage(urlStr, additionalHttpHeaders);
    } else {
        return;
    }
    WVLOG_I("PrefetchPage ret: %{public}d", ret);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
    return;
}

static void SetHttpDns(ani_env* env, ani_object object, ani_enum_item secureDnsMode, ani_string secureDnsConfig)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_boolean isUndefined = ANI_FALSE;
    if (env->Reference_IsUndefined(secureDnsMode, &isUndefined) != ANI_OK || isUndefined) {
        return;
    }

    ani_int dohMode;
    if (env->EnumItem_GetValue_Int(secureDnsMode, &dohMode) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    if(dohMode < static_cast<int>(SecureDnsModeType::OFF) ||
            dohMode > static_cast<int>(SecureDnsModeType::SECURE_ONLY)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    std::string dohConfig;
    if (!AniParseUtils::ParseString(env, secureDnsConfig, dohConfig)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    if(dohConfig.rfind("https", 0) != 0 && dohConfig.rfind("HTTPS", 0) != 0) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
    config->SetMode(dohMode);
    config->SetConfig(dohConfig);
    WVLOG_I("set http dns mode:%{public}d doh_config:%{public}s", dohMode, dohConfig.c_str());

    NWebHelper::Instance().SetHttpDns(config);
    return;
}

static void ClearWebSchemeHandler(ani_env *env, ani_object object)
{
    if (!env) {
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
    } else {
        WVLOG_I("AniWebviewController::ClearWebSchemeHandler successful");
    }
}

static ani_ref GetItemAtIndex(ani_env *env, ani_object object, ani_int aniIndex)
{
    if (!env) {
        WVLOG_E("[BACKFORWARD] env is nullptr");
        return nullptr;
    }

    int32_t index = static_cast<int32_t>(aniIndex);
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

static ani_boolean TransferBackForwardListToStaticInner(
    ani_env* env, ani_class aniClass, ani_object output, ani_object input)
{
    if (!env) {
        return ANI_FALSE;
    }

    void* nativePtr = nullptr;
    if (!arkts_esvalue_unwrap(env, input, &nativePtr) || !nativePtr) {
        WVLOG_E("[TRANSFER] arkts_esvalue_unwrap failed");
        return ANI_FALSE;
    }

    WebHistoryList *webHistoryList = reinterpret_cast<WebHistoryList *>(nativePtr);
    if (!AniParseUtils::Wrap(env, output, ANI_BACK_FORWARD_LIST_INNER_CLASS_NAME,
                             reinterpret_cast<ani_long>(webHistoryList))) {
        WVLOG_E("[TRANSFER] BackForwardList wrap failed");
        return ANI_FALSE;
    }
    webHistoryList->IncRefCount();
    return ANI_TRUE;
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
        ani_native_function { "transferBackForwardListToStaticInner", nullptr,
                              reinterpret_cast<void *>(TransferBackForwardListToStaticInner) },
    };

    status = env->Class_BindNativeMethods(backForwardListCls, methodArray.data(), methodArray.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }

    return ANI_OK;
}

static void Close(ani_env* env, ani_object object)
{
    WVLOG_D("[WebMessagePort] close message port");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    WebMessagePort* msgPort = reinterpret_cast<WebMessagePort*>(AniParseUtils::Unwrap(env, object));
    if (!msgPort) {
        WVLOG_E("close message port failed, napi unwrap msg port failed");
        return;
    }
    ErrCode ret = msgPort->ClosePort();
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
    return;
}

static ani_boolean TransferWebMessagePortToStaticInner(ani_env* env, ani_class aniClass, ani_object output,
                                                       ani_object input, ani_boolean extType)
{
    if (!env) {
        WVLOG_E("[TRANSFER] env is nullptr");
        return ANI_FALSE;
    }

    void* nativePtr = nullptr;
    if (!arkts_esvalue_unwrap(env, input, &nativePtr) || !nativePtr) {
        WVLOG_E("[TRANSFER] arkts_esvalue_unwrap failed");
        return ANI_FALSE;
    }

    WebMessagePort* msgPort = reinterpret_cast<WebMessagePort *>(nativePtr);
    if (!AniParseUtils::Wrap(env, output, ANI_WEB_MESSAGE_PORT_INNER_CLASS_NAME,
                             reinterpret_cast<ani_long>(msgPort))) {
        WVLOG_E("[TRANSFER] WebMessagePort wrap failed");
        return ANI_FALSE;
    }
    msgPort->IncRefCount();

    bool isExtentionType = static_cast<bool>(extType);
    ani_object jsType = {};
    if (!AniParseUtils::CreateBoolean(env, isExtentionType, jsType)) {
        return ANI_FALSE;
    }
    if (env->Object_SetPropertyByName_Ref(output, "isExtentionType", static_cast<ani_ref>(jsType)) != ANI_OK) {
        WVLOG_E("[TRANSFER] set isExtentionType failed");
        return ANI_FALSE;
    }
    return ANI_TRUE;
}

bool PostMessageEventMsgHandler(
    ani_env* env, ani_object messageObj, bool isString, bool isArrayBuffer, std::shared_ptr<NWebMessage> webMsg)
{
    WVLOG_D("[WebMessagePort] PostMessageEventMsgHandler");
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if (isString) {
        std::string message;
        AniParseUtils::ParseString(env, messageObj, message);
        webMsg->SetType(NWebValue::Type::STRING);
        webMsg->SetString(message);
    } else if (isArrayBuffer) {
        size_t byteLength = 0;
        uint8_t* arrayBuffer = nullptr;
        if (auto status = env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(messageObj), (void**)&arrayBuffer,
            &byteLength) != ANI_OK) {
            WVLOG_E("[WebMessagePort] ArrayBuffer_GetInfo failed, status is : %{public}d", status);
            return false;
        }
        std::vector<uint8_t> vecData(arrayBuffer, arrayBuffer + byteLength);
        webMsg->SetType(NWebValue::Type::BINARY);
        webMsg->SetBinary(vecData);
    }
    return true;
}

static void PostMessageEvent(ani_env* env, ani_object object, ani_object messageObj)
{
    WVLOG_D("[WebMessagePort] (PostMessageEvent) Start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_class arrayBufferClass;
    if (auto status = env->FindClass("Lescompat/ArrayBuffer;", &arrayBufferClass) != ANI_OK) {
        WVLOG_E("[WebMessagePort] Find class %{public}s failed, status is %{public}d.", "ArrayBuffer", status);
        return;
    }
    ani_boolean isArrayBuffer;
    if (auto status = env->Object_InstanceOf(messageObj, arrayBufferClass, &isArrayBuffer) != ANI_OK) {
        WVLOG_E("[WebMessagePort] Find type %{public}s failed, status is %{public}d.", "ArrayBuffer", status);
        return;
    }
    bool isString = AniParseUtils::IsString(env, messageObj);
    if (!isString && !isArrayBuffer) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        WVLOG_E("[WebMessagePort] (PostMessageEvent) Type Error");
        return;
    }
    auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
    if (!PostMessageEventMsgHandler(env, messageObj, isString, isArrayBuffer, webMsg)) {
        WVLOG_E("[WebMessagePort] PostMessageEventMsgHandler failed");
        return;
    }
    WebMessagePort* msgPort = reinterpret_cast<WebMessagePort*>(AniParseUtils::Unwrap(env, object));
    if (!msgPort) {
        WVLOG_E("[WebMessagePort] WebMessagePort failed");
        return;
    }
    ErrCode ret = msgPort->PostPortMessage(webMsg);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

static void PostMessageEventExt(ani_env* env, ani_object object, ani_object messageObj)
{
    WVLOG_D("[WebMessagePort] (PostMessageEventExt) Start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_class cls;
    if (auto status = env->FindClass("L@ohos/web/webview/webview/WebMessageExt;", &cls) != ANI_OK) {
        WVLOG_E("[WebMessagePort] Find class %{public}s failed, status is %{public}d.", "WebMessageExt", status);
        return;
    }
    ani_boolean isWebMessageExt;
    if (auto status = env->Object_InstanceOf(messageObj, cls, &isWebMessageExt) != ANI_OK) {
        WVLOG_E("[WebMessagePort] Find type %{public}s failed, status is %{public}d.", "WebMessageExt", status);
        return;
    }
    if (!isWebMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        WVLOG_E("[WebMessagePort] (PostMessageEventExt) Type Error");
        return;
    }

    WebMessageExt* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, messageObj));
    if (!webMessageExt) {
        WVLOG_E("[WebMessagePort] WebMessageExt failed");
        return;
    }
    WebMessagePort* msgPort = reinterpret_cast<WebMessagePort*>(AniParseUtils::Unwrap(env, object));
    if (!msgPort) {
        WVLOG_E("[WebMessagePort] WebMessagePort failed");
        return;
    }
    if (!msgPort->IsExtentionType()) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::PARAM_TYPE_INVALID, "message"));
        return;
    }
    ErrCode ret = msgPort->PostPortMessage(webMessageExt->GetData());
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

static void OnMessageEvent(ani_env* env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("[WebMessagePort] (OnMessageEvent) callback");
    if (!env) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return;
    }

    auto callbackImpl = std::make_shared<NWebValueCallbackImpl>(g_vmWebMessagePort, callback, false);
    WebMessagePort* msgPort = reinterpret_cast<WebMessagePort*>(AniParseUtils::Unwrap(env, object));
    if (!msgPort) {
        WVLOG_E("[WebMessagePort] Onmessage Unwrap failed");
        return;
    }
    ErrCode ret = msgPort->SetPortMessageCallback(callbackImpl);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

static void OnMessageEventExt(ani_env* env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("[WebMessagePort] (OnMessageEventExt) callback");
    if (!env) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return;
    }

    auto callbackImpl = std::make_shared<NWebValueCallbackImpl>(g_vmWebMessagePort, callback, true);
    WebMessagePort* msgPort = reinterpret_cast<WebMessagePort*>(AniParseUtils::Unwrap(env, object));
    if (!msgPort) {
        WVLOG_E("[WebMessagePort] OnMessage Unwrap failed");
        return;
    }
    ErrCode ret = msgPort->SetPortMessageCallback(callbackImpl);
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
}

ani_status StsWebMessagePortInit(ani_env* env)
{
    WVLOG_D("[WebMessagePort] StsWebMessagePortInit");
    if (!env) {
        WVLOG_E("[WebMessagePort] env is nullptr");
        return ANI_ERROR;
    }
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    g_vmWebMessagePort = vm;
    ani_class webMessagePortCls = nullptr;
    ani_status status = env->FindClass(ANI_WEB_MESSAGE_PORT_INNER_CLASS_NAME, &webMessagePortCls);
    if (status != ANI_OK || !webMessagePortCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_WEB_MESSAGE_PORT_INNER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array methodArray = {
        ani_native_function { "close", nullptr, reinterpret_cast<void*>(Close) },
        ani_native_function { "postMessageEvent", nullptr, reinterpret_cast<void*>(PostMessageEvent) },
        ani_native_function { "onMessageEvent", nullptr, reinterpret_cast<void*>(OnMessageEvent) },
        ani_native_function { "postMessageEventExt", nullptr, reinterpret_cast<void*>(PostMessageEventExt) },
        ani_native_function { "onMessageEventExt", nullptr, reinterpret_cast<void*>(OnMessageEventExt) },
        ani_native_function { "transferWebMessagePortToStaticInner", nullptr,
                              reinterpret_cast<void *>(TransferWebMessagePortToStaticInner) },
    };
    status = env->Class_BindNativeMethods(webMessagePortCls, methodArray.data(), methodArray.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
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

static bool CreateWebMessagePortObj(
    ani_env* env, ani_object& obj, int32_t nwebId, std::string& port, bool bIsExtentionType)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if (!AniParseUtils::CreateObjectVoid(env, ANI_WEB_MESSAGE_PORT_INNER_CLASS_NAME, obj)) {
        WVLOG_E("CreateObjectVoid failed");
        return false;
    }
    WebMessagePort* webMessagePort = new (std::nothrow) WebMessagePort(nwebId, port, bIsExtentionType);
    if (!webMessagePort) {
        WVLOG_E("new WebMessagePort failed");
        return false;
    }
    if (!AniParseUtils::Wrap(
            env, obj, ANI_WEB_MESSAGE_PORT_INNER_CLASS_NAME, reinterpret_cast<ani_long>(webMessagePort))) {
        WVLOG_E("WebMessagePort wrap failed");
        delete webMessagePort;
        webMessagePort = nullptr;
        return false;
    }
    webMessagePort->IncRefCount();
    return true;
}

static ani_object CreateWebMessagePortsObj(
    ani_env* env, ani_object isExtentionType, int32_t nwebId, std::vector<std::string>& ports)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_boolean isUndefined = ANI_TRUE;
    ani_boolean bIsExtentionType = ANI_FALSE;
    env->Reference_IsUndefined(isExtentionType, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        if (env->Object_CallMethodByName_Boolean(isExtentionType, "unboxed", nullptr, &bIsExtentionType) != ANI_OK) {
            AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
                NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "isExtentionType", "boolean"));
            return nullptr;
        }
    }

    ani_object arrayObj = nullptr;
    ani_class arrayCls = nullptr;
    if (env->FindClass("Lescompat/Array;", &arrayCls) != ANI_OK) {
        WVLOG_E("find class escompat/Array; failed");
        return nullptr;
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(arrayCls, "<ctor>", "I:V", &arrayCtor) != ANI_OK) {
        WVLOG_E("get ctor method failed");
        return nullptr;
    }
    if (env->Object_New(arrayCls, arrayCtor, &arrayObj, ports.size()) != ANI_OK) {
        WVLOG_E("Object_New Array failed");
        return nullptr;
    }

    for (size_t i = 0; i < ports.size(); ++i) {
        ani_object obj = {};
        if (!CreateWebMessagePortObj(env, obj, nwebId, ports[i], static_cast<bool>(bIsExtentionType))) {
            return nullptr;
        }
        if ((isUndefined != ANI_TRUE) &&
            (ANI_OK != env->Object_SetPropertyByName_Ref(obj, "isExtentionType", isExtentionType))) {
            WVLOG_E("set NWebWeMessagePort failed");
            return nullptr;
        }
        if (env->Object_CallMethodByName_Void(arrayObj, "$_set", "ILstd/core/Object;:V", i, obj) != ANI_OK) {
            WVLOG_E("Object_CallMethodByName_Void failed");
            return nullptr;
        }
    }
    return arrayObj;
}

static ani_object CreateWebMessagePorts(ani_env* env, ani_object object, ani_object isExtentionType)
{
    ani_object result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        WVLOG_E("create message port failed, napi unwrap webviewController failed");
        return result;
    }

    int32_t nwebId = controller->GetWebId();
    std::vector<std::string> ports = controller->CreateWebMessagePorts();
    if (ports.size() != INTEGER_TWO) {
        WVLOG_E("create web message port failed");
        return result;
    }
    return CreateWebMessagePortsObj(env, isExtentionType, nwebId, ports);
}

static void SetConnectionTimeout(ani_env* env, ani_object object, ani_int aniTimeout)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    int32_t timeout = static_cast<int32_t>(aniTimeout);
    if (timeout <= 0) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    NWebHelper::Instance().SetConnectionTimeout(timeout);
    return;
}

static void BackOrForward(ani_env* env, ani_object object, ani_int step)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    ErrCode ret = controller->BackOrForward(static_cast<int32_t>(step));
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
    }
    return;
}

static void SetWebDebuggingAccess(ani_env* env, ani_object object, ani_boolean aniDebugAccess)
{
    WVLOG_D(" SetWebDebuggingAccess start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    bool webDebuggingAccess = static_cast<bool>(aniDebugAccess);
    int32_t webDebuggingPort = 0;
    if (WebviewController::webDebuggingAccess_ != webDebuggingAccess ||
        WebviewController::webDebuggingPort_ != webDebuggingPort) {
        NWebHelper::Instance().SetWebDebuggingAccess(webDebuggingAccess);
    }
    WebviewController::webDebuggingAccess_ = webDebuggingAccess;
    WebviewController::webDebuggingPort_ = webDebuggingPort;
    return;
}

static void SetWebDebuggingAccessAndPort(
    ani_env* env, ani_object object, ani_boolean aniDebugAccess, ani_int aniDebugPort)
{
    WVLOG_D(" SetWebDebuggingAccess start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    bool webDebuggingAccess = static_cast<bool>(aniDebugAccess);
    int32_t webDebuggingPort = static_cast<int32_t>(aniDebugPort);
    const int32_t kValidPortRangeStart = 1025;
    const int32_t kValidPortRangeEnd = 65535;
    if (webDebuggingPort < kValidPortRangeStart ||
        webDebuggingPort > kValidPortRangeEnd) {
        AniBusinessError::ThrowErrorByErrCode(env, NOT_ALLOWED_PORT);
        return;
    }
    if (WebviewController::webDebuggingAccess_ != webDebuggingAccess ||
        WebviewController::webDebuggingPort_ != webDebuggingPort) {
        NWebHelper::Instance().SetWebDebuggingAccessAndPort(webDebuggingAccess, webDebuggingPort);
    }
    WebviewController::webDebuggingAccess_ = webDebuggingAccess;
    WebviewController::webDebuggingPort_ = webDebuggingPort;
    return;
}

static void EnableSafeBrowsing(ani_env* env, ani_object object, ani_boolean enable)
{
    WVLOG_D("[WebviewCotr] EnableSafeBrowsing");
    if (!env) {
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

static ani_int GetPageHeight(ani_env *env, ani_object object)
{
    int32_t pageHeight = 0;
    if (!env) {
        WVLOG_E("env is nullptr");
        return static_cast<ani_int>(pageHeight);
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return static_cast<ani_int>(pageHeight);
    }

    pageHeight = controller->GetPageHeight();
    return static_cast<ani_int>(pageHeight);
}

static ani_boolean TerminateRenderProcess(ani_env* env, ani_object object)
{
    WVLOG_D("TerminateRenderProcess");
    if (!env) {
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
    if (!env) {
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
    env->FindEnum("@ohos.web.webview.webview.RenderProcessMode", &enumType);

    renderProcessMode = static_cast<ani_int>(NWebHelper::Instance().GetRenderProcessMode());
    WVLOG_I("getRenderProcessMode mode = %{public}d", static_cast<int32_t>(renderProcessMode));
    ani_enum_item mode;
    env->Enum_GetEnumItemByIndex(enumType, renderProcessMode, &mode);
    return mode;
}

static void PauseAllTimers(ani_env* env, ani_object object)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_I("PauseAllTimers");
    NWebHelper::Instance().PauseAllTimers();
    return;
}

static void ResumeAllTimers(ani_env* env, ani_object object)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_I("ResumeAllTimers");
    NWebHelper::Instance().ResumeAllTimers();
    return;
}

static void SetCustomUserAgent(ani_env* env, ani_object object, ani_object userAgentObj)
{
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    if (!env) {
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
    return buffer;
}

static void RestoreWebState(ani_env* env, ani_object object, ani_object value)
{
    WVLOG_D("WebviewController RestoreWebState start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webviewController = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!webviewController || !webviewController->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    char* data = nullptr;
    size_t total = 0;
    size_t offset = 0;
    size_t length = 0;
    if (env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(value), reinterpret_cast<void**>(&data), &total) !=
        ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo failed");
        return;
    }
    length = std::min<size_t>(length, total - offset);
    std::vector<uint8_t> state(length);
    int retCode = memcpy_s(state.data(), state.size(), &data[offset], length);
    if (retCode != 0) {
        return;
    }
    webviewController->RestoreWebState(state);
}

static void TrimMemoryByPressureLevel(ani_env *env, ani_object object, ani_double level)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    int32_t memoryLevel = static_cast<int32_t>(level) == 1 ? 0 : static_cast<int32_t>(level);
    NWebHelper::Instance().TrimMemoryByPressureLevel(memoryLevel);
    return;
}

static void SetPathAllowingUniversalAccess(ani_env* env, ani_object object, ani_object pathList)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::vector<std::string> pathListArr;
    if (!AniParseUtils::ParseStringArray(env, pathList, pathListArr)) {
        WVLOG_E("SetPathAllowingUniversalAccess ParseStringArray fail");
        return;
    }

    std::string errorPath;
    controller->SetPathAllowingUniversalAccess(pathListArr, errorPath);
    if (!errorPath.empty()) {
        WVLOG_E("%{public}s is invalid.", errorPath.c_str());
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString("BusinessError 401: Parameter error. Path: '%s' is invalid", errorPath.c_str()));
        return;
    }
}

static void EnableWholeWebPageDrawing(ani_env* env, ani_object object)
{
    NWebHelper::Instance().EnableWholeWebPageDrawing();
}

static ani_string GetSurfaceId(ani_env* env, ani_object object)
{
    ani_string result = nullptr;
    if (!env) {
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
    if (!env) {
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
    WVLOG_D("AddResourceToMemoryCache begin");
    if (!env) {
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
    ani_array resourceMapsRef;
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!object) {
        WVLOG_E("object is nullptr");
        return;
    }
    resourceMapsRef = static_cast<ani_array>(resourceMaps);
    for (ani_int i = 0; i < resourceMapsCount; i++) {
        ani_ref resourceMapItem = nullptr;
        env->Array_Get(resourceMapsRef, i, &resourceMapItem);
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
    if (!env) {
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
    WVLOG_D("ClearPrefetchedResource invoked.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_array cacheKeyStr = nullptr;
    cacheKeyStr = static_cast<ani_array>(cacheKey);
    ani_double cacheKeyCount;
    env->Object_GetPropertyByName_Double(cacheKey, "length", &cacheKeyCount);
    std::vector<std::string> cacheKeyList;
    for (ani_double i = 0; i < cacheKeyCount; i++) {
        ani_ref pathItem = nullptr;
        env->Array_Get(cacheKeyStr, i, &pathItem);
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

static std::shared_ptr<NWebEnginePrefetchArgs> ParsePrefetchArgs(ani_env* env, ani_object object, ani_object request)
{
    if (!env) {
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
    if (!env) {
        return;
    }
    std::shared_ptr<NWebEnginePrefetchArgs> prefetchArgs = ParsePrefetchArgs(env, object, request);
    if (!prefetchArgs) {
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
    ani_int cacheValidTimeTemp = 300;
    if (!isUndefined &&
        (env->Object_CallMethodByName_Int(cacheValidTime, "unboxed", ":i", &cacheValidTimeTemp) != ANI_OK)) {
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
}

static void StartCamera(ani_env* env, ani_object object)
{
    WVLOG_D("StartCamera begin");
    if (!env) {
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
    WVLOG_D("CloseAllMediaPresentations begin");
    if (!env) {
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
    WVLOG_D("StopAllMedia begin");
    if (!env) {
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
    WVLOG_D("StopCamera begin");
    if (!env) {
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
    WVLOG_D("CloseCamera begin");
    if (!env) {
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
    WVLOG_D("PauseAllMedia begin");
    if (!env) {
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
    WVLOG_D("ResumeAllMedia begin");
    if (!env) {
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
    WVLOG_D("SetAudioMuted begin");
    if (!env) {
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
    WVLOG_D("GetMediaPlaybackState begin");
    if (!env) {
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
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_vm *vm = nullptr;
    env->GetVM(&vm);
    g_vm = vm;
    WVLOG_D("put on_create_native_media_player callback");

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    controller->OnCreateNativeMediaPlayer(g_vm, callback);
}

bool ParseJsLengthDoubleToInt(ani_env* env, ani_ref ref, int32_t& outValue)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_double value = 0;
    ani_status status = env->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "unboxed", ":d", &value);
    if (status != ANI_OK) {
        WVLOG_E("ParseJsLengthDoubleToInt failed status: %{public}d ",status);
        return false;
    }
    outValue = static_cast<int32_t>(value);
    return true;
}

bool ParseJsLengthResourceToInt(ani_env* env, ani_object jsLength, PixelUnit& type, int32_t& result)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_double resIdDouble;
    if ((env->Object_GetPropertyByName_Double(jsLength, "id", &resIdDouble) != ANI_OK)) {
        return false;
    }
    int32_t resId = static_cast<int32_t>(resIdDouble);
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
        ParseJsLengthDoubleToInt(env, jsResourceType, resourceTypeNum);
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
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if ((!AniParseUtils::IsResource(env, jsLength)) && (!AniParseUtils::IsString(env, jsLength)) &&
        (!AniParseUtils::IsDouble(env, jsLength))) {
        WVLOG_E("WebPageSnapshot Unable to parse js length object.");
        return false;
    }
    if (AniParseUtils::IsDouble(env, jsLength)) {
         ParseJsLengthDoubleToInt(env, jsLength, result);
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
    if (AniParseUtils::IsResource(env, jsLength)) {
        return ParseJsLengthResourceToInt(env, jsLength, type, result);
    }
    return false;
}

static void JsErrorCallback(ani_env* env, ani_ref jsCallback, int32_t err)
{
    if (!env) {
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

bool CreateSizeObject(ani_env* env, const char* className, ani_object& object, ani_int size)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_status status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", className, status);
        return false;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", "i:", &ctor)) != ANI_OK) {
        WVLOG_E("get %{public}s ctor method failed, status: %{public}d", className, status);
        return false;
    }
    if ((status = env->Object_New(cls, ctor, &object, size)) != ANI_OK) {
        WVLOG_E("new %{public}s failed, status: %{public}d", className, status);
        return false;
    }
    return true;
}

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
        int returnJsWidth = 0;
        int returnJsHeight = 0;
        if (radio > 0) {
            returnJsWidth = returnWidth / radio;
            returnJsHeight = returnHeight / radio;
        }
        if (check) {
            if (std::abs(returnJsWidth - inputWidth) < INTEGER_THREE) {
                returnJsWidth = inputWidth;
            }

            if (std::abs(returnJsHeight - inputHeight) < INTEGER_THREE) {
                returnJsHeight = inputHeight;
            }
        }
        ani_object jsSzieObj = {};
        if (AniParseUtils::CreateObjectVoid(env, ANI_SIZE_OPTIONS_CLASS_NAME, jsSzieObj) == false) {
            WVLOG_E("jsResult CreateObjectVoid failed");
            return;
        }
        ani_object jsWidthObj = {};
        if (CreateSizeObject(env, "std.core.Int", jsWidthObj, static_cast<ani_int>(returnJsWidth)) == false) {
            WVLOG_E("jsWidthObj CreateSizeObject failed");
            return;
        }
        ani_object jsHeightObj = {};
        if (CreateSizeObject(env, "std.core.Int", jsHeightObj, static_cast<ani_int>(returnJsHeight)) == false) {
            WVLOG_E("jsHeightObj CreateSizeObject failed");
            return;
        }
        auto status = env->Object_SetPropertyByName_Ref(jsSzieObj, "width", static_cast<ani_ref>(jsWidthObj));
        if (status != ANI_OK) {
            WVLOG_E("returnJsWidth set failed : %{public}d", status);
            return;
        }
        if (env->Object_SetPropertyByName_Ref(jsSzieObj, "height", static_cast<ani_ref>(jsHeightObj)) != ANI_OK) {
            WVLOG_E("returnJsHeight set failed");
            return;
        }
        if (env->Object_SetPropertyByName_Ref(jsResult, "size", jsSzieObj) != ANI_OK) {
            WVLOG_E("jsSzieObj set failed");
            return;
        }
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
    if (!env) {
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
    if (!env) {
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

static void InnerCompleteWindowNew(ani_env* env, ani_object object, ani_int parentNWebId)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("InnerCompleteWindow webviewController is nullptr");
        return;
    }
    controller->InnerCompleteWindowNew(parentNWebId);
}

ani_object PrecompileJavaScriptPromise(ani_env* env, ani_object object, std::string url, std::string script,
    std::shared_ptr<OHOS::NWeb::CacheOptions> cacheOptions)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WVLOG_D("PrecompileJavaScript Begin");
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    ani_resolver resolver {};
    ani_object promise {};
    ani_status status = env->Promise_New(&resolver, &promise);
    if (status != ANI_OK) {
        WVLOG_E("promise_new failed");
        return nullptr;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebPrecompileCallback>();
    callbackImpl->SetCallback([env, resolver](int64_t result) -> void {
        if (!env) {
            WVLOG_E("env is nullptr");
            return;
        }
        ani_object resolution = AniParseUtils::CreateInt(env, result);
        if (result == static_cast<int64_t>(PrecompileError::OK)) {
            if (env->PromiseResolver_Resolve(resolver, resolution) != ANI_OK) {
                WVLOG_E("PromiseResolver_Resolve fail");
            }
        } else {
            if (env->PromiseResolver_Reject(resolver, static_cast<ani_error>(resolution)) != ANI_OK) {
                WVLOG_E("PromiseResolver_Reject fail");
            }
        }
    });

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("PrecompileJavaScript controller fail");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    controller->PrecompileJavaScript(url, script, cacheOptions, callbackImpl);
    return promise;
}

ani_object PrecompileJavaScript(
    ani_env* env, ani_object object, ani_object url, ani_object script, ani_object cacheOptions)
{
    ani_object result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }
    std::string urlStr;
    if (!AniParseUtils::ParseString(env, url, urlStr)) {
        WVLOG_E("Parse url failed.");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "url", "string"));
        return result;
    }
    auto cacheOptionsPtr = AniParseUtils::ParseCacheOptions(env, cacheOptions);

    ani_class ArrayBufferCls;
    env->FindClass("escompat.ArrayBuffer", &ArrayBufferCls);
    ani_boolean isArrayBuffer;
    std::string scriptStr;
    ani_status res = env->Object_InstanceOf(script, ArrayBufferCls, &isArrayBuffer);
    if (res != ANI_OK) {
        WVLOG_E("Object_InstanceOf failed");
        return result;
    }
    if (isArrayBuffer) {
        uint8_t* arrayBuffer = nullptr;
        ani_size byteLength;
        if (env->ArrayBuffer_GetInfo(reinterpret_cast<ani_arraybuffer>(script), reinterpret_cast<void**>(&arrayBuffer),
                &byteLength) != ANI_OK) {
            WVLOG_E("ArrayBuffer_GetInfo failed");
            AniBusinessError::ThrowErrorByErrCode(env, NWebError::PARAM_CHECK_ERROR);
            return result;
        }
        std::vector<uint8_t> postData(arrayBuffer, arrayBuffer + byteLength);
        scriptStr = std::string(postData.begin(), postData.end());
    } else {
        if (!AniParseUtils::ParseString(env, script, scriptStr)) {
            WVLOG_E("PrecompileJavaScript :script must be string or Uint8Array");
            return result;
        }
    }
    return PrecompileJavaScriptPromise(env, object, urlStr, scriptStr, cacheOptionsPtr);
}

static void ConstructorExt(ani_env* env, ani_object object)
{
    if (g_WebMessagePort) {
        if (!env) {
            WVLOG_E("env is nullptr");
            return;
        }

        auto webMsg = std::make_shared<OHOS::NWeb::NWebMessage>(NWebValue::Type::NONE);
        WebMessageExt* webMessageExt = new (std::nothrow) WebMessageExt(webMsg);
        if (!webMessageExt) {
            WVLOG_E("new webMessageExt failed");
            return;
        }
        if (!AniParseUtils::Wrap(env, object, ANI_WEB_MESSAGE_EXT_NAME, reinterpret_cast<ani_long>(webMessageExt))) {
            WVLOG_E("webview webMessageExt wrap failed");
            delete webMessageExt;
            webMessageExt = nullptr;
        }
    }
}

static void SetType(ani_env* env, ani_object object, ani_enum_item value)
{
    WVLOG_D("WebMessageExt SetType start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_int iValue;
    if (env->EnumItem_GetValue_Int(value, &iValue) != ANI_OK) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }

    int32_t typeValue = static_cast<int32_t>(iValue);
    webMessageExt->SetType(typeValue);
}

static void SetString(ani_env* env, ani_object object, ani_string aniValue)
{
    WVLOG_D("WebMessageExt SetString start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::string value;
    if (!AniParseUtils::ParseString(env, aniValue, value)) {
        WVLOG_E("Parse url failed.");
        return;
    }
    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::STRING)) {
        WVLOG_E("web message SetString error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }

    webMessageExt->SetString(value);
}

static void SetNumber(ani_env* env, ani_object object, ani_double value)
{
    WVLOG_D("WebMessageExt SetNumber start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::NUMBER)) {
        WVLOG_E("web message SetNumber error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }
    webMessageExt->SetNumber(static_cast<double>(value));
}

static void SetBoolean(ani_env* env, ani_object object, ani_boolean value)
{
    WVLOG_D("WebMessageExt SetBoolean start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::BOOLEAN)) {
        WVLOG_E("web message SetBoolean error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }

    webMessageExt->SetBoolean(static_cast<bool>(value));
}

static void SetArrayBuffer(ani_env* env, ani_object object, ani_object value)
{
    WVLOG_D("WebMessageExt SetArrayBuffer start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    char* arrayBuffer = nullptr;
    size_t byteLength = 0;
    if (env->ArrayBuffer_GetInfo(
            reinterpret_cast<ani_arraybuffer>(value), reinterpret_cast<void**>(&arrayBuffer), &byteLength) != ANI_OK) {
        WVLOG_E("ArrayBuffer_GetInfo failed");
        return;
    }
    std::vector<uint8_t> vecData(arrayBuffer, arrayBuffer + byteLength);

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAYBUFFER)) {
        WVLOG_E("web message SetArrayBuffer error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }
    webMessageExt->SetArrayBuffer(vecData);
}

static void HandleStringArray(ani_env* env, ani_object array, WebMessageExt* webMessageExt)
{
    std::vector<std::string> strVec;
    if (!AniParseUtils::ParseStringArray(env, array, strVec)) {
        WVLOG_E("WebMessageExt ParseStringArray error");
        return;
    }
    webMessageExt->SetStringArray(strVec);
}

static void HandleBooleanArray(ani_env* env, ani_object array, WebMessageExt* webMessageExt)
{
    std::vector<bool> boolVec;
    if (!AniParseUtils::ParseBooleanArray(env, array, boolVec)) {
        WVLOG_E("WebMessageExt ParseBooleanArray error");
        return;
    }
    webMessageExt->SetBooleanArray(boolVec);
}

static void HandleDoubleArray(ani_env* env, ani_object array, WebMessageExt* webMessageExt)
{
    std::vector<double> doubleVec;
    if (!AniParseUtils::ParseDoubleArray(env, array, doubleVec)) {
        WVLOG_E("WebMessageExt ParseDoubleArray error");
        return;
    }
    webMessageExt->SetDoubleArray(doubleVec);
}

static void HandleInt64Array(ani_env* env, ani_object array, WebMessageExt* webMessageExt)
{
    std::vector<int64_t> intVec;
    if (!AniParseUtils::ParseInt64Array(env, array, intVec)) {
        WVLOG_E("WebMessageExt ParseInt64Array error");
        return;
    }
    webMessageExt->SetInt64Array(intVec);
}

static void SetArray(ani_env* env, ani_object object, ani_object array)
{
    WVLOG_D("WebMessageExt SetArray start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
        WVLOG_E("web message SetArray error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }

    ani_ref element = nullptr;
    if (env->Array_Get(static_cast<ani_array>(array), 0, &element) != ANI_OK) {
        WVLOG_E("WebMessageExt get element from array error");
        return;
    }

    ani_object elementObj = static_cast<ani_object>(element);
    if (AniParseUtils::IsString(env, elementObj)) {
        HandleStringArray(env, array, webMessageExt);
    } else if (AniParseUtils::IsBoolean(env, elementObj)) {
        HandleBooleanArray(env, array, webMessageExt);
    } else if (AniParseUtils::IsDouble(env, elementObj)) {
        HandleDoubleArray(env, array, webMessageExt);
    } else if (AniParseUtils::IsInteger(env, elementObj)) {
        HandleInt64Array(env, array, webMessageExt);
    } else {
        WVLOG_E("Unsupported array element type");
    }
}

static void SetError(ani_env* env, ani_object object, ani_object errorMsg)
{
    WVLOG_D("WebMessageExt setError start.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    ani_ref aniName;
    if (env->Object_GetPropertyByName_Ref(errorMsg, "name", &aniName) != ANI_OK) {
        return;
    }

    ani_ref aniMessage;
    if (env->Object_GetPropertyByName_Ref(errorMsg, "message", &aniMessage) != ANI_OK) {
        return;
    }
    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ERROR)) {
        WVLOG_E("web message setError error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return;
    }

    std::string name;
    if (!AniParseUtils::ParseString(env, aniName, name)) {
        return;
    }
    std::string message;
    if (!AniParseUtils::ParseString(env, aniMessage, message)) {
        return;
    }
    webMessageExt->SetError(name, message);
}

static ani_enum_item GetType(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetType.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        WVLOG_D("WebMessageExt GetType.");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    ani_int typeMode = 0;
    ani_enum enumType;
    env->FindEnum("@ohos.web.webview.webview.WebMessageType", &enumType);
    typeMode = static_cast<ani_int>(webMessageExt->GetType());
    WVLOG_D("WebMessageExt mode = %{public}d", static_cast<int32_t>(typeMode));
    ani_enum_item mode;
    env->Enum_GetEnumItemByIndex(enumType, typeMode, &mode);
    return mode;
}

static ani_string GetString(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetString.");
    ani_string result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::STRING)) {
        WVLOG_E("web message setError error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webMessageExt->GetData();
    if (!message) {
        WVLOG_E("message data is nullptr");
        return result;
    }

    std::string stringMsg = message->GetString();
    env->String_NewUTF8(stringMsg.c_str(), stringMsg.size(), &result);
    return result;
}

static ani_object GetNumber(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetNumber.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::NUMBER)) {
        WVLOG_E("web message GetNumber error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    auto message = webMessageExt->GetData();
    if (!message) {
        WVLOG_E("message data is nullptr");
        return nullptr;
    }

    return static_cast<ani_object>(AniParseUtils::ConvertNWebToAniValue(env, message));
}

static ani_boolean GetBoolean(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetBoolean.");
    if (!env) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return ANI_FALSE;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::BOOLEAN)) {
        WVLOG_E("web message GetBoolean error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return ANI_FALSE;
    }

    auto message = webMessageExt->GetData();
    if (!message) {
        WVLOG_E("message data is nullptr");
        return ANI_FALSE;
    }

    return static_cast<ani_boolean>(message->GetBoolean());
}

static ani_object GetArrayBuffer(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetArrayBuffer.");
    ani_object result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAYBUFFER)) {
        WVLOG_E("web message GetArrayBuffer error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webMessageExt->GetData();
    if (!message) {
        WVLOG_E("message data is nullptr");
        return result;
    }
    auto getArrayBuffer = message->GetBinary();

    void* arrayData = nullptr;
    ani_arraybuffer arraybuffer;
    ani_status status = env->CreateArrayBuffer(getArrayBuffer.size(), &arrayData, &arraybuffer);
    if (status != ANI_OK) {
        WVLOG_E("Create arraybuffer failed");
        return arraybuffer;
    }
    for (size_t i = 0; i < getArrayBuffer.size(); ++i) {
        *(uint8_t*)((uint8_t*)arrayData + i) = getArrayBuffer[i];
    }
    return arraybuffer;
}

ani_object ConvertToAniHandlerOfStringArray(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    if (!src) {
        WVLOG_E("src is nullptr");
        return nullptr;
    }
    std::vector<std::string> values = src->GetStringArray();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < values.size(); ++i) {
        ani_string result {};
        if (ANI_OK != env->String_NewUTF8(values[i].c_str(), values[i].size(), &result)) {
            continue;
        }
        if (ANI_OK != env->Array_Set(array, i, result)) {
            return array;
        }
    }
    return array;
}

ani_object ConvertToAniHandlerOfBooleanArray(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    if (!src) {
        WVLOG_E("src is nullptr");
        return nullptr;
    }
    std::vector<bool> values = src->GetBooleanArray();
    size_t valueSize = values.size();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < valueSize; i++) {
        ani_boolean item = static_cast<ani_boolean>(values[i]);
        ani_class booleanCls {};
        if (ANI_OK != env->FindClass("std.core.Boolean", &booleanCls)) {
            return nullptr;
        }
        ani_method ctor {};
        if (ANI_OK != env->Class_FindMethod(booleanCls, "<ctor>", "z:", &ctor)) {
            return nullptr;
        }
        ani_object obj {};
        if (env->Object_New(booleanCls, ctor, &obj, item) != ANI_OK) {
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, obj)) {
            return array;
        }
    }
    return array;
}

ani_object ConvertToAniHandlerOfDoubleArray(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    if (!src) {
        WVLOG_E("src is nullptr");
        return nullptr;
    }
    std::vector<double> values = src->GetDoubleArray();
    size_t valueSize = values.size();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < valueSize; i++) {
        ani_double item = static_cast<ani_double>(values[i]);
        ani_class cls {};
        if (ANI_OK != env->FindClass("std.core.Double", &cls)) {
            return nullptr;
        }
        ani_method ctor {};
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "d:", &ctor)) {
            return nullptr;
        }
        ani_object obj {};
        if (env->Object_New(cls, ctor, &obj, item) != ANI_OK) {
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, obj)) {
            return array;
        }
    }
    return array;
}

ani_object ConvertToAniHandlerOfInt64Array(ani_env* env, std::shared_ptr<NWebMessage> src)
{
    if (!src) {
        WVLOG_E("src is nullptr");
        return nullptr;
    }
    std::vector<int64_t> values = src->GetInt64Array();
    size_t valueSize = values.size();

    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("WebMessageExt GetUndefined Failed.");
        return nullptr;
    }

    ani_array array = nullptr;
    if (ANI_OK != env->Array_New(values.size(), undefinedRef, &array)) {
        WVLOG_E("WebMessageExt new array ref error.");
        return array;
    }

    for (size_t i = 0; i < valueSize; i++) {
        ani_long item = static_cast<ani_long>(values[i]);
        ani_class cls {};
        if (ANI_OK != env->FindClass("std.core.Long", &cls)) {
            return nullptr;
        }
        ani_method ctor {};
        if (ANI_OK != env->Class_FindMethod(cls, "<ctor>", "l:", &ctor)) {
            return nullptr;
        }
        ani_object obj {};
        if (env->Object_New(cls, ctor, &obj, item) != ANI_OK) {
            return nullptr;
        }
        if (ANI_OK != env->Array_Set(array, i, obj)) {
            return array;
        }
    }
    return array;
}

static ani_object GetArray(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetArray Start.");
    ani_object result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ARRAY)) {
        WVLOG_E("web message GetArray error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webMessageExt->GetData();
    if (!message) {
        WVLOG_E("web message failed.");
        return result;
    }
    NWebValue::Type type = message->GetType();
    if (type == NWebValue::Type::STRINGARRAY) {
        result = ConvertToAniHandlerOfStringArray(env, message);
        return result;
    }
    if (type == NWebValue::Type::BOOLEANARRAY) {
        result = ConvertToAniHandlerOfBooleanArray(env, message);
        return result;
    }
    if (type == NWebValue::Type::DOUBLEARRAY) {
        result = ConvertToAniHandlerOfDoubleArray(env, message);
        return result;
    }
    if (type == NWebValue::Type::INT64ARRAY) {
        result = ConvertToAniHandlerOfInt64Array(env, message);
        return result;
    }
    return result;
}

static ani_string GetError(ani_env* env, ani_object object)
{
    WVLOG_D("WebMessageExt GetError Start.");
    ani_string result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }

    auto* webMessageExt = reinterpret_cast<WebMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webMessageExt) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return result;
    }

    if (webMessageExt->GetType() != static_cast<int32_t>(WebMessageType::ERROR)) {
        WVLOG_E("web message GetError error type:%{public}d", webMessageExt->GetType());
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webMessageExt->GetData();
    if (!message) {
        WVLOG_E("message data is nullptr");
        return result;
    }
    std::string errMsg = message->GetErrName() + ": " + message->GetErrMsg();
    env->String_NewUTF8(errMsg.c_str(), errMsg.size(), &result);
    return result;
}

ani_status StsWebMessageExtInit(ani_env* env)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class webMessageExtCls = nullptr;
    ani_status status = env->FindClass(ANI_WEB_MESSAGE_EXT_NAME, &webMessageExtCls);
    if (status != ANI_OK || !webMessageExtCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_WEB_MESSAGE_EXT_NAME, status);
        return status;
    }
    std::array webMessageExtMethods = {
        ani_native_function { "<ctor>", nullptr, reinterpret_cast<void*>(ConstructorExt) },
        ani_native_function { "setType", nullptr, reinterpret_cast<void*>(SetType) },
        ani_native_function { "setString", nullptr, reinterpret_cast<void*>(SetString) },
        ani_native_function { "setNumber", nullptr, reinterpret_cast<void*>(SetNumber) },
        ani_native_function { "setBoolean", nullptr, reinterpret_cast<void*>(SetBoolean) },
        ani_native_function { "setArrayBuffer", nullptr, reinterpret_cast<void*>(SetArrayBuffer) },
        ani_native_function { "setArray", nullptr, reinterpret_cast<void *>(SetArray) },
        ani_native_function { "setError", nullptr, reinterpret_cast<void*>(SetError) },
        ani_native_function { "getType", nullptr, reinterpret_cast<void*>(GetType) },
        ani_native_function { "getString", nullptr, reinterpret_cast<void*>(GetString) },
        ani_native_function { "getNumber", nullptr, reinterpret_cast<void*>(GetNumber) },
        ani_native_function { "getBoolean", nullptr, reinterpret_cast<void*>(GetBoolean) },
        ani_native_function { "getArrayBuffer", nullptr, reinterpret_cast<void*>(GetArrayBuffer) },
        ani_native_function { "getArray", nullptr, reinterpret_cast<void *>(GetArray) },
        ani_native_function { "getError", nullptr, reinterpret_cast<void*>(GetError) },
    };
    status = env->Class_BindNativeMethods(webMessageExtCls, webMessageExtMethods.data(), webMessageExtMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }
    return status;
}

bool ParseRegisterJavaScriptProxyParam(ani_env* env, ani_object jsObject, ani_string name, ani_object methodList,
    ani_object asyncMethodList, ani_object permission, AniRegisterJavaScriptProxyParam* param)
{
    std::string objName;
    if (!AniParseUtils::ParseString(env, name, objName)) {
        AniBusinessError::ThrowError(
            env, PARAM_CHECK_ERROR, NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "name", "string"));
        return false;
    }

    std::vector<std::string> jsMethodList;
    if (!AniParseUtils::ParseStringArray(env, methodList, jsMethodList)) {
        WVLOG_E("ParseRegisterJavaScriptProxyParam build jsMethodList error");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return false;
    }
    WVLOG_I("jsMethodList size = %{public}d", jsMethodList.size());
    std::vector<std::string> jsAsyncMethodList;
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(asyncMethodList, &isUndefined) != ANI_OK) {
        return false;
    }
    WVLOG_I("ParseRegisterJavaScriptProxyParam jsAsyncMethodList is undefined? : %{public}d", isUndefined);
    if (!isUndefined && !AniParseUtils::ParseStringArray(env, asyncMethodList, jsAsyncMethodList)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return false;
    }
    WVLOG_I("jsAsyncMethodList size = %{public}d", jsAsyncMethodList.size());
    std::string jsPermission;
    if (env->Reference_IsUndefined(permission, &isUndefined) != ANI_OK) {
        return false;
    }
    WVLOG_I("ParseRegisterJavaScriptProxyParam Permission is undefined? : %{public}d", isUndefined);
    if (!isUndefined && !AniParseUtils::ParseString(env, static_cast<ani_ref>(permission), jsPermission)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return false;
    }

    param->env = env;
    param->obj = jsObject;
    param->objName = objName;
    param->syncMethodList = jsMethodList;
    param->asyncMethodList = jsAsyncMethodList;
    param->permission = jsPermission;
    return true;
}

static void RegisterJavaScriptProxy(ani_env* env, ani_object object, ani_object jsObject, ani_string name,
    ani_object methodList, ani_object asyncMethodList, ani_object permission)
{
    WVLOG_D("enter RegisterJavaScriptProxy");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    AniRegisterJavaScriptProxyParam param;
    if (!ParseRegisterJavaScriptProxyParam(env, jsObject, name, methodList, asyncMethodList, permission, &param)) {
        WVLOG_E("bulid AniRegisterJavaScriptProxyParam error");
        return;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    controller->SetNWebJavaScriptResultCallBack();
    controller->RegisterJavaScriptProxy(param);
    WVLOG_I("exit RegisterJavaScriptProxy");
    return;
}

static void DeleteJavaScriptRegister(ani_env* env, ani_object object, ani_string name)
{
    std::string objName;
    if (!AniParseUtils::ParseString(env, name, objName)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    ErrCode ret = controller->DeleteJavaScriptRegister(objName, {});
    if (ret != NO_ERROR) {
        AniBusinessError::ThrowErrorByErrCode(env, ret);
        return;
    }
    WVLOG_I("DeleteJavaScriptRegister successful!");
    return;
}

ErrCode ConstructFlowbuf(ani_env* env, ani_object script, int& fd, size_t& scriptLength)
{
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        return NWebError::NEW_OOM;
    }
    flowbufferAdapter->StartPerformanceBoost();
    std::string scriptStr;
    if (AniParseUtils::IsString(env, script)) {
        if (!AniParseUtils::ParseString(env, script, scriptStr)) {
            WVLOG_I("script is string constructstringflowbuf");
            return AniParseUtils::ConstructStringFlowbuf(env, scriptStr, fd, scriptLength);
        }
    }
    return AniParseUtils::ConstructArrayBufFlowbuf(env, script, fd, scriptLength);
}

static void RunJavaScriptInternal(
    ani_env* env, ani_object object, const std::string& script, ani_object callback, bool extention)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    WVLOG_D("enter RunJavaScriptInternal");
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    ani_class functionClass;
    env->FindClass("Lstd/core/Function;", &functionClass);
    ani_boolean isFunction;
    env->Object_InstanceOf(callback, functionClass, &isFunction);
    if (!isFunction) {
        WVLOG_E("callback is not fountion");
        return;
    }
    int32_t nwebId = controller->GetNWebId();
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId);
    if (!nweb_ptr) {
        WVLOG_I("nweb_ptr is null");
        ani_status status;
        std::vector<ani_ref> resultRef(CALLBACK_PARAM_LENGTH);
        resultRef[0] = OHOS::NWeb::CreateStsError(
            env, static_cast<ani_int>(NWebError::INIT_ERROR), GetErrMsgByErrCode(INIT_ERROR));
        env->GetNull(&resultRef[1]);
        ani_ref jsCallback = nullptr;
        env->GlobalReference_Create(callback, &jsCallback);
        if (jsCallback) {
            ani_ref fnReturnVal;
            if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(jsCallback), resultRef.size(),
                     resultRef.data(), &fnReturnVal)) != ANI_OK) {
                WVLOG_E("error callback FunctionalObject_Call Failed status : %{public}d!", status);
            }
        }
        env->GlobalReference_Delete(jsCallback);
        return;
    }
    if (callback) {
        auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, callback, nullptr, extention);
        nweb_ptr->ExecuteJavaScript(script, callbackImpl, extention);
    }
    return;
}

static ani_object RunJavaScriptInternalPromise(
    ani_env* env, ani_object object, const std::string& script, bool extention)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    WVLOG_D("enter RunJavaScriptInternal");
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    ani_object promise;
    ani_resolver resolver;
    if (env->Promise_New(&resolver, &promise) != ANI_OK) {
        WVLOG_E("create promise object error");
        return nullptr;
    }
    int32_t nwebId = controller->GetNWebId();
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId);
    if (!nweb_ptr) {
        ani_ref JsResult = OHOS::NWeb::CreateStsError(
            env, static_cast<ani_int>(NWebError::INIT_ERROR), GetErrMsgByErrCode(INIT_ERROR));
        if (env->PromiseResolver_Reject(resolver, reinterpret_cast<ani_error>(JsResult)) != ANI_OK) {
            WVLOG_E("reject promise error");
        } else {
            WVLOG_E("PromiseResolver_Reject error");
        }
        return nullptr;
    }
    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, nullptr, resolver, extention);
    nweb_ptr->ExecuteJavaScript(script, callbackImpl, extention);
    return promise;
}

static void RunJSBackToOriginal(ani_env* env, ani_object object, ani_object script, ani_object callback, bool extention)
{
    std::string scriptStr;
    bool parseResult = false;
    if (AniParseUtils::IsString(env, script)) {
        parseResult = AniParseUtils::ParseString(env, script, scriptStr);
    }

    if (!parseResult) {
        WVLOG_E("parseResult is false");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    return RunJavaScriptInternal(env, object, scriptStr, callback, extention);
}

static ani_object RunJSBackToOriginal(ani_env* env, ani_object object, ani_object script, bool extention)
{
    std::string scriptStr;
    bool parseResult = false;
    if (AniParseUtils::IsString(env, script)) {
        parseResult = AniParseUtils::ParseString(env, script, scriptStr);
    }

    if (!parseResult) {
        WVLOG_E("parseResult is false");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }
    return RunJavaScriptInternalPromise(env, object, scriptStr, extention);
}

static void RunJavaScriptInternalExt(
    ani_env* env, ani_object object, ani_object script, ani_object callbackObj, bool extention)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    int fd;
    size_t scriptLength;
    ErrCode constructResult = ConstructFlowbuf(env, script, fd, scriptLength);
    if (constructResult != NO_ERROR) {
        return RunJSBackToOriginal(env, object, script, callbackObj, extention);
    }
    usedFd_++;
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        close(fd);
        usedFd_--;
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    int32_t nwebId = controller->GetNWebId();
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId);
    if (!nweb_ptr) {
        std::vector<ani_ref> resultRef(CALLBACK_PARAM_LENGTH);
        resultRef[0] = OHOS::NWeb::CreateStsError(
            env, static_cast<ani_int>(NWebError::INIT_ERROR), GetErrMsgByErrCode(INIT_ERROR));
        env->GetNull(&resultRef[1]);
        ani_ref jsCallback = nullptr;
        env->GlobalReference_Create(callbackObj, &jsCallback);
        if (jsCallback) {
            ani_ref fnReturnVal;
            env->FunctionalObject_Call(
                static_cast<ani_fn_object>(jsCallback), resultRef.size(), resultRef.data(), &fnReturnVal);
        }
        env->GlobalReference_Delete(jsCallback);
        close(fd);
        usedFd_--;
        return;
    }
    if (callbackObj) {
        auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, callbackObj, nullptr, extention);
        nweb_ptr->ExecuteJavaScriptExt(fd, scriptLength, callbackImpl, extention);
    } else {
        close(fd);
    }
    usedFd_--;
    return;
}

static ani_object RunJavaScriptInternalPromiseExt(ani_env* env, ani_object object, ani_object script, bool extention)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    int fd;
    size_t scriptLength;
    ErrCode constructResult = ConstructFlowbuf(env, script, fd, scriptLength);
    if (constructResult != NO_ERROR) {
        return RunJSBackToOriginal(env, object, script, extention);
    }
    usedFd_++;
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        close(fd);
        usedFd_--;
        WVLOG_E("controller is nullptr or not init");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    ani_object promise;
    ani_resolver resolver;
    if (env->Promise_New(&resolver, &promise) != ANI_OK) {
        WVLOG_E("create promise object error");
        close(fd);
        usedFd_--;
        return nullptr;
    }
    int32_t nwebId = controller->GetNWebId();
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId);
    if (!nweb_ptr) {
        ani_ref JsResult = OHOS::NWeb::CreateStsError(
            env, static_cast<ani_int>(NWebError::INIT_ERROR), GetErrMsgByErrCode(INIT_ERROR));
        if (env->PromiseResolver_Reject(resolver, reinterpret_cast<ani_error>(JsResult)) != ANI_OK) {
            WVLOG_E("rejecct promise errpr");
        }
        close(fd);
        usedFd_--;
        return nullptr;
    }
    auto callbackImpl = std::make_shared<WebviewJavaScriptExecuteCallback>(env, nullptr, resolver, extention);
    nweb_ptr->ExecuteJavaScriptExt(fd, scriptLength, callbackImpl, extention);
    close(fd);
    usedFd_--;
    return promise;
}

static void RunJSCallback(ani_env* env, ani_object object, ani_object script, ani_object callbackObj, bool extention)
{
    WVLOG_D("enter RunJSCallback");
    if (maxFdNum_ == -1) {
        maxFdNum_ = std::atoi(NWebAdapterHelper::Instance().ParsePerfConfig("flowBufferConfig", "maxFdNumber").c_str());
    }
    if (usedFd_.load() < maxFdNum_) {
        WVLOG_I("enter RunJavaScriptInternalExt in RunJSCallback");
        return RunJavaScriptInternalExt(env, object, script, callbackObj, extention);
    }

    std::string scriptStr;
    bool parseResult = (AniParseUtils::IsString(env, script)) ? AniParseUtils::ParseString(env, script, scriptStr)
                                                              : AniParseUtils::ParseArrayBuffer(env, script, scriptStr);
    if (!parseResult) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "script", "string"));
        return;
    }

    WVLOG_I("enter RunJavaScriptInternal in RunJSCallback");
    return RunJavaScriptInternal(env, object, scriptStr, callbackObj, extention);
}

static ani_object RunJSPromise(ani_env* env, ani_object object, ani_object script, bool extention)
{
    WVLOG_D("enter RunJSPromise");
    if (maxFdNum_ == -1) {
        maxFdNum_ = std::atoi(NWebAdapterHelper::Instance().ParsePerfConfig("flowBufferConfig", "maxFdNumber").c_str());
    }
    if (usedFd_.load() < maxFdNum_) {
        WVLOG_I("enter RunJavaScriptInternalPromiseExt in RunJSCallback");
        return RunJavaScriptInternalPromiseExt(env, object, script, extention);
    }

    std::string scriptStr;
    bool parseResult = (AniParseUtils::IsString(env, script)) ? AniParseUtils::ParseString(env, script, scriptStr)
                                                              : AniParseUtils::ParseArrayBuffer(env, script, scriptStr);
    if (!parseResult) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "script", "string"));
        return nullptr;
    }

    WVLOG_I("enter RunJavaScriptInternalPromise in RunJSPromise");
    return RunJavaScriptInternalPromise(env, object, scriptStr, extention);
}

static void RunJavaScriptCallback(ani_env* env, ani_object object, ani_object script, ani_object callbackObj)
{
    WVLOG_D("start RunJavaScriptCallback");
    return RunJSCallback(env, object, script, callbackObj, false);
}

static ani_object RunJavaScriptPromise(ani_env* env, ani_object object, ani_object script)
{
    WVLOG_D("start RunJavaScriptPromise");
    return RunJSPromise(env, object, script, false);
}

static void RunJavaScriptCallbackExt(ani_env* env, ani_object object, ani_object script, ani_object callbackObj)
{
    WVLOG_D("start RunJavaScriptCallback");
    return RunJSCallback(env, object, script, callbackObj, true);
}

static ani_object RunJavaScriptPromiseExt(ani_env* env, ani_object object, ani_object script)
{
    WVLOG_D("start RunJavaScriptPromiseExt");
    return RunJSPromise(env, object, script, true);
}

static void EnableBackForwardCache(ani_env *env, ani_object object, ani_object featuresObject)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    bool nativeEmbed = false;
    bool mediaTakeOver = false;
    ani_boolean embedObj = ANI_FALSE;
    ani_boolean mediaObj = ANI_FALSE;
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(featuresObject, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        if (env->Object_GetFieldByName_Boolean(featuresObject, "nativeEmbed", &embedObj) == ANI_OK) {
            nativeEmbed = static_cast<bool>(embedObj);
        }

        if (env->Object_GetFieldByName_Boolean(featuresObject, "mediaTakeOver", &mediaObj) == ANI_OK) {
            mediaTakeOver = static_cast<bool>(mediaObj);
        }
    }
    NWebHelper::Instance().EnableBackForwardCache(nativeEmbed, mediaTakeOver);
    return;
}

static void SetBackForwardCacheOptions(ani_env *env, ani_object object, ani_object optionsObject)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto* controller = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("SetBackForwardCacheOptions: Init webview controller error.");
        return;
    }
    int32_t size = BFCACHE_DEFAULT_SIZE;
    int32_t timeToLive = BFCACHE_DEFAULT_TIMETOLIVE;
    ani_int sizeObj = static_cast<ani_int>(size);
    ani_int timeToLiveObj = static_cast<ani_int>(timeToLive);
    ani_boolean isUndefined = ANI_TRUE;
    env->Reference_IsUndefined(optionsObject, &isUndefined);
    if (isUndefined != ANI_TRUE) {
        if (env->Object_GetPropertyByName_Int(optionsObject, "size", &sizeObj) == ANI_OK) {
            size = static_cast<int32_t>(sizeObj);
        }
        if (env->Object_GetPropertyByName_Int(optionsObject, "timeToLive", &timeToLiveObj) == ANI_OK) {
            timeToLive = static_cast<int32_t>(timeToLiveObj);
        }
    }
    WVLOG_D("SetBackForwardCacheOptions size:%{public}d, timeToLive:%{public}d", size, timeToLive);
    controller->SetBackForwardCacheOptions(size, timeToLive);
    return;
}

static void SetAppCustomUserAgent(ani_env* env, ani_object object, ani_object aniUA)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    std::string userAgent;
    if (!AniParseUtils::ParseString(env, aniUA, userAgent)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "userAgent", "string"));
        return;
    }
    NWebHelper::Instance().SetAppCustomUserAgent(userAgent);
}

static void SetUserAgentForHosts(ani_env* env, ani_object object, ani_object aniUA, ani_object aniHosts)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }

    std::string userAgent;
    if (!AniParseUtils::ParseString(env, aniUA, userAgent)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "userAgent", "string"));
        return;
    }
    std::vector<std::string> hosts;
    if (!AniParseUtils::ParseStringArray(env, aniHosts, hosts)) {
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "hosts", "array"));
        return;
    }
    NWebHelper::Instance().SetUserAgentForHosts(userAgent, hosts);
}

static void StoreWebArchiveCallbackInternal(
    ani_env* env, std::string baseNameStr, bool autoNameStr, ani_ref jsCallback, int32_t nwebId)
{
    if (!env) {
        return;
    }
    if (!jsCallback) {
        return;
    }
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId);
    if (!nweb_ptr) {
        std::vector<ani_ref> resultRef;
        ani_ref tmp = NWebError::AniBusinessError::CreateError(env, INVALID_RESOURCE);
        resultRef.push_back(tmp);
        env->GetNull(&tmp);
        resultRef.push_back(tmp);
        ani_ref fnReturnVal;
        env->FunctionalObject_Call(
            static_cast<ani_fn_object>(jsCallback), resultRef.size(), resultRef.data(), &fnReturnVal);
        env->GlobalReference_Delete(jsCallback);
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebStoreWebArchiveCallback>();
    callbackImpl->SetCallBack([env, jCallback = std::move(jsCallback)](std::string result) {
        if (!env) {
            return;
        }
        std::vector<ani_ref> resultRef;
        if (!result.empty()) {
            ani_ref tmp;
            env->GetNull(&tmp);
            ani_string str {};
            env->String_NewUTF8(result.c_str(), result.size(), &str);
            resultRef.push_back(tmp);
            resultRef.push_back(str);
        } else {
            ani_ref tmp = NWebError::AniBusinessError::CreateError(env, INVALID_RESOURCE);
            resultRef.push_back(tmp);
            env->GetNull(&tmp);
            resultRef.push_back(tmp);
        }
        ani_ref fnReturnVal;
        env->FunctionalObject_Call(
            static_cast<ani_fn_object>(jCallback), resultRef.size(), resultRef.data(), &fnReturnVal);
        env->GlobalReference_Delete(jCallback);
    });
    nweb_ptr->StoreWebArchive(baseNameStr, autoNameStr, callbackImpl);
}

static void StoreWebArchiveCallback(
    ani_env* env, ani_object object, ani_string baseName, ani_boolean autoName, ani_fn_object callbackObj)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(baseName, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("baseName is undefined");
        return;
    }
    std::string baseNameStr;
    if (!AniParseUtils::ParseString(env, baseName, baseNameStr)) {
        WVLOG_E("Parse baseName failed");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "baseName", "string"));
        return;
    }
    bool autoNameStr = static_cast<bool>(autoName);

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("controller is null");
        return;
    }
    int32_t nwebId_ = controller->GetWebId();

    ani_ref jsCallback = nullptr;
    env->GlobalReference_Create(callbackObj, &jsCallback);
    return StoreWebArchiveCallbackInternal(env, baseNameStr, autoNameStr, jsCallback, nwebId_);
}

static void StoreWebArchivePromiseInternal(
    ani_env* env, std::string baseNameStr, bool autoNameStr, ani_resolver deferred, int32_t nwebId)
{
    if (!env) {
        return;
    }
    if (!deferred) {
        return;
    }
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId);
    if (!nweb_ptr) {
        WVLOG_E("nweb_ptr is nullptr");
        ani_ref jsResult = nullptr;
        jsResult = NWebError::AniBusinessError::CreateError(env, INIT_ERROR);
        env->PromiseResolver_Reject(deferred, reinterpret_cast<ani_error>(jsResult));
        return;
    }

    auto callbackImpl = std::make_shared<OHOS::NWeb::NWebStoreWebArchiveCallback>();
    callbackImpl->SetCallBack([env, deferred](std::string result) {
        if (!env) {
            return;
        }
        std::vector<ani_ref> resultRef(RESULT_COUNT);
        resultRef[0] = NWebError::AniBusinessError::CreateError(env, INVALID_RESOURCE);
        ani_string str {};
        env->String_NewUTF8(result.c_str(), result.size(), &str);
        resultRef[1] = static_cast<ani_ref>(str);
        if (!result.empty()) {
            env->PromiseResolver_Resolve(deferred, resultRef[1]);
        } else {
            env->PromiseResolver_Reject(deferred, reinterpret_cast<ani_error>(resultRef[0]));
        }
    });
    nweb_ptr->StoreWebArchive(baseNameStr, autoNameStr, callbackImpl);
}

static ani_object StoreWebArchivePromise(ani_env* env, ani_object object, ani_string baseName, ani_boolean autoName)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_boolean isUndefined = ANI_TRUE;
    if (env->Reference_IsUndefined(baseName, &isUndefined) != ANI_OK || isUndefined == ANI_TRUE) {
        WVLOG_E("baseName is undefined");
        return nullptr;
    }
    std::string baseNameStr;
    if (!AniParseUtils::ParseString(env, baseName, baseNameStr)) {
        WVLOG_E("Parse baseName failed");
        AniBusinessError::ThrowError(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "baseName", "string"));
        return nullptr;
    }
    bool autoNameStr = static_cast<bool>(autoName);
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller) {
        WVLOG_E("controller is null");
        return nullptr;
    }
    int32_t nwebId_ = controller->GetWebId();
    ani_object promise;
    ani_resolver deferred;
    env->Promise_New(&deferred, &promise);
    StoreWebArchivePromiseInternal(env, baseNameStr, autoNameStr, deferred, nwebId_);
    return promise;
}

static void HasImageCallback(ani_env* env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("HasImageCallback begin");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    if (!vm) {
        WVLOG_E("vm is nullptr");
        return;
    }

    WebviewController* webviewController = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!webviewController || !webviewController->IsInit()) {
        WVLOG_E("Unwrap failed");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    ani_ref jsCallback = nullptr;
    auto status = env->GlobalReference_Create(callback, &jsCallback);
    if (status != ANI_OK) {
        WVLOG_E("GlobalReference_Create failed, status is : %{public}d", status);
        return;
    }
    if (jsCallback) {
        ErrCode ret = webviewController->HasImagesCallback(vm, jsCallback);
        if (ret == NWEB_ERROR) {
            AniBusinessError::ThrowErrorByErrCode(env, ret);
            return;
        } else if (ret != NO_ERROR) {
            AniBusinessError::ThrowErrorByErrCode(env, ret);
            return;
        }
    }
}

ani_object HasImagePromise(ani_env* env, ani_object object)
{
    WVLOG_D("HasImagePromise begin");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_vm* vm = nullptr;
    env->GetVM(&vm);
    if (!vm) {
        WVLOG_E("vm is nullptr");
        return nullptr;
    }

    WebviewController* webviewController = reinterpret_cast<WebviewController *>(AniParseUtils::Unwrap(env, object));
    if (!webviewController || !webviewController->IsInit()) {
        WVLOG_E("Unwrap failed");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    ani_resolver deferred = nullptr;
    ani_object promise = nullptr;
    auto status = env->Promise_New(&deferred, &promise);
    if (status != ANI_OK) {
        WVLOG_E("Promise_New failed, status is : %{public}d", status);
        return nullptr;
    }

    if (promise && deferred) {
        ErrCode ret = webviewController->HasImagesPromise(vm, deferred);
        if (ret == NWEB_ERROR) {
            AniBusinessError::ThrowErrorByErrCode(env, ret);
            return nullptr;
        } else if (ret != NO_ERROR) {
            AniBusinessError::ThrowErrorByErrCode(env, ret);
            return nullptr;
        }
    }
    return promise;
}

static ani_object GetCertificateSync(ani_env* env, ani_object object)
{
    ani_object certificateObj = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return certificateObj;
    }
    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return certificateObj;
    }
    std::vector<std::string> certChainDerData;
    if (!controller->GetCertChainDerData(certChainDerData)) {
        WVLOG_E("GetCertificateSync failed");
        return certificateObj;
    }
    ani_class cls;
    if (env->FindClass("Lescompat/Array;", &cls) != ANI_OK) {
        return certificateObj;
    }
    ani_method arrayCtor;
    if (env->Class_FindMethod(cls, "<ctor>", "I:V", &arrayCtor) != ANI_OK) {
        return certificateObj;
    }
    if (env->Object_New(cls, arrayCtor, &certificateObj, certChainDerData.size()) != ANI_OK) {
        return certificateObj;
    }
    ani_status status;
    for (uint8_t i = 0; i < certChainDerData.size(); ++i) {
        if (i == UINT8_MAX) {
            WVLOG_E("error, cert chain data array reach max");
            break;
        }
        void* data = nullptr;
        ani_arraybuffer buffer = nullptr;
        env->CreateArrayBuffer(certChainDerData[i].size(), &data, &buffer);
        int retCode =
            memcpy_s(data, certChainDerData[i].size(), certChainDerData[i].data(), certChainDerData[i].size());
        if (retCode != 0) {
            return certificateObj;
        }
        if ((status = env->Array_Set(
                 static_cast<ani_array>(certificateObj), i, static_cast<ani_ref>(buffer))) != ANI_OK) {
            WVLOG_E("error in set element");
            return certificateObj;
        }
    }
    return certificateObj;
}

static ani_ref CreateWebPrintDocumentAdapter(ani_env* env, ani_object object, ani_string jobName)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    std::string jobNameString;
    if (!AniParseUtils::ParseString(env, jobName, jobNameString)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "jopName", "string"));
        return nullptr;
    }

    auto* controller = reinterpret_cast<WebviewController*>(AniParseUtils::Unwrap(env, object));
    if (!controller || !controller->IsInit()) {
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }

    void* webPrintDocument = controller->CreateWebPrintDocumentAdapter(jobNameString);
    if (!webPrintDocument) {
        WVLOG_E("failed to Unwrap webPrintDocument");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return nullptr;
    }

    WebPrintDocument* webPrintDoc = new (std::nothrow) WebPrintDocument(webPrintDocument);
    if (!webPrintDoc) {
        WVLOG_E("new web print failed");
        return nullptr;
    }

    ani_object printDocumentAdapterObj = {};
    if (AniParseUtils::CreateObjectVoid(
            env, "L@ohos/web/webview/webview/PrintDocumentAdapterInner;", printDocumentAdapterObj) == false) {
        WVLOG_E("[printDocumentAdapter] CreateObjectVoid failed");
        delete webPrintDoc;
        webPrintDoc = nullptr;
        return nullptr;
    }

    if (!AniParseUtils::Wrap(env, printDocumentAdapterObj, "L@ohos/web/webview/webview/PrintDocumentAdapterInner;",
            reinterpret_cast<ani_long>(webPrintDoc))) {
        WVLOG_E("[printDocumentAdapter] WebDownloadDelegate wrap failed");
        delete webPrintDoc;
        webPrintDoc = nullptr;
        return nullptr;
    }
    return printDocumentAdapterObj;
}

static void OnJobStateChanged(ani_env* env, ani_object object, ani_string jobId, ani_enum_item state)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    WebPrintDocument* webPrintDocument = reinterpret_cast<WebPrintDocument*>(AniParseUtils::Unwrap(env, object));
    if (!webPrintDocument) {
        WVLOG_E("failed to Unwrap webPrintDocument");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }
    std::string jobIDString;
    if (!AniParseUtils::ParseString(env, jobId, jobIDString)) {
        WVLOG_E("failed to parse jobId");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    int32_t stateInt = 0;
    if (!AniParseUtils::EnumParseInt32_t(env, state, stateInt)) {
        WVLOG_E("failed to parse state");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    webPrintDocument->OnJobStateChanged(jobIDString, stateInt);
    return;
}

WebPrintWriteResultCallback ParseWebPrintWriteResultCallback(ani_env* env, ani_object object, ani_fn_object callback)
{
    WVLOG_D("ParseWebPrintWriteResultCallback");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    ani_ref callbackRef;

    if (env->GlobalReference_Create(static_cast<ani_ref>(callback), &callbackRef) != ANI_OK) {
        WVLOG_E("failed to create reference for callback");
        return nullptr;
    }

    auto pCallback = [env, cbRef = std::move(callbackRef)](std::string jobId, uint32_t state) {
        std::vector<ani_ref> argv;
        ani_string jobIdString = nullptr;
        if (env->String_NewUTF8(jobId.c_str(), jobId.size(), &jobIdString) == ANI_OK) {
            argv.push_back(static_cast<ani_ref>(jobIdString));
        }
        ani_enum_item stateEnum;
        if (AniParseUtils::GetEnumItemByIndex(
                env, "L@ohos/print/print/PrintFileCreationState;", static_cast<int32_t>(state), stateEnum)) {
            argv.push_back(static_cast<ani_ref>(stateEnum));
        }
        ani_ref fnReturnVal = nullptr;
        env->FunctionalObject_Call(reinterpret_cast<ani_fn_object>(cbRef), argv.size(), argv.data(), &fnReturnVal);
        env->GlobalReference_Delete(cbRef);
    };
    return pCallback;
}

static void ParsePrintRangeAdapter(ani_env* env, ani_object pageRange, PrintAttributesAdapter& printAttr)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (!pageRange) {
        WVLOG_E("ParsePrintRangeAdapter failed.");
        return;
    }
    ani_int startPage = 0;
    ani_int endPage = 0;
    ani_ref pages = nullptr;
    if (env->Object_GetPropertyByName_Int(pageRange, "startPage", &startPage) != ANI_OK) {
        WVLOG_E("ParsePrintRangeAdapter failed to get startPage");
    }

    if (env->Object_GetPropertyByName_Int(pageRange, "endPage", &endPage) != ANI_OK) {
        WVLOG_E("ParsePrintRangeAdapter failed to get endPage");
    }

    if (env->Object_GetPropertyByName_Ref(pageRange, "pages", &pages) != ANI_OK) {
        WVLOG_E("ParsePrintRangeAdapter failed to get pages");
    }
    ani_array_int pagesArrayInt = static_cast<ani_array_int>(pages);

    printAttr.pageRange.startPage = static_cast<int>(startPage);

    printAttr.pageRange.endPage = static_cast<int>(endPage);
    WVLOG_D("printAttr.pageRange.startPage is %{public}d,printAttr.pageRange.endPage is %{public}d",
        printAttr.pageRange.startPage, printAttr.pageRange.endPage);

    ani_size length = 0;
    env->Array_GetLength(pagesArrayInt, &length);
    for (uint32_t i = 0; i < length; ++i) {
        ani_int pagesInt;
        env->Array_GetRegion_Int(pagesArrayInt, i, 1, &pagesInt);
        int pagesNum = static_cast<int>(pagesInt);
        printAttr.pageRange.pages.push_back(pagesNum);
    }
}

static void ParsePrintPageSizeAdapter(ani_env* env, ani_object pageSize, PrintAttributesAdapter& printAttr)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (!pageSize) {
        WVLOG_E("ParsePrintPageSizeAdapter failed.");
        return;
    }

    ani_int width = 0;
    ani_int height = 0;
    if (env->Object_GetPropertyByName_Int(pageSize, "width", &width) != ANI_OK) {
        WVLOG_E("ParsePrintPageSizeAdapter failed to get width");
    }
    if (env->Object_GetPropertyByName_Int(pageSize, "height", &height) != ANI_OK) {
        WVLOG_E("ParsePrintPageSizeAdapter failed to get height");
    }

    printAttr.pageSize.width = static_cast<int>(width);
    printAttr.pageSize.height = static_cast<int>(height);
    WVLOG_D("width is %{public}d,height is %{public}d", printAttr.pageSize.width, printAttr.pageSize.height);
}

static void ParsePrintMarginAdapter(ani_env* env, ani_object margin, PrintAttributesAdapter& printAttr)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (!margin) {
        WVLOG_E("ParsePrintMarginAdapter failed.");
        return;
    }

    ani_int top = 0;
    ani_int bottom = 0;
    ani_int left = 0;
    ani_int right = 0;
    if (env->Object_GetPropertyByName_Int(margin, "top", &top) != ANI_OK) {
        WVLOG_D("ParsePrintMarginAdapter failed to get top");
    }
    if (env->Object_GetPropertyByName_Int(margin, "bottom", &bottom) != ANI_OK) {
        WVLOG_D("ParsePrintMarginAdapter failed to get bottom");
    }
    if (env->Object_GetPropertyByName_Int(margin, "left", &left) != ANI_OK) {
        WVLOG_D("ParsePrintMarginAdapter failed to get left");
    }
    if (env->Object_GetPropertyByName_Int(margin, "right", &right) != ANI_OK) {
        WVLOG_D("ParsePrintMarginAdapter failed to get right");
    }

    printAttr.margin.top = static_cast<int>(top);
    printAttr.margin.bottom = static_cast<int>(bottom);
    printAttr.margin.left = static_cast<int>(left);
    printAttr.margin.right = static_cast<int>(right);
    WVLOG_D("top is %{public}d,bottom is %{public}d,left is %{public}d,right is %{public}d", printAttr.margin.top,
        printAttr.margin.bottom, printAttr.margin.left, printAttr.margin.right);
}

static bool ParseWebPrintAttrParams(ani_env* env, ani_object obj, PrintAttributesAdapter& printAttr)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }

    if (!obj) {
        WVLOG_E("ParseWebPrintAttrParams failed.");
        return false;
    }
    ani_ref copyNumber_ref = nullptr;
    ani_ref pageRange_ref = nullptr;
    ani_ref isSequential_ref = nullptr;
    ani_ref pageSize_ref = nullptr;
    ani_ref isLandscape_ref = nullptr;
    ani_ref colorMode_ref = nullptr;
    ani_ref duplexMode_ref = nullptr;
    ani_ref margin = nullptr;

    int32_t copyNumber = 0;
    if (AniParseUtils::GetRefProperty(env, obj, "copyNumber", copyNumber_ref)) {
        AniParseUtils::ParseInt32(env, copyNumber_ref, copyNumber);
    }

    if (AniParseUtils::GetRefProperty(env, obj, "pageRange", pageRange_ref)) {
        ParsePrintRangeAdapter(env, static_cast<ani_object>(pageRange_ref), printAttr);
    }

    bool isSequential = false;
    if (AniParseUtils::GetRefProperty(env, obj, "isSequential", isSequential_ref)) {
        AniParseUtils::ParseBoolean(env, isSequential_ref, isSequential);
    }

    if (AniParseUtils::GetRefProperty(env, obj, "pageSize", pageSize_ref)) {
        ParsePrintPageSizeAdapter(env, static_cast<ani_object>(pageSize_ref), printAttr);
    }

    bool isLandscape = false;
    if (AniParseUtils::GetRefProperty(env, obj, "isLandscape", isLandscape_ref)) {
        AniParseUtils::ParseBoolean(env, isLandscape_ref, isLandscape);
    }

    int32_t colorMode = 0;
    if (AniParseUtils::GetRefProperty(env, obj, "colorMode", colorMode_ref)) {
        AniParseUtils::ParseInt32(env, colorMode_ref, colorMode);
    }

    int32_t duplexMode = 0;
    if (AniParseUtils::GetRefProperty(env, obj, "duplexMode", duplexMode_ref)) {
        AniParseUtils::ParseInt32(env, duplexMode_ref, duplexMode);
    }

    if (AniParseUtils::GetRefProperty(env, obj, "margin", margin)) {
        ParsePrintMarginAdapter(env, static_cast<ani_object>(margin), printAttr);
    }

    printAttr.copyNumber = static_cast<uint32_t>(copyNumber);
    printAttr.isSequential = static_cast<bool>(isSequential);
    printAttr.isLandscape = static_cast<bool>(isLandscape);
    printAttr.colorMode = static_cast<uint32_t>(colorMode);
    printAttr.duplexMode = static_cast<uint32_t>(duplexMode);
    WVLOG_D("copyNumber is %{public}d,isSequential is %{public}d,isLandscape is %{public}d,colorMode is "
            "%{public}d,duplexMode is %{public}d",
        printAttr.copyNumber, printAttr.isSequential, printAttr.isLandscape, printAttr.colorMode, printAttr.duplexMode);

    return true;
}

static void OnStartLayoutWrite(ani_env* env, ani_object object, ani_string jobId, ani_object oldPrintAttr,
    ani_object newPrintAttr, ani_int fd, ani_fn_object callback)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    WebPrintDocument* webPrintDocument = reinterpret_cast<WebPrintDocument*>(AniParseUtils::Unwrap(env, object));
    if (!webPrintDocument) {
        WVLOG_E("failed to Unwrap webPrintDocument");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return;
    }

    std::string jobIDString;
    if (!AniParseUtils::ParseString(env, jobId, jobIDString)) {
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }

    if (fd < 0) {
        WVLOG_E("invalid file descriptor: %d", fd);
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }

    uint32_t fdUint = static_cast<uint32_t>(fd);
    PrintAttributesAdapter oldPA;
    if (!ParseWebPrintAttrParams(env, oldPrintAttr, oldPA)) {
        WVLOG_E("failed to ParseWebPrintAttrParams oldPrintAttr");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    PrintAttributesAdapter newPA;
    if (!ParseWebPrintAttrParams(env, newPrintAttr, newPA)) {
        WVLOG_E("failed to ParseWebPrintAttrParams newPrintAttr");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    WebPrintWriteResultCallback writeResultCallback = nullptr;
    writeResultCallback = ParseWebPrintWriteResultCallback(env, object, callback);
    if (!writeResultCallback) {
        WVLOG_E("failed to ParseWebPrintWriteResultCallback");
        AniBusinessError::ThrowErrorByErrCode(env, PARAM_CHECK_ERROR);
        return;
    }
    webPrintDocument->OnStartLayoutWrite(jobIDString, oldPA, newPA, fdUint, writeResultCallback);
    return;
}

ani_status StsPrintDocumentAdapterInit(ani_env* env)
{
    ani_class printDocumentAdapterCls = nullptr;
    ani_status status = env->FindClass(ANI_PRINT_DOCUMENT_ADAPTER_INNER_CLASS_NAME, &printDocumentAdapterCls);
    if (status != ANI_OK || !printDocumentAdapterCls) {
        WVLOG_E(
            "find %{public}s class failed, status: %{public}d", ANI_PRINT_DOCUMENT_ADAPTER_INNER_CLASS_NAME, status);
        return ANI_ERROR;
    }

    std::array methodArray = {
        ani_native_function { "onStartLayoutWrite", nullptr, reinterpret_cast<void*>(OnStartLayoutWrite) },
        ani_native_function { "onJobStateChanged", nullptr, reinterpret_cast<void*>(OnJobStateChanged) },
    };

    status = env->Class_BindNativeMethods(printDocumentAdapterCls, methodArray.data(), methodArray.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }

    return ANI_OK;
}


ani_status StsWebviewControllerInit(ani_env *env)
{
    WVLOG_D("[DOWNLOAD] StsWebviewControllerInit");
    if (!env) {
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
        ani_native_function { "loadUrl", nullptr, reinterpret_cast<void *>(LoadUrl) },
        ani_native_function { "onActive", nullptr, reinterpret_cast<void *>(OnActive) },
        ani_native_function { "onInactive", nullptr, reinterpret_cast<void *>(OnInactive) },
        ani_native_function { "getWebId", nullptr, reinterpret_cast<void *>(GetWebId) },
        ani_native_function { "innerGetWebId", nullptr, reinterpret_cast<void *>(InnerGetWebId) },
        ani_native_function { "getScrollable", nullptr, reinterpret_cast<void *>(GetScrollable) },
        ani_native_function { "requestFocus", nullptr, reinterpret_cast<void *>(RequestFocus) },
        ani_native_function { "clearClientAuthenticationCache", nullptr,
            reinterpret_cast<void *>(clearClientAuthenticationCache) },
        ani_native_function { "createPdfNative", nullptr, reinterpret_cast<void *>(CreatePdfNative) },
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
        ani_native_function { "isSafeBrowsingEnabled", nullptr, reinterpret_cast<void *>(IsSafeBrowsingEnabled) },
        ani_native_function { "getFavicon", nullptr, reinterpret_cast<void *>(GetFavicon) },
        ani_native_function { "setUrlTrustList", nullptr, reinterpret_cast<void *>(SetUrlTrustList) },
        ani_native_function { "enableIntelligentTrackingPrevention", nullptr,
                              reinterpret_cast<void *>(EnableIntelligentTrackingPrevention) },
        ani_native_function { "searchNext", nullptr, reinterpret_cast<void *>(SearchNext) },
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
        ani_native_function { "isAdsBlockEnabled", nullptr, reinterpret_cast<void *>(IsAdsBlockEnabled) },
        ani_native_function { "enableAdsBlock", nullptr, reinterpret_cast<void *>(EnableAdsBlock) },
        ani_native_function { "postUrl", nullptr, reinterpret_cast<void *>(PostUrl) },
        ani_native_function { "postMessage", nullptr, reinterpret_cast<void *>(PostMessage) },
        ani_native_function { "getUrl", nullptr, reinterpret_cast<void *>(GetUrlAni) },
        ani_native_function { "getTitle", nullptr, reinterpret_cast<void *>(GetTitle) },
        ani_native_function { "getProgress", nullptr, reinterpret_cast<void *>(GetProgress) },
        ani_native_function { "getOriginalUrl", nullptr, reinterpret_cast<void *>(GetOriginalUrl) },
        ani_native_function { "getUserAgent", nullptr, reinterpret_cast<void *>(GetUserAgent) },
        ani_native_function { "getCustomUserAgent", nullptr, reinterpret_cast<void *>(GetCustomUserAgent) },
        ani_native_function { "getLastJavascriptProxyCallingFrameUrl", nullptr,
                              reinterpret_cast<void *>(GetLastJavascriptProxyCallingFrameUrl) },
        ani_native_function { "getSecurityLevel", nullptr, reinterpret_cast<void *>(GetSecurityLevel) },
        ani_native_function { "prefetchPage", nullptr, reinterpret_cast<void *>(PrefetchPage) },
        ani_native_function { "setHttpDns", nullptr, reinterpret_cast<void *>(SetHttpDns) },
        ani_native_function { "searchAllAsync", nullptr, reinterpret_cast<void *>(SearchAllAsync) },
        ani_native_function { "clearServiceWorkerWebSchemeHandler", nullptr,
                              reinterpret_cast<void *>(ClearServiceWorkerWebSchemeHandler) },
        ani_native_function { "forward", nullptr, reinterpret_cast<void *>(Forward) },
        ani_native_function { "createWebMessagePorts", nullptr, reinterpret_cast<void *>(CreateWebMessagePorts) },
        ani_native_function { "backward", nullptr, reinterpret_cast<void *>(Backward) },
        ani_native_function {
            "SetWebDebuggingAccess", nullptr, reinterpret_cast<void*>(SetWebDebuggingAccess) },
        ani_native_function {
            "setWebDebuggingAccessAndPort", nullptr, reinterpret_cast<void*>(SetWebDebuggingAccessAndPort) },
        ani_native_function { "accessForward", nullptr, reinterpret_cast<void *>(AccessForward) },
        ani_native_function { "accessBackward", nullptr, reinterpret_cast<void *>(AccessBackward) },
        ani_native_function { "loadData", nullptr, reinterpret_cast<void *>(LoadData) },
        ani_native_function { "clearHistory", nullptr, reinterpret_cast<void *>(ClearHistory) },
        ani_native_function { "clearWebSchemeHandler", nullptr, reinterpret_cast<void *>(ClearWebSchemeHandler) },
        ani_native_function { "terminateRenderProcess", nullptr, reinterpret_cast<void *>(TerminateRenderProcess) },
        ani_native_function { "backOrForward", nullptr, reinterpret_cast<void *>(BackOrForward) },
        ani_native_function { "enableSafeBrowsing", nullptr, reinterpret_cast<void *>(EnableSafeBrowsing) },
        ani_native_function { "setCustomUserAgent", nullptr, reinterpret_cast<void *>(SetCustomUserAgent) },
        ani_native_function { "removeCache", nullptr, reinterpret_cast<void *>(RemoveCache) },
        ani_native_function { "setNetworkAvailable", nullptr, reinterpret_cast<void *>(SetNetworkAvailable) },
        ani_native_function { "isIncognitoMode", nullptr, reinterpret_cast<void *>(IsIncognitoMode) },
        ani_native_function { "serializeWebStateInternal", nullptr, reinterpret_cast<void *>(SerializeWebState) },
        ani_native_function { "setPathAllowingUniversalAccess", nullptr,
                              reinterpret_cast<void *>(SetPathAllowingUniversalAccess) },
        ani_native_function { "onCreateNativeMediaPlayer", "C{std.core.Function2}:",
                              reinterpret_cast<void *>(OnCreateNativeMediaPlayer) },
        ani_native_function { "injectOfflineResourcesInternal", nullptr,
                              reinterpret_cast<void *>(InjectOfflineResources) },
        ani_native_function { "precompileJavaScriptInternal", nullptr, reinterpret_cast<void *>(PrecompileJavaScript) },
        ani_native_function { "getSurfaceId", nullptr, reinterpret_cast<void *>(GetSurfaceId) },
        ani_native_function { "setPrintBackground", nullptr, reinterpret_cast<void*>(SetPrintBackground) },
        ani_native_function {
            "createWebPrintDocumentAdapter", nullptr, reinterpret_cast<void*>(CreateWebPrintDocumentAdapter) },
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
        ani_native_function { "innerCompleteWindowNew", nullptr, reinterpret_cast<void *>(InnerCompleteWindowNew) },
        ani_native_function { "setWebSchemeHandler", nullptr, reinterpret_cast<void *>(SetWebSchemeHandler) },
        ani_native_function { "customizeSchemes", nullptr, reinterpret_cast<void *>(CustomizeSchemes) },
        ani_native_function { "setServiceWorkerWebSchemeHandler", nullptr,
                              reinterpret_cast<void *>(SetServiceWorkerWebSchemeHandler) },
        ani_native_function { "registerJavaScriptProxy", nullptr, reinterpret_cast<void *>(RegisterJavaScriptProxy) },
        ani_native_function { "deleteJavaScriptRegister", nullptr, reinterpret_cast<void *>(DeleteJavaScriptRegister) },
        ani_native_function { "runJavaScriptCallback", nullptr, reinterpret_cast<void *>(RunJavaScriptCallback) },
        ani_native_function { "runJavaScriptPromise", nullptr, reinterpret_cast<void *>(RunJavaScriptPromise) },
        ani_native_function { "runJavaScriptCallbackExt", nullptr, reinterpret_cast<void *>(RunJavaScriptCallbackExt) },
        ani_native_function { "runJavaScriptPromiseExt", nullptr, reinterpret_cast<void *>(RunJavaScriptPromiseExt) },
        ani_native_function { "enableBackForwardCache", nullptr, reinterpret_cast<void *>(EnableBackForwardCache) },
        ani_native_function { "setBackForwardCacheOptions", nullptr,
                              reinterpret_cast<void *>(SetBackForwardCacheOptions) },
        ani_native_function { "setAppCustomUserAgent", nullptr, reinterpret_cast<void *>(SetAppCustomUserAgent) },
        ani_native_function { "setUserAgentForHosts", nullptr, reinterpret_cast<void *>(SetUserAgentForHosts) },
        ani_native_function { "storeWebArchiveCallback", nullptr, reinterpret_cast<void *>(StoreWebArchiveCallback) },
        ani_native_function { "storeWebArchivePromise", nullptr, reinterpret_cast<void *>(StoreWebArchivePromise) },
        ani_native_function { "hasImageCallback", nullptr, reinterpret_cast<void *>(HasImageCallback) },
        ani_native_function { "hasImagePromise", nullptr, reinterpret_cast<void *>(HasImagePromise) },
        ani_native_function { "restoreWebState", nullptr, reinterpret_cast<void*>(RestoreWebState) },
        ani_native_function { "getCertificateSync", nullptr, reinterpret_cast<void*>(GetCertificateSync) },
    };
    status = env->Class_BindNativeMethods(webviewControllerCls, controllerMethods.data(), controllerMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
    }

    std::array controllerStaticMethods = {
        ani_native_function { "initializeWebEngine", nullptr, reinterpret_cast<void *>(InitializeWebEngine) },
        ani_native_function { "addIntelligentTrackingPreventionBypassingList", nullptr,
            reinterpret_cast<void *>(AddIntelligentTrackingPreventionBypassingList) },
        ani_native_function { "removeIntelligentTrackingPreventionBypassingList", nullptr,
            reinterpret_cast<void *>(RemoveIntelligentTrackingPreventionBypassingList) },
        ani_native_function { "clearIntelligentTrackingPreventionBypassingList", nullptr,
            reinterpret_cast<void *>(ClearIntelligentTrackingPreventionBypassingList) },
        ani_native_function { "setHostIP", nullptr, reinterpret_cast<void *>(SetHostIP) },
        ani_native_function { "clearHostIP", nullptr,reinterpret_cast<void *>(ClearHostIP) },
        ani_native_function { "warmupServiceWorker", nullptr, reinterpret_cast<void *>(WarmupServiceWorker) },
        ani_native_function { "prepareForPageLoad", nullptr, reinterpret_cast<void *>(PrepareForPageLoad) },
        ani_native_function { "getDefaultUserAgent", nullptr, reinterpret_cast<void *>(GetDefaultUserAgent) },
        ani_native_function { "removeAllCache", nullptr, reinterpret_cast<void *>(RemoveAllCache) },
        ani_native_function { "setRenderProcessMode", nullptr, reinterpret_cast<void *>(SetRenderProcessMode) },
        ani_native_function { "getRenderProcessMode", nullptr, reinterpret_cast<void *>(GetRenderProcessMode) },
        ani_native_function { "setConnectionTimeout", nullptr, reinterpret_cast<void *>(SetConnectionTimeout) },
        ani_native_function { "pauseAllTimers", nullptr, reinterpret_cast<void *>(PauseAllTimers) },
        ani_native_function { "resumeAllTimers", nullptr, reinterpret_cast<void *>(ResumeAllTimers) },
        ani_native_function { "trimMemoryByPressureLevel", nullptr,
                              reinterpret_cast<void *>(TrimMemoryByPressureLevel) },
        ani_native_function { "clearPrefetchedResource", nullptr, reinterpret_cast<void *>(ClearPrefetchedResource) },
        ani_native_function { "prefetchResource", nullptr, reinterpret_cast<void *>(PrefetchResource) },
        ani_native_function { "enableWholeWebPageDrawing", nullptr,
                              reinterpret_cast<void *>(EnableWholeWebPageDrawing) },
    };
    status = env->Class_BindStaticNativeMethods(webviewControllerCls, controllerStaticMethods.data(),
        controllerStaticMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindStaticNativeMethods failed status: %{public}d", status);
    }
    return ANI_OK;
}
} // namespace NWeb
} // namespace OHOS
