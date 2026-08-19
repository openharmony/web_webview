/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_AAFWK_BROWSER_CLIENT_ADAPTER_WRAPPER_H
#define ARK_AAFWK_BROWSER_CLIENT_ADAPTER_WRAPPER_H

#include "aafwk_browser_client_adapter.h"
#include "ohos_adapter/include/ark_aafwk_browser_client_adapter.h"

namespace OHOS::ArkWeb {
class ArkAafwkBrowserClientAdapterWrapper : public OHOS::NWeb::AafwkBrowserClientAdapter {
public:
    explicit ArkAafwkBrowserClientAdapterWrapper(ArkWebRefPtr<ArkAafwkBrowserClientAdapter>);

    void* QueryRenderSurface(int32_t surface_id) override;

    void ReportThread(NWeb::ResSchedStatusAdapter status, int32_t process_id, int32_t thread_id,
        NWeb::ResSchedRoleAdapter role) override;

    void PassSurface(int64_t surface_id) override;

    void DestroyRenderSurface(int32_t surface_id) override;

    void DestroyPassedSurface(int64_t surface_id) override;

    ArkWebRefPtr<ArkAafwkBrowserClientAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif
