/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_ANI_WEB_DOWNLOAD_DELEGATE_H
#define OHOS_NWEB_ANI_WEB_DOWNLOAD_DELEGATE_H

#include <string>

#include "web_download_item.h"

namespace OHOS {
namespace NWeb {
class WebDownloadDelegate {
public:
    static WebDownloadDelegate *FromNWebID(int32_t nwebId);

    explicit WebDownloadDelegate(ani_env* env);
    ~WebDownloadDelegate();

    void DownloadBeforeStart(WebDownloadItem *webDownloadItem);
    void DownloadDidUpdate(WebDownloadItem *webDownloadItem);
    void DownloadDidFail(WebDownloadItem *webDownloadItem);
    void DownloadDidFinish(WebDownloadItem *webDownloadItem);

    void PutDownloadBeforeStart(ani_fn_object callback);
    void PutDownloadDidUpdate(ani_fn_object callback);
    void PutDownloadDidFinish(ani_fn_object callback);
    void PutDownloadDidFail(ani_fn_object callback);

    int32_t GetNWebId() const;
    void SetNWebId(int32_t nwebId);
    ani_env* GetEnv();

    void RemoveSelfRef();

    //  create this reference to make web download delegate not be deleted by gc.
    ani_ref delegate_;

private:
    int32_t nwebId_ = -1;

    ani_ref download_before_start_callback_;
    ani_ref download_did_update_callback_;
    ani_ref download_did_finish_callback_;
    ani_ref download_did_fail_callback_;

    ani_env* env_ = nullptr;
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEB_DOWNLOAD_DELEGATE_H