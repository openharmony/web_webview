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

#ifndef NWEB_WEBVIEW_CONTROLLER_H
#define NWEB_WEBVIEW_CONTROLLER_H

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_web_message.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"
#include "print_manager_adapter.h"

#include "web_scheme_handler_request.h"

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

class WebPrintDocument;
class WebviewController {
public:
    explicit WebviewController() = default;
    explicit WebviewController(int32_t nwebId);
    explicit WebviewController(const std::string& webTag);
    ~WebviewController();

    bool IsInit();

    void SetWebId(int32_t nwebId);

    WebviewController* FromID(int32_t nwebId);

    bool AccessForward();

    bool AccessBackward();

    bool AccessStep(int32_t step);

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

    int32_t GetPageHeight();

    ErrCode BackOrForward(int32_t step);

    void StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env, napi_ref jsCallback);

    void StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env, napi_deferred deferred);

    std::vector<std::string> CreateWebMessagePorts();

    ErrCode PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl);

    std::shared_ptr<HitTestResult> GetHitTestValue();

    void RequestFocus();

    bool ParseUrl(napi_env env, napi_value urlObj, std::string& result);

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

    bool TerminateRenderProcess();

    void PutNetworkAvailable(bool available);

    bool HasImage(std::shared_ptr<NWebBoolValueCallback> callback);

    ErrCode HasImagesCallback(napi_env env, napi_ref jsCallback);

    ErrCode HasImagesPromise(napi_env env, napi_deferred deferred);

    void RemoveCache(bool includeDiskFiles);

    std::shared_ptr<NWebHistoryList> GetHistoryList();

    bool GetFavicon(
        const void **data, size_t &width, size_t &height, ImageColorType &colorType, ImageAlphaType &alphaType);

    std::vector<uint8_t> SerializeWebState();

    bool RestoreWebState(const std::vector<uint8_t> &state);

    void ScrollPageDown(bool bottom);

    void ScrollPageUp(bool top);

    void ScrollTo(float x, float y);

    void ScrollBy(float deltaX, float deltaY);

    void SlideScroll(float vx, float vy);

    void SetScrollable(bool enable);

    void SetScrollable(bool enable, int32_t scrollType);

    bool GetScrollable();

    void InnerSetHapPath(const std::string &hapPath);

    bool GetCertChainDerData(std::vector<std::string> &certChainDerData);

    ErrCode SetAudioMuted(bool muted);

    ErrCode PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders);

    void* CreateWebPrintDocumentAdapter(const std::string &jobName);

    ErrCode PostUrl(std::string& url, std::vector<char>& postData);

    void EnableSafeBrowsing(bool enable);

    bool IsSafeBrowsingEnabled();

    int GetSecurityLevel();

    bool IsIncognitoMode();

    void SetPrintBackground(bool enable);

    bool GetPrintBackground();

    static std::string GenerateWebTag();

    bool SetWebSchemeHandler(const char* scheme, WebSchemeHandler* handler);

    int32_t ClearWebSchemeHandler();

    static bool SetWebServiveWorkerSchemeHandler(
        const char* scheme, WebSchemeHandler* handler);

    static int32_t ClearWebServiceWorkerSchemeHandler();

    std::string GetLastJavascriptProxyCallingFrameUrl();

    void EnableIntelligentTrackingPrevention(bool enable);

    bool IsIntelligentTrackingPreventionEnabled();

    ErrCode StartCamera();

    ErrCode StopCamera();

    ErrCode CloseCamera();

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
                              std::map<std::string, std::string> &responseHeaders);

    ParseURLResult ParseURLList(napi_env env, napi_value value, std::vector<std::string>& urlList);

    bool CheckURL(std::string& url);

    std::vector<uint8_t> ParseUint8Array(napi_env env, napi_value value);

    void InjectOfflineResource(const std::vector<std::string>& urlList,
                               const std::vector<uint8_t>& resource,
                               const std::map<std::string, std::string>& response_headers,
                               const uint32_t type);

    ErrCode SetUrlTrustList(const std::string& urlTrustList, std::string& detailErrMsg);

    std::string GetSurfaceId();
    void EnableAdsBlock(bool enable);

    bool IsAdsBlockEnabled();

    bool IsAdsBlockEnabledForCurPage();

    bool ParseJsLengthToInt(napi_env env,
                            napi_value jsLength,
                            PixelUnit& type,
                            int32_t& result);

    ErrCode WebPageSnapshot(const char* id,
                            PixelUnit type,
                            int32_t width,
                            int32_t height,
                            const WebSnapshotCallback callback);

    void UpdateInstanceId(int32_t newId);

    void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive);

    void SetPathAllowingUniversalAccess(const std::vector<std::string>& pathList,
                                        std::string& errorPath);

    bool ScrollByWithResult(float deltaX, float deltaY);

    void GetScrollOffset(float* offset_x, float* offset_y);

    void ScrollToWithAnime(float x, float y, int32_t duration) ;
  
    void ScrollByWithAnime(float deltaX, float deltaY, int32_t duration) ;
private:
    int ConverToWebHitTestType(int hitType);

    bool GetRawFileUrl(const std::string &fileName,
        const std::string& bundleName, const std::string& moduleName, std::string &result);

    bool ParseRawFileUrl(napi_env env, napi_value urlObj, std::string& result);

    bool GetResourceUrl(napi_env env, napi_value urlObj, std::string& result);

    bool ParseJsLengthResourceToInt(napi_env env,
                                    napi_value jsLength,
                                    PixelUnit& type,
                                    int32_t& result);
    bool GetHapModuleInfo();

public:
    static std::string customeSchemeCmdLine_;
    static bool existNweb_;
    static bool webDebuggingAccess_;
    static std::set<std::string> webTagSet_;
    static int32_t webTagStrId_;

private:
    std::mutex webMtx_;
    int32_t nwebId_ = -1;
    std::shared_ptr<WebviewJavaScriptResultCallBack> javaScriptResultCb_ = nullptr;
    std::string hapPath_ = "";
    std::string webTag_ = "";
    std::vector<std::string> moduleName_;
};

class WebMessagePort {
public:
    WebMessagePort(int32_t nwebId, std::string& port, bool isExtentionType);

    ~WebMessagePort() = default;

    ErrCode ClosePort();

    ErrCode PostPortMessage(std::shared_ptr<NWebMessage> data);

    ErrCode SetPortMessageCallback(std::shared_ptr<NWebMessageValueCallback> callback);

    std::string GetPortHandle() const;

    bool IsExtentionType()
    {
        return isExtentionType_;
    }

private:
    int32_t nwebId_ = -1;
    std::string portHandle_;
    bool isExtentionType_;
};

class WebMessageExt {
public:
    explicit WebMessageExt(std::shared_ptr<NWebMessage> data) : data_(data) {};
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
    }

    void SetNumber(double value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::DOUBLE);
            data_->SetDouble(value);
        }
    }

    void SetBoolean(bool value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::BOOLEAN);
            data_->SetBoolean(value);
        }
    }

    void SetArrayBuffer(std::vector<uint8_t>& value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::BINARY);
            data_->SetBinary(value);
        }
    }

    void SetStringArray(std::vector<std::string> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::STRINGARRAY);
            data_->SetStringArray(value);
        }
    }

    void SetDoubleArray(std::vector<double> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::DOUBLEARRAY);
            data_->SetDoubleArray(value);
        }
    }

    void SetInt64Array(std::vector<int64_t> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::INT64ARRAY);
            data_->SetInt64Array(value);
        }
    }

    void SetBooleanArray(std::vector<bool> value)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::BOOLEANARRAY);
            data_->SetBooleanArray(value);
        }
    }

    void SetError(std::string name, std::string message)
    {
        if (data_) {
            data_->SetType(NWebValue::Type::ERROR);
            data_->SetErrName(name);
            data_->SetErrMsg(message);
        }
    }

    std::shared_ptr<NWebMessage> GetData() const
    {
        return data_;
    }

private:
    int type_ = 0;
    std::shared_ptr<NWebMessage> data_;
};

class WebHistoryList {
public:
    explicit WebHistoryList(std::shared_ptr<NWebHistoryList> sptrHistoryList) : sptrHistoryList_(sptrHistoryList) {};
    ~WebHistoryList() = default;

    int32_t GetCurrentIndex();

    std::shared_ptr<NWebHistoryItem> GetItem(int32_t index);

    int32_t GetListSize();

private:
    OHOS::NWeb::NWeb* nweb_ = nullptr;
    std::shared_ptr<NWebHistoryList> sptrHistoryList_ = nullptr;
};

class WebPrintDocument {
public:
    explicit WebPrintDocument(void* webPrintdoc) : printDocAdapter_((PrintDocumentAdapterAdapter*)webPrintdoc) {};
    ~WebPrintDocument() = default;
    void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::function<void(std::string, uint32_t)> writeResultCallback);

    void OnJobStateChanged(const std::string& jobId, uint32_t state);

private:
    std::unique_ptr<PrintDocumentAdapterAdapter> printDocAdapter_ = nullptr;
};

class WebPrintWriteResultCallbackAdapter : public PrintWriteResultCallbackAdapter {
public:
    explicit WebPrintWriteResultCallbackAdapter(std::function<void(std::string, uint32_t)>& cb) : cb_(cb) {};

    void WriteResultCallback(std::string jobId, uint32_t code) override;

private:
    std::function<void(std::string, uint32_t)> cb_;
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEBVIEW_CONTROLLER_H
