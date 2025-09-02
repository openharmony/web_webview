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

#include "nweb.h"
#include "nweb_access_request.h"
#include "nweb_app_link_callback.h"
#include "nweb_console_log.h"
#include "nweb_context_menu_params.h"
#include "nweb_controller_handler.h"
#include "nweb_custom_keyboard_handler.h"
#include "nweb_data_resubmission_callback.h"
#include "nweb_date_time_chooser.h"
#include "nweb_drag_data.h"
#include "nweb_file_selector_params.h"
#include "nweb_first_meaningful_paint_details.h"
#include "nweb_full_screen_exit_handler.h"
#include "nweb_geolocation_callback_interface.h"
#include "nweb_gesture_event_result.h"
#include "nweb_mouse_event_result.h"
#include "nweb_js_dialog_result.h"
#include "nweb_js_http_auth_result.h"
#include "nweb_js_ssl_error_result.h"
#include "nweb_js_ssl_select_cert_result.h"
#include "nweb_key_event.h"
#include "nweb_largest_contentful_paint_details.h"
#include "nweb_load_committed_details.h"
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

enum class RenderProcessNotRespondingReason {
    // Input  ack from Render process timeout
    INPUT_TIMEOUT,

    // navigation commit ack from Render process timeout
    NAVIGATION_COMMIT_TIMEOUT,
};

enum class ViewportFit {
    // No effect - the whole web page is viewable(default)
    AUTO,

    // The initial layout viewport and the visual viewport are set to the
    // largest rectangle which is inscribed in the display of the device.
    CONTAIN,

    // The initial layout viewport and the visual viewport are set to the
    // circumscribed rectangle of the physical screen of the device.
    COVER,
};

class NWebImageOptions {
public:
    virtual ~NWebImageOptions() = default;

    virtual ImageColorType GetColorType() = 0;
    virtual ImageAlphaType GetAlphaType() = 0;
    virtual size_t GetWidth() = 0;
    virtual size_t GetHeight() = 0;
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

// Cursor type values.
enum class CursorType : int32_t {
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
    CT_LOCK,
    CT_UNLOCK,
    CT_MAX_VALUE,
};

class NWebCursorInfo {
public:
    virtual ~NWebCursorInfo() = default;

    virtual int32_t GetX() = 0;
    virtual int32_t GetY() = 0;
    virtual uint8_t* GetBuff() = 0;
    virtual float GetScale() = 0;
    virtual int32_t GetWidth() = 0;
    virtual int32_t GetHeight() = 0;
};

class NWebTouchHandleHotZone {
public:
    virtual ~NWebTouchHandleHotZone() = default;

    virtual void SetWidth(double width) = 0;
    virtual void SetHeight(double height) = 0;
};

enum class MediaPlayingState {
    // Media is playing
    PLAYING,

    // Media playing is paused
    PAUSED,

    // Media playing is end
    END_OF_STREAM,
};

enum class FormState {
    kHadInteraction,
    kNoInteraction,
};

enum class ActivityType {
    VIDEO = 0,
    AUDIO,
    FORM,
};

enum class NativeEmbedStatus {
    CREATE,
    UPDATE,
    DESTROY,
    ENTER_BFCACHE,
    LEAVE_BFCACHE,
    VISIBLE,
    HIDDEN,
};

enum class NWebFocusSource {
    FOCUS_SOURCE_DEFAULT = -1,
    FOCUS_SOURCE_NAVIGATION = 0,
    FOCUS_SOURCE_SYSTEM,
    FOCUS_SOURCE_GESTURE,
};

class NWebNativeEmbedInfo {
public:
    virtual ~NWebNativeEmbedInfo() = default;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual std::string GetId() = 0;

    virtual std::string GetSrc() = 0;

    virtual std::string GetUrl() = 0;

    virtual std::string GetType() = 0;

    virtual std::string GetTag() = 0;

    virtual std::map<std::string, std::string> GetParams() = 0;

    virtual int32_t GetX() = 0;

    virtual int32_t GetY() = 0;
};

class NWebNativeEmbedDataInfo {
public:
    virtual ~NWebNativeEmbedDataInfo() = default;

    virtual NativeEmbedStatus GetStatus() = 0;

    virtual std::string GetEmbedId() = 0;

    virtual std::string GetSurfaceId() = 0;

    virtual std::shared_ptr<NWebNativeEmbedInfo> GetNativeEmbedInfo() = 0;
};

enum class TouchType : size_t {
    DOWN = 0,
    UP,
    MOVE,
    CANCEL,
};

class NWebNativeEmbedTouchEvent {
public:
    virtual ~NWebNativeEmbedTouchEvent() = default;

    virtual float GetX() = 0;

    virtual float GetY() = 0;

    virtual int32_t GetId() = 0;

    virtual TouchType GetType() = 0;

    virtual float GetOffsetX() = 0;

    virtual float GetOffsetY() = 0;

    virtual float GetScreenX() = 0;

    virtual float GetScreenY() = 0;

    virtual std::string GetEmbedId() = 0;

    virtual std::shared_ptr<NWebGestureEventResult> GetResult() = 0;
};

enum class MouseType : size_t {
    NONE = 0,
    PRESS = 1,
    RELEASE = 2,
    MOVE = 3,
    WINDOW_ENTER = 4,
    WINDOW_LEAVE = 5,
    HOVER,
    HOVER_ENTER,
    HOVER_MOVE,
    HOVER_EXIT,
    PULL_DOWN,
    PULL_MOVE,
    PULL_UP,
    CANCEL
};

enum class MouseButton : size_t {
    NONE_BUTTON = 0,
    LEFT_BUTTON = 1,
    RIGHT_BUTTON = 2,
    MIDDLE_BUTTON = 4,
    BACK_BUTTON = 8,
    FORWARD_BUTTON = 16,
    SIDE_BUTTON = 32,
    EXTRA_BUTTON = 64,
    TASK_BUTTON = 128,
};

class NWebNativeEmbedMouseEvent {
public:
    virtual ~NWebNativeEmbedMouseEvent() = default;

    virtual float GetX() = 0;

    virtual float GetY() = 0;

    virtual bool IsHitNativeArea() = 0;

    virtual MouseType GetType() = 0;

    virtual MouseButton GetButton() = 0;

    virtual float GetOffsetX() = 0;

    virtual float GetOffsetY() = 0;

    virtual float GetScreenX() = 0;

    virtual float GetScreenY() = 0;

    virtual std::string GetEmbedId() = 0;

    virtual std::shared_ptr<NWebMouseEventResult> GetResult() = 0;
};

enum class NativeEmbedParamStatus {
    ADD,
    UPDATE,
    DELETE,
};

class NWebNativeEmbedParamItem {
public:
    virtual ~NWebNativeEmbedParamItem() = default;

    virtual NativeEmbedParamStatus GetStatus() = 0;

    virtual std::string GetId() = 0;

    virtual std::string GetName() = 0;

    virtual std::string GetValue() = 0;
};

class NWebNativeEmbedParamDataInfo {
public:
    virtual ~NWebNativeEmbedParamDataInfo() = default;

    virtual std::string GetEmbedId() = 0;

    virtual std::string GetObjectAttributeId() = 0;

    virtual std::vector<std::shared_ptr<NWebNativeEmbedParamItem>> GetParamItems() = 0;
};

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
    virtual void OnPageLoadError(int errorCode, const std::string& description, const std::string& failingUrl) {}

    /**
     * @brief Give the SDK a chance to decide whether to continue loading the
     * url.
     *
     * @param url The url to be loaded.
     * @return true to cancel the loading, false to continue the loading.
     */
    virtual bool OnHandleInterceptUrlLoading(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request)
    {
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
     * @return Return true if request focus success, false if request focus fail.
     */
    virtual bool OnFocus()
    {
        return false;
    }

    /**
     * @brief Obtains a list of all visited history items, used for link coloring
     *
     * @retval visited history
     */
    virtual std::vector<std::string> VisitedUrlHistory()
    {
        return std::vector<std::string>();
    }

    /**
     * @brief Notify the host application of a resource request and allow the
     * application to return the data.
     *
     * @param request request information
     * @param response response information
     *
     * @retval true if handle success, otherwise false.
     */
    virtual bool OnHandleInterceptRequest(
        std::shared_ptr<NWebUrlResourceRequest> request, std::shared_ptr<NWebUrlResourceResponse> response)
    {
        return false;
    }

    /**
     * @brief Report web resource loading error to the SDK. These errors usually
     * indicate inability to connect to the server.
     *
     * @param request The request information.
     * @param error The error information.
     */
    virtual void OnResourceLoadError(
        std::shared_ptr<NWebUrlResourceRequest> request, std::shared_ptr<NWebUrlResourceError> error)
    {}

    /**
     * @brief Notify the SDK that an HTTP error has been received from the server
     * while loading a resource.
     *
     * @param request The request information.
     * @param errorResponse The error occurred.
     */
    virtual void OnHttpError(
        std::shared_ptr<NWebUrlResourceRequest> request, std::shared_ptr<NWebUrlResourceResponse> errorResponse)
    {}

    /**
     * @brief Notify the SDK of a new favicon for the current web site.
     *
     * @param data The raw image data for the icon.
     * @param width The width of the icon in pixel.
     * @param height The height of the icon in pixel.
     * @param color_type The color data encoding type.
     * @param alpha_type The alpha value of any pixel.
     */
    virtual void OnPageIcon(
        const void* data, size_t width, size_t height, ImageColorType color_type, ImageAlphaType alpha_type)
    {}

    /**
     * @brief Notify the SDK of the url for a touch icon.
     *
     * @param icon_url The icon url.
     * @param precomposed The touch icon type.
     */
    virtual void OnDesktopIconUrl(const std::string& icon_url, bool precomposed) {}

    /**
     * @brief Report a JavaScript console message to the host application.
     *
     * @param console_log Details of the console message.
     * @return Return true to stop the message from being output to the console.
     */
    virtual bool OnConsoleLog(std::shared_ptr<NWebConsoleLog> console_log)
    {
        return false;
    }

    /**
     * @brief Show prompt to ask for the geolocation permission.
     *
     * @param origin  String: the origin of the resource to get geolocation
     * @param callback  GeolocationCallbackInterface: callback to report
     * geolocation
     */
    virtual void OnGeolocationShow(
        const std::string& origin, std::shared_ptr<NWebGeolocationCallbackInterface> callback)
    {}

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
    virtual bool OnAlertDialogByJS(
        const std::string& url, const std::string& message, std::shared_ptr<NWebJSDialogResult> result)
    {
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
    virtual bool OnBeforeUnloadByJS(
        const std::string& url, const std::string& message, std::shared_ptr<NWebJSDialogResult> result)
    {
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
    virtual bool OnPromptDialogByJS(const std::string& url, const std::string& message, const std::string& defaultValue,
        std::shared_ptr<NWebJSDialogResult> result)
    {
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
    virtual bool OnConfirmDialogByJS(
        const std::string& url, const std::string& message, std::shared_ptr<NWebJSDialogResult> result)
    {
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
    virtual bool OnFileSelectorShow(
        std::shared_ptr<NWebStringVectorValueCallback> callback, std::shared_ptr<NWebFileSelectorParams> params)
    {
        return false;
    }

    virtual void OnScaleChanged(float oldScaleFactor, float newScaleFactor) {}

    virtual bool RunContextMenu(
        std::shared_ptr<NWebContextMenuParams> params, std::shared_ptr<NWebContextMenuCallback> callback)
    {
        return false;
    }

    virtual void OnContextMenuDismissed() {}

    virtual bool RunQuickMenu(
        std::shared_ptr<NWebQuickMenuParams> params, std::shared_ptr<NWebQuickMenuCallback> callback)
    {
        return false;
    }

    virtual void OnQuickMenuDismissed() {}

    virtual void OnTouchSelectionChanged(std::shared_ptr<NWebTouchHandleState> insertHandle,
        std::shared_ptr<NWebTouchHandleState> startSelectionHandle,
        std::shared_ptr<NWebTouchHandleState> endSelectionHandle)
    {}

    virtual bool OnHttpAuthRequestByJS(
        std::shared_ptr<NWebJSHttpAuthResult> result, const std::string& host, const std::string& realm)
    {
        return false;
    }

    virtual void OnScroll(double xOffset, double yOffset) {}

    virtual bool OnDragAndDropData(const void* data, size_t len, std::shared_ptr<NWebImageOptions> opt)
    {
        return false;
    }

    virtual bool OnSslErrorRequestByJS(std::shared_ptr<NWebJSSslErrorResult> result, SslError error)
    {
        return false;
    }

    virtual bool OnSslSelectCertRequestByJS(std::shared_ptr<NWebJSSslSelectCertResult> result, const std::string& host,
        int port, const std::vector<std::string>& keyTypes, const std::vector<std::string>& issuers)
    {
        return false;
    }

    /**
     * @brief called when the page enter the full-screen mode.
     * @param handler to exit full-screen mode.
     */
    virtual void OnFullScreenEnter(std::shared_ptr<NWebFullScreenExitHandler> handler) {}

    /**
     * @brief called when the page exit the full-screen mode.
     */
    virtual void OnFullScreenExit() {}

    /**
     * @brief called when new window required.
     *
     * @param targetUrl URL to be loaded in the new window.
     * @param isAlert dialog window or not.
     * @param isUserTrigger triggered by User.
     * @param handler set the new web object.
     */
    virtual void OnWindowNewByJS(
        const std::string& targetUrl, bool isAlert, bool isUserTrigger, std::shared_ptr<NWebControllerHandler> handler)
    {}

    /**
     * @brief called when window exit.
     */
    virtual void OnWindowExitByJS() {}

    /**
     * @brief called when the page being loaded is about to be made visible.
     */
    virtual void OnPageVisible(const std::string& url) {}

    /**
     * @brief shows the repost form confirmation dialog box.
     * @param handler sets whether to resend data.
     */
    virtual void OnDataResubmission(std::shared_ptr<NWebDataResubmissionCallback> handler) {}

    /**
     * @brief Give the host application a chance to handle the key event synchronousl.
     * @param event The key event.
     * @return True if the host application wants to handle the key event itself, otherwise return false.
     */
    virtual bool OnPreKeyEvent(std::shared_ptr<NWebKeyEvent> event)
    {
        return false;
    }

    /**
     * @brief Notify the host application that a key was not handled by the WebView.
     * @param event The key event.
     * @return True if the host application wants to handle the key event itself, otherwise return false.
     */
    virtual bool OnUnProcessedKeyEvent(std::shared_ptr<NWebKeyEvent> event)
    {
        return false;
    }

    /**
     * @brief Called when the browser's cursor has changed.
     * @param type Cursor type.
     * @param info If |type| is CT_CUSTOM then |info| will be populated with the custom cursor information.
     * @return True if the cursor change was handled or false for default handling.
     */
    virtual bool OnCursorChange(const CursorType& type, std::shared_ptr<NWebCursorInfo> info)
    {
        return false;
    }

    virtual void OnSelectPopupMenu(
        std::shared_ptr<NWebSelectPopupMenuParam> params, std::shared_ptr<NWebSelectPopupMenuCallback> callback)
    {}

    /**
     * @brief Called when the audio playing state on web page changed.
     * @param playing Whether the audio is playing or not.
     */
    virtual void OnAudioStateChanged(bool playing) {}

    /**
     * @brief Called when the first content rendering of web page.
     * @param navigationStartTick Absolute navigation start time, as TimeTicks.
     * @param firstContentfulPaintMs Time to first contentful paint from
     * navigation start.
     */
    virtual void OnFirstContentfulPaint(int64_t navigationStartTick, int64_t firstContentfulPaintMs) {}

    /**
     * @brief Called when the first meaningful paint rendering of web page.
     * @param details represents the details of first meaningful paint.
     */
    virtual void OnFirstMeaningfulPaint(std::shared_ptr<NWebFirstMeaningfulPaintDetails> details) {}

    /**
     * @brief Called when the largest contentful paint rendering of web page.
     * @param details represents the details of largest contentful paint.
     */
    virtual void OnLargestContentfulPaint(std::shared_ptr<NWebLargestContentfulPaintDetails> details) {}

    /**
     * @brief Called when swap buffer completed with new size.
     */
    virtual void OnCompleteSwapWithNewSize() {}

    /**
     * @brief Called when resize not work.
     */
    virtual void OnResizeNotWork() {}

    virtual void OnGetTouchHandleHotZone(std::shared_ptr<NWebTouchHandleHotZone> hotZone) {}

    virtual void OnDateTimeChooserPopup(std::shared_ptr<NWebDateTimeChooser> chooser,
        const std::vector<std::shared_ptr<NWebDateTimeSuggestion>>& suggestions,
        std::shared_ptr<NWebDateTimeChooserCallback> callback)
    {}

    virtual void OnDateTimeChooserClose() {}

    virtual bool OnDragAndDropDataUdmf(std::shared_ptr<NWebDragData> dragData)
    {
        return false;
    }

    virtual void UpdateDragCursor(NWebDragData::DragOperation op) {}

    virtual void OnOverScroll(float xOffset, float yOffset) {}

    /**
     * @brief Ask for the screen capture permission.
     *
     * @param request std::shared_ptr<NWebScreenCaptureAccessRequest>: A request to ask for the
     * screen capture permission.
     */
    virtual void OnScreenCaptureRequest(std::shared_ptr<NWebScreenCaptureAccessRequest> request) {}

    virtual void OnOverScrollFlingVelocity(float xVelocity, float yVelocity, bool isFling) {}

    virtual void OnOverScrollFlingEnd() {}

    /**
     * @brief Called when the media or form state on the web page changed.
     * @param state state of the media or form. Refer to the enum class MediaPlayingState and FormState
     * @param ActivityType it can be form, media, or audio
     */
    virtual void OnActivityStateChanged(int state, ActivityType type) {}

    virtual void OnScrollState(bool scrollState) {}

    virtual void OnRootLayerChanged(int width, int height) {}

    virtual bool FilterScrollEvent(const float x, const float y, const float xVelocity, const float yVelocity)
    {
        return false;
    }

    /**
     * @brief Called when received website security risk check result.
     * @param threat_type The threat type of website.
     */
    virtual void OnSafeBrowsingCheckResult(int threat_type) {}

    /**
     * @brief called when the navigation entry has been committed.
     * @param details represents the details of a committed navigation entry.
     */
    virtual void OnNavigationEntryCommitted(std::shared_ptr<NWebLoadCommittedDetails> details) {}

    virtual void OnNativeEmbedLifecycleChange(std::shared_ptr<NWebNativeEmbedDataInfo> dataInfo) {}

    virtual void OnNativeEmbedGestureEvent(std::shared_ptr<NWebNativeEmbedTouchEvent> event) {}

    /**
     * @brief called when the page enter the full-screen mode.
     *
     * @param handler to exit full-screen mode.
     * @param video_natural_width indicates the width of the <video> element
     * entering full screen.
     * @param video_natural_height indicates the height of the <video> element
     * entering full screen.
     */
    virtual void OnFullScreenEnterWithVideoSize(
        std::shared_ptr<NWebFullScreenExitHandler> handler, int video_natural_width, int video_natural_height)
    {}

    /**
     * @brief Called when tracker's cookie is prevented.
     * @param website_host The host of website url.
     * @param tracker_host The host of tracker url.
     */
    virtual void OnIntelligentTrackingPreventionResult(const std::string& websiteHost, const std::string& trackerHost)
    {}

    /**
     * @brief Give the application a chance to decide whether to override loading the
     * url.
     *
     * @param request The request information.
     * @return true to abort loading the url, false to continue loading the url
     * as usual.
     */
    virtual bool OnHandleOverrideUrlLoading(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request)
    {
        return false;
    }

    virtual bool OnAllSslErrorRequestByJS(std::shared_ptr<NWebJSAllSslErrorResult> result, SslError error,
        const std::string& url, const std::string& originalUrl, const std::string& referrer, bool isFatalError,
        bool isMainFrame)
    {
        return false;
    }

    /**
     * @brief Called when a tooltip should be presented for a component.
     *
     * @param tooltip The content of the tooltip.
     */
    virtual void OnTooltip(const std::string& param) {}

    /**
     * @brief called when resizehold is released.
     */
    virtual void ReleaseResizeHold() {}

    /**
     * @brief Called when select a word.
     *
     * @param text The content of the text.
     * @param offset The offset of the point.
     */
    virtual std::vector<int8_t> GetWordSelection(const std::string& text, int8_t offset)
    {
        return { -1, -1 };
    }

    virtual void UpdateClippedSelectionBounds(int x, int y, int w, int h) {}

    virtual bool OnOpenAppLink(const std::string& url, std::shared_ptr<NWebAppLinkCallback> callback)
    {
        return false;
    }

    /**
     * @brief Called when the render process not responding.
     *
     * @param js_stack Javascript stack info of webpage when render process not
     * responding.
     * @param pid Process id of the render process not responding.
     * @param reason Reason of the render process not responding.
     */
    virtual void OnRenderProcessNotResponding(
        const std::string& js_stack, int pid, RenderProcessNotRespondingReason reason)
    {}

    /**
     * @brief Called when the unresponding render process becomes responsive.
     *
     */
    virtual void OnRenderProcessResponding() {}

    virtual void OnShowAutofillPopup(
        const float offsetX, const float offsetY, const std::vector<std::string>& menu_items)
    {}

    virtual void OnHideAutofillPopup() {}

    /**
     * @brief Called when the viewport-fit meta is detected for web page.
     *
     * @param viewportFit The type of the viewport-fit.
     */
    virtual void OnViewportFitChange(ViewportFit viewportFit) {}

    /**
     * @brief Request display and focus for a new nweb.
     *
     * @param source The Focus Source.
     * @return Return true if request focus success, false if request focus fail.
     */
    virtual bool OnFocus(NWebFocusSource source)
    {
        return false;
    }

    /**
     * @brief Called when the page is over scroll.
     *
     * @param xOffset The offset of x axis.
     * @param yOffset The offset of y axis.
     * @param xVelocity The velocity of x axis.
     * @param yVelocity The velocity of y axis.
     * @return Return true if value is consumed, false if value is unconsumed.
     */
    virtual bool OnOverScroll(float xOffset, float yOffset, float xVelocity, float yVelocity)
    {
        return false;
    }

    /**
     * @brief called when creating overlay.
     */
    virtual void CreateOverlay(void* data, size_t len, int width, int height, int offsetX, int offsetY, int rectWidth,
        int rectHeight, int pointX, int pointY)
    {}

    /**
     * @brief called when state changed.
     */
    virtual void OnOverlayStateChanged(int offsetX, int offsetY, int rectWidth, int rectHeight) {}

    /**
     * @brief Called when the key board redispatch.
     *
     * @param event Key information.
     * @param isUsed Whether the key is used by the kernel.
     */
    virtual void KeyboardReDispatch(std::shared_ptr<NWebKeyEvent> event, bool isUsed) {}

    /**
     * @brief Called when received Ads blocked results.
     *
     * @param url The url of webpage.
     * @param adsBlocked The ads' blocked urls.
     *
     */
    virtual void OnAdsBlocked(const std::string& url, const std::vector<std::string>& adsBlocked) {}

    virtual void OnInterceptKeyboardAttach(
        const std::shared_ptr<NWebCustomKeyboardHandler> keyboardHandler,
        const std::map<std::string, std::string> &attributes, bool &useSystemKeyboard, int32_t &enterKeyType) {
        useSystemKeyboard = true;
        enterKeyType = -1;
    }

    virtual void OnCustomKeyboardAttach() {}

    virtual void OnCustomKeyboardClose() {}

    /**
     * @brief Called when you need to temporarily hide/restore the handle menu.
     *
     * @param hide hide.
     */
    virtual void HideHandleAndQuickMenuIfNecessary(bool hide) {}

    /**
     * @brief called when the cursor info is updated.
     *
     * @param x, y relative coordinates within web components of the cursor
     * @param width, height width and height of the cursor
     */
    virtual void OnCursorUpdate(double x, double y, double width, double height) {}

    /**
     * @brief Called When you click on the selected area.
     *
     */
    virtual void ChangeVisibilityOfQuickMenu() {}

    /**
     * @brief Called when you need to start vibrator.
     */
    virtual void StartVibraFeedback(const std::string& vibratorType) {}

    virtual void OnNativeEmbedVisibilityChange(const std::string& embed_id, bool visibility) {}

    virtual bool CloseImageOverlaySelection() { return false; }

    /**
     * @brief Called when a popup is shown with the given size.
     *
     * @param x The offset of the popup on the x coordinate axis.
     * @param y The offset of the popup on the y coordinate axis.
     * @param width The width of the popup.
     * @param height The height of the popup.
     *
     */
    virtual void OnPopupSize(int x, int y, int width, int height) {}

    /**
     * @brief Called when the popup is shown or hidden.
     *
     * @param show Whether the popup is shown or hidden.
     *
     */
    virtual void OnPopupShow(bool show) {}

    virtual void OnAccessibilityEvent(int64_t accessibilityId, int32_t eventType) {}

    /**
     * @brief Called when web occurs frame loss event.
     *
     * @param sceneId The id of event scene.
     * @param isStart True if is start.
     */
    virtual void ReportDynamicFrameLossEvent(const std::string& sceneId, bool isStart) {}

    virtual bool IsCurrentFocus() { return false; }
    
    virtual bool OnSslErrorRequestByJSV2(std::shared_ptr<NWebJSSslErrorResult> result, SslError error,
        const std::vector<std::string>& certChainData)
    {
        return false;
    }

    /**
     * @brief Get the visible area relative to the web.
     */
    virtual void GetVisibleRectToWeb(int& visibleX, int& visibleY, int& visibleWidth, int& visibleHeight) {}

    virtual void OnScrollStart(const float x, const float y) {}

    virtual void OnShowAutofillPopupV2(
        const float offsetX, const float offsetY, const float height, const float width,
        const std::vector<std::string>& menu_items)
    {}

    /**
     * @brief Restore web component renderfit.
     */
    virtual void RestoreRenderFit() {}

    virtual void OnAccessibilityEventV2(int64_t accessibilityId, int32_t eventType, const std::string& argument) {}

    virtual bool OnNestedScroll(float& x, float& y, float& xVelocity, float& yVelocity, bool& isAvailable)
    {
        return false;
    }

    /**
     * @brief Called When you click on the selected area.
     */
    virtual bool ChangeVisibilityOfQuickMenuV2() { return false; }

    /**
     * @brief called when the web page is active for window.open called by other web component.
     */
    virtual void OnActivateContentByJS() {}

    /**
     * @brief Notify the SDK that a web site has started loading. This method is
     * called once for each main frame load. Embedded frame changes, i.e. clicking
     * a link whose target is an iframe and fragment navigations (navigations to
     * #fragment_id) will not trigger this callback.
     *
     * @param url The url to be loaded.
     */
   virtual void OnLoadStarted(const std::string& url) {}

    /**
     * @brief Notify the SDK that a web site has finished loading. This method is
     * called only for main frame. Different from onPageEnd, onLoadFinished is
     * triggered only once if the mainframe is automatically redirected before the
     * page is completely loaded. OnPageEnd is triggered every navigation.
     * fragment navigation also triggers onLoadFinished.
     *
     * @param url The url of the web site.
     */
    virtual void OnLoadFinished(const std::string& url) {}

    /**
     * @brief Notify the host application that the web page wants to handle
     * JavaScript onbeforeunload.
     *
     * @param url  String: The url of the page requesting.
     * @param message  String: The message of the dialog.
     * @param isReload bool: The isReload parameter is set to true when the page is refreshed;
     *        otherwise, it remains false. Default is false.
     * @param result  std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     *        confirm that the user closed the window.
     * @return To show a custom dialog, the app should return true.
     */
    virtual bool OnBeforeUnloadByJSV2(
        const std::string& url, const std::string& message, bool isReload, std::shared_ptr<NWebJSDialogResult> result)
    {
        return false;
    }

    /**
    * @Description: Called when an mouse native event occurs on native embed area.
     * @Input mouse_event: Mouse events that contain information about the same layer.
     */
    virtual void OnNativeEmbedMouseEvent(std::shared_ptr<NWebNativeEmbedMouseEvent> event) {}

    /**
     * @brief Called when web occurs event of picture in picture.
     */
    virtual void OnPip(int status,
                       int delegate_id,
                       int child_id,
                       int frame_routing_id,
                       int width,
                       int height) {}
    
    virtual void EnableSecurityLayer(bool isNeedSecurityLayer) {}

    virtual bool OnAllSslErrorRequestByJSV2(std::shared_ptr<NWebJSAllSslErrorResult> result, SslError error,
        const std::string& url, const std::string& originalUrl, const std::string& referrer, bool isFatalError,
        bool isMainFrame, const std::vector<std::string>& certChainData)
    {
        return false;
    }

    /**
     * @brief Notify the web client to insert blankless frame.
     *
     * @param pathToFrame The file used to insert frame.
     */
    virtual void OnInsertBlanklessFrame(const std::string& pathToFrame) {}

    /**
     * @brief Notify the web client to remove blankless frame.
     *
     * @param delayTime The delayTime for web client to remove blankless frame.
     */
    virtual void OnRemoveBlanklessFrame(int delayTime) {}

    /**
     * @brief Called when you need to show magnifier.
     */
    virtual void ShowMagnifier() {}

    /**
     * @brief Called when you need to hide magnifier.
     */
    virtual void HideMagnifier() {}

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     * @param isRealTitle Mark the source of the title. If it is true, the title is derived from the H5 title element;
     *        If it is false, it is calculated from the URL. By default, it is calculated from the URL.
     */
    virtual void OnPageTitleV2(const std::string& title, bool isRealTitle) {}

    /**
     * @brief Triggered when the web page's document resource error
     *
     * @param request The request information.
     * @param error The error information.
     */
    virtual std::string OnHandleOverrideErrorPage(
        std::shared_ptr<NWebUrlResourceRequest> request,
        std::shared_ptr<NWebUrlResourceError> error)
    {
        return "";
    }

    /**
     * @brief Called when web occurs event of pdf scroll at bottom.
     *
     * @param url The url of the pdf.
     */
    virtual void OnPdfScrollAtBottom(const std::string& url) {}

    /**
     * @brief Called when web occurs event of pdf load.
     *
     * @param result The result of the pdf load.
     * @param url The url of the pdf.
     */
    virtual void OnPdfLoadEvent(int32_t result, const std::string& url) {}

    virtual void OnTakeFocus(std::shared_ptr<NWebKeyEvent> event) {}

    /**
     * @brief Notify the web client to insert blankless frame with size.
     *
     * @param pathToFrame The file used to insert frame.
     * @param width The width of the blankless frame.
     * @param height The height of the blankless frame.
     */
    virtual void OnInsertBlanklessFrameWithSize(const std::string& pathToFrame, uint32_t width, uint32_t height) {}

    /**
     * @brief Notify the web client to set the soft keyboard panel status.
     *
     * @param visible the soft keyboard panel status. If true, it represents the keyboard display;
     *        If it is false, it indicates that the keyboard is hidden.
     *
     */
    virtual void SetImeShow(bool visible) {}

    /**
     * @brief Called when you click on caret.
     */
    virtual bool IsShowHandle() { return false; }

    /**
     * @brief Called when params of the native object are changed.
     *
     * @param paramDataInfo The information containing the set of all changed params for the native object.
     *
     */
    virtual void OnNativeEmbedObjectParamChange(std::shared_ptr<NWebNativeEmbedParamDataInfo> paramDataInfo) {}
};

} // namespace OHOS::NWeb

#endif // NWEB_HANDLER_H
