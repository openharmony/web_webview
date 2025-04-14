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

#include "webview_ffi.h"

#include <arpa/inet.h>
#include <regex>

#include "application_context.h"
#include "cj_lambda.h"
#include "geolocation_permission.h"
#include "nweb_cache_options_impl.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "parameters.h"
#include "pixel_map.h"
#include "pixel_map_impl.h"
#include "web_cookie_manager.h"
#include "web_data_base.h"
#include "web_download.pb.h"
#include "web_download_delegate_impl.h"
#include "web_download_item_impl.h"
#include "web_download_manager_impl.h"
#include "web_errors.h"
#include "web_scheme_handler_request.h"
#include "web_scheme_handler_response_impl.h"
#include "web_storage.h"
#include "webview_controller_impl.h"
#include "webview_function.h"
#include "webview_javascript_execute_callback.h"
#include "webview_log.h"
#include "webview_utils.h"


using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {

constexpr uint32_t SOCKET_MAXIMUM = 6;
constexpr uint32_t URL_MAXIMUM = 2048;
constexpr int INTEGER_TWO = 2;
constexpr int INTEGER_THREE = 3;
constexpr char URL_REGEXPR[] = "^http(s)?:\\/\\/.+";
constexpr size_t MAX_URL_TRUST_LIST_STR_LEN = 10 * 1024 * 1024;
std::atomic<bool> g_inWebPageSnapshot {false};

bool CheckUrl(std::string url)
{
    if (url.size() > URL_MAXIMUM) {
        WEBVIEWLOGE("The URL exceeds the maximum length of %{public}d.", URL_MAXIMUM);
        return false;
    }

    if (!regex_match(url, std::regex("^http(s)?:\\/\\/.+", std::regex_constants::icase))) {
        WEBVIEWLOGE("The Parse URL error. URL: %{private}s", url.c_str());
        return false;
    }

    return true;
}

NWeb::WebSnapshotCallback CreateWebPageSnapshotResultCallback(bool check, int32_t inputWidth, int32_t inputHeight,
    int32_t inputSizeType, const std::function<void(RetDataCSnapshotResult)>& callbackRef)
{
    return [check, inputWidth, inputHeight, inputSizeType, callbackRef](
            const char* returnId, bool returnStatus, float radio, void* returnData,
            int returnWidth, int returnHeight) {
            RetDataCSnapshotResult ret = { .code = NWebError::INIT_ERROR, .data = { .id = nullptr, .imageId = 0,
                .status = true, .width = 0, .height = 0, .widthType = 1, .heightType = 1 } };
            OHOS::Media::InitializationOptions opt;
            opt.size.width = static_cast<int32_t>(returnWidth);
            opt.size.height = static_cast<int32_t>(returnHeight);
            opt.pixelFormat = OHOS::Media::PixelFormat::RGBA_8888;
            opt.alphaType = OHOS::Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
            opt.editable = true;
            std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMapImpl::CreatePixelMap(opt);
            if (pixelMap != nullptr) {
                uint64_t stride = static_cast<uint64_t>(returnWidth) << 2;
                uint64_t bufferSize = stride * static_cast<uint64_t>(returnHeight);
                pixelMap->WritePixels(static_cast<const uint8_t*>(returnData), bufferSize);
            } else {
                WEBVIEWLOGE("WebPageSnapshot create pixel map error");
            }
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
            ret.code = NWebError::NO_ERROR;
            ret.data.status = check;
            ret.data.width = returnJsWidth;
            ret.data.height = returnJsHeight;
            ret.data.widthType = inputSizeType;
            ret.data.heightType = inputSizeType;
            ret.data.id = MallocCString(returnId);
            auto nativeImage = FFIData::Create<Media::PixelMapImpl>(move(pixelMap));
            if (nativeImage != nullptr) {
                ret.data.imageId = nativeImage->GetID();
            }
            callbackRef(ret);
            g_inWebPageSnapshot = false;
        };
}

extern "C" {
    int32_t FfiOHOSWebviewOnce(char* type, void (*callbackRef)(void))
    {
        return FfiOnce(type, callbackRef);
    }

    int64_t FfiOHOSWebviewCtlConstructor()
    {
        auto nativeWebviewCtl = FFIData::Create<WebviewControllerImpl>();
        if (nativeWebviewCtl == nullptr) {
            WEBVIEWLOGE("new webview controller failed");
            return -1;
        }
        WebviewControllerImpl::webDebuggingAccess_ = OHOS::system::GetBoolParameter("web.debug.devtools", false);
        return nativeWebviewCtl->GetID();
    }

    int64_t FfiOHOSWebviewCtlConstructorWithWebTag(char *cWebTag)
    {
        std::string webTag = cWebTag;
        auto nativeWebviewCtl = FFIData::Create<WebviewControllerImpl>(webTag);
        if (nativeWebviewCtl == nullptr) {
            WEBVIEWLOGE("new webview controller failed");
            return -1;
        }
        WebviewControllerImpl::webDebuggingAccess_ = OHOS::system::GetBoolParameter("web.debug.devtools", false);
        return nativeWebviewCtl->GetID();
    }

    void FfiOHOSWebviewCtlInitializeWebEngine()
    {
        std::shared_ptr<AbilityRuntime::ApplicationContext> ctx =
            AbilityRuntime::ApplicationContext::GetApplicationContext();
        if (ctx == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebviewCtlInitializeWebEngine Failed to init web engine due to ctx is null.");
            return;
        }
        const std::string& bundle_path = ctx->GetBundleCodeDir();
        NWebHelper::Instance().SetBundlePath(bundle_path);
        if (!NWebHelper::Instance().InitAndRun(true)) {
            WEBVIEWLOGI("FfiOHOSWebviewCtlInitializeWebEngine Failed to init web engine due to NWebHelper failure.");
        }
        WEBVIEWLOGI("FfiOHOSWebviewCtlInitializeWebEngine NWebHelper initialized, \
            init web engine done, bundle_path: %{public}s", bundle_path.c_str());
        return;
    }

    void FfiOHOSWebviewCtlSetHttpDns(int32_t secureDnsMode, char* secureDnsConfig)
    {
        std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
        config->SetMode(secureDnsMode);
        config->SetConfig(secureDnsConfig);
        WEBVIEWLOGI("set http dns mode:%{public}d doh_config:%{public}s", secureDnsMode, secureDnsConfig);
        NWebHelper::Instance().SetHttpDns(config);
    }

    void FfiOHOSWebviewCtlSetWebDebuggingAccess(bool webDebuggingAccess)
    {
        if (OHOS::system::GetBoolParameter("web.debug.devtools", false)) {
            return;
        }

        WebviewControllerImpl::webDebuggingAccess_ = webDebuggingAccess;
        return;
    }

    int32_t FfiOHOSWebviewCtlLoadUrl(int64_t id, char *url)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string webSrc = url;

        return nativeWebviewCtl->LoadUrl(webSrc);
    }

    int32_t FfiOHOSWebviewCtlLoadUrlWithHeaders(int64_t id, char *url, ArrWebHeader headers)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }

        std::map<std::string, std::string> httpHeaders;
        uint32_t arrayLength = static_cast<uint32_t>(headers.size);
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string key = headers.head[i].headerKey;
            std::string value = headers.head[i].headerValue;
            httpHeaders[key] = value;
        }

        return nativeWebviewCtl->LoadUrl(url, httpHeaders);
    }

    int32_t FfiOHOSWebviewCtlLoadData(int64_t id, LoadDatas loadDatas)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string data = loadDatas.cData;
        std::string mimeType = loadDatas.cMimeType;
        std::string encoding = loadDatas.cEncoding;
        std::string baseUrl = loadDatas.cBaseUrl;
        std::string historyUrl = loadDatas.cHistoryUrl;
        return nativeWebviewCtl->LoadData(data, mimeType, encoding, baseUrl, historyUrl);
    }

    int32_t FfiOHOSWebviewCtlPreFetchPage(int64_t id, char *url)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string webSrc = url;
        if (webSrc.size() > URL_MAXIMUM) {
            WEBVIEWLOGE("The URL exceeds the maximum length of %{public}d", URL_MAXIMUM);
            return NWebError::PARAM_CHECK_ERROR;
        }

        if (!regex_match(webSrc, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            WEBVIEWLOGE("ParsePrepareUrl error");
            return NWebError::PARAM_CHECK_ERROR;
        }
        int32_t ret = nativeWebviewCtl->PreFetchPage(webSrc);
        if (ret != NWebError::NO_ERROR) {
            if (ret == NWebError::NWEB_ERROR) {
                return ret;
            }
        }
        return ret;
    }

    int32_t FfiOHOSWebviewCtlPreFetchPageWithHeaders(int64_t id, char *url, ArrWebHeader headers)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string webSrc = url;
        if (webSrc.size() > URL_MAXIMUM) {
            WEBVIEWLOGE("The URL exceeds the maximum length of %{public}d", URL_MAXIMUM);
            return NWebError::PARAM_CHECK_ERROR;
        }

        if (!regex_match(webSrc, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            WEBVIEWLOGE("ParsePrepareUrl error");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::map<std::string, std::string> httpHeaders;
        uint32_t arrayLength = static_cast<uint32_t>(headers.size);
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string key = headers.head[i].headerKey;
            std::string value = headers.head[i].headerValue;
            httpHeaders[key] = value;
        }

        int32_t ret = nativeWebviewCtl->PreFetchPage(webSrc, httpHeaders);
        if (ret != NWebError::NO_ERROR) {
            if (ret == NWebError::NWEB_ERROR) {
                WEBVIEWLOGE("preFetchPage failed.");
                return ret;
            }
        }
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSetAudioMuted(int64_t id, bool mute)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t ret = nativeWebviewCtl->SetAudioMuted(mute);
        if (ret != NWebError::NO_ERROR) {
            if (ret == NWebError::NWEB_ERROR) {
                WEBVIEWLOGE("SetAudioMuted failed, error code: %{public}d", ret);
                return ret;
            }
        }
        WEBVIEWLOGI("SetAudioMuted: %{public}s", (mute ? "true" : "false"));
        return ret;
    }

    int32_t FfiOHOSWebviewCtlRefresh(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->Refresh();
        return NWebError::NO_ERROR;
    }

    char *FfiOHOSWebviewCtlGetUserAgent(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return nullptr;
        }
        std::string userAgent = nativeWebviewCtl->GetUserAgent();
        *errCode = NWebError::NO_ERROR;
        return MallocCString(userAgent);
    }

    int32_t FfiOHOSWebviewCtlGetWebId(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int32_t webId = -1;
        webId = nativeWebviewCtl->GetWebId();
        *errCode = NWebError::NO_ERROR;
        return webId;
    }

    bool FfiOHOSWebviewCtlAccessForward(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool access = nativeWebviewCtl->AccessForward();
        *errCode = NWebError::NO_ERROR;
        return access;
    }

    bool FfiOHOSWebviewCtlAccessBackward(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool access = nativeWebviewCtl->AccessBackward();
        *errCode = NWebError::NO_ERROR;
        return access;
    }

    int32_t FfiOHOSWebviewCtlSetCustomUserAgent(int64_t id, char *cUserAgent)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        std::string userAgent = cUserAgent;
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        return nativeWebviewCtl->SetCustomUserAgent(userAgent);
    }

    RetDataCString FfiOHOSWebviewCtlGetCustomUserAgent(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return ret;
        }
        std::string userAgent = "";
        userAgent = nativeWebviewCtl->GetCustomUserAgent();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(userAgent);
        return ret;
    }

    int32_t FfiOHOSWebviewCtlRunJavaScript(int64_t id, char* cScript,
        void (*callbackRef)(RetDataCString infoRef))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        std::string script = std::string(cScript);
        auto onChange = [lambda = CJLambda::Create(callbackRef)]
            (RetDataCString infoRef) -> void { lambda(infoRef); };
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->RunJavaScript(script, onChange);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlRunJavaScriptExt(int64_t id, char* cScript,
        void (*callbackRef)(RetDataI64))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        std::string script = std::string(cScript);
        auto onChange = CJLambda::Create(callbackRef);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->RunJavaScriptExt(script, onChange);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlRunJavaScriptExtArr(int64_t id, CArrUI8 cScript,
        void (*callbackRef)(RetDataI64))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        std::string script = std::string(reinterpret_cast<char*>(cScript.head), cScript.size);
        auto onChange = CJLambda::Create(callbackRef);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->RunJavaScriptExt(script, onChange);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlRegisterJavaScriptProxy(int64_t id,
        CArrI64 cFuncIds,  const char* cName, CArrString cMethodList)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string objName = std::string(cName);
        std::vector<std::string> methodList;
        for (int64_t i = 0; i < cMethodList.size; i++) {
            methodList.push_back(std::string(cMethodList.head[i]));
        }
        std::vector<std::function<char*(const char*)>> cjFuncs;
        for (int64_t i = 0; i < cFuncIds.size; i++) {
            auto cFunc = reinterpret_cast<char*(*)(const char*)>(cFuncIds.head[i]);
            auto onChange = [lambda = CJLambda::Create(cFunc)]
                (const char* infoRef) -> char* { return lambda(infoRef); };
            cjFuncs.push_back(onChange);
        }
        nativeWebviewCtl->SetNWebJavaScriptResultCallBack();
        nativeWebviewCtl->RegisterJavaScriptProxy(cjFuncs, objName, methodList);
        return NWebError::NO_ERROR;
    }

    RetDataCString FfiOHOSWebviewCtlGetUrl(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return ret;
        }
        std::string url = "";
        url = nativeWebviewCtl->GetUrl();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(url);
        return ret;
    }

    RetDataCString FfiOHOSWebviewCtlGetOriginalUrl(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return ret;
        }
        std::string url = "";
        url = nativeWebviewCtl->GetOriginalUrl();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(url);
        return ret;
    }

    int32_t FfiOHOSWebviewCtlPageUp(int64_t id, bool top)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->ScrollPageUp(top);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlPageDown(int64_t id, bool bottom)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->ScrollPageDown(bottom);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlScrollTo(int64_t id, float x, float y)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->ScrollTo(x, y);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlScrollBy(int64_t id, float deltaX, float deltaY)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->ScrollBy(deltaX, deltaY);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlScrollToWithAnime(int64_t id, float x, float y, int32_t duration)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        if (duration > 0) {
            nativeWebviewCtl->ScrollToWithAnime(x, y, duration);
        } else {
            nativeWebviewCtl->ScrollTo(x, y);
        }
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlScrollByWithAnime(int64_t id, float deltaX, float deltaY, int32_t duration)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        if (duration > 0) {
            nativeWebviewCtl->ScrollByWithAnime(deltaX, deltaY, duration);
        } else {
            nativeWebviewCtl->ScrollBy(deltaX, deltaY);
        }
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlForward(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->Forward();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlBackward(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->Backward();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlBackOrForward(int64_t id, int32_t step)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        return nativeWebviewCtl->BackOrForward(step);
    }

    int32_t FfiOHOSWebviewCtlGetPageHeight(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int32_t pageHeight = nativeWebviewCtl->GetPageHeight();
        *errCode = NWebError::NO_ERROR;
        return pageHeight;
    }

    RetDataCString FfiOHOSWebviewCtlGetTitle(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return ret;
        }
        std::string title = "";
        title = nativeWebviewCtl->GetTitle();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(title);
        return ret;
    }

    int32_t FfiOHOSWebviewCtlZoom(int64_t id, float factor)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t ret = nativeWebviewCtl->Zoom(factor);
        return ret;
    }

    int32_t FfiOHOSWebviewCtlZoomIn(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t ret = nativeWebviewCtl->ZoomIn();
        return ret;
    }

    int32_t FfiOHOSWebviewCtlZoomOut(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t ret = nativeWebviewCtl->ZoomOut();
        return ret;
    }

    int32_t FfiOHOSWebviewCtlRequestFocus(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t ret = nativeWebviewCtl->RequestFocus();
        return ret;
    }

    int32_t FfiOHOSWebviewCtlClearHistory(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->ClearHistory();
        return NWebError::NO_ERROR;
    }

    bool FfiOHOSWebviewCtlAccessStep(int64_t id, int32_t *errCode, int32_t step)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool access = nativeWebviewCtl->AccessStep(step);
        *errCode = NWebError::NO_ERROR;
        return access;
    }

    int32_t FfiOHOSWebviewCtlOnActive(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->OnActive();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlOnInactive(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->OnInactive();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlGetHitTest(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int32_t type = nativeWebviewCtl->GetHitTest();
        *errCode = NWebError::NO_ERROR;
        return type;
    }

    RetDataCString FfiOHOSWebviewCtlGetHitTestValue(int64_t id, int32_t *errCode)
    {
        RetDataCString ret = { .code = NWeb::HitTestResult::UNKNOWN_TYPE, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        std::shared_ptr<NWeb::HitTestResult> nwebResult = nativeWebviewCtl->GetHitTestValue();
        if (nwebResult == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        *errCode = NWebError::NO_ERROR;
        ret.code = nwebResult->GetType();
        ret.data = MallocCString(nwebResult->GetExtra());
        return ret;
    }

    int32_t FfiOHOSWebviewCtlStoreWebArchive(int64_t id, const char* cBaseName,
        bool autoName, void (*callbackRef)(RetDataCString infoRef))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        std::string baseName = std::string(cBaseName);
        auto onChange = [lambda = CJLambda::Create(callbackRef)]
            (RetDataCString infoRef) -> void { lambda(infoRef); };
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->StoreWebArchiveCallback(baseName, autoName, onChange);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlEnableSafeBrowsing(int64_t id, bool enable)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->EnableSafeBrowsing(enable);
        return NWebError::NO_ERROR;
    }

    bool FfiOHOSWebviewCtlIsSafeBrowsingEnabled(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool isSafeBrowsingEnabled = nativeWebviewCtl->IsSafeBrowsingEnabled();
        *errCode = NWebError::NO_ERROR;
        return isSafeBrowsingEnabled;
    }

    int32_t FfiOHOSWebviewCtlGetSecurityLevel(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int32_t securityLevel = nativeWebviewCtl->GetSecurityLevel();
        *errCode = NWebError::NO_ERROR;
        return securityLevel;
    }

    bool FfiOHOSWebviewCtlIsIncognitoMode(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool incognitoMode = nativeWebviewCtl->IsIncognitoMode();
        *errCode = NWebError::NO_ERROR;
        return incognitoMode;
    }

    int32_t FfiOHOSWebviewCtlRemoveCache(int64_t id, bool clearRom)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->RemoveCache(clearRom);
        return NWebError::NO_ERROR;
    }

    int64_t FfiOHOSWebviewCtlGetBackForwardEntries(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }

        std::shared_ptr<NWebHistoryList> list = nativeWebviewCtl->GetHistoryList();
        if (!list) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }

        auto nativeWebHistoryList = FFIData::Create<WebHistoryListImpl>(list);
        if (nativeWebHistoryList == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            WEBVIEWLOGE("new WebHistoryList failed");
            return -1;
        }
        *errCode = NWebError::NO_ERROR;
        return nativeWebHistoryList->GetID();
    }

    int32_t FfiOHOSWebviewCtlStop(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->Stop();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlPostUrl(int64_t id, char *url, CArrUI8 buffer)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string sUrl = url;
        std::vector<char> postData(buffer.head, buffer.head + buffer.size);
        return nativeWebviewCtl->PostUrl(sUrl, postData);
    }

    int32_t FfiOHOSWebviewCtlSetDownloadDelegate(int64_t id, int64_t delegateId)
    {
        NWebHelper::Instance().LoadNWebSDK();
        auto delegate = FFIData::GetData<WebDownloadDelegateImpl>(delegateId);
        if (!delegate) {
            WEBVIEWLOGE("[DOWNLOAD] webDownloadDelegate is null");
            return NWebError::INIT_ERROR;
        }
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t nwebId = nativeWebviewCtl->GetWebId();
        WebDownloadManagerImpl::AddDownloadDelegateForWeb(nwebId, delegate);
        return NWebError::NO_ERROR;
    }

    bool ParsePrepareUrl(std::string& url)
    {
        if (url.size() > URL_MAXIMUM) {
            WEBVIEWLOGE("The URL exceeds the maximum length of %{public}d", URL_MAXIMUM);
            return false;
        }
        if (!regex_match(url, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            WEBVIEWLOGE("ParsePrepareUrl error");
            return false;
        }
        return true;
    }

    int32_t FfiOHOSWebviewCtlStartDownload(int64_t id, char *url)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::string webSrc = url;
        if (!ParsePrepareUrl(webSrc)) {
            return NWebError::INVALID_URL;
        }
        int32_t nwebId = nativeWebviewCtl->GetWebId();
        NWebHelper::Instance().LoadNWebSDK();
        WebDownloader_StartDownload(nwebId, webSrc.c_str());
        return NWebError::NO_ERROR;
    }

    CArrI64 FfiOHOSWebviewCtlCreateWebMessagePorts(int64_t id, bool isExtentionType, int32_t *errCode)
    {
        CArrI64 messagePorts = {.head = nullptr, .size = 0};
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return messagePorts;
        }

        int32_t nwebId = nativeWebviewCtl->GetWebId();
        std::vector<std::string> ports = nativeWebviewCtl->CreateWebMessagePorts();
        if (ports.size() != INTEGER_TWO) {
            WEBVIEWLOGE("create web message port failed");
            *errCode = NWebError::CAN_NOT_POST_MESSAGE;
            return messagePorts;
        }
        auto arr = static_cast<int64_t*>(malloc(sizeof(int64_t) * INTEGER_TWO));
        if (!arr) {
            WEBVIEWLOGE("FfiOHOSWebviewCtlCreateWebMessagePorts failed to malloc arr.");
            *errCode = NWebError::NEW_OOM;
            return messagePorts;
        }
        for (uint32_t i = 0; i < INTEGER_TWO; i++) {
            auto nativeWebMessagePort = FFIData::Create<WebMessagePortImpl>(nwebId, ports[i], isExtentionType);
            if (nativeWebMessagePort == nullptr) {
                *errCode = NWebError::CAN_NOT_POST_MESSAGE;
                WEBVIEWLOGE("new nativeWebMessagePort failed");
                free(arr);
                return messagePorts;
            }
            arr[i] = nativeWebMessagePort->GetID();
        }
        *errCode = NWebError::NO_ERROR;
        messagePorts.head = arr;
        messagePorts.size = INTEGER_TWO;
        return messagePorts;
    }

    int32_t GetSendPorts(CArrI64 ports, std::vector<std::string>& sendPorts)
    {
        int64_t arrayLen = ports.size;
        if (arrayLen == 0) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        int64_t* portsId = ports.head;
        if (!portsId) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        for (int64_t i = 0; i < arrayLen; i++) {
            WebMessagePortImpl *msgPort = FFIData::GetData<WebMessagePortImpl>(portsId[i]);
            if ((!msgPort)) {
                return NWebError::PARAM_CHECK_ERROR;
            }
            std::string portHandle = msgPort->GetPortHandle();
            sendPorts.emplace_back(portHandle);
        }
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlPostMessage(int64_t id, char* name, CArrI64 ports, char* uri)
    {
        WEBVIEWLOGD("post message port");
        std::string portName = std::string(name);
        std::vector<std::string> sendPorts;
        int32_t ret = GetSendPorts(ports, sendPorts);
        if (ret != NWebError::NO_ERROR) {
            WEBVIEWLOGE("post port to html failed, getSendPorts fail");
            return ret;
        }
        std::string urlStr = std::string(uri);
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->PostWebMessage(portName, sendPorts, urlStr);
        return NWebError::NO_ERROR;
    }

    CArrUI8 FfiOHOSWebviewCtlSerializeWebState(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return CArrUI8{nullptr, 0};
        }
        std::vector<uint8_t> webState = nativeWebviewCtl->SerializeWebState();
        uint8_t* result = VectorToCArrUI8(webState);
        if (result == nullptr) {
            WEBVIEWLOGE("FfiOHOSWebviewCtlSerializeWebStatee malloc failed");
            *errCode = NWebError::NEW_OOM;
            return CArrUI8{nullptr, 0};
        }
        *errCode = NWebError::NO_ERROR;
        return CArrUI8{result, webState.size()};
    }

    int32_t FfiOHOSWebviewCtlRestoreWebState(int64_t id, CArrUI8 cState)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::vector<uint8_t> state(cState.head, cState.head + cState.size);
        nativeWebviewCtl->RestoreWebState(state);
        return NWebError::NO_ERROR;
    }

    CArrString FfiOHOSWebviewCtlGetCertificate(int64_t id, int32_t *errCode)
    {
        CArrString arrCertificate = {.head = nullptr, .size = 0};
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return arrCertificate;
        }
        std::vector<std::string> certChainDerData;
        bool ans = nativeWebviewCtl->GetCertChainDerData(certChainDerData);
        if (!ans) {
            WEBVIEWLOGE("get cert chain data failed");
            return arrCertificate;
        }
        if (certChainDerData.size() > UINT8_MAX) {
            WEBVIEWLOGE("error, cert chain data array reach max");
            return arrCertificate;
        }
        *errCode = NWebError::NO_ERROR;
        arrCertificate.size = static_cast<int64_t>(certChainDerData.size());
        arrCertificate.head = OHOS::Webview::VectorToCArrString(certChainDerData);
        return arrCertificate;
    }

    int32_t FfiOHOSWebviewCtlHasImage(int64_t id, void (*callbackRef)(RetDataBool))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        return nativeWebviewCtl->HasImagesCallback(CJLambda::Create(callbackRef));
    }

    int32_t FfiWebviewCtlCustomizeSchemes(OHOS::Webview::CArrScheme schemes)
    {
        return WebviewControllerImpl::CustomizeSchemesArrayDataHandler(schemes);
    }

    bool FfiOHOSWebviewCtlTerminateRenderProcess(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool terminate = nativeWebviewCtl->TerminateRenderProcess();
        *errCode = NWebError::NO_ERROR;
        return terminate;
    }

    int32_t FfiOHOSWebviewCtlCloseAllMediaPresentations(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->CloseAllMediaPresentations();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlPauseAllMedia(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->PauseAllMedia();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlResumeAllMedia(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->ResumeAllMedia();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlStopAllMedia(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->StopAllMedia();
        return NWebError::NO_ERROR;
    }

    void FfiOHOSWebviewCtlResumeAllTimers()
    {
        NWeb::NWebHelper::Instance().ResumeAllTimers();
    }

    void FfiOHOSWebviewCtlPauseAllTimers()
    {
        NWeb::NWebHelper::Instance().PauseAllTimers();
    }

    int32_t FfiOHOSWebviewCtlSetPrintBackground(int64_t id, bool enable)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->SetPrintBackground(enable);
        return NWebError::NO_ERROR;
    }

    bool FfiOHOSWebviewCtlGetPrintBackground(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool printBackground = nativeWebviewCtl->GetPrintBackground();
        *errCode = NWebError::NO_ERROR;
        return printBackground;
    }

    int32_t FfiOHOSWebviewCtlSetScrollable(int64_t id, bool enable)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->SetScrollable(enable);
        return NWebError::NO_ERROR;
    }

    bool FfiOHOSWebviewCtlGetScrollable(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool scrollable = nativeWebviewCtl->GetScrollable();
        *errCode = NWebError::NO_ERROR;
        return scrollable;
    }

    void FfiOHOSWebviewCtlEnableAdsBlock(int64_t id, bool enable)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return;
        }
        nativeWebviewCtl->EnableAdsBlock(enable);
    }

    bool FfiOHOSWebviewCtlIsAdsBlockEnabled(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return false;
        }
        return nativeWebviewCtl->IsAdsBlockEnabled();
    }

    bool FfiOHOSWebviewCtlIsAdsBlockEnabledForCurPage(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return false;
        }
        return nativeWebviewCtl->IsAdsBlockEnabledForCurPage();
    }

    bool FfiOHOSWebviewCtlIsIntelligentTrackingPreventionEnabled(int64_t id, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool isIntelligentTrackingPreventionEnabled = nativeWebviewCtl->IsIntelligentTrackingPreventionEnabled();
        *errCode = NWebError::NO_ERROR;
        return isIntelligentTrackingPreventionEnabled;
    }

    int32_t FfiOHOSWebviewCtlEnableIntelligentTrackingPrevention(int64_t id, bool enable)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->EnableIntelligentTrackingPrevention(enable);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlGetMediaPlaybackState(int64_t id, int32_t *errorCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errorCode = NWebError::INIT_ERROR;
            return -1;
        }
        *errorCode = NWebError::NO_ERROR;
        return nativeWebviewCtl->GetMediaPlaybackState();
    }

    int32_t FfiOHOSWebviewCtlGetRenderProcessMode()
    {
        WEBVIEWLOGI("get render process mode.");
        int32_t mode = static_cast<int32_t>(NWebHelper::Instance().GetRenderProcessMode());
        return mode;
    }

    void FfiOHOSWebviewCtlSetRenderProcessMode(int32_t mode)
    {
        WEBVIEWLOGI("set render process mode.");
        NWebHelper::Instance().SetRenderProcessMode(
            static_cast<NWeb::RenderProcessMode>(mode)
        );
    }

    int32_t FfiOHOSWebviewCtlWarmupServiceWorker(char* url)
    {
        size_t size = strlen(url);
        if (size > URL_MAXIMUM || !regex_match(url, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            return NWebError::INVALID_URL;
        }
        NWeb::NWebHelper::Instance().WarmupServiceWorker(url);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlSetHostIP(char* hostname, char* address, int32_t aliveTime)
    {
        if (strlen(hostname) + 1 > UINT_MAX || strlen(hostname) + 1 > UINT_MAX || aliveTime <= 0) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        unsigned char buf[sizeof(in6_addr)];
        if (!((inet_pton(AF_INET, address, buf) == 1) || (inet_pton(AF_INET6, address, buf) == 1))) {
            WEBVIEWLOGE("IP Error.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        NWeb::NWebHelper::Instance().SetHostIP(hostname, address, aliveTime);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlClearHostIP(char* hostname)
    {
        if (strlen(hostname) + 1 > UINT_MAX) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        NWeb::NWebHelper::Instance().ClearHostIP(hostname);
        return NWebError::NO_ERROR;
    }

    RetDataCString FfiOHOSWebviewCtlGetLastJavascriptProxyCallingFrameUrl(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return ret;
        }
        std::string lastCallingFrameUrl = nativeWebviewCtl->GetLastJavascriptProxyCallingFrameUrl();
        if (lastCallingFrameUrl == "") {
            ret.code = NWebError::INIT_ERROR;
        } else {
            ret.code = NWebError::NO_ERROR;
        }
        ret.data = MallocCString(lastCallingFrameUrl);
        return ret;
    }

    int32_t FfiOHOSWebviewCtlEnableWholeWebPageDrawing()
    {
        NWebHelper::Instance().EnableWholeWebPageDrawing();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlClearPrefetchedResource(CArrString cacheKeyList)
    {
        std::vector<std::string> CcacheKeyList;
        for (int64_t i = 0; i < cacheKeyList.size; i++) {
            CcacheKeyList.push_back(std::string(cacheKeyList.head[i]));
        }
        NWebHelper::Instance().ClearPrefetchedResource(CcacheKeyList);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlStartCamera(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->StartCamera();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlStopCamera(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->StopCamera();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlCloseCamera(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->CloseCamera();
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlAddIntelligentTrackingPreventionBypassingList(CArrString hostList)
    {
        std::vector<std::string> hosts;
        for (int64_t i = 0; i < hostList.size; i++) {
            hosts.push_back(std::string(hostList.head[i]));
        }
        NWebHelper::Instance().AddIntelligentTrackingPreventionBypassingList(hosts);
        return NWebError::NO_ERROR;
    }

    void FfiOHOSWebviewCtlClearIntelligentTrackingPreventionBypassingList()
    {
        WEBVIEWLOGD("Clear intelligent tracking prevention bypassing list.")
        NWebHelper::Instance().ClearIntelligentTrackingPreventionBypassingList();
        return;
    }

    void FfiOHOSWebviewCtlRemoveIntelligentTrackingPreventionBypassingList(CArrString hostList)
    {
        WEBVIEWLOGD("Remove intelligent tracking prevention bypassing list.")
        std::vector<std::string> hosts;
        for (int64_t i = 0; i < hostList.size; i++) {
            hosts.push_back(std::string(hostList.head[i]));
        }
        NWebHelper::Instance().RemoveIntelligentTrackingPreventionBypassingList(hosts);
        return;
    }

    void FfiOHOSWebviewCtlEnableBackForwardCache(bool nativeEmbed, bool mediaTakeOver)
    {
        NWebHelper::Instance().EnableBackForwardCache(nativeEmbed, mediaTakeOver);
        return;
    }

    RetDataCString FfiOHOSWebviewCtlGetSurfaceId(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return ret;
        }
        std::string surfaceId = nativeWebviewCtl->GetSurfaceId();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(surfaceId);
        return ret;
    }

    int32_t FfiOHOSWebviewCtlInjectOfflineResources(int64_t id, CArrOfflineResourceMap maps)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int64_t count = maps.size;
        for (int64_t i = 0; i < count; i++) {
            COfflineResourceMap map = maps.head[i];
            std::vector<std::string> urlList;
            for (int i = 0; i < map.urlList.size; i++) {
                urlList.push_back(std::string(map.urlList.head[i]));
            }
            for (auto url : urlList) {
                if (!CheckUrl(url)) {
                    return NWebError::INVALID_URL;
                }
            }
            std::vector<uint8_t> resource;
            for (int i = 0; i < map.resourceCSize; i++) {
                resource.push_back(map.resource[i]);
            }
            std::map<std::string, std::string> responseHeaders;
            for (int i = 0; i < map.responseHeaders.size; i++) {
                std::string key = map.responseHeaders.head[i].headerKey;
                std::string value = map.responseHeaders.head[i].headerValue;
                responseHeaders[key] = value;
            }
            uint32_t type = map.type;
            nativeWebviewCtl->InjectOfflineResources(urlList, resource, responseHeaders, type);
        }
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlSetBackForwardCacheOptions(int64_t id, int32_t size, int32_t timeToLive)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        nativeWebviewCtl->SetBackForwardCacheOptions(size, timeToLive);
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlSetPathAllowingUniversalAccess(int64_t id, CArrString pathList)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        std::vector<std::string> paths;
        for (int i = 0; i < pathList.size; i++) {
            paths.push_back(pathList.head[i]);
        }
        std::string errorPath;
        nativeWebviewCtl->SetPathAllowingUniversalAccess(paths, errorPath);
        if (!errorPath.empty()) {
            WEBVIEWLOGE("%{public}s is invalid.", errorPath.c_str());
            return NWebError::PARAM_CHECK_ERROR;
        }
        return NWebError::NO_ERROR;
    }

    int32_t FfiOHOSWebviewCtlSetUrlTrustList(int64_t id, char* cUrl)
    {
        WEBVIEWLOGD("SetUrlTrustList invoked");
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr) {
            return NWebError::INIT_ERROR;
        }
        std::string urlTrustList = cUrl;
        if (urlTrustList.size() > MAX_URL_TRUST_LIST_STR_LEN) {
            WEBVIEWLOGE("url trust list len is too large.");
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::string detailMsg;
        int32_t ret = nativeWebviewCtl->SetUrlTrustList(urlTrustList, detailMsg);
        if (ret != NWebError::NO_ERROR) {
            WEBVIEWLOGE("SetUrlTrustList failed, error code : %{public}d", ret);
        }
        return ret;
    }

    int32_t FfiOHOSWebviewCtlPrefetchResource(
        CRequestInfo request, ArrWebHeader additionalHttpHeaders, char* cKey, int32_t cacheValidTime)
    {
        std::string url = request.url;
        std::string method = request.method;
        std::string formData = request.formData;
        if (method != "POST") {
            WEBVIEWLOGE(
                "The method %{public}s is not supported. The Type of 'method' must be string 'POST'.", method.c_str());
            return NWebError::PARAM_CHECK_ERROR;
        }
        std::shared_ptr<NWebEnginePrefetchArgs> prefetchArgs =
            std::make_shared<NWebEnginePrefetchArgsImpl>(url, method, formData);
        std::map<std::string, std::string> headers;
        for (int i = 0; i < additionalHttpHeaders.size; i++) {
            std::string key = additionalHttpHeaders.head[i].headerKey;
            std::string value = additionalHttpHeaders.head[i].headerValue;
            headers[key] = value;
        }

        std::string cacheKey = cKey;
        if (cacheKey == "") {
            cacheKey = url;
        }
        for (char c : cacheKey) {
            if (!isalnum(c)) {
                WEBVIEWLOGE("The character of 'cacheKey' must be numbers or letters.");
                return NWebError::PARAM_CHECK_ERROR;
            }
        }
        NWebHelper::Instance().PrefetchResource(prefetchArgs, headers, cacheKey, cacheValidTime);
        return NWebError::NO_ERROR;
    }

    // BackForwardList
    int32_t FfiOHOSBackForwardListCurrentIndex(int64_t id, int32_t *errCode)
    {
        auto nativeWebHistoryListImpl = FFIData::GetData<WebHistoryListImpl>(id);
        if (nativeWebHistoryListImpl == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        *errCode = NWebError::NO_ERROR;
        return nativeWebHistoryListImpl->GetCurrentIndex();
    }

    int32_t FfiOHOSBackForwardListSize(int64_t id, int32_t *errCode)
    {
        auto nativeWebHistoryListImpl = FFIData::GetData<WebHistoryListImpl>(id);
        if (nativeWebHistoryListImpl == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        *errCode = NWebError::NO_ERROR;
        return nativeWebHistoryListImpl->GetListSize();
    }

    Media::PixelFormat GetColorType(ImageColorType colorType)
    {
        Media::PixelFormat pixelFormat;
        switch (colorType) {
            case ImageColorType::COLOR_TYPE_UNKNOWN:
                pixelFormat = Media::PixelFormat::UNKNOWN;
                break;
            case ImageColorType::COLOR_TYPE_RGBA_8888:
                pixelFormat = Media::PixelFormat::RGBA_8888;
                break;
            case ImageColorType::COLOR_TYPE_BGRA_8888:
                pixelFormat = Media::PixelFormat::BGRA_8888;
                break;
            default:
                pixelFormat = Media::PixelFormat::UNKNOWN;
                break;
        }
        return pixelFormat;
    }

    Media::AlphaType GetAlphaType(ImageAlphaType imageAlphaType)
    {
        Media::AlphaType alphaType;
        switch (imageAlphaType) {
            case ImageAlphaType::ALPHA_TYPE_UNKNOWN:
                alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
                break;
            case ImageAlphaType::ALPHA_TYPE_OPAQUE:
                alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
                break;
            case ImageAlphaType::ALPHA_TYPE_PREMULTIPLIED:
                alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
                break;
            case ImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED:
                alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
                break;
            default:
                alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
                break;
        }
        return alphaType;
    }

    int64_t GetFavicon(std::shared_ptr<NWebHistoryItem> item)
    {
        void *data = nullptr;
        int32_t width = 0;
        int32_t height = 0;
        ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
        ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
        bool isGetFavicon = item->GetFavicon(&data, width, height, colorType, alphaType);
        if (!isGetFavicon) {
            return -1;
        }
        OHOS::Media::InitializationOptions opt;
        opt.size.width = width;
        opt.size.height = height;
        opt.pixelFormat = GetColorType(colorType);
        opt.alphaType = GetAlphaType(alphaType);
        opt.editable = true;
        std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMapImpl::CreatePixelMap(opt);
        if (pixelMap == nullptr) {
            return -1;
        }
        uint64_t stride = static_cast<uint64_t>(width) << 2;
        uint64_t bufferSize = stride * static_cast<uint64_t>(height);
        pixelMap->WritePixels(static_cast<const uint8_t *>(data), bufferSize);
        auto nativeImage = FFIData::Create<Media::PixelMapImpl>(move(pixelMap));
        if (nativeImage == nullptr) {
            return -1;
        }
        WEBVIEWLOGI("[PixelMap] create PixelMap success");
        return nativeImage->GetID();
    }

    int64_t FfiOHOSWebviewCtlGetFavicon(int64_t id, int32_t *errCode)
    {
        int64_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        const void *data = nullptr;
        size_t width = 0;
        size_t height = 0;
        ImageColorType colorType = ImageColorType::COLOR_TYPE_UNKNOWN;
        ImageAlphaType alphaType = ImageAlphaType::ALPHA_TYPE_UNKNOWN;
        bool isGetFavicon = nativeWebviewCtl->GetFavicon(&data, width, height, colorType, alphaType);
        if (!isGetFavicon) {
            return ret;
        }
        OHOS::Media::InitializationOptions opt;
        opt.size.width = static_cast<int32_t>(width);
        opt.size.height = static_cast<int32_t>(height);
        opt.pixelFormat = GetColorType(colorType);
        opt.alphaType = GetAlphaType(alphaType);
        opt.editable = true;
        auto pixelMap = Media::PixelMap::Create(opt);
        if (pixelMap == nullptr) {
            return ret;
        }
        uint64_t stride = static_cast<uint64_t>(width) << 2;
        uint64_t bufferSize = stride * static_cast<uint64_t>(height);
        pixelMap->WritePixels(static_cast<const uint8_t *>(data), bufferSize);
        auto nativeImage = FFIData::Create<Media::PixelMapImpl>(move(pixelMap));
        if (nativeImage == nullptr) {
            return ret;
        }
        return nativeImage->GetID();
    }

    CHistoryItem FfiOHOSGetItemAtIndex(int64_t id, int32_t index, int32_t *errCode)
    {
        CHistoryItem ret = {.icon = -1, .historyUrl = nullptr, .historyRawUrl = nullptr, .title = nullptr};
        auto nativeWebHistoryListImpl = FFIData::GetData<WebHistoryListImpl>(id);
        if (nativeWebHistoryListImpl == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        if (index >= nativeWebHistoryListImpl->GetListSize() || index < 0) {
            *errCode = NWebError::PARAM_CHECK_ERROR;
            return ret;
        }
        std::shared_ptr<NWebHistoryItem> item = nativeWebHistoryListImpl->GetItem(index);
        if (!item) {
            *errCode = NWebError::NWEB_ERROR;
            return ret;
        }
        ret.historyUrl = MallocCString(item->GetHistoryUrl());
        ret.historyRawUrl = MallocCString(item->GetHistoryRawUrl());
        ret.title = MallocCString(item->GetHistoryTitle());
        ret.icon = GetFavicon(item);
        *errCode = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlPrepareForPageLoad(char *url, bool preconnectable, int32_t numSockets)
    {
        int32_t ret = -1;
        std::string webSrc = url;
        if (webSrc.size() > URL_MAXIMUM) {
            WEBVIEWLOGE("The URL exceeds the maximum length of %{public}d", URL_MAXIMUM);
            return NWebError::PARAM_CHECK_ERROR;
        }

        if (!regex_match(webSrc, std::regex(URL_REGEXPR, std::regex_constants::icase))) {
            WEBVIEWLOGE("ParsePrepareUrl error");
            return NWebError::PARAM_CHECK_ERROR;
        }

        if (numSockets <= 0 || static_cast<uint32_t>(numSockets) > SOCKET_MAXIMUM) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        NWeb::NWebHelper::Instance().PrepareForPageLoad(webSrc, preconnectable, numSockets);
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSetConnectionTimeout(int32_t timeout)
    {
        int32_t ret = -1;
        if (timeout <= 0) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        NWeb::NWebHelper::Instance().SetConnectionTimeout(timeout);
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSlideScroll(int64_t id, float vx, float vy)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        nativeWebviewCtl->SlideScroll(vx, vy);
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSetNetworkAvailable(int64_t id, bool enable)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        nativeWebviewCtl->PutNetworkAvailable(enable);
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlClearClientAuthenticationCache(int64_t id)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        nativeWebviewCtl->ClearClientAuthenticationCache();
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlClearSslCache(int64_t id)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        nativeWebviewCtl->ClearSslCache();
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSearchNext(int64_t id, bool forward)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        nativeWebviewCtl->SearchNext(forward);
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlClearMatches(int64_t id)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        nativeWebviewCtl->ClearMatches();
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSearchAllAsync(int64_t id, char * searchString)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        std::string str = searchString;
        nativeWebviewCtl->SearchAllAsync(str);
        ret = NWebError::NO_ERROR;
        return ret;
    }

    int32_t FfiOHOSWebviewCtlDeleteJavaScriptRegister(int64_t id, char *name)
    {
        int32_t ret = -1;
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            ret = NWebError::INIT_ERROR;
            return ret;
        }
        std::string str = name;
        ret = nativeWebviewCtl->DeleteJavaScriptRegister(str, {});
        return ret;
    }

    void FfiOHOSWebviewCtlSetWebSchemeHandler(int64_t id, char* cScheme, int64_t classId, int32_t *errCode)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        WebSchemeHandlerImpl* nativeWebSchemeHandler = FFIData::GetData<WebSchemeHandlerImpl>(classId);
        if (!nativeWebSchemeHandler) {
            WEBVIEWLOGE("set WebSchemeHandler is null");
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        bool ret = nativeWebviewCtl->SetWebSchemeHandler(cScheme, nativeWebSchemeHandler);
        if (!ret) {
            WEBVIEWLOGE("FfiOHOSWebviewCtlSetWebSchemeHandler failed");
        }
        *errCode = NWebError::NO_ERROR;
        return;
    }

    int32_t FfiOHOSWebviewCtlClearWebSchemeHandler(int64_t id)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        int32_t ret = nativeWebviewCtl->ClearWebSchemeHandler();
        if (ret != NWebError::NO_ERROR) {
            WEBVIEWLOGE("FfiOHOSWebviewCtlClearWebSchemeHandler failed");
        }
        return ret;
    }

    int32_t FfiOHOSWebviewCtlSetServiceWorkerWebSchemeHandler(char* cScheme, int64_t classId)
    {
        auto nativeWebSchemeHandler = FFIData::GetData<WebSchemeHandlerImpl>(classId);
        if (!nativeWebSchemeHandler) {
            return NWebError::INIT_ERROR;
        }
        std::string scheme = cScheme;
        bool ret = WebviewControllerImpl::SetWebServiceWorkerSchemeHandler(scheme.c_str(), nativeWebSchemeHandler);
        if (!ret) {
            WEBVIEWLOGE("SetServiceWorkerWebSchemeHandler failed")
        }
        return ret;
    }

    int32_t FfiOHOSWebviewCtlClearServiceWorkerWebSchemeHandler()
    {
        int32_t ret = WebviewControllerImpl::ClearWebServiceWorkerSchemeHandler();
        if (ret != 0) {
            WEBVIEWLOGE("FfiOHOSWebviewCtlClearServiceWorkerWebSchemeHandler ret=%{public}d", ret)
        }
        return ret;
    }

    void FfiOHOSWebviewCtlOnCreateNativeMediaPlayer(int64_t id,
        int64_t (* callback)(int64_t, OHOS::Webview::CMediaInfo))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            WEBVIEWLOGE("Webview controller is null or not init ");
            return;
        }
        nativeWebviewCtl->OnCreateNativeMediaPlayer(CJLambda::Create(callback));
    }

    int32_t FfiOHOSWebviewCtlPrecompileJavaScript(int64_t id,
        char* url, char* script, OHOS::Webview::CacheOptions cacheOptions)
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        ArrWebHeader headers = cacheOptions.arrHead;
        uint32_t arrayLength = static_cast<uint32_t>(headers.size);
        std::map<std::string, std::string> responseHeaders;
        for (uint32_t i = 0; i < arrayLength; ++i) {
            std::string key = headers.head[i].headerKey;
            std::string value = headers.head[i].headerValue;
            responseHeaders[key] = value;
        }
        auto cache = std::make_shared<NWebCacheOptionsImpl>(responseHeaders);
        std::string str = std::string(script);
        return nativeWebviewCtl->PrecompileJavaScript(url, str, cache);
    }

    int32_t FfiOHOSWebviewCtlWebPageSnapshot(
        int64_t id, CSnapshotInfo snapshot, void (*callbackRef)(RetDataCSnapshotResult infoRef))
    {
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            return NWebError::INIT_ERROR;
        }
        if (g_inWebPageSnapshot) {
            return NWebError::FUNCTION_NOT_ENABLE;
        }
        g_inWebPageSnapshot = true;
        std::string nativeSnapshotId = snapshot.id;
        int32_t nativeSnapshotSizeWidth = snapshot.width;
        int32_t nativeSnapshotSizeHeight = snapshot.height;
        int32_t nativeSnapshotSizeHeightType = snapshot.widthType;
        int32_t nativeSnapshotSizeWidthType = snapshot.heightType;
        NWeb::PixelUnit nativeSnapshotSizeType = NWeb::PixelUnit::NONE;

        if (nativeSnapshotSizeHeightType != nativeSnapshotSizeWidthType) {
            WEBVIEWLOGE("WebPageSnapshot input different pixel unit");
            g_inWebPageSnapshot = false;
            return NWebError::PARAM_CHECK_ERROR;
        }
        if (NWeb::PixelUnit(nativeSnapshotSizeHeightType) != NWeb::PixelUnit::NONE) {
            nativeSnapshotSizeType = NWeb::PixelUnit(nativeSnapshotSizeHeightType);
        }
        if (nativeSnapshotSizeWidth < 0 || nativeSnapshotSizeHeight < 0) {
            WEBVIEWLOGE("WebPageSnapshot input pixel length less than 0");
            g_inWebPageSnapshot = false;
            return NWebError::PARAM_CHECK_ERROR;
        }
        bool pixelCheck = false;
        if (nativeSnapshotSizeType != NWeb::PixelUnit::VP) {
            pixelCheck = true;
        }
        auto onChange = CJLambda::Create(callbackRef);
        auto resultCallback = CreateWebPageSnapshotResultCallback(
            pixelCheck, nativeSnapshotSizeWidth, nativeSnapshotSizeHeight,
            static_cast<int32_t>(nativeSnapshotSizeType), onChange);
        int32_t ret = nativeWebviewCtl->WebPageSnapshot(nativeSnapshotId.c_str(),
            nativeSnapshotSizeType, nativeSnapshotSizeWidth, nativeSnapshotSizeHeight, std::move(resultCallback));
        if (ret != NO_ERROR) {
            g_inWebPageSnapshot = false;
        }
        return ret;
    }

    RetDataCString FfiOHOSWebviewCtlGetLastHitTest(int64_t id, int32_t *errCode)
    {
        RetDataCString ret = { .code = NWeb::HitTestResult::UNKNOWN_TYPE, .data = nullptr };
        auto nativeWebviewCtl = FFIData::GetData<WebviewControllerImpl>(id);
        if (nativeWebviewCtl == nullptr || !nativeWebviewCtl->IsInit()) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        std::shared_ptr<NWeb::HitTestResult> nwebResult = nativeWebviewCtl->GetLastHitTest();
        *errCode = NWebError::NO_ERROR;
        if (nwebResult) {
            ret.code = nwebResult->GetType();
            ret.data = MallocCString(nwebResult->GetExtra());
        }
        return ret;
    }
}
}
}