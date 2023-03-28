/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_HANDLER_H
#define NWEB_HANDLER_H

#include <memory>
#include <string>
#include <vector>

#include "nweb_access_request.h"
#include "nweb.h"
#include "nweb_console_log.h"
#include "nweb_context_menu_params.h"
#include "nweb_controller_handler.h"
#include "nweb_data_resubmission_callback.h"
#include "nweb_file_selector_params.h"
#include "nweb_full_screen_exit_handler.h"
#include "nweb_geolocation_callback_interface.h"
#include "nweb_js_dialog_result.h"
#include "nweb_js_http_auth_result.h"
#include "nweb_js_ssl_error_result.h"
#include "nweb_js_ssl_select_cert_result.h"
#include "nweb_select_popup_menu.h"
#include "nweb_touch_handle_state.h"
#include "nweb_url_resource_error.h"
#include "nweb_url_resource_request.h"
#include "nweb_url_resource_response.h"

namespace OHOS::NWeb {
enum class RenderExitReason {
    // Render process non-zero exit status
    PROCESS_ABNORMAL_TERMINATION,

    // SIGKILL or task manager kill
    PROCESS_WAS_KILLED,

    // Segmentation fault
    PROCESS_CRASHED,

    // Out of memory
    PROCESS_OOM,

    // Unknown reason
    PROCESS_EXIT_UNKNOWN,
};

enum class SslError {
    // General error
    INVALID,

    // Hostname mismatch
    HOSTMISMATCH,

    // The certificate date is invalid
    DATEINVALID,

    // The certificate authority is not trusted
    UNTRUSTED,
};

struct ImageOptions {
    ImageColorType colorType;
    ImageAlphaType alphaType;
    size_t width;
    size_t height;
};

// Cursor type values.
enum class CursorType: int32_t {
    CT_POINTER = 0,
    CT_CROSS,
    CT_HAND,
    CT_IBEAM,
    CT_WAIT,
    CT_HELP,
    CT_EASTRESIZE,
    CT_NORTHRESIZE,
    CT_NORTHEASTRESIZE,
    CT_NORTHWESTRESIZE,
    CT_SOUTHRESIZE,
    CT_SOUTHEASTRESIZE,
    CT_SOUTHWESTRESIZE,
    CT_WESTRESIZE,
    CT_NORTHSOUTHRESIZE,
    CT_EASTWESTRESIZE,
    CT_NORTHEASTSOUTHWESTRESIZE,
    CT_NORTHWESTSOUTHEASTRESIZE,
    CT_COLUMNRESIZE,
    CT_ROWRESIZE,
    CT_MIDDLEPANNING,
    CT_EASTPANNING,
    CT_NORTHPANNING,
    CT_NORTHEASTPANNING,
    CT_NORTHWESTPANNING,
    CT_SOUTHPANNING,
    CT_SOUTHEASTPANNING,
    CT_SOUTHWESTPANNING,
    CT_WESTPANNING,
    CT_MOVE,
    CT_VERTICALTEXT,
    CT_CELL,
    CT_CONTEXTMENU,
    CT_ALIAS,
    CT_PROGRESS,
    CT_NODROP,
    CT_COPY,
    CT_NONE,
    CT_NOTALLOWED,
    CT_ZOOMIN,
    CT_ZOOMOUT,
    CT_GRAB,
    CT_GRABBING,
    CT_MIDDLE_PANNING_VERTICAL,
    CT_MIDDLE_PANNING_HORIZONTAL,
    CT_CUSTOM,
    CT_DND_NONE,
    CT_DND_MOVE,
    CT_DND_COPY,
    CT_DND_LINK,
    CT_MAX_VALUE,
};

struct NWebCursorInfo {
    int32_t width = 0;
    int32_t height = 0;
    int32_t x = 0;
    int32_t y = 0;
    float scale = 1.0;
    // buff will be width*height*4 bytes in size and represents a BGRA image with an upper-left origin.
    std::unique_ptr<uint8_t[]> buff = nullptr;
};

using FileSelectorCallback = NWebValueCallback<std::vector<std::string>&>;

class OHOS_NWEB_EXPORT NWebHandler {
public:
    NWebHandler() = default;

    virtual ~NWebHandler() = default;

    virtual void SetNWeb(std::shared_ptr<NWeb> nweb) {}

    virtual void OnProxyDied() {}

    virtual void OnRouterPush(const std::string& param) {}

    virtual void OnMessage(const std::string& param) {}

    /**
     * @brief Notify the SDK that a web site has finished loading. This method is
     * called only for main frame.
     *
     * @param httpStatusCode The status code for the http request.
     * @param url The url of the web site.
     */
    virtual void OnPageLoadEnd(int httpStatusCode, const std::string& url) {}

    /**
     * @brief Notify the SDK that a web site has started loading. This method is
     * called once for each main frame load.
     *
     * @param url The url to be loaded.
     */
    virtual void OnPageLoadBegin(const std::string& url) {}

    /**
     * @brief Report a load error to the SDK.
     *
     * @param errorCode The error code.
     * @param description The error description.
     * @param failingUrl The failed url.
     */
    virtual void OnPageLoadError(int errorCode,
                                 const std::string& description,
                                 const std::string& failingUrl) {}

    /**
     * @brief Give the SDK a chance to decide whether to continue loading the
     * url.
     *
     * @param url The url to be loaded.
     * @return true to cancel the loading, false to continue the loading.
     */
    virtual bool OnHandleInterceptUrlLoading(const std::string& url) {
        return false;
    }

    /**
     * @brief Notify the SDK that the nweb will load the resource specified by
     * the given url.
     *
     * @param url The url of the resource.
     */
    virtual void OnResource(const std::string& url) {}

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     */
    virtual void OnPageTitle(const std::string& title) {}

    /**
     * @brief Notify the SDK the current progress of loading a web site.
     *
     * @param newProgress Loading progress, an integer between 0 and 100.
     */
    virtual void OnLoadingProgress(int newProgress) {}

    /**
     * @brief Request display and focus for a new nweb.
     *
     */
    virtual void OnFocus() {}

    /**
     * @brief Obtains a list of all visited history items, used for link coloring
     *
     * @retval visited history
     */
    virtual const std::vector<std::string> VisitedUrlHistory() {
        return std::vector<std::string>();
    }

    /**
     * @brief Notify the host application of a resource request and allow the
     * application to return the data.
     *
     * @param request request information
     *
     * @retval std::shared_ptr<NWebUrlResourceResponse> response information
     */
    virtual std::shared_ptr<NWebUrlResourceResponse> OnHandleInterceptRequest(
        std::shared_ptr<NWebUrlResourceRequest> request) {
        return nullptr;
    }

    /**
     * @brief Report web resource loading error to the SDK. These errors usually
     * indicate inability to connect to the server.
     *
     * @param request The request information.
     * @param error The error information.
     */
    virtual void OnResourceLoadError(std::shared_ptr<NWebUrlResourceRequest> request,
                                     std::shared_ptr<NWebUrlResourceError> error) {}

    /**
     * @brief Notify the SDK that an HTTP error has been received from the server
     * while loading a resource.
     *
     * @param request The request information.
     * @param errorResponse The error occurred.
     */
    virtual void OnHttpError(std::shared_ptr<NWebUrlResourceRequest> request,
                             std::shared_ptr<NWebUrlResourceResponse> errorResponse) {}

    /**
     * @brief Notify the SDK of a new favicon for the current web site.
     *
     * @param data The raw image data for the icon.
     * @param width The width of the icon in pixel.
     * @param height The height of the icon in pixel.
     * @param color_type The color data encoding type.
     * @param alpha_type The alpha value of any pixel.
     */
    virtual void OnPageIcon(const void* data,
                            size_t width,
                            size_t height,
                            ImageColorType color_type,
                            ImageAlphaType alpha_type) {}

    /**
     * @brief Notify the SDK of the url for a touch icon.
     *
     * @param icon_url The icon url.
     * @param precomposed The touch icon type.
     */
    virtual void OnDesktopIconUrl(const std::string& icon_url, bool precomposed) {
    }

    /**
     * @brief Report a JavaScript console message to the host application.
     *
     * @param message Details of the console message.
     * @return Return true to stop the message from being output to the console.
     */
    virtual bool OnConsoleLog(const NWebConsoleLog& message) { return false; }

    /**
     * @brief Show prompt to ask for the geolocation permission.
     *
     * @param origin  String: the origin of the resource to get geolocation
     * @param callback  GeolocationCallbackInterface: callback to report
     * geolocation
     */
    virtual void OnGeolocationShow(const std::string& origin,
                                   std::shared_ptr<NWebGeolocationCallbackInterface> callback) {}

    /**
     * @brief Notify the host application that the web page wants to display a
     * JavaScript alert() dialog.
     *
     * @param url  String: The url of the page requesting the dialog.
     * @param message  String: The message of the dialog.
     * @param result  std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     * confirm that the user closed the window.
     * @return To show a custom dialog, the app should return true.
     */
    virtual bool OnAlertDialogByJS(const std::string& url,
                                   const std::string& message,
                                   std::shared_ptr<NWebJSDialogResult> result) {
        return false;
    }

    /**
     * @brief Notify the host application that the web page wants to handle
     * JavaScript onbeforeunload.
     *
     * @param url  String: The url of the page requesting.
     * @param message  String: The message of the dialog.
     * @param result  std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     * confirm that the user closed the window.
     * @return To show a custom dialog, the app should return true.
     */
    virtual bool OnBeforeUnloadByJS(const std::string& url,
                                    const std::string& message,
                                    std::shared_ptr<NWebJSDialogResult> result) {
        return false;
    }

    /**
     * @brief Notify the host application that the web page wants to display a
     * JavaScript prompt() dialog.
     *
     * @param url  String: The url of the page requesting the dialog.
     * @param message  String: The message of the dialog.
     * @param defaultValue  String: The default value of the input message.
     * @param result  std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     * confirm that the user closed the window.
     * @return To show a custom dialog, the app should return true.
     */
    virtual bool OnPromptDialogByJS(const std::string& url,
                                    const std::string& message,
                                    const std::string& defaultValue,
                                    std::shared_ptr<NWebJSDialogResult> result) {
        return false;
    }

    /**
     * @brief Notify the host application that the web page wants to display a
     * JavaScript Confirm() dialog.
     *
     * @param url  String: The url of the page requesting the dialog.
     * @param message  String: The message of the dialog.
     * @param result  std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     * confirm that the user closed the window.
     * @return To show a custom dialog, the app should return true.
     */
    virtual bool OnConfirmDialogByJS(const std::string& url,
                                     const std::string& message,
                                     std::shared_ptr<NWebJSDialogResult> result) {
        return false;
    }

    /**
     * @brief Hide prompt to ask for the geolocation permission.
     */
    virtual void OnGeolocationHide() {}

    /**
     * @brief Ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     * permission.
     */
    virtual void OnPermissionRequest(std::shared_ptr<NWebAccessRequest> request) {}

    /**
     * @brief Cancel the request to ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     * permission.
     */
    virtual void OnPermissionRequestCanceled(std::shared_ptr<NWebAccessRequest> request) {}

    /**
     * @brief called when the render process exit.
     *
     * @param reason  the detail reason why render process exit, the implementation of this callback
     * should attempt to clean up the specific nweb that was set by SetNWeb interface.
     */
    virtual void OnRenderExited(RenderExitReason reason) {}

    /**
     * @brief inform application to update its visited links database.
     *
     * @param url the url being visited.
     * @param isReload true if the url is being reload.
     */
    virtual void OnRefreshAccessedHistory(const std::string& url, bool isReload) {}

    /**
     * @brief inform application to show a file selector.
     * @param callback the file list to select.
     * @param params the params of file selector.
     */
    virtual bool OnFileSelectorShow(std::shared_ptr<FileSelectorCallback> callback,
                                    std::shared_ptr<NWebFileSelectorParams> params) {
        return false;
    }

    virtual void OnScaleChanged(float oldScaleFactor, float newScaleFactor) {}

    virtual bool RunContextMenu(std::shared_ptr<NWebContextMenuParams> params,
                                std::shared_ptr<NWebContextMenuCallback> callback) {
        return false;
    }

    virtual void OnContextMenuDismissed() {}

    virtual bool RunQuickMenu(std::shared_ptr<NWebQuickMenuParams> params,
                              std::shared_ptr<NWebQuickMenuCallback> callback) {
        return false;
    }

    virtual void OnQuickMenuDismissed() {}

    virtual void OnTouchSelectionChanged(
        std::shared_ptr<NWebTouchHandleState> insertHandle,
        std::shared_ptr<NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<NWebTouchHandleState> endSelectionHandle) {}

    virtual bool OnHttpAuthRequestByJS(std::shared_ptr<NWebJSHttpAuthResult> result,
                                       const std::string& host,
                                       const std::string& realm) {
        return false;
    }

    virtual void OnScroll(double xOffset, double yOffset) {}

    virtual bool OnDragAndDropData(const void* data, size_t len, const ImageOptions& opt) {
        return false;
    }

    virtual bool OnSslErrorRequestByJS(std::shared_ptr<NWebJSSslErrorResult> result,
                                       SslError error) {
        return false;
    }

    virtual bool OnSslSelectCertRequestByJS(
        std::shared_ptr<NWebJSSslSelectCertResult> result,
        const std::string& host,
        int port,
        const std::vector<std::string>& keyTypes,
        const std::vector<std::string>& issuers) {
        return false;
    }

    /**
     * @brief called when the page enter the full-screen mode.
     * @param handler to exit the full-screen mode.
     */
    virtual void OnFullScreenEnter(std::shared_ptr<NWebFullScreenExitHandler> handler) {}

    /**
     * @brief called when the page exit the full-screen mode.
     */
    virtual void OnFullScreenExit() {}

    /**
     * @brief Notification window creation request.
     * @param targetUrl target url.
     * @param isAlert Whether it is a dialog box.
     * @param isUserTrigger Whether it was triggered by the user.
     * @param handler set the new web object.
     */
    virtual void OnWindowNewByJS(
        const std::string& targetUrl,
        bool isAlert,
        bool isUserTrigger,
        std::shared_ptr<NWebControllerHandler> handler) {}

    /**
     * @brief Notification window close request.
     */
    virtual void OnWindowExitByJS() {}

    /**
     * @brief called when the page beging loaded is about to be made visible.
     * @param url url.
     */
    virtual void OnPageVisible(const std::string& url) {}

    /**
     * @brief shows the repost from confirmation dialog box.
     * @param handler sets whether to resend data.
     */
    virtual void OnDataResubmission(std::shared_ptr<NWebDataResubmissionCallback> handler) {}

    /**
     * @brief Called when the browser is cursor has changed.
     * @param type Cursor type.
     * @param info If |type| is CT_CUSTOM then |info| will be populated with the custom cursor information.
     * @return True if the cursor change was handled or false for default handling.
     */
    virtual bool OnCursorChange(const CursorType& type, const NWebCursorInfo& info) {
        return false;
    }

    virtual void OnSelectPopupMenu(std::shared_ptr<NWebSelectPopupMenuParam> params,
                                   std::shared_ptr<NWebSelectPopupMenuCallback> callback) {}
};
}  // namespace OHOS::NWeb

#endif  // NWEB_HANDLER_H
