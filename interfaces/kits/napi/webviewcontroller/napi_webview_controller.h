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

#ifndef NWEB_NAPI_WEBVIEW_CONTROLLER_H
#define NWEB_NAPI_WEBVIEW_CONTROLLER_H

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "uv.h"
#include "webview_controller.h"

namespace OHOS {
namespace NWeb {
const std::string WEBVIEW_CONTROLLER_CLASS_NAME = "WebviewController";
const std::string WEB_MESSAGE_PORT_CLASS_NAME = "WebMessagePort";
const std::string WEB_PORT_MSG_ENUM_NAME = "WebMessageType";
const std::string WEB_EXT_MSG_CLASS_NAME = "WebMessageExt";
const std::string WEB_HITTESTTYPE_V9_ENUM_NAME = "HitTestTypeV9";
const std::string WEB_HITTESTTYPE_ENUM_NAME = "WebHitTestType";
const std::string WEB_HISTORY_LIST_CLASS_NAME = "WebHistoryList";
const std::string WEB_SECURE_DNS_MODE_ENUM_NAME = "SecureDnsMode";
const std::string WEB_PRINT_DOCUMENT_CLASS_NAME = "WebPrintDocument";
const std::string WEB_SECURITY_LEVEL_ENUM_NAME = "WebSecurityLevel";
const std::string WEB_RENDER_PROCESS_MODE_ENUM_NAME = "RenderProcessMode";
const std::string OFFLINE_RESOURCE_TYPE_ENUM_NAME = "OfflineResourceType";
const std::string WEB_SCROLL_TYPE_ENUM_NAME = "ScrollType";
const std::string WEB_PRESSURE_LEVEL_ENUM_NAME = "PressureLevel";
constexpr double TEN_MILLIMETER_TO_INCH = 0.39;

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

struct OfflineResourceValue {
    napi_value urlList;
    napi_value resource;
    napi_value responseHeaders;
    napi_value type;
};

struct PDFMarginConfig {
    double top = TEN_MILLIMETER_TO_INCH;
    double bottom = TEN_MILLIMETER_TO_INCH;
    double right = TEN_MILLIMETER_TO_INCH;
    double left = TEN_MILLIMETER_TO_INCH;
};

class NapiWebviewController {
public:
    NapiWebviewController() {}
    ~NapiWebviewController() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value InitializeWebEngine(napi_env env, napi_callback_info info);

    static napi_value SetHttpDns(napi_env env, napi_callback_info info);

    static napi_value SetWebDebuggingAccess(napi_env env, napi_callback_info info);

    static napi_value EnableSafeBrowsing(napi_env env, napi_callback_info info);

    static napi_value IsSafeBrowsingEnabled(napi_env env, napi_callback_info info);

    static napi_value InnerGetWebDebuggingAccess(napi_env env, napi_callback_info info);

    static napi_value SetWebId(napi_env env, napi_callback_info info);

    static napi_value InnerJsProxy(napi_env env, napi_callback_info info);

    static napi_value InnerGetCustomeSchemeCmdLine(napi_env env, napi_callback_info info);

    static napi_value AccessForward(napi_env env, napi_callback_info info);

    static napi_value AccessBackward(napi_env env, napi_callback_info info);

    static napi_value Forward(napi_env env, napi_callback_info info);

    static napi_value Backward(napi_env env, napi_callback_info info);

    static napi_value AccessStep(napi_env env, napi_callback_info info);

    static napi_value ClearHistory(napi_env env, napi_callback_info info);

    static napi_value OnActive(napi_env env, napi_callback_info info);

    static napi_value OnInactive(napi_env env, napi_callback_info info);

    static napi_value Refresh(napi_env env, napi_callback_info info);

    static napi_value ZoomIn(napi_env env, napi_callback_info info);

    static napi_value ZoomOut(napi_env env, napi_callback_info info);

    static napi_value GetWebId(napi_env env, napi_callback_info info);

    static napi_value GetUserAgent(napi_env env, napi_callback_info info);

    static napi_value GetCustomUserAgent(napi_env env, napi_callback_info info);

    static napi_value SetCustomUserAgent(napi_env env, napi_callback_info info);

    static napi_value GetTitle(napi_env env, napi_callback_info info);

    static napi_value GetPageHeight(napi_env env, napi_callback_info info);

    static napi_value BackOrForward(napi_env env, napi_callback_info info);

    static napi_value StoreWebArchive(napi_env env, napi_callback_info info);

    static napi_value StoreWebArchiveInternal(napi_env env, napi_callback_info info,
        const std::string &baseName, bool autoName);

    static napi_value CreateWebMessagePorts(napi_env env, napi_callback_info info);

    static napi_value PostMessage(napi_env env, napi_callback_info info);

    static napi_value GetHitTestValue(napi_env env, napi_callback_info info);

    static napi_value RequestFocus(napi_env env, napi_callback_info info);

    static napi_value LoadUrl(napi_env env, napi_callback_info info);

    static napi_value LoadUrlWithHttpHeaders(napi_env env, napi_callback_info info, const std::string& url,
        const napi_value* argv, WebviewController* webviewController);

    static napi_value LoadData(napi_env env, napi_callback_info info);

    static napi_value GetHitTest(napi_env env, napi_callback_info info);

    static napi_value ClearMatches(napi_env env, napi_callback_info info);

    static napi_value SearchNext(napi_env env, napi_callback_info info);

    static napi_value SearchAllAsync(napi_env env, napi_callback_info info);

    static napi_value ClearSslCache(napi_env env, napi_callback_info info);

    static napi_value ClearClientAuthenticationCache(napi_env env, napi_callback_info info);

    static napi_value Stop(napi_env env, napi_callback_info info);

    static napi_value Zoom(napi_env env, napi_callback_info info);

    static napi_value InnerCompleteWindowNew(napi_env env, napi_callback_info info);

    static napi_value RegisterJavaScriptProxy(napi_env env, napi_callback_info info);

    static napi_value DeleteJavaScriptRegister(napi_env env, napi_callback_info info);

    static napi_value RunJavaScript(napi_env env, napi_callback_info info);

    static napi_value RunJavaScriptExt(napi_env env, napi_callback_info info);

    static napi_value RunJS(napi_env env, napi_callback_info info, bool extention);

    static napi_value RunJavaScriptInternal(napi_env env, napi_callback_info info,
        const std::string &script, bool extention);

    static ErrCode ConstructFlowbuf(napi_env env, napi_value argv, int& fd, size_t& scriptLength);

    static napi_value RunJSBackToOriginal(napi_env env, napi_callback_info info,
        bool extention, napi_value argv, napi_value result);

    static napi_value RunJavaScriptInternalExt(napi_env env, napi_callback_info info, bool extention);

    static napi_value RunCreatePDFExt(napi_env env, napi_callback_info info);

    static napi_value TerminateRenderProcess(napi_env env, napi_callback_info info);

    static napi_value GetUrl(napi_env env, napi_callback_info info);

    static napi_value GetOriginalUrl(napi_env env, napi_callback_info info);

    static napi_value SetNetworkAvailable(napi_env env, napi_callback_info info);

    static napi_value InnerGetWebId(napi_env env, napi_callback_info info);

    static napi_value HasImage(napi_env env, napi_callback_info info);

    static napi_value HasImageInternal(napi_env env, napi_callback_info info);

    static napi_value RemoveCache(napi_env env, napi_callback_info info);

    static napi_value getBackForwardEntries(napi_env env, napi_callback_info info);

    static napi_value GetFavicon(napi_env env, napi_callback_info info);

    static napi_value SerializeWebState(napi_env env, napi_callback_info info);

    static napi_value RestoreWebState(napi_env env, napi_callback_info info);

    static napi_value ScrollPageDown(napi_env env, napi_callback_info info);

    static napi_value ScrollPageUp(napi_env env, napi_callback_info info);

    static napi_value ScrollTo(napi_env env, napi_callback_info info);

    static napi_value ScrollBy(napi_env env, napi_callback_info info);

    static napi_value SlideScroll(napi_env env, napi_callback_info info);

    static napi_value SetScrollable(napi_env env, napi_callback_info info);

    static napi_value GetScrollable(napi_env env, napi_callback_info info);

    static napi_value CustomizeSchemes(napi_env env, napi_callback_info info);

    static napi_value InnerSetHapPath(napi_env env, napi_callback_info info);

    static napi_value InnerGetCertificate(napi_env env, napi_callback_info info);

    static napi_value SetAudioMuted(napi_env env, napi_callback_info info);

    static napi_value InnerGetThisVar(napi_env env, napi_callback_info info);

    static napi_value PrefetchPage(napi_env env, napi_callback_info info);

    static napi_value PrefetchPageWithHttpHeaders(napi_env env, napi_callback_info info, std::string& url,
        const napi_value* argv, WebviewController* webviewController);

    static napi_value PrepareForPageLoad(napi_env env, napi_callback_info info);

    static napi_value CreateWebPrintDocumentAdapter(napi_env env, napi_callback_info info);

    static napi_value PostUrl(napi_env env, napi_callback_info info);

    static napi_value SetDownloadDelegate(napi_env env, napi_callback_info info);

    static napi_value StartDownload(napi_env env, napi_callback_info info);

    static napi_value SetConnectionTimeout(napi_env env, napi_callback_info info);

    static napi_value GetSecurityLevel(napi_env env, napi_callback_info info);

    static napi_value IsIncognitoMode(napi_env env, napi_callback_info info);

    static napi_value SetPrintBackground(napi_env env, napi_callback_info info);

    static napi_value GetPrintBackground(napi_env env, napi_callback_info info);

    static napi_value SetWebSchemeHandler(napi_env env, napi_callback_info info);

    static napi_value ClearWebSchemeHandler(napi_env env, napi_callback_info info);

    static napi_value SetServiceWorkerWebSchemeHandler(
        napi_env env, napi_callback_info info);

    static napi_value ClearServiceWorkerWebSchemeHandler(
        napi_env env, napi_callback_info info);

    static napi_value EnableIntelligentTrackingPrevention(
        napi_env env, napi_callback_info info);

    static napi_value IsIntelligentTrackingPreventionEnabled(
        napi_env env, napi_callback_info info);

    static napi_value AddIntelligentTrackingPreventionBypassingList(
        napi_env env, napi_callback_info info);

    static napi_value RemoveIntelligentTrackingPreventionBypassingList(
        napi_env env, napi_callback_info info);

    static napi_value ClearIntelligentTrackingPreventionBypassingList(
        napi_env env, napi_callback_info info);

    static napi_value GetDefaultUserAgent(napi_env env, napi_callback_info info);

    static napi_value GetLastJavascriptProxyCallingFrameUrl(napi_env env, napi_callback_info info);

    static napi_value PauseAllTimers(napi_env env, napi_callback_info info);

    static napi_value ResumeAllTimers(napi_env env, napi_callback_info info);

    static napi_value StartCamera(napi_env env, napi_callback_info info);

    static napi_value StopCamera(napi_env env, napi_callback_info info);

    static napi_value CloseCamera(napi_env env, napi_callback_info info);

    static napi_value CloseAllMediaPresentations(napi_env env, napi_callback_info info);

    static napi_value StopAllMedia(napi_env env, napi_callback_info info);

    static napi_value ResumeAllMedia(napi_env env, napi_callback_info info);

    static napi_value PauseAllMedia(napi_env env, napi_callback_info info);

    static napi_value GetMediaPlaybackState(napi_env env, napi_callback_info info);

    static napi_value PrefetchResource(napi_env env, napi_callback_info info);

    static napi_value ClearPrefetchedResource(napi_env env, napi_callback_info info);

    static napi_value OnCreateNativeMediaPlayer(napi_env env, napi_callback_info info);

    static napi_value SetRenderProcessMode(napi_env env, napi_callback_info info);

    static napi_value GetRenderProcessMode(napi_env env, napi_callback_info info);

    static napi_value PrecompileJavaScript(napi_env env, napi_callback_info info);

    static napi_value InjectOfflineResources(napi_env env, napi_callback_info info);

    static void AddResourcesToMemoryCache(napi_env env,
                                          napi_callback_info info,
                                          napi_value& resourcesList);

    static void AddResourceItemToMemoryCache(napi_env env,
                                             napi_callback_info info,
                                             OfflineResourceValue resourceValue);

    static napi_value SetHostIP(napi_env env, napi_callback_info info);

    static napi_value ClearHostIP(napi_env env, napi_callback_info info);

    static napi_value WarmupServiceWorker(napi_env env, napi_callback_info info);

    static napi_value GetSurfaceId(napi_env env, napi_callback_info info);

    static napi_value EnableWholeWebPageDrawing(napi_env env, napi_callback_info info);

    static napi_value EnableAdsBlock(napi_env env, napi_callback_info info);

    static napi_value IsAdsBlockEnabled(napi_env env, napi_callback_info info);

    static napi_value IsAdsBlockEnabledForCurPage(napi_env env, napi_callback_info info);

    static napi_value WebPageSnapshot(napi_env env, napi_callback_info info);

    static napi_value SetUrlTrustList(napi_env env, napi_callback_info info);

    static napi_value UpdateInstanceId(napi_env env, napi_callback_info info);

    static napi_value SetPathAllowingUniversalAccess(napi_env env, napi_callback_info info);

    static napi_value EnableBackForwardCache(napi_env env, napi_callback_info info);

    static napi_value SetBackForwardCacheOptions(napi_env env, napi_callback_info info);

    static napi_value ScrollByWithResult(napi_env env, napi_callback_info info);

    static napi_value GetScrollOffset(napi_env env, napi_callback_info info);

    static napi_value TrimMemoryByPressureLevel(napi_env env, napi_callback_info info);

    static int32_t maxFdNum_;
    static std::atomic<int32_t> usedFd_;
};

class NWebValueCallbackImpl : public NWebMessageValueCallback {
public:
    NWebValueCallbackImpl(napi_env env, napi_ref callback, bool extention)
        : env_(env), callback_(callback), extention_(extention)
    {}
    ~NWebValueCallbackImpl();
    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;

private:
    napi_env env_;
    napi_ref callback_;
    bool extention_;
    static void UvWebMessageOnReceiveValueCallback(uv_work_t *work, int status);
};

class NapiWebMessageExt {
public:
    NapiWebMessageExt() = default;
    ~NapiWebMessageExt() = default;

    static napi_value JsConstructor(napi_env env, napi_callback_info info);
    static napi_value GetType(napi_env env, napi_callback_info info);
    static napi_value GetString(napi_env env, napi_callback_info info);
    static napi_value GetNumber(napi_env env, napi_callback_info info);
    static napi_value GetBoolean(napi_env env, napi_callback_info info);
    static napi_value GetArrayBuffer(napi_env env, napi_callback_info info);
    static napi_value GetArray(napi_env env, napi_callback_info info);
    static napi_value GetError(napi_env env, napi_callback_info info);
    static napi_value SetType(napi_env env, napi_callback_info info);
    static napi_value SetString(napi_env env, napi_callback_info info);
    static napi_value SetNumber(napi_env env, napi_callback_info info);
    static napi_value SetBoolean(napi_env env, napi_callback_info info);
    static napi_value SetArrayBuffer(napi_env env, napi_callback_info info);
    static napi_value SetArray(napi_env env, napi_callback_info info);
    static napi_value SetError(napi_env env, napi_callback_info info);
};

class NapiWebMessagePort {
public:
    NapiWebMessagePort() = default;
    ~NapiWebMessagePort() = default;

    struct WebMsgPortParam {
        bool extention_;
        napi_env env_;
        napi_ref callback_;
        std::shared_ptr<NWebMessage> msg_;
        std::mutex mutex_;
        bool ready_ = false;
        std::condition_variable condition_;
    };

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value Close(napi_env env, napi_callback_info info);

    static napi_value PostMessageEvent(napi_env env, napi_callback_info info);

    static napi_value OnMessageEvent(napi_env env, napi_callback_info info);

    static napi_value PostMessageEventExt(napi_env env, napi_callback_info info);

    static napi_value OnMessageEventExt(napi_env env, napi_callback_info info);
};

class NapiWebHistoryList {
public:
    NapiWebHistoryList() = default;
    ~NapiWebHistoryList() = default;

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value GetItem(napi_env env, napi_callback_info info);

private:
    static napi_value GetFavicon(napi_env env, std::shared_ptr<NWebHistoryItem> item);
};

class NapiWebPrintDocument {
public:
    NapiWebPrintDocument() = default;
    ~NapiWebPrintDocument() = default;

    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value OnStartLayoutWrite(napi_env env, napi_callback_info info);

    static napi_value OnJobStateChanged(napi_env env, napi_callback_info info);
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEBVIEW_CONTROLLER_H
