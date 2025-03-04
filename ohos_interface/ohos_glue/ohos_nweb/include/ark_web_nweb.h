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

#ifndef ARK_WEB_NWEB_H_
#define ARK_WEB_NWEB_H_
#pragma once

#include "ohos_nweb/include/ark_web_accessibility_event_callback.h"
#include "ohos_nweb/include/ark_web_accessibility_node_info.h"
#include "ohos_nweb/include/ark_web_array_buffer_value_callback.h"
#include "ohos_nweb/include/ark_web_bool_value_callback.h"
#include "ohos_nweb/include/ark_web_cache_options.h"
#include "ohos_nweb/include/ark_web_create_native_media_player_callback.h"
#include "ohos_nweb/include/ark_web_download_callback.h"
#include "ohos_nweb/include/ark_web_drag_data.h"
#include "ohos_nweb/include/ark_web_drag_event.h"
#include "ohos_nweb/include/ark_web_find_callback.h"
#include "ohos_nweb/include/ark_web_history_list.h"
#include "ohos_nweb/include/ark_web_hit_test_result.h"
#include "ohos_nweb/include/ark_web_js_proxy_callback_vector.h"
#include "ohos_nweb/include/ark_web_js_result_callback.h"
#include "ohos_nweb/include/ark_web_keyboard_event.h"
#include "ohos_nweb/include/ark_web_message_value_callback.h"
#include "ohos_nweb/include/ark_web_pdfconfig_args.h"
#include "ohos_nweb/include/ark_web_preference.h"
#include "ohos_nweb/include/ark_web_release_surface_callback.h"
#include "ohos_nweb/include/ark_web_screen_lock_callback.h"
#include "ohos_nweb/include/ark_web_spanstring_convert_html_callback.h"
#include "ohos_nweb/include/ark_web_string_value_callback.h"
#include "ohos_nweb/include/ark_web_system_configuration.h"
#include "ohos_nweb/include/ark_web_touch_point_info_vector.h"

namespace OHOS::ArkWeb {

class ArkWebHandler;

/*--ark web(source=webcore)--*/
class ArkWebNWeb : public virtual ArkWebBaseRefCounted {
 public:
  /*--ark web()--*/
  virtual void Resize(uint32_t width, uint32_t height, bool is_keyboard) = 0;

  /*--ark web()--*/
  virtual void OnPause() = 0;

  /*--ark web()--*/
  virtual void OnContinue() = 0;

  /*--ark web()--*/
  virtual void OnDestroy() = 0;

  /*--ark web()--*/
  virtual void OnFocus(const int32_t& focus_reason) = 0;

  /*--ark web()--*/
  virtual void OnBlur(const int32_t& blur_reason) = 0;

  /*--ark web()--*/
  virtual void OnTouchPress(int32_t id,
                            double x,
                            double y,
                            bool from_overlay) = 0;

  /*--ark web()--*/
  virtual void OnTouchRelease(int32_t id,
                              double x,
                              double y,
                              bool from_overlay) = 0;

  /*--ark web()--*/
  virtual void OnTouchMove(int32_t id,
                           double x,
                           double y,
                           bool from_overlay) = 0;

  /*--ark web()--*/
  virtual void OnTouchMove(const ArkWebTouchPointInfoVector& touch_point_infos,
                           bool from_overlay) = 0;

  /*--ark web()--*/
  virtual void OnTouchCancel() = 0;

  /*--ark web()--*/
  virtual void OnNavigateBack() = 0;

  /*--ark web()--*/
  virtual bool SendKeyEvent(int32_t key_code, int32_t key_action) = 0;

  /*--ark web()--*/
  virtual void SendMouseWheelEvent(double x,
                                   double y,
                                   double delta_x,
                                   double delta_y) = 0;

  /*--ark web()--*/
  virtual void SendMouseEvent(int x,
                              int y,
                              int button,
                              int action,
                              int count) = 0;

  /**
   * @brief Loads the given URL.
   *
   * @param url: the URL of the resource to load This value cannot be null.
   *
   * @return title string for the current page.
   */
  /*--ark web()--*/
  virtual int Load(const ArkWebString& url) = 0;

  /**
   * @brief Gets whether this NWeb has a back history item.
   *
   * @return true if this NWeb has a back history item
   */
  /*--ark web()--*/
  virtual bool IsNavigatebackwardAllowed() = 0;

  /**
   * @brief Gets whether this NWeb has a forward history item.
   *
   * @return true if this NWeb has a forward history item
   */
  /*--ark web()--*/
  virtual bool IsNavigateForwardAllowed() = 0;

  /**
   * @brief Gets whether this NWeb has a back or forward history item for number
   *        of steps.
   *
   * @param num_steps: the negative or positive number of steps to move the
   *        history
   *
   * @return true if this NWeb has a forward history item
   */
  /*--ark web()--*/
  virtual bool CanNavigateBackOrForward(int num_steps) = 0;

  /**
   * @brief Goes back in the history of this NWeb.
   */
  /*--ark web()--*/
  virtual void NavigateBack() = 0;

  /**
   * @brief Goes forward in the history of this NWeb.
   */
  /*--ark web()--*/
  virtual void NavigateForward() = 0;

  /**
   * @brief Goes to the history item that is the number of steps away from the
   *        current item.
   */
  /*--ark web()--*/
  virtual void NavigateBackOrForward(int step) = 0;

  /**
   * @brief Delete back and forward history list.
   */
  /*--ark web()--*/
  virtual void DeleteNavigateHistory() = 0;

  /**
   * @brief Reloads the current URL.
   */
  /*--ark web()--*/
  virtual void Reload() = 0;

  /**
   * @brief Performs a zoom operation in this NWeb.
   *
   * @param zoom_factor: the zoom factor to apply. The zoom factor will be
   *        clamped to the NWeb's zoom limits. This value must be in the range
   *        0.01 to 100.0 inclusive.
   *
   * @return the error id.
   */
  /*--ark web()--*/
  virtual int Zoom(float zoom_factor) = 0;

  /**
   * @brief Performs a zooming in operation in this NWeb.
   *
   * @return the error id.
   */
  /*--ark web()--*/
  virtual int ZoomIn() = 0;

  /**
   * @brief Performs a zooming out operation in this NWeb.
   *
   * @return the error id.
   */
  /*--ark web()--*/
  virtual int ZoomOut() = 0;

  /**
   * @brief Stops the current load.
   *
   * @param code string: javascript code
   */
  /*--ark web()--*/
  virtual void Stop() = 0;

  /**
   * @brief ExecuteJavaScript
   */
  /*--ark web()--*/
  virtual void ExecuteJavaScript(const ArkWebString& code) = 0;

  /**
   * @brief ExecuteJavaScript plus
   *
   * @param code: javascript code
   * @param callback: javascript running result
   */
  /*--ark web()--*/
  virtual void ExecuteJavaScript(
      const ArkWebString& code,
      ArkWebRefPtr<ArkWebMessageValueCallback> callback,
      bool extention) = 0;

  /**
   * @brief Gets the NWebPreference object used to control the settings for this
   *        NWeb.
   *
   * @return a NWebPreference object that can be used to control this NWeb's
   *         settings This value cannot be null.
   */
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebPreference> GetPreference() = 0;

  /**
   * @brief Gets the web id.
   *
   * @return the web id
   */
  /*--ark web()--*/
  virtual unsigned int GetWebId() = 0;

  /**
   * @brief Gets the last hit test result.
   *
   * @return the last HitTestResult
   */
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebHitTestResult> GetHitTestResult() = 0;

  /**
   * @brief Sets the background color for this view.
   *
   * @param color: the color of the background
   */
  /*--ark web()--*/
  virtual void PutBackgroundColor(int color) = 0;

  /**
   * @brief Sets the initla scale for the page.
   *
   * @param scale: the initla scale of the page.
   */
  /*--ark web()--*/
  virtual void InitialScale(float scale) = 0;

  /**
   * @brief Sets the NWebDownloadCallback that will receive download event. This
   *        will replace the current handler.
   *
   * @param download_listener: download listener
   */
  /*--ark web()--*/
  virtual void PutDownloadCallback(
      ArkWebRefPtr<ArkWebDownloadCallback> download_listener) = 0;

  /**
   * @brief Set the NWebAccessibilityEventCallback that will receive
   *        accessibility event. This will replace the current handler.
   *
   * @param accessibility_event_listener: accessibility event listener
   */
  /*--ark web()--*/
  virtual void PutAccessibilityEventCallback(
      ArkWebRefPtr<ArkWebAccessibilityEventCallback>
          accessibility_event_listener) = 0;

  /**
   * @brief Set the accessibility id generator that will generate accessibility
   *        id for accessibility nodes in the web. This will replace the current
   *        handler.
   *
   * @param accessibility_id_generator: Accessibility id generator.
   */
  /*--ark web()--*/
  virtual void PutAccessibilityIdGenerator(
      const AccessibilityIdGenerateFunc accessibility_id_generator) = 0;

  /**
   * @brief Set the NWebHandler that will receive various notifications and
   *        requests. This will replace the current handler.
   *
   * @param handler: an implementation of NWebHandler This value cannot be null.
   */
  /*--ark web()--*/
  virtual void SetNWebHandler(ArkWebRefPtr<ArkWebHandler> handler) = 0;

  /**
   * @brief Gets the title for the current page.
   *
   * @return title string for the current page.
   */
  /*--ark web()--*/
  virtual ArkWebString Title() = 0;

  /**
   * @brief Gets the progress for the current page.
   *
   * @return progress for the current page.
   */
  /*--ark web()--*/
  virtual int PageLoadProgress() = 0;

  /**
   * @brief Gets the height of the HTML content.
   *
   * @return the height of the HTML content.
   */
  /*--ark web()--*/
  virtual int ContentHeight() = 0;

  /**
   * @brief Gets the current scale of this NWeb.
   *
   * @return the current scale
   */
  /*--ark web()--*/
  virtual float Scale() = 0;

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
  /*--ark web()--*/
  virtual int Load(const ArkWebString& url,
                   const ArkWebStringMap& additional_http_headers) = 0;

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
  /*--ark web()--*/
  virtual int LoadWithDataAndBaseUrl(const ArkWebString& base_url,
                                     const ArkWebString& data,
                                     const ArkWebString& mime_type,
                                     const ArkWebString& encoding,
                                     const ArkWebString& history_url) = 0;

  /**
   * @brief Loads the given data into this NWeb.
   *
   * @param data: the URL to use as the page's base URL. If null defaults to
   *        'about:blank'. This value may be null.
   * @param mime_type: the MIME type of the data, e.g. 'text/html'. This value
   *        may be null.
   * @param encoding: the encoding of the data This value may be null.
   */
  /*--ark web()--*/
  virtual int LoadWithData(const ArkWebString& data,
                           const ArkWebString& mime_type,
                           const ArkWebString& encoding) = 0;

  /**
   * @brief RegisterArkJSfunction
   *
   * @param object_name  String: objector name
   * @param method_list vector<String>: vector list ,method list
   * @param object_id int32_t: object id
   */
  /*--ark web()--*/
  virtual void RegisterArkJSfunction(const ArkWebString& object_name,
                                     const ArkWebStringVector& method_list,
                                     const int32_t object_id) = 0;

  /**
   * @brief UnregisterArkJSfunction
   *
   * @param object_name: objector name
   * @param method_list: vector list ,method list
   */
  /*--ark web()--*/
  virtual void UnregisterArkJSfunction(
      const ArkWebString& object_name,
      const ArkWebStringVector& method_list) = 0;

  /**
   * @brief SetNWebJavaScriptResultCallBack
   *
   * @param callback: callback client
   */
  /*--ark web()--*/
  virtual void SetNWebJavaScriptResultCallBack(
      ArkWebRefPtr<ArkWebJsResultCallback> callback) = 0;

  /**
   * @brief Set the NWebFindCallback that will receive find event. This will
   *        replace the current handler.
   *
   * @param find_listener: find callback
   */
  /*--ark web()--*/
  virtual void PutFindCallback(
      ArkWebRefPtr<ArkWebFindCallback> find_listener) = 0;

  /**
   * @brief Finds all instances of find on the page and highlights them,
   *        asynchronously.
   *
   * @param search_str: target string to find.
   */
  /*--ark web()--*/
  virtual void FindAllAsync(const ArkWebString& search_str) = 0;

  /**
   * @brief Clears the highlighting surrounding text matches created by
   *        findAllAsync
   */
  /*--ark web()--*/
  virtual void ClearMatches() = 0;

  /**
   * @brief Highlights and scrolls to the next match found by
   *        findAllAsync(String), wrapping around page boundaries as necessary.
   *
   * @param forward: find back or forward:
   */
  /*--ark web()--*/
  virtual void FindNext(const bool forward) = 0;

  /**
   * @brief Saves the current view as a web archive.
   *
   * @param base_name: the filename where the archive should be placed This
   *        value cannot be null.
   * @param auto_name: if false, takes basename to be a file. If true, basename
   *        is assumed to be a directory in which a filename will be chosen
   *        according to the URL of the current page.
   */
  /*--ark web()--*/
  virtual void StoreWebArchive(
      const ArkWebString& base_name,
      bool auto_name,
      ArkWebRefPtr<ArkWebStringValueCallback> callback) = 0;

  /**
   * @brief creating two ends of a message channel.
   *
   * @return the web message ports get from nweb.
   */
  /*--ark web()--*/
  virtual ArkWebStringVector CreateWebMessagePorts() = 0;

  /**
   * @brief Posts MessageEvent to the main frame.
   *
   * @param message: message send to mmain frame.
   * @param ports: the web message ports send to main frame.
   * @param target_uri: the uri which can received the ports.
   */
  /*--ark web()--*/
  virtual void PostWebMessage(const ArkWebString& message,
                              const ArkWebStringVector& ports,
                              const ArkWebString& target_uri) = 0;

  /**
   * @brief close the message port.
   *
   * @param port_handle: the port to close.
   */
  /*--ark web()--*/
  virtual void ClosePort(const ArkWebString& port_handle) = 0;

  /**
   * @brief use the port to send message.
   *
   * @param port_handle: the port to send message.
   * @param data: the message to send.
   */
  /*--ark web()--*/
  virtual void PostPortMessage(const ArkWebString& port_handle,
                               const ArkWebMessage& data) = 0;

  /**
   * @brief set the callback of the message port.
   *
   * @param port_handle: the port to set callback.
   * @param callback: to reveive the result when the other port post message.
   */
  /*--ark web()--*/
  virtual void SetPortMessageCallback(
      const ArkWebString& port_handle,
      ArkWebRefPtr<ArkWebMessageValueCallback> callback) = 0;

  /*--ark web()--*/
  virtual void SendDragEvent(ArkWebRefPtr<ArkWebDragEvent> drag_event) = 0;

  /**
   * @brief Clear ssl cache.
   */
  /*--ark web()--*/
  virtual void ClearSslCache() = 0;

  /**
   * @brief get web page url.
   *
   * @return web page url.
   */
  /*--ark web()--*/
  virtual ArkWebString GetUrl() = 0;

  /**
   * @brief Clears the client authentication certificate Cache in the Web.
   */
  /*--ark web()--*/
  virtual void ClearClientAuthenticationCache() = 0;

  /**
   * @brief set the locale name of current system setting..
   *
   * @param region: the locale name of current system setting.
   */
  /*--ark web()--*/
  virtual void UpdateLocale(const ArkWebString& language,
                            const ArkWebString& region) = 0;

  /**
   * @brief get original url of the request.
   *
   * @return original url.
   */
  /*--ark web()--*/
  virtual const ArkWebString GetOriginalUrl() = 0;

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
  /*--ark web()--*/
  virtual bool GetFavicon(const void** data,
                          size_t& width,
                          size_t& height,
                          int& color_type,
                          int& alpha_type) = 0;

  /**
   * @brief set the network status, just notify the webview to change the JS
   *        navigatoer.online.
   *
   * @param available: width of the icon.
   */
  /*--ark web()--*/
  virtual void PutNetworkAvailable(bool available) = 0;

  /**
   * @brief web has image or not.
   *
   * @param callback: has image or not
   */
  /*--ark web()--*/
  virtual void HasImages(ArkWebRefPtr<ArkWebBoolValueCallback> callback) = 0;

  /**
   * @brief web remove cache.
   *
   * @param include_disk_files: if false, only the RAM cache is removed
   */
  /*--ark web()--*/
  virtual void RemoveCache(bool include_disk_files) = 0;

  /**
   * @brief web has image or not.
   */
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebHistoryList> GetHistoryList() = 0;

  /**
   * @brief Set the NWebReleaseSurfaceCallback that will receive release surface
   *        event. This will replace the current handler.
   *
   * @param release_surface_listener: release surface listener
   */
  /*--ark web()--*/
  virtual void PutReleaseSurfaceCallback(
      ArkWebRefPtr<ArkWebReleaseSurfaceCallback> release_surface_listener) = 0;

  /**
   * @brief Get web back forward state.
   *
   * @return web back forward state.
   */
  /*--ark web()--*/
  virtual ArkWebUint8Vector SerializeWebState() = 0;

  /**
   * @brief Restore web back forward state.
   *
   * @param state: web back forward state.
   */
  /*--ark web()--*/
  virtual bool RestoreWebState(const ArkWebUint8Vector& state) = 0;

  /**
   * @brief Move page up.
   *
   * @param top: whether move to the top.
   */
  /*--ark web()--*/
  virtual void PageUp(bool top) = 0;

  /**
   * @brief Move page down.
   *
   * @param bottom: whether move to the bottom.
   */
  /*--ark web()--*/
  virtual void PageDown(bool bottom) = 0;

  /**
   * @brief Scroll to the position.
   *
   * @param x: horizontal coordinate.
   * @param y: vertical coordinate.
   */
  /*--ark web()--*/
  virtual void ScrollTo(float x, float y) = 0;

  /**
   * @brief Scroll by the delta distance.
   *
   * @param delta_x: horizontal offset.
   * @param delta_y: vertical offset.
   */
  /*--ark web()--*/
  virtual void ScrollBy(float delta_x, float delta_y) = 0;

  /**
   * @brief Slide scroll by the speed.
   *
   * @param vx: horizontal slide speed.
   * @param vy: vertical slide speed.
   */
  /*--ark web()--*/
  virtual void SlideScroll(float vx, float vy) = 0;

  /**
   * @brief Get current website certificate.
   *
   * @param cert_chain_data: current website certificate array.
   * @param is_single_cert: true if only get one certificate of current website,
   *        false if get certificate chain of the website.
   *
   * @return true if get certificate successfully, otherwise false.
   */
  /*--ark web()--*/
  virtual bool GetCertChainDerData(ArkWebStringVector& cert_chain_data,
                                   bool is_single_cert) = 0;

  /**
   * @brief Set screen offset.
   *
   * @param x: the offset in x direction.
   * @param y: the offset in y direction.
   */
  /*--ark web()--*/
  virtual void SetScreenOffSet(double x, double y) = 0;

  /**
   * @brief Set audio muted.
   *
   * @param muted: Aduio mute state.
   */
  /*--ark web()--*/
  virtual void SetAudioMuted(bool muted) = 0;

  /**
   * @brief Set should frame submission before draw.
   *
   * @param should: whether wait render frame submission.
   */
  /*--ark web()--*/
  virtual void SetShouldFrameSubmissionBeforeDraw(bool should) = 0;

  /**
   * @brief Notify whether the popup window is initialized successfully.
   *
   * @param result: whether success.
   */
  /*--ark web()--*/
  virtual void NotifyPopupWindowResult(bool result) = 0;

  /**
   * @brief Set audio resume interval.
   *
   * @param resume_interval: Aduio resume interval.
   */
  /*--ark web()--*/
  virtual void SetAudioResumeInterval(int32_t resume_interval) = 0;

  /**
   * @brief Set audio exclusive state.
   *
   * @param audio_exclusive: Aduio exclusive state.
   */
  /*--ark web()--*/
  virtual void SetAudioExclusive(bool audio_exclusive) = 0;

  /**
   * @brief Rigest the keep srceen on interface.
   *
   * @param window_id: the window id.
   * @param callback the screenon handle callback.
   */
  /*--ark web()--*/
  virtual void RegisterScreenLockFunction(
      int32_t window_id,
      ArkWebRefPtr<ArkWebScreenLockCallback> callback) = 0;

  /**
   * @brief UnRigest the keep srceen on interface.
   *
   * @param window_id: the window id.
   */
  /*--ark web()--*/
  virtual void UnRegisterScreenLockFunction(int32_t window_id) = 0;

  /**
   * @brief Notify memory level.
   *
   * @param level: the memory level.
   */
  /*--ark web()--*/
  virtual void NotifyMemoryLevel(int32_t level) = 0;

  /**
   * @brief Notify webview window status.
   */
  /*--ark web()--*/
  virtual void OnWebviewHide() = 0;

  /*--ark web()--*/
  virtual void OnWebviewShow() = 0;

  /**
   * @brief Get drag data.
   *
   * @return the drag data.
   */
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebDragData> GetOrCreateDragData() = 0;

  /**
   * @brief Prefetch the resources required by the page, but will not execute js
   *        or render the page.
   *
   * @param url: Which url to preresolve/preconnect.
   * @param additional_http_headers: Additional HTTP request header of the URL.
   */
  /*--ark web()--*/
  virtual void PrefetchPage(const ArkWebString& url,
                            const ArkWebStringMap& additional_http_headers) = 0;

  /**
   * @brief Set the window id.
   */
  /*--ark web()--*/
  virtual void SetWindowId(uint32_t window_id) = 0;

  /**
   * @brief Notify that browser was occluded by other windows.
   */
  /*--ark web()--*/
  virtual void OnOccluded() = 0;

  /**
   * @brief Notify that browser was unoccluded by other windows.
   */
  /*--ark web()--*/
  virtual void OnUnoccluded() = 0;

  /**
   * @brief Set the token.
   */
  /*--ark web()--*/
  virtual void SetToken(void* token) = 0;

  /**
   * @brief Set the nested scroll mode.
   */
  /*--ark web()--*/
  virtual void SetNestedScrollMode(const int32_t& nested_scroll_mode) = 0;

  /**
   * @brief Set enable lower the frame rate.
   */
  /*--ark web()--*/
  virtual void SetEnableLowerFrameRate(bool enabled) = 0;

  /**
   * @brief Set the property values for width, height, and keyboard height.
   */
  /*--ark web()--*/
  virtual void SetVirtualKeyBoardArg(int32_t width,
                                     int32_t height,
                                     double keyboard) = 0;

  /**
   * @brief Set the virtual keyboard to override the web status.
   */
  /*--ark web()--*/
  virtual bool ShouldVirtualKeyboardOverlay() = 0;

  /**
   * @brief Set draw rect.
   */
  /*--ark web()--*/
  virtual void SetDrawRect(int32_t x,
                           int32_t y,
                           int32_t width,
                           int32_t height) = 0;

  /**
   * @brief Set draw mode.
   */
  /*--ark web()--*/
  virtual void SetDrawMode(int32_t mode) = 0;

  /**
   * @brief Create web print document adapter.
   */
  /*--ark web()--*/
  virtual void* CreateWebPrintDocumentAdapter(const ArkWebString& job_name) = 0;

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
  /*--ark web()--*/
  virtual int PostUrl(const ArkWebString& url,
                      const ArkWebCharVector& post_data) = 0;

  /**
   * @brief Inject the JavaScript before WebView load the DOM tree.
   */
  /*--ark web()--*/
  virtual void JavaScriptOnDocumentStart(
      const ArkWebStringVectorMap& script_items) = 0;

  /**
   * @brief Execute an accessibility action on an accessibility node in the
   *        browser.
   *
   * @param accessibility_id: The id of the accessibility node.
   * @param action: The action to be performed on the accessibility node.
   */
  /*--ark web()--*/
  virtual void ExecuteAction(int64_t accessibility_id, uint32_t action) = 0;

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
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebAccessibilityNodeInfo>
  GetFocusedAccessibilityNodeInfo(int64_t accessibility_id,
                                  bool is_accessibility_focus) = 0;

  /**
   * @brief Get the information of the accessibility node by its accessibility
   *        id in the browser.
   *
   * @param accessibility_id: The accessibility id of the accessibility node.
   *
   * @return The obtained information of the accessibility node.
   */
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoById(int64_t accessibility_id) = 0;

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
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebAccessibilityNodeInfo>
  GetAccessibilityNodeInfoByFocusMove(int64_t accessibility_id,
                                      int32_t direction) = 0;

  /**
   * @brief Set the accessibility state in the browser.
   *
   * @param state: Indicate whether the accessibility state is enabled or
   *        disabled.
   */
  /*--ark web()--*/
  virtual void SetAccessibilityState(bool state) = 0;

  /**
   * @brief Get whether need soft keyboard.
   *
   * @return true if need soft keyboard, otherwise false.
   */
  /*--ark web()--*/
  virtual bool NeedSoftKeyboard() = 0;

  /**
   * @brief Discard the webview window.
   *
   * @return true if the discarding success, otherwise false.
   */
  /*--ark web()--*/
  virtual bool Discard() = 0;

  /**
   * @brief Reload the webview window that has been discarded before.
   *
   * @return true if the discarded window reload success, otherwise false.
   */
  /*--ark web()--*/
  virtual bool Restore() = 0;

  /**
   * @brief Get the security level of current page.
   *
   * @return security level for current page.
   */
  /*--ark web()--*/
  virtual int GetSecurityLevel() = 0;

  /**
   * @brief CallH5Function
   *
   * @param routingId         int32_t: the h5 frame routing id
   * @param h5ObjectId        int32_t: the h5 side object id
   * @param h5MethodName      string: the h5 side object method name
   * @param args              vector<shared_ptr<NWebValue>>: the call args
   */
  /*--ark web()--*/
  virtual void CallH5Function(int32_t routingId,
                              int32_t h5ObjectId,
                              const ArkWebString& h5MethodName,
                              const ArkWebValueVector& args) = 0;

  /**
   * @brief Get web weather has been set incognito mode.
   *
   * @return true if web is in incognito mode; otherwise fase.
   */
  /*--ark web()--*/
  virtual bool IsIncognitoMode() = 0;

  /**
   * @brief Register native function.
   */
  /*--ark web()--*/
  virtual void RegisterNativeArkJSFunction(
      const char* objName,
      const ArkWebJsProxyCallbackVector& callbacks) = 0;

  /**
   * @brief Unregister native function.
   */
  /*--ark web()--*/
  virtual void UnRegisterNativeArkJSFunction(const char* objName) = 0;

  /**
   * @brief Register native valide callback function.
   */
  /*--ark web()--*/
  virtual void RegisterNativeValideCallback(
      const char* webName,
      const NativeArkWebOnValidCallback callback) = 0;

  /**
   * @brief Register native destroy callback function.
   */
  /*--ark web()--*/
  virtual void RegisterNativeDestroyCallback(
      const char* webName,
      const NativeArkWebOnValidCallback callback) = 0;

  /**
   * @brief Inject the JavaScript after WebView loads the DOM tree and run
   *        JavaScripts.
   */
  /*--ark web()--*/
  virtual void JavaScriptOnDocumentEnd(
      const ArkWebStringVectorMap& script_items) = 0;

  /**
   * @brief Enable the ability to check website security risks.Illegal and
   *        fraudulent websites are mandatory enabled and cann't be disabled by
   *        this function.
   */
  /*--ark web()--*/
  virtual void EnableSafeBrowsing(bool enable) = 0;

  /**
   * @brief Get whether checking website security risks is enabled.
   *
   * @return true if enable the ability to check website security risks else
   *         false.
   */
  /*--ark web()--*/
  virtual bool IsSafeBrowsingEnabled() = 0;

  /**
   * @brief Set the ability to print web page background.
   *
   * @param enable Indicate whether the ability is enabled or disabled.
   */
  /*--ark web()--*/
  virtual void SetPrintBackground(bool enable) = 0;

  /**
   * @brief Obtains whether to print the background of a web page.
   *
   * @return true if enable print web page background, otherwise false.
   */
  /*--ark web()--*/
  virtual bool GetPrintBackground() = 0;

  /**
   * @brief Close fullScreen video.
   */
  /*--ark web()--*/
  virtual void CloseAllMediaPresentations() = 0;

  /**
   * @brief Stop all audio and video playback on the web page.
   */
  /*--ark web()--*/
  virtual void StopAllMedia() = 0;

  /**
   * @brief Restart playback of all audio and video on the web page.
   */
  /*--ark web()--*/
  virtual void ResumeAllMedia() = 0;

  /**
   * @brief Pause all audio and video playback on the web page.
   */
  /*--ark web()--*/
  virtual void PauseAllMedia() = 0;

  /**
   * @brief View the playback status of all audio and video on the web page.
   *
   * @return The playback status of all audio and video.
   */
  /*--ark web()--*/
  virtual int GetMediaPlaybackState() = 0;

  /**
   * @brief Enable the ability to intelligent tracking prevention, default
   * disabled.
   *
   * @param enable Indicate whether the abilitiy is enabled or disabled.
   */
  /*--ark web()--*/
  virtual void EnableIntelligentTrackingPrevention(bool enable) = 0;

  /**
   * @brief Get whether intelligent tracking prevention is enabled.
   * @return true if enable the ability intelligent tracking prevention; else
   * false.
   */
  /*--ark web()--*/
  virtual bool IsIntelligentTrackingPreventionEnabled() = 0;

  /**
   * @brief Start current camera.
   */
  /*--ark web()--*/
  virtual void StartCamera() = 0;

  /**
   * @brief Stop current camera.
   */
  /*--ark web()--*/
  virtual void StopCamera() = 0;

  /**
   * @brief Close current camera.
   */
  /*--ark web()--*/
  virtual void CloseCamera() = 0;

  /**
   * @brief Obtains the last javascript proxy calling frame url.
   *
   * @return the url of last calling frame url.
   */
  /*--ark web()--*/
  virtual ArkWebString GetLastJavascriptProxyCallingFrameUrl() = 0;

  /**
   * @brief Get pendingsize status.
   *
   * @return result of last pendingsize status.
   */
  /*--ark web()--*/
  virtual bool GetPendingSizeStatus() = 0;

  /**
   * @brief Scroll by the delta distance or velocity takes the screen as a
   * reference.
   *
   * @param delta_x horizontal offset in physical pixel.
   * @param delta_y vertical offset in physical pixel.
   * @param vx      horizontal velocity in physical pixel.
   * @param vx      vertical velocity in physical pixel.
   */
  /*--ark web()--*/
  virtual void ScrollByRefScreen(float delta_x,
                                 float delta_y,
                                 float vx,
                                 float vy) = 0;

  /**
   * @brief ExecuteJavaScript with ashmem
   *
   * @param fd fd of the ashmem
   * @param scriptLength javascript code length
   * @param callback javascript running result
   * @param extention true if is extension
   */
  /*--ark web()--*/
  virtual void ExecuteJavaScriptExt(
      const int fd,
      const size_t scriptLength,
      ArkWebRefPtr<ArkWebMessageValueCallback> callback,
      bool extention) = 0;

  /**
   * @brief Render process switch to background.
   */
  /*--ark web()--*/
  virtual void OnRenderToBackground() = 0;

  /**
   * @brief Render process switch to foreground.
   */
  /*--ark web()--*/
  virtual void OnRenderToForeground() = 0;

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
  virtual void PrecompileJavaScript(
      const ArkWebString& url,
      const ArkWebString& script,
      ArkWebRefPtr<ArkWebCacheOptions>& cacheOptions,
      ArkWebRefPtr<ArkWebMessageValueCallback> callback) = 0;

  /*--ark web()--*/
  virtual void OnCreateNativeMediaPlayer(
      ArkWebRefPtr<ArkWebCreateNativeMediaPlayerCallback> callback) = 0;

  /**
   * @brief Send touchpad fling event.
   *
   * @param x location of x.
   * @param y location of y.
   * @param vx velocity of x.
   * @param vy velocity of y.
   */
  /*--ark web()--*/
  virtual void SendTouchpadFlingEvent(double x,
                                      double y,
                                      double vx,
                                      double vy) = 0;

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
  virtual void InjectOfflineResource(const ArkWebString& url,
                                     const ArkWebString& origin,
                                     const ArkWebUint8Vector& resource,
                                     const ArkWebStringMap& responseHeaders,
                                     const int type) = 0;

  /**
   * @brief RegisterArkJSfunction
   *
   * @param object_name  String: objector name
   * @param method_list vector<String>: vector list, sync method list
   * @param async_method_list vector<String>: vector list, async method list
   * @param object_id int32_t: object id
   */
  /*--ark web()--*/
  virtual void RegisterArkJSfunction(
      const ArkWebString& object_name,
      const ArkWebStringVector& method_list,
      const ArkWebStringVector& async_method_list,
      const int32_t object_id) = 0;

  /**
   * @brief Set fit content mode.
   */
  /*--ark web()--*/
  virtual void SetFitContentMode(int32_t mode) = 0;

  /**
   * @brief Terminate render process
   *
   * @return true if it was possible to terminate this render process, false
   * otherwise.
   */
  /*--ark web()--*/
  virtual bool TerminateRenderProcess() = 0;

  /**
   * @brief Get select info.
   *
   * @return the info of selection.
   */
  /*--ark web()--*/
  virtual ArkWebString GetSelectInfo() = 0;

  /**
   * @brief Online Render process switch to foreground.
   */
  /*--ark web()--*/
  virtual void OnOnlineRenderToForeground() = 0;

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
  virtual int ScaleGestureChange(double scale,
                                 double centerX,
                                 double centerY) = 0;

  /**
   * Get value of Autofill index.
   * @param index index value.
   */
  /*--ark web()--*/
  virtual void SuggestionSelected(int32_t index) = 0;

  /**
   * @brief Notify that safe insets change.
   *
   */
  /*--ark web()--*/
  virtual void OnSafeInsetsChange(int left, int top, int right, int bottom) = 0;

  /**
   * @brief Called when text is selected in image.
   */
  /*--ark web()--*/
  virtual void OnTextSelected() = 0;

  /**
   * @brief Notify for next touch move event.
   */
  /*--ark web()--*/
  virtual void NotifyForNextTouchEvent() = 0;

  /**
   * @brief Enable the ability to block Ads, default disabled.
   */
  /*--ark web()--*/
  virtual void EnableAdsBlock(bool enable) = 0;

  /**
   * @brief Get whether Ads block is enabled.
   */
  /*--ark web()--*/
  virtual bool IsAdsBlockEnabled() = 0;

  /**
   * @brief Get whether Ads block is enabled for current Webpage.
   */
  /*--ark web()--*/
  virtual bool IsAdsBlockEnabledForCurPage() = 0;

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
  virtual bool WebPageSnapshot(const char* id,
                               int type,
                               int width,
                               int height,
                               const WebSnapshotCallback callback) = 0;

  /**
    * @brief Notify that system configuration changed.
    *
    * @param configuration system configuration.
    */
  /*--ark web()--*/
  virtual void OnConfigurationUpdated(
      ArkWebRefPtr<ArkWebSystemConfiguration> configuration) = 0;

  /**
    * @brief Set url trust list.
    *
  */
  /*--ark web()--*/
  virtual int SetUrlTrustList(const ArkWebString& urlTrustList) = 0;

  /**
    * @brief Put the callback, convert sapnstring to html.
    *
    * @param callback will convert spanstring to html.
  */
  /*--ark web()--*/
  virtual void PutSpanstringConvertHtmlCallback(
      ArkWebRefPtr<ArkWebSpanstringConvertHtmlCallback> callback) = 0;

  /**
    * Web send key event.
    * @param key_code code value.
    * @param key_action action value.
    * @param pressedCodes pressedCodes value.
  */
  /*--ark web()--*/
  virtual bool WebSendKeyEvent(int32_t key_code, int32_t key_action, const ArkWebInt32Vector& pressedCodes) = 0;

  /**
   * @brief Set grant file access dirs.
   */
  /*--ark web()--*/
  virtual void SetPathAllowingUniversalAccess(
      const ArkWebStringVector& dirs,
      const ArkWebStringVector& moduleName,
      ArkWebString& errorPath) = 0;

  /**
   * @brief Send mouse wheel event.
   */
  /*--ark web()--*/
  virtual void WebSendMouseWheelEvent(
      double x,
      double y,
      double delta_x,
      double delta_y,
      const ArkWebInt32Vector& pressedCodes) = 0;

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
  virtual void WebSendTouchpadFlingEvent(
      double x,
      double y,
      double vx,
      double vy,
      const ArkWebInt32Vector& pressedCodes) = 0;

  /**
   * @brief Set url trust list with error message.
   *
   */
  /*--ark web()--*/
  virtual int SetUrlTrustListWithErrMsg(const ArkWebString& urlTrustList,
                                        ArkWebString& detailErrMsg) = 0;

  /**
   * @brief resize visual viewport.
   *
   * @param width width.
   * @param height height.
   * @param iskeyboard from keybord.
   */
  /*--ark web()--*/
  virtual void ResizeVisibleViewport(uint32_t width,
                                     uint32_t height,
                                     bool isKeyboard) = 0;

  /**
   * @brief set backforward cache options.
   * @param size The size of the back forward cache could saved.
   * @param timeToLive The time of the back forward cache page could stay.
   */
  /*--ark web()--*/
  virtual void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) = 0;

  /**
   * @brief RegisterArkJSfunctionV2
   *
   * @param object_name  String: objector name
   * @param method_list vector<String>: vector list, sync method list
   * @param async_method_list vector<String>: vector list, async method list
   * @param object_id int32_t: object id
   * @param permission String: allow list
   */
  /*--ark web()--*/
  virtual void RegisterArkJSfunctionV2(
      const ArkWebString& object_name,
      const ArkWebStringVector& method_list,
      const ArkWebStringVector& async_method_list,
      const int32_t object_id,
      const ArkWebString& permission) = 0;

  /**
   * @brief set the callback of the autofill event.
   * @param callback callback.
   */
  /*--ark web()--*/
  virtual void SetAutofillCallback(
      ArkWebRefPtr<ArkWebMessageValueCallback> callback) = 0;

  /**
   * @brief fill autofill data.
   * @param data data.
   */
  /*--ark web()--*/
  virtual void FillAutofillData(const ArkWebMessage& data) = 0;

  /**
   * @brief on autofill cancel.
   * @param fillContent fillContent
   */
  /*--ark web()--*/
  virtual void OnAutofillCancel(const ArkWebString& fillContent) = 0;

  /**
   * Execute an accessibility action on an accessibility node in the browser.
   * @param accessibilityId The id of the accessibility node.
   * @param action The action to be performed on the accessibility node.
   * @param actionArguments Data related to the current action.
   */
  /*--ark web()--*/
  virtual void PerformAction(int64_t accessibilityId,
                             uint32_t action,
                             const ArkWebStringMap& actionArguments) = 0;

  /**
   * @brief Send the accessibility hover event coordinate.
   *
   * @param x horizontal location of coordinate.
   * @param y vertical location of coordinate.
   */
  /*--ark web()--*/
  virtual void SendAccessibilityHoverEvent(int32_t x, int32_t y) = 0;

  /**
   * Scroll by the delta distance if web is not foucsed.
   *
   * @param delta_x horizontal offset.
   * @param delta_y vertical offset.
   * @return false if web is touched down.
   */
  /*--ark web()--*/
  virtual bool ScrollByWithResult(float delta_x, float delta_y) = 0;

  /**
   * @brief Called when image analyzer is destory.
   */
  /*--ark web()--*/
  virtual void OnDestroyImageAnalyzerOverlay() = 0;

  /**
   * @brief Get the current scroll offset of the webpage.
   * @param offset_x The current horizontal scroll offset of the webpage.
   * @param offset_y The current vertical scroll offset of the webpage.
   */
  /*--ark web()--*/
  virtual void GetScrollOffset(float* offset_x, float* offset_y) = 0;

  /**
   * @brief set DPI when DPI changes.
   * @param density The new density value.
   */
  /*--ark web()--*/
  virtual void SetSurfaceDensity(const double& density) = 0;

  /**
   * @Description: Get the accessibility visibility of the accessibility node by its accessibility id in the browser.
   * @Input accessibility_id: The accessibility id of the accessibility node.
   * @Return: The accessibility visibility of the accessibility node.
   */
  /*--ark web()--*/
  virtual bool GetAccessibilityVisible(int64_t accessibility_id) = 0;

  /**
  * @brief Web components blur when the keyboard is hidden by gesture back.
  */
  /*--ark web()--*/
  virtual void WebComponentsBlur() = 0;

  /**
   * Scroll to the position.
   *
   * @param x horizontal coordinate.
   * @param y vertical coordinate.
   * @param duration: anime duration.
   */
  /*--ark web()--*/
  virtual void ScrollToWithAnime(float x, float y, int32_t duration) = 0;

  /**
   * Scroll by the delta distance.
   *
   * @param delta_x: horizontal offset.
   * @param delta_y: vertical offset.
   * @param duration: anime duration.
   */
  /*--ark web()--*/
  virtual void ScrollByWithAnime(float delta_x, float delta_y, int32_t duration) = 0;

    /**
     * @brief ExecuteCreatePDFExt
     *
     * @param pdfConfig The current configuration when creating pdf.
     * @param callback NWebArrayBufferValueCallback: CreatePDF running result.
     */
    /*--ark web()--*/
    virtual void ExecuteCreatePDFExt(ArkWebRefPtr<ArkWebPDFConfigArgs> pdfConfig,
        ArkWebRefPtr<ArkWebArrayBufferValueCallback> callback) = 0;

     /**
     * @Description: Set the params when the scale of WebView changed by pinch gestrue.
     *
     * @Input type: gesture status
     * @Input scale: the scale factor to apply. The scale will be
     *        clamped to the pinch limits. This value must be in the range
     *        0.01 to 8.0 inclusive.
     * @Input originScale: the origin scale factor to apply. The scale will be
     *        clamped to the pinch limits. This value must be in the range
     *        0.01 to 8.0 inclusive.
     * @Input centerX: X-coordinate of the pinch center
     * @Input centerY: Y-coordinate of the pinch center
     *
     * @Return: the error id.
     */
    /*--ark web()--*/
    virtual int ScaleGestureChangeV2(int type, double scale, double originScale, double centerX, double centerY) = 0;

    /**
     * @Description: Sends key events to the web kernel.
     * @Input mouseEvent: Basic information about key events.
     */
    /*--ark web()--*/
    virtual bool SendKeyboardEvent(ArkWebRefPtr<ArkWebKeyboardEvent> keyboardEvent) = 0;

    /**
     * @Description: Optimize HTML parser budget to reduce FCP time.
     * @Input enable: Set whether to use optimized parser budget.
     */
    /*--ark web()--*/
    virtual void PutOptimizeParserBudgetEnabled(bool enable) = 0;

    /**
     * @brief Inject the JavaScript before WebView load the DOM tree.
     */
    /*--ark web()--*/
    virtual void JavaScriptOnDocumentStartByOrder(const ArkWebStringVectorMap& script_items,
        const ArkWebStringVector& script_items_by_order) = 0;

    /**
     * @brief Inject the JavaScript after WebView loads the DOM tree and run
     *        JavaScripts.
     */
    /*--ark web()--*/
    virtual void JavaScriptOnDocumentEndByOrder(const ArkWebStringVectorMap& script_items,
        const ArkWebStringVector& script_items_by_order) = 0;

    /**
     * @Description: Inject the JavaScript when the head element has been created.
     * @Input scriptItems: The injected JavaScript code is stored in lexicographical order.
     * @Input scriptItemsByOrder: The injected JavaScript code is stored in the order of the injection array.
     */
    /*--ark web()--*/
    virtual void JavaScriptOnHeadReadyByOrder(const ArkWebStringVectorMap& script_items,
        const ArkWebStringVector& script_items_by_order) = 0;

    /**
     * @brief Send mouse wheel event with sourceTool info.
     */
    /*--ark web()--*/
    virtual bool WebSendMouseWheelEventV2(
        double x, double y, double delta_x, double delta_y, const ArkWebInt32Vector &pressedCodes, int32_t source) = 0;

    /**
     * @brief Web maximize resize optimize.
     */
    /*--ark web()--*/
    virtual void MaximizeResize() = 0;

    /**
     * @brief Try to attach web inputmethod after drag.
     */
    /*--ark web()--*/
    virtual void OnDragAttach()  = 0;
};

}  // namespace OHOS::ArkWeb

#endif  // ARK_WEB_NWEB_H_
