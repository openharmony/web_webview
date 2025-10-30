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

#include "ohos_nweb/bridge/ark_web_engine_wrapper.h"

#include "ohos_nweb/bridge/ark_web_adsblock_manager_wrapper.h"
#include "ohos_nweb/bridge/ark_web_cookie_manager_wrapper.h"
#include "ohos_nweb/bridge/ark_web_data_base_wrapper.h"
#include "ohos_nweb/bridge/ark_web_download_manager_wrapper.h"
#include "ohos_nweb/bridge/ark_web_engine_init_args_impl.h"
#include "ohos_nweb/bridge/ark_web_engine_prefetch_args_impl.h"
#include "ohos_nweb/bridge/ark_web_nweb_create_info_impl.h"
#include "ohos_nweb/bridge/ark_web_nweb_wrapper.h"
#include "ohos_nweb/bridge/ark_web_web_storage_wrapper.h"
#include "ohos_nweb/bridge/ark_web_proxy_changed_callback_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"
#include "base/include/ark_web_errno.h"

namespace OHOS::NWeb {

std::shared_ptr<NWebEngine> NWebEngine::GetInstance()
{
    ArkWebRefPtr<OHOS::ArkWeb::ArkWebEngine> ark_web_engine = OHOS::ArkWeb::ArkWebEngine::GetInstance();
    if (CHECK_REF_PTR_IS_NULL(ark_web_engine)) {
        return nullptr;
    }

    return std::make_shared<OHOS::ArkWeb::ArkWebEngineWrapper>(ark_web_engine);
}

} // namespace OHOS::NWeb

namespace OHOS::ArkWeb {

ArkWebEngineWrapper::ArkWebEngineWrapper(ArkWebRefPtr<ArkWebEngine> ark_web_engine) : ark_web_engine_(ark_web_engine) {}

std::shared_ptr<OHOS::NWeb::NWeb> ArkWebEngineWrapper::CreateNWeb(
    std::shared_ptr<OHOS::NWeb::NWebCreateInfo> create_info)
{
    ArkWebRefPtr<ArkWebNWebCreateInfo> ark_web_create_info = nullptr;
    if (!CHECK_SHARED_PTR_IS_NULL(create_info)) {
        ark_web_create_info = new ArkWebNWebCreateInfoImpl(create_info);
    }

    ArkWebRefPtr<ArkWebNWeb> ark_web_nweb = ark_web_engine_->CreateNWeb(ark_web_create_info);
    if (CHECK_REF_PTR_IS_NULL(ark_web_nweb)) {
        return nullptr;
    }

    return std::make_shared<ArkWebNWebWrapper>(ark_web_nweb);
}

std::shared_ptr<OHOS::NWeb::NWeb> ArkWebEngineWrapper::GetNWeb(int32_t nweb_id)
{
    ArkWebRefPtr<ArkWebNWeb> ark_web_nweb = ark_web_engine_->GetNWeb(nweb_id);
    if (CHECK_REF_PTR_IS_NULL(ark_web_nweb)) {
        return nullptr;
    }

    return std::make_shared<ArkWebNWebWrapper>(ark_web_nweb);
}

std::shared_ptr<OHOS::NWeb::NWebDataBase> ArkWebEngineWrapper::GetDataBase()
{
    ArkWebRefPtr<ArkWebDataBase> ark_web_data_base = ark_web_engine_->GetDataBase();
    if (CHECK_REF_PTR_IS_NULL(ark_web_data_base)) {
        return nullptr;
    }

    return std::make_shared<ArkWebDataBaseWrapper>(ark_web_data_base);
}

std::shared_ptr<OHOS::NWeb::NWebWebStorage> ArkWebEngineWrapper::GetWebStorage()
{
    ArkWebRefPtr<ArkWebWebStorage> ark_web_web_storage = ark_web_engine_->GetWebStorage();
    if (CHECK_REF_PTR_IS_NULL(ark_web_web_storage)) {
        return nullptr;
    }

    return std::make_shared<ArkWebWebStorageWrapper>(ark_web_web_storage);
}

std::shared_ptr<OHOS::NWeb::NWebCookieManager> ArkWebEngineWrapper::GetCookieManager()
{
    ArkWebRefPtr<ArkWebCookieManager> ark_web_cookie_manager = ark_web_engine_->GetCookieManager();
    if (CHECK_REF_PTR_IS_NULL(ark_web_cookie_manager)) {
        return nullptr;
    }

    return std::make_shared<ArkWebCookieManagerWrapper>(ark_web_cookie_manager);
}

std::shared_ptr<OHOS::NWeb::NWebDownloadManager> ArkWebEngineWrapper::GetDownloadManager()
{
    ArkWebRefPtr<ArkWebDownloadManager> ark_web_download_manager = ark_web_engine_->GetDownloadManager();
    if (CHECK_REF_PTR_IS_NULL(ark_web_download_manager)) {
        return nullptr;
    }

    return std::make_shared<ArkWebDownloadManagerWrapper>(ark_web_download_manager);
}

void ArkWebEngineWrapper::SetWebTag(int32_t nweb_id, const char* web_tag)
{
    ark_web_engine_->SetWebTag(nweb_id, web_tag);
}

void ArkWebEngineWrapper::InitializeWebEngine(std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> init_args)
{
    if (CHECK_SHARED_PTR_IS_NULL(init_args)) {
        ark_web_engine_->InitializeWebEngine(nullptr);
        return;
    }

    ArkWebRefPtr<ArkWebEngineInitArgs> ark_web_engine_init_args = new ArkWebEngineInitArgsImpl(init_args);
    ark_web_engine_->InitializeWebEngine(ark_web_engine_init_args);
}

void ArkWebEngineWrapper::PrepareForPageLoad(const std::string& url, bool preconnectable, int32_t num_sockets)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_engine_->PrepareForPageLoad(stUrl, preconnectable, num_sockets);

    ArkWebStringStructRelease(stUrl);
}

void ArkWebEngineWrapper::SetWebDebuggingAccess(bool isEnableDebug)
{
    ark_web_engine_->SetWebDebuggingAccess(isEnableDebug);
}

void ArkWebEngineWrapper::AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts)
{
    ArkWebStringVector stHosts = ArkWebStringVectorClassToStruct(hosts);

    ark_web_engine_->AddIntelligentTrackingPreventionBypassingList(stHosts);

    ArkWebStringVectorStructRelease(stHosts);
}

void ArkWebEngineWrapper::RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts)
{
    ArkWebStringVector stHosts = ArkWebStringVectorClassToStruct(hosts);

    ark_web_engine_->RemoveIntelligentTrackingPreventionBypassingList(stHosts);

    ArkWebStringVectorStructRelease(stHosts);
}

void ArkWebEngineWrapper::ClearIntelligentTrackingPreventionBypassingList()
{
    ark_web_engine_->ClearIntelligentTrackingPreventionBypassingList();
}

std::string ArkWebEngineWrapper::GetDefaultUserAgent()
{
    ArkWebString stUA = ark_web_engine_->GetDefaultUserAgent();

    std::string objUA = ArkWebStringStructToClass(stUA);
    ArkWebStringStructRelease(stUA);
    return objUA;
}

void ArkWebEngineWrapper::PauseAllTimers()
{
    ark_web_engine_->PauseAllTimers();
}

void ArkWebEngineWrapper::ResumeAllTimers()
{
    ark_web_engine_->ResumeAllTimers();
}

void ArkWebEngineWrapper::PrefetchResource(const std::shared_ptr<OHOS::NWeb::NWebEnginePrefetchArgs>& pre_args,
    const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
    const uint32_t& cache_valid_time)
{
    ArkWebRefPtr<ArkWebEnginePrefetchArgs> ark_web_engine_pre_args = new ArkWebEnginePrefetchArgsImpl(pre_args);
    ArkWebStringMap stHeaders = ArkWebStringMapClassToStruct(additional_http_headers);
    ArkWebString stCacheKey = ArkWebStringClassToStruct(cache_key);
    ark_web_engine_->PrefetchResource(ark_web_engine_pre_args, stHeaders, stCacheKey, cache_valid_time);

    ArkWebStringMapStructRelease(stHeaders);
    ArkWebStringStructRelease(stCacheKey);
}

void ArkWebEngineWrapper::SetRenderProcessMode(ArkWebRenderProcessMode mode)
{
    ark_web_engine_->SetRenderProcessMode(static_cast<int32_t>(mode));
}

ArkWebRenderProcessMode ArkWebEngineWrapper::GetRenderProcessMode()
{
    return static_cast<ArkWebRenderProcessMode>(ark_web_engine_->GetRenderProcessMode());
}

void ArkWebEngineWrapper::ClearPrefetchedResource(const std::vector<std::string>& cache_key_list)
{
    ArkWebStringVector stCacheKeyList = ArkWebStringVectorClassToStruct(cache_key_list);

    ark_web_engine_->ClearPrefetchedResource(stCacheKeyList);

    ArkWebStringVectorStructRelease(stCacheKeyList);
}

void ArkWebEngineWrapper::WarmupServiceWorker(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_engine_->WarmupServiceWorker(stUrl);

    ArkWebStringStructRelease(stUrl);
}

void ArkWebEngineWrapper::SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime)
{
    ark_web_engine_->SetHostIP(ArkWebStringClassToStruct(hostName), ArkWebStringClassToStruct(address), aliveTime);
}

void ArkWebEngineWrapper::ClearHostIP(const std::string& hostName)
{
    ark_web_engine_->ClearHostIP(ArkWebStringClassToStruct(hostName));
}

void ArkWebEngineWrapper::SetAppCustomUserAgent(const std::string& userAgent)
{
    ark_web_engine_->SetAppCustomUserAgent(ArkWebStringClassToStruct(userAgent));
}

void ArkWebEngineWrapper::SetUserAgentForHosts(const std::string& userAgent, const std::vector<std::string>& hosts)
{
    ark_web_engine_->SetUserAgentForHosts(ArkWebStringClassToStruct(userAgent), ArkWebStringVectorClassToStruct(hosts));
}

void ArkWebEngineWrapper::EnableWholeWebPageDrawing()
{
    ark_web_engine_->EnableWholeWebPageDrawing();
}

std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> ArkWebEngineWrapper::GetAdsBlockManager()
{
    ArkWebRefPtr<ArkWebAdsBlockManager> ark_web_adsblock_manager = ark_web_engine_->GetAdsBlockManager();
    if (CHECK_REF_PTR_IS_NULL(ark_web_adsblock_manager)) {
        return nullptr;
    }

    return std::make_shared<ArkWebAdsBlockManagerWrapper>(ark_web_adsblock_manager);
}

void ArkWebEngineWrapper::TrimMemoryByPressureLevel(int32_t memoryLevel)
{
    ark_web_engine_->TrimMemoryByPressureLevel(memoryLevel);
}

void ArkWebEngineWrapper::SetArkWebRomApiLevel(int apiLevel)
{
    ark_web_engine_->SetArkWebRomApiLevel(apiLevel);
}

int ArkWebEngineWrapper::GetArkWebCoreApiLevel()
{
    return ark_web_engine_->GetArkWebCoreApiLevel();
}

void ArkWebEngineWrapper::RemoveAllCache(bool include_disk_files)
{
    ark_web_engine_->RemoveAllCache(include_disk_files);
}

void ArkWebEngineWrapper::SetProxyOverride(
    const std::vector<std::string>& proxyUrls,
    const std::vector<std::string>& proxySchemeFilters,
    const std::vector<std::string>& bypassRules,
    const bool& reverseBypass,
    std::shared_ptr<OHOS::NWeb::NWebProxyChangedCallback> callback)
{
    ArkWebStringVector stProxyRules = ArkWebStringVectorClassToStruct(proxyUrls);
    ArkWebStringVector stProxySchemeFilters = ArkWebStringVectorClassToStruct(proxySchemeFilters);
    ArkWebStringVector stBypassRules = ArkWebStringVectorClassToStruct(bypassRules);
    ArkWebRefPtr<ArkWebProxyChangedCallback> ark_web_proxy_callback = new ArkWebProxyChangedCallbackWrapper(callback);

    ark_web_engine_->SetProxyOverride(stProxyRules, stProxySchemeFilters, stBypassRules,
                                      reverseBypass, ark_web_proxy_callback);

    ArkWebStringVectorStructRelease(stProxyRules);
    ArkWebStringVectorStructRelease(stProxySchemeFilters);
    ArkWebStringVectorStructRelease(stBypassRules);
}

void ArkWebEngineWrapper::RemoveProxyOverride(std::shared_ptr<OHOS::NWeb::NWebProxyChangedCallback> callback)
{
    ArkWebRefPtr<ArkWebProxyChangedCallback> ark_web_proxy_callback = new ArkWebProxyChangedCallbackWrapper(callback);
    ark_web_engine_->RemoveProxyOverride(ark_web_proxy_callback);
}

void ArkWebEngineWrapper::SetWebDebuggingAccessAndPort(
    bool isEnableDebug, int32_t port)
{
    ark_web_engine_->SetWebDebuggingAccessAndPort(isEnableDebug, port);
    if (ArkWebGetErrno() != RESULT_OK) {
        ark_web_engine_->SetWebDebuggingAccess(isEnableDebug);
    }
}

uint32_t ArkWebEngineWrapper::AddBlanklessLoadingUrls(const std::vector<std::string>& urls)
{
    ArkWebStringVector stUrls = ArkWebStringVectorClassToStruct(urls);

    uint32_t addCounts = ark_web_engine_->AddBlanklessLoadingUrls(stUrls);

    ArkWebStringVectorStructRelease(stUrls);
    return addCounts;
}

void ArkWebEngineWrapper::RemoveBlanklessLoadingUrls(const std::vector<std::string>& urls)
{
    ArkWebStringVector stUrls = ArkWebStringVectorClassToStruct(urls);

    ark_web_engine_->RemoveBlanklessLoadingUrls(stUrls);

    ArkWebStringVectorStructRelease(stUrls);
}

void ArkWebEngineWrapper::ClearBlanklessLoadingCache(const std::vector<std::string>& urls)
{
    ArkWebStringVector stUrls = ArkWebStringVectorClassToStruct(urls);

    ark_web_engine_->ClearBlanklessLoadingCache(stUrls);

    ArkWebStringVectorStructRelease(stUrls);
}

std::string ArkWebEngineWrapper::CheckBlankOptEnable(const std::string& url, int32_t nweb_id)
{
    ArkWebString url_ = ArkWebStringClassToStruct(url);
    std::string ret = ArkWebStringStructToClass(ark_web_engine_->CheckBlankOptEnable(url_, nweb_id));
    ArkWebStringStructRelease(url_);
    return ret;
}

void ArkWebEngineWrapper::SetBlanklessLoadingCacheCapacity(int32_t capacity)
{
    ark_web_engine_->SetBlanklessLoadingCacheCapacity(capacity);
}

void ArkWebEngineWrapper::EnablePrivateNetworkAccess(bool enable)
{
    ark_web_engine_->EnablePrivateNetworkAccess(enable);
}

bool ArkWebEngineWrapper::IsPrivateNetworkAccessEnabled()
{
    return ark_web_engine_->IsPrivateNetworkAccessEnabled();
}

void ArkWebEngineWrapper::SetWebDestroyMode(OHOS::NWeb::WebDestroyMode mode)
{
    ark_web_engine_->SetWebDestroyMode(static_cast<int32_t>(mode));
}

int32_t ArkWebEngineWrapper::SetSiteIsolationMode(ArkWebSiteIsolationMode mode)
{
    return static_cast<int32_t>(ark_web_engine_->SetSiteIsolationMode(static_cast<int32_t>(mode)));
}

ArkWebSiteIsolationMode ArkWebEngineWrapper::GetSiteIsolationMode()
{
    int32_t res = ark_web_engine_->GetSiteIsolationMode();
    return static_cast<ArkWebSiteIsolationMode>(res);
}

void ArkWebEngineWrapper::SetSocketIdleTimeout(int32_t timeout)
{
    ark_web_engine_->SetSocketIdleTimeout(timeout);
}

void ArkWebEngineWrapper::SetSoftKeyboardBehaviorMode(OHOS::NWeb::WebSoftKeyboardBehaviorMode mode)
{
    ark_web_engine_->SetSoftKeyboardBehaviorMode(static_cast<int32_t>(mode));
}

void ArkWebEngineWrapper::SetScrollbarMode(OHOS::NWeb::ScrollbarMode mode)
{
    ark_web_engine_->SetScrollbarMode(static_cast<int32_t>(mode));
}
} // namespace OHOS::ArkWeb
