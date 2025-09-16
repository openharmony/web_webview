/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "nweb_helper.h"

#include <cstdint>
#include <refbase.h>
#include <surface.h>
#include <thread>

#include "app_mgr_client.h"
#include "application_context.h"
#include "ark_web_nweb_webview_bridge_helper.h"
#include "config_policy_utils.h"
#include "locale_config.h"
#include "nweb_config_helper.h"
#include "nweb_adapter_helper.h"
#include "nweb_api_level.h"
#include "nweb_c_api.h"
#include "nweb_enhance_surface_adapter.h"
#include "nweb_hisysevent.h"
#include "nweb_log.h"
#include "nweb_surface_adapter.h"
#include "parameter.h"
#include "parameters.h"

namespace {

const int32_t NS_TO_S = 1000000000;
const uint32_t NWEB_SURFACE_MAX_WIDTH = 7680;
const uint32_t NWEB_SURFACE_MAX_HEIGHT = 7680;

// Run DO macro for every function defined in the API.
#define FOR_EACH_API_FN(DO)                           \
    DO(WebDownloadManager_PutDownloadCallback);       \
    DO(WebDownloader_ResumeDownloadStatic);           \
    DO(WebDownloader_StartDownload);                  \
    DO(WebDownloader_CreateDownloadDelegateCallback); \
    DO(WebDownloader_SetDownloadBeforeStart);         \
    DO(WebDownloader_SetDownloadDidUpdate);           \
    DO(WebDownload_Continue);                         \
    DO(WebDownload_CancelBeforeDownload);             \
    DO(WebDownload_PauseBeforeDownload);              \
    DO(WebDownload_ResumeBeforeDownload);             \
    DO(WebDownload_Cancel);                           \
    DO(WebDownload_Pause);                            \
    DO(WebDownload_Resume);                           \
    DO(WebDownload_GetItemState);                     \
    DO(WebDownload_GetItemStateByGuid);               \
    DO(WebDownload_GetItemStateByGuidV2);             \
    DO(WebDownloadItem_Guid);                         \
    DO(WebDownloadItem_GetDownloadItemId);            \
    DO(WebDownloadItem_GetState);                     \
    DO(WebDownloadItem_CurrentSpeed);                 \
    DO(WebDownloadItem_PercentComplete);              \
    DO(WebDownloadItem_TotalBytes);                   \
    DO(WebDownloadItem_ReceivedBytes);                \
    DO(WebDownloadItem_FullPath);                     \
    DO(WebDownloadItem_Url);                          \
    DO(WebDownloadItem_OriginalUrl);                  \
    DO(WebDownloadItem_SuggestedFileName);            \
    DO(WebDownloadItem_ContentDisposition);           \
    DO(WebDownloadItem_ETag);                         \
    DO(WebDownloadItem_MimeType);                     \
    DO(WebDownloadItem_NWebId);                       \
    DO(WebDownloadItem_IsPaused);                     \
    DO(WebDownloadItem_Method);                       \
    DO(WebDownloadItem_LastErrorCode);                \
    DO(WebDownloadItem_ReceivedSlices);               \
    DO(WebDownloadItem_LastModified);                 \
    DO(WebDownloadItem_CreateWebDownloadItem);        \
    DO(WebDownloadItem_Destroy);                      \
    DO(WebDownloadItem_SetUrl);                       \
    DO(WebDownloadItem_SetFullPath);                  \
    DO(WebDownloadItem_SetETag);                      \
    DO(WebDownloadItem_SetLastModified);              \
    DO(WebDownloadItem_SetMimeType);                  \
    DO(WebDownloadItem_SetReceivedBytes);             \
    DO(WebDownloadItem_SetTotalBytes);                \
    DO(WebDownloadItem_SetReceivedSlices);            \
    DO(WebDownloadItem_SetGuid);                      \
    DO(DestroyBeforeDownloadCallbackWrapper);         \
    DO(DestroyDownloadItemCallbackWrapper)

struct NWebCApi {
    // Generate a function pointer field for every NWeb C API function.
#define GEN_FN_PTR(fn) decltype(&(fn)) impl_##fn = nullptr
    FOR_EACH_API_FN(GEN_FN_PTR);
#undef GEN_FN_PTR
};

template<typename Fn>
void LoadFunction(const char* functionName, Fn* fnOut)
{
    void* fn = OHOS::NWeb::NWebHelper::Instance().LoadFuncSymbol(functionName);
    if (!fn) {
        WVLOG_E("%{public}s not found.", functionName);
        return;
    }
    *fnOut = reinterpret_cast<Fn>(fn);
}

NWebCApi* g_nwebCApi = nullptr;

void LoadNWebCApi(NWebCApi* api)
{
    // Initialize each NWebExApi function pointer field from the DLL
#define LOAD_FN_PTR(fn) LoadFunction(#fn, &api->impl_##fn)
    FOR_EACH_API_FN(LOAD_FN_PTR);
#undef LOAD_FN_PTR
}

bool LoadNWebSDK()
{
    if (g_nwebCApi) {
        WVLOG_I("LoadNWebSDK had loaded.");
        return true;
    }

    auto* nwebCApi = new NWebCApi();
    if (nwebCApi == nullptr) {
        WVLOG_E("nwebCApi is nullptr.");
        return false;
    }
    LoadNWebCApi(nwebCApi);
    g_nwebCApi = nwebCApi;
    return true;
}
#undef FOR_EACH_API_FN
} // namespace

extern "C" void WebDownloadManager_PutDownloadCallback(WebDownloadDelegateCallback* callback)
{
    if (!g_nwebCApi->impl_WebDownloadManager_PutDownloadCallback) {
        WVLOG_E("WebDownloadManager_PutDownloadCallback not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadManager_PutDownloadCallback(callback);
}

extern "C" void WebDownloader_SetDownloadBeforeStart(WebDownloadDelegateCallback* callback, OnDownloadBeforeStart fun)
{
    if (!g_nwebCApi->impl_WebDownloader_SetDownloadBeforeStart) {
        WVLOG_E("WebDownloader_SetDownloadBeforeStart not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloader_SetDownloadBeforeStart(callback, fun);
}

extern "C" void WebDownloader_SetDownloadDidUpdate(WebDownloadDelegateCallback* callback, OnDownloadDidUpdate fun)
{
    if (!g_nwebCApi->impl_WebDownloader_SetDownloadDidUpdate) {
        WVLOG_E("WebDownloader_SetDownloadDidUpdate not found");
        return;
    }
    g_nwebCApi->impl_WebDownloader_SetDownloadDidUpdate(callback, fun);
}

extern "C" void WebDownloader_ResumeDownloadStatic(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloader_ResumeDownloadStatic) {
        WVLOG_E("WebDownloader_ResumeDownloadStatic not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloader_ResumeDownloadStatic(downloadItem);
}

extern "C" void WebDownloader_StartDownload(int32_t nwebId, const char* url)
{
    if (!g_nwebCApi->impl_WebDownloader_StartDownload) {
        WVLOG_E("WebDownloader_StartDownload not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloader_StartDownload(nwebId, url);
}

extern "C" void WebDownloader_CreateDownloadDelegateCallback(WebDownloadDelegateCallback** callback)
{
    if (!g_nwebCApi || !g_nwebCApi->impl_WebDownloader_CreateDownloadDelegateCallback) {
        WVLOG_E("WebDownloader_CreateDownloadDelegateCallback not found.");
        return;
    }

    return g_nwebCApi->impl_WebDownloader_CreateDownloadDelegateCallback(callback);
}

extern "C" void WebDownload_Continue(const WebBeforeDownloadCallbackWrapper* wrapper, const char* downloadPath)
{
    if (!g_nwebCApi->impl_WebDownload_Continue) {
        WVLOG_E("WebDownload_Continue not found.");
        return;
    }
    g_nwebCApi->impl_WebDownload_Continue(wrapper, downloadPath);
}

extern "C" void WebDownload_CancelBeforeDownload(const WebBeforeDownloadCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_WebDownload_CancelBeforeDownload) {
        WVLOG_E("WebDownload_CancelBeforeDownload not found.");
        return;
    }
    g_nwebCApi->impl_WebDownload_CancelBeforeDownload(wrapper);
}

extern "C" void WebDownload_PauseBeforeDownload(const WebBeforeDownloadCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_WebDownload_PauseBeforeDownload) {
        WVLOG_E("WebDownload_PauseBeforeDownload not found.");
        return;
    }
    g_nwebCApi->impl_WebDownload_PauseBeforeDownload(wrapper);
}

extern "C" void WebDownload_ResumeBeforeDownload(const WebBeforeDownloadCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_WebDownload_ResumeBeforeDownload) {
        WVLOG_E("WebDownload_ResumeBeforeDownload not found.");
        return;
    }
    g_nwebCApi->impl_WebDownload_ResumeBeforeDownload(wrapper);
}

extern "C" void WebDownload_Cancel(const WebDownloadItemCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_WebDownload_Cancel) {
        WVLOG_E("WebDownload_Cancel not found.");
        return;
    }
    g_nwebCApi->impl_WebDownload_Cancel(wrapper);
}

extern "C" void WebDownload_Pause(const WebDownloadItemCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_WebDownload_Pause) {
        WVLOG_E("WebDownload_Pause not found");
        return;
    }
    g_nwebCApi->impl_WebDownload_Pause(wrapper);
}

extern "C" void WebDownload_Resume(const WebDownloadItemCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_WebDownload_Resume) {
        WVLOG_E("WebDownload_Resume not found.");
        return;
    }
    g_nwebCApi->impl_WebDownload_Resume(wrapper);
}

extern "C" NWebDownloadItemState WebDownload_GetItemState(int32_t nwebId, long downloadItemId)
{
    if (!g_nwebCApi || !g_nwebCApi->impl_WebDownload_GetItemState) {
        return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
    }
    return g_nwebCApi->impl_WebDownload_GetItemState(nwebId, downloadItemId);
}

extern "C" NWebDownloadItemState WebDownload_GetItemStateByGuid(const std::string& guid)
{
    if (!g_nwebCApi) {
        return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
    }
    if (g_nwebCApi->impl_WebDownload_GetItemStateByGuidV2) {
        return g_nwebCApi->impl_WebDownload_GetItemStateByGuidV2(guid.c_str());
    }
    if (g_nwebCApi->impl_WebDownload_GetItemStateByGuid) {
        return g_nwebCApi->impl_WebDownload_GetItemStateByGuid(guid);
    }
    return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
}

extern "C" NWebDownloadItemState WebDownload_GetItemStateByGuidV2(const char* guid)
{
    if (!g_nwebCApi || !g_nwebCApi->impl_WebDownload_GetItemStateByGuidV2) {
        return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
    }
    return g_nwebCApi->impl_WebDownload_GetItemStateByGuidV2(guid);
}

extern "C" char* WebDownloadItem_Guid(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_Guid) {
        WVLOG_E("WebDownloadItem_Guid not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_Guid(downloadItem);
}

extern "C" long WebDownloadItem_GetDownloadItemId(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_GetDownloadItemId) {
        return false;
    }
    return g_nwebCApi->impl_WebDownloadItem_GetDownloadItemId(downloadItem);
}

extern "C" NWebDownloadItemState WebDownloadItem_GetState(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_GetState) {
        return NWebDownloadItemState::MAX_DOWNLOAD_STATE;
    }
    return g_nwebCApi->impl_WebDownloadItem_GetState(downloadItem);
}

extern "C" int WebDownloadItem_CurrentSpeed(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_CurrentSpeed) {
        WVLOG_E("WebDownloadItem_CurrentSpeed not found.");
        return 0;
    }
    return g_nwebCApi->impl_WebDownloadItem_CurrentSpeed(downloadItem);
}

extern "C" int WebDownloadItem_PercentComplete(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_PercentComplete) {
        WVLOG_E("WebDownloadItem_TotalBytes not found.");
        return 0;
    }
    return g_nwebCApi->impl_WebDownloadItem_PercentComplete(downloadItem);
}

extern "C" int64_t WebDownloadItem_TotalBytes(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_TotalBytes) {
        WVLOG_E("WebDownloadItem_TotalBytes not found.");
        return 0;
    }
    return g_nwebCApi->impl_WebDownloadItem_TotalBytes(downloadItem);
}

extern "C" int64_t WebDownloadItem_ReceivedBytes(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_ReceivedBytes) {
        WVLOG_E("WebDownloadItem_ReceivedBytes not found.");
        return 0;
    }
    return g_nwebCApi->impl_WebDownloadItem_ReceivedBytes(downloadItem);
}

extern "C" char* WebDownloadItem_FullPath(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_FullPath) {
        WVLOG_E("WebDownloadItem_FullPath not found");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_FullPath(downloadItem);
}

extern "C" char* WebDownloadItem_Url(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_Url) {
        WVLOG_E("WebDownloadItem_Url not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_Url(downloadItem);
}

extern "C" char* WebDownloadItem_OriginalUrl(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_OriginalUrl) {
        WVLOG_E("WebDownloadItem_OriginalUrl not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_OriginalUrl(downloadItem);
}

extern "C" char* WebDownloadItem_SuggestedFileName(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SuggestedFileName) {
        WVLOG_E("WebDownloadItem_SuggestedFileName not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_SuggestedFileName(downloadItem);
}

extern "C" char* WebDownloadItem_ContentDisposition(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_ContentDisposition) {
        WVLOG_E("WebDownloadItem_ContentDisposition not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_ContentDisposition(downloadItem);
}

extern "C" char* WebDownloadItem_ETag(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_ETag) {
        WVLOG_E("WebDownloadItem_ETag not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_ETag(downloadItem);
}

extern "C" char* WebDownloadItem_MimeType(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_MimeType) {
        WVLOG_E("WebDownloadItem_MimeType not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_MimeType(downloadItem);
}

extern "C" bool WebDownloadItem_IsPaused(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_IsPaused) {
        WVLOG_E("WebDownloadItem_IsPaused not found.");
        return false;
    }
    return g_nwebCApi->impl_WebDownloadItem_IsPaused(downloadItem);
}

extern "C" char* WebDownloadItem_Method(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_Method) {
        WVLOG_E("WebDownloadItem_Method not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_Method(downloadItem);
}

extern "C" int WebDownloadItem_LastErrorCode(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_LastErrorCode) {
        WVLOG_E("WebDownloadItem_LastErrorCode not found.");
        return 0;
    }
    return g_nwebCApi->impl_WebDownloadItem_LastErrorCode(downloadItem);
}

extern "C" char* WebDownloadItem_ReceivedSlices(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_ReceivedSlices) {
        WVLOG_E("WebDownloadItem_ReceivedSlices not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_ReceivedSlices(downloadItem);
}

extern "C" char* WebDownloadItem_LastModified(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_LastModified) {
        WVLOG_E("WebDownloadItem_LastModified not found.");
        return nullptr;
    }
    return g_nwebCApi->impl_WebDownloadItem_LastModified(downloadItem);
}

extern "C" int WebDownloadItem_NWebId(const NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_NWebId) {
        WVLOG_E("WebDownloadItem_NWebId not found.");
        return -1;
    }
    return g_nwebCApi->impl_WebDownloadItem_NWebId(downloadItem);
}

extern "C" void WebDownloadItem_CreateWebDownloadItem(NWebDownloadItem** downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_CreateWebDownloadItem) {
        WVLOG_E("WebDownloadItem_CreateWebDownloadItem not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_CreateWebDownloadItem(downloadItem);
}

extern "C" void WebDownloadItem_Destroy(NWebDownloadItem* downloadItem)
{
    if (!g_nwebCApi->impl_WebDownloadItem_Destroy) {
        WVLOG_E("WebDownloadItem_Destroy not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_Destroy(downloadItem);
}

extern "C" void DestroyBeforeDownloadCallbackWrapper(WebBeforeDownloadCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_DestroyBeforeDownloadCallbackWrapper) {
        WVLOG_E("DestroyBeforeDownloadCallbackWrapper not found.");
        return;
    }
    g_nwebCApi->impl_DestroyBeforeDownloadCallbackWrapper(wrapper);
}

extern "C" void DestroyDownloadItemCallbackWrapper(WebDownloadItemCallbackWrapper* wrapper)
{
    if (!g_nwebCApi->impl_DestroyDownloadItemCallbackWrapper) {
        WVLOG_E("DestroyDownloadItemCallbackWrapper not found.");
        return;
    }
    g_nwebCApi->impl_DestroyDownloadItemCallbackWrapper(wrapper);
}

extern "C" void WebDownloadItem_SetGuid(NWebDownloadItem* downloadItem, const char* guid)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetGuid) {
        WVLOG_E("WebDownloadItem_SetGuid not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetGuid(downloadItem, guid);
}

extern "C" void WebDownloadItem_SetUrl(NWebDownloadItem* downloadItem, const char* url)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetUrl) {
        WVLOG_E("WebDownloadItem_SetUrl not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetUrl(downloadItem, url);
}

extern "C" void WebDownloadItem_SetFullPath(NWebDownloadItem* downloadItem, const char* fullPath)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetFullPath) {
        WVLOG_E("WebDownloadItem_SetFullPath not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetFullPath(downloadItem, fullPath);
}

extern "C" void WebDownloadItem_SetETag(NWebDownloadItem* downloadItem, const char* etag)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetETag) {
        WVLOG_E("WebDownloadItem_SetETag not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetETag(downloadItem, etag);
}

extern "C" void WebDownloadItem_SetLastModified(NWebDownloadItem* downloadItem, const char* lastModified)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetLastModified) {
        WVLOG_E("WebDownloadItem_SetLastModified not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetLastModified(downloadItem, lastModified);
}

extern "C" void WebDownloadItem_SetMimeType(NWebDownloadItem* downloadItem, const char* mimeType)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetMimeType) {
        WVLOG_E("WebDownloadItem_SetMimeType not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetMimeType(downloadItem, mimeType);
}

extern "C" void WebDownloadItem_SetReceivedBytes(NWebDownloadItem* downloadItem, int64_t receivedBytes)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetReceivedBytes) {
        WVLOG_E("WebDownloadItem_SetReceivedBytes not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetReceivedBytes(downloadItem, receivedBytes);
}

extern "C" void WebDownloadItem_SetTotalBytes(NWebDownloadItem* downloadItem, int64_t totalBytes)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetTotalBytes) {
        WVLOG_E("WebDownloadItem_SetTotalBytes not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetTotalBytes(downloadItem, totalBytes);
}

extern "C" void WebDownloadItem_SetReceivedSlices(NWebDownloadItem* downloadItem, const char* receivedSlices)
{
    if (!g_nwebCApi->impl_WebDownloadItem_SetReceivedSlices) {
        WVLOG_E("WebDownloadItem_SetReceivedSlices not found.");
        return;
    }
    g_nwebCApi->impl_WebDownloadItem_SetReceivedSlices(downloadItem, receivedSlices);
}

namespace OHOS::NWeb {
bool NWebHelper::LoadNWebSDK()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return false;
    }

    return ::LoadNWebSDK();
}

NWebHelper& NWebHelper::Instance()
{
    static NWebHelper helper;
    return helper;
}

bool NWebHelper::Init(bool from_ark)
{
    return LoadWebEngine(from_ark, false);
}

bool NWebHelper::InitAndRun(bool from_ark)
{
    return LoadWebEngine(from_ark, true);
}

bool NWebHelper::HasLoadWebEngine()
{
    return nwebEngine_ != nullptr;
}

void NWebHelper::SaveSchemeVector(const char* name, int32_t option)
{
    struct NwebScheme scheme = {.name = std::string(name), .option = option};
    schemeVector_.push_back(scheme);
}

bool NWebHelper::RegisterCustomSchemes()
{
    int32_t (*registerCustomSchemes)(const char* scheme, int32_t option) = nullptr;

#define LOAD_FN_PTR(apiMember, funcImpl) LoadFunction(#funcImpl, &(apiMember))
    LOAD_FN_PTR(registerCustomSchemes, OH_ArkWeb_RegisterCustomSchemes);
#undef LOAD_FN_PTR
    if (!registerCustomSchemes) {
        WVLOG_E("OH_ArkWeb_RegisterCustomSchemes failed to load function registerCustomSchemes");
        return false;
    }

    int32_t registerResult;
    for (auto it = schemeVector_.begin(); it != schemeVector_.end(); ++it) {
        registerResult = registerCustomSchemes(it->name.c_str(), it->option);
        if (registerResult != NO_ERROR) {
            WVLOG_E("register custom schemes fails, registerResult = %{public}d", registerResult);
            return false;
        }
    }
    return true;
}

bool NWebHelper::GetWebEngine(bool fromArk)
{
    if (nwebEngine_) {
        return true;
    }

    if (bundlePath_.empty()) {
        auto ctx = AbilityRuntime::ApplicationContext::GetApplicationContext();
        if (!ctx) {
            WVLOG_E("failed to get application context");
            return false;
        }

        SetBundlePath(ctx->GetBundleCodeDir());
        if (bundlePath_.empty()) {
            WVLOG_E("bundle path is empty");
            return false;
        }
    }

    fromArk = fromArk && !NWebConfigHelper::Instance().IsWebPlayGroundEnable();
    if (!ArkWeb::ArkWebNWebWebviewBridgeHelper::GetInstance().Init(fromArk, bundlePath_)) {
        WVLOG_E("failed to init arkweb nweb bridge helper");
        return false;
    }

    auto appMgrClient = std::make_unique<OHOS::AppExecFwk::AppMgrClient>();
    if (appMgrClient->ConnectAppMgrService() == OHOS::AppExecFwk::AppMgrResultCode::RESULT_OK) {
        WVLOG_I("call func NotifyProcessDependedOnWeb and return code is %{public}d",
            appMgrClient->NotifyProcessDependedOnWeb());
    }

    nwebEngine_ = NWebEngine::GetInstance();
    if (nwebEngine_ == nullptr) {
        WVLOG_E("failed to get web engine instance");
        return false;
    }

    if (RegisterCustomSchemes() == false) {
        WVLOG_E("register custom schemes failed");
    }

    coreApiLevel_ = nwebEngine_->GetArkWebCoreApiLevel();
    WVLOG_E("api level of arkweb core is %{public}d", coreApiLevel_);

    nwebEngine_->SetArkWebRomApiLevel(ARKWEB_CORE_API_LEVEL);
    return true;
}

bool NWebHelper::InitWebEngine()
{
    if (initFlag_) {
        return true;
    }

    auto ctx = AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!ctx) {
        WVLOG_E("failed to get application context");
        return false;
    }

    if (ctx->GetBaseDir().empty()) {
        WVLOG_E("base dir of application context is empty");
        return false;
    }

    auto initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    NWebAdapterHelper::Instance().ParseConfig(initArgs);

    initArgs->AddArg(std::string("--user-data-dir=").append(ctx->GetBaseDir()));
    initArgs->AddArg(std::string("--bundle-installation-dir=").append(bundlePath_));

    std::string arkWebInstallPath = OHOS::system::GetParameter("persist.arkwebcore.install_path", "");
    if (!arkWebInstallPath.empty()) {
        initArgs->AddArg(std::string("--arkwebcore-install-path=").append(arkWebInstallPath));
    }

    if (!customSchemeCmdLine_.empty()) {
        initArgs->AddArg(std::string("--ohos-custom-scheme=").append(customSchemeCmdLine_));
    }

    std::string systemRegion = OHOS::Global::I18n::LocaleConfig::GetSystemRegion();
    std::string systemLanguage = OHOS::Global::I18n::LocaleConfig::GetSystemLanguage();

    size_t dashPos = systemLanguage.find('-');
    if (dashPos == std::string::npos) {
        initArgs->AddArg(std::string("--lang=").append(systemLanguage + "-" + systemRegion));
    } else {
        initArgs->AddArg(std::string("--lang=").append(systemLanguage.substr(0, dashPos) + "-" + systemRegion));
    }

    for (auto backForwardCacheCmdLine : backForwardCacheCmdLine_) {
        initArgs->AddArg(backForwardCacheCmdLine);
        WVLOG_I("add command line when init web engine: %{public}s", backForwardCacheCmdLine.c_str());
    }

    // Append API version.
    std::shared_ptr<AppExecFwk::ApplicationInfo> appInfo = ctx->GetApplicationInfo();
    if (appInfo) {
      std::string apiVersion = std::to_string(appInfo->apiTargetVersion);
      initArgs->AddArg(std::string("--user-api-version=").append(apiVersion));
      WVLOG_D("apiTargetVersion: %{public}s", apiVersion.c_str());
    }

    if (!autoPreconnectEnabled_) {
        initArgs->AddArg(std::string("--disable-auto-preconnect"));
    }

    initArgs->AddArg(std::string("--socket-idle-timeout=").append(std::to_string(socketIdleTimeout_)));

    nwebEngine_->InitializeWebEngine(initArgs);
    initFlag_ = true;

    WVLOG_I("succeed to init web engine");
    return true;
}

bool NWebHelper::LoadWebEngine(bool fromArk, bool runFlag)
{
    std::lock_guard<std::mutex> lock(lock_);
    if (!GetWebEngine(fromArk)) {
        return false;
    }

    if (runFlag) {
        return InitWebEngine();
    }

    return true;
}

void* NWebHelper::LoadFuncSymbol(const char* funcName)
{
    return ArkWeb::ArkWebNWebWebviewBridgeHelper::GetInstance().LoadFuncSymbol(funcName);
}

void NWebAdapterHelper::ReadConfigIfNeeded()
{
    NWebConfigHelper::Instance().ReadConfigIfNeeded();
}

void NWebHelper::SetBundlePath(const std::string& path)
{
    bundlePath_ = path;
}

std::shared_ptr<NWeb> NWebHelper::CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return nullptr;
    }

    return nwebEngine_->CreateNWeb(create_info);
}

std::shared_ptr<NWebCookieManager> NWebHelper::GetCookieManager()
{
    if (!LoadWebEngine(true, true)) {
        WVLOG_E("failed to load web engine");
        return nullptr;
    }

    return nwebEngine_->GetCookieManager();
}

std::shared_ptr<NWeb> NWebHelper::GetNWeb(int32_t nweb_id)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return nullptr;
    }

    return nwebEngine_->GetNWeb(nweb_id);
}

void NWebHelper::SetWebTag(int32_t nweb_id, const char* webTag)
{
    if (!LoadWebEngine(true, false)) {
        WVLOG_E("failed to load web engine");
        return;
    }

    nwebEngine_->SetWebTag(nweb_id, webTag);
}

void NWebHelper::SetHttpDns(std::shared_ptr<NWebDOHConfig> config)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    auto downloadManager = nwebEngine_->GetDownloadManager();
    if (!downloadManager) {
        WVLOG_E("download manager is nullptr");
        return;
    }

    downloadManager->SetHttpDns(config);
}

void NWebHelper::SetProxyOverride(const std::vector<std::string>& proxyUrls,
                                  const std::vector<std::string>& proxySchemeFilters,
                                  const std::vector<std::string>& bypassRules,
                                  const bool& reverseBypass,
                                  std::shared_ptr<NWebProxyChangedCallback> callback)
{
    if (!LoadWebEngine(true, true)) {
        WVLOG_E("failed to load web engine");
        return;
    }

    nwebEngine_->SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules, reverseBypass, callback);
}

void NWebHelper::RemoveProxyOverride(std::shared_ptr<NWebProxyChangedCallback> callback)
{
    if (!LoadWebEngine(true, true)) {
        WVLOG_E("failed to load web engine");
        return;
    }

    nwebEngine_->RemoveProxyOverride(callback);
}

void NWebHelper::PrepareForPageLoad(std::string url, bool preconnectable, int32_t numSockets)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->PrepareForPageLoad(url, preconnectable, numSockets);
}

std::shared_ptr<NWebDataBase> NWebHelper::GetDataBase()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return nullptr;
    }

    return nwebEngine_->GetDataBase();
}

std::shared_ptr<NWebWebStorage> NWebHelper::GetWebStorage()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return nullptr;
    }

    return nwebEngine_->GetWebStorage();
}

void NWebHelper::SetConnectionTimeout(const int32_t& timeout)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    auto downloadManager = nwebEngine_->GetDownloadManager();
    if (!downloadManager) {
        WVLOG_E("download manager is nullptr");
        return;
    }

    downloadManager->SetConnectionTimeout(timeout);
    WVLOG_I("timeout value in NWebHelper: %{public}d", timeout);
}

void NWebHelper::AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->AddIntelligentTrackingPreventionBypassingList(hosts);
}

void NWebHelper::RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->RemoveIntelligentTrackingPreventionBypassingList(hosts);
}

void NWebHelper::ClearIntelligentTrackingPreventionBypassingList()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->ClearIntelligentTrackingPreventionBypassingList();
}

std::string NWebHelper::GetDefaultUserAgent()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return "";
    }

    return nwebEngine_->GetDefaultUserAgent();
}

void NWebHelper::PauseAllTimers()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->PauseAllTimers();
}

void NWebHelper::ResumeAllTimers()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->ResumeAllTimers();
}

void NWebHelper::PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
    const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
    const uint32_t& cache_valid_time)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->PrefetchResource(pre_args, additional_http_headers, cache_key, cache_valid_time);
}

void NWebHelper::ClearPrefetchedResource(const std::vector<std::string>& cache_key_list)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->ClearPrefetchedResource(cache_key_list);
}

void NWebHelper::SetRenderProcessMode(RenderProcessMode mode)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->SetRenderProcessMode(mode);
}

RenderProcessMode NWebHelper::GetRenderProcessMode()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return RenderProcessMode::SINGLE_MODE;
    }

    return nwebEngine_->GetRenderProcessMode();
}

int32_t NWebHelper::SetSiteIsolationMode(SiteIsolationMode mode)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return 0;
    }

    return nwebEngine_->SetSiteIsolationMode(mode);
}

SiteIsolationMode NWebHelper::GetSiteIsolationMode()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return SiteIsolationMode::PARTIAL;
    }

    return nwebEngine_->GetSiteIsolationMode();
}

void NWebHelper::SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->SetHostIP(hostName, address, aliveTime);
}

void NWebHelper::EnableBackForwardCache(bool enableNativeEmbed, bool enableMediaTakeOver)
{
    this->backForwardCacheCmdLine_.emplace_back("--enable-bfcache");
    if (enableNativeEmbed) {
        this->backForwardCacheCmdLine_.emplace_back("--enable-cache-native-embed");
    }

    if (enableMediaTakeOver) {
        this->backForwardCacheCmdLine_.emplace_back("--enable-cache-media-take-over");
    }
}

void NWebHelper::ClearHostIP(const std::string& hostName)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->ClearHostIP(hostName);
}

void NWebHelper::SetAppCustomUserAgent(const std::string& userAgent)
{
    if (!LoadWebEngine(true, false)) {
        WVLOG_E("failed to load web engine");
        return;
    }

    nwebEngine_->SetAppCustomUserAgent(userAgent);
}

void NWebHelper::SetUserAgentForHosts(const std::string& userAgent, const std::vector<std::string>& hosts)
{
    if (!LoadWebEngine(true, false)) {
        WVLOG_E("failed to load web engine");
        return;
    }

    nwebEngine_->SetUserAgentForHosts(userAgent, hosts);
}

void NWebHelper::WarmupServiceWorker(const std::string& url)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->WarmupServiceWorker(url);
}

void NWebHelper::EnableWholeWebPageDrawing()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->EnableWholeWebPageDrawing();
}

std::shared_ptr<NWebAdsBlockManager> NWebHelper::GetAdsBlockManager()
{
    if (!LoadWebEngine(true, false)) {
        WVLOG_E("failed to load web engine");
        return nullptr;
    }

    return nwebEngine_->GetAdsBlockManager();
}

void NWebHelper::TrimMemoryByPressureLevel(int32_t memoryLevel)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }

    nwebEngine_->TrimMemoryByPressureLevel(memoryLevel);
}

void NWebHelper::RemoveAllCache(bool includeDiskFiles)
{
    if (!LoadWebEngine(true, true)) {
        WVLOG_E("failed to load web engine");
        return;
    }
    if (nwebEngine_ == nullptr) {
        WVLOG_E("nweb engine is nullptr");
        return;
    }

    nwebEngine_->RemoveAllCache(includeDiskFiles);
}

void NWebHelper::SetBlanklessLoadingCacheCapacity(int32_t capacity)
{
    if (!LoadWebEngine(true, false)) {
        WVLOG_E("blankless failed to load web engine");
        return;
    }

    if (nwebEngine_ == nullptr) {
        WVLOG_E("blankless web engine is nullptr");
        return;
    }

    nwebEngine_->SetBlanklessLoadingCacheCapacity(capacity);
}

void NWebHelper::ClearBlanklessLoadingCache(const std::vector<std::string>& urls)
{
    if (!LoadWebEngine(true, false)) {
        WVLOG_E("blankless failed to load web engine");
        return;
    }

    if (nwebEngine_ == nullptr) {
        WVLOG_E("blankless web engine is nullptr");
        return;
    }

    nwebEngine_->ClearBlanklessLoadingCache(urls);
}

NWebAdapterHelper& NWebAdapterHelper::Instance()
{
    static NWebAdapterHelper helper;
    return helper;
}

bool NWebAdapterHelper::Init(bool from_ark)
{
    return NWebHelper::Instance().Init(from_ark);
}

static int64_t GetCurrentRealTimeNs()
{
    struct timespec ts = { 0, 0 };
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return (ts.tv_sec * NS_TO_S + ts.tv_nsec);
}

std::shared_ptr<NWeb> NWebAdapterHelper::CreateNWeb(sptr<Surface> surface,
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs, uint32_t width, uint32_t height, bool incognitoMode)
{
    int64_t startTime = GetCurrentRealTimeNs();
    if (surface == nullptr) {
        WVLOG_E("fail to create nweb, input surface is nullptr");
        return nullptr;
    }
    if (width > NWEB_SURFACE_MAX_WIDTH || height > NWEB_SURFACE_MAX_HEIGHT) {
        WVLOG_E("input size %{public}u*%{public}u is invalid.", width, height);
        return nullptr;
    }
    initArgs->AddArg(NWebConfigHelper::Instance().GetWebPlayGroundInitArg());
    if (!NWebHelper::Instance().IsAutoPreconnectEnabled()) {
        initArgs->AddArg(std::string("--disable-auto-preconnect"));
    }
    std::string socketIdleTimeout = std::to_string(NWebHelper::Instance().GetSocketIdleTimeout());
    initArgs->AddArg(std::string("--socket-idle-timeout=").append(socketIdleTimeout));
    auto createInfo = NWebSurfaceAdapter::Instance().GetCreateInfo(surface, initArgs, width, height, incognitoMode);
    NWebConfigHelper::Instance().ParseConfig(initArgs);

    // obtain bundle path
    std::shared_ptr<AbilityRuntime::ApplicationContext> ctx =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (ctx) {
        std::string bundleName = ctx->GetBundleName();
        NWebConfigHelper::Instance().SetBundleName(bundleName);
        initArgs->AddArg(std::string("--bundle-name=").append(bundleName));
    }

    auto nweb = NWebHelper::Instance().CreateNWeb(createInfo);
    if (nweb == nullptr) {
        WVLOG_E("fail to create nweb instance");
        return nullptr;
    }
    int64_t endTime = GetCurrentRealTimeNs();
    EventReport::ReportCreateWebInstanceTime(nweb->GetWebId(), endTime - startTime);
    return nweb;
}

std::shared_ptr<NWeb> NWebAdapterHelper::CreateNWeb(void* enhanceSurfaceInfo,
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs, uint32_t width, uint32_t height, bool incognitoMode)
{
    int64_t startTime = GetCurrentRealTimeNs();
    if (enhanceSurfaceInfo == nullptr) {
        WVLOG_E("fail to create nweb, input surface is nullptr");
        return nullptr;
    }
    if (width > NWEB_SURFACE_MAX_WIDTH || height > NWEB_SURFACE_MAX_HEIGHT) {
        WVLOG_E("input size %{public}u*%{public}u is invalid.", width, height);
        return nullptr;
    }
    initArgs->AddArg(NWebConfigHelper::Instance().GetWebPlayGroundInitArg());
    if (!NWebHelper::Instance().IsAutoPreconnectEnabled()) {
        initArgs->AddArg(std::string("--disable-auto-preconnect"));
    }
    std::string socketIdleTimeout = std::to_string(NWebHelper::Instance().GetSocketIdleTimeout());
    initArgs->AddArg(std::string("--socket-idle-timeout=").append(socketIdleTimeout));
    auto createInfo =
        NWebEnhanceSurfaceAdapter::Instance().GetCreateInfo(enhanceSurfaceInfo, initArgs, width, height, incognitoMode);
    auto nweb = NWebHelper::Instance().CreateNWeb(createInfo);
    if (nweb == nullptr) {
        WVLOG_E("fail to create nweb instance");
        return nullptr;
    }
    int64_t endTime = GetCurrentRealTimeNs();
    EventReport::ReportCreateWebInstanceTime(nweb->GetWebId(), endTime - startTime);
    return nweb;
}

void NWebAdapterHelper::ParseConfig(std::shared_ptr<NWebEngineInitArgsImpl> initArgs)
{
    NWebConfigHelper::Instance().ParseConfig(initArgs);
}

std::vector<FrameRateSetting> NWebAdapterHelper::GetPerfConfig(const std::string& settingName)
{
    auto perfConfig = NWebConfigHelper::Instance().GetPerfConfig(settingName);
    return perfConfig;
}

std::string NWebAdapterHelper::ParsePerfConfig(const std::string& configNodeName, const std::string& argsNodeName)
{
    std::string config = NWebConfigHelper::Instance().ParsePerfConfig(configNodeName, argsNodeName);
    return config;
}

bool NWebAdapterHelper::IsLTPODynamicApp(const std::string& bundleName)
{
    return NWebConfigHelper::Instance().IsLTPODynamicApp(bundleName);
}

int32_t NWebAdapterHelper::GetLTPOStrategy()
{
    return NWebConfigHelper::Instance().GetLTPOStrategy();
}

std::string NWebAdapterHelper::GetBundleName()
{
    return NWebConfigHelper::Instance().GetBundleName();
}

void NWebHelper::SetWebDebuggingAccess(bool isEnableDebug)
{
    if (!initFlag_) {
        WVLOG_E("SetWebDebuggingAccess, not initialized");
        return;
    }
    if (!nwebEngine_) {
        WVLOG_E("SetWebDebuggingAccess, nweb engine is nullptr");
        return;
    }
    nwebEngine_->SetWebDebuggingAccess(isEnableDebug);
}

void NWebHelper::SetWebDebuggingAccessAndPort(bool isEnableDebug, int32_t port)
{
    if (!initFlag_) {
        WVLOG_E("SetWebDebuggingAccessAndPort, not initialized");
        return;
    }
    if (!nwebEngine_) {
        WVLOG_E("SetWebDebuggingAccessAndPort, nweb engine is nullptr");
        return;
    }
    nwebEngine_->SetWebDebuggingAccessAndPort(isEnableDebug, port);
}

std::string NWebHelper::CheckBlankOptEnable(const std::string& url, int32_t nweb_id)
{
    if (!nwebEngine_) {
        WVLOG_E("blankless CheckBlankOptEnable, nweb engine is nullptr");
        return "";
    }
    return nwebEngine_->CheckBlankOptEnable(url, nweb_id);
}

void NWebHelper::EnablePrivateNetworkAccess(bool enable)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }
    nwebEngine_->EnablePrivateNetworkAccess(enable);
}

bool NWebHelper::IsPrivateNetworkAccessEnabled()
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return false;
    }
    return nwebEngine_->IsPrivateNetworkAccessEnabled();
}

void NWebHelper::SetWebDestroyMode(WebDestroyMode mode)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }
 
    nwebEngine_->SetWebDestroyMode(mode);
}

void NWebHelper::SetAutoPreconnect(bool enable)
{
    autoPreconnectEnabled_ = enable;
}

bool NWebHelper::IsAutoPreconnectEnabled()
{
    return autoPreconnectEnabled_;
}

void NWebHelper::SetSocketIdleTimeout(int32_t timeout)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        socketIdleTimeout_ = timeout;
        return;
    }

    nwebEngine_->SetSocketIdleTimeout(timeout);
}

int32_t NWebHelper::GetSocketIdleTimeout()
{
    return socketIdleTimeout_;
}

void NWebHelper::SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode mode)
{
    if (nwebEngine_ == nullptr) {
        WVLOG_E("web engine is nullptr");
        return;
    }
    nwebEngine_->SetSoftKeyboardBehaviorMode(mode);
}

} // namespace OHOS::NWeb
