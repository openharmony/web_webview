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

#ifndef NWEB_HANDLER_IMPL_TEST_H
#define NWEB_HANDLER_IMPL_TEST_H

#include "nweb_handler.h"

namespace OHOS::NWeb {
class NWebHandlerImplTest : public NWebHandler {
public:
    NWebHandlerImplTest() = default;
    ~NWebHandlerImplTest() = default;
    void SetNWeb(std::shared_ptr<NWeb> nweb) override;
    void OnProxyDied() override;
    void OnRouterPush(const std::string& param) override;
    void OnMessage(const std::string& param) override;

    void OnPageLoadEnd(int httpStatusCode, const std::string& url) override;
    void OnPageLoadBegin(const std::string& url) override;
    void OnPageLoadError(int errorCode,
                         const std::string& description,
                         const std::string& failingUrl) override;
    bool OnHandleInterceptUrlLoading(const std::string& url) override;
    const std::vector<std::string> VisitedUrlHistory() override;
    void OnResourceLoadError(std::shared_ptr<NWebUrlResourceRequest> request,
                             std::shared_ptr<NWebUrlResourceError> error) override;
    void OnHttpError(std::shared_ptr<NWebUrlResourceRequest> request,
                     std::shared_ptr<NWebUrlResourceResponse> errorResponse) override;
    void OnPageIcon(const void* data,
                    size_t width,
                    size_t height,
                    ImageColorType colorType,
                    ImageAlphaType alphaType) override;
    void OnDesktopIconUrl(const std::string& iconUrl, bool precomposed) override;
    void OnFocus() override;
    void OnLoadingProgress(int newProgress) override;
    void OnPageTitle(const std::string &title) override;
    void OnResource(const std::string &url) override;
    void OnGeolocationShow(const std::string& origin,
                           std::shared_ptr<NWebGeolocationCallbackInterface> callback) override;
    void OnGeolocationHide() override;
    void OnPermissionRequest(std::shared_ptr<NWebAccessRequest> request) override;
    void OnPermissionRequestCanceled(std::shared_ptr<NWebAccessRequest> request) override;
private:
    std::weak_ptr<NWeb> nwebweak_;
};
} // namespace OHOS::NWeb

#endif // NWEB_HANDLER_IMPL_TEST_H
