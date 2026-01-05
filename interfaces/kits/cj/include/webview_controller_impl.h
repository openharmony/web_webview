/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef WEBVIEW_CONTROLLER_IMPL_FFI_H
#define WEBVIEW_CONTROLLER_IMPL_FFI_H

#include <cstdint>
#include <map>
#include "ffi_remote_data.h"
#include "webview_utils.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_web_message.h"
#include "nweb_message_ext.h"
#include "web_scheme_handler_request.h"

namespace OHOS::Webview {
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

enum class WebMessageType : int {
    NOTSUPPORT = 0,
    STRING,
    NUMBER,
    BOOLEAN,
    ARRAYBUFFER,
    ARRAY,
    ERROR
};

enum class MediaPlaybackState: int {
    NONE = 0,
    PLAYING = 1,
    PAUSED = 2,
    STOPPED = 3
};

enum class RenderProcessMode: int {
    SINGLE = 0,
    MULTIPLE = 1
};

enum class OfflineResourceType : int {
    IMAGE = 0,
    CSS,
    CLASSIC_JS,
    MODULE_JS
};

enum class UrlListSetResult : int {
    INIT_ERROR = -2,
    PARAM_ERROR = -1,
    SET_OK = 0
};

class __attribute__((visibility("default"))) WebviewControllerImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(WebviewControllerImpl, OHOS::FFI::FFIData)
public:
    explicit WebviewControllerImpl() = default;

    explicit WebviewControllerImpl(int32_t nwebId);

    explicit WebviewControllerImpl(const std::string& webTag) : webTag_(webTag)
    {
        NWeb::NWebHelper::Instance().SetWebTag(-1, webTag_.c_str());
    };

    bool IsInit();

    void SetWebId(int32_t nwebId);

    void InnerSetHapPath(const std::string& hapPath);

    int32_t GetWebId() const;

    int32_t LoadUrl(std::string url);

    int32_t LoadUrl(std::string url, std::map<std::string, std::string> headers);

    ErrCode LoadData(
        std::string data, std::string mimeType, std::string encoding, std::string baseUrl, std::string historyUrl);

    int32_t PreFetchPage(std::string url);

    int32_t PreFetchPage(std::string url, std::map<std::string, std::string> headers);

    int32_t SetAudioMuted(bool mute);

    void SlideScroll(float vx, float vy);

    void PutNetworkAvailable(bool enable);

    void ClearClientAuthenticationCache();

    void ClearSslCache();

    void SearchNext(bool forward);

    void ClearMatches();

    void SearchAllAsync(std::string str);

    ErrCode DeleteJavaScriptRegister(const std::string& objName, const std::vector<std::string>& methodList);

    void Refresh();

    void ReloadIgnoreCache();

    std::string GetUserAgent();

    bool AccessForward();

    bool AccessBackward();

    int32_t SetCustomUserAgent(const std::string& userAgent);

    std::string GetCustomUserAgent() const;

    void RunJavaScript(std::string script, const std::function<void(RetDataCString)>& callbackRef);

    void RunJavaScriptExt(std::string script, const std::function<void(RetDataI64)>& callbackRef);

    std::string GetUrl();

    std::string GetOriginalUrl();

    void ScrollPageUp(bool top);

    void ScrollPageDown(bool bottom);

    void ScrollTo(float x, float y);

    void ScrollBy(float deltaX, float deltaY);

    void ScrollToWithAnime(float x, float y, int32_t duration);

    void ScrollByWithAnime(float deltaX, float deltaY, int32_t duration);

    void Forward();

    void Backward();

    int32_t BackOrForward(int32_t step);

    int32_t GetProgress();

    int32_t GetPageHeight();

    std::string GetTitle();

    int32_t Zoom(float factor);

    int32_t ZoomIn();

    int32_t ZoomOut();

    int32_t RequestFocus();

    void ClearHistory();

    bool AccessStep(int32_t step);

    void OnActive();

    void OnInactive();

    int32_t GetHitTest();

    std::shared_ptr<NWeb::HitTestResult> GetHitTestValue();

    void StoreWebArchiveCallback(
        std::string baseName, bool autoName, const std::function<void(RetDataCString)>& callbackRef);

    void EnableSafeBrowsing(bool enable);

    bool IsSafeBrowsingEnabled();

    int32_t GetSecurityLevel();

    bool IsIncognitoMode();

    void RemoveCache(bool includeDiskFiles);

    std::shared_ptr<OHOS::NWeb::NWebHistoryList> GetHistoryList();

    bool GetFavicon(const void **data, size_t &width, size_t &height,
        NWeb::ImageColorType &colorType, NWeb::ImageAlphaType &alphaType) const;

    void SetNWebJavaScriptResultCallBack();

    void RegisterJavaScriptProxy(const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::string& objName, const std::vector<std::string>& methodList);

    void RegisterJavaScriptProxyEx(const std::vector<std::function<char*(const char*)>>& cjFuncs,
        const std::string& objName, const std::vector<std::string>& methodList, char* permission);
    void Stop();

    void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive);

    int32_t PostUrl(std::string& url, std::vector<char>& postData);

    std::vector<std::string> CreateWebMessagePorts();

    ErrCode PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl);

    std::vector<uint8_t> SerializeWebState();

    bool RestoreWebState(const std::vector<uint8_t>& state) const;

    bool GetCertChainDerData(std::vector<std::string>& certChainDerData) const;

    ErrCode HasImagesCallback(const std::function<void(RetDataBool)>& callbackRef);

    static int32_t CustomizeSchemesArrayDataHandler(CArrScheme schemes);

    bool TerminateRenderProcess();

    void CloseAllMediaPresentations();

    void PauseAllMedia();

    void ResumeAllMedia();

    void StopAllMedia();

    void SetPrintBackground(bool enable);

    bool GetPrintBackground();

    bool GetScrollable();

    void SetScrollable(bool enable);

    void SetScrollable(bool enable, int32_t scrollType);

    void EnableAdsBlock(bool enable);

    bool IsAdsBlockEnabled();

    bool IsAdsBlockEnabledForCurPage();

    bool IsIntelligentTrackingPreventionEnabled();

    void EnableIntelligentTrackingPrevention(bool enable);

    int32_t GetMediaPlaybackState();

    std::string GetLastJavascriptProxyCallingFrameUrl();

    void StartCamera();

    void StopCamera();

    void CloseCamera();

    std::string GetSurfaceId();

    void InjectOfflineResources(const std::vector<std::string>& urlList, const std::vector<uint8_t>& resource,
        const std::map<std::string, std::string>& response_headers, const uint32_t type);

    int32_t SetUrlTrustList(const std::string& urlTrustList, std::string& detailErrMsg);

    void SetPathAllowingUniversalAccess(const std::vector<std::string>& pathList, std::string& errorPath);

    bool SetWebSchemeHandler(const char* scheme, WebSchemeHandlerImpl* handler);

    int32_t ClearWebSchemeHandler();

    static bool SetWebServiceWorkerSchemeHandler(const char* scheme, WebSchemeHandlerImpl* handler);

    static int32_t ClearWebServiceWorkerSchemeHandler();

    void OnCreateNativeMediaPlayer(std::function<int64_t(int64_t, CMediaInfo)> callback);

    int32_t PrecompileJavaScript(std::string url, std::string script,
        std::shared_ptr<OHOS::NWeb::CacheOptions> cacheOptions);

    int32_t WebPageSnapshot(const char* id, NWeb::PixelUnit type, int32_t width, int32_t height,
        const NWeb::WebSnapshotCallback callback);

    std::shared_ptr<NWeb::HitTestResult> GetLastHitTest();

    void* CreateWebPrintDocumentAdapter(const std::string &jobName);

    void GetScrollOffset(float* offset_x, float* offset_y);

    bool ScrollByWithResult(float deltaX, float deltaY) const;

    int32_t AvoidVisibleViewportBottom(int32_t avoidHeight);

    int32_t SetErrorPageEnabled(bool enable);

    bool GetErrorPageEnabled();

public:
    static std::string customeSchemeCmdLine_;
    static bool existNweb_;
    static bool webDebuggingAccess_;

private:
    int ConverToWebHitTestType(int hitType);
    bool GetHapModuleInfo();

private:
    std::mutex webMtx_;
    int32_t nwebId_ = -1;
    std::shared_ptr<WebviewJavaScriptResultCallBackImpl> javaScriptResultCb_ = nullptr;
    std::string hapPath_ = "";
    std::string webTag_ = "";
    std::vector<std::string> moduleName_;
};

class __attribute__((visibility("default"))) WebHistoryListImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(WebHistoryListImpl, OHOS::FFI::FFIData)
public:
    explicit WebHistoryListImpl(std::shared_ptr<NWeb::NWebHistoryList> sptrHistoryList)
        : sptrHistoryList_(sptrHistoryList) {};

    int32_t GetCurrentIndex();

    std::shared_ptr<OHOS::NWeb::NWebHistoryItem> GetItem(int32_t index);

    int32_t GetListSize();

private:
    std::shared_ptr<OHOS::NWeb::NWebHistoryList> sptrHistoryList_ = nullptr;
};

class WebMessagePortImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(WebMessagePortImpl, OHOS::FFI::FFIData)
public:
    WebMessagePortImpl(int32_t nwebId, std::string port, bool isExtentionType);

    ~WebMessagePortImpl() = default;

    ErrCode ClosePort();

    ErrCode PostPortMessage(std::shared_ptr<NWeb::NWebMessage> data);

    ErrCode SetPortMessageCallback(std::shared_ptr<NWeb::NWebMessageValueCallback> callback);

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

class WebMessageExtImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(WebMessageExtImpl, OHOS::FFI::FFIData)
public:
    explicit WebMessageExtImpl(std::shared_ptr<NWeb::NWebMessage> data) : data_(data) {}
    explicit WebMessageExtImpl(std::shared_ptr<NWeb::NWebHapValue> data)
    {
        data_ = NWeb::ConvertNwebHap2NwebMessage(data);
    }
    ~WebMessageExtImpl() = default;

    void SetType(int type)
    {
        type_ = type;
        WebMessageType jsType = static_cast<WebMessageType>(type);
        NWeb::NWebValue::Type nwebType = NWeb::NWebValue::Type::NONE;
        switch (jsType) {
            case WebMessageType::STRING: {
                nwebType = NWeb::NWebValue::Type::STRING;
                break;
            }
            case WebMessageType::NUMBER: {
                nwebType = NWeb::NWebValue::Type::DOUBLE;
                break;
            }
            case WebMessageType::BOOLEAN: {
                nwebType = NWeb::NWebValue::Type::BOOLEAN;
                break;
            }
            case WebMessageType::ARRAYBUFFER: {
                nwebType = NWeb::NWebValue::Type::BINARY;
                break;
            }
            case WebMessageType::ARRAY: {
                nwebType = NWeb::NWebValue::Type::STRINGARRAY;
                break;
            }
            case WebMessageType::ERROR: {
                nwebType = NWeb::NWebValue::Type::ERROR;
                break;
            }
            default: {
                nwebType = NWeb::NWebValue::Type::NONE;
                break;
            }
        }
        if (data_) {
            data_->SetType(nwebType);
        }
    }

    int ConvertNwebType2JsType(NWeb::NWebValue::Type type)
    {
        WebMessageType jsType = WebMessageType::NOTSUPPORT;
        switch (type) {
            case NWeb::NWebValue::Type::STRING: {
                jsType = WebMessageType::STRING;
                break;
            }
            case NWeb::NWebValue::Type::DOUBLE:
            case NWeb::NWebValue::Type::INTEGER: {
                jsType = WebMessageType::NUMBER;
                break;
            }
            case NWeb::NWebValue::Type::BOOLEAN: {
                jsType = WebMessageType::BOOLEAN;
                break;
            }
            case NWeb::NWebValue::Type::STRINGARRAY:
            case NWeb::NWebValue::Type::DOUBLEARRAY:
            case NWeb::NWebValue::Type::INT64ARRAY:
            case NWeb::NWebValue::Type::BOOLEANARRAY: {
                jsType = WebMessageType::ARRAY;
                break;
            }
            case NWeb::NWebValue::Type::BINARY: {
                jsType = WebMessageType::ARRAYBUFFER;
                break;
            }
            case NWeb::NWebValue::Type::ERROR: {
                jsType = WebMessageType::ERROR;
                break;
            }
            default: {
                jsType = WebMessageType::NOTSUPPORT;
                break;
            }
        }
        return static_cast<int>(jsType);
    }

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
            data_->SetType(NWeb::NWebValue::Type::STRING);
            data_->SetString(value);
        }
    }

    void SetNumber(double value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::DOUBLE);
            data_->SetDouble(value);
        }
    }

    void SetBoolean(bool value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::BOOLEAN);
            data_->SetBoolean(value);
        }
    }

    void SetArrayBuffer(std::vector<uint8_t>& value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::BINARY);
            data_->SetBinary(value);
        }
    }

    void SetStringArray(std::vector<std::string> value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::STRINGARRAY);
            data_->SetStringArray(value);
        }
    }

    void SetDoubleArray(std::vector<double> value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::DOUBLEARRAY);
            data_->SetDoubleArray(value);
        }
    }

    void SetInt64Array(std::vector<int64_t> value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::INT64ARRAY);
            data_->SetInt64Array(value);
        }
    }

    void SetBooleanArray(std::vector<bool> value)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::BOOLEANARRAY);
            data_->SetBooleanArray(value);
        }
    }

    void SetError(std::string name, std::string message)
    {
        if (data_) {
            data_->SetType(NWeb::NWebValue::Type::ERROR);
            data_->SetErrName(name);
            data_->SetErrMsg(message);
        }
    }

    std::shared_ptr<NWeb::NWebMessage> GetData()
    {
        return data_;
    }

private:
    int type_ = 0;
    std::shared_ptr<NWeb::NWebMessage> data_;
};

class NWebMessageCallbackImpl : public NWeb::NWebMessageValueCallback {
public:
    NWebMessageCallbackImpl(std::function<void(RetWebMessage)> callback) : callback_(callback) {}
    ~NWebMessageCallbackImpl() = default;
    void OnReceiveValue(std::shared_ptr<NWeb::NWebMessage> result) override;
    void OnReceiveValueV2(std::shared_ptr<NWeb::NWebHapValue> value) override;

private:
    std::function<void(RetWebMessage)> callback_;
};

class NWebWebMessageExtCallbackImpl : public NWeb::NWebMessageValueCallback {
public:
    NWebWebMessageExtCallbackImpl(std::function<void(int64_t)> callback) : callback_(callback) {}
    ~NWebWebMessageExtCallbackImpl() = default;
    void OnReceiveValue(std::shared_ptr<NWeb::NWebMessage> result) override;
    void OnReceiveValueV2(std::shared_ptr<NWeb::NWebHapValue> value) override;

private:
    std::function<void(int64_t)> callback_;
};
} // namespace OHOS::Webview
#endif // WEBVIEW_CONTROLLER_IMPL_FFI_H