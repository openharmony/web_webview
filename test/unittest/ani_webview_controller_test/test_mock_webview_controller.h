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

#ifndef TEST_MOCK_WEBVIEW_CONTROLLER_H
#define TEST_MOCK_WEBVIEW_CONTROLLER_H

#include <string>
#include <cstdint>
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <functional>
#include "web_errors.h"


// Forward declaration of napi related types
struct napi_env__;
typedef napi_env__* napi_env;
struct napi_value__;
typedef napi_value__* napi_value;
struct napi_deferred__;
typedef napi_deferred__* napi_deferred;
struct napi_ref__;
typedef napi_ref__* napi_ref;

namespace OHOS {
namespace NWeb {

struct RegisterJavaScriptProxyParam;
struct AniRegisterJavaScriptProxyParam;
struct PrintAttributesAdapter;

class NWebBoolValueCallback;
class NWebArrayBufferValueCallback;
class NWebMessageValueCallback;

class WebSchemeHandler;
class NWebPrefetchOptions;
class NWebPDFConfigArgs;
class NWebHistoryList;
class HitTestResult;
class NWebMessage;
class NWebRomValue;
class NWebUserAgentMetadata;
class CacheOptions;
class WebJsMessageExt;
class NWebValue;

const std::string CONTROLLER_ATTACH_STATE_CHANGE = "controllerAttachStateChange";

using WebSnapshotCallback = std::function<void(const char*, bool, float, void*, int, int)>;

enum class ImageColorType : int;
enum class ImageAlphaType : int;
enum class PixelUnit : int;

enum class ParseURLResult : int {
    OK = 0,
    FAILED,
    INVALID_URL
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

enum class SecureDnsModeType : int {
    OFF = 0,
    AUTO = 1,
    SECURE_ONLY = 2,
};

struct OfflineResourceValueAni {
    ani_ref urlListArray;
    ani_ref resourceArrayBuffer;
    ani_ref responseHeadersArray;
    ani_ref typeRef;
};

struct AniBlanklessLoadingParam {
    bool enable = false;
    int32_t duration = 0;
    int64_t expirationTime = 0;
    ani_ref callbackRef = nullptr;
};

class WebRegObj {
    WebRegObj() : m_regEnv(nullptr), m_regHandlerRef(nullptr) {}

    explicit WebRegObj(ani_env *env, const ani_ref &ref)
    {
        m_regEnv = env;
        m_regHandlerRef = ref;
        m_isMarked = false;
    }

    ~WebRegObj() = default;

    ani_env* m_regEnv;
    ani_ref m_regHandlerRef;
    bool m_isMarked;
};

class WebMessageExt {
public:
    explicit WebMessageExt(std::shared_ptr<NWebMessage> data, std::shared_ptr<NWebRomValue> value = nullptr)
        : data_(data), value_(value) {}
    ~WebMessageExt() = default;

    void SetType(int type) {}

    int ConvertNwebType2JsType(int type)
    {
        return 0;
    }

    int GetType()
    {
        return 0;
    }

    void SetString(std::string value) {}

    void SetNumber(double value) {}

    void SetBoolean(bool value) {}

    void SetArrayBuffer(std::vector<uint8_t>& value) {}

    void SetStringArray(std::vector<std::string> value) {}

    void SetDoubleArray(std::vector<double> value) {}

    void SetInt64Array(std::vector<int64_t> value) {}

    void SetBooleanArray(std::vector<bool> value) {}

    void SetError(std::string name, std::string message) {}

    std::shared_ptr<NWebMessage> GetData() const
    {
        return data_;
    }

    std::shared_ptr<NWebRomValue> GetValue() const
    {
        return value_;
    }

private:
    std::shared_ptr<NWebMessage> data_;
    std::shared_ptr<NWebRomValue> value_;
};

class WebPrintDocument {
public:
    explicit WebPrintDocument(void* webPrintdoc) {}
    ~WebPrintDocument() = default;

    void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::function<void(std::string, uint32_t)> writeResultCallback) {}

    void OnJobStateChanged(const std::string& jobId, uint32_t state) {}
};

// Mock WebviewController class: Replace real WebviewController in tests
// Class name and namespace are identical to real class, replacement achieved through conditional header include order
class WebviewController {
public:
    explicit WebviewController(const std::string& tag = "")
        : tag_(tag), isInitialized_(false), onActiveCalled_(false), onInactiveCalled_(false),
          webId_(-1), scrollX_(0), scrollY_(0)
    {
        if (!tag_.empty()) {
            webTagSet_.insert(tag_);
        }
    }

    explicit WebviewController(int32_t nwebId) : WebviewController("")
    {
        webId_ = nwebId;
    }

    ~WebviewController() = default;

    bool IsInit() const
    {
        return isInitialized_;
    }

    void OnActive()
    {
        onActiveCalled_ = true;
    }

    void OnInactive()
    {
        onInactiveCalled_ = true;
    }

    void SetWebId(int32_t nwebId)
    {
        webId_ = nwebId;
    }

    int32_t GetWebId() const
    {
        return webId_;
    }

    static WebviewController* FromID(int32_t nwebId)
    {
        return nullptr;
    }

    static std::string GenerateWebTag()
    {
        static int counter = 0;
        return "webTag_" + std::to_string(++counter);
    }

    ErrCode LoadUrl(std::string url)
    {
        return 0;
    }

    ErrCode LoadUrl(std::string url, std::map<std::string, std::string> httpHeaders)
    {
        return 0;
    }

    ErrCode LoadData(std::string data, std::string mimeType, std::string encoding,
                     std::string baseUrl, std::string historyUrl)
    {
        return 0;
    }

    ErrCode PostUrl(std::string& url, std::vector<char>& postData)
    {
        return 0;
    }

    bool GetRawFileUrl(const std::string& fileName, const std::string& bundleName, const std::string& moduleName,
        std::string& result) const
    {
        result = "";
        return false;
    }

    void ScrollTo(float x, float y)
    {
        // Mock implementation
    }

    void ScrollBy(float deltaX, float deltaY)
    {
        // Mock implementation
    }

    void ScrollToWithAnime(float x, float y, int32_t duration)
    {
        // Mock implementation
    }

    void ScrollByWithAnime(float deltaX, float deltaY, int32_t duration)
    {
        // Mock implementation
    }

    bool ScrollByWithResult(float deltaX, float deltaY)
    {
        return true;
    }

    void GetScrollOffset(float* offsetX, float* offsetY)
    {
        // Mock implementation
    }

    void GetPageOffset(float* offsetX, float* offsetY)
    {
        // Mock implementation
    }

    void SlideScroll(float vx, float vy)
    {
        // Mock implementation
    }

    void ScrollPageDown(bool bottom)
    {
        // Mock implementation
    }

    void ScrollPageUp(bool top)
    {
        // Mock implementation
    }

    bool GetScrollable() const
    {
        return false;
    }

    void SetScrollable(bool enable)
    {
        // Mock implementation
    }

    void SetScrollable(bool enable, int32_t scrollType)
    {
        // Mock implementation
    }

    void Forward()
    {
        // Mock implementation
    }

    void Backward()
    {
        // Mock implementation
    }

    bool AccessForward() const
    {
        return false;
    }

    bool AccessBackward() const
    {
        return false;
    }

    bool AccessStep(int32_t step) const
    {
        return false;
    }

    void ClearHistory()
    {
        // Mock implementation
    }

    ErrCode BackOrForward(int32_t step)
    {
        return 0;
    }

    void Refresh()
    {
        // Mock implementation
    }

    void ReloadIgnoreCache()
    {
        // Mock implementation
    }

    void Stop()
    {
        // Mock implementation
    }

    ErrCode ZoomIn()
    {
        return 0;
    }

    ErrCode ZoomOut()
    {
        return 0;
    }

    ErrCode Zoom(float factor)
    {
        return 0;
    }

    void RequestFocus()
    {
        // Mock implementation
    }

    void ClearClientAuthenticationCache()
    {
        // Mock implementation
    }

    void ClearSslCache()
    {
        // Mock implementation
    }

    int32_t GetProgress()
    {
        return 0;
    }

    int32_t GetPageHeight()
    {
        return 0;
    }

    int GetHitTest()
    {
        return 0;
    }

    std::shared_ptr<HitTestResult> GetLastHitTest()
    {
        return nullptr;
    }

    void SearchNext(bool forward)
    {
        // Mock implementation
    }

    void SearchAllAsync(const std::string& searchString)
    {
        // Mock implementation
    }

    void ClearMatches()
    {
        // Mock implementation
    }

    void RemoveCache(bool includeDiskFiles)
    {
        // Mock implementation
    }

    bool GetFavicon(
        const void** data, size_t& width, size_t& height, ImageColorType& colorType, ImageAlphaType& alphaType) const
    {
        return false;
    }

    std::shared_ptr<NWebHistoryList> GetHistoryList()
    {
        return nullptr;
    }

    std::string GetUrl()
    {
        return "";
    }

    std::string GetOriginalUrl()
    {
        return "";
    }

    std::string GetTitle()
    {
        return "";
    }

    std::string GetUserAgent()
    {
        return "";
    }

    std::string GetCustomUserAgent() const
    {
        return "";
    }

    std::string GetLastJavascriptProxyCallingFrameUrl()
    {
        return "";
    }

    void RegisterJavaScriptProxy(RegisterJavaScriptProxyParam& param)
    {
        // Mock implementation
    }

    void RegisterJavaScriptProxy(AniRegisterJavaScriptProxyParam& param)
    {
        // Mock implementation
    }

    ErrCode DeleteJavaScriptRegister(const std::string& objName, const std::vector<std::string>& methodList)
    {
        return 0;
    }

    void CloseAllMediaPresentations()
    {
        // Mock implementation
    }

    void StopAllMedia()
    {
        // Mock implementation
    }

    void ResumeAllMedia()
    {
        // Mock implementation
    }

    void PauseAllMedia()
    {
        // Mock implementation
    }

    int GetMediaPlaybackState()
    {
        return 0;
    }

    ErrCode StartCamera()
    {
        return 0;
    }

    ErrCode StopCamera()
    {
        return 0;
    }

    ErrCode CloseCamera()
    {
        return 0;
    }

    ErrCode ResumeMicrophone()
    {
        return 0;
    }

    ErrCode StopMicrophone()
    {
        return 0;
    }

    ErrCode PauseMicrophone()
    {
        return 0;
    }

    void EnableSafeBrowsing(bool enable)
    {
        // Mock implementation
    }

    bool IsSafeBrowsingEnabled() const
    {
        return false;
    }

    bool IsIncognitoMode() const
    {
        return false;
    }

    void EnableIntelligentTrackingPrevention(bool enable)
    {
        // Mock implementation
    }

    bool IsIntelligentTrackingPreventionEnabled() const
    {
        return false;
    }

    bool IsAdsBlockEnabledForCurPage()
    {
        return false;
    }

    void EnableAdsBlock(bool enable)
    {
        // Mock implementation
    }

    bool IsAdsBlockEnabled() const
    {
        return false;
    }

    int GetSecurityLevel(ani_env* env)
    {
        return 0;
    }

    void SetPrintBackground(bool enable)
    {
        // Mock implementation
    }

    bool GetPrintBackground() const
    {
        return false;
    }

    void* CreateWebPrintDocumentAdapter(const std::string& jobName)
    {
        return nullptr;
    }

    void InnerSetHapPath(const std::string& hapPath)
    {
        // Mock implementation
    }

    void InnerCompleteWindowNew(int32_t parentNwebId)
    {
        // Mock implementation
    }

    ErrCode SetCustomUserAgent(const std::string& userAgent)
    {
        return 0;
    }

    ErrCode SetAudioMuted(bool muted)
    {
        return 0;
    }

    ErrCode PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders)
    {
        return 0;
    }

    ErrCode PrefetchPage(std::string& url, std::map<std::string, std::string> additionalHttpHeaders,
                         std::shared_ptr<NWebPrefetchOptions> options)
    {
        return 0;
    }

    bool GetCertChainDerData(std::vector<std::string>& certChainDerData) const
    {
        return false;
    }

    ErrCode AvoidVisibleViewportBottom(int32_t avoidHeight)
    {
        return 0;
    }

    bool ParseUrl(napi_env env, napi_value urlObj, std::string& result) const
    {
        result = "";
        return false;
    }

    bool ParseScriptContent(napi_env env, napi_value value, std::string& script)
    {
        script = "";
        return false;
    }

    void PrecompileJavaScriptPromise(napi_env env, napi_value script, napi_deferred deferred)
    {
        // Mock implementation
    }

    int32_t PrecompileJavaScript(const std::string& url, const std::string& script, int32_t type)
    {
        return 0;
    }

    bool CheckURL(std::string& url) const
    {
        return true;
    }

    ParseURLResult ParseURLList(napi_env env, napi_value value, std::vector<std::string>& urlList)
    {
        return ParseURLResult::OK;
    }

    void InjectOfflineResource(const std::vector<std::string>& urlList, const std::string& baseUrl)
    {
        // Mock implementation
    }

    void InjectOfflineResource(const std::vector<std::string>& urlList, const std::vector<uint8_t>& resource,
                               const std::map<std::string, std::string>& response_headers, const uint32_t type)
    {
        // Mock implementation
    }

    ErrCode SetUrlTrustList(const std::string& urlTrustList, std::string& detailErrMsg)
    {
        return 0;
    }

    ErrCode SetUrlTrustList(const std::string& urlTrustList, bool allowOpaqueOrigin, bool supportWildcard,
        std::string& detailErrMsg)
    {
        return 0;
    }

    bool SetWebSchemeHandler(const char* scheme, WebSchemeHandler* handler) const
    {
        return false;
    }

    static bool SetWebServiveWorkerSchemeHandler(const char* scheme, WebSchemeHandler* handler)
    {
        return false;
    }

    int32_t ClearWebSchemeHandler()
    {
        return 0;
    }

    static int32_t ClearWebServiceWorkerSchemeHandler()
    {
        return 0;
    }

    bool RestoreWebState(const std::vector<uint8_t>& state) const
    {
        return false;
    }

    std::vector<uint8_t> SerializeWebState()
    {
        // Return mock data to avoid length=0 case
        return std::vector<uint8_t>{0x01, 0x02, 0x03, 0x04};
    }

    void StoreWebArchiveCallback(const std::string& baseName, bool autoName, napi_env env, napi_ref jsCallback)
    {
        // Mock implementation
    }

    void StoreWebArchivePromise(const std::string& baseName, bool autoName, napi_env env, napi_deferred deferred)
    {
        // Mock implementation
    }

    ErrCode PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl)
    {
        return 0;
    }

    std::vector<std::string> CreateWebMessagePorts()
    {
        return std::vector<std::string>();
    }

    bool HasImage(std::shared_ptr<NWebBoolValueCallback> callback)
    {
        return false;
    }

    ErrCode HasImagesCallback(ani_vm* vm, ani_ref jsCallback)
    {
        return 0;
    }

    ErrCode HasImagesPromise(ani_vm* vm, ani_resolver deferred)
    {
        return 0;
    }

    void PutNetworkAvailable(bool available)
    {
        // Mock implementation
    }

    bool TerminateRenderProcess() const
    {
        return !g_terminateRenderProcessShouldFail;
    }

    void CreatePDFExt(std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
                     std::shared_ptr<NWebArrayBufferValueCallback> callbackImpl)
    {
        // Mock implementation
    }

    std::string GetSurfaceId()
    {
        return "";
    }

    void OnCreateNativeMediaPlayer(ani_vm* vm, ani_fn_object callback)
    {
        // Mock implementation
    }

    ErrCode WebPageSnapshot(const char* id, PixelUnit type, int32_t width, int32_t height,
        const WebSnapshotCallback callback)
    {
        return 0;
    }

    void SetPathAllowingUniversalAccess(const std::vector<std::string>& pathList, std::string& errorPath)
    {
        // Mock implementation
    }

    void UpdateInstanceId(int32_t newId)
    {
        // Mock implementation
    }

    std::shared_ptr<HitTestResult> GetHitTestValue()
    {
        return nullptr;
    }

    int32_t GetNWebId()
    {
        return webId_;
    }

    void SetNWebJavaScriptResultCallBack()
    {
        // Mock implementation
    }

    ErrCode SetErrorPageEnabled(bool enable)
    {
        return 0;
    }

    bool GetErrorPageEnabled()
    {
        return false;
    }

    void SetSoftKeyboardBehaviorMode(int32_t mode)
    {
        // Mock implementation
    }

    int32_t GetAttachState()
    {
        return 0;
    }

    void SetWebDetach(int32_t nwebId)
    {
        // Mock implementation
    }

    void SetUserAgentMetadata(const std::string& userAgent, std::shared_ptr<NWebUserAgentMetadata> metaData)
    {
        // Mock implementation
    }

    std::shared_ptr<NWebUserAgentMetadata> GetUserAgentMetadata(const std::string& userAgent)
    {
        return nullptr;
    }

    std::shared_ptr<CacheOptions> ParseCacheOptions(napi_env env, napi_value value)
    {
        return nullptr;
    }

    void PrecompileJavaScriptPromise(napi_env env, napi_deferred deferred, const std::string& url,
        const std::string& script, std::shared_ptr<CacheOptions> cacheOptions)
    {
        // Mock implementation
    }

    int32_t PrecompileJavaScript(const std::string& url, const std::string& script,
        std::shared_ptr<OHOS::NWeb::CacheOptions> cacheOptions,
        std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callbackImpl)
    {
        return 0;
    }

    bool ParseResponseHeaders(napi_env env, napi_value value, std::map<std::string,
        std::string>& responseHeaders) const
    {
        return false;
    }

    std::vector<uint8_t> ParseUint8Array(napi_env env, napi_value value)
    {
        return std::vector<uint8_t>();
    }

    bool ParseJsLengthToInt(napi_env env, napi_value jsLength, PixelUnit& type, int32_t& result) const
    {
        return false;
    }

    void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive)
    {
        // Mock implementation
    }

    int32_t GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime)
    {
        return 0;
    }

    int32_t SetBlanklessLoadingWithKey(const std::string& key, bool isStart)
    {
        return 0;
    }

    int32_t SetBlanklessLoadingParams(ani_env* env, const std::string& key, AniBlanklessLoadingParam& param)
    {
        return 0;
    }

    void RegisterStateChangeCallback(ani_env* env, const std::string& type, ani_object handler)
    {
        // Mock implementation
    }

    void DeleteRegisterObj(ani_env* env, std::vector<WebRegObj>& vecRegObjs, ani_object& handler)
    {
        // Mock implementation
    }

    void DeleteAllRegisterObj(ani_env* env, std::vector<WebRegObj>& vecRegObjs)
    {
        // Mock implementation
    }

    void UnregisterStateChangeCallback(ani_env* env, const std::string& type, ani_object handler)
    {
        // Mock implementation
    }

    static void WaitForAttachedDeal(ani_env* env, void* data)
    {
        // Mock implementation
    }

    static void WaitForAttachedFinish(ani_env* env, void* status, void* data)
    {
        // Mock implementation
    }

    void WaitForAttachedInternal(ani_env* env, ani_int timeout, ani_resolver resolver)
    {
        // Mock implementation
    }

    void TriggerStateChangeCallback(const std::string& type)
    {
        // Mock implementation
    }

    void SetInitialized(bool initialized)
    {
        isInitialized_ = initialized;
    }

    bool WasOnActiveCalled() const
    {
        return onActiveCalled_;
    }

    bool WasOnInactiveCalled() const
    {
        return onInactiveCalled_;
    }

    void Reset()
    {
        isInitialized_ = false;
        onActiveCalled_ = false;
        onInactiveCalled_ = false;
        scrollX_ = 0;
        scrollY_ = 0;
    }

    const std::string& GetTag() const
    {
        return tag_;
    }

    static std::set<std::string> webTagSet_;
    static bool existNweb_;
    static bool webDebuggingAccess_;
    static int32_t webDebuggingPort_;
    static bool g_terminateRenderProcessShouldFail;

private:
    std::string tag_;
    bool isInitialized_;
    bool onActiveCalled_;
    bool onInactiveCalled_;
    int32_t webId_;
    float scrollX_;
    float scrollY_;
};

std::set<std::string> WebviewController::webTagSet_;
bool WebviewController::existNweb_ = false;
bool WebviewController::webDebuggingAccess_ = false;
int32_t WebviewController::webDebuggingPort_ = 0;
bool WebviewController::g_terminateRenderProcessShouldFail = false;

} // namespace NWeb
} // namespace OHOS

#endif // TEST_MOCK_WEBVIEW_CONTROLLER_H
