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
#include <string>
#include <vector>

#include "nweb.h"
#include "nweb_cookie_manager.h"
#include "nweb_data_base.h"
#include "nweb_engine.h"
#include "nweb_export.h"
#include "nweb_web_storage.h"

namespace OHOS::NWeb {
struct FrameRateSetting {
    int32_t min_ { 0 };
    int32_t max_ { 0 };
    int32_t preferredFrameRate_ { 0 };
};

class OHOS_NWEB_EXPORT NWebHelper {
public:
    static NWebHelper& Instance();
    ~NWebHelper() = default;
    bool Init(bool from_ark = true);
    bool InitAndRun(bool from_ark = true);
    bool LoadWebEngine(bool fromArk, bool runFlag);
    void* LoadFuncSymbol(const char* funcName);
    static void TryPreReadLib(bool isFirstTimeStartUpWeb, const std::string& bundlePath);

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
    std::string GetDefaultUserAgent();

    void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
        const uint32_t& cache_valid_time);

    void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list);

    void SetRenderProcessMode(RenderProcessMode mode);
    RenderProcessMode GetRenderProcessMode();

    void SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime);
    void ClearHostIP(const std::string& hostName);

    void WarmupServiceWorker(const std::string& url);

    void SetWholeWebDrawing();

    std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager();

    void EnableBackForwardCache(bool enableNativeEmbed, bool enableMediaTakeOver);

    void TrimMemoryByPressureLevel(int32_t memoryLevel);

private:
    NWebHelper() = default;
    bool GetWebEngine(bool fromArk);
    bool InitWebEngine();

private:
    bool initFlag_ = false;
    std::string bundlePath_;
    std::string customSchemeCmdLine_;
    std::shared_ptr<NWebEngine> nwebEngine_ = nullptr;
    std::vector<std::string> backForwardCacheCmdLine_;
};
} // namespace OHOS::NWeb

#endif // NWEB_HELPER_H
