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

#ifndef ARK_WEB_ENGINE_WRAPPER_H_
#define ARK_WEB_ENGINE_WRAPPER_H_
#pragma once

#include "include/nweb_engine.h"
#include "ohos_nweb/include/ark_web_engine.h"
#include "ohos_nweb/include/ark_web_proxy_changed_callback.h"

namespace OHOS::ArkWeb {
using ArkWebRenderProcessMode = OHOS::NWeb::RenderProcessMode;

using ArkWebSiteIsolationMode = OHOS::NWeb::SiteIsolationMode;

class ArkWebEngineWrapper : public OHOS::NWeb::NWebEngine {
public:
    ArkWebEngineWrapper(ArkWebRefPtr<ArkWebEngine> ark_web_engine);
    ~ArkWebEngineWrapper() = default;

    std::shared_ptr<OHOS::NWeb::NWeb> CreateNWeb(std::shared_ptr<OHOS::NWeb::NWebCreateInfo> create_info) override;

    std::shared_ptr<OHOS::NWeb::NWeb> GetNWeb(int32_t nweb_id) override;

    std::shared_ptr<OHOS::NWeb::NWebDataBase> GetDataBase() override;

    std::shared_ptr<OHOS::NWeb::NWebWebStorage> GetWebStorage() override;

    std::shared_ptr<OHOS::NWeb::NWebCookieManager> GetCookieManager() override;

    std::shared_ptr<OHOS::NWeb::NWebDownloadManager> GetDownloadManager() override;

    void SetWebTag(int32_t nweb_id, const char* web_tag) override;

    void InitializeWebEngine(std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args) override;

    void PrepareForPageLoad(const std::string& url, bool preconnectable, int32_t num_sockets) override;

    void SetWebDebuggingAccess(bool isEnableDebug) override;

    void AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) override;

    void RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) override;

    void ClearIntelligentTrackingPreventionBypassingList() override;

    std::string GetDefaultUserAgent() override;

    void PauseAllTimers() override;

    void ResumeAllTimers() override;

    void PrefetchResource(const std::shared_ptr<OHOS::NWeb::NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
        const uint32_t& cache_valid_time) override;

    void SetRenderProcessMode(ArkWebRenderProcessMode mode) override;

    ArkWebRenderProcessMode GetRenderProcessMode() override;

    void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list) override;

    void WarmupServiceWorker(const std::string& url) override;

    void SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime) override;

    void ClearHostIP(const std::string& hostName) override;

    void SetAppCustomUserAgent(const std::string& userAgent) override;

    void SetUserAgentForHosts(const std::string& userAgent, const std::vector<std::string>& hosts) override;

    void EnableWholeWebPageDrawing() override;

    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> GetAdsBlockManager() override;

    void TrimMemoryByPressureLevel(int32_t memoryLevel) override;

    void SetArkWebRomApiLevel(int apiLevel) override;

    int GetArkWebCoreApiLevel() override;

    void RemoveAllCache(bool include_disk_files) override;

    void SetProxyOverride(const std::vector<std::string>& proxyUrls,
                          const std::vector<std::string>& proxySchemeFilters,
                          const std::vector<std::string>& bypassRules,
                          const bool& reverseBypass,
                          std::shared_ptr<OHOS::NWeb::NWebProxyChangedCallback> callback) override;

    void RemoveProxyOverride(std::shared_ptr<OHOS::NWeb::NWebProxyChangedCallback> callback) override;

    void SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port) override;

    uint32_t AddBlanklessLoadingUrls(const std::vector<std::string>& urls) override;

    void RemoveBlanklessLoadingUrls(const std::vector<std::string>& urls) override;

    void ClearBlanklessLoadingCache(const std::vector<std::string>& urls) override;

    std::string CheckBlankOptEnable(const std::string& url, int32_t nweb_id) override;

    void SetBlanklessLoadingCacheCapacity(int32_t capacity) override;

    void EnablePrivateNetworkAccess(bool enable) override;

    bool IsPrivateNetworkAccessEnabled() override;

    void SetWebDestroyMode(OHOS::NWeb::WebDestroyMode mode) override;

    int32_t SetSiteIsolationMode(ArkWebSiteIsolationMode mode) override;

    ArkWebSiteIsolationMode GetSiteIsolationMode() override;

    void SetSocketIdleTimeout(int32_t timeout) override;
    
    void SetSoftKeyboardBehaviorMode(OHOS::NWeb::WebSoftKeyboardBehaviorMode mode) override;

    void SetScrollbarMode(OHOS::NWeb::ScrollbarMode mode) override;

    void LibraryLoaded(std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args, bool lazy) override;

private:
    ArkWebRefPtr<ArkWebEngine> ark_web_engine_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ENGINE_WRAPPER_H_
