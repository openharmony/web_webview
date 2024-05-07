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

#ifndef NWEB_H
#define NWEB_H

#include <list>
#include <map>
#include <memory>
#include <string>

#include "nweb_accessibility_event_callback.h"
#include "nweb_accessibility_node_info.h"
#include "nweb_download_callback.h"
#include "nweb_drag_data.h"
#include "nweb_export.h"
#include "nweb_find_callback.h"
#include "nweb_history_list.h"
#include "nweb_hit_testresult.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_native_media_player.h"
#include "nweb_preference.h"
#include "nweb_release_surface_callback.h"
#include "nweb_value_callback.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {
class NWebHandler;
class NWebValue;

/**
 * @brief Describes how pixel bits encoder color data.
 */
enum class ImageColorType {
    // Unknown color type.
    COLOR_TYPE_UNKNOWN = -1,

    // RGBA with 8 bits per pixel (32bits total).
    COLOR_TYPE_RGBA_8888 = 0,

    // BGRA with 8 bits per pixel (32bits total).
    COLOR_TYPE_BGRA_8888 = 1,
};

/**
 * @brief Describes how to interpret the alpha value of a pixel.
 */
enum class ImageAlphaType {
    // Unknown alpha type.
    ALPHA_TYPE_UNKNOWN = -1,

    // No transparency. The alpha component is ignored.
    ALPHA_TYPE_OPAQUE = 0,

    // Transparency with pre-multiplied alpha component.
    ALPHA_TYPE_PREMULTIPLIED = 1,

    // Transparency with post-multiplied alpha component.
    ALPHA_TYPE_POSTMULTIPLIED = 2,
};

class OHOS_NWEB_EXPORT NWebEngineInitArgs {
    public:
    virtual ~NWebEngineInitArgs() = default;

    virtual std::string GetDumpPath() = 0;
    virtual bool GetIsFrameInfoDump() = 0;
    virtual std::list<std::string> GetArgsToAdd() = 0;
    virtual std::list<std::string> GetArgsToDelete() = 0;
    virtual bool GetIsMultiRendererProcess() = 0;
    virtual bool GetIsEnhanceSurface() = 0;
    virtual bool GetIsPopup() = 0;
};

class OHOS_NWEB_EXPORT NWebOutputFrameCallback {
    public:
    virtual ~NWebOutputFrameCallback() = default;

    virtual bool Handle(const char *buffer, uint32_t width, uint32_t height) = 0;
};

class OHOS_NWEB_EXPORT NWebCreateInfo {
    public:
    virtual ~NWebCreateInfo() = default;

    /* size info */
    virtual uint32_t GetWidth() = 0;
    virtual uint32_t GetHeight() = 0;

    /* output frame cb */
    virtual std::shared_ptr<NWebOutputFrameCallback> GetOutputFrameCallback() = 0;

    /* init args */
    virtual std::shared_ptr<NWebEngineInitArgs> GetEngineInitArgs() = 0;

    /* rs producer surface, for acquiring elgsurface from ohos */
    virtual void *GetProducerSurface() = 0;
    virtual void *GetEnhanceSurfaceInfo() = 0;

    virtual bool GetIsIncognitoMode() = 0;
};

enum class OHOS_NWEB_EXPORT DragAction {
    DRAG_START = 0,
    DRAG_ENTER,
    DRAG_LEAVE,
    DRAG_OVER,
    DRAG_DROP,
    DRAG_END,
    DRAG_CANCEL,
};

class NWebDragEvent {
    public:
    virtual ~NWebDragEvent() = default;

    virtual double GetX() = 0;
    virtual double GetY() = 0;
    virtual DragAction GetAction() = 0;
};

enum class BlurReason : int32_t {
    FOCUS_SWITCH = 0,
    WINDOW_BLUR = 1,
    FRAME_DESTROY = 2,
};

enum class FocusReason : int32_t {
    FOCUS_DEFAULT = 0,
    EVENT_REQUEST = 1,
};

enum class NestedScrollMode : int32_t {
    SELF_ONLY = 0,
    SELF_FIRST = 1,
    PARENT_FIRST = 2,
    PARALLEL = 3,
};

enum class RenderProcessMode : int32_t {
    SINGLE_MODE = 0,
    MULTIPLE_MODE = 1,
};

class NWebTouchPointInfo {
    public:
    virtual ~NWebTouchPointInfo() = default;

    virtual int GetId() = 0;
    virtual double GetX() = 0;
    virtual double GetY() = 0;
};

class NWebScreenLockCallback {
    public:
    virtual ~NWebScreenLockCallback() = default;

    virtual void Handle(bool key) = 0;
};

typedef char *(*NativeArkWebOnJavaScriptProxyCallback)(const char **, int32_t);
class NWebJsProxyCallback {
public:
    virtual ~NWebJsProxyCallback() = default;

    virtual std::string GetMethodName() = 0;

    virtual NativeArkWebOnJavaScriptProxyCallback GetMethodCallback() = 0;
};

typedef int64_t (*AccessibilityIdGenerateFunc)();
typedef void (*NativeArkWebOnValidCallback)(const char *);
typedef void (*NativeArkWebOnDestroyCallback)(const char *);
using ScriptItems = std::map<std::string, std::vector<std::string>>;

class OHOS_NWEB_EXPORT NWebEnginePrefetchArgs {
    public:
    virtual ~NWebEnginePrefetchArgs() = default;

    virtual std::string GetUrl() = 0;
    virtual std::string GetMethod() = 0;
    virtual std::string GetFormData() = 0;
};

enum class PrecompileError : int32_t {
    OK = 0,
    INTERNAL_ERROR = -1
};

class OHOS_NWEB_EXPORT CacheOptions {
    public:
    virtual ~CacheOptions() = default;

    virtual std::map<std::string, std::string> GetResponseHeaders() = 0;
    virtual bool IsModule() = 0;
    virtual bool IsTopLevel() = 0;
};

class OHOS_NWEB_EXPORT NWeb : public std::enable_shared_from_this<NWeb> {
public:
    NWeb() = default;
    virtual ~NWeb() = default;

    virtual void Resize(uint32_t width, uint32_t height, bool isKeyboard = false) = 0;

    /* lifecycle interface */
    virtual void OnPause() = 0;
    virtual void OnContinue() = 0;
    virtual void OnDestroy() = 0;

    /* focus event */
    virtual void OnFocus(const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) = 0;
    virtual void OnBlur(const BlurReason& blurReason) = 0;

    /* event interface */
    virtual void OnTouchPress(int32_t id, double x, double y, bool fromOverlay = false) = 0;
    virtual void OnTouchRelease(int32_t id, double x = 0, double y = 0, bool fromOverlay = false) = 0;
    virtual void OnTouchMove(int32_t id, double x, double y, bool fromOverlay = false) = 0;
    virtual void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                             bool fromOverlay = false) = 0;
    virtual void OnTouchCancel() = 0;
    virtual void OnNavigateBack() = 0;
    virtual bool SendKeyEvent(int32_t keyCode, int32_t keyAction) = 0;
    virtual void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY) = 0;
    virtual void SendMouseEvent(int x, int y, int button, int action, int count) = 0;

    /**
     * Load the given URL.
     *
     * @param url String: the URL of the resource to load. This value cannot be
     * null.
     *
     * @return title string for the current page.
     */
    virtual int Load(const std::string& url) = 0;

    /**
     * Get whether this NWeb has a back history item.
     *
     * @return true if this NWeb has a back history item
     */
    virtual bool IsNavigatebackwardAllowed() = 0;

    /**
     * Get whether this NWeb has a forward history item.
     *
     * @return true if this NWeb has a forward history item
     */
    virtual bool IsNavigateForwardAllowed() = 0;

    /**
     * Get whether this NWeb has a back or forward history item for number of
     * steps.
     *
     * @param numSteps int: the negative or positive number of steps to move the
     * history.
     * @return true if this NWeb has a forward history item
     */
    virtual bool CanNavigateBackOrForward(int numSteps) = 0;

    /**
     * Go back in the history of this NWeb.
     *
     */
    virtual void NavigateBack() = 0;

    /**
     * Go forward in the history of this NWeb.
     *
     */
    virtual void NavigateForward() = 0;

    /**
     * Goes to the history item that is the number of steps away from the current item.
     *
     */
    virtual void NavigateBackOrForward(int step) = 0;

    /**
     * Delete back and forward history list.
     */
    virtual void DeleteNavigateHistory() = 0;

    /**
     * Reload the current URL.
     *
     */
    virtual void Reload() = 0;

    /**
     * Perform a zoom operation in this NWeb.
     *
     * @param zoomFactor float: the zoom factor to apply. The zoom factor will be
     * clamped to the NWeb's zoom limits. This value must be in the range 0.01
     * to 100.0 inclusive.
     *
     * @return the error id.
     *
     */
    virtual int Zoom(float zoomFactor) = 0;

    /**
     * Performs a zooming in operation in this NWeb.
     *
     * @return the error id.
     *
     */
    virtual int ZoomIn() = 0;

    /**
     * Performs a zooming out operation in this NWeb.
     *
     * @return the error id.
     *
     */
    virtual int ZoomOut() = 0;

    /**
     * Stop the current load.
     *
     * @param code string: javascript code
     */
    virtual void Stop() = 0;

    /**
     * ExecuteJavaScript
     *
     */
    virtual void ExecuteJavaScript(const std::string& code) = 0;

    /**
     * ExecuteJavaScript plus
     *
     * @param code string: javascript code
     *
     * @param callback NWebValueCallback: javascript running result
     *
     */
    virtual void ExecuteJavaScript(
        const std::string& code,
        std::shared_ptr<NWebMessageValueCallback> callback,
        bool extention) = 0;

    /**
     * ExecuteJavaScript with ashmem
     *
     * @param fd fd of the ashmem
     * @param scriptLength javascript code length
     * @param callback NWebValueCallback: javascript running result
     * @param extention true if is extention
     */
    virtual void ExecuteJavaScriptExt(
        const int fd,
        const size_t scriptLength,
        std::shared_ptr<NWebMessageValueCallback> callback,
        bool extention) = 0;

    /**
     * Get the NWebPreference object used to control the settings for this
     * NWeb.
     *
     * @return a NWebPreference object that can be used to control this NWeb's
     * settings This value cannot be null.
     */
    virtual std::shared_ptr<NWebPreference> GetPreference() = 0;

    /**
     * Gets the web id.
     *
     * @return the web id
     */
    virtual unsigned int GetWebId() = 0;

    /**
     * Get the last hit test result.
     *
     * @return the last HitTestResult
     */
    virtual std::shared_ptr<HitTestResult> GetHitTestResult() = 0;

    /**
     * Set the background color for this view.
     *
     * @param color int: the color of the background
     *
     */
    virtual void PutBackgroundColor(int color) = 0;

    /**
     * Sets the initla scale for the page.
     *
     * @param scale float: the initla scale of the page.
     *
     */
    virtual void InitialScale(float scale) = 0;

    /**
     * Set the NWebDownloadCallback that will receive download event.
     * This will replace the current handler.
     *
     * @param downloadListener NWebDownloadCallback.
     */
    virtual void PutDownloadCallback(
        std::shared_ptr<NWebDownloadCallback> downloadListener) = 0;

     /**
     * Set the NWebAccessibilityEventCallback that will receive accessibility event.
     * This will replace the current handler.
     *
     * @param accessibilityEventListener NWebDownloadCallback.
     */
    virtual void PutAccessibilityEventCallback(
        std::shared_ptr<NWebAccessibilityEventCallback> accessibilityEventListener) = 0;

     /**
     * Set the accessibility id generator that will generate accessibility id for accessibility nodes in the web.
     * This will replace the current handler.
     *
     * @param accessibilityIdGenerator Accessibility id generator.
     */
    virtual void PutAccessibilityIdGenerator(
        const AccessibilityIdGenerateFunc accessibilityIdGenerator) = 0;

    /**
     * Set the NWebHandler that will receive various notifications and
     * requests. This will replace the current handler.
     *
     * @param client NWebHandler: an implementation of NWebHandler. This value
     * cannot be null.
     */
    virtual void SetNWebHandler(std::shared_ptr<NWebHandler> handler) = 0;

    /**
     * Get the title for the current page.
     *
     * @return title string for the current page.
     */
    virtual std::string Title() = 0;

    /**
     * Get the progress for the current page.
     *
     * @return progress for the current page.
     */
    virtual int PageLoadProgress() = 0;

    /**
     * Get the height of the HTML content.
     *
     * @return the height of the HTML content.
     */
    virtual int ContentHeight() = 0;

    /**
     * Get the current scale of this NWeb.
     *
     * @return the current scale
     */
    virtual float Scale() = 0;

    /**
     * Load the given URL with additional HTTP headers, specified as a map
     * from name to value. Note that if this map contains any of the headers that
     * are set by default by this NWeb, such as those controlling caching,
     * accept types or the User-Agent, their values may be overridden by this
     * NWeb's defaults.
     *
     * @param url  String: the URL of the resource to load This value cannot be
     * null.
     *
     * @param additionalHttpHeaders additionalHttpHeaders
     */
    virtual int Load(
        const std::string& url,
        const std::map<std::string, std::string> &additionalHttpHeaders) = 0;

    /**
     * Load the given data into this NWeb, using baseUrl as the base URL for
     * the content. The base URL is used both to resolve relative URLs and when
     * applying JavaScript's same origin policy. The historyUrl is used for the
     * history entry.
     *
     * @param baseUrl  String: the URL to use as the page's base URL. If null
     * defaults to 'about:blank'. This value may be null.
     * @param data String: the URL to use as the page's base URL. If null defaults
     * to 'about:blank'. This value may be null.
     * @param mimeType String: the MIME type of the data, e.g. 'text/html'. This
     * value may be null.
     * @param encoding String: the encoding of the data This value may be null.
     * @param historyUrl String: the URL to use as the history entry. If null
     * defaults to 'about:blank'. If non-null, this must be a valid URL. This
     * value may be null.
     */
    virtual int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                        const std::string& data,
                                        const std::string& mimeType,
                                        const std::string& encoding,
                                        const std::string& historyUrl) = 0;

    /**
     * Load the given data into this NWeb.
     *
     * @param data String: the URL to use as the page's base URL. If null defaults
     * to 'about:blank'. This value may be null.
     * @param mimeType String: the MIME type of the data, e.g. 'text/html'. This
     * value may be null.
     * @param encoding String: the encoding of the data This value may be null.
     */
    virtual int LoadWithData(const std::string& data,
                              const std::string& mimeType,
                              const std::string& encoding) = 0;

    /**
     * RegisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list  vector<String>: vector list, method list
     * @param object_id    int32_t: object id
     */
    virtual void RegisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list,
        const int32_t object_id) = 0;

    /**
     * UnregisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list vector<String>: vector list, method list
     */
    virtual void UnregisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list) = 0;

    /**
     * SetNWebJavaScriptResultCallBack
     *
     * @param callback  NWebJavaScriptResultCallBack: callback client
     */
    virtual void SetNWebJavaScriptResultCallBack(
        std::shared_ptr<NWebJavaScriptResultCallBack> callback) = 0;

    /**
     * Set the NWebFindCallback that will receive find event.
     * This will replace the current handler.
     *
     * @param findListener NWebFindCallback : find callback
     */
    virtual void PutFindCallback(
        std::shared_ptr<NWebFindCallback> findListener) = 0;

    /**
     * Finds all instances of find on the page and highlights them,
     * asynchronously.
     *
     * @param searchStr String: target string to find.
     */
    virtual void FindAllAsync(const std::string &searchStr) = 0;

    /**
     * Clears the highlighting surrounding text matches created by findAllAsync
     */
    virtual void ClearMatches() = 0;

    /**
     * Highlights and scrolls to the next match found by findAllAsync(String),
     * wrapping around page boundaries as necessary.
     *
     * @param forward bool: find back or forward.
     */
    virtual void FindNext(const bool forward) = 0;

    /**
     * Saves the current view as a web archive.
     *
     * @param baseName the filename where the archive should be placed This value cannot be null.
     * @param autoName if false, takes basename to be a file. If true, basename is assumed to be
     * a directory in which a filename will be chosen according to the URL of the
     * current page.
     */
    virtual void StoreWebArchive(const std::string &baseName, bool autoName,
        std::shared_ptr<NWebStringValueCallback> callback) = 0;

    /**
     * create two web message ports.
     * @param ports the web message ports.
     */
    virtual std::vector<std::string> CreateWebMessagePorts() = 0;

    /**
     * post messag event to the html main frame.
     * @param message the message
     * @param ports the web message ports.
     * @param uri the uri
     */
    virtual void PostWebMessage(const std::string& message,
                                const std::vector<std::string>& ports,
                                const std::string& targetUri) = 0;

    /**
     * close the message port.
     * @param handle the web message port handle.
     */
    virtual void ClosePort(const std::string& portHandle) = 0;

    /**
     * use the port to send message.
     * @param handle the web message port handle.
     * @param data the message send to html5.
     */
    virtual void PostPortMessage(const std::string& portHandle,
                                 std::shared_ptr<NWebMessage> data) = 0;

    /**
     * set the callback of th port handle.
     * @param handle the web message port handle.
     * @param callback to receive the message when th other port post message.
     */
    virtual void SetPortMessageCallback(
        const std::string& portHandle,
        std::shared_ptr<NWebMessageValueCallback> callback) = 0;

    /**
     * send drag event to nweb.
     * @param dragEvent the drag event information.
     */
    virtual void SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) = 0;

    /**
     * Clear ssl cache.
     *
     */
    virtual void ClearSslCache() = 0;

    /**
     * get web page url.
     *
     * @return web page url.
     */
    virtual std::string GetUrl() = 0;

    /**
     * Clears the client authentication certificate cache in the Web.
     * @since 9
     */
    virtual void ClearClientAuthenticationCache() = 0;

    /**
     * set the locale name of current system setting.
     *
     * @param locale the locale name of current system setting.
     */
    virtual void UpdateLocale(const std::string& language, const std::string& region) = 0;

    /**
     * Get the original url of the current web page.
     *
     * @return original url
     */
    virtual const std::string GetOriginalUrl() = 0;

    /**
     * get the favicon of the request web page.
     *
     * @param data the raw data of the favicon.
     * @param width the width of the favicon.
     * @param height the height of the favicon.
     * @param colorType the color type of the favicon.
     * @param alphaType the alpha type of the favicon.
     * @return true if get the favicon successfully, otherwise return false.
     */
    virtual bool GetFavicon(const void** data, size_t& width, size_t& height,
        ImageColorType& colorType, ImageAlphaType& alphaType) = 0;

    /**
     * Set the network status, just notify the webview to change the property of navigator.online.
     *
     * @param available the status of the network.
     */
    virtual void PutNetworkAvailable(bool available) = 0;

    /**
     * web has image or not
     *
     * @param callback has image or not
     */
    virtual void HasImages(std::shared_ptr<NWebBoolValueCallback> callback) = 0;

    /**
     * web remove cache
     *
     * @param include_disk_files bool:if false, only tje RAM cache is removed
     */
    virtual void RemoveCache(bool include_disk_files) = 0;

    /**
     * Get navigation history list
     *
     * @return navigation history list
     */
    virtual std::shared_ptr<NWebHistoryList> GetHistoryList() = 0;

    /**
     * Set the NWebReleaseSurfaceCallback that will receive release surface
     * event. This will replace the current handler.
     *
     * @param releaseSurfaceListener NWebReleaseSurfaceCallback.
     */
    virtual void PutReleaseSurfaceCallback(
        std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) = 0;

    /**
     * Get Web back forward state.
     *
     * @return web back forward state.
     */
    virtual std::vector<uint8_t> SerializeWebState() = 0;

    /**
     * Restore Web back forward state.
     *
     * @param web back forward state.
     */
    virtual bool RestoreWebState(const std::vector<uint8_t> &state) = 0;

    /**
     * Move page up.
     *
     * @param top whether move to the top.
     */
    virtual void PageUp(bool top) = 0;

    /**
     * Move page down.
     *
     * @param bottom whether move to the bottom.
     */
    virtual void PageDown(bool bottom) = 0;

    /**
     * Scroll to the position.
     *
     * @param x the x of the position.
     * @param y the y of the position.
     */
    virtual void ScrollTo(float x, float y) = 0;

    /**
     * Scroll by the delta position.
     *
     * @param deltaX the deltaX of the position.
     * @param deltaY the deltaY of the position.
     */
    virtual void ScrollBy(float deltaX, float deltaY) = 0;

    /**
     * Slide by the speed.
     *
     * @param vx the vx of the speed.
     * @param vy the vy of the speed.
     */
    virtual void SlideScroll(float vx, float vy) = 0;

    /**
     * Get current website certificate.
     *
     * @param certChainData current website certificate array.
     * @param isSingleCert true if only get one certificate of current website,
     *                     false if get certificate chain of the website.
     * @return true if get certificate successfully, otherwise false.
    */
    virtual bool GetCertChainDerData(std::vector<std::string>& certChainData, bool isSingleCert) = 0;

    /**
     * Set screen offset.
     *
     * @param x the offset in x direction.
     * @param y the offset in y direction.
    */
    virtual void SetScreenOffSet(double x, double y) = 0;

    /**
     * Set audio muted.
     *
     * @param muted Aduio mute state.
     */
    virtual void SetAudioMuted(bool muted) = 0;

    /**
     * Set should frame submission before draw.
     *
     * @param should whether wait render frame submission.
    */
    virtual void SetShouldFrameSubmissionBeforeDraw(bool should) = 0;

    /**
     * Notify whether the popup window is initialized successfully.
     *
     * @param result whether success.
     */
    virtual void NotifyPopupWindowResult(bool result) = 0;

    /**
     * Set audio resume interval.
     *
     * @param resumeInterval Aduio resume interval.
     */
    virtual void SetAudioResumeInterval(int32_t resumeInterval) = 0;

    /**
     * Set audio exclusive state.
     *
     * @param audioExclusive Aduio exclusive state.
     */
    virtual void SetAudioExclusive(bool audioExclusive) = 0;

    /**
     * Rigest the keep srceen on interface.
     *
     * @param windowId the window id.
     * @param callback the screenon handle callback.
     */
    virtual void RegisterScreenLockFunction(int32_t windowId,
        std::shared_ptr<NWebScreenLockCallback> callback) = 0;

    /**
     * UnRigest the keep srceen on interface.
     *
     * @param windowId the window id.
     */
    virtual void UnRegisterScreenLockFunction(int32_t windowId) = 0;

    /**
     * Notify memory level.
     *
     * @param level the memory level.
     */
    virtual void NotifyMemoryLevel(int32_t level) = 0;

    /**
     * Notify webview window status.
     */
    virtual void OnWebviewHide() = 0;
    virtual void OnWebviewShow() = 0;

    /**
     * Get the drag data.
     *
     * @return the data being dragged.
    */
    virtual std::shared_ptr<NWebDragData> GetOrCreateDragData() = 0;

    /**
     * Prefetch the resources required by the page, but will not execute js or
     * render the page.
     *
     * @param url  String: Which url to preresolve/preconnect.
     * @param additionalHttpHeaders Additional HTTP request headers of the URL.
     */
    virtual void PrefetchPage(
        const std::string& url,
        const std::map<std::string, std::string> &additionalHttpHeaders) = 0;

    /**
     * Set the window id.
     */
    virtual void SetWindowId(uint32_t window_id) = 0;

    /**
     * Notify that browser was occluded by other windows.
     */
    virtual void OnOccluded() = 0;

    /**
     * Notify that browser was unoccluded by other windows.
     */
    virtual void OnUnoccluded() = 0;

    /**
     * Set the token.
     */
    virtual void SetToken(void* token) = 0;

    /**
     * Set the nested scroll mode.
     */
    virtual void SetNestedScrollMode(const NestedScrollMode& nestedScrollMode) = 0;

    /**
     * Set enable lower the frame rate.
     */
    virtual void SetEnableLowerFrameRate(bool enabled) = 0;

    /**
     * Set the property values for width, height, and keyboard height.
     */
    virtual void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) = 0;

    /**
     * Set the virtual keyboard to override the web status.
     */
    virtual bool ShouldVirtualKeyboardOverlay() = 0;

    /**
     * Set draw rectmessage
     * @param x mean origin.x.
     * @param y mean origin.y.
     * @param width mean visible area'width.
     * @param height mean visible area.height.
     */
    virtual void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

    /**
     * Set draw mode.
     *
    */
    virtual void SetDrawMode(int32_t mode) = 0;

    /**
     * Create the web print document adapter.
     */
    virtual void* CreateWebPrintDocumentAdapter(const std::string& jobName) = 0;

    /**
     * Loads the URL with postData using "POST" method into this WebView.
     * If url is not a network URL, it will be loaded with loadUrl(String) instead.
     *
     * @param url String: the URL of the resource to load This value cannot be null.
     * @param postData the data will be passed to "POST" request,
     * whilch must be "application/x-www-form-urlencoded" encoded.
     *
     * @return title string for the current page.
     */
    virtual int PostUrl(const std::string& url,
                        const std::vector<char>& postData) = 0;

    /**
     * Inject the JavaScript before WebView loads the DOM tree and run JavaScripts.
     */
    virtual void JavaScriptOnDocumentStart(const ScriptItems& scriptItems) = 0;

    /**
     * Execute an accessibility action on an accessibility node in the browser.
     * @param accessibilityId The id of the accessibility node.
     * @param action The action to be performed on the accessibility node.
     */
    virtual void ExecuteAction(int64_t accessibilityId, uint32_t action) = 0;

    /**
     * Get the information of the focused accessibility node on the given accessibility node in the browser.
     * @param accessibilityId Indicate the accessibility id of the parent node of the focused accessibility node.
     * @param isAccessibilityFocus Indicate whether the focused accessibility node is accessibility focused or input
     * focused.
     * @return The obtained information of the accessibility node.
     */
    virtual std::shared_ptr<NWebAccessibilityNodeInfo>
    GetFocusedAccessibilityNodeInfo(int64_t accessibilityId,
                                    bool isAccessibilityFocus) = 0;

    /**
     * Get the information of the accessibility node by its accessibility id in the browser.
     * @param accessibilityId The accessibility id of the accessibility node.
     * @return The obtained information of the accessibility node.
     */
    virtual std::shared_ptr<NWebAccessibilityNodeInfo>
    GetAccessibilityNodeInfoById(int64_t accessibilityId) = 0;

    /**
     * Get the information of the accessibility node by focus move in the browser.
     * @param accessibilityId The accessibility id of the original accessibility node.
     * @param direction The focus move direction of the original accessibility node.
     * @return The obtained information of the accessibility node.
     */
    virtual std::shared_ptr<NWebAccessibilityNodeInfo>
    GetAccessibilityNodeInfoByFocusMove(int64_t accessibilityId,
                                        int32_t direction) = 0;

    /**
     * Set the accessibility state in the browser.
     * @param state Indicate whether the accessibility state is enabled or disabled.
     */
    virtual void SetAccessibilityState(bool state) = 0;

     /**
     * Get whether need soft keyboard.
     *
     * @return true if need soft keyboard, otherwise false.
     */
    virtual bool NeedSoftKeyboard() = 0;

    /**
     * Discard the webview window.
     * @return true if the discarding success, otherwise false.
     */
    virtual bool Discard() = 0;

    /**
     * Reload the webview window that has been discarded before.
     * @return true if the discarded window reload success, otherwise false.
     */
    virtual bool Restore() = 0;

    /**
     * Get the security level of current page.
     * @return security level for current page.
     */
    virtual int GetSecurityLevel() = 0;

    /**
     * CallH5Function
     *
     * @param routingId         int32_t: the h5 frame routing id
     * @param h5ObjectId        int32_t: the h5 side object id
     * @param h5MethodName      string: the h5 side object method name
     * @param args              vector<shared_ptr<NWebValue>>: the call args
     */
    virtual void CallH5Function(
        int32_t routingId,
        int32_t h5ObjectId,
        const std::string& h5MethodName,
        const std::vector<std::shared_ptr<NWebValue>>& args) = 0;

    /**
     * Get web weather has been set incognito mode.
     *
     * @return true if web is in incognito mode; otherwise fase.
     */
    virtual bool IsIncognitoMode() = 0;

    /**
     * Register native function.
     */
    virtual void RegisterNativeArkJSFunction(const char* objName,
        const std::vector<std::shared_ptr<NWebJsProxyCallback>> &callbacks) = 0;

    /**
     * Unregister native function.
     */
    virtual void UnRegisterNativeArkJSFunction(const char* objName) = 0;

    /**
     * Register native valide callback function.
     */
    virtual void RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback) = 0;

    /**
     * Register native destroy callback function.
     */
    virtual void RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnDestroyCallback callback) = 0;

    /**
     * Inject the JavaScript after WebView loads the DOM tree and run JavaScripts.
     */
    virtual void JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) = 0;

   /**
     * Enable the ability to check website security risks.
     * Illegal and fraudulent websites are mandatory enabled and cann't be disabled by this function.
     */
    virtual void EnableSafeBrowsing(bool enable) = 0;

    /**
     * Get whether checking website security risks is enabled.
     * @return true if enable the ability to check website security risks else false.
     */
    virtual bool IsSafeBrowsingEnabled() = 0;

    /**
     * Set the ability to print web page background.
     * @param enable Indicate whether the ability is enabled or disabled.
     */
    virtual void SetPrintBackground(bool enable) = 0;

    /**
     * Obtains whether to print the background of a web page.
     * @return true if enable print web page background, otherwise false.
     */
    virtual bool GetPrintBackground() = 0;

    /**
     * Close picture-in-picture video and fullScreen video.
     */
    virtual void CloseAllMediaPresentations() = 0;

    /**
     * Stop all audio and video playback on the web page.
     */
    virtual void StopAllMedia() = 0;

    /**
     * Restart playback of all audio and video on the web page.
     */
    virtual void ResumeAllMedia() = 0;

    /**
     * Pause all audio and video playback on the web page.
     */
    virtual void PauseAllMedia() = 0;

    /**
     * View the playback status of all audio and video on the web page.
     * @return The playback status of all audio and video.
     */
    virtual int GetMediaPlaybackState() = 0;

    /**
     * Enable the ability to intelligent tracking prevention, default disabled.
     */
    virtual void EnableIntelligentTrackingPrevention(bool enable) = 0;

    /**
     * Get whether intelligent tracking prevention is enabled.
     * @return true if enable the ability intelligent tracking prevention; else false.
     */
    virtual bool IsIntelligentTrackingPreventionEnabled() const = 0;

    /**
     * Start current camera.
     */
    virtual void StartCamera() = 0;

    /**
     * Stop current camera.
     */
    virtual void StopCamera() = 0;

    /**
     * Close current camera.
     */
    virtual void CloseCamera() = 0;

    /**
     * @brief Obtains the last javascript proxy calling frame url.
     *
     * @return the url of last calling frame url.
     */
    /*--ark web()--*/
    virtual std::string GetLastJavascriptProxyCallingFrameUrl() = 0;

    /**
     * @brief get pendingsize status.
     *
     * @return the result of last pendingsize status.
     */
    /*--ark web()--*/
    virtual bool GetPendingSizeStatus() = 0;

    /**
     * Scroll by the delta distance or velocity takes the screen as a reference.
     *
     * @param delta_x horizontal offset in physical pixel.
     * @param delta_y vertical offset in physical pixel.
     * @param vx      horizontal velocity in physical pixel.
     * @param vx      vertical velocity in physical pixel.
    */
    virtual void ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy) = 0;

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
     * @param callback callback will be called on getting the result of compiling javascript.
     */
    virtual void PrecompileJavaScript(
        const std::string &url,
        const std::string &script,
        std::shared_ptr<CacheOptions> &cacheOptions,
        std::shared_ptr<NWebMessageValueCallback> callback) = 0;

    virtual void OnCreateNativeMediaPlayer(std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) = 0;

    /**
     * @brief Web drag resize optimize.
     */
    /*--ark web()--*/
    virtual void DragResize(uint32_t width,
                            uint32_t height,
                            uint32_t pre_height,
                            uint32_t pre_width) = 0;

    virtual void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) = 0;

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
    virtual int ScaleGestureChange(double scale, double centerX, double centerY) = 0;

    /**
     * @brief Inject offline resource into MemoryCache.
     *
     * @param url url of resource.
     * @param origin origin of resource.
     * @param resource data of resource.
     * @param response_headers response headers of resource.
     * @param type resource type.
     */
    virtual void InjectOfflineResource(const std::string& url,
                                       const std::string& origin,
                                       const std::vector<uint8_t>& resource,
                                       const std::map<std::string, std::string>& responseHeaders,
                                       const int type) = 0;

    /*
     * @brief Terminate render process
     *
     * @return true if it was possible to terminate this render process, false
     *         otherwise.
     */
    /*--ark web()--*/
    virtual bool TerminateRenderProcess() = 0;
   
    /**
     * Get value of Autofill index.
     * @param index index value.
     */
    virtual void SuggestionSelected(int32_t index) = 0;

    /**
     * RegisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list  vector<String>: vector list, sync method list
     * @param async_method_list  vector<String>: vector list, async method list
     * @param object_id    int32_t: object id
     */
    virtual void RegisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list,
        const std::vector<std::string>& async_method_list,
        const int32_t object_id) = 0;

    /**
     * @brief Send touchpad fling event.
     *
     * @param x location of x.
     * @param y location of y.
     * @param vx velocity of x.
     * @param vy velocity of y.
     */
    virtual void SendTouchpadFlingEvent(double x, double y, double vx, double vy) = 0;
};
}  // namespace OHOS::NWeb

#endif // NWEB_H
