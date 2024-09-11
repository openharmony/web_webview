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

#include "napi_fuzzer.h"

#include <cstring>
#include <securec.h>

#include "nweb_c_api.h"
#include "nweb_helper.h"

using namespace OHOS::NWeb;

namespace OHOS {
bool NapiFuzzTest(const uint8_t* data, size_t size)
{
    const std::string path("/data/app/el1/bundle/public/com.ohos.nweb");
    NWebHelper::Instance().SetBundlePath(path);

    NWebHelper::Instance().Init(false);

    if (NWebHelper::Instance().LoadNWebSDK()) {
        WebDownloadManager_PutDownloadCallback(nullptr);
        WebDownloader_SetDownloadBeforeStart(nullptr, nullptr);
        WebDownloader_SetDownloadDidUpdate(nullptr, nullptr);
        WebDownloader_ResumeDownloadStatic(nullptr);
        WebDownloader_StartDownload(size, NULL);
        WebDownloader_CreateDownloadDelegateCallback(nullptr);
        WebDownload_Continue(nullptr, nullptr);
        WebDownload_CancelBeforeDownload(nullptr);
        WebDownload_PauseBeforeDownload(nullptr);
        WebDownload_ResumeBeforeDownload(nullptr);
        WebDownload_Cancel(nullptr);
        WebDownload_Pause(nullptr);
        WebDownload_Resume(nullptr);

        WebDownload_GetItemState(size, size);
        if (data != nullptr && size != 0) {
            std::string dataTmp(reinterpret_cast<const char*>(data), size);
            WebDownload_GetItemStateByGuid(dataTmp);
        }
        WebDownloadItem_Guid(nullptr);
        WebDownloadItem_GetDownloadItemId(nullptr);
        WebDownloadItem_GetState(nullptr);
        WebDownloadItem_CurrentSpeed(nullptr);
        WebDownloadItem_PercentComplete(nullptr);
        WebDownloadItem_TotalBytes(nullptr);
        WebDownloadItem_ReceivedBytes(nullptr);
        WebDownloadItem_FullPath(nullptr);
        WebDownloadItem_Url(nullptr);
        WebDownloadItem_OriginalUrl(nullptr);
        WebDownloadItem_SuggestedFileName(nullptr);
        WebDownloadItem_ContentDisposition(nullptr);
        WebDownloadItem_ETag(nullptr);
        WebDownloadItem_MimeType(nullptr);
        WebDownloadItem_IsPaused(nullptr);
        WebDownloadItem_Method(nullptr);
        WebDownloadItem_LastErrorCode(nullptr);
        WebDownloadItem_ReceivedSlices(nullptr);
        WebDownloadItem_LastModified(nullptr);
        WebDownloadItem_NWebId(nullptr);
        WebDownloadItem_CreateWebDownloadItem(nullptr);
        WebDownloadItem_Destroy(nullptr);
        DestroyBeforeDownloadCallbackWrapper(nullptr);
        DestroyDownloadItemCallbackWrapper(nullptr);
        WebDownloadItem_SetGuid(nullptr, nullptr);
        WebDownloadItem_SetUrl(nullptr, nullptr);
        WebDownloadItem_SetFullPath(nullptr, nullptr);
        WebDownloadItem_SetETag(nullptr, nullptr);
        WebDownloadItem_SetLastModified(nullptr, nullptr);
        WebDownloadItem_SetMimeType(nullptr, nullptr);
        WebDownloadItem_SetReceivedBytes(nullptr, size);
        WebDownloadItem_SetTotalBytes(nullptr, size);
        WebDownloadItem_SetReceivedSlices(nullptr, nullptr);
    }
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::NapiFuzzTest(data, size);
    return 0;
}
