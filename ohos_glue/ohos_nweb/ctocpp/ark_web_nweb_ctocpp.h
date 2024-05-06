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

#ifndef ARK_WEB_NWEB_CTOCPP_H_
#define ARK_WEB_NWEB_CTOCPP_H_
#pragma once

#include "ohos_nweb/capi/ark_web_nweb_capi.h"
#include "ohos_nweb/include/ark_web_nweb.h"

#include "base/ctocpp/ark_web_ctocpp_ref_counted.h"

namespace OHOS::ArkWeb {

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class ArkWebNWebCToCpp : public ArkWebCToCppRefCounted<ArkWebNWebCToCpp, ArkWebNWeb, ark_web_nweb_t> {
public:
    ArkWebNWebCToCpp();
    virtual ~ArkWebNWebCToCpp();

    // ArkWebNWeb methods.
    void Resize(uint32_t width, uint32_t height, bool is_keyboard) override;

    void OnPause() override;

    void OnContinue() override;

    void OnDestroy() override;

    void OnFocus(const int32_t& focus_reason) override;

    void OnBlur(const int32_t& blur_reason) override;

    void OnTouchPress(int32_t id, double x, double y, bool from_overlay) override;

    void OnTouchRelease(int32_t id, double x, double y, bool from_overlay) override;

    void OnTouchMove(int32_t id, double x, double y, bool from_overlay) override;

    void OnTouchMove(const ArkWebTouchPointInfoVector& touch_point_infos, bool from_overlay) override;

    void OnTouchCancel() override;

    void OnNavigateBack() override;

    bool SendKeyEvent(int32_t key_code, int32_t key_action) override;

    void SendMouseWheelEvent(double x, double y, double delta_x, double delta_y) override;

    void SendMouseEvent(int x, int y, int button, int action, int count) override;

    int Load(const ArkWebString& url) override;

    bool IsNavigatebackwardAllowed() override;

    bool IsNavigateForwardAllowed() override;

    bool CanNavigateBackOrForward(int num_steps) override;

    void NavigateBack() override;

    void NavigateForward() override;

    void NavigateBackOrForward(int step) override;

    void DeleteNavigateHistory() override;

    void Reload() override;

    int Zoom(float zoom_factor) override;

    int ZoomIn() override;

    int ZoomOut() override;

    void Stop() override;

    void ExecuteJavaScript(const ArkWebString& code) override;

    void ExecuteJavaScript(
        const ArkWebString& code, ArkWebRefPtr<ArkWebMessageValueCallback> callback, bool extention) override;

    ArkWebRefPtr<ArkWebPreference> GetPreference() override;

    unsigned int GetWebId() override;

    ArkWebRefPtr<ArkWebHitTestResult> GetHitTestResult() override;

    void PutBackgroundColor(int color) override;

    void InitialScale(float scale) override;

    void PutDownloadCallback(ArkWebRefPtr<ArkWebDownloadCallback> download_listener) override;

    void PutAccessibilityEventCallback(
        ArkWebRefPtr<ArkWebAccessibilityEventCallback> accessibility_event_listener) override;

    void PutAccessibilityIdGenerator(const AccessibilityIdGenerateFunc accessibility_id_generator) override;

    void SetNWebHandler(ArkWebRefPtr<ArkWebHandler> handler) override;

    ArkWebString Title() override;

    int PageLoadProgress() override;

    int ContentHeight() override;

    float Scale() override;

    int Load(const ArkWebString& url, const ArkWebStringMap& additional_http_headers) override;

    int LoadWithDataAndBaseUrl(const ArkWebString& base_url, const ArkWebString& data, const ArkWebString& mime_type,
        const ArkWebString& encoding, const ArkWebString& history_url) override;

    int LoadWithData(const ArkWebString& data, const ArkWebString& mime_type, const ArkWebString& encoding) override;

    void RegisterArkJSfunction(
        const ArkWebString& object_name, const ArkWebStringVector& method_list, const int32_t object_id) override;

    void UnregisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list) override;

    void SetNWebJavaScriptResultCallBack(ArkWebRefPtr<ArkWebJsResultCallback> callback) override;

    void PutFindCallback(ArkWebRefPtr<ArkWebFindCallback> find_listener) override;

    void FindAllAsync(const ArkWebString& search_str) override;

    void ClearMatches() override;

    void FindNext(const bool forward) override;

    void StoreWebArchive(
        const ArkWebString& base_name, bool auto_name, ArkWebRefPtr<ArkWebStringValueCallback> callback) override;

    ArkWebStringVector CreateWebMessagePorts() override;

    void PostWebMessage(
        const ArkWebString& message, const ArkWebStringVector& ports, const ArkWebString& target_uri) override;

    void ClosePort(const ArkWebString& port_handle) override;

    void PostPortMessage(const ArkWebString& port_handle, const ArkWebMessage& data) override;

    void SetPortMessageCallback(
        const ArkWebString& port_handle, ArkWebRefPtr<ArkWebMessageValueCallback> callback) override;

    void SendDragEvent(ArkWebRefPtr<ArkWebDragEvent> drag_event) override;

    void ClearSslCache() override;

    ArkWebString GetUrl() override;

    void ClearClientAuthenticationCache() override;

    void UpdateLocale(const ArkWebString& language, const ArkWebString& region) override;

    const ArkWebString GetOriginalUrl() override;

    bool GetFavicon(const void** data, size_t& width, size_t& height, int& color_type, int& alpha_type) override;

    void PutNetworkAvailable(bool available) override;

    void HasImages(ArkWebRefPtr<ArkWebBoolValueCallback> callback) override;

    void RemoveCache(bool include_disk_files) override;

    ArkWebRefPtr<ArkWebHistoryList> GetHistoryList() override;

    void PutReleaseSurfaceCallback(ArkWebRefPtr<ArkWebReleaseSurfaceCallback> release_surface_listener) override;

    ArkWebUint8Vector SerializeWebState() override;

    bool RestoreWebState(const ArkWebUint8Vector& state) override;

    void PageUp(bool top) override;

    void PageDown(bool bottom) override;

    void ScrollTo(float x, float y) override;

    void ScrollBy(float delta_x, float delta_y) override;

    void SlideScroll(float vx, float vy) override;

    bool GetCertChainDerData(ArkWebStringVector& cert_chain_data, bool is_single_cert) override;

    void SetScreenOffSet(double x, double y) override;

    void SetAudioMuted(bool muted) override;

    void SetShouldFrameSubmissionBeforeDraw(bool should) override;

    void NotifyPopupWindowResult(bool result) override;

    void SetAudioResumeInterval(int32_t resume_interval) override;

    void SetAudioExclusive(bool audio_exclusive) override;

    void RegisterScreenLockFunction(int32_t window_id, ArkWebRefPtr<ArkWebScreenLockCallback> callback) override;

    void UnRegisterScreenLockFunction(int32_t window_id) override;

    void NotifyMemoryLevel(int32_t level) override;

    void OnWebviewHide() override;

    void OnWebviewShow() override;

    ArkWebRefPtr<ArkWebDragData> GetOrCreateDragData() override;

    void PrefetchPage(const ArkWebString& url, const ArkWebStringMap& additional_http_headers) override;

    void SetWindowId(uint32_t window_id) override;

    void OnOccluded() override;

    void OnUnoccluded() override;

    void SetToken(void* token) override;

    void SetNestedScrollMode(const int32_t& nested_scroll_mode) override;

    void SetEnableLowerFrameRate(bool enabled) override;

    void SetVirtualKeyBoardArg(int32_t width, int32_t height, double keyboard) override;

    bool ShouldVirtualKeyboardOverlay() override;

    void SetDrawRect(int32_t x, int32_t y, int32_t width, int32_t height) override;

    void SetDrawMode(int32_t mode) override;

    void* CreateWebPrintDocumentAdapter(const ArkWebString& job_name) override;

    int PostUrl(const ArkWebString& url, const ArkWebCharVector& post_data) override;

    void JavaScriptOnDocumentStart(const ArkWebStringVectorMap& script_items) override;

    void ExecuteAction(int64_t accessibility_id, uint32_t action) override;

    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> GetFocusedAccessibilityNodeInfo(
        int64_t accessibility_id, bool is_accessibility_focus) override;

    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> GetAccessibilityNodeInfoById(int64_t accessibility_id) override;

    ArkWebRefPtr<ArkWebAccessibilityNodeInfo> GetAccessibilityNodeInfoByFocusMove(
        int64_t accessibility_id, int32_t direction) override;

    void SetAccessibilityState(bool state) override;

    bool NeedSoftKeyboard() override;

    bool Discard() override;

    bool Restore() override;

    int GetSecurityLevel() override;

    void CallH5Function(int32_t routingId, int32_t h5ObjectId, const ArkWebString& h5MethodName,
        const ArkWebValueVector& args) override;

    bool IsIncognitoMode() override;

    void RegisterNativeArkJSFunction(const char* objName, const ArkWebJsProxyCallbackVector& callbacks) override;

    void UnRegisterNativeArkJSFunction(const char* objName) override;

    void RegisterNativeValideCallback(const char* webName, const NativeArkWebOnValidCallback callback) override;

    void RegisterNativeDestroyCallback(const char* webName, const NativeArkWebOnValidCallback callback) override;

    void JavaScriptOnDocumentEnd(const ArkWebStringVectorMap& script_items) override;

    void EnableSafeBrowsing(bool enable) override;

    bool IsSafeBrowsingEnabled() override;

    void SetPrintBackground(bool enable) override;

    bool GetPrintBackground() override;

    void CloseAllMediaPresentations() override;

    void StopAllMedia() override;

    void ResumeAllMedia() override;

    void PauseAllMedia() override;

    int GetMediaPlaybackState() override;

    void EnableIntelligentTrackingPrevention(bool enable) override;

    bool IsIntelligentTrackingPreventionEnabled() override;

    void StartCamera() override;

    void StopCamera() override;

    void CloseCamera() override;

    ArkWebString GetLastJavascriptProxyCallingFrameUrl() override;

    bool GetPendingSizeStatus() override;

    void ScrollByRefScreen(float delta_x, float delta_y, float vx, float vy) override;

    void ExecuteJavaScriptExt(const int fd, const size_t scriptLength,
        ArkWebRefPtr<ArkWebMessageValueCallback> callback, bool extention) override;

    void OnRenderToBackground() override;

    void OnRenderToForeground() override;

    void PrecompileJavaScript(const ArkWebString& url, const ArkWebString& script,
        ArkWebRefPtr<ArkWebCacheOptions>& cacheOptions, ArkWebRefPtr<ArkWebMessageValueCallback> callback) override;

    void OnCreateNativeMediaPlayer(ArkWebRefPtr<ArkWebCreateNativeMediaPlayerCallback> callback) override;

    void DragResize(uint32_t width, uint32_t height, uint32_t pre_height, uint32_t pre_width) override;

    void OnTouchCancelById(int32_t id, double x, double y, bool from_overlay) override;

    int ScaleGestureChange(double scale, double centerX, double centerY) override;

    void InjectOfflineResource(const ArkWebString& url, const ArkWebString& origin, const ArkWebUint8Vector& resource,
        const ArkWebStringMap& responseHeaders, const int type) override;

    bool TerminateRenderProcess() override;

    void SuggestionSelected(int32_t index) override;

    void RegisterArkJSfunction(const ArkWebString& object_name, const ArkWebStringVector& method_list,
        const ArkWebStringVector& async_method_list, const int32_t object_id) override;

    void SendTouchpadFlingEvent(double x, double y, double vx, double vy) override;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NWEB_CTOCPP_H_
