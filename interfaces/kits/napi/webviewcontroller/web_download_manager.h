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

#ifndef NWEB_WEB_DOWNLOAD_MANAGER_H
#define NWEB_WEB_DOWNLOAD_MANAGER_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "web_download_delegate.h"
#include "web_download_item.h"

namespace OHOS {
namespace NWeb {
class WebDownloadDelegate;

class WebDownloadManager {
public:
    static void SetDownloadDelegate(WebDownloadDelegate *delegate);
    static void ResumeDownload(const WebDownloadItem *webDownload);

    static void AddDownloadDelegateForWeb(int32_t nwebId, WebDownloadDelegate *delegate);
    static void RemoveDownloadDelegate(WebDownloadDelegate *delegate);
    static void RemoveDownloadDelegateRef(int32_t nwebId);

    static bool HasValidDelegate();

    WebDownloadManager() = default;
    ~WebDownloadManager() = default;

private:
    static void RegisterDownloadCallback();
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEB_DOWNLOAD_MANAGER_H