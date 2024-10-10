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

#ifndef NWEB_WEB_DOWNLOAD_DELEGATE_H
#define NWEB_WEB_DOWNLOAD_DELEGATE_H

#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "web_download_item.h"

namespace OHOS {
namespace NWeb {
class WebDownloadDelegate {
public:
    static WebDownloadDelegate *FromNWebID(int32_t nwebId);

    explicit WebDownloadDelegate(napi_env env);
    ~WebDownloadDelegate();

    void DownloadBeforeStart(WebDownloadItem *webDownloadItem);
    void DownloadDidUpdate(WebDownloadItem *webDownloadItem);
    void DownloadDidFail(WebDownloadItem *webDownloadItem);
    void DownloadDidFinish(WebDownloadItem *webDownloadItem);

    void PutDownloadBeforeStart(napi_env, napi_value callback);
    void PutDownloadDidUpdate(napi_env, napi_value callback);
    void PutDownloadDidFinish(napi_env, napi_value callback);
    void PutDownloadDidFail(napi_env, napi_value callback);

    int32_t GetNWebId() const;
    void SetNWebId(int32_t nwebId);
    napi_env GetEnv();

    void RemoveSelfRef();

    //  create this reference to make web download delegate not be deleted by gc.
    napi_ref delegate_;

private:
    int32_t nwebId_ = -1;

    napi_ref download_before_start_callback_;
    napi_ref download_did_update_callback_;
    napi_ref download_did_finish_callback_;
    napi_ref download_did_fail_callback_;
    napi_env env_;
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEB_DOWNLOAD_DELEGATE_H