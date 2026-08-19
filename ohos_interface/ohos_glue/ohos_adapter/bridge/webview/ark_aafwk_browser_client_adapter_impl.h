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

#ifndef ARK_AAFWK_BROWSER_CLIENT_ADAPTER_IMPL_H
#define ARK_AAFWK_BROWSER_CLIENT_ADAPTER_IMPL_H
#pragma once

#include "aafwk_browser_client_adapter.h"
#include "ohos_adapter/include/ark_aafwk_browser_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkAafwkBrowserClientAdapterImpl : public ArkAafwkBrowserClientAdapter {
public:
    explicit ArkAafwkBrowserClientAdapterImpl(std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter>);

    void* QueryRenderSurface(int32_t surface_id) override;

    void ReportThread(int32_t status, int32_t process_id, int32_t thread_id, int32_t role) override;

    void PassSurface(int64_t surface_id) override;

    void DestroyRenderSurface(int32_t surface_id) override;

    void DestroyPassedSurface(int64_t surface_id) override;

private:
    std::shared_ptr<OHOS::NWeb::AafwkBrowserClientAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkAafwkBrowserClientAdapterImpl);
};
} // namespace OHOS::ArkWeb

#endif // ARK_AAFWK_BROWSER_CLIENT_ADAPTER_IMPL_H
