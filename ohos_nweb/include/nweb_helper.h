/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_HELPER_H
#define NWEB_HELPER_H

#include <iosfwd>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "nweb.h"
#include "nweb_cookie_manager.h"
#include "nweb_data_base.h"
#include "nweb_engine.h"
#include "nweb_export.h"
#include "nweb_web_storage.h"
#include "nweb_proxy_changed_callback.h"
#include "application_state_change_callback.h"

namespace OHOS::NWeb {
struct NwebScheme {
    const std::string name;
    int32_t option = 0;
};

struct FrameRateSetting {
    int32_t min_ { 0 };
    int32_t max_ { 0 };
    int32_t preferredFrameRate_ { 0 };
};

class WebApplicationStateChangeCallback : public AbilityRuntime::ApplicationStateChangeCallback {
public:
    static std::shared_ptr<WebApplicationStateChangeCallback> GetInstance()
    {
        static std::shared_ptr<WebApplicationStateChangeCallback> instance(
        new WebApplicationStateChangeCallback,
        [](WebApplicationStateChangeCallback*) {}
        );
        return instance;
    }
    WebApplicationStateChangeCallback(const WebApplicationStateChangeCallback&) = delete;
    WebApplicationStateChangeCallback& operator=(const WebApplicationStateChangeCallback&) = delete;
    ~WebApplicationStateChangeCallback() = default;
    void NotifyApplicationForeground() override;
    void NotifyApplicationBackground() override;
    std::shared_ptr<NWeb> nweb_ = nullptr;
    bool isRegistered = false;
private:
    WebApplicationStateChangeCallback() = default;
};

class OHOS_NWEB_EXPORT NWebHelper {
public:
    static NWebHelper& Instance();
    ~NWebHelper() = default;
    bool Init(bool from_ark = true);
    bool InitAndRun(bool from_ark = true);
    bool LoadWebEngine(bool fromArk, bool runFlag);
    void* LoadFuncSymbol(const char* funcName);

    std::shared_ptr<NWeb> CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info);
    std::shared_ptr<NWebCookieManager> GetCookieManager();
    std::shared_ptr<NWebDataBase> GetDataBase();
    std::shared_ptr<NWebWebStorage> GetWebStorage();
    std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id);
    void SetBundlePath(const std::string& path);
    void SetHttpDns(std::shared_ptr<NWebDOHConfig> config);
    void SetWebTag(int32_t nwebId, const char* webTag);
    void PrepareForPageLoad(std::string url, bool preconnectable, int32_t numSockets);
    bool LoadNWebSDK();
    void SetConnectionTimeout(const int32_t& timeout);
    void SetCustomSchemeCmdLine(const std::string& cmd)
    {
        customSchemeCmdLine_ = cmd;
    }
    void PauseAllTimers();
    void ResumeAllTimers();

    void AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts);
    void RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts);
    void ClearIntelligentTrackingPreventionBypassingList();

    void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
        const uint32_t& cache_valid_time);

    std::string GetDefaultUserAgent();

    void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list);

    void SetRenderProcessMode(RenderProcessMode mode);
    RenderProcessMode GetRenderProcessMode();

    void SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime);
    void ClearHostIP(const std::string& hostName);

    void SetAppCustomUserAgent(const std::string& userAgent);

    void SetUserAgentForHosts(const std::string& userAgent, const std::vector<std::string>& hosts);

    void WarmupServiceWorker(const std::string& url);

    void EnableWholeWebPageDrawing();

    std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager();

    void EnableBackForwardCache(bool enableNativeEmbed, bool enableMediaTakeOver);

    void TrimMemoryByPressureLevel(int32_t memoryLevel);

    void RemoveAllCache(bool includeDiskFiles);

    void SetProxyOverride(const std::vector<std::string>& proxyUrls,
                          const std::vector<std::string>& proxySchemeFilters,
                          const std::vector<std::string>& bypassRules,
                          const bool& reverseBypass,
                          std::shared_ptr<NWebProxyChangedCallback> callback);
 
    void RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback);

    void SetWebDebuggingAccess(bool isEnableDebug);
    void SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port);

    bool HasLoadWebEngine();

    void SaveSchemeVector(const char* name, int32_t option);

    bool RegisterCustomSchemes();

    void SetBlanklessLoadingCacheCapacity(int32_t capacity);

    void ClearBlanklessLoadingCache(const std::vector<std::string>& urls);

    std::string CheckBlankOptEnable(const std::string& url, int32_t nweb_id);

    void EnablePrivateNetworkAccess(bool enable);

    bool IsPrivateNetworkAccessEnabled();

    void SetWebDestroyMode(WebDestroyMode mode);

    void SetAutoPreconnect(bool enable);

    bool IsAutoPreconnectEnabled();

private:
    NWebHelper() = default;
    bool GetWebEngine(bool fromArk);
    bool InitWebEngine();

private:
    int coreApiLevel_ = 0;
    bool initFlag_ = false;
    bool autoPreconnectEnabled_ = true;
    std::string bundlePath_;
    std::string customSchemeCmdLine_;
    std::shared_ptr<NWebEngine> nwebEngine_ = nullptr;
    std::vector<NwebScheme> schemeVector_;
    std::vector<std::string> backForwardCacheCmdLine_;
    std::shared_ptr<WebApplicationStateChangeCallback> webApplicationStateCallback_;
    mutable std::mutex lock_;
};
} // namespace OHOS::NWeb

#endif // NWEB_HELPER_H
