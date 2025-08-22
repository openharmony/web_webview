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

#ifndef NWEB_CREAT_WINDOW_H
#define NWEB_CREAT_WINDOW_H

#include "nweb.h"
#include "nweb_init_params.h"

#define private public
#include "ui/rs_surface_node.h"
#undef private

namespace OHOS::NWeb {
std::shared_ptr<NWebEngineInitArgsImpl> GetInitArgs(void);
std::shared_ptr<NWeb> GetNwebForTest();

class NWebMock : public NWeb {
public:
    void Resize(uint32_t width, uint32_t height, bool isKeyboard = false) override
    {}
    void OnPause() override
    {}
    void OnContinue() override
    {}
    void OnDestroy() override
    {}
    void OnFocus(const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) override
    {}
    void OnBlur(const BlurReason& blurReason) override
    {}
    void OnTouchPress(int32_t id, double x, double y, bool fromOverlay = false) override
    {}
    void OnTouchRelease(int32_t id, double x = 0, double y = 0, bool fromOverlay = false) override
    {}
    void OnTouchMove(int32_t id, double x, double y, bool fromOverlay = false) override
    {}
    void OnTouchMove(const std::vector<std::shared_ptr<NWebTouchPointInfo>> &touch_point_infos,
                     bool fromOverlay = false) override
    {}
    void OnTouchCancel() override
    {}
    void OnStylusTouchPress(
        std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info, 
        bool fromOverlay = false) override
    {}
    void OnStylusTouchRelease(
        std::shared_ptr<NWebStylusTouchPointInfo> stylus_touch_point_info, 
        bool fromOverlay = false) override
    {}
    void OnStylusTouchMove(
        const std::vector<std::shared_ptr<NWebStylusTouchPointInfo>>& stylus_touch_point_infos, 
        bool fromOverlay = false) override
    {}
    void OnNavigateBack() override
    {}
    bool SendKeyEvent(int32_t keyCode, int32_t keyAction) override
    {
        return true;
    }
    void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY) override
    {}
    void SendMouseEvent(int x, int y, int button, int action, int count) override
    {}
    int Load(const std::string& url) override
    {
        return 0;
    }
    bool IsNavigatebackwardAllowed() override
    {
        return true;
    }
    bool IsNavigateForwardAllowed() override
    {
        return true;
    }
    bool CanNavigateBackOrForward(int numSteps) override
    {
        return true;
    }
    void NavigateBack() override
    {}
    void NavigateForward() override
    {}
    void NavigateBackOrForward(int step) override
    {}
    void DeleteNavigateHistory() override
    {}
    void Reload() override
    {}
    int Zoom(float zoomFactor) override
    {
        return 0;
    }
    int ZoomIn() override
    {
        return 0;
    }
    int ZoomOut() override
    {
        return 0;
    }
    void Stop() override
    {}
    void ExecuteJavaScript(const std::string& code) override
    {}
    void ExecuteJavaScript(
        const std::string& code,
        std::shared_ptr<NWebMessageValueCallback> callback,
        bool extention) override
    {}
    std::shared_ptr<NWebPreference> GetPreference() override
    {
        return nullptr;
    }
    unsigned int GetWebId() override
    {
        return 0;
    }
    std::shared_ptr<HitTestResult> GetHitTestResult() override
    {
        std::shared_ptr<HitTestResult> test;
        return test;
    }
    void PutBackgroundColor(int color) override
    {}
    void InitialScale(float scale) override
    {}
    void PutDownloadCallback(
        std::shared_ptr<NWebDownloadCallback> downloadListener) override
    {}
    void PutReleaseSurfaceCallback(
        std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) override
    {}
    void SetNWebHandler(std::shared_ptr<NWebHandler> handler) override
    {}
    std::string Title() override
    {
        return nullptr;
    }
    int PageLoadProgress() override
    {
        return 0;
    }
    int ContentHeight() override
    {
        return 0;
    }
    float Scale() override
    {
        return 0;
    }
    int Load(
        const std::string& url,
        const std::map<std::string, std::string> &additionalHttpHeaders) override
    {
        return 0;
    }
    int LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                        const std::string& data,
                                        const std::string& mimeType,
                                        const std::string& encoding,
                                        const std::string& historyUrl) override
    {
        return 0;
    }
    int LoadWithData(const std::string& data,
                              const std::string& mimeType,
                              const std::string& encoding) override
    {
        return 0;
    }
    void RegisterArkJSfunction(
        const std::string& object_name, const std::vector<std::string>& method_list, const int32_t object_id) override
    {}
    void UnregisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list) override
    {}
    void SetNWebJavaScriptResultCallBack(
        std::shared_ptr<NWebJavaScriptResultCallBack> callback) override
    {}
    void PutFindCallback(
        std::shared_ptr<NWebFindCallback> findListener) override
    {}
    void FindAllAsync(const std::string &searchStr) override
    {}
    void ClearMatches() override
    {}
    void FindNext(const bool forward) override
    {}
    void StoreWebArchive(const std::string &baseName, bool autoName,
        std::shared_ptr<NWebStringValueCallback> callback) override
    {}
    std::vector<std::string> CreateWebMessagePorts() override
    {
        std::vector<std::string> empty;
        return empty;
    }
    void PostWebMessage(const std::string& message, const std::vector<std::string>& ports, const std::string& targetUri)
        override
    {}
    void ClosePort(const std::string& handle) override
    {}
    void PostPortMessage(const std::string& handle, std::shared_ptr<NWebMessage> data) override
    {}
    void SetPortMessageCallback(const std::string& handle,
        std::shared_ptr<NWebMessageValueCallback> callback) override
    {}
    void SendDragEvent(std::shared_ptr<NWebDragEvent> dragEvent) override
    {}
    void ClearSslCache() override
    {}
    std::string GetUrl() override
    {
        return "/data";
    }
    void ClearClientAuthenticationCache() override
    {}
    void UpdateLocale(const std::string& language, const std::string& region) override
    {}

    const std::string GetOriginalUrl() override
    {
        return "";
    }
    bool GetFavicon(const void** data, size_t& width, size_t& height,
        ImageColorType& colorType, ImageAlphaType& alphaType) override
    {
        return false;
    }
    void PutNetworkAvailable(bool available) override
    {}

    void HasImages(std::shared_ptr<NWebBoolValueCallback> callback) override
    {}

    void RemoveCache(bool include_disk_files) override
    {}
    std::shared_ptr<NWebHistoryList> GetHistoryList() override
    {
        return nullptr;
    }
    std::vector<uint8_t> SerializeWebState() override
    {
        std::vector<uint8_t> empty;
        return empty;
    }
    bool RestoreWebState(const std::vector<uint8_t> &state) override
    {
        return false;
    }
    void PageUp(bool top) override
    {}
    void PageDown(bool bottom) override
    {}
    void ScrollTo(float x, float y) override
    {}
    void ScrollBy(float deltaX, float deltaY) override
    {}
    void SlideScroll(float vx, float vy) override
    {}
    bool GetCertChainDerData(std::vector<std::string>& certChainData, bool isSingleCert) override
    {
        return false;
    }
    void SetScreenOffSet(double x, double y) override
    {}
    void SetAudioMuted(bool muted) override
    {}
    void SetShouldFrameSubmissionBeforeDraw(bool should) override
    {}
    void NotifyPopupWindowResult(bool result) override
    {}
    void SetAudioResumeInterval(int32_t resumeInterval) override
    {}
    void SetAudioExclusive(bool audioExclusive) override
    {}
    void RegisterScreenLockFunction(int32_t windowId, std::shared_ptr<NWebScreenLockCallback> callback) override
    {}
    void UnRegisterScreenLockFunction(int32_t windowId) override
    {}
    std::shared_ptr<NWebDragData> GetOrCreateDragData() override
    {
        return nullptr;
    }
    void SendTouchpadFlingEvent(double x, double y, double vx, double vy) override
    {}
    int SetUrlTrustList(const std::string& urlTrustList) override
    {
        return 0;
    }
    int SetUrlTrustListWithErrMsg(const std::string& urlTrustList, std::string& detailErrMsg) override
    {
        return 0;
    }
    bool SetFocusByPosition(float x, float y) override
    {
        return false;
    }
    void SetAudioSessionType(int32_t audioSessionType) override
    {}
    void UpdateSingleHandleVisible(bool isVisible) override
    {}
    void AvoidVisibleViewportBottom(int32_t avoidHeight) override
    {}
    int32_t GetVisibleViewportAvoidHeight() override
    {
        return 0;
    }
};
}
#endif
