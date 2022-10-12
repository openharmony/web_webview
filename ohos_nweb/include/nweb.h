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
#include "nweb_export.h"

#include "nweb_download_callback.h"
#include "nweb_find_callback.h"
#include "nweb_javascript_result_callback.h"
#include "nweb_preference.h"
#include "nweb_value_callback.h"
#include "nweb_hit_testresult.h"

namespace OHOS::NWeb {
class NWebHandler;

struct OHOS_NWEB_EXPORT NWebInitArgs {
    std::string dump_path = "";
    bool frame_info_dump = false;
    std::list<std::string> web_engine_args_to_add;
    std::list<std::string> web_engine_args_to_delete;
    bool multi_renderer_process = false;
};

struct OHOS_NWEB_EXPORT NWebCreateInfo {
    /* size info */
    uint32_t width = 0;
    uint32_t height = 0;

    /* output frame cb */
    std::function<bool(const char*, uint32_t, uint32_t)> output_render_frame = nullptr;

    /* init args */
    NWebInitArgs init_args;

    void* producer_surface = nullptr;
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

struct OHOS_NWEB_EXPORT DragEvent {
    double x;
    double y;
    DragAction action;
};

class OHOS_NWEB_EXPORT NWeb : public std::enable_shared_from_this<NWeb> {
public:
    NWeb() = default;
    virtual ~NWeb() = default;

    virtual void Resize(uint32_t width, uint32_t height) = 0;

    /* lifecycle interface */
    virtual void OnPause() const = 0;
    virtual void OnContinue() const = 0;
    virtual void OnDestroy() = 0;

    /* focus event */
    virtual void OnFocus() const = 0;
    virtual void OnBlur() const = 0;

    /* event interface */
    virtual void OnTouchPress(int32_t id, double x, double y) = 0;
    virtual void OnTouchRelease(int32_t id, double x = 0, double y = 0) = 0;
    virtual void OnTouchMove(int32_t id, double x, double y) = 0;
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
    virtual int Load(const std::string& url) const = 0;

    /**
     * Get whether this NWeb has a back history item.
     *
     * @return true if this NWeb has a back history item
     */
    virtual bool IsNavigatebackwardAllowed() const = 0;

    /**
     * Get whether this NWeb has a forward history item.
     *
     * @return true if this NWeb has a forward history item
     */
    virtual bool IsNavigateForwardAllowed() const = 0;

    /**
     * Get whether this NWeb has a back or forward history item for number of
     * steps.
     *
     * @param numSteps int: the negative or positive number of steps to move the
     * history.
     * @return true if this NWeb has a forward history item
     */
    virtual bool CanNavigateBackOrForward(int numSteps) const = 0;

    /**
     * Go back in the history of this NWeb.
     *
     */
    virtual void NavigateBack() const = 0;

    /**
     * Go forward in the history of this NWeb.
     *
     */
    virtual void NavigateForward() const = 0;

    /**
     * Goes to the history item that is the number of steps away from the current item.
     *
     */
    virtual void NavigateBackOrForward(int step) const = 0;

    /**
     * Delete back and forward history list.
     */
    virtual void DeleteNavigateHistory() = 0;

    /**
     * Reload the current URL.
     *
     */
    virtual void Reload() const = 0;

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
    virtual int Zoom(float zoomFactor) const = 0;

    /**
     * Performs a zooming in operation in this NWeb.
     *
     * @return the error id.
     *
     */
    virtual int ZoomIn() const = 0;

    /**
     * Performs a zooming out operation in this NWeb.
     *
     * @return the error id.
     *
     */
    virtual int ZoomOut() const = 0;

    /**
     * Stop the current load.
     *
     * @param code string: javascript code
     */
    virtual void Stop() const = 0;

    /**
     * ExecuteJavaScript
     *
     */
    virtual void ExecuteJavaScript(const std::string& code) const = 0;

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
        std::shared_ptr<NWebValueCallback<std::string>> callback) const = 0;

    /**
     * Get the NWebPreference object used to control the settings for this
     * NWeb.
     *
     * @return a NWebPreference object that can be used to control this NWeb's
     * settings This value cannot be null.
     */
    virtual const std::shared_ptr<NWebPreference> GetPreference() const = 0;

    /**
     * Gets the web id.
     *
     * @return the web id
     */
    virtual unsigned int GetWebId() const = 0;

    /**
     * Get the last hit test result.
     *
     * @return the last HitTestResult
     */
    virtual HitTestResult GetHitTestResult() const = 0;

    /**
     * Set the background color for this view.
     *
     * @param color int: the color of the background
     *
     */
    virtual void PutBackgroundColor(int color) const = 0;

    /**
     * Sets the initla scale for the page.
     *
     * @param scale float: the initla scale of the page.
     *
     */
    virtual void InitialScale(float scale) const = 0;

    /**
     * Set the NWebDownloadCallback that will receive download event.
     * This will replace the current handler.
     *
     * @param downloadListener NWebDownloadCallback.
     */
    virtual void PutDownloadCallback(
        std::shared_ptr<NWebDownloadCallback> downloadListener) = 0;

    /**
     * Set the NWebHandler that will receive various notifications and
     * requests. This will replace the current handler.
     *
     * @param client NWebHandler: an implementation of NWebHandler. This value
     * cannot be null.
     */
    virtual void SetNWebHandler(std::shared_ptr<NWebHandler> handler) = 0;

    /**
     * Get the NWebHandler.
     *
     * @return Gets the NWebHandler.
     */
    virtual const std::shared_ptr<NWebHandler> GetNWebHandler() const = 0;

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
        std::string& url,
        std::map<std::string, std::string> additionalHttpHeaders) = 0;

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
     * @param method_list vector<String>: vector list, method list
     */
    virtual void RegisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list) = 0;

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
    virtual void FindAllAsync(const std::string &searchStr) const = 0;

    /**
     * Clears the highlighting surrounding text matches created by findAllAsync
     */
    virtual void ClearMatches() const = 0;

    /**
     * Highlights and scrolls to the next match found by findAllAsync(String),
     * wrapping around page boundaries as necessary.
     *
     * @param forward bool: find back or forward.
     */
    virtual void FindNext(const bool forward) const = 0;

    /**
     * Saves the current view as a web archive.
     *
     * @param baseName the filename where the archive should be placed This value cannot be null.
     * @param autoName if false, takes basename to be a file. If true, basename is assumed to be
     * a directory in which a filename will be chosen according to the URL of the
     * current page.
     */
    virtual void StoreWebArchive(const std::string &baseName, bool autoName,
        std::shared_ptr<NWebValueCallback<std::string>> callback) const = 0;

    /**
     * create two web message ports.
     * @param ports the web message ports.
     */
    virtual void CreateWebMessagePorts(std::vector<std::string>& ports) = 0;

    /**
     * post messag event to the html main frame.
     * @param message the message
     * @param ports the web message ports.
     * @param uri the uri
     */
    virtual void PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUri) = 0;

    /**
     * close the message port.
     * @param handle the web message port handle.
     */
    virtual void ClosePort(std::string& handle) = 0;

    /**
     * use the port to send message.
     * @param handle the web message port handle.
     * @param data the message send to html5.
     */
    virtual void PostPortMessage(std::string& handle, std::string& data) = 0;

    /**
     * set the callback of th port handle.
     * @param handle the web message port handle.
     * @param callback to receive the message when th other port post message.
     */
    virtual void SetPortMessageCallback(std::string& handle,
        std::shared_ptr<NWebValueCallback<std::string>> callback) = 0;

    /**
     * send drag event to nweb.
     * @param dragEvent the drag event information.
     */
    virtual void SendDragEvent(const DragEvent& dragEvent) const = 0;

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
    virtual std::string GetUrl() const = 0;

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
};
}  // namespace OHOS::NWeb

#endif // NWEB_H
