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

#ifndef ARK_WEB_HANDLER_WRAPPER_H_
#define ARK_WEB_HANDLER_WRAPPER_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_handler.h"

namespace OHOS::ArkWeb {

using ArkWebSslError = OHOS::NWeb::SslError;
using ArkWebCursorType = OHOS::NWeb::CursorType;
using ArkWebActivityType = OHOS::NWeb::ActivityType;
using ArkWebImageColorType = OHOS::NWeb::ImageColorType;
using ArkWebImageAlphaType = OHOS::NWeb::ImageAlphaType;
using ArkWebRenderExitReason = OHOS::NWeb::RenderExitReason;
using ArkWebDragOperation = OHOS::NWeb::NWebDragData::DragOperation;
using ArkWebRenderProcessNotRespondingReason = OHOS::NWeb::RenderProcessNotRespondingReason;
using ArkWebViewportFit = OHOS::NWeb::ViewportFit;
using ArkWebFocusSource = OHOS::NWeb::NWebFocusSource;

class ArkWebHandlerWrapper : public OHOS::NWeb::NWebHandler {
public:
    ArkWebHandlerWrapper(ArkWebRefPtr<ArkWebHandler> ark_web_handler);
    ~ArkWebHandlerWrapper() = default;

    void SetNWeb(std::shared_ptr<OHOS::NWeb::NWeb> nweb) override;

    /**
     * @brief Request display and focus for a new nweb.
     *
     */
    bool OnFocus() override;

    void OnMessage(const std::string& param) override;

    /**
     * @brief Notify the SDK that the nweb will load the resource specified by the
     *        given url.
     *
     * @param url The url of the resource.
     */
    void OnResource(const std::string& url) override;

    /**
     * @brief Notify the SDK of a new favicon for the current web site.
     *
     * @param data The raw image data for the icon.
     * @param width The width of the icon in pixel.
     * @param height The height of the icon in pixel.
     * @param color_type The color data encoding type.
     * @param alpha_type The alpha value of any pixel.
     */
    void OnPageIcon(const void* data, size_t width, size_t height, ArkWebImageColorType color_type,
        ArkWebImageAlphaType alpha_type) override;

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     */
    void OnPageTitle(const std::string& title) override;

    void OnProxyDied() override;

    /**
     * @brief Notify the SDK that an HTTP error has been received from the server
     *        while loading a resource.
     *
     * @param request The request information.
     * @param response The error occurred.
     */
    void OnHttpError(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response) override;

    /**
     * @brief Report a JavaScript console message to the host application.
     *
     * @param console_log Details of the console message.
     *
     * @return Return true to stop the message from being output to the console.
     */
    bool OnConsoleLog(std::shared_ptr<OHOS::NWeb::NWebConsoleLog> console_log) override;

    void OnRouterPush(const std::string& param) override;

    /**
     * @brief Notify the SDK that a web site has finished loading. This method is
     *        called only for main frame.
     *
     * @param http_status_code The status code for the http request.
     * @param url The url of the web site.
     */
    void OnPageLoadEnd(int http_status_code, const std::string& url) override;

    /**
     * @brief Notify the SDK that a web site has started loading. This method is
     *        called once for each main frame load.
     *
     * @param url The url to be loaded.
     */
    void OnPageLoadBegin(const std::string& url) override;

    /**
     * @brief Report a load error to the SDK.
     *
     * @param error_code The error code.
     * @param description The error description.
     * @param url The failed url.
     */
    void OnPageLoadError(int error_code, const std::string& description, const std::string& url) override;

    /**
     * @brief Notify the SDK of the url for a touch icon.
     *
     * @param icon_url The icon url.
     * @param precomposed The touch icon type.
     */
    void OnDesktopIconUrl(const std::string& icon_url, bool precomposed) override;

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
    void OnGeolocationShow(
        const std::string& origin, std::shared_ptr<OHOS::NWeb::NWebGeolocationCallbackInterface> callback) override;

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
    bool OnFileSelectorShow(std::shared_ptr<OHOS::NWeb::NWebStringVectorValueCallback> callback,
        std::shared_ptr<OHOS::NWeb::NWebFileSelectorParams> params) override;

    /**
     * @brief Report web resource loading error to the SDK. These errors usually
     *        indicate inability to connect to the server.
     *
     * @param request The request information.
     * @param error The error information.
     */
    void OnResourceLoadError(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> error) override;

    /**
     * @brief Ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     *        permission.
     */
    void OnPermissionRequest(std::shared_ptr<OHOS::NWeb::NWebAccessRequest> request) override;

    void OnQuickMenuDismissed() override;

    void OnContextMenuDismissed() override;

    void OnTouchSelectionChanged(std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> insert_handle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> start_selection_handle,
        std::shared_ptr<OHOS::NWeb::NWebTouchHandleState> end_selection_handle) override;

    /**
     * @brief Notify the host application of a resource request and allow the
     *        application to return the data.
     *
     * @param request request information
     * @param response response information
     *
     * @retval true if handle success, otherwise false.
     */
    bool OnHandleInterceptRequest(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> response) override;

    /**
     * @brief inform application to update its visited links database.
     *
     * @param url the url being visited.
     * @param is_reload true if the url is being reload.
     */
    void OnRefreshAccessedHistory(const std::string& url, bool is_reload) override;

    /**
     * @brief Cancel the request to ask for the permission.
     *
     * @param request std::shared_ptr<NWebAccessRequest>: A request to ask for the
     *        permission.
     */
    void OnPermissionRequestCanceled(std::shared_ptr<OHOS::NWeb::NWebAccessRequest> request) override;

    /**
     * @brief Give the SDK a chance to decide whether to continue loading the url.
     *
     * @param url The url to be loaded.
     *
     * @return true to cancel the loading, false to continue the loading.
     */
    bool OnHandleInterceptUrlLoading(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request) override;

    bool RunQuickMenu(std::shared_ptr<OHOS::NWeb::NWebQuickMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebQuickMenuCallback> callback) override;

    bool RunContextMenu(std::shared_ptr<OHOS::NWeb::NWebContextMenuParams> params,
        std::shared_ptr<OHOS::NWeb::NWebContextMenuCallback> callback) override;

    void UpdateDragCursor(ArkWebDragOperation op) override;

    bool FilterScrollEvent(const float x, const float y, const float x_velocity, const float y_velocity) override;

    /**
     * @brief Obtains a list of all visited history items, used for link coloring
     *
     * @return visited history
     */
    std::vector<std::string> VisitedUrlHistory() override;

    /**
     * @brief called when new window required.
     *
     * @param target_url URL to be loaded in the new window.
     * @param is_alert dialog window or not.
     * @param is_user_trigger triggered by User.
     * @param handler set the new web object.
     */
    void OnWindowNewByJS(const std::string& target_url, bool is_alert, bool is_user_trigger,
        std::shared_ptr<OHOS::NWeb::NWebControllerHandler> handler) override;

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
    bool OnAlertDialogByJS(const std::string& url, const std::string& message,
        std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result) override;

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
    bool OnBeforeUnloadByJS(const std::string& url, const std::string& message,
        std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result) override;

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
    bool OnPromptDialogByJS(const std::string& url, const std::string& message, const std::string& default_value,
        std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result) override;

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
    bool OnConfirmDialogByJS(const std::string& url, const std::string& message,
        std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result) override;

    bool OnHttpAuthRequestByJS(std::shared_ptr<OHOS::NWeb::NWebJSHttpAuthResult> result, const std::string& host,
        const std::string& realm) override;

    bool OnSslErrorRequestByJS(std::shared_ptr<OHOS::NWeb::NWebJSSslErrorResult> result, ArkWebSslError error) override;

    bool OnSslSelectCertRequestByJS(std::shared_ptr<OHOS::NWeb::NWebJSSslSelectCertResult> result,
        const std::string& host, int port, const std::vector<std::string>& key_types,
        const std::vector<std::string>& issuers) override;

    void OnScroll(double x_offset, double y_offset) override;

    void OnOverScroll(float x_offset, float y_offset) override;

    void OnScrollState(bool scroll_state) override;

    /**
     * @brief called when the page being loaded is about to be made visible.
     */
    void OnPageVisible(const std::string& url) override;

    /**
     * @brief Give the host application a chance to handle the key event
     *        synchronousl.
     *
     * @param event The key event.
     *
     * @return True if the host application wants to handle the key event itself,
     *         otherwise return false.
     */
    bool OnPreKeyEvent(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event) override;

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
    bool OnCursorChange(const ArkWebCursorType& type, std::shared_ptr<OHOS::NWeb::NWebCursorInfo> info) override;

    /**
     * @brief called when the render process exit.
     *
     * @param reason the detail reason why render process exit, the implementation
     *        of this callback should attempt to clean up the specific nweb that
     *        was set by SetNWeb interface.
     */
    void OnRenderExited(ArkWebRenderExitReason reason) override;

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
    void OnFullScreenEnter(std::shared_ptr<OHOS::NWeb::NWebFullScreenExitHandler> handler) override;

    bool OnDragAndDropData(const void* data, size_t len, std::shared_ptr<OHOS::NWeb::NWebImageOptions> opt) override;

    void OnSelectPopupMenu(std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuParam> params,
        std::shared_ptr<OHOS::NWeb::NWebSelectPopupMenuCallback> callback) override;

    /**
     * @brief shows the repost form confirmation dialog box.
     *
     * @param handler sets whether to resend data.
     */
    void OnDataResubmission(std::shared_ptr<OHOS::NWeb::NWebDataResubmissionCallback> handler) override;

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
    bool OnUnProcessedKeyEvent(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event) override;

    bool OnDragAndDropDataUdmf(std::shared_ptr<OHOS::NWeb::NWebDragData> drag_data) override;

    /**
     * @brief Called when the first content rendering of web page.
     *
     * @param navigation_start_tick Absolute navigation start time, as TimeTicks.
     * @param first_contentful_paint_ms Time to first contentful paint from
     *        navigation start.
     */
    void OnFirstContentfulPaint(int64_t navigation_start_tick, int64_t first_contentful_paint_ms) override;

    void OnDateTimeChooserPopup(std::shared_ptr<OHOS::NWeb::NWebDateTimeChooser> chooser,
        const std::vector<std::shared_ptr<OHOS::NWeb::NWebDateTimeSuggestion>>& suggestions,
        std::shared_ptr<OHOS::NWeb::NWebDateTimeChooserCallback> callback) override;

    void OnDateTimeChooserClose() override;

    /**
     * @brief Ask for the screen capture permission.
     *
     * @param request std::shared_ptr<NWebScreenCaptureAccessRequest>: A request
     *        to ask for the screen capture permission.
     */
    void OnScreenCaptureRequest(std::shared_ptr<OHOS::NWeb::NWebScreenCaptureAccessRequest> request) override;

    /**
     * @brief Called when the media or form state on the web page changed.
     *
     * @param state state of the media or form. Refer to the enum class
     *        MediaPlayingState and FormState
     * @param type it can be form, media, or audio
     */
    void OnActivityStateChanged(int state, ArkWebActivityType type) override;

    void OnGetTouchHandleHotZone(std::shared_ptr<OHOS::NWeb::NWebTouchHandleHotZone> hot_zone) override;

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
    void OnNavigationEntryCommitted(std::shared_ptr<OHOS::NWeb::NWebLoadCommittedDetails> details) override;

    void OnNativeEmbedLifecycleChange(std::shared_ptr<OHOS::NWeb::NWebNativeEmbedDataInfo> dataInfo) override;

    void OnNativeEmbedGestureEvent(std::shared_ptr<OHOS::NWeb::NWebNativeEmbedTouchEvent> touchEvent) override;

    /**
     * @brief Called when received website security risk check result.
     *
     * @param threat_type The threat type of website.
     */
    void OnSafeBrowsingCheckResult(int threat_type) override;

    /**
     * @brief called when the page enter the full-screen mode.
     *
     * @param handler to exit full-screen mode.
     * @param video_natural_width indicates the width of the <video> element
     * entering full screen.
     * @param video_natural_height indicates the height of the <video> element
     * entering full screen.
     */
    void OnFullScreenEnterWithVideoSize(std::shared_ptr<OHOS::NWeb::NWebFullScreenExitHandler> handler,
        int video_natural_width, int video_natural_height) override;

    /**
     * @brief Give the application a chance to decide whether to override loading
     * the url.
     *
     * @param request The request information.
     * @return true to abort loading the url, false to continue loading the url
     * as usual.
     */
    bool OnHandleOverrideUrlLoading(std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request) override;

    /**
     * @brief Called when tacker's cookie is prevented.
     *
     * @param website_host The host of website url.
     *
     * @param tracker_host The host of tracker url.
     */
    void OnIntelligentTrackingPreventionResult(
        const std::string& website_host, const std::string& tracker_host) override;

    /**
     * @brief Called when the first meaningful paint rendering of web page.
     *
     * @param details represents the details of first meaningful paint.
     */
    void OnFirstMeaningfulPaint(std::shared_ptr<OHOS::NWeb::NWebFirstMeaningfulPaintDetails> details) override;

    /**
     * @brief Called when the largest contentful paint rendering of web page.
     *
     * @param details represents the details of largest contentful paint.
     */
    void OnLargestContentfulPaint(std::shared_ptr<OHOS::NWeb::NWebLargestContentfulPaintDetails> details) override;

    bool OnAllSslErrorRequestByJS(std::shared_ptr<OHOS::NWeb::NWebJSAllSslErrorResult> result, ArkWebSslError error,
        const std::string& url, const std::string& originalUrl, const std::string& referrer, bool isFatalError,
        bool isMainFrame) override;

    void OnTooltip(const std::string& tooltip) override;

    /**
     * @brief called when resizehold is released.
     *
     */
    void ReleaseResizeHold() override;

    void OnShowAutofillPopup(
        const float offsetX, const float offsetY, const std::vector<std::string>& menu_items) override;

    void OnHideAutofillPopup() override;

    /**
     * @brief Called when select a word.
     *
     * @param text The content of the text.
     * @param offset The offset of the point.
     */
    std::vector<int8_t> GetWordSelection(const std::string& text, int8_t offset) override;

    void UpdateClippedSelectionBounds(int x, int y, int w, int h) override;

    bool OnOpenAppLink(const std::string& url, std::shared_ptr<OHOS::NWeb::NWebAppLinkCallback> callback) override;

    /**
     * @brief called when the render process  not responding.
     *
     * @param js_stack
     * @param pid
     * @param reason
     */
    void OnRenderProcessNotResponding(
        const std::string& js_stack, int pid, ArkWebRenderProcessNotRespondingReason reason) override;

    /**
     * @brief called when the render process  not responding.
     *
     */
    void OnRenderProcessResponding() override;

    void OnViewportFitChange(ArkWebViewportFit viewport_fit) override;

    void CreateOverlay(void* data, size_t len, int width, int height, int offset_x, int offset_y, int rect_width,
        int rect_height, int point_x, int point_y) override;

    void OnOverlayStateChanged(int offset_x, int offset_y, int rect_width, int rect_height) override;

    bool OnFocus(ArkWebFocusSource source) override;

    bool OnOverScroll(float xOffset, float yOffset, float xVelocity, float yVelocity) override;

    void KeyboardReDispatch(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event, bool isUsed) override;

    void OnInterceptKeyboardAttach(const std::shared_ptr<OHOS::NWeb::NWebCustomKeyboardHandler> keyboardHandler,
        const std::map<std::string, std::string> &attributes, bool &useSystemKeyboard, int32_t &enterKeyType) override;

    void OnCustomKeyboardAttach() override;

    void OnCustomKeyboardClose() override;

    void OnAdsBlocked(const std::string &url, const std::vector<std::string> &adsBlocked) override;

    void OnCursorUpdate(double x, double y, double width, double height) override;

    void ReportDynamicFrameLossEvent(const std::string& sceneId, bool isStart) override;

    void HideHandleAndQuickMenuIfNecessary(bool hide) override;

    void ChangeVisibilityOfQuickMenu() override;

    void StartVibraFeedback(const std::string& vibratorType) override;

    void OnPopupSize(int x, int y, int width, int height) override;

    void OnPopupShow(bool show) override;

    void OnNativeEmbedVisibilityChange(const std::string& embed_id, bool visibility) override;

    bool CloseImageOverlaySelection() override;

    bool OnSslErrorRequestByJSV2(std::shared_ptr<OHOS::NWeb::NWebJSSslErrorResult> result, ArkWebSslError error,
        const std::vector<std::string>& certChainData) override;

    void OnAccessibilityEvent(int64_t accessibilityId, int32_t eventType) override;

    bool IsCurrentFocus() override;

    void GetVisibleRectToWeb(int& visibleX, int& visibleY, int& visibleWidth, int& visibleHeight) override;

    void OnScrollStart(const float x, const float y) override;

    void OnShowAutofillPopupV2(
        const float offsetX, const float offsetY, const float height, const float width,
        const std::vector<std::string>& menu_items) override;

    void RestoreRenderFit() override;

    void OnAccessibilityEventV2(int64_t accessibilityId, int32_t eventType, const std::string& argument) override;

    bool OnNestedScroll(float& x, float& y, float& xVelocity, float& yVelocity, bool& isAvailable) override;

    void EnableSecurityLayer(bool isNeedSecurityLayer) override;

    bool ChangeVisibilityOfQuickMenuV2() override;

    void OnPip(int status,
               int delegate_id,
               int child_id,
               int frame_routing_id,
               int width,
               int height) override;

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
    bool OnBeforeUnloadByJSV2(const std::string& url, const std::string& message, bool isReload,
        std::shared_ptr<OHOS::NWeb::NWebJSDialogResult> result) override;

    void OnNativeEmbedMouseEvent(std::shared_ptr<OHOS::NWeb::NWebNativeEmbedMouseEvent> mouseEvent) override;

    /**
     * @brief called when the web page is active for window.open called by other web component.
     */
    void OnActivateContentByJS() override;

    void OnLoadStarted(const std::string& url) override;

    void OnLoadFinished(const std::string& url) override;

    bool OnAllSslErrorRequestByJSV2(std::shared_ptr<OHOS::NWeb::NWebJSAllSslErrorResult> result, ArkWebSslError error,
        const std::string& url, const std::string& originalUrl, const std::string& referrer, bool isFatalError,
        bool isMainFrame, const std::vector<std::string>& certChainData) override;

    /**
     * @brief Called when you need to show magnifier.
     */
    void ShowMagnifier() override;

    /**
     * @brief Called when you need to hide magnifier.
     */
    void HideMagnifier() override;

    /**
     * @brief Notify the SDK of the changed document title.
     *
     * @param title The document title.
     * @param isRealTitle Mark the source of the title. If it is true, the title is derived from the H5 title element;
     *        If it is false, it is calculated from the URL. By default, it is calculated from the URL.
     */
    void OnPageTitleV2(const std::string& title, bool isRealTitle) override;

    /**
     * @brief Notify the web client to do something for blankless.
     *
     * @param pathToFrame The file used to insert frame. If empty, means remove frame.
     */
    void OnInsertBlanklessFrame(const std::string& pathToFrame) override;

    /**
     * @brief Notify the web client to remove blankless frame.
     *
     * @param delayTime The delayTime for web client to remove blankless frame.
     */
    void OnRemoveBlanklessFrame(int delayTime) override;

    /**
     * @brief Triggered when the web page's document resource error
     *
     * @param request The request information.
     * @param error The error information.
     */
    std::string OnHandleOverrideErrorPage(
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceRequest> request,
        std::shared_ptr<OHOS::NWeb::NWebUrlResourceError> error) override;

    /**
     * @brief Notify the web client pdf scroll at bottom.
     *
     * @param url The url of the pdf.
     */
    void OnPdfScrollAtBottom(const std::string& url) override;

    /**
     * @brief Notify the web client pdf load event.
     *
     * @param result The result of the pdf load.
     * @param url The url of the pdf.
     */
    void OnPdfLoadEvent(int32_t result, const std::string& url) override;

    void OnTakeFocus(std::shared_ptr<OHOS::NWeb::NWebKeyEvent> event) override;

    /**
     * @brief Notify the web client to insert blankless frame with size.
     *
     * @param pathToFrame The file used to insert frame. If empty, means remove frame.
     * @param width The width of the blankless frame.
     * @param height The height of the blankless frame.
     */
    void OnInsertBlanklessFrameWithSize(const std::string& pathToFrame,
                                        uint32_t width,
                                        uint32_t height) override;

    /**
     * @brief Notify the web client to set the soft keyboard panel status.
     *
     * @param visible the soft keyboard panel status. If true, it represents the keyboard display;
     *        If it is false, it indicates that the keyboard is hidden.
     *
     */
    void SetImeShow(bool visible) override;
private:
    ArkWebRefPtr<ArkWebHandler> ark_web_handler_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_HANDLER_WRAPPER_H_
