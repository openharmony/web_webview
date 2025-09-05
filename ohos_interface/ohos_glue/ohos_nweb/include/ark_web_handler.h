/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_HANDLER_H_
#define ARK_WEB_HANDLER_H_
#pragma once

#include "ohos_nweb/include/ark_web_access_request.h"
#include "ohos_nweb/include/ark_web_applink_callback.h"
#include "ohos_nweb/include/ark_web_console_log.h"
#include "ohos_nweb/include/ark_web_context_menu_callback.h"
#include "ohos_nweb/include/ark_web_context_menu_params.h"
#include "ohos_nweb/include/ark_web_controller_handler.h"
#include "ohos_nweb/include/ark_web_cursor_info.h"
#include "ohos_nweb/include/ark_web_custom_keyboard_handler.h"
#include "ohos_nweb/include/ark_web_data_resubmission_callback.h"
#include "ohos_nweb/include/ark_web_date_time_chooser.h"
#include "ohos_nweb/include/ark_web_date_time_chooser_callback.h"
#include "ohos_nweb/include/ark_web_date_time_suggestion_vector.h"
#include "ohos_nweb/include/ark_web_file_selector_params.h"
#include "ohos_nweb/include/ark_web_first_meaningful_paint_details.h"
#include "ohos_nweb/include/ark_web_full_screen_exit_handler.h"
#include "ohos_nweb/include/ark_web_geo_location_callback.h"
#include "ohos_nweb/include/ark_web_image_options.h"
#include "ohos_nweb/include/ark_web_js_all_ssl_error_result.h"
#include "ohos_nweb/include/ark_web_js_dialog_result.h"
#include "ohos_nweb/include/ark_web_js_http_auth_result.h"
#include "ohos_nweb/include/ark_web_js_ssl_error_result.h"
#include "ohos_nweb/include/ark_web_js_ssl_select_cert_result.h"
#include "ohos_nweb/include/ark_web_key_event.h"
#include "ohos_nweb/include/ark_web_largest_contentful_paint_details.h"
#include "ohos_nweb/include/ark_web_load_committed_details.h"
#include "ohos_nweb/include/ark_web_native_embed_data_info.h"
#include "ohos_nweb/include/ark_web_native_embed_touch_event.h"
#include "ohos_nweb/include/ark_web_native_embed_mouse_event.h"
#include "ohos_nweb/include/ark_web_native_embed_param_data_info.h"
#include "ohos_nweb/include/ark_web_nweb.h"
#include "ohos_nweb/include/ark_web_quick_menu_callback.h"
#include "ohos_nweb/include/ark_web_quick_menu_params.h"
#include "ohos_nweb/include/ark_web_screen_capture_access_request.h"
#include "ohos_nweb/include/ark_web_select_popup_menu_callback.h"
#include "ohos_nweb/include/ark_web_select_popup_menu_param.h"
#include "ohos_nweb/include/ark_web_string_vector_value_callback.h"
#include "ohos_nweb/include/ark_web_touch_handle_hot_zone.h"
#include "ohos_nweb/include/ark_web_touch_handle_state.h"
#include "ohos_nweb/include/ark_web_url_resource_error.h"
#include "ohos_nweb/include/ark_web_url_resource_request.h"
#include "ohos_nweb/include/ark_web_url_resource_response.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebHandler : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void SetNWeb(ArkWebRefPtr<ArkWebNWeb> nweb) = 0;

    /**
     * @brief Request display and focus for a new nweb.
     *
     */
    /*--ark web()--*/
    virtual bool OnFocus() = 0;

    /*--ark web()--*/
    virtual void OnMessage(const ArkWebString& param) = 0;

    /**
     * @brief Notify the SDK that the nweb will load the resource specified by the
     *        given url.
     *
     * @param url The url of the resource.
     */
    /*--ark web()--*/
    virtual void OnResource(const ArkWebString& url) = 0;

    /**
     * @brief Notify the SDK of a new favicon for the current web site.
     *
     * @param data The raw image data for the icon.
     * @param width The width of the icon in pixel.
     * @param height The height of the icon in pixel.
     * @param color_type The color data encoding type.
     * @param alpha_type The alpha value of any pixel.
     */
    /*--ark web()--*/
    virtual void OnPageIcon(const void* data, size_t width, size_t height, int color_type, int alpha_type) = 0;

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     */
    /*--ark web()--*/
    virtual void OnPageTitle(const ArkWebString& title) = 0;

    /*--ark web()--*/
    virtual void OnProxyDied() = 0;

    /**
     * @brief Notify the SDK that an HTTP error has been received from the server
     *        while loading a resource.
     *
     * @param request The request information.
     * @param error_response The error occurred.
     */
    /*--ark web()--*/
    virtual void OnHttpError(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceResponse> error_response) = 0;

    /**
     * @brief Report a JavaScript console message to the host application.
     *
     * @param message Details of the console message.
     *
     * @return Return true to stop the message from being output to the console.
     */
    /*--ark web()--*/
    virtual bool OnConsoleLog(ArkWebRefPtr<ArkWebConsoleLog> message) = 0;

    /*--ark web()--*/
    virtual void OnRouterPush(const ArkWebString& param) = 0;

    /**
     * @brief Notify the SDK that a web site has finished loading. This method is
     *        called only for main frame.
     *
     * @param http_status_code The status code for the http request.
     * @param url The url of the web site.
     */
    /*--ark web()--*/
    virtual void OnPageLoadEnd(int http_status_code, const ArkWebString& url) = 0;

    /**
     * @brief Notify the SDK that a web site has started loading. This method is
     *        called once for each main frame load.
     *
     * @param url The url to be loaded.
     */
    /*--ark web()--*/
    virtual void OnPageLoadBegin(const ArkWebString& url) = 0;

    /**
     * @brief Report a load error to the SDK.
     *
     * @param error_code The error code.
     * @param description The error description.
     * @param failing_url The failed url.
     */
    /*--ark web()--*/
    virtual void OnPageLoadError(int error_code, const ArkWebString& description, const ArkWebString& failing_url) = 0;

    /**
     * @brief Notify the SDK of the url for a touch icon.
     *
     * @param icon_url The icon url.
     * @param precomposed The touch icon type.
     */
    /*--ark web()--*/
    virtual void OnDesktopIconUrl(const ArkWebString& icon_url, bool precomposed) = 0;

    /**
     * @brief Notify the SDK the current progress of loading a web site.
     *
     * @param new_progress Loading progress, an integer between 0 and 100.
     */
    /*--ark web()--*/
    virtual void OnLoadingProgress(int new_progress) = 0;

    /**
     * @brief Show prompt to ask for the geo location permission.
     *
     * @param origin String: the origin of the resource to get geo location
     * @param callback GeolocationCallbackInterface: callback to report geo
     *        location
     */
    /*--ark web()--*/
    virtual void OnGeolocationShow(const ArkWebString& origin, ArkWebRefPtr<ArkWebGeoLocationCallback> callback) = 0;

    /**
     * @brief Hide prompt to ask for the geolocation permission.
     */
    /*--ark web()--*/
    virtual void OnGeolocationHide() = 0;

    /**
     * @brief inform application to show a file selector.
     *
     * @param callback the file list to select.
     * @param params the params of file selector.
     */
    /*--ark web()--*/
    virtual bool OnFileSelectorShow(
        ArkWebRefPtr<ArkWebStringVectorValueCallback> callback, ArkWebRefPtr<ArkWebFileSelectorParams> params) = 0;

    /**
     * @brief Report web resource loading error to the SDK. These errors usually
     *        indicate inability to connect to the server.
     *
     * @param request The request information.
     * @param error The error information.
     */
    /*--ark web()--*/
    virtual void OnResourceLoadError(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceError> error) = 0;

    /**
     * @brief Ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     *        permission.
     */
    /*--ark web()--*/
    virtual void OnPermissionRequest(ArkWebRefPtr<ArkWebAccessRequest> request) = 0;

    /*--ark web()--*/
    virtual void OnQuickMenuDismissed() = 0;

    /*--ark web()--*/
    virtual void OnContextMenuDismissed() = 0;

    /*--ark web()--*/
    virtual void OnTouchSelectionChanged(ArkWebRefPtr<ArkWebTouchHandleState> insert_handle,
        ArkWebRefPtr<ArkWebTouchHandleState> start_selection_handle,
        ArkWebRefPtr<ArkWebTouchHandleState> end_selection_handle) = 0;

    /**
     * @brief Notify the host application of a resource request and allow the
     *        application to return the data.
     *
     * @param request request information
     * @param response response information
     *
     * @retval true if handle success, otherwise false.
     */
    /*--ark web()--*/
    virtual bool OnHandleInterceptRequest(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceResponse> response) = 0;

    /**
     * @brief inform application to update its visited links database.
     *
     * @param url the url being visited.
     * @param is_reload true if the url is being reload.
     */
    /*--ark web()--*/
    virtual void OnRefreshAccessedHistory(const ArkWebString& url, bool is_reload) = 0;

    /**
     * @brief Cancel the request to ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     *        permission.
     */
    /*--ark web()--*/
    virtual void OnPermissionRequestCanceled(ArkWebRefPtr<ArkWebAccessRequest> request) = 0;

    /**
     * @brief Give the SDK a chance to decide whether to continue loading the url.
     *
     * @param url The url to be loaded.
     *
     * @return true to cancel the loading, false to continue the loading.
     */
    /*--ark web()--*/
    virtual bool OnHandleInterceptUrlLoading(ArkWebRefPtr<ArkWebUrlResourceRequest> request) = 0;

    /*--ark web()--*/
    virtual bool RunQuickMenu(
        ArkWebRefPtr<ArkWebQuickMenuParams> params, ArkWebRefPtr<ArkWebQuickMenuCallback> callback) = 0;

    /*--ark web()--*/
    virtual bool RunContextMenu(
        ArkWebRefPtr<ArkWebContextMenuParams> params, ArkWebRefPtr<ArkWebContextMenuCallback> callback) = 0;

    /*--ark web()--*/
    virtual void UpdateDragCursor(unsigned char op) = 0;

    /*--ark web()--*/
    virtual bool FilterScrollEvent(const float x, const float y, const float x_velocity, const float y_velocity) = 0;

    /**
     * @brief Obtains a list of all visited history items, used for link coloring
     *
     * @return visited history
     */
    /*--ark web()--*/
    virtual ArkWebStringVector VisitedUrlHistory() = 0;

    /**
     * @brief called when new window required.
     *
     * @param target_url URL to be loaded in the new window.
     * @param is_alert dialog window or not.
     * @param is_user_trigger triggered by User.
     * @param handler set the new web object.
     */
    /*--ark web()--*/
    virtual void OnWindowNewByJS(const ArkWebString& target_url, bool is_alert, bool is_user_trigger,
        ArkWebRefPtr<ArkWebControllerHandler> handler) = 0;

    /**
     * @brief called when window exit.
     */
    /*--ark web()--*/
    virtual void OnWindowExitByJS() = 0;

    /**
     * @brief Notify the host application that the web page wants to display a
     *        JavaScript alert() dialog.
     *
     * @param url String: The url of the page requesting the dialog.
     * @param message String: The message of the dialog.
     * @param result std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     *        confirm that the user closed the window.
     *
     * @return To show a custom dialog, the app should return true.
     */
    /*--ark web()--*/
    virtual bool OnAlertDialogByJS(
        const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result) = 0;

    /**
     * @brief Notify the host application that the web page wants to handle
     *        JavaScript onbeforeunload.
     *
     * @param url String: The url of the page requesting.
     * @param message String: The message of the dialog.
     * @param result std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     *        confirm that the user closed the window.
     *
     * @return To show a custom dialog, the app should return true.
     */
    /*--ark web()--*/
    virtual bool OnBeforeUnloadByJS(
        const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result) = 0;

    /**
     * @brief Notify the host application that the web page wants to display a
     *        JavaScript prompt() dialog.
     *
     * @param url String: The url of the page requesting the dialog.
     * @param message String: The message of the dialog.
     * @param default_value String: The default value of the input message.
     * @param result std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     *        confirm that the user closed the window.
     *
     * @return To show a custom dialog, the app should return true.
     */
    /*--ark web()--*/
    virtual bool OnPromptDialogByJS(const ArkWebString& url, const ArkWebString& message,
        const ArkWebString& default_value, ArkWebRefPtr<ArkWebJsDialogResult> result) = 0;

    /**
     * @brief Notify the host application that the web page wants to display a
     *        JavaScript Confirm() dialog.
     *
     * @param url String: The url of the page requesting the dialog.
     * @param message String: The message of the dialog.
     * @param result std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     *        confirm that the user closed the window.
     *
     * @return To show a custom dialog, the app should return true.
     */
    /*--ark web()--*/
    virtual bool OnConfirmDialogByJS(
        const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result) = 0;

    /*--ark web()--*/
    virtual bool OnHttpAuthRequestByJS(
        ArkWebRefPtr<ArkWebJsHttpAuthResult> result, const ArkWebString& host, const ArkWebString& realm) = 0;

    /*--ark web()--*/
    virtual bool OnSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error) = 0;

    /*--ark web()--*/
    virtual bool OnSslSelectCertRequestByJS(ArkWebRefPtr<ArkWebJsSslSelectCertResult> result, const ArkWebString& host,
        int port, const ArkWebStringVector& key_types, const ArkWebStringVector& issuers) = 0;

    /*--ark web()--*/
    virtual void OnScroll(double x_offset, double y_offset) = 0;

    /*--ark web()--*/
    virtual void OnOverScroll(float x_offset, float y_offset) = 0;

    /*--ark web()--*/
    virtual void OnScrollState(bool scroll_state) = 0;

    /**
     * @brief called when the page being loaded is about to be made visible.
     */
    /*--ark web()--*/
    virtual void OnPageVisible(const ArkWebString& url) = 0;

    /**
     * @brief Give the host application a chance to handle the key event
     *        synchronousl.
     *
     * @param event The key event.
     *
     * @return True if the host application wants to handle the key event itself,
     *         otherwise return false.
     */
    /*--ark web()--*/
    virtual bool OnPreKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) = 0;

    /*--ark web()--*/
    virtual void OnScaleChanged(float old_scale_factor, float new_scale_factor) = 0;

    /**
     * @brief Called when the browser's cursor has changed.
     *
     * @param type Cursor type.
     * @param info If |type| is CT_CUSTOM then |info| will be populated with the
     *        custom cursor information.
     *
     * @return True if the cursor change was handled or false for default
     *         handling.
     */
    /*--ark web()--*/
    virtual bool OnCursorChange(const int32_t& type, ArkWebRefPtr<ArkWebCursorInfo> info) = 0;

    /**
     * @brief called when the render process exit.
     *
     * @param reason the detail reason why render process exit, the implementation
     *        of this callback should attempt to clean up the specific nweb that
     *        was set by SetNWeb interface.
     */
    /*--ark web()--*/
    virtual void OnRenderExited(int reason) = 0;

    /**
     * @brief Called when resize not work.
     */
    /*--ark web()--*/
    virtual void OnResizeNotWork() = 0;

    /**
     * @brief called when the page exit the full-screen mode.
     */
    /*--ark web()--*/
    virtual void OnFullScreenExit() = 0;

    /**
     * @brief called when the page enter the full-screen mode.
     *
     * @param handler to exit full-screen mode.
     */
    /*--ark web()--*/
    virtual void OnFullScreenEnter(ArkWebRefPtr<ArkWebFullScreenExitHandler> handler) = 0;

    /*--ark web()--*/
    virtual bool OnDragAndDropData(const void* data, size_t len, ArkWebRefPtr<ArkWebImageOptions> opt) = 0;

    /*--ark web()--*/
    virtual void OnSelectPopupMenu(
        ArkWebRefPtr<ArkWebSelectPopupMenuParam> params, ArkWebRefPtr<ArkWebSelectPopupMenuCallback> callback) = 0;

    /**
     * @brief shows the repost form confirmation dialog box.
     *
     * @param handler sets whether to resend data.
     */
    /*--ark web()--*/
    virtual void OnDataResubmission(ArkWebRefPtr<ArkWebDataResubmissionCallback> handler) = 0;

    /*--ark web()--*/
    virtual void OnRootLayerChanged(int width, int height) = 0;

    /**
     * @brief Called when the audio playing state on web page changed.
     *
     * @param playing Whether the audio is playing or not.
     */
    /*--ark web()--*/
    virtual void OnAudioStateChanged(bool playing) = 0;

    /*--ark web()--*/
    virtual void OnOverScrollFlingEnd() = 0;

    /**
     * @brief Notify the host application that a key was not handled by the
     *        WebView.
     *
     * @param event The key event.
     *
     * @return True if the host application wants to handle the key event itself,
     *         otherwise return false.
     */
    /*--ark web()--*/
    virtual bool OnUnProcessedKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) = 0;

    /*--ark web()--*/
    virtual bool OnDragAndDropDataUdmf(ArkWebRefPtr<ArkWebDragData> drag_data) = 0;

    /**
     * @brief Called when the first content rendering of web page.
     *
     * @param navigation_start_tick Absolute navigation start time, as TimeTicks.
     * @param first_contentful_paint_ms Time to first contentful paint from
     *        navigation start.
     */
    /*--ark web()--*/
    virtual void OnFirstContentfulPaint(int64_t navigation_start_tick, int64_t first_contentful_paint_ms) = 0;

    /*--ark web()--*/
    virtual void OnDateTimeChooserPopup(ArkWebRefPtr<ArkWebDateTimeChooser> chooser,
        const ArkWebDateTimeSuggestionVector& suggestions, ArkWebRefPtr<ArkWebDateTimeChooserCallback> callback) = 0;

    /*--ark web()--*/
    virtual void OnDateTimeChooserClose() = 0;

    /**
     * @brief Ask for the screen capture permission.
     *
     * @param request std::shared_ptr<NWebScreenCaptureAccessRequest>: A request
     *        to ask for the screen capture permission.
     */
    /*--ark web()--*/
    virtual void OnScreenCaptureRequest(ArkWebRefPtr<ArkWebScreenCaptureAccessRequest> request) = 0;

    /**
     * @brief Called when the media or form state on the web page changed.
     *
     * @param state state of the media or form. Refer to the enum class
     *        MediaPlayingState and FormState
     * @param type it can be form, media, or audio
     */
    /*--ark web()--*/
    virtual void OnActivityStateChanged(int state, int type) = 0;

    /*--ark web()--*/
    virtual void OnGetTouchHandleHotZone(ArkWebRefPtr<ArkWebTouchHandleHotZone> hot_zone) = 0;

    /**
     * @brief Called when swap buffer completed with new size.
     */
    /*--ark web()--*/
    virtual void OnCompleteSwapWithNewSize() = 0;

    /*--ark web()--*/
    virtual void OnOverScrollFlingVelocity(float x_velocity, float y_velocity, bool is_fling) = 0;

    /**
     * @brief called when the navigation entry has been committed.
     *
     * @param details represents the details of a committed navigation entry.
     */
    /*--ark web()--*/
    virtual void OnNavigationEntryCommitted(ArkWebRefPtr<ArkWebLoadCommittedDetails> details) = 0;

    /*--ark web()--*/
    virtual void OnNativeEmbedLifecycleChange(ArkWebRefPtr<ArkWebNativeEmbedDataInfo> data_info) = 0;

    /*--ark web()--*/
    virtual void OnNativeEmbedGestureEvent(ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> touch_event) = 0;

    /**
     * @brief Called when received website security risk check result.
     *
     * @param threat_type The threat type of website.
     */
    /*--ark web()--*/
    virtual void OnSafeBrowsingCheckResult(int threat_type) = 0;

    /**
     * @brief Called when tracker's cookie is prevented.
     *
     * @param website_host The host of website url.
     *
     * @param tracker_host The host of tracker url.
     */
    /*--ark web()--*/
    virtual void OnIntelligentTrackingPreventionResult(
        const ArkWebString& website_host, const ArkWebString& tracker_host) = 0;

    /**
     * @brief called when the page enter the full-screen mode.
     *
     * @param handler to exit full-screen mode.
     * @param video_natural_width indicates the width of the <video> element
     * entering full screen.
     * @param video_natural_height indicates the height of the <video> element
     * entering full screen.
     */
    /*--ark web()--*/
    virtual void OnFullScreenEnterWithVideoSize(
        ArkWebRefPtr<ArkWebFullScreenExitHandler> handler, int video_natural_width, int video_natural_height) = 0;

    /**
     * @brief Give the application a chance to decide whether to override loading the
     * url.
     *
     * @param request The request information.
     * @return true to abort loading the url, false to continue loading the url
     * as usual.
     */
    /*--ark web()--*/
    virtual bool OnHandleOverrideUrlLoading(ArkWebRefPtr<ArkWebUrlResourceRequest> request) = 0;

    /*--ark web()--*/
    virtual void OnFirstMeaningfulPaint(ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> details) = 0;

    /*--ark web()--*/
    virtual void OnLargestContentfulPaint(ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> details) = 0;

    /*--ark web()--*/
    virtual bool OnAllSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsAllSslErrorResult> result, int error,
        const ArkWebString& url, const ArkWebString& originalUrl, const ArkWebString& referrer, bool isFatalError,
        bool isMainFrame) = 0;

    /*--ark web()--*/
    virtual void OnTooltip(const ArkWebString& tooltip) = 0;

    /*--ark web()--*/
    virtual void ReleaseResizeHold() = 0;

    /*--ark web()--*/
    virtual void OnShowAutofillPopup(
        const float offsetX, const float offsetY, const ArkWebStringVector& menu_items) = 0;

    /*--ark web()--*/
    virtual void OnHideAutofillPopup() = 0;

    /*--ark web()--*/
    virtual ArkWebCharVector GetWordSelection(const ArkWebString& text, int8_t offset) = 0;

    /*--ark web()--*/
    virtual void UpdateClippedSelectionBounds(int x, int y, int w, int h) = 0;

    /*--ark web()--*/
    virtual bool OnOpenAppLink(const ArkWebString& url, ArkWebRefPtr<ArkWebAppLinkCallback> callback) = 0;

    /**
     * @brief Called when the render process not responding.
     *
     * @param js_stack Javascript stack info of webpage when render process not
     * responding.
     * @param pid Process id of the render process not responding.
     * @param reason Reason of the render process not responding.
     */
    /*--ark web()--*/
    virtual void OnRenderProcessNotResponding(const ArkWebString& js_stack, int pid, int reason) = 0;

    /**
     * @brief Called when the unresponding render process becomes responsive.
     *
     */
    /*--ark web()--*/
    virtual void OnRenderProcessResponding() = 0;

    /**
     * @brief Called when the viewport-fit meta is detected for web page.
     *
     * @param viewportFit The type of the viewport-fit.
     */
    /*--ark web()--*/
    virtual void OnViewportFitChange(int viewportFit) = 0;

    /**
     * @brief Called when creating overlay.
     *
     * @param data
     * @param len
     * @param width
     * @param height
     * @param offset_x
     * @param offset_y
     * @param rect_width
     * @param rect_height
     * @param point_x
     * @param point_y
     */
    /*--ark web()--*/
    virtual void CreateOverlay(void* data, size_t len, int width, int height, int offset_x, int offset_y,
        int rect_width, int rect_height, int point_x, int point_y) = 0;

    /**
     * @brief Called when overlay state changed.
     *
     * @param offset_x
     * @param offset_y
     * @param rect_width
     * @param rect_height
     */
    /*--ark web()--*/
    virtual void OnOverlayStateChanged(int offset_x, int offset_y, int rect_width, int rect_height) = 0;

    /**
     * @brief Request display and focus for a new nweb.
     *
     * @param source The Focus Source.
     * @return Return true if request focus success, false if request focus fail.
     */
    /*--ark web()--*/
    virtual bool OnFocus(int source) = 0;

    /**
     * @brief Called when the page is over scroll.
     *
     * @param xOffset The offset of x axis.
     * @param yOffset The offset of y axis.
     * @param xVelocity The velocity of x axis.
     * @param yVelocity The velocity of y axis.
     * @return Return true if value is consumed, false if value is unconsumed.
     */
    /*--ark web()--*/
    virtual bool OnOverScroll(float xOffset, float yOffset, float xVelocity, float yVelocity) = 0;

    /**
     * @brief Called when the key board redispatch.
     *
     * @param event Key information.
     * @param isUsed Whether the key is used by the kernel.
     */
    /*--ark web()--*/
    virtual void KeyboardReDispatch(ArkWebRefPtr<ArkWebKeyEvent> event, bool isUsed) = 0;

    /*--ark web()--*/
    virtual void OnInterceptKeyboardAttach(ArkWebRefPtr<ArkWebCustomKeyboardHandler> keyboardHandler,
        const ArkWebStringMap &attributes, bool &useSystemKeyboard, int32_t &enterKeyType) = 0;

    /*--ark web()--*/
    virtual void OnCustomKeyboardAttach() = 0;

    /*--ark web()--*/
    virtual void OnCustomKeyboardClose() = 0;
    /**
     * @brief Called when ads blocked.
     *
     * @param url the url of main frame.
     * @param adsBlocked the url of ads.
     */
    /*--ark web()--*/
    virtual void OnAdsBlocked(const ArkWebString &url, const ArkWebStringVector &adsBlocked) = 0;

    /**
     * @brief called when the cursor info is updated.
     *
     * @param x, y relative coordinates within web components of the cursor
     * @param width, height width and height of the cursor
     */
    /*--ark web()--*/
    virtual void OnCursorUpdate(double x, double y, double width, double height) = 0;

    /**
     * @brief Called when web occurs frame loss event.
     *
     * @param sceneId The id of event scene.
     * @param isStart True if is start.
     */
    /*--ark web()--*/
    virtual void ReportDynamicFrameLossEvent(const ArkWebString& sceneId, bool isStart) = 0;

    /**
     * @brief Called when you need to temporarily hide/restore the handle menu.
     *
     * @param hide hide.
     */
    /*--ark web()--*/
    virtual void HideHandleAndQuickMenuIfNecessary(bool hide) = 0;

    /**
     * @brief Called When you click on the selected area.
     *
     */
    /*--ark web()--*/
    virtual void ChangeVisibilityOfQuickMenu() = 0;

    /**
     * @brief Called when you need to start vibrator.
     */
    /*--ark web()--*/
    virtual void StartVibraFeedback(const ArkWebString& vibratorType) = 0;

    /**
     * @brief Called when a popup is shown with the given size.
     *
     * @param x The offset of the popup on the x coordinate axis.
     * @param y The offset of the popup on the y coordinate axis.
	 * @param width The width of the popup.
     * @param height The height of the popup.
     *
     */
    /*--ark web()--*/
    virtual void OnPopupSize(int x, int y, int width, int height) = 0;

    /**
     * @brief Called when the popup is shown or hidden.
     *
     * @param show Whether the popup is shown or hidden.
     *
     */
    /*--ark web()--*/
    virtual void OnPopupShow(bool show) = 0;

    /*--ark web()--*/
    virtual void OnNativeEmbedVisibilityChange(const ArkWebString& embed_id, bool visibility) = 0;

    /*--ark web()--*/
    virtual bool CloseImageOverlaySelection() = 0;
    
    /**
     * @Description: Called when web occurs ssl error event.
     * @Input result: handler of result.
     * @Input error: error code.
     * @Input certChainData: cert chain data.
     * @Return: true/false
     */
    /*--ark web()--*/
    virtual bool OnSslErrorRequestByJSV2(ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error,
        const ArkWebStringVector& certChainData) = 0;
    
    /**
     * @Description: Called when an accessibility event occurs.
     * @Input accessibilityId: the accessibility id of the accessibility node of the accessibility event.
     * @Input eventType: the event type of the accessibility event.
     */
    /*--ark web()--*/
    virtual void OnAccessibilityEvent(int64_t accessibilityId, int32_t eventType) = 0;

    /**
     * @Description: Web gets focus on the framework.
     * @Return: true/false
     */
    /*--ark web()--*/
    virtual bool IsCurrentFocus() = 0;

    /**
     * @brief Get the visible area relative to the web.
     */
    /*--ark web()--*/
    virtual void GetVisibleRectToWeb(int& visibleX, int& visibleY, int& visibleWidth, int& visibleHeight) = 0;

    /*--ark web()--*/
    virtual void OnScrollStart(const float x, const float y) = 0;

    /*--ark web()--*/
    virtual void OnShowAutofillPopupV2(
        const float offsetX, const float offsetY, const float height, const float width,
        const ArkWebStringVector& menu_items) = 0;

    /**
     * @brief Restore web component renderfit.
     */
    /*--ark web()--*/
    virtual void RestoreRenderFit() = 0;

    /**
     * @Description: Called when an accessibility event occurs.
     * @Input accessibilityId: the accessibility id of the accessibility node of the accessibility event.
     * @Input eventType: the event type of the accessibility event.
     * @Input argument: the argument of the accessibility event.
     */
    /*--ark web()--*/
    virtual void OnAccessibilityEventV2(int64_t accessibilityId, int32_t eventType, const ArkWebString& argument) = 0;

    /*--ark web()--*/
    virtual bool OnNestedScroll(float& x, float& y, float& xVelocity, float& yVelocity, bool& isAvailable) = 0;

    /*--ark web()--*/
    virtual void EnableSecurityLayer(bool isNeedSecurityLayer) = 0;

    /**
     * @brief Called When you click on the selected area.
     */
    /*--ark web()--*/
    virtual bool ChangeVisibilityOfQuickMenuV2() = 0;

    /*--ark web()--*/
    virtual void OnPip(int status,
                       int delegate_id,
                       int child_id,
                       int frame_routing_id,
                       int width,
                       int height) = 0;

    /**
     * @brief Notify the host application that the web page wants to handle
     *        JavaScript onbeforeunload.
     *
     * @param url String: The url of the page requesting.
     * @param message String: The message of the dialog.
     * @param isReload bool: The isReload parameter is set to true when the page is refreshed;
     *        otherwise, it remains false. Default is false.
     * @param result std::shared_ptr<NWebJSDialogResult>: A NWebJSDialogResult to
     *        confirm that the user closed the window.
     * @return To show a custom dialog, the app should return true.
     */
    /*--ark web()--*/
    virtual bool OnBeforeUnloadByJSV2(const ArkWebString& url, const ArkWebString& message, bool isReload,
        ArkWebRefPtr<ArkWebJsDialogResult> result) = 0;

    /**
     * @Description: Called when an mouse native event occurs on native embed area.
     * @Input mouse_event: Mouse events that contain information about the same layer.
     */
    /*--ark web()--*/
    virtual void OnNativeEmbedMouseEvent(ArkWebRefPtr<ArkWebNativeEmbedMouseEvent> mouse_event) = 0;

    /**
     * @brief called when the web page is active for window.open called by other web component.
     */
    /*--ark web()--*/
    virtual void OnActivateContentByJS() = 0;

    /**
     * @Description Notify the SDK that a web site has started loading. This method is
     * called once for each main frame load. Embedded frame changes, i.e. clicking
     * a link whose target is an iframe and fragment navigations (navigations to
     * #fragment_id) will not trigger this callback.
     *
     * @Input url The url to be loaded.
     */
    /*--ark web()--*/
    virtual void OnLoadStarted(const ArkWebString& url) = 0;

    /**
     * @Description Notify the SDK that a web site has finished loading. This method is
     * called only for main frame. Different from onPageEnd, onLoadFinished is
     * triggered only once if the mainframe is automatically redirected before the
     * page is completely loaded. OnPageEnd is triggered every navigation.
     * fragment navigation also triggers onLoadFinished.
     *
     * @Input url The url of the web site.
     */
    /*--ark web()--*/
    virtual void OnLoadFinished(const ArkWebString& url) = 0;

    /**
     * @Description Called when an SSL error occurs during the loading of resources (for the main frame and subframes).
     * @Input result: handler of result.
     * @Input error: error code.
     * @Input url: request url
     * @Input originalUrl: original url.
     * @Input referrer: referrer url.
     * @Input isFatalError: whether the error is a fatal error.
     * @Input isMainFrame: whether the request is made for the main frame.
     * @Input certChainData: cert chain data.
     * @Return: true/false
     */
    /*--ark web()--*/
    virtual bool OnAllSslErrorRequestByJSV2(ArkWebRefPtr<ArkWebJsAllSslErrorResult> result, int error,
        const ArkWebString& url, const ArkWebString& originalUrl, const ArkWebString& referrer, bool isFatalError,
        bool isMainFrame, const ArkWebStringVector& certChainData) = 0;

    /**
     * @brief Called when you need to show magnifier.
     */
    /*--ark web()--*/
    virtual void ShowMagnifier() {}

    /**
     * @brief Called when you need to hide magnifier.
     */
    /*--ark web()--*/
    virtual void HideMagnifier() {}

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     * @param isRealTitle Mark the source of the title. If it is true, the title is derived from the H5 title element;
     *        If it is false, it is calculated from the URL. By default, it is calculated from the URL.
     */
    /*--ark web()--*/
    virtual void OnPageTitleV2(const ArkWebString& title, bool isRealTitle) = 0;

    /**
     * @brief Notify the web client to do something for blankless.
     *
     * @param pathToFrame The file used to insert frame. If empty, means remove frame.
     */
    /*--ark web()--*/
    virtual void OnInsertBlanklessFrame(const ArkWebString& pathToFrame) = 0;

    /**
     * @brief Notify the web client to remove blankless frame.
     *
     * @param delayTime The delayTime for web client to remove blankless frame.
     */
    /*--ark web()--*/
    virtual void OnRemoveBlanklessFrame(int delayTime) = 0;

    /**
     * @brief Triggered when the web page's document resource error
     *
     * @param request The request information.
     * @param error The error information.
     */
    /*--ark web()--*/
    virtual ArkWebString OnHandleOverrideErrorPage(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request,
        ArkWebRefPtr<ArkWebUrlResourceError> error) = 0;

    /**
     * @brief Notify the web client pdf scroll at bottom.
     *
     * @param url The url of the pdf.
     */
    /*--ark web()--*/
    virtual void OnPdfScrollAtBottom(const ArkWebString& url) = 0;

    /**
     * @brief Notify the web client pdf load event.
     *
     * @param result The result of the pdf load.
     * @param url The url of the pdf.
     */
    /*--ark web()--*/
    virtual void OnPdfLoadEvent(int32_t result, const ArkWebString& url) = 0;

    /*--ark web()--*/
    virtual void OnTakeFocus(ArkWebRefPtr<ArkWebKeyEvent> event) = 0;

    /**
     * @brief Notify the web client to insert blankless frame with size.
     *
     * @param pathToFrame The file used to insert frame.
     * @param width The width of the blankless frame.
     * @param height The height of the blankless frame.
     */
    /*--ark web()--*/
    virtual void OnInsertBlanklessFrameWithSize(const ArkWebString& pathToFrame,
                                                uint32_t width,
                                                uint32_t height) = 0;

    /**
     * @brief Notify the web client to set the soft keyboard panel status.
     *
     * @param visible the soft keyboard panel status. If true, it represents the keyboard display;
     *        If it is false, it indicates that the keyboard is hidden.
     *
     */
    /*--ark web()--*/
    virtual void SetImeShow(bool visible) = 0;

    /**
     * @brief Called when you click on caret.
     */
    /*--ark web()--*/
    virtual bool IsShowHandle() { return false; }

    /**
     * @brief Called when params of the native object are changed.
     *
     * @param data_info The information containing the set of all changed params for the native object.
     *
     */
    /*--ark web()--*/
    virtual void OnNativeEmbedObjectParamChange(ArkWebRefPtr<ArkWebNativeEmbedParamDataInfo> data_info) = 0;

    /**
     * @brief Called when received website security risk check finish.
     *
     * @param threat_type The threat type of website.
     */
    /*--ark web()--*/
    virtual void OnSafeBrowsingCheckFinish(int threat_type) = 0;

    /**
     * @brief inform application to update its visited links database.
     *
     * @param url the url being visited.
     * @param isReload true if the url is being reload.
     * @param isMainFrame true if the request is for the main frame.
     */
    /*--ark web()--*/
    virtual void OnRefreshAccessedHistoryV2(const ArkWebString& url, bool isReload, bool isMainFrame) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HANDLER_H_
