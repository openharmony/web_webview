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

#include "ohos_nweb/bridge/ark_web_engine_impl.h"

#include "ohos_nweb/bridge/ark_web_adsblock_manager_impl.h"
#include "ohos_nweb/bridge/ark_web_cookie_manager_impl.h"
#include "ohos_nweb/bridge/ark_web_data_base_impl.h"
#include "ohos_nweb/bridge/ark_web_download_manager_impl.h"
#include "ohos_nweb/bridge/ark_web_engine_init_args_wrapper.h"
#include "ohos_nweb/bridge/ark_web_engine_prefetch_args_wrapper.h"
#include "ohos_nweb/bridge/ark_web_nweb_create_info_wrapper.h"
#include "ohos_nweb/bridge/ark_web_nweb_impl.h"
#include "ohos_nweb/bridge/ark_web_web_storage_impl.h"
#include "ohos_nweb/bridge/ark_web_proxy_changed_callback_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {
using ArkWebRenderProcessMode = OHOS::NWeb::RenderProcessMode;

using ArkWebSiteIsolationMode = OHOS::NWeb::SiteIsolationMode;

ArkWebEngineImpl::ArkWebEngineImpl(std::shared_ptr<OHOS::NWeb::NWebEngine> nweb_engine) : nweb_engine_(nweb_engine) {}

ArkWebRefPtr<ArkWebEngine> ArkWebEngine::GetInstance()
{
    std::shared_ptr<OHOS::NWeb::NWebEngine> nweb_engine = OHOS::NWeb::NWebEngine::GetInstance();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_engine)) {
        return nullptr;
    }

    return new ArkWebEngineImpl(nweb_engine);
}

ArkWebRefPtr<ArkWebNWeb> ArkWebEngineImpl::CreateNWeb(ArkWebRefPtr<ArkWebNWebCreateInfo> create_info)
{
    std::shared_ptr<OHOS::NWeb::NWebCreateInfo> nweb_create_info = nullptr;
    if (!CHECK_REF_PTR_IS_NULL(create_info)) {
        nweb_create_info = std::make_shared<ArkWebNWebCreateInfoWrapper>(create_info);
    }

    std::shared_ptr<OHOS::NWeb::NWeb> nweb = nweb_engine_->CreateNWeb(nweb_create_info);
    if (CHECK_SHARED_PTR_IS_NULL(nweb)) {
        return nullptr;
    }

    return new ArkWebNWebImpl(nweb);
}

ArkWebRefPtr<ArkWebNWeb> ArkWebEngineImpl::GetNWeb(int32_t nweb_id)
{
    std::shared_ptr<OHOS::NWeb::NWeb> nweb = nweb_engine_->GetNWeb(nweb_id);
    if (CHECK_SHARED_PTR_IS_NULL(nweb)) {
        return nullptr;
    }

    return new ArkWebNWebImpl(nweb);
}

ArkWebRefPtr<ArkWebDataBase> ArkWebEngineImpl::GetDataBase()
{
    std::shared_ptr<OHOS::NWeb::NWebDataBase> nweb_data_base = nweb_engine_->GetDataBase();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_data_base)) {
        return nullptr;
    }

    return new ArkWebDataBaseImpl(nweb_data_base);
}

ArkWebRefPtr<ArkWebWebStorage> ArkWebEngineImpl::GetWebStorage()
{
    std::shared_ptr<OHOS::NWeb::NWebWebStorage> nweb_web_storage = nweb_engine_->GetWebStorage();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_web_storage)) {
        return nullptr;
    }

    return new ArkWebWebStorageImpl(nweb_web_storage);
}

ArkWebRefPtr<ArkWebCookieManager> ArkWebEngineImpl::GetCookieManager()
{
    std::shared_ptr<OHOS::NWeb::NWebCookieManager> nweb_cookie_manager = nweb_engine_->GetCookieManager();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_cookie_manager)) {
        return nullptr;
    }

    return new ArkWebCookieManagerImpl(nweb_cookie_manager);
}

ArkWebRefPtr<ArkWebDownloadManager> ArkWebEngineImpl::GetDownloadManager()
{
    std::shared_ptr<OHOS::NWeb::NWebDownloadManager> nweb_download_manager = nweb_engine_->GetDownloadManager();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_download_manager)) {
        return nullptr;
    }

    return new ArkWebDownloadManagerImpl(nweb_download_manager);
}

void ArkWebEngineImpl::SetWebTag(int32_t nweb_id, const char* web_tag)
{
    nweb_engine_->SetWebTag(nweb_id, web_tag);
}

void ArkWebEngineImpl::InitializeWebEngine(ArkWebRefPtr<ArkWebEngineInitArgs> init_args)
{
    if (CHECK_REF_PTR_IS_NULL(init_args)) {
        nweb_engine_->InitializeWebEngine(nullptr);
        return;
    }

    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> nweb_engine_init_args =
        std::make_shared<ArkWebEngineInitArgsWrapper>(init_args);
    nweb_engine_->InitializeWebEngine(nweb_engine_init_args);
}

void ArkWebEngineImpl::PrepareForPageLoad(const ArkWebString& url, bool preconnectable, int32_t num_sockets)
{
    nweb_engine_->PrepareForPageLoad(ArkWebStringStructToClass(url), preconnectable, num_sockets);
}

void ArkWebEngineImpl::SetWebDebuggingAccess(bool isEnableDebug)
{
    nweb_engine_->SetWebDebuggingAccess(isEnableDebug);
}

void ArkWebEngineImpl::AddIntelligentTrackingPreventionBypassingList(const ArkWebStringVector& hosts)
{
    nweb_engine_->AddIntelligentTrackingPreventionBypassingList(ArkWebStringVectorStructToClass(hosts));
}

void ArkWebEngineImpl::RemoveIntelligentTrackingPreventionBypassingList(const ArkWebStringVector& hosts)
{
    nweb_engine_->RemoveIntelligentTrackingPreventionBypassingList(ArkWebStringVectorStructToClass(hosts));
}

void ArkWebEngineImpl::ClearIntelligentTrackingPreventionBypassingList()
{
    nweb_engine_->ClearIntelligentTrackingPreventionBypassingList();
}

ArkWebString ArkWebEngineImpl::GetDefaultUserAgent()
{
    return ArkWebStringClassToStruct(nweb_engine_->GetDefaultUserAgent());
}

void ArkWebEngineImpl::PauseAllTimers()
{
    nweb_engine_->PauseAllTimers();
}

void ArkWebEngineImpl::ResumeAllTimers()
{
    nweb_engine_->ResumeAllTimers();
}

void ArkWebEngineImpl::PrefetchResource(ArkWebRefPtr<ArkWebEnginePrefetchArgs>& pre_args,
    const ArkWebStringMap& additional_http_headers, const ArkWebString& cache_key, const uint32_t& cache_valid_time)
{
    std::shared_ptr<OHOS::NWeb::NWebEnginePrefetchArgs> nweb_engine_pre_args =
        std::make_shared<ArkWebEnginePrefetchArgsWrapper>(pre_args);
    nweb_engine_->PrefetchResource(nweb_engine_pre_args, ArkWebStringMapStructToClass(additional_http_headers),
        ArkWebStringStructToClass(cache_key), cache_valid_time);
}

void ArkWebEngineImpl::SetRenderProcessMode(int32_t mode)
{
    nweb_engine_->SetRenderProcessMode(static_cast<ArkWebRenderProcessMode>(mode));
}

int32_t ArkWebEngineImpl::GetRenderProcessMode()
{
    return static_cast<int32_t>(nweb_engine_->GetRenderProcessMode());
}

void ArkWebEngineImpl::ClearPrefetchedResource(const ArkWebStringVector& cache_key_list)
{
    nweb_engine_->ClearPrefetchedResource(ArkWebStringVectorStructToClass(cache_key_list));
}

void ArkWebEngineImpl::WarmupServiceWorker(const ArkWebString& url)
{
    nweb_engine_->WarmupServiceWorker(ArkWebStringStructToClass(url));
}

void ArkWebEngineImpl::SetHostIP(const ArkWebString& hostName, const ArkWebString& address, int32_t aliveTime)
{
    nweb_engine_->SetHostIP(ArkWebStringStructToClass(hostName), ArkWebStringStructToClass(address), aliveTime);
}

void ArkWebEngineImpl::ClearHostIP(const ArkWebString& hostName)
{
    nweb_engine_->ClearHostIP(ArkWebStringStructToClass(hostName));
}

void ArkWebEngineImpl::SetAppCustomUserAgent(const ArkWebString& userAgent)
{
    nweb_engine_->SetAppCustomUserAgent(ArkWebStringStructToClass(userAgent));
}

void ArkWebEngineImpl::SetUserAgentForHosts(const ArkWebString& userAgent, const ArkWebStringVector& hosts)
{
    nweb_engine_->SetUserAgentForHosts(ArkWebStringStructToClass(userAgent), ArkWebStringVectorStructToClass(hosts));
}

void ArkWebEngineImpl::EnableWholeWebPageDrawing()
{
    nweb_engine_->EnableWholeWebPageDrawing();
}

ArkWebRefPtr<ArkWebAdsBlockManager> ArkWebEngineImpl::GetAdsBlockManager()
{
    std::shared_ptr<OHOS::NWeb::NWebAdsBlockManager> nweb_adsBlock_manager = nweb_engine_->GetAdsBlockManager();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_adsBlock_manager)) {
        return nullptr;
    }
    return new ArkWebAdsBlockManagerImpl(nweb_adsBlock_manager);
}

void ArkWebEngineImpl::TrimMemoryByPressureLevel(int32_t memoryLevel)
{
    nweb_engine_->TrimMemoryByPressureLevel(memoryLevel);
}

void ArkWebEngineImpl::SetArkWebRomApiLevel(int apiLevel)
{
    nweb_engine_->SetArkWebRomApiLevel(apiLevel);
}

int ArkWebEngineImpl::GetArkWebCoreApiLevel()
{
    return nweb_engine_->GetArkWebCoreApiLevel();
}

void ArkWebEngineImpl::RemoveAllCache(bool include_disk_files)
{
    nweb_engine_->RemoveAllCache(include_disk_files);
}

void ArkWebEngineImpl::SetProxyOverride(const ArkWebStringVector& proxyUrls,
                                        const ArkWebStringVector& proxySchemeFilters,
                                        const ArkWebStringVector& bypassRules,
                                        const bool& reverseBypass,
                                        ArkWebRefPtr<ArkWebProxyChangedCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebProxyChangedCallback> nweb_proxy_callback =
        std::make_shared<ArkWebProxyChangedCallbackImpl>(callback);
    nweb_engine_->SetProxyOverride(ArkWebStringVectorStructToClass(proxyUrls),
                                   ArkWebStringVectorStructToClass(proxySchemeFilters),
                                   ArkWebStringVectorStructToClass(bypassRules),
                                   reverseBypass,
                                   nweb_proxy_callback);
}

void ArkWebEngineImpl::RemoveProxyOverride(ArkWebRefPtr<ArkWebProxyChangedCallback> callback)
{
    std::shared_ptr<OHOS::NWeb::NWebProxyChangedCallback> nweb_proxy_callback =
        std::make_shared<ArkWebProxyChangedCallbackImpl>(callback);
    nweb_engine_->RemoveProxyOverride(nweb_proxy_callback);
}

void ArkWebEngineImpl::SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port)
{
    nweb_engine_->SetWebDebuggingAccessAndPort(isEnableDebug, port);
}

uint32_t ArkWebEngineImpl::AddBlanklessLoadingUrls(const ArkWebStringVector& urls)
{
    return nweb_engine_->AddBlanklessLoadingUrls(ArkWebStringVectorStructToClass(urls));
}

void ArkWebEngineImpl::RemoveBlanklessLoadingUrls(const ArkWebStringVector& urls)
{
    nweb_engine_->RemoveBlanklessLoadingUrls(ArkWebStringVectorStructToClass(urls));
}

void ArkWebEngineImpl::ClearBlanklessLoadingCache(const ArkWebStringVector& urls)
{
    nweb_engine_->ClearBlanklessLoadingCache(ArkWebStringVectorStructToClass(urls));
}

ArkWebString ArkWebEngineImpl::CheckBlankOptEnable(const ArkWebString& url, int32_t nweb_id)
{
    return ArkWebStringClassToStruct(nweb_engine_->CheckBlankOptEnable(ArkWebStringStructToClass(url), nweb_id));
}

void ArkWebEngineImpl::SetBlanklessLoadingCacheCapacity(int32_t capacity)
{
    nweb_engine_->SetBlanklessLoadingCacheCapacity(capacity);
}

void ArkWebEngineImpl::EnablePrivateNetworkAccess(bool enable)
{
    nweb_engine_->EnablePrivateNetworkAccess(enable);
}

bool ArkWebEngineImpl::IsPrivateNetworkAccessEnabled()
{
    return nweb_engine_->IsPrivateNetworkAccessEnabled();
}

void ArkWebEngineImpl::SetWebDestroyMode(int32_t mode)
{
    nweb_engine_->SetWebDestroyMode(static_cast<OHOS::NWeb::WebDestroyMode>(mode));
}

int32_t ArkWebEngineImpl::SetSiteIsolationMode(int32_t mode)
{
    return static_cast<int32_t>(nweb_engine_->SetSiteIsolationMode(static_cast<ArkWebSiteIsolationMode>(mode)));
}

int32_t ArkWebEngineImpl::GetSiteIsolationMode()
{
    return static_cast<int32_t>(nweb_engine_->GetSiteIsolationMode());
}

void ArkWebEngineImpl::SetSocketIdleTimeout(int32_t timeout)
{
    nweb_engine_->SetSocketIdleTimeout(timeout);
}

void ArkWebEngineImpl::SetSoftKeyboardBehaviorMode(int32_t mode)
{
    nweb_engine_->SetSoftKeyboardBehaviorMode(static_cast<OHOS::NWeb::WebSoftKeyboardBehaviorMode>(mode));
}

} // namespace OHOS::ArkWeb
