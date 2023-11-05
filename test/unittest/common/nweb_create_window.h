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

#define private public
#include "ui/rs_surface_node.h"
#undef private

namespace OHOS::NWeb {
NWebInitArgs GetInitArgs(void);
std::shared_ptr<NWeb> GetNwebForTest();

class NWebMock : public NWeb {
public:
    void Resize(uint32_t width, uint32_t height, bool isKeyboard = false) override
    {}
    void OnPause() const override
    {}
    void OnContinue() const override
    {}
    void OnDestroy() override
    {}
    void OnFocus(const FocusReason& focusReason = FocusReason::FOCUS_DEFAULT) const override
    {}
    void OnBlur(const BlurReason& blurReason) const override
    {}
    void OnTouchPress(int32_t id, double x, double y, bool fromOverlay = false) override
    {}
    void OnTouchRelease(int32_t id, double x = 0, double y = 0, bool fromOverlay = false) override
    {}
    void OnTouchMove(int32_t id, double x, double y, bool fromOverlay = false) override
    {}
    void OnTouchCancel() override
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
    int Load(const std::string& url) const override
    {
        return 0;
    }
    bool IsNavigatebackwardAllowed() const override
    {
        return true;
    }
    bool IsNavigateForwardAllowed() const override
    {
        return true;
    }
    bool CanNavigateBackOrForward(int numSteps) const override
    {
        return true;
    }
    void NavigateBack() const override
    {}
    void NavigateForward() const override
    {}
    void NavigateBackOrForward(int step) const override
    {}
    void DeleteNavigateHistory() override
    {}
    void Reload() const override
    {}
    int Zoom(float zoomFactor) const override
    {
        return 0;
    }
    int ZoomIn() const override
    {
        return 0;
    }
    int ZoomOut() const override
    {
        return 0;
    }
    void Stop() const override
    {}
    void ExecuteJavaScript(const std::string& code) const override
    {}
    void ExecuteJavaScript(
        const std::string& code,
        std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback,
        bool extention) const override
    {}
    const std::shared_ptr<NWebPreference> GetPreference() const override
    {
        return nullptr;
    }
    unsigned int GetWebId() const override
    {
        return 0;
    }
    HitTestResult GetHitTestResult() const override
    {
        HitTestResult test;
        return test;
    }
    void PutBackgroundColor(int color) const override
    {}
    void InitialScale(float scale) const override
    {}
    void PutDownloadCallback(
        std::shared_ptr<NWebDownloadCallback> downloadListener) override
    {}
    void PutReleaseSurfaceCallback(
        std::shared_ptr<NWebReleaseSurfaceCallback> releaseSurfaceListener) override
    {}
    void SetNWebHandler(std::shared_ptr<NWebHandler> handler) override
    {}
    const std::shared_ptr<NWebHandler> GetNWebHandler() const override
    {
        return nullptr;
    }
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
        std::string& url,
        std::map<std::string, std::string> additionalHttpHeaders) override
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
        const std::string& object_name,
        const std::vector<std::string>& method_list) override
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
    void FindAllAsync(const std::string &searchStr) const override
    {}
    void ClearMatches() const override
    {}
    void FindNext(const bool forward) const override
    {}
    void StoreWebArchive(const std::string &baseName, bool autoName,
        std::shared_ptr<NWebValueCallback<std::string>> callback) const override
    {}
    void CreateWebMessagePorts(std::vector<std::string>& ports) override
    {}
    void PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUri) override
    {}
    void ClosePort(std::string& handle) override
    {}
    void PostPortMessage(std::string& handle, std::shared_ptr<NWebMessage> data) override
    {}
    void SetPortMessageCallback(std::string& handle,
        std::shared_ptr<NWebValueCallback<std::shared_ptr<NWebMessage>>> callback) override
    {}
    void SendDragEvent(const DragEvent& dragEvent) const override
    {}
    void ClearSslCache() override
    {}
    std::string GetUrl() const override
    {
        return "/data";
    }
    void ClearClientAuthenticationCache() override
    {}
    void UpdateLocale(const std::string& language, const std::string& region) override
    {}

    const std::string GetOriginalUrl() const override
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

    void HasImages(std::shared_ptr<NWebValueCallback<bool>> callback) override
    {}

    void RemoveCache(bool include_disk_files) override
    {}
    std::shared_ptr<NWebHistoryList> GetHistoryList() override
    {
        return nullptr;
    }
    WebState SerializeWebState() override
    {
        return nullptr;
    }
    bool RestoreWebState(WebState state) override
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
    void RegisterScreenLockFunction(int32_t windowId, const SetKeepScreenOn&& handle) override
    {}
    void UnRegisterScreenLockFunction(int32_t windowId) override
    {}
    std::shared_ptr<NWebDragData> GetOrCreateDragData() override
    {
        return nullptr;
    }
};
}
#endif
