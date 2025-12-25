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

#ifndef ARK_WEB_NWEB_WRAPPER_H_
#define ARK_WEB_NWEB_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "include/nweb_blankless_callback.h"
#include "include/nweb_snapshot_callback.h"
#include "ohos_nweb/include/ark_web_nweb.h"

namespace OHOS::ArkWeb {

using ArkWebBlurReason = OHOS::NWeb::BlurReason;
using ArkWebFocusReason = OHOS::NWeb::FocusReason;
using ArkWebImageColorType = OHOS::NWeb::ImageColorType;
using ArkWebImageAlphaType = OHOS::NWeb::ImageAlphaType;
using ArkWebNestedScrollMode = OHOS::NWeb::NestedScrollMode;
using ArkPixelUnit = OHOS::NWeb::PixelUnit;
using ArkWebDestroyMode = OHOS::NWeb::WebDestroyMode;
using ArkWebAutoFillTriggerType = OHOS::NWeb::NWebAutoFillTriggerType;

class ArkWebNWebWrapper : public OHOS::NWeb::NWeb {
public:
    ArkWebNWebWrapper(ArkWebRefPtr<ArkWebNWeb> ark_web_nweb);
    ~ArkWebNWebWrapper() = default;

    void Resize(uint32_t width, uint32_t height, bool is_keyboard) override;

    void OnPause() override;

    void OnContinue() override;

    void OnDestroy() override;

    void OnFocus(const ArkWebFocusReason& focus_reason) override;

    void OnBlur(const ArkWebBlurReason& blur_reason) override;

    void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;

    void OnTouchRelease(int32_t id, double x, double y, bool from_overlay) override;

    void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;

    void OnTouchMove(const std::vector<std::shared_ptr<OHOS::NWeb::NWebTouchPointInfo>>& touch_point_infos,
        bool from_overlay) override;

    void OnTouchCancel() override;

    void OnNavigateBack() override;

    bool SendKeyEvent(int32_t key_code, int32_t key_action) override;

    void SendMouseWheelEvent(double x, double y, double delta_x, double delta_y) override;

    void SendMouseEvent(int x, int y, int button, int action, int count) override;

    /**
     * @brief Loads the given URL.
     *
     * @param url: the URL of the resource to load This value cannot be null.
     *
     * @return title string for the current page.
     */
    int Load(const std::string& url) override;

    /**
     * @brief Gets whether this NWeb has a back history item.
     *
     * @return true if this NWeb has a back history item
     */
    bool IsNavigatebackwardAllowed() override;

    /**
     * @brief Gets whether this NWeb has a forward history item.
     *
     * @return true if this NWeb has a forward history item
     */
    bool IsNavigateForwardAllowed() override;

    /**
     * @brief Gets whether this NWeb has a back or forward history item for number
     *        of steps.
     *
     * @param num_steps: the negative or positive number of steps to move the
     *        history
     *
     * @return true if this NWeb has a forward history item
     */
    bool CanNavigateBackOrForward(int num_steps) override;

    /**
     * @brief Goes back in the history of this NWeb.
     */
    void NavigateBack() override;

    /**
     * @brief Goes forward in the history of this NWeb.
     */
    void NavigateForward() override;

    /**
     * @brief Goes to the history item that is the number of steps away from the
     *        current item.
     */
    void NavigateBackOrForward(int step) override;

    /**
     * @brief Delete back and forward history list.
     */
    void DeleteNavigateHistory() override;

    /**
     * @brief Reloads the current URL.
     */
    void Reload() override;

    /**
     * @brief Performs a zoom operation in this NWeb.
     *
     * @param zoom_factor: the zoom factor to apply. The zoom factor will be
     *        clamped to the NWeb's zoom limits. This value must be in the range
     *        0.01 to 100.0 inclusive.
     *
     * @return the error id.
     */
    int Zoom(float zoom_factor) override;

    /**
     * @brief Performs a zooming in operation in this NWeb.
     *
     * @return the error id.
     */
    int ZoomIn() override;

    /**
     * @brief Performs a zooming out operation in this NWeb.
     *
     * @return the error id.
     */
    int ZoomOut() override;

    /**
     * @brief Stops the current load.
     *
     * @param code string: javascript code
     */
    void Stop() override;

    /**
     * @brief ExecuteJavaScript
     */
    void ExecuteJavaScript(const std::string& code) override;

    /**
     * @brief ExecuteJavaScript plus
     *
     * @param code: javascript code
     * @param callback: javascript running result
     */
    void ExecuteJavaScript(const std::string& code, std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback,
        bool extention) override;

    /**
     * @brief Gets the NWebPreference object used to control the settings for this
     *        NWeb.
     *
     * @return a NWebPreference object that can be used to control this NWeb's
     *         settings This value cannot be null.
     */
    std::shared_ptr<OHOS::NWeb::NWebPreference> GetPreference() override;

    /**
     * @brief Gets the web id.
     *
     * @return the web id
     */
    unsigned int GetWebId() override;

    /**
     * @brief Gets the last hit test result.
     *
     * @return the last HitTestResult
     */
    std::shared_ptr<OHOS::NWeb::HitTestResult> GetHitTestResult() override;

    /**
     * @brief Sets the background color for this view.
     *
     * @param color: the color of the background
     */
    void PutBackgroundColor(int color) override;

    /**
     * @brief Sets the initla scale for the page.
     *
     * @param scale: the initla scale of the page.
     */
    void InitialScale(float scale) override;

    /**
     * @brief Sets the NWebDownloadCallback that will receive download event. This
     *        will replace the current handler.
     *
     * @param download_listener: download listener
     */
    void PutDownloadCallback(std::shared_ptr<OHOS::NWeb::NWebDownloadCallback> download_listener) override;

    /**
     * @brief Set the NWebAccessibilityEventCallback that will receive
     *        accessibility event. This will replace the current handler.
     *
     * @param accessibility_event_listener: accessibility event listener
     */
    void PutAccessibilityEventCallback(
        std::shared_ptr<OHOS::NWeb::NWebAccessibilityEventCallback> accessibility_event_listener) override;

    /**
     * @brief Set the accessibility id generator that will generate accessibility
     *        id for accessibility nodes in the web. This will replace the current
     *        handler.
     *
     * @param accessibility_id_generator: Accessibility id generator.
     */
    void PutAccessibilityIdGenerator(const AccessibilityIdGenerateFunc accessibility_id_generator) override;

    /**
     * @brief Set the NWebHandler that will receive various notifications and
     *        requests. This will replace the current handler.
     *
     * @param handler: an implementation of NWebHandler This value cannot be null.
     */
    void SetNWebHandler(std::shared_ptr<OHOS::NWeb::NWebHandler> handler) override;

    /**
     * @brief Gets the title for the current page.
     *
     * @return title string for the current page.
     */
    std::string Title() override;

    /**
     * @brief Gets the progress for the current page.
     *
     * @return progress for the current page.
     */
    int PageLoadProgress() override;

    /**
     * @brief Gets the height of the HTML content.
     *
     * @return the height of the HTML content.
     */
    int ContentHeight() override;

    /**
     * @brief Gets the current scale of this NWeb.
     *
     * @return the current scale
     */
    float Scale() override;

    /**
     * @brief Loads the given URL with additional HTTP headers, specified as a map
     *        from name to value. Note that if this map contains any of the
     *        headers that are set by default by this NWeb, such as those
     *        controlling caching, accept types or the User-Agent, their values
     *        may be overridden by this NWeb's defaults.
     *
     * @param url: the URL of the resource to load This value cannot be null.
     * @param additional_http_headers: additional http headers
     */
    int Load(const std::string& url, const std::map<std::string, std::string>& additional_http_headers) override;

    /**
     * @brief Loads the given data into this NWeb, using baseUrl as the base URL
     *        for the content. The base URL is used both to resolve relative URLs
     *        and when applying JavaScript's same origin policy. The historyUrl is
     *        used for the history entry.
     *
     * @param base_url: the URL to use as the page's base URL. If null defaults to
     *        'about:blank'. This value may be null.
     * @param data: the URL to use as the page's base URL. If null defaults to
     *        'about:blank'. This value may be null.
     * @param mime_type: the MIME type of the data, e.g. 'text/html'. This value
     *        may be null.
     * @param encoding: the encoding of the data This value may be null.
     * @param history_url: the URL to use as the history entry. If null defaults
     *        to 'about:blank'. If non-null, this must be a valid URL. This value
     *        may be null.
     */
    int LoadWithDataAndBaseUrl(const std::string& base_url, const std::string& data, const std::string& mime_type,
        const std::string& encoding, const std::string& history_url) override;

    /**
     * @brief Loads the given data into this NWeb.
     *
     * @param data: the URL to use as the page's base URL. If null defaults to
     *        'about:blank'. This value may be null.
     * @param mime_type: the MIME type of the data, e.g. 'text/html'. This value
     *        may be null.
     * @param encoding: the encoding of the data This value may be null.
     */
    int LoadWithData(const std::string& data, const std::string& mime_type, const std::string& encoding) override;

    /**
     * @brief RegisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list vector<String>: vector list ,method list
     * @param object_id int32_t: object id
     */
    void RegisterArkJSfunction(
        const std::string& object_name, const std::vector<std::string>& method_list, const int32_t object_id) override;

    /**
     * @brief UnregisterArkJSfunction
     *
     * @param object_name: objector name
     * @param method_list: vector list ,method list
     */
    void UnregisterArkJSfunction(const std::string& object_name, const std::vector<std::string>& method_list) override;

    /**
     * @brief SetNWebJavaScriptResultCallBack
     *
     * @param callback: callback client
     */
    void SetNWebJavaScriptResultCallBack(std::shared_ptr<OHOS::NWeb::NWebJavaScriptResultCallBack> callback) override;

    /**
     * @brief Set the NWebFindCallback that will receive find event. This will
     *        replace the current handler.
     *
     * @param find_listener: find callback
     */
    void PutFindCallback(std::shared_ptr<OHOS::NWeb::NWebFindCallback> find_listener) override;

    /**
     * @brief Finds all instances of find on the page and highlights them,
     *        asynchronously.
     *
     * @param search_str: target string to find.
     */
    void FindAllAsync(const std::string& search_str) override;

    /**
     * @brief Clears the highlighting surrounding text matches created by
     *        findAllAsync
     */
    void ClearMatches() override;

    /**
     * @brief Highlights and scrolls to the next match found by
     *        findAllAsync(String), wrapping around page boundaries as necessary.
     *
     * @param forward: find back or forward:
     */
    void FindNext(const bool forward) override;

    /**
     * @brief Saves the current view as a web archive.
     *
     * @param base_name: the filename where the archive should be placed This
     *        value cannot be null.
     * @param auto_name: if false, takes basename to be a file. If true, basename
     *        is assumed to be a directory in which a filename will be chosen
     *        according to the URL of the current page.
     */
    void StoreWebArchive(const std::string& base_name, bool auto_name,
        std::shared_ptr<OHOS::NWeb::NWebStringValueCallback> callback) override;

    /**
     * @brief creating two ends of a message channel.
     *
     * @return the web message ports get from nweb.
     */
    std::vector<std::string> CreateWebMessagePorts() override;

    /**
     * @brief Posts MessageEvent to the main frame.
     *
     * @param message: message send to mmain frame.
     * @param ports: the web message ports send to main frame.
     * @param target_uri: the uri which can received the ports.
     */
    void PostWebMessage(
        const std::string& message, const std::vector<std::string>& ports, const std::string& target_uri) override;

    /**
     * @brief close the message port.
     *
     * @param port_handle: the port to close.
     */
    void ClosePort(const std::string& port_handle) override;

    /**
     * @brief use the port to send message.
     *
     * @param port_handle: the port to send message.
     * @param data: the message to send.
     */
    void PostPortMessage(const std::string& port_handle, std::shared_ptr<OHOS::NWeb::NWebMessage> data) override;

    /**
     * @brief set the callback of the message port.
     *
     * @param port_handle: the port to set callback.
     * @param callback: to reveive the result when the other port post message.
     */
    void SetPortMessageCallback(
        const std::string& port_handle, std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback) override;

    void SendDragEvent(std::shared_ptr<OHOS::NWeb::NWebDragEvent> drag_event) override;

    /**
     * @brief Clear ssl cache.
     */
    void ClearSslCache() override;

    /**
     * @brief get web page url.
     *
     * @return web page url.
     */
    std::string GetUrl() override;

    /**
     * @brief Clears the client authentication certificate Cache in the Web.
     */
    void ClearClientAuthenticationCache() override;

    /**
     * @brief set the locale name of current system setting..
     *
     * @param region: the locale name of current system setting.
     */
    void UpdateLocale(const std::string& language, const std::string& region) override;

    /**
     * @brief get original url of the request.
     *
     * @return original url.
     */
    const std::string GetOriginalUrl() override;

    /**
     * @brief get original url of the request.
     *
     * @param data: raw image data of the icon.
     * @param width: width of the icon.
     * @param height: height of the icon.
     * @param color_type: the color type of the icon.
     * @param alpha_type: the alpha type of the icon.
     *
     * @return the result of get favicon.
     */
    bool GetFavicon(const void** data, size_t& width, size_t& height, ArkWebImageColorType& color_type,
        ArkWebImageAlphaType& alpha_type) override;

    /**
     * @brief set the network status, just notify the webview to change the JS
     *        navigatoer.online.
     *
     * @param available: width of the icon.
     */
    void PutNetworkAvailable(bool available) override;

    /**
     * @brief web has image or not.
     *
     * @param callback: has image or not
     */
    void HasImages(std::shared_ptr<OHOS::NWeb::NWebBoolValueCallback> callback) override;

    /**
     * @brief web remove cache.
     *
     * @param include_disk_files: if false, only the RAM cache is removed
     */
    void RemoveCache(bool include_disk_files) override;

    /**
     * @brief web has image or not.
     */
    std::shared_ptr<OHOS::NWeb::NWebHistoryList> GetHistoryList() override;

    /**
     * @brief Set the NWebReleaseSurfaceCallback that will receive release surface
     *        event. This will replace the current handler.
     *
     * @param release_surface_listener: release surface listener
     */
    void PutReleaseSurfaceCallback(
        std::shared_ptr<OHOS::NWeb::NWebReleaseSurfaceCallback> release_surface_listener) override;

    /**
     * @brief Get web back forward state.
     *
     * @return web back forward state.
     */
    std::vector<uint8_t> SerializeWebState() override;

    /**
     * @brief Restore web back forward state.
     *
     * @param state: web back forward state.
     */
    bool RestoreWebState(const std::vector<uint8_t>& state) override;

    /**
     * @brief Move page up.
     *
     * @param top: whether move to the top.
     */
    void PageUp(bool top) override;

    /**
     * @brief Move page down.
     *
     * @param bottom: whether move to the bottom.
     */
    void PageDown(bool bottom) override;

    /**
     * @brief Scroll to the position.
     *
     * @param x: horizontal coordinate.
     * @param y: vertical coordinate.
     */
    void ScrollTo(float x, float y) override;

    /**
     * @brief Scroll by the delta distance.
     *
     * @param delta_x: horizontal offset.
     * @param delta_y: vertical offset.
     */
    void ScrollBy(float delta_x, float delta_y) override;

    /**
     * @brief Slide scroll by the speed.
     *
     * @param vx: horizontal slide speed.
     * @param vy: vertical slide speed.
     */
    void SlideScroll(float vx, float vy) override;

    /**
     * @brief Get current website certificate.
     *
     * @param cert_chain_data: current website certificate array.
     * @param is_single_cert: true if only get one certificate of current website,
     *        false if get certificate chain of the website.
     *
     * @return true if get certificate successfully, otherwise false.
     */
    bool GetCertChainDerData(std::vector<std::string>& cert_chain_data, bool is_single_cert) override;

    /**
     * @brief Set screen offset.
     *
     * @param x: the offset in x direction.
     * @param y: the offset in y direction.
     */
    void SetScreenOffSet(double x, double y) override;

    /**
     * @brief Set audio muted.
     *
     * @param muted: Aduio mute state.
     */
    void SetAudioMuted(bool muted) override;

    /**
     * @brief Set should frame submission before draw.
     *
     * @param should: whether wait render frame submission.
     */
    void SetShouldFrameSubmissionBeforeDraw(bool should) override;

    /**
     * @brief Notify whether the popup window is initialized successfully.
     *
     * @param result: whether success.
     */
    void NotifyPopupWindowResult(bool result) override;

    /**
     * @brief Set audio resume interval.
     *
     * @param resume_interval: Aduio resume interval.
     */
    void SetAudioResumeInterval(int32_t resume_interval) override;

    /**
     * @brief Set audio exclusive state.
     *
     * @param audio_exclusive: Aduio exclusive state.
     */
    void SetAudioExclusive(bool audio_exclusive) override;

    /**
     * @brief Rigest the keep srceen on interface.
     *
     * @param window_id: the window id.
     * @param callback the screenon handle callback.
     */
    void RegisterScreenLockFunction(
        int32_t window_id, std::shared_ptr<OHOS::NWeb::NWebScreenLockCallback> callback) override;

    /**
     * @brief UnRigest the keep srceen on interface.
     *
     * @param window_id: the window id.
     */
    void UnRegisterScreenLockFunction(int32_t window_id) override;

    /**
     * @brief Notify memory level.
     *
     * @param level: the memory level.
     */
    void NotifyMemoryLevel(int32_t level) override;

    /**
     * @brief Notify webview window status.
     */
    void OnWebviewHide() override;

    void OnWebviewShow() override;

    /**
     * @brief Get drag data.
     *
     * @return the drag data.
     */
    std::shared_ptr<OHOS::NWeb::NWebDragData> GetOrCreateDragData() override;

    /**
     * @brief Prefetch the resources required by the page, but will not execute js
     *        or render the page.
     *
     * @param url: Which url to preresolve/preconnect.
     * @param additional_http_headers: Additional HTTP request header of the URL.
     */
    void PrefetchPage(
        const std::string& url, const std::map<std::string, std::string>& additional_http_headers) override;

    /**
     * @brief Set the window id.
     */
    void SetWindowId(uint32_t window_id) override;

    /**
     * @brief Notify that browser was occluded by other windows.
     */
    void OnOccluded() override;

    /**
     * @brief Notify that browser was unoccluded by other windows.
     */
    void OnUnoccluded() override;

    /**
     * @brief Set the token.
     */
    void SetToken(void* token) override;

    /**
     * @brief Set the nested scroll mode.
     */
    void SetNestedScrollMode(const ArkWebNestedScrollMode& nested_scroll_mode) override;

    /**
     * @brief Set enable lower the frame rate.
     */
    void SetEnableLowerFrameRate(bool enabled) override;

    /**
     * @brief Set the property values for width, height, and keyboard height.
     */
    void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) override;

    /**
     * @brief Set the virtual keyboard to override the web status.
     */
    bool ShouldVirtualKeyboardOverlay() override;

    /**
     * @brief Set draw rect.
     */
    void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) override;

    /**
     * @brief Set draw mode.
     */
    void SetDrawMode(int32_t mode) override;

    /**
     * @brief Create web print document adapter.
     */
    void* CreateWebPrintDocumentAdapter(const std::string& job_name) override;

    /**
     * @brief Loads the URL with postData using "POST" method into this WebView.
     *        If url is not a network URL, it will be loaded with loadUrl(String)
     * instead.
     *
     * @param url: the URL of the resource to load This value cannot be null.
     * @param post_data: the data will be passed to "POST" request, whilch must be
     *        "application/x-www-form-urlencoded" encoded.
     *
     * @return title string for the current page.
     */
    int PostUrl(const std::string& url, const std::vector<char>& post_data) override;

    /**
     * @brief Inject the JavaScript before WebView load the DOM tree.
     */
    void JavaScriptOnDocumentStart(const std::map<std::string, std::vector<std::string>>& script_items) override;

    /**
     * @brief Execute an accessibility action on an accessibility node in the
     *        browser.
     *
     * @param accessibility_id: The id of the accessibility node.
     * @param action: The action to be performed on the accessibility node.
     */
    void ExecuteAction(int64_t accessibility_id, uint32_t action) override;

    /**
     * @brief Get the information of the focused accessibility node on the given
     *        accessibility node in the browser.
     *
     * @param accessibility_id: Indicate the accessibility id of the parent node
     *        of the focused accessibility node.
     * @param is_accessibility_focus: Indicate whether the focused accessibility
     *        node is accessibility focused or input focused.
     *
     * @return The obtained information of the accessibility node.
     */
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> GetFocusedAccessibilityNodeInfo(
        int64_t accessibility_id, bool is_accessibility_focus) override;

    /**
     * @brief Get the information of the accessibility node by its accessibility
     *        id in the browser.
     *
     * @param accessibility_id: The accessibility id of the accessibility node.
     *
     * @return The obtained information of the accessibility node.
     */
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoById(
        int64_t accessibility_id) override;

    /**
     * @brief Get the information of the accessibility node by focus move in the
     *        browser.
     *
     * @param accessibility_id: The accessibility id of the original accessibility
     *        node.
     * @param direction: The focus move direction of the original accessibility
     *        node.
     *
     * @return The obtained information of the accessibility node.
     */
    std::shared_ptr<OHOS::NWeb::NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoByFocusMove(
        int64_t accessibility_id, int32_t direction) override;

    /**
     * @brief Set the accessibility state in the browser.
     *
     * @param state: Indicate whether the accessibility state is enabled or
     *        disabled.
     */
    void SetAccessibilityState(bool state) override;

    /**
     * @brief Get whether need soft keyboard.
     *
     * @return true if need soft keyboard, otherwise false.
     */
    bool NeedSoftKeyboard() override;

    /**
     * @brief Discard the webview window.
     *
     * @return true if the discarding success, otherwise false.
     */
    bool Discard() override;

    /**
     * @brief Reload the webview window that has been discarded before.
     *
     * @return true if the discarded window reload success, otherwise false.
     */
    bool Restore() override;

    /**
     * @brief Get the security level of current page.
     *
     * @return security level for current page.
     */
    int GetSecurityLevel() override;

    /**
     * @brief CallH5Function
     *
     * @param routingId         int32_t: the h5 frame routing id
     * @param h5ObjectId        int32_t: the h5 side object id
     * @param h5MethodName      string: the h5 side object method name
     * @param args              vector<shared_ptr<NWebValue>>: the call args
     */
    void CallH5Function(int32_t routingId, int32_t h5ObjectId, const std::string& h5MethodName,
        const std::vector<std::shared_ptr<OHOS::NWeb::NWebValue>>& args) override;

    /**
     * @brief Get web weather has been set incognito mode.
     *
     * @return true if web is in incognito mode; otherwise fase.
     */
    bool IsIncognitoMode() override;

    /**
     * @brief Register native function.
     */
    void RegisterNativeArkJSFunction(
        const char* objName, const std::vector<std::shared_ptr<OHOS::NWeb::NWebJsProxyCallback>>& callbacks) override;

    /**
     * @brief Unregister native function.
     */
    void UnRegisterNativeArkJSFunction(const char* objName) override;

    /**
     * @brief Register native valide callback function.
     */
    void RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback) override;

    /**
     * @brief Register native destroy callback function.
     */
    void RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnValidCallback callback) override;

    /**
     * @brief Inject the JavaScript after WebView loads the DOM tree and run
     *        JavaScripts.
     */
    void JavaScriptOnDocumentEnd(const std::map<std::string, std::vector<std::string>>& script_items) override;

    /**
     * @brief Enable the ability to check website security risks.Illegal and
     *        fraudulent websites are mandatory enabled and cann't be disabled by
     *        this function.
     */
    void EnableSafeBrowsing(bool enable) override;

    /**
     * @brief Get whether checking website security risks is enabled.
     *
     * @return true if enable the ability to check website security risks else
     *         false.
     */
    bool IsSafeBrowsingEnabled() override;

    /**
     * @brief Set the ability to print web page background.
     *
     * @param enable Indicate whether the ability is enabled or disabled.
     */
    void SetPrintBackground(bool enable) override;

    /**
     * @brief Obtains whether to print the background of a web page.
     *
     * @return true if enable print web page background, otherwise false.
     */
    bool GetPrintBackground() override;

    /**
     * @brief Close fullScreen video.
     */
    void CloseAllMediaPresentations() override;

    /**
     * @brief Stop all audio and video playback on the web page.
     */
    void StopAllMedia() override;

    /**
     * @brief Restart playback of all audio and video on the web page.
     */
    void ResumeAllMedia() override;

    /**
     * @brief Pause all audio and video playback on the web page.
     */
    void PauseAllMedia() override;

    /**
     * @brief View the playback status of all audio and video on the web page.
     *
     * @return The playback status of all audio and video.
     */
    int GetMediaPlaybackState() override;

    /**
     * Enable the ability to intelligent tracking prevention, default disabled.
     */
    void EnableIntelligentTrackingPrevention(bool enable) override;

    /**
     * Get whether intelligent tracking prevention is enabled.
     * @return true if enable the ability intelligent tracking prevention; else
     * false.
     */
    bool IsIntelligentTrackingPreventionEnabled() const override;

    /**
     * @brief Start current camera.
     */
    /*--ark web()--*/
    void StartCamera() override;

    /**
     * @brief Stop current camera.
     */
    /*--ark web()--*/
    void StopCamera() override;

    /**
     * @brief Close current camera.
     */
    /*--ark web()--*/
    void CloseCamera() override;

    /**
     * @brief Obtains the last javascript proxy calling frame url.
     *
     * @return the url of last calling frame url.
     */
    /*--ark web()--*/
    std::string GetLastJavascriptProxyCallingFrameUrl() override;

    /**
     * @brief get pendingsize status.
     *
     * @return the result of last pendingsize status.
     */
    /*--ark web()--*/
    bool GetPendingSizeStatus() override;

    /**
     * Scroll by the delta distance or velocity takes the screen as a reference.
     *
     * @param delta_x horizontal offset in physical pixel.
     * @param delta_y vertical offset in physical pixel.
     * @param vx      horizontal velocity in physical pixel.
     * @param vx      vertical velocity in physical pixel.
     */
    /*--ark web()--*/
    void ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy) override;

    /**
     * @brief ExecuteJavaScript with ashmem
     *
     * @param fd fd of the ashmem
     * @param scriptLength javascript code length
     * @param callback javascript running result
     * @param extention true if is extension
     */
    void ExecuteJavaScriptExt(const int fd, const size_t scriptLength,
        std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback, bool extention) override;

    /**
     * @brief Render process switch to background.
     */
    /*--ark web()--*/
    void OnRenderToBackground() override;

    /**
     * @brief Render process switch to foreground.
     */
    /*--ark web()--*/
    void OnRenderToForeground() override;

    /**
     * @brief Compile javascript and generate code cache.
     *
     * @param url url of javascript.
     * @param script javascript text content.
     * @param cacheOptions compile options and info.
     * @param callback callback will be called on getting the result of compiling
     * javascript.
     */
    /*--ark web()--*/
    void PrecompileJavaScript(const std::string& url, const std::string& script,
        std::shared_ptr<OHOS::NWeb::CacheOptions>& cacheOptions,
        std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback) override;

    void OnCreateNativeMediaPlayer(std::shared_ptr<OHOS::NWeb::NWebCreateNativeMediaPlayerCallback> callback) override;

    /**
     * @brief Send touchpad fling event.
     *
     * @param x location of x.
     * @param y location of y.
     * @param vx velocity of x.
     * @param vy velocity of y.
     */
    /*--ark web()--*/
    void SendTouchpadFlingEvent(double x, double y, double vx, double vy) override;

    /**
     * @brief Web drag resize optimize.
     */
    /*--ark web()--*/
    void DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width) override;

    void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) override;

    /**
     * @brief Inject offline resource into MemoryCache.
     *
     * @param url url of resource.
     * @param origin origin of resource.
     * @param resource data of resource.
     * @param response_headers response headers of resource.
     * @param type resource type.
     */
    /*--ark web()--*/
    void InjectOfflineResource(const std::string& url, const std::string& origin, const std::vector<uint8_t>& resource,
        const std::map<std::string, std::string>& responseHeaders, const int type) override;
    /**
     * @brief RegisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list vector<String>: vector list, sync method list
     * @param async_method_list vector<String>: vector list, async method list
     * @param object_id int32_t: object id
     */
    /*--ark web()--*/
    void RegisterArkJSfunction(const std::string& object_name, const std::vector<std::string>& method_list,
        const std::vector<std::string>& async_method_list, const int32_t object_id) override;

    /**
     * @brief Set fit content mode.
     */
    void SetFitContentMode(int32_t mode) override;

    /**
     * @brief Terminate render process
     *
     * @return true if it was possible to terminate this render process, false
     * otherwise.
     */
    /*--ark web()--*/
    bool TerminateRenderProcess() override;

    /**
     * @brief Get select info.
     */
    std::string GetSelectInfo() override;

    /**
     * @brief Render process switch to foreground.
     */
    /*--ark web()--*/
    void OnOnlineRenderToForeground() override;

    /**
     * @brief Set the params when the scale of WebView changed by pinch gestrue.
     *
     * @param scale: the scale factor to apply. The scale will be
     *        clamped to the pinch limits. This value must be in the range
     *        0.01 to 8.0 inclusive.
     * @param centerX: X-coordinate of the pinch center
     * @param centerX: Y-coordinate of the pinch center
     *
     * @return the error id.
     */
    /*--ark web()--*/
    int ScaleGestureChange(double scale, double centerX, double centerY) override;

    /**
     * Get value of Autofill index.
     * @param index index value.
     */
    /*--ark web()--*/
    void SuggestionSelected(int32_t index) override;

    /**
     * @brief Notify that safe insets changed.
     */
    /*--ark web()--*/
    void OnSafeInsetsChange(int left, int top, int right, int bottom) override;

    /**
     * @brief Called when text is selected in image.
     */
    /*--ark web()--*/
    void OnTextSelected() override;

    /**
     * @brief Notify for next touch move event.
     */
    /*--ark web()--*/
    void NotifyForNextTouchEvent() override;

    /**
     * @brief Enable the ability to block Ads, default disabled.
     */
    /*--ark web()--*/
    void EnableAdsBlock(bool enable) override;

    /**
     * @brief Get whether Ads block is enabled.
     */
    /*--ark web()--*/
    bool IsAdsBlockEnabled() override;

    /**
     * @brief Get whether Ads block is enabled for current Webpage.
     */
    /*--ark web()--*/
    bool IsAdsBlockEnabledForCurPage() override;

    /**
     * @brief Get Web page snapshot
     *
     * @param id Request id.
     * @param type Request snapshot pixel unit.
     * @param width Request SnapShot width.
     * @param height Request SnapShot height.
     * @param callback SnapShot result callback.
     * @return ture if succuess request snapshot to renderer.
     */
    /*--ark web()--*/
    bool WebPageSnapshot(const char* id,
                         ArkPixelUnit type,
                         int width,
                         int height,
                         const WebSnapshotCallback callback) override;

    /**
     * @brief Notify that system configuration changed.
     *
     * @param configuration system configuration.
     */
    /*--ark web()--*/
    void OnConfigurationUpdated(std::shared_ptr<OHOS::NWeb::NWebSystemConfiguration> configuration) override;

    /**
     * @brief Set url trust list.
     */
    /*--ark web()--*/
    int SetUrlTrustList(const std::string& urlTrustList) override;

    /**
     * @brief Put the callback for convert spanstring to html.
     *
     * @param callback will convert spanstring to html.
     */
    /*--ark web()--*/
    void PutSpanstringConvertHtmlCallback(
        std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback) override;

    /**
     * Web send key event.
     * @param key_code code value.
     * @param key_action action value.
     * @param pressedCodes pressedCodes value.
     */
    /*--ark web()--*/
    bool WebSendKeyEvent(int32_t key_code, int32_t key_action,
        const std::vector<int32_t>& pressedCodes) override;

    /**
     * @brief Set grant file access dirs.
     */
    /*--ark web()--*/
    void SetPathAllowingUniversalAccess(const std::vector<std::string>& dirs,
        const std::vector<std::string>& moduleName, std::string& errorPath) override;

    /**
     * @brief Send mouse wheel event.
     */
    /*--ark web()--*/
    void WebSendMouseWheelEvent(double x,
                                double y,
                                double delta_x,
                                double delta_y,
                                const std::vector<int32_t>& pressedCodes) override;

    /**
     * @brief Send touchpad fling event.
     *
     * @param x location of x.
     * @param y location of y.
     * @param vx velocity of x.
     * @param vy velocity of y.
     * @param pressedCodes pressed codes.
     */
    /*--ark web()--*/
    void WebSendTouchpadFlingEvent(double x,
                                   double y,
                                   double vx,
                                   double vy,
                                   const std::vector<int32_t>& pressedCodes) override;

    /**
     * @brief Set url trust list with error message.
     */
    /*--ark web()--*/
    int SetUrlTrustListWithErrMsg(const std::string& urlTrustList, std::string& detailErrMsg) override;

    /**
     * @brief resize visual viewport.
     *
     * @param width width.
     * @param height height.
     * @param iskeyboard from keybord.
     */
    /*--ark web()--*/
    void ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard) override;

    /**
     * @brief set backforward cache options.
     * @param size The size of the back forward cache could saved.
     * @param timeToLive The time of the back forward cache page could stay.
     */
    /*--ark web()--*/
    void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) override;

    /**
     * @brief RegisterArkJSfunction
     *
     * @param object_name String: objector name
     * @param method_list vector<String>: vector list, sync method list
     * @param async_method_list vector<String>: vector list, async method list
     * @param object_id int32_t: object id
     * @param permission String: allow list
     */
    /*--ark web()--*/
    void RegisterArkJSfunction(const std::string& object_name,
                               const std::vector<std::string>& method_list,
                               const std::vector<std::string>& async_method_list,
                               const int32_t object_id,
                               const std::string& permission) override;

    /**
     * @brief set the callback of the autofill event.
     * @param callback callback.
     */
    /*--ark web()--*/
    void SetAutofillCallback(std::shared_ptr<OHOS::NWeb::NWebMessageValueCallback> callback) override;

    /**
     * @brief fill autofill data.
     * @param data data.
     */
    /*--ark web()--*/
    void FillAutofillData(std::shared_ptr<OHOS::NWeb::NWebMessage> data) override;

    /**
     * @brief on autofill cancel.
     * @param fillContent fillContent.
     */
    /*--ark web()--*/
    void OnAutofillCancel(const std::string& fillContent) override;

    /**
     * Execute an accessibility action on an accessibility node in the browser.
     * @param accessibilityId The id of the accessibility node.
     * @param action The action to be performed on the accessibility node.
     * @param actionArguments Data related to the current action.
     */
    /*--ark web()--*/
    void PerformAction(int64_t accessibilityId, uint32_t action,
        const std::map<std::string, std::string>& actionArguments) override;

    /**
     * @brief Send the accessibility hover event coordinate.
     *
     * @param x horizontal location of coordinate.
     * @param y vertical location of coordinate.
     */
    /*--ark web()--*/
    void SendAccessibilityHoverEvent(int32_t x, int32_t y) override;

    /**
     * @brief Get the current scroll offset of the webpage.
     * @param offset_x The current horizontal scroll offset of the webpage.
     * @param offset_y The current vertical scroll offset of the webpage.
     */
    /*--ark web()--*/
    void GetScrollOffset(float* offset_x, float* offset_y) override;

    /**
     * @brief ExecuteCreatePDF
     *
     * @param pdfConfig The current configuration when creating pdf.
     * @param callback NWebArrayBufferValueCallback: CreatePDF running result.
     */
    /*--ark web()--*/
    void ExecuteCreatePDFExt(std::shared_ptr<OHOS::NWeb::NWebPDFConfigArgs> pdfConfig,
        std::shared_ptr<OHOS::NWeb::NWebArrayBufferValueCallback> callback) override;

    /**
     * @brief Scroll by the delta distance.
     * @param delta_x: horizontal offset.
     * @param delta_y: vertical offset.
     * @return false if web don't scroll by the delta, when web is focused.
     */
    /*--ark web()--*/
    bool ScrollByWithResult(float delta_x, float delta_y) override;

    /**
     * @brief Called when image analyzer is destory.
     */
    /*--ark web()--*/
    void OnDestroyImageAnalyzerOverlay() override;

    /**
     * Scroll to the position.
     *
     * @param x horizontal coordinate.
     * @param y vertical coordinate.
     * @param duration: anime duration.
     */
    /*--ark web()--*/
    void ScrollToWithAnime(float x, float y, int32_t duration) override;

    /**
     * Scroll by the delta distance.
     *
     * @param delta_x: horizontal offset.
     * @param delta_y: vertical offset.
     * @param duration: anime duration.
     */
    /*--ark web()--*/
    void ScrollByWithAnime(float delta_x, float delta_y, int32_t duration) override;

    /**
     * @brief set a popupSurface to draw popup content
     * @param popupSurface popupSurface.
     */
    /*--ark web()--*/
    void SetPopupSurface(void* popupSurface) override;

    /**
     * @Description: Sends mouse events to the web kernel.
     * @Input mouseEvent: Basic information about mouse events.
     * @Since: 12005
     */
    /*--ark web()--*/
    void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) override;

    /**
     * @Description: Get the accessibility visibility of the accessibility node by its accessibility id in the browser.
     * @Input accessibility_id: The accessibility id of the accessibility node.
     * @Return: The accessibility visibility of the accessibility node.
     * @Since: 12005
     */
    /*--ark web()--*/
    bool GetAccessibilityVisible(int64_t accessibility_id) override;

    /**
     * @Description: Set the rotation to psurface.
     * @Input rotation: The rotation of buffer.
     * @Since: 12005
     */
    /*--ark web()--*/
    void SetTransformHint(uint32_t rotation) override;

    /**
     * @brief Web components blur when the keyboard is hidden by gesture back.
     */
    /*--ark web()--*/
    void WebComponentsBlur() override;

    /**
     * @Description: Get the GPU memory size used by web.
     * @Return: Total size of web GPU.
     */
    /*--ark web()--*/
    float DumpGpuInfo() override;

    /**
     * @brief Set the params when the scale of WebView changed by pinch gestrue.
     *
     * @param type: gesture status
     * @param scale: the scale factor to apply. The scale will be
     *        clamped to the pinch limits. This value must be in the range
     *        0.01 to 8.0 inclusive.
     * @param originScale: the origin scale factor to apply. The scale will be
     *        clamped to the pinch limits. This value must be in the range
     *        0.01 to 8.0 inclusive.
     * @param centerX: X-coordinate of the pinch center
     * @param centerX: Y-coordinate of the pinch center
     *
     * @return the error id.
     */
    int ScaleGestureChangeV2(int type, double scale, double originScale, double centerX, double centerY) override;

    /**
     * @Description: Sends key events to the web kernel.
     * @Input mouseEvent: Basic information about key events.
     */
    /*--ark web()--*/
    bool SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) override;

    /**
     * @Description: Execute an accessibility action on an accessibility node in the browser.
     * @Input accessibilityId: The id of the accessibility node.
     * @Input action: The action to be performed on the accessibility node.
     * @Input actionArguments: Data related to the current action.
     * @Return: Whether the action is performed successfully.
     */
    /*--ark web()--*/
    bool PerformActionV2(int64_t accessibilityId, uint32_t action,
        const std::map<std::string, std::string>& actionArguments) override;

    /**
     * @brief Inject the JavaScript before WebView load the DOM tree.
     */
    void JavaScriptOnDocumentStartByOrder(const std::map<std::string, std::vector<std::string>>& script_items,
        const std::vector<std::string>& script_items_by_order) override;

    /**
     * @brief Inject the JavaScript after WebView loads the DOM tree and run
     *        JavaScripts.
     */
    void JavaScriptOnDocumentEndByOrder(const std::map<std::string, std::vector<std::string>>& script_items,
        const std::vector<std::string>& script_items_by_order) override;

    /**
     * @Description: Check web component active policy disable, default: false
     * @Return: Whether the policy is disable.
     */
    /*--ark web()--*/
    virtual bool IsActivePolicyDisable() override;

    /**
     * @Description: Inject the JavaScript when the head element has been created.
     * @Input scriptItems: The injected JavaScript code is stored in lexicographical order.
     * @Input scriptItemsByOrder: The injected JavaScript code is stored in the order of the injection array.
     */
    /*--ark web()--*/
    void JavaScriptOnHeadReadyByOrder(const std::map<std::string, std::vector<std::string>>& script_items,
        const std::vector<std::string>& script_items_by_order) override;

    /**
     * @Description: Optimize HTML parser budget to reduce FCP time.
     * @Input enable: Set whether to use optimized parser budget.
     */
    void PutOptimizeParserBudgetEnabled(bool enable) override;

    /**
     * @Description: Get the bounding rectangle of the accessibility node of the given id.
     * @Input accessibilityId: The id of the accessibility node.
     * @Output width: The width of the rectangle.
     * @Output height: The height of the rectangle.
     * @Output offsetX: The X-coordinate offset of the rectangle.
     * @Output offsetY: The Y-coordinate offset of the rectangle.
     * @Return: Whether the bounding rectangle is obtained successfully.
     */
    /*--ark web()--*/
    bool GetAccessibilityNodeRectById(
        int64_t accessibilityId, int32_t* width, int32_t* height, int32_t* offsetX, int32_t* offsetY) override;

    /**
     * @brief Gets the last hit test result.
     *
     * @return the last HitTestResult
     */
    std::shared_ptr<OHOS::NWeb::HitTestResult> GetLastHitTestResult() override;

    /**
     * @Description: Get the current language in the webview.
     * @Return: The current language in the webview.
     */
    std::string GetCurrentLanguage() override;

    /*
     * @brief Send mouse wheel event with sourceTool info.
     */
    /*--ark web()--*/
    bool WebSendMouseWheelEventV2(double x, double y, double delta_x, double delta_y,
        const std::vector<int32_t> &pressedCodes, int32_t source) override;

    /**
     * @brief judge if browser use drag resize.
     */
    bool IsNWebEx() override;

    /**
     * @brief Set enable Half the frame rate.
     */
    void SetEnableHalfFrameRate(bool enabled) override;

    /**
     * @brief Web maximize resize optimize.
     */
    /*--ark web()--*/
    void MaximizeResize() override;

    /**
     * @brief Try to attach web inputmethod after drag.
     */
    void OnDragAttach() override;

    /**
     * Set focus by position
     * 
     * @Return: if hit node editable.
     */
    bool SetFocusByPosition(float x, float y) override;

    /**
     * @brief set DPI when DPI changes.
     * @param density The new density value.
     */
    /*--ark web()--*/
    void SetSurfaceDensity(const double& density) override;

    /**
     * @brief When the user sets the webpage's border radius,
     *        update Chromium with this radius value for repaint the scrollbar.
     * @param borderRadiusTopLeft: Radius value of the rounded corner in the top-left of the webpage.
     * @param borderRadiusTopRight: Radius value of the rounded corner in the top-right of the webpage.
     * @param borderRadiusBottomLeft: Radius value of the rounded corner in the bottom-left of the webpage.
     * @param borderRadiusBottomRight: Radius value of the rounded corner in the bottom-right of the webpage.
     */
    void SetBorderRadiusFromWeb(double borderRadiusTopLeft, double borderRadiusTopRight, double borderRadiusBottomLeft,
        double borderRadiusBottomRight) override;

    /**
     * @brief Set the native inner web
     */
    void SetNativeInnerWeb(bool isInnerWeb) override;

    /**
     * @brief Send the accessibility hover event coordinate.
     *
     * @param x horizontal location of coordinate.
     * @param y vertical location of coordinate.
     * @param isHoverEnter whether the accessibility hover event is a hover enter event.
     */
    /*--ark web()--*/
    void SendAccessibilityHoverEventV2(int32_t x, int32_t y, bool isHoverEnter) override;

    /**
     * @brief Notify browser is foreground.
     */
    void OnBrowserForeground() override;

    /**
     * @brief Notify browser is background.
     */
    void OnBrowserBackground() override;

    /**
     * @brief: register native javaScriptProxy.
     *
     * @param objName object name.
     * @param methodName methodName list
     * @param data The ptr of NWebJsProxyMethod.
     * @param isAsync True mean.
     * @param permission permission.
     */
    virtual void RegisterNativeJavaScriptProxy(const std::string& objName,
        const std::vector<std::string>& methodName,
        std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data,
        bool isAsync,
        const std::string& permission) override;

    /**
     * @brief Set the window id.
     */
    void SetFocusWindowId(uint32_t focus_window_id) override;

    /**
     * @brief Run data detector JS
     */
    void RunDataDetectorJS() override;

    /**
     * @brief Set data detector enable.
     */
    void SetDataDetectorEnable(bool enable) override;

    /**
     * @brief On data detector select text.
     */
    void OnDataDetectorSelectText() override;

    /**
     * @brief On data detector copy.
     */
    void OnDataDetectorCopy(const std::vector<std::string>& recordMix) override;

    /**
     * @brief Set the native window of picture in picture.
     */
    void SetPipNativeWindow(int delegate_id,
                            int child_id,
                            int frame_routing_id,
                            void* window) override;

    /**
     * @brief Send event of picture in picture.
     */
    void SendPipEvent(int delegate_id,
                      int child_id,
                      int frame_routing_id,
                      int event) override;

    /*
     * @brief Set unique key of current page for insert frame.
     *
     * @param key The unique key of current page.
     */
    void SetBlanklessLoadingKey(const std::string& key) override;

    /**
     * @brief Set privacy status.
     *
     * @param isPrivate bool: privacy status page.
     */
    void SetPrivacyStatus(bool isPrivate) override;

    /**
     * @brief Get select startIndex.
     */
    int GetSelectStartIndex() override;

    /**
     * @brief Get select endIndex.
     */
    int GetSelectEndIndex() override;

    /**
     * @brief Get all text info.
     */
    std::string GetAllTextInfo() override;

    /**
     * @brief Set audio session type.
     *
     * @param audio_session_type Audio session type.
     */
    void SetAudioSessionType(int32_t audio_session_type) override;

        /**
     * @brief Get accessibility id by its html element id in the browser.
     * @param htmlElementId The html element id of the Same-layer rendering.
     * @return The accessibility id of the accessibility node with Same-layer rendering.
     */
    int64_t GetWebAccessibilityIdByHtmlElementId(const std::string& htmlElementId) override;

    /**
     * @brief Get the prediction info of blankless loading on the current page.
     *
     * @param key The unique key of current page.
     * @param similarity The historical snapshot similarity.
     * @param loadingTime The historical loading time.
     * @return The error code.
     */
    int32_t GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime) override;

    /**
     * @brief Set whether to enable blankless loading on the current page.
     *
     * @param key The unique key of current page.
     * @param isStart Whether to enable blankless loading.
     * @return The error code.
     */
    int32_t SetBlanklessLoadingWithKey(const std::string& key, bool isStart) override;

    /**
     * @brief Update the single handle visible.
     * @param isVisible The single handle visible.
     */
    void UpdateSingleHandleVisible(bool isVisible) override;

    /**
     * @brief Set the state of touch handle when it exists.
     * @param touchHandleExist The state of the touch handle, Which is true if the touch handle exists.
     */
    void SetTouchHandleExistState(bool touchHandleExist) override;

    /**
     * @brief Sets the bottom avoidance height of the web visible viewport.
     * @param avoidHeight The height value of the visible viewport avoidance. Unit: px.
     */
    void AvoidVisibleViewportBottom(int32_t avoidHeight) override;

    /**
     * @brief Get the bottom avoidance height of the web visible viewport.
     * @return The bottom avoidance height of the visible viewport.
     */
    int32_t GetVisibleViewportAvoidHeight() override;

    /**
     * @brief Try to trigger blankless for url.
     * @param url The url to use for blankless.
     * @return Blankless is triggered for this url.
     */
    bool TriggerBlanklessForUrl(const std::string& url) override;

    /**
     * @brief Set visibility of the web.
     * @param isVisible The visibility to be set.
     */
    void SetVisibility(bool isVisible) override;

    /**
     * @brief Current viewport is being scaled.
     */
    void SetViewportScaleState() override;

    /**
    * @brief Get the current scroll offset of the webpage.
    * @param offset_x The current horizontal scroll offset of the webpage.
    * @param offset_y The current vertical scroll offset of the webpage.
    */
   /*--ark web()--*/
   void GetPageOffset(float* offset_x, float* offset_y) override;

    /**
     * @brief Set whether enable the error page. onOverrideErrorPage will be triggered when the page error.
     *
     * @param enable bool: Whether enable the error page.
     */
    void SetErrorPageEnabled(bool enable) override;

    /**
     * @brief Get whether default error page feature is enabled.
     */
    bool GetErrorPageEnabled() override;

    /**
     * @brief Get web component destroy mode.
     * @return The web destroy mode.
     */
    ArkWebDestroyMode GetWebDestroyMode() override;

    void CallH5FunctionV2(int32_t routing_id, int32_t h5_object_id, const std::string& h5_method_name,
        const std::vector<std::shared_ptr<OHOS::NWeb::NWebRomValue>>& args) override;

    void PostPortMessageV2(const std::string& portHandle, std::shared_ptr<OHOS::NWeb::NWebRomValue> data) override;

    void FillAutofillDataV2(std::shared_ptr<OHOS::NWeb::NWebRomValue> data) override;

    /**
     * @brief Create web print document adapter V2.
     */
    std::unique_ptr<OHOS::NWeb::NWebPrintDocumentAdapterAdapter> CreateWebPrintDocumentAdapterV2(
        const std::string& job_name) override;

    /**
     * @brief Handle stylus touch press event.
     * @param stylus_touch_point_info The NWeb stylus touch point information object.
     * @param from_overlay Indicates whether the event comes from an overlay layer.
     */
    void OnStylusTouchPress(
        std::shared_ptr<OHOS::NWeb::NWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay) override;

    /**
     * @brief Handle stylus touch release event.
     * @param stylus_touch_point_info The NWeb stylus touch point information object.
     * @param from_overlay Indicates whether the event comes from an overlay layer.
     */
    void OnStylusTouchRelease(
        std::shared_ptr<OHOS::NWeb::NWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay) override;

    /**
     * @brief Handle stylus touch move event.
     * @param stylus_touch_point_infos The vector of NWeb stylus touch point information objects.
     * @param from_overlay Indicates whether the event comes from an overlay layer.
     */
    void OnStylusTouchMove(
        const std::vector<std::shared_ptr<OHOS::NWeb::NWebStylusTouchPointInfo>>& stylus_touch_point_infos,
        bool from_overlay) override;

    /**
     * @brief Record the blankless frame size of the web.
     * @param width The width of the blankless frame.
     * @param height The height of the blankless frame.
     */
    void RecordBlanklessFrameSize(uint32_t width, uint32_t height) override;

    /**
     * @brief Prefetch the resources required by the page, but will not execute js or
     *        render the page.
     * @param url:  String: Which url to preresolve/preconnect.
     * @param additional_http_headers: Additional HTTP request header of the URL.
     * @param minTimeBetweenPrefetchesMs: djusting the throttling interval.
     * @param ignoreCacheControlNoStore:  true if ignoring Cache-Control: no-store.
     */
    void PrefetchPageV2(
        const std::string& url, const std::map<std::string, std::string>& additional_http_headers,
        int32_t minTimeBetweenPrefetchesMs, bool ignoreCacheControlNoStore) override;

    /**
     * @brief Set web should ignore zoom limit.
     */
    void SetForceEnableZoom(bool forceEnableZoom) const override;

    /**
     * @brief Set the config for blank screen detection.
     *
     * @param enable Whether the blank screen detection is enabled.
     * @param detectionTiming The timing of the blank screen detection.
     * @param detectionMethods The methods of the blank screen detection.
     * @param contentfulNodesCountThreshold The contentful nodes count threshold of the blank screen detection.
     */
    void SetBlankScreenDetectionConfig(bool enable, const std::vector<double>& detectionTiming,
        const std::vector<int32_t>& detectionMethods, int32_t contentfulNodesCountThreshold) override;

    /**
     * @brief Get Web page snapshot V2
     *
     * @param id Request id.
     * @param type Request snapshot pixel unit.
     * @param width Request SnapShot width.
     * @param height Request SnapShot height.
     * @param callback SnapShot result callback.
     * @return ture if succuess request snapshot to renderer.
     */
    /*--ark web()--*/
    bool WebPageSnapshotV2(const char* id,
                           ArkPixelUnit type,
                           int width,
                           int height,
                           std::shared_ptr<OHOS::NWeb::NWebSnapshotCallback> callback) override;

    /**
     * @brief Set web should stop fling.
     */
    /*--ark web()--*/
    void StopFling() override;

    /**
     * @brief Resume current microphone.
     */
    /*--ark web()--*/
    void ResumeMicrophone() override;

    /**
     * @brief Stop current microphone.
     */
    /*--ark web()--*/
    void StopMicrophone() override;

    /**
     * @brief Pause current microphone.
     */
    /*--ark web()--*/
    void PauseMicrophone() override;

    /**
     * @brief Inject the JavaScript before WebView load the DOM tree.
     *
     * @param script_items: Multiple injected JavaScript codes are stored in a map in lexicographical order.
     * @param script_regex_items: Multiple injected regular expression rule codes are
     *                            stored in a map in lexicographical order.
     * @param script_items_by_order: Multiple injected JavaScript codes are stored in the order of injection.
     */
    void JavaScriptOnDocumentStartByOrderV2(const std::map<std::string, std::vector<std::string>>& script_items,
        const std::map<std::string, std::vector<std::pair<std::string, std::string>>>& script_regex_items,
        const std::vector<std::string>& script_items_by_order) override;

    /**
     * @brief Inject the JavaScript after WebView loads the DOM tree and run
     *        JavaScripts.
     *
     * @param script_items: Multiple injected JavaScript codes are stored in a map in lexicographical order.
     * @param script_regex_items: Multiple injected regular expression rule codes are
     *                            stored in a map in lexicographical order.
     * @param script_items_by_order: Multiple injected JavaScript codes are stored in the order of injection.
     */
    void JavaScriptOnDocumentEndByOrderV2(const std::map<std::string, std::vector<std::string>>& script_items,
        const std::map<std::string, std::vector<std::pair<std::string, std::string>>>& script_regex_items,
        const std::vector<std::string>& script_items_by_order) override;

    /**
     * @Description: Inject the JavaScript when the head element has been created.
     * @Input scriptItems: The injected JavaScript code is stored in lexicographical order.
     * @Input script_regex_items: Multiple injected regular expression rule codes are
     *                            stored in a map in lexicographical order.
     * @Input scriptItemsByOrder: The injected JavaScript code is stored in the order of the injection array.
     */
    /*--ark web()--*/
    void JavaScriptOnHeadReadyByOrderV2(const std::map<std::string, std::vector<std::string>>& script_items,
        const std::map<std::string, std::vector<std::pair<std::string, std::string>>>& script_regex_items,
        const std::vector<std::string>& script_items_by_order) override;

    /**
     * @brief Put the callback, get plain text from password vault.
     *
     * @param callback get plain text from password vault.
     */
    void PutVaultPlainTextCallback(
        std::shared_ptr<OHOS::NWeb::NWebVaultPlainTextCallback> callback) override;
    
    /**
     * @brief fill autofill data.
     *
     * @param data data.
     * @param type type.
     */
    void FillAutofillDataFromTriggerType(
        std::shared_ptr<OHOS::NWeb::NWebRomValue> data, const ArkWebAutoFillTriggerType& type) override;

    /**
     * @brief Set soft keyboard behavior mode.
     * @param mode WebSoftKeyboardBehaviorMode: the soft keyboard behavior mode.
     */
    void SetSoftKeyboardBehaviorMode(OHOS::NWeb::WebSoftKeyboardBehaviorMode mode) override;

    /**
     * @brief Get NWebAgentManager
     */
    std::shared_ptr<OHOS::NWeb::NWebAgentManager> GetAgentManager() override;

    /**
     * @brief Set NWebAgentHandler
     */
    void SetNWebAgentHandler(std::shared_ptr<OHOS::NWeb::NWebAgentHandler> handler) override;

    /**
     * @brief Send command action to nweb.
     *
     * @param action The action of msdp's command.
     * @return The result of command.
     */
    int32_t SendCommandAction(std::shared_ptr<OHOS::NWeb::NWebCommandAction> action) override;

    /*
     * @brief Set unique key of current page for insert frame.
     *
     * @param key string: the unique key of current page.
     * @param enable Whether to enable frame interpolation.
     * @param duration The duration time of frame interpolation.
     * @param expirationTime Indicates the time when the historical frame interpolation expires.
     * @param callback Called after the frame insertion successful, failed, or removed.
     */
    int32_t SetBlanklessLoadingParams(const std::string& key, bool enable, int32_t duration,
        int64_t expirationTime, std::shared_ptr<OHOS::NWeb::NWebBlanklessCallback> callback) override;

    /**
     * @brief Sends state to the web kernel to execute blankless callback.
     *
     * @param state Frame insertion successful, failed, or removed.
     * @param reason Reason of frame insertion failed.
     */
    void CallExecuteBlanklessCallback(int32_t state, const std::string& reason) override;
private:
    ArkWebRefPtr<ArkWebNWeb> ark_web_nweb_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NWEB_WRAPPER_H_
