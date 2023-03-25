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

#include <condition_variable>
#include <mutex>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "webview_controller.h"
#include "uv.h"

namespace OHOS {
namespace NWeb {
const std::string WEBVIEW_CONTROLLER_CLASS_NAME = "WebviewController";
const std::string WEB_MESSAGE_PORT_CLASS_NAME = "WebMessagePort";
const std::string WEB_HITTESTTYPE_V9_ENUM_NAME = "HitTestTypeV9";
const std::string WEB_HITTESTTYPE_ENUM_NAME = "WebHitTestType";
const std::string WEB_HISTORY_LIST_CLASS_NAME = "WebHistoryList";

class NapiWebviewController {
public:
    NapiWebviewController() {}
    ~NapiWebviewController() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value InitializeWebEngine(napi_env env, napi_callback_info info);

    static napi_value SetWebDebuggingAccess(napi_env env, napi_callback_info info);

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

    static napi_value RegisterJavaScriptProxy(napi_env env, napi_callback_info info);

    static napi_value DeleteJavaScriptRegister(napi_env env, napi_callback_info info);

    static napi_value RunJavaScript(napi_env env, napi_callback_info info);

    static napi_value RunJavaScriptInternal(napi_env env, napi_callback_info info,
        const std::string &script);

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

    static napi_value CustomizeSchemes(napi_env env, napi_callback_info info);

    static napi_value InnerGetThisVar(napi_env env, napi_callback_info info);
};

class NWebValueCallbackImpl : public OHOS::NWeb::NWebValueCallback<std::shared_ptr<NWebMessage>> {
public:
    NWebValueCallbackImpl(napi_env env, napi_ref callback) : env_(env), callback_(callback) {}
    ~NWebValueCallbackImpl();
    void OnReceiveValue(std::shared_ptr<NWebMessage> result) override;

private:
    napi_env env_;
    napi_ref callback_;
    static void UvWebMessageOnReceiveValueCallback(uv_work_t *work, int status);
};

class NapiWebMessagePort {
public:
    NapiWebMessagePort() = default;
    ~NapiWebMessagePort() = default;

    struct WebMsgPortParam {
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
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_NAPI_WEBVIEW_CONTROLLER_H
