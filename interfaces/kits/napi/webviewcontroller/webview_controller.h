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

#ifndef NWEB_WEBVIEW_CONTROLLER_H
#define NWEB_WEBVIEW_CONTROLLER_H

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "auto_napi_ref.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_snapshot_callback.h"
#include "nweb_web_message.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"
#include "webview_controller_print_structs.h"

#include "web_scheme_handler.h"
#include "webview_value.h"

namespace OHOS {
namespace NWeb {
enum class WebHitTestType : int {
    EDIT = 0,
    EMAIL,
    HTTP,
    HTTP_IMG,
    IMG,
    MAP,
    PHONE,
    UNKNOWN
};

enum class SecureDnsModeType : int {
    OFF = 0,
    AUTO,
    SECURE_ONLY
};

enum class ResourceType : uint32_t {
    COLOR = 10001,
    FLOAT,
    STRING,
    PLURAL,
    BOOLEAN,
    INTARRAY,
    INTEGER,
    PATTERN,
    STRARRAY,
    MEDIA = 20000,
    RAWFILE = 30000
};

enum class WebMessageType : int {
    NOTSUPPORT = 0,
    STRING,
    NUMBER,
    BOOLEAN,
    ARRAYBUFFER,
    ARRAY,
    ERROR
};

enum class MediaPlaybackState : int {
    NONE = 0,
    PLAYING,
    PAUSED,
    STOP
};

enum class SecurityLevel : int {
    NONE = 0,
    SECURE,
    WARNING,
    DANGEROUS
};

enum class CoreSecurityLevel : int {
    NONE = 0,
    SECURE = 3,
    WARNING = 6,
    DANGEROUS = 5
};

enum class OfflineResourceType : int {
    IMAGE = 0,
    CSS,
    CLASSIC_JS,
    MODULE_JS
};

enum class ParseURLResult : int {
    OK = 0,
    FAILED,
    INVALID_URL
};

enum class UrlListSetResult : int {
    INIT_ERROR = -2,
    PARAM_ERROR = -1,
    SET_OK = 0,
};

enum class ScrollType : int {
    EVENT = 0,
};

enum class PressureLevel : int {
    MEMORY_PRESSURE_LEVEL_MODERATE = 1,
    MEMORY_PRESSURE_LEVEL_CRITICAL = 2,
};

enum class AttachState : int {
    NOT_ATTACHED = 0,
    ATTACHED = 1
};

enum class BlanklessErrorCode : int {
    SUCCESS = 0,
    ERR_UNKNOWN = -1,
    ERR_INVALID_PARAM = -2,
    ERR_CONTROLLER_NOT_INITED = -3,
    ERR_KEY_NOT_MATCH = -4,
    ERR_SIGNIFICANT_CHANGE = -5
};

struct BlanklessLoadingParamValue {
    bool enable = false;
    int32_t duration = 0;
    int64_t expirationTime = 0;
    napi_ref callback = nullptr;
};

class WebRegObj {
public:
    WebRegObj() : m_regEnv(0), m_regHanderRef(nullptr) {
    }

    explicit WebRegObj(const napi_env& env, const napi_ref& ref)
    {
        m_regEnv = env;
        m_regHanderRef = ref;
        m_isMarked = false;
    }

    ~WebRegObj() {
    }

    napi_env m_regEnv;
    napi_ref m_regHanderRef;
    bool m_isMarked;
};

class WebPrintDocument;
class WebviewController {
public:
    explicit WebviewController() = default;
    explicit WebviewController(int32_t nwebId);
    explicit WebviewController(const std::string& webTag);
    ~WebviewController();

    bool IsInit() const;

    void SetWebId(int32_t nwebId);

    WebviewController* FromID(int32_t nwebId);

    bool AccessForward() const;

    bool AccessBackward() const;

    bool AccessStep(int32_t step) const;

    void ClearHistory();

    void Forward();

    void Backward();

    void OnActive();

    void OnInactive();

    void Refresh();

    ErrCode ZoomIn();

    ErrCode ZoomOut();

    int32_t GetWebId() const;

    std::string GetUserAgent();

    std::string GetCustomUserAgent() const;

    ErrCode SetCustomUserAgent(const std::string& userAgent);

    std::string GetTitle();

    int32_t GetProgress();

    int32_t GetPageHeight();

    ErrCode BackOrForward(int32_t step);

    void StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env, napi_ref jsCallback);

    void StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env, napi_deferred deferred);

    std::vector<std::string> CreateWebMessagePorts();

    ErrCode PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl);

    std::shared_ptr<HitTestResult> GetHitTestValue();

    void RequestFocus();

    bool ParseUrl(napi_env env, napi_value urlObj, std::string& result) const;

    ErrCode LoadUrl(std::string url);

    ErrCode LoadUrl(std::string url, std::map<std::string, std::string> httpHeaders);

    ErrCode LoadData(std::string data, std::string mimeType, std::string encoding,
        std::string baseUrl, std::string historyUrl);

    int GetHitTest();

    void ClearMatches();

    void SearchNext(bool forward);

    void SearchAllAsync(const std::string& searchString);

    void ClearSslCache();

    void ClearClientAuthenticationCache();

    void Stop();

    ErrCode Zoom(float factor);

    void InnerCompleteWindowNew(int32_t parentNwebId);

    void SetNWebJavaScriptResultCallBack();

    void RegisterJavaScriptProxy(RegisterJavaScriptProxyParam& param);

    ErrCode DeleteJavaScriptRegister(const std::string& objName,
        const std::vector<std::string>& methodList);

    void RunJavaScriptCallback(const std::string &script, napi_env env, napi_ref jsCallback, bool extention);

    void RunJavaScriptPromise(const std::string &script, napi_env env, napi_deferred deferred, bool extention);

    void RunJavaScriptCallbackExt(
        const int fd, const size_t scriptLength, napi_env env, napi_ref jsCallback, bool extention);

    void RunJavaScriptPromiseExt(
        const int fd, const size_t scriptLength, napi_env env, napi_deferred deferred, bool extention);

    std::string GetUrl();

    std::string GetOriginalUrl();

    bool TerminateRenderProcess() const;

    void PutNetworkAvailable(bool available);

    bool HasImage(std::shared_ptr<NWebBoolValueCallback> callback);

    ErrCode HasImagesCallback(napi_env env, napi_ref jsCallback);

    ErrCode HasImagesPromise(napi_env env, napi_deferred deferred);

    void RemoveCache(bool includeDiskFiles);

    std::shared_ptr<NWebHistoryList> GetHistoryList();

    bool GetFavicon(
        const void **data, size_t &width, size_t &height, ImageColorType &colorType, ImageAlphaType &alphaType) const;

    std::vector<uint8_t> SerializeWebState();

    bool RestoreWebState(const std::vector<uint8_t> &state) const;

    void ScrollPageDown(bool bottom);

    void ScrollPageUp(bool top);

    void ScrollTo(float x, float y);

    void ScrollBy(float deltaX, float deltaY);

    void SlideScroll(float vx, float vy);

    void SetScrollable(bool enable);

    void SetScrollable(bool enable, int32_t scrollType);

    bool GetScrollable() const;

    void InnerSetHapPath(const std::string &hapPath);

    void InnerSetFavicon(napi_env env, napi_value favicon);

    napi_value InnerGetFavicon(napi_env env);

    bool GetCertChainDerData(std::vector<std::string> &certChainDerData) const;

    ErrCode SetAudioMuted(bool muted);

    ErrCode PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders);

    ErrCode PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders,
        std::shared_ptr<NWebPrefetchOptions> prefetchOptions);

    void* CreateWebPrintDocumentAdapter(const std::string &jobName);

    ErrCode PostUrl(std::string& url, std::vector<char>& postData);

    void EnableSafeBrowsing(bool enable);

    bool IsSafeBrowsingEnabled() const;

    int GetSecurityLevel();

    bool IsIncognitoMode() const;

    void SetPrintBackground(bool enable);

    bool GetPrintBackground() const;

    static std::string GenerateWebTag();

    bool SetWebSchemeHandler(const char* scheme, WebSchemeHandler* handler) const;

    int32_t ClearWebSchemeHandler();

    static bool SetWebServiveWorkerSchemeHandler(
        const char* scheme, WebSchemeHandler* handler);

    static int32_t ClearWebServiceWorkerSchemeHandler();

    std::string GetLastJavascriptProxyCallingFrameUrl();

    void EnableIntelligentTrackingPrevention(bool enable);

    bool IsIntelligentTrackingPreventionEnabled() const;

    ErrCode StartCamera();

    ErrCode StopCamera();

    ErrCode CloseCamera();

    ErrCode ResumeMicrophone();

    ErrCode StopMicrophone();

    ErrCode PauseMicrophone();

    void CloseAllMediaPresentations();

    void StopAllMedia();

    void ResumeAllMedia();

    void PauseAllMedia();

    int GetMediaPlaybackState();

    void OnCreateNativeMediaPlayer(napi_env env, napi_ref callback);

    bool ParseScriptContent(napi_env env, napi_value value, std::string &script);

    std::shared_ptr<CacheOptions> ParseCacheOptions(napi_env env, napi_value value);

    void PrecompileJavaScriptPromise(napi_env env,
                                     napi_deferred deferred,
                                     const std::string &url, const std::string &script,
                                     std::shared_ptr<CacheOptions> cacheOptions);

    bool ParseResponseHeaders(napi_env env,
                              napi_value value,
                              std::map<std::string, std::string> &responseHeaders) const;

    ParseURLResult ParseURLList(napi_env env, napi_value value, std::vector<std::string>& urlList);

    bool CheckURL(std::string& url) const;

    std::vector<uint8_t> ParseUint8Array(napi_env env, napi_value value);

    void InjectOfflineResource(const std::vector<std::string>& urlList,
                               const std::vector<uint8_t>& resource,
                               const std::map<std::string, std::string>& response_headers,
                               const uint32_t type);

    ErrCode SetUrlTrustList(const std::string& urlTrustList, std::string& detailErrMsg);

    std::string GetSurfaceId();
    void EnableAdsBlock(bool enable);

    bool IsAdsBlockEnabled() const;

    bool IsAdsBlockEnabledForCurPage() const;

    bool ParseJsLengthToInt(napi_env env,
                            napi_value jsLength,
                            PixelUnit& type,
                            int32_t& result) const;

    ErrCode WebPageSnapshot(const char* id,
                            PixelUnit type,
                            int32_t width,
                            int32_t height,
                            const WebSnapshotCallback callback);

    void UpdateInstanceId(int32_t newId);

    void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive);

    void SetPathAllowingUniversalAccess(const std::vector<std::string>& pathList,
                                        std::string& errorPath);

    bool ScrollByWithResult(float deltaX, float deltaY) const;

    void ScrollToWithAnime(float x, float y, int32_t duration) ;

    void ScrollByWithAnime(float deltaX, float deltaY, int32_t duration) ;

    void GetScrollOffset(float* offset_x, float* offset_y);

    void GetPageOffset(float* offset_x, float* offset_y);

    void CreatePDFCallbackExt(
        napi_env env, std::shared_ptr<NWebPDFConfigArgs> pdfConfig, napi_ref pdfCallback);

    void CreatePDFPromiseExt(
        napi_env env, std::shared_ptr<NWebPDFConfigArgs> pdfConfig, napi_deferred deferred);

    std::shared_ptr<HitTestResult> GetLastHitTest();

    void SaveWebSchemeHandler(const char* scheme, WebSchemeHandler* handler);

    static void SaveWebServiceWorkerSchemeHandler(const char* scheme, WebSchemeHandler* handler);

    int GetAttachState();

    void RegisterStateChangeCallback(
        const napi_env& env, const std::string& type, napi_value handler);

    void TriggerStateChangeCallback(const std::string& type);

    void DeleteRegisterObj(
        const napi_env& env, std::vector<WebRegObj>& vecRegObjs, napi_value& handler);

    void DeleteAllRegisterObj(const napi_env& env, std::vector<WebRegObj>& vecRegObjs);

    void UnregisterStateChangeCallback(
        const napi_env& env, const std::string& type, napi_value handler);

    static void WaitForAttached(napi_env env, void* data);

    static void TriggerWaitforAttachedPromise(napi_env env, napi_status status, void *data);

    napi_value WaitForAttachedPromise(napi_env env, int32_t timeout, napi_deferred deferred);

    int32_t GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime);

    int32_t SetBlanklessLoadingWithKey(const std::string& key, bool isStart);

    int32_t SetBlanklessLoadingParams(napi_env env, const std::string& key, BlanklessLoadingParamValue& paramsValue);

    void SetWebDetach(int32_t nwebId);

    void SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode mode);

    ErrCode AvoidVisibleViewportBottom(int32_t avoidHeight);

    ErrCode SetErrorPageEnabled(bool enable);

    bool GetErrorPageEnabled();
private:
    int ConverToWebHitTestType(int hitType);

    bool GetRawFileUrl(const std::string &fileName,
        const std::string& bundleName, const std::string& moduleName, std::string &result) const;

    bool ParseRawFileUrl(napi_env env, napi_value urlObj, std::string& result) const;

    bool GetResourceUrl(napi_env env, napi_value urlObj, std::string& result) const;

    bool ParseJsLengthResourceToInt(napi_env env,
                                    napi_value jsLength,
                                    PixelUnit& type,
                                    int32_t& result) const;
    bool GetHapModuleInfo();

    void DeleteWebSchemeHandler();

    static void DeleteWebServiceWorkerSchemeHandler();

public:
    static std::string customeSchemeCmdLine_;
    static bool existNweb_;
    static bool webDebuggingAccess_;
    static int32_t webDebuggingPort_;
    static std::set<std::string> webTagSet_;
    static int32_t webTagStrId_;

private:
    std::mutex webMtx_;
    int32_t nwebId_ = -1;
    std::shared_ptr<WebviewJavaScriptResultCallBack> javaScriptResultCb_ = nullptr;
    std::string hapPath_ = "";
    std::string webTag_ = "";
    AutoNapiRef favicon_;
    std::vector<std::string> moduleName_;
    std::map<std::string, WebSchemeHandler*> webSchemeHandlerMap_;
    static std::map<std::string, WebSchemeHandler*> webServiceWorkerSchemeHandlerMap_;
    AttachState attachState_ = AttachState::NOT_ATTACHED;
    std::unordered_map<std::string, std::vector<WebRegObj>> attachEventRegisterInfo_;
    std::condition_variable attachCond_;
    std::mutex attachMtx_;
};

class WebMessageExt {
public:
    explicit WebMessageExt(std::shared_ptr<NWebMessage> data,
        std::shared_ptr<NWebRomValue> value = nullptr)
        : data_(data), value_(value) {}
    ~WebMessageExt() = default;

    void SetType(int type);

    int ConvertNwebType2JsType(NWebValue::Type type);

    int GetType()
    {
        if (data_) {
            return ConvertNwebType2JsType(data_->GetType());
        }
        return static_cast<int>(WebMessageType::NOTSUPPORT);
    }

    void SetString(std::string value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::STRING);
            data_->SetString(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::STRING);
            value_->SetString(value);
        }
    }

    void SetNumber(double value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::DOUBLE);
            data_->SetDouble(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::DOUBLE);
            value_->SetDouble(value);
        }
    }

    void SetBoolean(bool value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::BOOLEAN);
            data_->SetBoolean(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::BOOLEAN);
            value_->SetBool(value);
        }
    }

    void SetArrayBuffer(std::vector<uint8_t>& value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::BINARY);
            data_->SetBinary(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::BINARY);
            value_->SetBinary(value);
        }
    }

    void SetStringArray(std::vector<std::string> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::STRINGARRAY);
            data_->SetStringArray(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::STRINGARRAY);
            value_->SetStringArray(value);
        }
    }

    void SetDoubleArray(std::vector<double> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::DOUBLEARRAY);
            data_->SetDoubleArray(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::DOUBLEARRAY);
            value_->SetDoubleArray(value);
        }
    }

    void SetInt64Array(std::vector<int64_t> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::INT64ARRAY);
            data_->SetInt64Array(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::INT64ARRAY);
            value_->SetInt64Array(value);
        }
    }

    void SetBooleanArray(std::vector<bool> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::BOOLEANARRAY);
            data_->SetBooleanArray(value);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::BOOLEANARRAY);
            value_->SetBoolArray(value);
        }
    }

    void SetError(std::string name, std::string message)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::ERROR);
            data_->SetErrName(name);
            data_->SetErrMsg(message);
        }
        if (value_) {
            value_->SetType(NWebRomValue::Type::ERROR);
            value_->SetErrName(name);
            value_->SetErrMsg(message);
        }
    }

    std::shared_ptr<NWebMessage> GetData() const
    {
        return data_;
    }

    std::shared_ptr<NWebRomValue> GetValue() const
    {
        return value_;
    }

private:
    int type_ = 0;
    std::shared_ptr<NWebMessage> data_;
    std::shared_ptr<NWebRomValue> value_;
};

} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEBVIEW_CONTROLLER_H
