/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "nweb_accessibility_event_callback.h"
#include "nweb_accessibility_node_info.h"
#include "nweb_agent_manager.h"
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
#include "nweb_rom_value.h"
#include "nweb_snapshot_callback.h"
#include "nweb_spanstring_convert_html_callback.h"
#include "nweb_value_callback.h"
#include "nweb_vault_plain_text_callback.h"
#include "nweb_web_message.h"
#include "nweb_print_manager_adapter.h"
#include "nweb_blankless_callback.h"
#include "nweb_user_agent_metadata.h"

namespace OHOS::NWeb {

class NWebAgentHandler;
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

enum class SourceTool : int32_t {
    UNKNOWN = 0,
    FINGER = 1,
    PEN = 2,
    RUBBER = 3,
    BRUSH = 4,
    PENCIL = 5,
    AIRBRUSH = 6,
    MOUSE = 7,
    LENS = 8,
    TOUCHPAD = 9,
    JOYSTICK = 10,
};

enum class NWebAutoFillTriggerType : int32_t {
    AUTO_REQUEST = 0,
    MANUAL_REQUEST,
    PASTE_REQUEST,
    UNSPECIFIED
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
    virtual std::string GetSharedRenderProcessToken() {return "";}
    virtual bool GetEmulateTouchFromMouseEvent() {return false;}
};

class OHOS_NWEB_EXPORT NWebOutputFrameCallback {
public:
    virtual ~NWebOutputFrameCallback() = default;

    virtual bool Handle(const char* buffer, uint32_t width, uint32_t height) = 0;
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
    virtual void* GetProducerSurface() = 0;
    virtual void* GetEnhanceSurfaceInfo() = 0;

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
    virtual OHOS::NWeb::NWebDragData::DragOperation GetDragOperation() const {
        return OHOS::NWeb::NWebDragData::DragOperation::DRAG_OPERATION_COPY;
    }
    virtual OHOS::NWeb::NWebDragData::DragOperationsMask GetAllowedDragOperation() const {
        return OHOS::NWeb::NWebDragData::DragOperationsMask::DRAG_ALLOW_EVERY;
    }
    virtual bool IsDragOpValid() const {
        return false;
    };
};

enum class BlurReason : int32_t {
    FOCUS_SWITCH = 0,
    WINDOW_BLUR = 1,
    FRAME_DESTROY = 2, // frame node detached from main tree
    VIEW_SWITCH = 3,
    CLEAR_FOCUS = 4, // User api clearFocus triggered
};

enum class FocusReason : int32_t {
    FOCUS_DEFAULT = 0,
    EVENT_REQUEST = 1,
};

enum class RenderProcessMode : int32_t {
    SINGLE_MODE = 0,
    MULTIPLE_MODE = 1,
};

enum class SiteIsolationMode {
    PARTIAL,
    STRICT,
};

enum class SetSiteIsolationModeErr : int32_t {
    ADVANCED_SECURITY_SET_ERR = 2,
    SINGLE_RENDER_SET_STRICT_ERR = 3,
    ALREADY_SET_ERR = 4,
};

class NWebTouchPointInfo {
public:
    virtual ~NWebTouchPointInfo() = default;

    virtual int GetId() = 0;
    virtual double GetX() = 0;
    virtual double GetY() = 0;
};

class OHOS_NWEB_EXPORT NWebCommandAction {
public:
    virtual ~NWebCommandAction() = default;

    /**
     * @brief Retrieves the type of the simulated event (e.g., "click", "scroll").
     * @return A string representing the event type.
     */
    virtual std::string GetEventType() = 0;

    /**
     * @brief Retrieves the XPath path of the target element.
     * @return A string representing the XPath path.
     */
    virtual std::string GetXPath() = 0;

    /**
     * @brief Retrieves the duration of the action (e.g., duration for a scroll).
     * @return The duration in milliseconds (ms).
     */
    virtual int32_t GetDuration() = 0;

    /**
     * @brief Retrieves the alignment mode (e.g., alignment for scroll top/mid/bottom).
     * @return A string representing the alignment mode.
     */
    virtual std::string GetAlign() = 0;

    /**
     * @brief Retrieves the offset value for the action.
     * @return The offset value (typically in vp).
     */
    virtual int32_t GetOffset() = 0;
};

class NWebStylusTouchPointInfo : public NWebTouchPointInfo {
public:
    virtual ~NWebStylusTouchPointInfo() = default;
    virtual float GetForce() = 0;
    virtual float GetTiltX() = 0;
    virtual float GetTiltY() = 0;
    virtual float GetRollAngle() = 0;
    virtual int32_t GetWidth() = 0;
    virtual int32_t GetHeight() = 0;
    virtual SourceTool GetSourceTool() = 0;
};

enum class NestedScrollMode : int32_t {
    SELF_ONLY = 0,
    SELF_FIRST = 1,
    PARENT_FIRST = 2,
    PARALLEL = 3,
};

class NWebScreenLockCallback {
public:
    virtual ~NWebScreenLockCallback() = default;

    virtual void Handle(bool key) = 0;
};

typedef char* (*NativeArkWebOnJavaScriptProxyCallback)(const char**, int32_t);
class NWebJsProxyCallback {
public:
    virtual ~NWebJsProxyCallback() = default;

    virtual std::string GetMethodName() = 0;

    virtual NativeArkWebOnJavaScriptProxyCallback GetMethodCallback() = 0;
};

class OHOS_NWEB_EXPORT NWebEnginePrefetchArgs {
public:
    virtual ~NWebEnginePrefetchArgs() = default;

    virtual std::string GetUrl() = 0;
    virtual std::string GetMethod() = 0;
    virtual std::string GetFormData() = 0;
};

class OHOS_NWEB_EXPORT NWebPrefetchOptions {
public:
    virtual ~NWebPrefetchOptions() = default;

    virtual int32_t GetminTimeBetweenPrefetchesMs() = 0;
    virtual bool GetignoreCacheControlNoStore() = 0;
};

class OHOS_NWEB_EXPORT NWebPDFConfigArgs {
public:
    virtual ~NWebPDFConfigArgs() = default;

    virtual double GetWidth() = 0;
    virtual double GetHeight() = 0;
    virtual double GetScale() = 0;
    virtual double GetMarginTop() = 0;
    virtual double GetMarginBottom() = 0;
    virtual double GetMarginRight() = 0;
    virtual double GetMarginLeft() = 0;
    virtual bool GetShouldPrintBackground() = 0;
};

enum class PrecompileError : int32_t { OK = 0, INTERNAL_ERROR = -1 };

class OHOS_NWEB_EXPORT CacheOptions {
public:
    virtual ~CacheOptions() = default;

    virtual std::map<std::string, std::string> GetResponseHeaders() = 0;
};

class OHOS_NWEB_EXPORT NWebKeyboardEvent {
public:
    virtual ~NWebKeyboardEvent() = default;

    virtual int32_t GetKeyCode() = 0;

    virtual int32_t GetAction() = 0;

    virtual int32_t GetUnicode() = 0;

    virtual bool IsEnableCapsLock() = 0;

    virtual std::vector<int32_t> GetPressKeyCodes() = 0;
};

enum class PixelUnit {
    PX = 0,
    VP = 1,
    PERCENTAGE = 2,
    NONE = 3,
};

enum class WebDestroyMode {
    NORMAL_MODE,
    FAST_MODE
};

enum class ScrollbarMode {
    OVERLAY_LAYOUT_SCROLLBAR,
    FORCE_DISPLAY_SCROLLBAR
};

enum class NavigationPolicy : int32_t {
    NEW_POPUP = 0,
    NEW_WINDOW = 1,
    NEW_BACKGROUND_TAB = 2,
    NEW_FOREGROUND_TAB = 3
};

enum class WebHttpCookieSameSitePolicy {
    NONE = 0,
    LAX = 1,
    STRICT = 2
};

class OHOS_NWEB_EXPORT NWebMouseEvent {
public:
    virtual ~NWebMouseEvent() = default;

    virtual int32_t GetX() = 0;

    virtual int32_t GetY() = 0;

    virtual int32_t GetButton() = 0;

    virtual int32_t GetAction() = 0;

    virtual int32_t GetClickNum() = 0;

    virtual std::vector<int32_t> GetPressKeyCodes() = 0;

    virtual int32_t GetRawX() { return 0; }

    virtual int32_t GetRawY() { return 0; }

};

typedef int64_t (*AccessibilityIdGenerateFunc)();
typedef void (*NativeArkWebOnValidCallback)(const char*);
typedef void (*NativeArkWebOnDestroyCallback)(const char*);
using ScriptItems = std::map<std::string, std::vector<std::string>>;
using ScriptRegexItems = std::map<std::string, std::vector<std::pair<std::string, std::string>>>;
using ScriptItemsByOrder = std::vector<std::string>;
using WebSnapshotCallback = std::function<void(const char*, bool, float, void*, int, int)>;

enum class SystemThemeFlags : uint8_t {
    NONE = 0,
    THEME_FONT = 1 << 0,
};

enum class WebSoftKeyboardBehaviorMode : int32_t {
    DEFAULT = 0,
    DISABLE_AUTO_KEYBOARD_ON_ACTIVE,
};

class NWebSystemConfiguration {
    public:
    virtual ~NWebSystemConfiguration() = default;

    virtual uint8_t GetThemeFlags() = 0;
};

class OHOS_NWEB_EXPORT NWebJsProxyMethod {
    public:
        virtual ~NWebJsProxyMethod() = default;
    
        virtual int32_t GetSize() = 0;
    
        virtual void OnHandle(int32_t number, const std::vector<std::string>& param) = 0;
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
    virtual void OnTouchMove(
        const std::vector<std::shared_ptr<NWebTouchPointInfo>>& touch_point_infos, bool fromOverlay = false) = 0;
    virtual void OnTouchCancel() = 0;
    virtual void OnNavigateBack() = 0;
    virtual bool SendKeyEvent(int32_t keyCode, int32_t keyAction) = 0;
    virtual void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY) = 0;
    virtual void SendMouseEvent(int x, int y, int button, int action, int count) = 0;

    /**
     * Loads the given URL.
     *
     * @param url String: the URL of the resource to load This value cannot be
     * null.
     *
     * @return title string for the current page.
     */
    virtual int Load(const std::string& url) = 0;
    /**
     * Gets whether this NWeb has a back history item.
     *
     * @return true if this NWeb has a back history item
     */
    virtual bool IsNavigatebackwardAllowed() = 0;
    /**
     * Gets whether this NWeb has a forward history item.
     *
     * @return true if this NWeb has a forward history item
     */
    virtual bool IsNavigateForwardAllowed() = 0;
    /**
     * Gets whether this NWeb has a back or forward history item for number of
     * steps.
     *
     * @param numSteps int: the negative or positive number of steps to move the
     * history
     * @return true if this NWeb has a forward history item
     */
    virtual bool CanNavigateBackOrForward(int numSteps) = 0;
    /**
     * Goes back in the history of this NWeb.
     *
     */
    virtual void NavigateBack() = 0;
    /**
     * Goes forward in the history of this NWeb.
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
     * Reloads the current URL.
     *
     */
    virtual void Reload() = 0;
    /**
     * Performs a zoom operation in this NWeb.
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
     * Stops the current load.
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
        const std::string& code, std::shared_ptr<NWebMessageValueCallback> callback, bool extention) = 0;
    /**
     * ExecuteJavaScript with ashmem
     *
     * @param fd fd of the ashmem
     * @param scriptLength javascript code length
     * @param callback NWebValueCallback: javascript running result
     * @param extention true if is extention
     */
    virtual void ExecuteJavaScriptExt(const int fd, const size_t scriptLength,
        std::shared_ptr<NWebMessageValueCallback> callback, bool extention) = 0;
    /**
     * Gets the NWebPreference object used to control the settings for this
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
     * Gets the last hit test result.
     *
     * @return the last HitTestResult
     */
    virtual std::shared_ptr<HitTestResult> GetHitTestResult() = 0;

    /**
     * Sets the background color for this view.
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
     * Sets the NWebDownloadCallback that will receive download event.
     * This will replace the current handler.
     *
     * @param downloadListener NWebDownloadCallback:
     *
     */
    virtual void PutDownloadCallback(std::shared_ptr<NWebDownloadCallback> downloadListener) = 0;

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
    virtual void PutAccessibilityIdGenerator(const AccessibilityIdGenerateFunc accessibilityIdGenerator) = 0;

    /**
     * Set the NWebHandler that will receive various notifications and
     * requests. This will replace the current handler.
     *
     * @param client NWebHandler: an implementation of NWebHandler This value
     * cannot be null.
     *
     */
    virtual void SetNWebHandler(std::shared_ptr<NWebHandler> handler) = 0;

    /**
     * Gets the title for the current page.
     *
     * @return title string for the current page.
     */
    virtual std::string Title() = 0;

    /**
     * Gets the progress for the current page.
     *
     * @return progress for the current page.
     */
    virtual int PageLoadProgress() = 0;

    /**
     * Gets the height of the HTML content.
     *
     * @return the height of the HTML content.
     */
    virtual int ContentHeight() = 0;

    /**
     * Gets the current scale of this NWeb.
     *
     * @return the current scale
     */
    virtual float Scale() = 0;

    /**
     * Loads the given URL with additional HTTP headers, specified as a map
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
    virtual int Load(const std::string& url, const std::map<std::string, std::string>& additionalHttpHeaders) = 0;

    /**
     * Loads the given data into this NWeb, using baseUrl as the base URL for
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
    virtual int LoadWithDataAndBaseUrl(const std::string& baseUrl, const std::string& data, const std::string& mimeType,
        const std::string& encoding, const std::string& historyUrl) = 0;

    /**
     * Loads the given data into this NWeb.
     *
     * @param data String: the URL to use as the page's base URL. If null defaults
     * to 'about:blank'. This value may be null.
     * @param mimeType String: the MIME type of the data, e.g. 'text/html'. This
     * value may be null.
     * @param encoding String: the encoding of the data This value may be null.
     */
    virtual int LoadWithData(const std::string& data, const std::string& mimeType, const std::string& encoding) = 0;

    /**
     * RegisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list vector<String>: vector list ,method list
     * @param object_id int32_t: object id
     */
    virtual void RegisterArkJSfunction(
        const std::string& object_name, const std::vector<std::string>& method_list, const int32_t object_id) = 0;

    /**
     * UnregisterArkJSfunction
     *
     * @param object_name  String: objector name
     * @param method_list vector<String>: vector list ,method list
     */
    virtual void UnregisterArkJSfunction(
        const std::string& object_name, const std::vector<std::string>& method_list) = 0;

    /**
     * SetNWebJavaScriptResultCallBack
     *
     * @param callback  NWebJavaScriptResultCallBack: callback client
     */
    virtual void SetNWebJavaScriptResultCallBack(std::shared_ptr<NWebJavaScriptResultCallBack> callback) = 0;

    /**
     * Set the NWebFindCallback that will receive find event.
     * This will replace the current handler.
     *
     * @param findListener NWebFindCallback : find callback
     */
    virtual void PutFindCallback(std::shared_ptr<NWebFindCallback> findListener) = 0;

    /**
     * Finds all instances of find on the page and highlights them,
     * asynchronously.
     *
     * @param searchStr String: target string to find.
     */
    virtual void FindAllAsync(const std::string& searchStr) = 0;

    /**
     * Clears the highlighting surrounding text matches created by findAllAsync
     *
     */
    virtual void ClearMatches() = 0;

    /**
     * Highlights and scrolls to the next match found by findAllAsync(String),
     * wrapping around page boundaries as necessary.
     *
     * @param forward bool: find back or forward:
     */
    virtual void FindNext(const bool forward) = 0;

    /**
     * Saves the current view as a web archive.
     *
     * @param baseName the filename where the archive should be placed This
     * value cannot be null.
     * @param autoName if false, takes basename to be a file. If true, basename
     * is assumed to be a directory in which a filename will be chosen according
     * to the URL of the current page.
     */
    virtual void StoreWebArchive(
        const std::string& baseName, bool autoName, std::shared_ptr<NWebStringValueCallback> callback) = 0;

    /**
     * creating two ends of a message channel.
     *
     * @return the web message ports get from nweb.
     */
    virtual std::vector<std::string> CreateWebMessagePorts() = 0;

    /**
     * Posts MessageEvent to the main frame.
     *
     * @param message message send to mmain frame.
     * @param ports the web message ports send to main frame.
     * @param targetUri the uri which can received the ports.
     */
    virtual void PostWebMessage(
        const std::string& message, const std::vector<std::string>& ports, const std::string& targetUri) = 0;

    /**
     * close the message port.
     *
     * @param portHandle the port to close.
     */
    virtual void ClosePort(const std::string& portHandle) = 0;

    /**
     * use the port to send message.
     *
     * @param portHandle the port to send message.
     * @param data the message to send.
     */
    virtual void PostPortMessage(const std::string& portHandle, std::shared_ptr<NWebMessage> data) = 0;

    /**
     * set the callback of the message port.
     *
     * @param portHandle the port to set callback.
     * @param callback to reveive the result when the other port post message.
     */
    virtual void SetPortMessageCallback(
        const std::string& portHandle, std::shared_ptr<NWebMessageValueCallback> callback) = 0;

    /**
     * send drag event to nweb.
     * @param dragEvent the drag event information.
     */
    virtual void SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) = 0;

    /**
     * Clear ssl cache.
     */
    virtual void ClearSslCache() = 0;

    /**
     * get web page url.
     *
     * @return web page url.
     */
    virtual std::string GetUrl() = 0;

    /**
     * Clears the client authentication certificate Cache in the Web.
     *
     */
    virtual void ClearClientAuthenticationCache() = 0;

    /**
     * set the locale name of current system setting..
     *
     * @param locale the locale name of current system setting.
     */
    virtual void UpdateLocale(const std::string& language, const std::string& region) = 0;

    /**
     * get original url of the request.
     *
     * @return original url.
     */
    virtual const std::string GetOriginalUrl() = 0;

    /**
     * get original url of the request.
     *
     * @param data raw image data of the icon.
     * @param width width of the icon.
     * @param height height of the icon.
     * @param colorType the color type of the icon.
     * @param alphaType the alpha type of the icon.
     * @return the result of get favicon.
     */
    virtual bool GetFavicon(
        const void** data, size_t& width, size_t& height, ImageColorType& c, ImageAlphaType& alphaType) = 0;

    /**
     * set the network status, just notify the webview to change the JS navigatoer.online.
     *
     * @param available width of the icon.
     */
    virtual void PutNetworkAvailable(bool available) = 0;

    /**
     * web has image or not.
     *
     * @param callback has image or not
     */
    virtual void HasImages(std::shared_ptr<NWebBoolValueCallback> callback) = 0;

    /**
     * web remove cache.
     *
     * @param include_disk_files bool: if false, only the RAM cache is removed
     */
    virtual void RemoveCache(bool include_disk_files) = 0;

    /**
     * web has image or not.
     *
     * @param web has image or not
     */
    virtual std::shared_ptr<NWebHistoryList> GetHistoryList() = 0;

    /**
     * Set the NWebReleaseSurfaceCallback that will receive release surface event.
     * This will replace the current handler.
     *
     * @param releaseSurfaceListener NWebReleaseSurfaceCallback.
     */
    virtual void PutReleaseSurfaceCallback(std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) = 0;

    /**
     * Get web back forward state.
     *
     * @return web back forward state.
     */
    virtual std::vector<uint8_t> SerializeWebState() = 0;

    /**
     * Restore web back forward state.
     *
     * @param web back forward state.
     */
    virtual bool RestoreWebState(const std::vector<uint8_t>& state) = 0;

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
     * @param x horizontal coordinate.
     * @param y vertical coordinate.
     */
    virtual void ScrollTo(float x, float y) = 0;

    /**
     * Scroll by the delta distance.
     *
     * @param delta_x horizontal offset.
     * @param delta_y vertical offset.
     */
    virtual void ScrollBy(float delta_x, float delta_y) = 0;

    /**
     * Slide scroll by the speed.
     *
     * @param vx horizontal slide speed.
     * @param vy vertical slide speed.
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
    virtual void RegisterScreenLockFunction(int32_t windowId, std::shared_ptr<NWebScreenLockCallback> callback) = 0;

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
     * Get drag data.
     *
     * @return the drag data.
     */
    virtual std::shared_ptr<NWebDragData> GetOrCreateDragData() = 0;

    /**
     * Prefetch the resources required by the page, but will not execute js or
     * render the page.
     *
     * @param url  String: Which url to preresolve/preconnect.
     * @param additionalHttpHeaders Additional HTTP request header of the URL.
     */
    virtual void PrefetchPage(
        const std::string& url, const std::map<std::string, std::string>& additionalHttpHeaders) = 0;

    /**
     * Set the window id.
     */
    virtual void SetWindowId(uint32_t window_id) = 0;

    /**
     * Notify that browser was occluded by other windows.
     */
    virtual void OnOccluded() = 0;

    /**
     *Notify that browser was unoccluded by other windows.
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
     * Set draw rect.
     *
     */
    virtual void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

    /**
     * Set draw mode.
     *
     */
    virtual void SetDrawMode(int32_t mode) = 0;

    /**
     * Create web print document adapter.
     *
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
    virtual int PostUrl(const std::string& url, const std::vector<char>& postData) = 0;

    /**
     * Set the property values for width, height, and keyboard height.
     */
    virtual void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) = 0;

    /**
     * Set the virtual keyboard to override the web status.
     */
    virtual bool ShouldVirtualKeyboardOverlay() = 0;

    /**
     * Inject the JavaScript before WebView load the DOM tree.
     */
    virtual void JavaScriptOnDocumentStart(const ScriptItems& scriptItems) = 0;

    /**
     * Set enable lower the frame rate.
     */
    virtual void SetEnableLowerFrameRate(bool enabled) = 0;

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
    virtual std::shared_ptr<NWebAccessibilityNodeInfo> GetFocusedAccessibilityNodeInfo(
        int64_t accessibilityId, bool isAccessibilityFocus) = 0;

    /**
     * Get the information of the accessibility node by its accessibility id in the browser.
     * @param accessibilityId The accessibility id of the accessibility node.
     * @return The obtained information of the accessibility node.
     */
    virtual std::shared_ptr<NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoById(int64_t accessibilityId) = 0;

    /**
     * Get the information of the accessibility node by focus move in the browser.
     * @param accessibilityId The accessibility id of the original accessibility node.
     * @param direction The focus move direction of the original accessibility node.
     * @return The obtained information of the accessibility node.
     */
    virtual std::shared_ptr<NWebAccessibilityNodeInfo> GetAccessibilityNodeInfoByFocusMove(
        int64_t accessibilityId, int32_t direction) = 0;

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
     * CallH5Function
     *
     * @param routing_id       int32_t: the h5 frmae routing id
     * @param h5_object_id     int32_t: the h5 side object id
     * @param h5_method_name   string:  the h5 side object method name
     * @param args             vector<shared_ptr<NWebValue>>: the call args
     */
    virtual void CallH5Function(int32_t routing_id, int32_t h5_object_id, const std::string& h5_method_name,
        const std::vector<std::shared_ptr<NWebValue>>& args) = 0;

    /**
     * Get web whether has been set incognito mode.
     *
     * @return true if web is in incognito mode; otherwise fase.
     */
    virtual bool IsIncognitoMode() = 0;

    /**
     * Register native function.
     */
    virtual void RegisterNativeArkJSFunction(
        const char* objName, const std::vector<std::shared_ptr<NWebJsProxyCallback>>& callbacks) = 0;

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
     * Inject the JavaScript after WebView load the DOM tree.
     */
    virtual void JavaScriptOnDocumentEnd(const ScriptItems& scriptItems) = 0;

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
     * Get the security level of current page.
     * @return security level for current page.
     */
    virtual int GetSecurityLevel() = 0;

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
     * Enable the ability to intelligent tracking prevention, default disabled.
     */
    virtual void EnableIntelligentTrackingPrevention(bool enable) = 0;

    /**
     * Get whether intelligent tracking prevention is enabled.
     * @return true if enable the ability intelligent tracking prevention; else false.
     */
    virtual bool IsIntelligentTrackingPreventionEnabled() const = 0;

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
    virtual void PrecompileJavaScript(const std::string& url, const std::string& script,
        std::shared_ptr<CacheOptions>& cacheOptions, std::shared_ptr<NWebMessageValueCallback> callback) = 0;

    virtual void OnCreateNativeMediaPlayer(std::shared_ptr<NWebCreateNativeMediaPlayerCallback> callback) = 0;

    virtual void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) = 0;

    /**
     * Inject Offline Resource into Memory Cache.
     *
     * @param url url of resource.
     * @param origin origin of resource.
     * @param resource data of resource.
     * @param response_headers response headers of resource.
     * @param type resource type.
     */
    virtual void InjectOfflineResource(const std::string& url, const std::string& origin,
        const std::vector<uint8_t>& resource, const std::map<std::string, std::string>& responseHeaders,
        const int type) = 0;

    /**
     * @brief Terminate render process
     *
     * @return true if it was possible to terminate this render process, false
     *         otherwise.
     */
    /*--ark web()--*/
    virtual bool TerminateRenderProcess() = 0;

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
     * RegisterArkJSfunction
     *
     * @param object_name  String: object name
     * @param method_list vector<String>: vector list, async method list
     * @param method_list vector<String>: vector list, sync method list
     * @param object_id int32_t: object id
     */
    virtual void RegisterArkJSfunction(const std::string& object_name, const std::vector<std::string>& method_list,
        const std::vector<std::string>& async_method_list, const int32_t object_id) = 0;

    /**
     * Get value of Autofill index.
     * @param index index value.
     */
    virtual void SuggestionSelected(int32_t index) = 0;

    /**
     * @brief Send touchpad fling event.
     *
     * @param x location of x.
     * @param y location of y.
     * @param vx velocity of x.
     * @param vy velocity of y.
     */
    virtual void SendTouchpadFlingEvent(double x, double y, double vx, double vy) = 0;

    /**
     * Set fit content mode.
     *
     */
    virtual void SetFitContentMode(int32_t mode) = 0;

    /**
     * Get select info.
     *
     */
    virtual std::string GetSelectInfo() = 0;

    /**
     * @brief Notify that safe insets change.
     *
     */
    virtual void OnSafeInsetsChange(int left, int top, int right, int bottom) = 0;

    /**
     * @brief Render process switch to foreground.
     */
    /*--ark web()--*/
    virtual void OnOnlineRenderToForeground() = 0;

    /**
     * @brief Called when text is selected in image.
     */
    /*--ark web()--*/
    virtual void OnTextSelected() = 0;

    /**
     * @brief Notify for next touch move event.
     *
     */
    /*--ark web()--*/
    virtual void NotifyForNextTouchEvent() {}

    /**
     * @brief Enable the ability to block Ads, default disabled.
     */
    virtual void EnableAdsBlock(bool enable) {}

    /**
     * @brief Get whether Ads block is enabled.
     */
    virtual bool IsAdsBlockEnabled()
    {
        return false;
    }

    /**
     * @brief Get whether Ads block is enabled for current Webpage.
     */
    virtual bool IsAdsBlockEnabledForCurPage()
    {
        return false;
    }

    /**
     * @brief Get Web page snapshot
     *
     * @param id Request id.
     * @param width Request SnapShot width.
     * @param height Request SnapShot height.
     * @param callback SnapShot result callback.
     * @return ture if succuess request snapshot to renderer.
     */
    /*--ark web()--*/
    virtual bool WebPageSnapshot(const char* id,
                                 PixelUnit type,
                                 int width,
                                 int height,
                                 const WebSnapshotCallback callback) {
        return false;
    }

    /**
     * @brief Notify that system configuration change.
     *
     * @param configuration system configuration.
    */
    /*--ark web()--*/
    virtual void OnConfigurationUpdated(std::shared_ptr<NWebSystemConfiguration> configuration) {}

    /**
     * @brief Set url trust list.
     *
     * @param urlTrustList The url Trust list.
     */
    virtual int SetUrlTrustList(const std::string& urlTrustList) {
        return 0;
    }

    /**
     * @brief Put the callback, convert sapnstring to html.
     *
     * @param callback will convert spanstring to html.
     */
    virtual void PutSpanstringConvertHtmlCallback(
        std::shared_ptr<NWebSpanstringConvertHtmlCallback> callback) {}

    /**
     * Web send key event.
     * @param key_code code value.
     * @param key_action action value.
     * @param pressedCodes pressedCodes value.
     */
    /*--ark web()--*/
    virtual bool WebSendKeyEvent(int32_t keyCode, int32_t keyAction, const std::vector<int32_t>& pressedCodes) {
        return false;
    }

    /**
     * Set grant file access dirs.
     */
    virtual void SetPathAllowingUniversalAccess(const std::vector<std::string>& dirList,
                                                const std::vector<std::string>& moduleName,
                                                std::string& errorPath) {}

    /**
     * @brief Send mouse wheel event.
     */
    virtual void WebSendMouseWheelEvent(double x,
                                        double y,
                                        double delta_x,
                                        double delta_y,
                                        const std::vector<int32_t>& pressedCodes) {}

    /**
     * @brief Send touchpad fling event.
     *
     * @param x location of x.
     * @param y location of y.
     * @param vx velocity of x.
     * @param vy velocity of y.
     * @param pressedCodes pressed codes.
     */
    virtual void WebSendTouchpadFlingEvent(double x,
                                           double y,
                                           double vx,
                                           double vy,
                                           const std::vector<int32_t>& pressedCodes) {}

    /**
     * @brief Set url trust list with error message.
     *
     * @param urlTrustList The url Trust list.
     * @param detailErrMsg The url trust list detail message.
     */
    virtual int SetUrlTrustListWithErrMsg(const std::string& urlTrustList, std::string& detailErrMsg) {
        return 0;
    }

    /**
     * RegisterArkJSfunction
     *
     * @param object_name  String: object name
     * @param method_list vector<String>: vector list, async method list
     * @param method_list vector<String>: vector list, sync method list
     * @param object_id int32_t: object id
     * @param permission String: allow list
     */
    virtual void RegisterArkJSfunction(const std::string& object_name, const std::vector<std::string>& method_list,
        const std::vector<std::string>& async_method_list, const int32_t object_id, const std::string& permission) {}

    /**
     * RegisterArkJSfunctionV2
     *
     * @param object_name  String: object name
     * @param method_list vector<String>: vector list, async method list
     * @param method_list vector<String>: vector list, sync method list
     * @param object_id int32_t: object id
     * @param permission String: allow list
     */
    virtual void RegisterArkJSfunctionV2(const std::string& object_name, const std::vector<std::string>& method_list,
        const std::vector<std::string>& async_method_list, const int32_t object_id, const std::string& permission) {}

    /**
     * @brief resize visual viewport.
     *
     * @param width width.
     * @param height height.
     * @param iskeyboard from keybord.
     */
    virtual void ResizeVisibleViewport(uint32_t width, uint32_t height, bool isKeyboard) {}

    /**
     * @brief Set backforward cache options.
     *
     * @param size The size of the back forward cache could saved.
     * @param timeToLive The time of the back forward cache page could stay.
     */
    virtual void SetBackForwardCacheOptions(int32_t size, int32_t timeToLive) { return; }

    /**
     * @brief set the callback of the autofill event.
     * @param callback callback.
     */
    virtual void SetAutofillCallback(std::shared_ptr<NWebMessageValueCallback> callback) {}

    /**
     * @brief fill autofill data.
     * @param data data.
     */
    virtual void FillAutofillData(std::shared_ptr<NWebMessage> data) {}

    /**
     * @brief on autofill cancel.
     * @param fillContent fillContent
     */
    virtual void OnAutofillCancel(const std::string& fillContent) {}

    /**
     * Execute an accessibility action on an accessibility node in the browser.
     * @param accessibilityId The id of the accessibility node.
     * @param action The action to be performed on the accessibility node.
     * @param actionArguments Data related to the current action.
     */
    virtual void PerformAction(int64_t accessibilityId, uint32_t action,
        const std::map<std::string, std::string>& actionArguments) {}

    /**
     * @brief Send the accessibility hover event coordinate.
     *
     * @param x horizontal location of coordinate.
     * @param y vertical location of coordinate.
     */
    virtual void SendAccessibilityHoverEvent(int32_t x, int32_t y) {}

    /**
     * Scroll by the delta distance if web is not foucsed.
     *
     * @param delta_x horizontal offset.
     * @param delta_y vertical offset.
     * @return false if web is focused.
    */
    virtual bool ScrollByWithResult(float delta_x, float delta_y) {
        return false;
    }

    /**
     * @brief Called when image analyzer is destory.
     */
    virtual void OnDestroyImageAnalyzerOverlay() {}

    /**
     * Scroll to the position.
     *
     * @param x horizontal coordinate.
     * @param y vertical coordinate.
     * @param duration: anime duration.
     */
    virtual void ScrollToWithAnime(float x, float y, int32_t duration) {}

    /**
     * Scroll by the delta distance.
     *
     * @param delta_x: horizontal offset.
     * @param delta_y: vertical offset.
     * @param duration: anime duration.
     */
    virtual void ScrollByWithAnime(float delta_x, float delta_y, int32_t duration) {}

    /**
     * @brief Get the current scroll offset of the webpage.
     * @param offset_x The current horizontal scroll offset of the webpage.
     * @param offset_y The current vertical scroll offset of the webpage.
     */
    virtual void GetScrollOffset(float* offset_x, float* offset_y) {}

    /**
     * @brief set a popupSurface to draw popup content
     * @param popupSurface  popupSurface.
     */
    virtual void SetPopupSurface(void* popupSurface) {}

    /**
     * @Description: Sends mouse events to the web kernel.
     * @Input mouseEvent: Basic information about mouse events.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual void WebSendMouseEvent(const std::shared_ptr<OHOS::NWeb::NWebMouseEvent>& mouseEvent) {}

    /**
     * @brief set DPI when DPI changes.
     * @param density The new density value.
     */
    virtual void SetSurfaceDensity(const double& density) {}

    /**
     * @brief ExecuteCreatePDFExt
     *
     * @param pdfConfig The current configuration when creating pdf.
     * @param callback NWebArrayBufferValueCallback: CreatePDF running result.
     */
    virtual void ExecuteCreatePDFExt(std::shared_ptr<NWebPDFConfigArgs> pdfConfig,
        std::shared_ptr<NWebArrayBufferValueCallback> callback) {}

     /**
     * @Description: Get the accessibility visibility of the accessibility node by its accessibility id in the browser.
     * @Input accessibility_id: The accessibility id of the accessibility node.
     * @Return: The accessibility visibility of the accessibility node.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual bool GetAccessibilityVisible(int64_t accessibility_id) {
        return true;
    }

    /**
     * @Description: Set the rotation to psurface.
     * @Input rotation: The rotation of buffer.
     * @Since: 12005
     */
    /*--ark web()--*/
    virtual void SetTransformHint(uint32_t rotation) {}

    /**
     * @brief Web components blur when the keyboard is hidden by gesture back.
     */
    virtual void WebComponentsBlur() {}

    /**
     * @brief Set the params when the scale of WebView changed by pinch gesture.
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
    virtual int ScaleGestureChangeV2(int type, double scale, double originScale, double centerX, double centerY) {
        return 0;
    }

    /**
     * @Description: Sends key events to the web kernel.
     * @Input keyEvent: Basic information about key events.
     * @Return: Whether the keyboard event is successful sent.
     */
    /*--ark web()--*/
    virtual bool SendKeyboardEvent(const std::shared_ptr<OHOS::NWeb::NWebKeyboardEvent>& keyboardEvent) {
        return false;
    }

    /**
     * Inject the JavaScript before WebView load the DOM tree.
     */
    virtual void JavaScriptOnDocumentStartByOrder(const ScriptItems& scriptItems,
        const ScriptItemsByOrder& scriptItemsByOrder) {}

    /**
     * Inject the JavaScript after WebView load the DOM tree.
     */
    virtual void JavaScriptOnDocumentEndByOrder(const ScriptItems& scriptItems,
        const ScriptItemsByOrder& scriptItemsByOrder) {}

    /**
     * @Description: Check web component active policy disable, default: false
     * @Return: Whether the policy is disable.
     */
    /*--ark web()--*/
    virtual bool IsActivePolicyDisable()
    {
        return false;
    }

    /**
     * @Description: Optimize HTML parser budget to reduce FCP time.
     * @Input enable: Set whether to use optimized parser budget.
     */
    virtual void PutOptimizeParserBudgetEnabled(bool enable) {}

    /**
     * @brief Web drag resize optimize.
     */
    /*--ark web()--*/
    virtual void DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width) = 0;

    /**
     * @Description: Inject the JavaScript when the head element has been created.
     * @Input scriptItems: The injected JavaScript code is stored in lexicographical order.
     * @Input scriptItemsByOrder: The injected JavaScript code is stored in the order of the injection array.
     */
    virtual void JavaScriptOnHeadReadyByOrder(const ScriptItems& scriptItems,
        const ScriptItemsByOrder& scriptItemsByOrder) {}

    /**
     * @Description: Execute an accessibility action on an accessibility node in the browser.
     * @Input accessibilityId: The id of the accessibility node.
     * @Input action: The action to be performed on the accessibility node.
     * @Input actionArguments: Data related to the current action.
     * @Return: Whether the action is performed successfully.
     */
    virtual bool PerformActionV2(int64_t accessibilityId, uint32_t action,
        const std::map<std::string, std::string>& actionArguments) {
        return false;
    }

    /**
     * @Description: Get the bounding rectangle of the accessibility node of the given id.
     * @Input accessibilityId: The id of the accessibility node.
     * @Output width: The width of the rectangle.
     * @Output height: The height of the rectangle.
     * @Output offsetX: The X-coordinate offset of the rectangle.
     * @Output offsetY: The Y-coordinate offset of the rectangle.
     * @Return: Whether the bounding rectangle is obtained successfully.
     */
    virtual bool GetAccessibilityNodeRectById(
        int64_t accessibilityId, int32_t* width, int32_t* height, int32_t* offsetX, int32_t* offsetY)
    {
        return false;
    }

    /** @Description: Get the GPU memory size used by web.
     * @Return: Total size of web GPU.
     */
    virtual float DumpGpuInfo() {
        return 0;
    }

    /**
     * Gets the last hit test result.
     *
     * @return the last HitTestResult
     */
    /*--ark web()--*/
    virtual std::shared_ptr<HitTestResult> GetLastHitTestResult()
    {
        return std::shared_ptr<HitTestResult>();
    }

    /**
     * @brief Web maximize resize optimize.
     */
    /*--ark web()--*/
    virtual void MaximizeResize() {}

    /**
     * @brief: register native javaScriptProxy.
     *
     * @param objName  String: object name.
     * @param methodName std::vector<std::string>: methodName list
     * @param data std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod>: The ptr of NWebJsProxyMethod.
     * @param isAsync bool: True mean.
     * @param permission string: permission.
     */
    virtual void RegisterNativeJavaScriptProxy(const std::string& objName,
        const std::vector<std::string>& methodName,
        std::shared_ptr<OHOS::NWeb::NWebJsProxyMethod> data,
        bool isAsync,
        const std::string& permission) {}

    /**
     * @Description: Get the current language in the webview.
     * @Return: The current language in the webview.
     */
    /*--ark web()--*/
    virtual std::string GetCurrentLanguage() {
        return "";
    }

    /**
     * @brief Send mouse wheel event with sourceTool info.
     */
    virtual bool WebSendMouseWheelEventV2(
        double x, double y, double delta_x, double delta_y, const std::vector<int32_t> &pressedCodes, int32_t source)
    {
        return false;
    }

    /**
     * @brief Try to attach web inputmethod after drag.
     */
    virtual void OnDragAttach() {}

    /**
     * @brief judge if browser use drag resize
     */
    virtual bool IsNWebEx() {
        return false;
    }

    /**
     * Set enable half the frame rate.
     */
    /*--ark web()--*/
    virtual void SetEnableHalfFrameRate(bool enable) {}

    /**
     * Set focus by position
     *
     * @Return: if hit node editable.
     */
    /*--ark web()--*/
    virtual bool SetFocusByPosition(float x, float y)
    {
        return false;
    }

    /**
     * @brief Set the native inner web
     */
    virtual void SetNativeInnerWeb(bool isInnerWeb) {}

    /**
     * @brief Send the accessibility hover event coordinate.
     *
     * @param x horizontal location of coordinate.
     * @param y vertical location of coordinate.
     * @param isHoverEnter whether the accessibility hover event is a hover enter event.
     */
    virtual void SendAccessibilityHoverEventV2(int32_t x, int32_t y, bool isHoverEnter) {}

    /**
     * @brief Notify browser is foreground.
     */
    virtual void OnBrowserForeground() {}

    /**
     * @brief Notify browser is background.
     */
    virtual void OnBrowserBackground() {}
    
    /**
     * @brief Set the window id.
     */
    virtual void SetFocusWindowId(uint32_t focus_window_id) {}

    /**
     * Get select startIndex.
     */
    virtual int GetSelectStartIndex()
    {
        return 0;
    }

    /**
     * Get select endIndex.
     */
    virtual int GetSelectEndIndex()
    {
        return 0;
    }

    /**
     * Get All text info.
     */
    virtual std::string GetAllTextInfo()
    {
        return "";
    }

    /**
     * CallH5FunctionV2
     *
     * @param routing_id       int32_t: the h5 frmae routing id
     * @param h5_object_id     int32_t: the h5 side object id
     * @param h5_method_name   string:  the h5 side object method name
     * @param args             vector<shared_ptr<NWebRomValue>>: the call args
     */
    virtual void CallH5FunctionV2(int32_t routing_id, int32_t h5_object_id, const std::string& h5_method_name,
        const std::vector<std::shared_ptr<NWebRomValue>>& args)
    {}

    /**
     * use the port to send message.
     *
     * @param portHandle the port to send message.
     * @param data the message to send.
     */
    virtual void PostPortMessageV2(const std::string& portHandle, std::shared_ptr<NWebRomValue> data) {}

    /**
     * @brief fill autofill data.
     * @param data data.
     */
    virtual void FillAutofillDataV2(std::shared_ptr<NWebRomValue> data) {}

    /**
     * @brief Run data detector JS.
     */
    virtual void RunDataDetectorJS() {}

    /**
     * @brief Set data detector enable.
     */
    virtual void SetDataDetectorEnable(bool enable) {}

    /**
     * @brief On data detector select text.
     */
    virtual void OnDataDetectorSelectText() {}

    /**
     * @brief On data detector copy.
     */
    virtual void OnDataDetectorCopy(const std::vector<std::string>& recordMix) {}

    /**
     * @brief When the user sets the webpage's border radius,
     *        update Chromium with this radius value for repaint the scrollbar.
     * @param borderRadiusTopLeft: Radius value of the rounded corner in the top-left of the webpage.
     * @param borderRadiusTopRight: Radius value of the rounded corner in the top-right of the webpage.
     * @param borderRadiusBottomLeft: Radius value of the rounded corner in the bottom-left of the webpage.
     * @param borderRadiusBottomRight: Radius value of the rounded corner in the bottom-right of the webpage.
     */
    /*--ark web()--*/
    virtual void SetBorderRadiusFromWeb(double borderRadiusTopLeft, double borderRadiusTopRight,
        double borderRadiusBottomLeft, double borderRadiusBottomRight) {}

    /**
     *  @brief Set the native window of picture in picture.
     */
    virtual void SetPipNativeWindow(int delegate_id,
                                    int child_id,
                                    int frame_routing_id,
                                    void* window) {}

    /**
     * @brief Send event of picture in picture.
     */
    virtual void SendPipEvent(int delegate_id,
                              int child_id,
                              int frame_routing_id,
                              int event) {}

    /**
     * @brief Create web print document adapter V2.
     */
    virtual std::unique_ptr<NWebPrintDocumentAdapterAdapter> CreateWebPrintDocumentAdapterV2(
        const std::string& jobName) {
        return nullptr;
    }

    /*
     * @brief Set unique key of current page for insert frame.
     *
     * @param key string: the unique key of current page.
     */
    virtual void SetBlanklessLoadingKey(const std::string& key) {}

    /**
     * @brief Set privacy status.
     *
     * @param isPrivate bool: privacy status page.
     */
    virtual void SetPrivacyStatus(bool isPrivate) {}
 
    /**
     * Set audio session type.
     *
     * @param audioSessionType Audio session type.
     */
    virtual void SetAudioSessionType(int32_t audioSessionType) {}
    
    /**
     * Get accessibility id by its html element id in the browser.
     * @param htmlElementId The html element id of the Same-layer rendering.
     * @return The accessibility id of the accessibility node with Same-layer rendering.
     */
    virtual int64_t GetWebAccessibilityIdByHtmlElementId(const std::string& htmlElementId)
    {
        return -1;
    }

    /**
     * @brief Get the prediction info of blankless loading on the current page.
     *
     * @param key The unique key of current page.
     * @param similarity The historical snapshot similarity.
     * @param loadingTime The historical loading time.
     * @return The error code.
     */
    virtual int32_t GetBlanklessInfoWithKey(const std::string& key, double* similarity, int32_t* loadingTime)
    {
        return -1;
    }

    /**
     * @brief Set whether to enable blankless loading on the current page.
     *
     * @param key The unique key of current page.
     * @param isStart Whether to enable blankless loading.
     * @return The error code.
     */
    virtual int32_t SetBlanklessLoadingWithKey(const std::string& key, bool isStart)
    {
        return -1;
    }

    /**
     * @brief Try to trigger blankless for url.
     * @param url The url to use for blankless.
     * @return Blankless is triggered for this url.
     */
    virtual bool TriggerBlanklessForUrl(const std::string& url) { return false; }

    /**
     * @brief Set visibility of the web.
     * @param isVisible The visibility to be set.
     */
    virtual void SetVisibility(bool isVisible) {}

    /**
     * @brief Update the single handle visible.
     * @param isVisible The single handle visible.
     */
    virtual void UpdateSingleHandleVisible(bool isVisible) {}

    /**
     * @brief Set the state of touch handle when it exists.
     * @param touchHandleExist The state of the touch handle, Which is true if the touch handle exists.
     */
    virtual void SetTouchHandleExistState(bool touchHandleExist) {}

    /**
     * @brief Get the current scroll offset of the webpage.
     * @param offset_x The current horizontal scroll offset of the webpage.
     * @param offset_y The current vertical scroll offset of the webpage.
     */
    virtual void GetPageOffset(float* offset_x, float* offset_y) {}

    /**
     * @brief Sets the bottom avoidance height of the web visible viewport.
     * @param avoidHeight The height value of the visible viewport avoidance. Unit: px.
     */
    virtual void AvoidVisibleViewportBottom(int32_t avoidHeight) {}

    /**
     * @brief Get the bottom avoidance height of the web visible viewport.
     * @return The bottom avoidance height of the visible viewport.
     */
    virtual int32_t GetVisibleViewportAvoidHeight()
    {
        return 0;
    }

    /**
     * @brief Current viewport is being scaled.
     */
    virtual void SetViewportScaleState() {}

    /**
     * @brief Set whether enable the error page. onOverrideErrorPage will be triggered when the page error.
     *
     * @param enable bool: Whether enable the error page.
     */
    virtual void SetErrorPageEnabled(bool enable) {}

    /**
     * @brief Get whether default error page feature is enabled.
     */
    virtual bool GetErrorPageEnabled() { return false; }

    /**
     * @brief Get web component destroy mode.
     * @return The web component destroy mode.
     */
    /*--ark web()--*/
    virtual WebDestroyMode GetWebDestroyMode()
    {
        return WebDestroyMode::NORMAL_MODE;
    }

    /**
     * @brief Handle stylus touch press event.
     * @param stylus_touch_point_info The stylus touch point information containing comprehensive parameters.
     * @param from_overlay Indicates whether the event comes from an overlay layer.
     */
    virtual void OnStylusTouchPress(
        std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay) {}

    /**
     * @brief Handle stylus touch release event.
     * @param stylus_touch_point_info The stylus touch point information containing comprehensive parameters.
     * @param from_overlay Indicates whether the event comes from an overlay layer.
     */
    virtual void OnStylusTouchRelease(
        std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info, bool from_overlay) {}

    /**
     * @brief Handle batch stylus touch move events.
     * @param stylus_touch_point_infos The vector containing multiple stylus touch points,
     *                                 each is a shared pointer to NWebStylusTouchPointInfo.
     * @param from_overlay Indicates whether the events come from an overlay layer.
     */
    virtual void OnStylusTouchMove(
        const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>& stylus_touch_point_infos, bool from_overlay) {}

    /**
     * @brief Record the blankless frame size of the web.
     * @param width The width of the blankless frame.
     * @param height The height of the blankless frame.
     */
    virtual void RecordBlanklessFrameSize(uint32_t width, uint32_t height) {}

    /**
     * @brief Prefetch the resources required by the page, but will not execute js or
     *        render the page.
     * @param url:  String: Which url to preresolve/preconnect.
     * @param additional_http_headers: Additional HTTP request header of the URL.
     * @param minTimeBetweenPrefetchesMs: djusting the throttling interval.
     * @param ignoreCacheControlNoStore:  true if ignoring Cache-Control: no-store.
     */
    virtual void PrefetchPageV2(
        const std::string& url, const std::map<std::string, std::string>& additionalHttpHeaders,
        int32_t minTimeBetweenPrefetchesMs, bool ignoreCacheControlNoStore) {}

    /**
     * @brief Set web should ignore zoom limit.
     */
    /*--ark web()--*/
    virtual void SetForceEnableZoom(bool forceEnableZoom) const {}

    /**
     * @brief Set the config for blank screen detection.
     *
     * @param enable Whether the blank screen detection is enabled.
     * @param detectionTiming The timing of the blank screen detection.
     * @param detectionMethods The methods of the blank screen detection.
     * @param contentfulNodesCountThreshold The contentful nodes count threshold of the blank screen detection.
     */
    virtual void SetBlankScreenDetectionConfig(bool enable, const std::vector<double>& detectionTiming,
        const std::vector<int32_t>& detectionMethods, int32_t contentfulNodesCountThreshold)
    {}

    /**
     * @brief Get Web page snapshot V2
     *
     * @param id Request id.
     * @param width Request SnapShot width.
     * @param height Request SnapShot height.
     * @param callback SnapShot result callback.
     * @return ture if succuess request snapshot to renderer.
     */
    /*--ark web()--*/
    virtual bool WebPageSnapshotV2(const char* id,
                                   PixelUnit type,
                                   int width,
                                   int height,
                                   std::shared_ptr<NWebSnapshotCallback> callback) {
        return false;
    }

    /**
     * @brief Set web should stop fling.
     */
    /*--ark web()--*/
    virtual void StopFling() {}

    /**
     * Resume current microphone.
     */
    virtual void ResumeMicrophone() {}

    /**
     * Stop current microphone.
     */
    virtual void StopMicrophone() {}

    /**
     * Pause current microphone.
     */
    virtual void PauseMicrophone() {}

    /**
     * @brief Inject the JavaScript before WebView load the DOM tree.
     *
     * @param scriptItems: Multiple injected JavaScript codes are stored in a map in lexicographical order.
     * @param scriptRegexItems: Multiple injected regular expression rule codes are
     *                          stored in a map in lexicographical order.
     * @param scriptItemsByOrder: Multiple injected JavaScript codes are stored in the order of injection.
     */
    virtual void JavaScriptOnDocumentStartByOrderV2(const ScriptItems& scriptItems,
        const ScriptRegexItems& scriptRegexItems, const ScriptItemsByOrder& scriptItemsByOrder) {}

    /**
     * @brief Inject the JavaScript after WebView load the DOM tree.
     *
     * @param scriptItems: Multiple injected JavaScript codes are stored in a map in lexicographical order.
     * @param scriptRegexItems: Multiple injected regular expression rule codes are
     *                          stored in a map in lexicographical order.
     * @param scriptItemsByOrder: Multiple injected JavaScript codes are stored in the order of injection.
     */
    virtual void JavaScriptOnDocumentEndByOrderV2(const ScriptItems& scriptItems,
        const ScriptRegexItems& scriptRegexItems, const ScriptItemsByOrder& scriptItemsByOrder) {}

    /**
     * @Description: Inject the JavaScript when the head element has been created.
     * @Input scriptItems: The injected JavaScript code is stored in lexicographical order.
     * @Input scriptRegexItems: Multiple injected regular expression rule codes are
     *                          stored in a map in lexicographical order.
     * @Input scriptItemsByOrder: The injected JavaScript code is stored in the order of the injection array.
     */
    virtual void JavaScriptOnHeadReadyByOrderV2(const ScriptItems& scriptItems,
        const ScriptRegexItems& scriptRegexItems, const ScriptItemsByOrder& scriptItemsByOrder) {}

    /**
     * @brief Put the callback, get plain text from password vault.
     *
     * @param callback get plain text from password vault.
     */
    virtual void PutVaultPlainTextCallback(
        std::shared_ptr<NWebVaultPlainTextCallback> callback) {}
    
    /**
     * @brief fill autofill data.
     *
     * @param data data.
     * @param type type.
     */
    virtual void FillAutofillDataFromTriggerType(
        std::shared_ptr<NWebRomValue> data, const NWebAutoFillTriggerType& type) {}

    /**
     * @brief Set soft keyboard behavior mode.
     *
     * @param mode WebSoftKeyboardBehaviorMode: the soft keyboard behavior mode.
     */
    virtual void SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode mode) {}
    
    /**
     * @brief Get the NWebAgentManager that manages agent related features.
     *
     * @return a shared_ptr to NWebAgentManager. If agent features are not supported, returns nullptr.
     */
    virtual std::shared_ptr<NWebAgentManager> GetAgentManager()
    {
        return nullptr;
    }

    /**
     * @brief Set the NWebAgentHandler that will receive agent related callbacks.
     * This will replace the current handler.
     *
     * @param agentHandler: a shared_ptr to an implementation of NWebAgentHandler.
     */
    virtual void SetNWebAgentHandler(std::shared_ptr<NWebAgentHandler> agentHandler) {};

    /**
     * @brief Sends command to the web kernel.
     *
     * @param action Basic information about msdp command.
     * @return The result of command.
     */
    virtual int32_t SendCommandAction(std::shared_ptr<OHOS::NWeb::NWebCommandAction> action)
    {
        return -1;
    }

    /*
     * @brief Set unique key of current page for insert frame.
     *
     * @param key string: the unique key of current page.
     * @param enable Whether to enable frame interpolation.
     * @param duration The duration time of frame interpolation.
     * @param expirationTime Indicates the time when the historical frame interpolation expires.
     * @param callback Called after the frame insertion successful, failed, or removed.
     */
    virtual int32_t SetBlanklessLoadingParams(const std::string& key, bool enable, int32_t duration,
        int64_t expirationTime, std::shared_ptr<NWebBlanklessCallback> callback)
    {
        return -1;
    }

    /**
     * @brief Sends state to the web kernel to execute blankless callback.
     *
     * @param state Frame insertion successful, failed, or removed.
     * @param reason Reason of frame insertion failed.
     */
    virtual void CallExecuteBlanklessCallback(int32_t state, const std::string& reason) {}

    /**
     * @brief Forces a full reload of the current page, bypassing the browser cache.
     */
    virtual void ReloadIgnoreCache() {};

    /**
     * @brief Sets the User-Agent metadata corresponding to the User-Agent.
     *
     * @param userAgent The User-Agent string.
     * @param metaData The UserAgentMetadata for the userAgent.
     */
    virtual void SetUserAgentMetadata(const std::string& userAgent, std::shared_ptr<NWebUserAgentMetadata> metaData) {}

    /**
     * @brief Get the User-Agent metadata corresponding to the User-Agent.
     *
     * @param userAgent The User-Agent string.
     */
    virtual std::shared_ptr<NWebUserAgentMetadata> GetUserAgentMetadata(const std::string& userAgent)
    {
        return nullptr;
    }

    /**
     * @brief Set url trust list with error message.
     *
     * @param urlTrustList The url Trust list.
     * @param allowOpaqueOrigin If true, loading of opaque origin URLs (e.g., javascript, data) is
     *     allowed. If false, it is not allowed.
     * @param supportWildcard If true, wildcard matching is supported (e.g., *.example.com matches all
     *     subdomains). If false, wildcard matching is not supported.
     * @param detailErrMsg The url trust list detail message.
     */
    virtual int SetUrlTrustListWithErrMsg(const std::string& urlTrustList,
        bool allowOpaqueOrigin, bool supportWildcard, std::string& detailErrMsg) {
        return 0;
    }

    /**
     * @brief Set is offline web Component.
     */
    virtual void SetIsOfflineWebComponent() {}

    /**
     * @brief Send cancel fling event.
     *
     */
    virtual void WebSendCancelFlingEvent() {}
};
} // namespace OHOS::NWeb

#endif
