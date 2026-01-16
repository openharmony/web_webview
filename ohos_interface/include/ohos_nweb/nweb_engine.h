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

#ifndef NWEB_ENGINE_H
#define NWEB_ENGINE_H

#include <memory>

#include "nweb.h"
#include "nweb_adsblock_manager.h"
#include "nweb_cookie_manager.h"
#include "nweb_data_base.h"
#include "nweb_download_manager.h"
#include "nweb_web_storage.h"
#include "nweb_proxy_changed_callback.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebEngine {
public:
    virtual ~NWebEngine() = default;

    static std::shared_ptr<NWebEngine> GetInstance();

    virtual std::shared_ptr<NWeb> CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info) = 0;

    virtual std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id) = 0;

    virtual std::shared_ptr<NWebDataBase> GetDataBase() = 0;

    virtual std::shared_ptr<NWebWebStorage> GetWebStorage() = 0;

    virtual std::shared_ptr<NWebCookieManager> GetCookieManager() = 0;

    virtual std::shared_ptr<NWebDownloadManager> GetDownloadManager() = 0;

    virtual void SetWebTag(int32_t nweb_id, const char* web_tag) = 0;

    virtual void InitializeWebEngine(std::shared_ptr<NWebEngineInitArgs> init_args) = 0;

    virtual void PrepareForPageLoad(const std::string& url, bool preconnectable, int32_t num_sockets) = 0;

    virtual void SetWebDebuggingAccess(bool isEnableDebug) = 0;

    virtual void AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) = 0;

    virtual void RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) = 0;

    virtual void ClearIntelligentTrackingPreventionBypassingList() = 0;

    virtual void PauseAllTimers() = 0;

    virtual void ResumeAllTimers() = 0;

    virtual void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
        const uint32_t& cache_valid_time) = 0;

    virtual void SetRenderProcessMode(RenderProcessMode mode) = 0;

    virtual RenderProcessMode GetRenderProcessMode() = 0;

    virtual int32_t SetSiteIsolationMode(SiteIsolationMode mode) { return 0; }

    virtual SiteIsolationMode GetSiteIsolationMode() { return SiteIsolationMode::PARTIAL; }

    virtual void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list) = 0;

    virtual void WarmupServiceWorker(const std::string& url) = 0;

    virtual void SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime) = 0;

    virtual void ClearHostIP(const std::string& hostName) = 0;

    virtual void EnableWholeWebPageDrawing() {}

    virtual std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager()
    {
        return nullptr;
    }

    virtual void TrimMemoryByPressureLevel(int32_t memoryLevel) {}

    virtual void SetArkWebRomApiLevel(int apiLevel) {}

    virtual int GetArkWebCoreApiLevel()
    {
        return 0;
    }

    virtual std::string GetDefaultUserAgent() {
        return "";
    }

    virtual void RemoveAllCache(bool include_disk_files) {}

    virtual void SetProxyOverride(const std::vector<std::string>& proxyUrls,
                                  const std::vector<std::string>& proxySchemeFilters,
                                  const std::vector<std::string>& bypassRules,
                                  const bool& reverseBypass,
                                  std::shared_ptr<NWebProxyChangedCallback> callback) {}

    virtual void RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback) {}

    virtual void SetAppCustomUserAgent(const std::string& userAgent) {}

    virtual void SetUserAgentForHosts(const std::string& userAgent, const std::vector<std::string>& hosts) {}

    virtual void SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port) {}

    virtual uint32_t AddBlanklessLoadingUrls(const std::vector<std::string>& urls) { return 0; }

    virtual void RemoveBlanklessLoadingUrls(const std::vector<std::string>& urls) {}

    virtual void ClearBlanklessLoadingCache(const std::vector<std::string>& urls) {}

    virtual std::string CheckBlankOptEnable(const std::string& url, int32_t nweb_id) { return ""; }

    virtual void SetBlanklessLoadingCacheCapacity(int32_t capacity) {}

    virtual void EnablePrivateNetworkAccess(bool enable) {}

    virtual bool IsPrivateNetworkAccessEnabled() {
        return false;
    }

    virtual void SetWebDestroyMode(WebDestroyMode mode) {}

    virtual void SetSocketIdleTimeout(int32_t timeout) {}

    virtual void SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode mode) {}

    virtual void SetScrollbarMode(ScrollbarMode mode) {}

    virtual void LibraryLoaded(std::shared_ptr<NWebEngineInitArgs> init_args, bool lazy) {}

    virtual std::string DumpArkWebInfo(const std::string& param) {
        return "";
    }

    virtual void SetUserAgentClientHintsEnabled(bool enabled) {}

    virtual bool GetUserAgentClientHintsEnabled()
    {
        return false;
    }
};

} // namespace OHOS::NWeb

#endif // NWEB_ENGINE_H
