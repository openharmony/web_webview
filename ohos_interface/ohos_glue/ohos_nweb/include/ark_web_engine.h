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

#ifndef ARK_WEB_ENGINE_H_
#define ARK_WEB_ENGINE_H_
#pragma once

#include "ohos_nweb/include/ark_web_adsblock_manager.h"
#include "ohos_nweb/include/ark_web_cookie_manager.h"
#include "ohos_nweb/include/ark_web_data_base.h"
#include "ohos_nweb/include/ark_web_download_manager.h"
#include "ohos_nweb/include/ark_web_engine_init_args.h"
#include "ohos_nweb/include/ark_web_engine_prefetch_args.h"
#include "ohos_nweb/include/ark_web_nweb.h"
#include "ohos_nweb/include/ark_web_nweb_create_info.h"
#include "ohos_nweb/include/ark_web_web_storage.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebEngine : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    static ArkWebRefPtr<ArkWebEngine> GetInstance();

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebNWeb> CreateNWeb(ArkWebRefPtr<ArkWebNWebCreateInfo> create_info) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebNWeb> GetNWeb(int32_t nweb_id) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebDataBase> GetDataBase() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebWebStorage> GetWebStorage() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebCookieManager> GetCookieManager() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebDownloadManager> GetDownloadManager() = 0;

    /*--ark web()--*/
    virtual void SetWebTag(int32_t nweb_id, const char* web_tag) = 0;

    /*--ark web()--*/
    virtual void InitializeWebEngine(ArkWebRefPtr<ArkWebEngineInitArgs> init_args) = 0;

    /*--ark web()--*/
    virtual void PrepareForPageLoad(const ArkWebString& url, bool preconnectable, int32_t num_sockets) = 0;

    /*--ark web()--*/
    virtual void SetWebDebuggingAccess(bool isEnableDebug) = 0;

    /*--ark web()--*/
    virtual void AddIntelligentTrackingPreventionBypassingList(const ArkWebStringVector& hosts) = 0;

    /*--ark web()--*/
    virtual void RemoveIntelligentTrackingPreventionBypassingList(const ArkWebStringVector& hosts) = 0;

    /*--ark web()--*/
    virtual void ClearIntelligentTrackingPreventionBypassingList() = 0;

    /*--ark web()--*/
    virtual void PauseAllTimers() = 0;

    /*--ark web()--*/
    virtual void ResumeAllTimers() = 0;

    /*--ark web()--*/
    virtual void PrefetchResource(ArkWebRefPtr<ArkWebEnginePrefetchArgs>& pre_args,
        const ArkWebStringMap& additional_http_headers, const ArkWebString& cache_key,
        const uint32_t& cache_valid_time) = 0;

    /*--ark web()--*/
    virtual void SetRenderProcessMode(int32_t mode) = 0;

    /*--ark web()--*/
    virtual int32_t GetRenderProcessMode() = 0;

    /*--ark web()--*/
    virtual void ClearPrefetchedResource(const ArkWebStringVector& cache_key_list) = 0;

    /*--ark web()--*/
    virtual void WarmupServiceWorker(const ArkWebString& url) = 0;

    /*--ark web()--*/
    virtual void SetHostIP(const ArkWebString& hostName, const ArkWebString& address, int32_t aliveTime) = 0;

    /*--ark web()--*/
    virtual void ClearHostIP(const ArkWebString& hostName) = 0;

    /*--ark web()--*/
    virtual void SetWholeWebDrawing() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebAdsBlockManager> GetAdsBlockManager() = 0;

    /*--ark web()--*/
    virtual void TrimMemoryByPressureLevel(int32_t memoryLevel) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ENGINE_H_
