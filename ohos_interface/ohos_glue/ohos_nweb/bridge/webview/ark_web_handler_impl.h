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

#ifndef ARK_WEB_HANDLER_IMPL_H_
#define ARK_WEB_HANDLER_IMPL_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_handler.h"

namespace OHOS::ArkWeb {

class ArkWebHandlerImpl : public ArkWebHandler {
    IMPLEMENT_REFCOUNTING(ArkWebHandlerImpl);

public:
    ArkWebHandlerImpl(std::shared_ptr<OHOS::NWeb::NWebHandler> nweb_handler);
    ~ArkWebHandlerImpl() = default;

    void SetNWeb(ArkWebRefPtr<ArkWebNWeb> nweb) override;

    /**
     * @brief Request display and focus for a new nweb.
     *
     */
    bool OnFocus() override;

    void OnMessage(const ArkWebString& param) override;

    /**
     * @brief Notify the SDK that the nweb will load the resource specified by the
     *        given url.
     *
     * @param url The url of the resource.
     */
    void OnResource(const ArkWebString& url) override;

    /**
     * @brief Notify the SDK of a new favicon for the current web site.
     *
     * @param data The raw image data for the icon.
     * @param width The width of the icon in pixel.
     * @param height The height of the icon in pixel.
     * @param color_type The color data encoding type.
     * @param alpha_type The alpha value of any pixel.
     */
    void OnPageIcon(const void* data, size_t width, size_t height, int color_type, int alpha_type) override;

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     */
    void OnPageTitle(const ArkWebString& title) override;

    void OnProxyDied() override;

    /**
     * @brief Notify the SDK that an HTTP error has been received from the server
     *        while loading a resource.
     *
     * @param request The request information.
     * @param response The error occurred.
     */
    void OnHttpError(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceResponse> response) override;

    /**
     * @brief Report a JavaScript console message to the host application.
     *
     * @param console_log Details of the console message.
     *
     * @return Return true to stop the message from being output to the console.
     */
    bool OnConsoleLog(ArkWebRefPtr<ArkWebConsoleLog> console_log) override;

    void OnRouterPush(const ArkWebString& param) override;

    /**
     * @brief Notify the SDK that a web site has finished loading. This method is
     *        called only for main frame.
     *
     * @param http_status_code The status code for the http request.
     * @param url The url of the web site.
     */
    void OnPageLoadEnd(int http_status_code, const ArkWebString& url) override;

    /**
     * @brief Notify the SDK that a web site has started loading. This method is
     *        called once for each main frame load.
     *
     * @param url The url to be loaded.
     */
    void OnPageLoadBegin(const ArkWebString& url) override;

    /**
     * @brief Report a load error to the SDK.
     *
     * @param error_code The error code.
     * @param description The error description.
     * @param url The failed url.
     */
    void OnPageLoadError(int error_code, const ArkWebString& description, const ArkWebString& url) override;

    /**
     * @brief Notify the SDK of the url for a touch icon.
     *
     * @param icon_url The icon url.
     * @param precomposed The touch icon type.
     */
    void OnDesktopIconUrl(const ArkWebString& icon_url, bool precomposed) override;

    /**
     * @brief Notify the SDK the current progress of loading a web site.
     *
     * @param new_progress Loading progress, an integer between 0 and 100.
     */
    void OnLoadingProgress(int new_progress) override;

    /**
     * @brief Show prompt to ask for the geo location permission.
     *
     * @param origin String: the origin of the resource to get geo location
     * @param callback GeolocationCallbackInterface: callback to report geo
     *        location
     */
    void OnGeolocationShow(const ArkWebString& origin, ArkWebRefPtr<ArkWebGeoLocationCallback> callback) override;

    /**
     * @brief Hide prompt to ask for the geolocation permission.
     */
    void OnGeolocationHide() override;

    /**
     * @brief inform application to show a file selector.
     *
     * @param callback the file list to select.
     * @param params the params of file selector.
     */
    bool OnFileSelectorShow(
        ArkWebRefPtr<ArkWebStringVectorValueCallback> callback, ArkWebRefPtr<ArkWebFileSelectorParams> params) override;

    /**
     * @brief Report web resource loading error to the SDK. These errors usually
     *        indicate inability to connect to the server.
     *
     * @param request The request information.
     * @param error The error information.
     */
    void OnResourceLoadError(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceError> error) override;

    /**
     * @brief Ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     *        permission.
     */
    void OnPermissionRequest(ArkWebRefPtr<ArkWebAccessRequest> request) override;

    void OnQuickMenuDismissed() override;

    void OnContextMenuDismissed() override;

    void OnTouchSelectionChanged(ArkWebRefPtr<ArkWebTouchHandleState> insert_handle,
        ArkWebRefPtr<ArkWebTouchHandleState> start_selection_handle,
        ArkWebRefPtr<ArkWebTouchHandleState> end_selection_handle) override;

    /**
     * @brief Notify the host application of a resource request and allow the
     *        application to return the data.
     *
     * @param request request information
     * @param response response information
     *
     * @retval true if handle success, otherwise false.
     */
    bool OnHandleInterceptRequest(
        ArkWebRefPtr<ArkWebUrlResourceRequest> request, ArkWebRefPtr<ArkWebUrlResourceResponse> response) override;

    /**
     * @brief inform application to update its visited links database.
     *
     * @param url the url being visited.
     * @param is_reload true if the url is being reload.
     */
    void OnRefreshAccessedHistory(const ArkWebString& url, bool is_reload) override;

    /**
     * @brief Cancel the request to ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     *        permission.
     */
    void OnPermissionRequestCanceled(ArkWebRefPtr<ArkWebAccessRequest> request) override;

    /**
     * @brief Give the SDK a chance to decide whether to continue loading the url.
     *
     * @param url The url to be loaded.
     *
     * @return true to cancel the loading, false to continue the loading.
     */
    bool OnHandleInterceptUrlLoading(ArkWebRefPtr<ArkWebUrlResourceRequest> request) override;

    bool RunQuickMenu(
        ArkWebRefPtr<ArkWebQuickMenuParams> params, ArkWebRefPtr<ArkWebQuickMenuCallback> callback) override;

    bool RunContextMenu(
        ArkWebRefPtr<ArkWebContextMenuParams> params, ArkWebRefPtr<ArkWebContextMenuCallback> callback) override;

    void UpdateDragCursor(unsigned char op) override;

    bool FilterScrollEvent(const float x, const float y, const float x_velocity, const float y_velocity) override;

    /**
     * @brief Obtains a list of all visited history items, used for link coloring
     *
     * @return visited history
     */
    ArkWebStringVector VisitedUrlHistory() override;

    /**
     * @brief called when new window required.
     *
     * @param target_url URL to be loaded in the new window.
     * @param is_alert dialog window or not.
     * @param is_user_trigger triggered by User.
     * @param handler set the new web object.
     */
    void OnWindowNewByJS(const ArkWebString& target_url, bool is_alert, bool is_user_trigger,
        ArkWebRefPtr<ArkWebControllerHandler> handler) override;

    /**
     * @brief called when window exit.
     */
    void OnWindowExitByJS() override;

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
    bool OnAlertDialogByJS(
        const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result) override;

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
    bool OnBeforeUnloadByJS(
        const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result) override;

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
    bool OnPromptDialogByJS(const ArkWebString& url, const ArkWebString& message, const ArkWebString& default_value,
        ArkWebRefPtr<ArkWebJsDialogResult> result) override;

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
    bool OnConfirmDialogByJS(
        const ArkWebString& url, const ArkWebString& message, ArkWebRefPtr<ArkWebJsDialogResult> result) override;

    bool OnHttpAuthRequestByJS(
        ArkWebRefPtr<ArkWebJsHttpAuthResult> result, const ArkWebString& host, const ArkWebString& realm) override;

    bool OnSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error) override;

    bool OnSslSelectCertRequestByJS(ArkWebRefPtr<ArkWebJsSslSelectCertResult> result, const ArkWebString& host,
        int port, const ArkWebStringVector& key_types, const ArkWebStringVector& issuers) override;

    void OnScroll(double x_offset, double y_offset) override;

    void OnOverScroll(float x_offset, float y_offset) override;

    void OnScrollState(bool scroll_state) override;

    /**
     * @brief called when the page being loaded is about to be made visible.
     */
    void OnPageVisible(const ArkWebString& url) override;

    /**
     * @brief Give the host application a chance to handle the key event
     *        synchronousl.
     *
     * @param event The key event.
     *
     * @return True if the host application wants to handle the key event itself,
     *         otherwise return false.
     */
    bool OnPreKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) override;

    void OnScaleChanged(float old_scale_factor, float new_scale_factor) override;

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
    bool OnCursorChange(const int32_t& type, ArkWebRefPtr<ArkWebCursorInfo> info) override;

    /**
     * @brief called when the render process exit.
     *
     * @param reason the detail reason why render process exit, the implementation
     *        of this callback should attempt to clean up the specific nweb that
     *        was set by SetNWeb interface.
     */
    void OnRenderExited(int reason) override;

    /**
     * @brief Called when resize not work.
     */
    void OnResizeNotWork() override;

    /**
     * @brief called when the page exit the full-screen mode.
     */
    void OnFullScreenExit() override;

    /**
     * @brief called when the page enter the full-screen mode.
     *
     * @param handler to exit full-screen mode.
     */
    void OnFullScreenEnter(ArkWebRefPtr<ArkWebFullScreenExitHandler> handler) override;

    bool OnDragAndDropData(const void* data, size_t len, ArkWebRefPtr<ArkWebImageOptions> opt) override;

    void OnSelectPopupMenu(
        ArkWebRefPtr<ArkWebSelectPopupMenuParam> params, ArkWebRefPtr<ArkWebSelectPopupMenuCallback> callback) override;

    /**
     * @brief shows the repost form confirmation dialog box.
     *
     * @param handler sets whether to resend data.
     */
    void OnDataResubmission(ArkWebRefPtr<ArkWebDataResubmissionCallback> handler) override;

    void OnRootLayerChanged(int width, int height) override;

    /**
     * @brief Called when the audio playing state on web page changed.
     *
     * @param playing Whether the audio is playing or not.
     */
    void OnAudioStateChanged(bool playing) override;

    void OnOverScrollFlingEnd() override;

    /**
     * @brief Notify the host application that a key was not handled by the
     *        WebView.
     *
     * @param event The key event.
     *
     * @return True if the host application wants to handle the key event itself,
     *         otherwise return false.
     */
    bool OnUnProcessedKeyEvent(ArkWebRefPtr<ArkWebKeyEvent> event) override;

    bool OnDragAndDropDataUdmf(ArkWebRefPtr<ArkWebDragData> drag_data) override;

    /**
     * @brief Called when the first content rendering of web page.
     *
     * @param navigation_start_tick Absolute navigation start time, as TimeTicks.
     * @param first_contentful_paint_ms Time to first contentful paint from
     *        navigation start.
     */
    void OnFirstContentfulPaint(int64_t navigation_start_tick, int64_t first_contentful_paint_ms) override;

    void OnDateTimeChooserPopup(ArkWebRefPtr<ArkWebDateTimeChooser> chooser,
        const ArkWebDateTimeSuggestionVector& suggestions,
        ArkWebRefPtr<ArkWebDateTimeChooserCallback> callback) override;

    void OnDateTimeChooserClose() override;

    /**
     * @brief Ask for the screen capture permission.
     *
     * @param request std::shared_ptr<NWebScreenCaptureAccessRequest>: A request
     *        to ask for the screen capture permission.
     */
    void OnScreenCaptureRequest(ArkWebRefPtr<ArkWebScreenCaptureAccessRequest> request) override;

    /**
     * @brief Called when the media or form state on the web page changed.
     *
     * @param state state of the media or form. Refer to the enum class
     *        MediaPlayingState and FormState
     * @param type it can be form, media, or audio
     */
    void OnActivityStateChanged(int state, int type) override;

    void OnGetTouchHandleHotZone(ArkWebRefPtr<ArkWebTouchHandleHotZone> hot_zone) override;

    /**
     * @brief Called when swap buffer completed with new size.
     */
    void OnCompleteSwapWithNewSize() override;

    void OnOverScrollFlingVelocity(float x_velocity, float y_velocity, bool is_fling) override;

    /**
     * @brief called when the navigation entry has been committed.
     *
     * @param details represents the details of a committed navigation entry.
     */
    void OnNavigationEntryCommitted(ArkWebRefPtr<ArkWebLoadCommittedDetails> details) override;

    void OnNativeEmbedLifecycleChange(ArkWebRefPtr<ArkWebNativeEmbedDataInfo> data_info) override;

    void OnNativeEmbedGestureEvent(ArkWebRefPtr<ArkWebNativeEmbedTouchEvent> touch_event) override;

    /**
     * @brief Called when received website security risk check result.
     *
     * @param threat_type The threat type of website.
     */
    void OnSafeBrowsingCheckResult(int threat_type) override;

    /**
     * @brief Called when tracker's cookie is prevented.
     *
     * @param website_host The host of website url.
     *
     * @param tracker_host The host of tracker url.
     */
    void OnIntelligentTrackingPreventionResult(
        const ArkWebString& website_host, const ArkWebString& tracker_host) override;

    /**
     * @brief called when the page enter the full-screen mode.
     *
     * @param handler to exit full-screen mode.
     * @param video_natural_width indicates the width of the <video> element
     * entering full screen.
     * @param video_natural_height indicates the height of the <video> element
     * entering full screen.
     */
    void OnFullScreenEnterWithVideoSize(
        ArkWebRefPtr<ArkWebFullScreenExitHandler> handler, int video_natural_width, int video_natural_height) override;

    /**
     * @brief Give the application a chance to decide whether to override loading the
     * url.
     *
     * @param request The request information.
     * @return true to abort loading the url, false to continue loading the url
     * as usual.
     */
    bool OnHandleOverrideUrlLoading(ArkWebRefPtr<ArkWebUrlResourceRequest> request) override;

    /**
     * @brief Called when the first meaningful paint rendering of web page.
     *
     * @param details represents the details of first meaningful paint.
     */
    void OnFirstMeaningfulPaint(ArkWebRefPtr<ArkWebFirstMeaningfulPaintDetails> details) override;

    /**
     * @brief Called when the largest contentful paint rendering of web page.
     *
     * @param details represents the details of largest contentful paint.
     */
    void OnLargestContentfulPaint(ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> details) override;

    bool OnAllSslErrorRequestByJS(ArkWebRefPtr<ArkWebJsAllSslErrorResult> result, int error, const ArkWebString& url,
        const ArkWebString& originalUrl, const ArkWebString& referrer, bool isFatalError, bool isMainFrame) override;

    void OnTooltip(const ArkWebString& tooltip) override;

    void ReleaseResizeHold() override;

    ArkWebCharVector GetWordSelection(const ArkWebString& text, int8_t offset) override;

    void UpdateClippedSelectionBounds(int x, int y, int w, int h) override;

    void OnRenderProcessNotResponding(const ArkWebString& js_stack, int pid, int reason) override;
    void OnRenderProcessResponding() override;

    bool OnOpenAppLink(const ArkWebString& url, ArkWebRefPtr<ArkWebAppLinkCallback> callback) override;

    void OnShowAutofillPopup(const float offsetX, const float offsetY, const ArkWebStringVector& menu_items) override;

    void OnHideAutofillPopup() override;

    void OnViewportFitChange(int viewportFit) override;

    bool OnFocus(int source) override;

    bool OnOverScroll(float xOffset, float yOffset, float xVelocity, float yVelocity) override;

    void CreateOverlay(void* data, size_t len, int width, int height, int offset_x, int offset_y, int rect_width,
        int rect_height, int point_x, int point_y) override;

    void OnOverlayStateChanged(int offset_x, int offset_y, int rect_width, int rect_height) override;

    void OnAdsBlocked(const ArkWebString& url, const ArkWebStringVector& adsBlocked) override;

    void OnInterceptKeyboardAttach(ArkWebRefPtr<ArkWebCustomKeyboardHandler> keyboardHandler,
        const ArkWebStringMap &attributes, bool &useSystemKeyboard, int32_t &enterKeyType) override;

    void OnCustomKeyboardAttach() override;

    void OnCustomKeyboardClose() override;

    void KeyboardReDispatch(ArkWebRefPtr<ArkWebKeyEvent> event, bool isUsed) override;

    void HideHandleAndQuickMenuIfNecessary(bool hide) override;
 
    void OnCursorUpdate(double x, double y, double width, double height) override;
 
    void ChangeVisibilityOfQuickMenu() override;

    void StartVibraFeedback(const ArkWebString& vibratorType) override;

    void OnNativeEmbedVisibilityChange(const ArkWebString& embed_id, bool visibility) override;

    bool CloseImageOverlaySelection() override;

    void GetVisibleRectToWeb(int& visibleX, int& visibleY, int& visibleWidth, int& visibleHeight) override;

    void OnScrollStart(const float x, const float y) override;

    bool OnSslErrorRequestByJSV2(ArkWebRefPtr<ArkWebJsSslErrorResult> result, int error,
        const ArkWebStringVector& certChainData) override;

    void RestoreRenderFit() override;

    void OnAccessibilityEvent(int64_t accessibilityId, int32_t eventType) override;

    void OnAccessibilityEventV2(int64_t accessibilityId, int32_t eventType, const ArkWebString& argument) override;
private:
    std::shared_ptr<OHOS::NWeb::NWebHandler> nweb_handler_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HANDLER_IMPL_H_
