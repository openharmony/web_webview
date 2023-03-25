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

#include <memory>
#include <string>
#include <unordered_map>

#include "context/application_context.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_web_message.h"
#include "web_errors.h"
#include "webview_javascript_result_callback.h"

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

class WebviewController {
public:
    explicit WebviewController() = default;
    explicit WebviewController(int32_t nwebId);
    ~WebviewController() = default;

    bool IsInit();

    void SetWebId(int32_t nwebId);

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

    std::string GetTitle();

    int32_t GetPageHeight();

    ErrCode BackOrForward(int32_t step);

    void StoreWebArchiveCallback(const std::string &baseName, bool autoName, napi_env env, napi_ref jsCallback);

    void StoreWebArchivePromise(const std::string &baseName, bool autoName, napi_env env, napi_deferred deferred);

    ErrCode CreateWebMessagePorts(std::vector<std::string>& ports);

    ErrCode PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUrl);

    HitTestResult GetHitTestValue();

    void RequestFocus();

    ErrCode ParseUrl(napi_env env, napi_value urlObj, std::string& result);

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

    void SetNWebJavaScriptResultCallBack();

    void RegisterJavaScriptProxy(napi_env env, napi_value obj,
        const std::string& objName, const std::vector<std::string>& methodList);

    ErrCode DeleteJavaScriptRegister(const std::string& objName,
        const std::vector<std::string>& methodList);

    void RunJavaScriptCallback(const std::string &script, napi_env env, napi_ref jsCallback);

    void RunJavaScriptPromise(const std::string &script, napi_env env, napi_deferred deferred);

    std::string GetUrl();

    std::string GetOriginalUrl();

    void PutNetworkAvailable(bool available);

    bool HasImage(std::shared_ptr<NWebValueCallback<bool>> callback);

    ErrCode HasImagesCallback(napi_env env, napi_ref jsCallback);

    ErrCode HasImagesPromise(napi_env env, napi_deferred deferred);

    void RemoveCache(bool include_disk_files);

    std::shared_ptr<NWebHistoryList> GetHistoryList();

    bool GetFavicon(
        const void **data, size_t &width, size_t &height, ImageColorType &colorType, ImageAlphaType &alphaType);

    WebState SerializeWebState();

    bool RestoreWebState(WebState state);

    void ScrollPageDown(bool bottom);

    void ScrollPageUp(bool top);

    void ScrollTo(float x, float y);

    void ScrollBy(float deltaX, float deltaY);

    void SlideScroll(float vx, float vy);

private:
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceMgr(std::string bundleName, std::string moduleName);

    int ConverToWebHitTestType(int hitType);

public:
    static std::string customeSchemeCmdLine_;
    static bool existNweb_;
    static bool webDebuggingAccess_;

private:
    std::weak_ptr<OHOS::NWeb::NWeb> nweb_;
    std::shared_ptr<WebviewJavaScriptResultCallBack> javaScriptResultCb_ = nullptr;
};

class WebMessagePort {
public:
    WebMessagePort(int32_t nwebId, std::string& port);

    ~WebMessagePort() = default;

    ErrCode ClosePort();

    ErrCode PostPortMessage(std::shared_ptr<NWebMessage> data);

    ErrCode SetPortMessageCallback(std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback);

    std::string GetPortHandle() const;

private:
    std::weak_ptr<OHOS::NWeb::NWeb> nweb_;
    std::string portHandle_;
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
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEBVIEW_CONTROLLER_H
