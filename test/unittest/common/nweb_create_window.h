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
#include "window.h"

namespace OHOS::NWeb {
NWebInitArgs GetInitArgs(void);
OHOS::sptr<OHOS::Rosen::Window> CreateWindow(void);

class NWebMock : public NWeb {
public:
    void Resize(uint32_t width, uint32_t height) override {}
    void OnPause() const override {}
    void OnContinue() const override {}
    void OnDestroy() override {}
    void OnFocus() const override {}
    void OnBlur() const override {}
    void OnTouchPress(int32_t id, double x, double y) override {}
    void OnTouchRelease(int32_t id, double x = 0, double y = 0) override {}
    void OnTouchMove(int32_t id, double x, double y) override {}
    void OnTouchCancel() override {}
    void OnNavigateBack() override {}
    bool SendKeyEvent(int32_t keyCode, int32_t keyAction) override
    {
        return true;
    }
    void SendMouseWheelEvent(double x, double y, double deltaX, double deltaY) override {}
    void SendMouseEvent(int x, int y, int button, int action, int count) override {}
    void Load(const std::string& url) const override {}
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
    void NavigateBack() const override {}
    void NavigateForward() const override {}
    void NavigateBackOrForward(int step) const override {}
    void DeleteNavigateHistory() override {}
    void Reload() const override {}
    void Zoom(float zoomFactor) const override {}
    bool ZoomIn() const override
    {
        return true;
    }
    bool ZoomOut() const override
    {
        return true;
    }
    void Stop() const override {}
    void ExecuteJavaScript(const std::string& code) const override {}
    void ExecuteJavaScript(
        const std::string& code,
        std::shared_ptr<NWebValueCallback<std::string>> callback) const override {}
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
    void PutBackgroundColor(int color) const override {}
    void InitialScale(float scale) const override {}
    void PutDownloadCallback(
        std::shared_ptr<NWebDownloadCallback> downloadListener) override {}
    void SetNWebHandler(std::shared_ptr<NWebHandler> handler) override {}
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
    void Load(
        std::string& url,
        std::map<std::string, std::string> additionalHttpHeaders) override {}
    void LoadWithDataAndBaseUrl(const std::string& baseUrl,
                                        const std::string& data,
                                        const std::string& mimeType,
                                        const std::string& encoding,
                                        const std::string& historyUrl) override {}
    void LoadWithData(const std::string& data,
                              const std::string& mimeType,
                              const std::string& encoding) override {}
    void RegisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list) override {}
    void UnregisterArkJSfunction(
        const std::string& object_name,
        const std::vector<std::string>& method_list) override {}
    void SetNWebJavaScriptResultCallBack(
        std::shared_ptr<NWebJavaScriptResultCallBack> callback) override {}
    void PutFindCallback(
        std::shared_ptr<NWebFindCallback> findListener) override {}
    void FindAllAsync(const std::string &searchStr) const override {}
    void ClearMatches() const override {}
    void FindNext(const bool forward) const override {}
    void StoreWebArchive(const std::string &baseName, bool autoName,
        std::shared_ptr<NWebValueCallback<std::string>> callback) const override {}
    void CreateWebMessagePorts(std::vector<std::string>& ports) override {}
    void PostWebMessage(std::string& message, std::vector<std::string>& ports, std::string& targetUri) override {}
    void ClosePort(std::string& handle) override {}
    void PostPortMessage(std::string& handle, std::string& data) override {}
    void SetPortMessageCallback(std::string& handle,
        std::shared_ptr<NWebValueCallback<std::string>> callback) override {}
    void SendDragEvent(const DragEvent& dragEvent) const override {}
    void ClearSslCache() override {}
    std::string GetUrl() const override
    {
        return "/data";
    }
    void ClearClientAuthenticationCache() override {}
};
}
#endif