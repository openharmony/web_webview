/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_HIVIEWDFX_ADAPTER_WRAPPER_H
#define ARK_HIVIEWDFX_ADAPTER_WRAPPER_H
#pragma once

#include "hiviewdfx_adapter.h"
#include "ohos_adapter/include/ark_hiviewdfx_adapter.h"

namespace OHOS::ArkWeb {

class ArkHiViewDfxAdapterWrapper : public OHOS::NWeb::HiViewDfxAdapter {
public:
    explicit ArkHiViewDfxAdapterWrapper(ArkWebRefPtr<ArkHiViewDfxAdapter>);

    void DfxSetSubmitterStackId(uint64_t ctx) override;

    void DfxPopSubmitterStackId(uint64_t ctx) override;

    void ReleaseAsyncContext(uint64_t ctx) override;

    uint64_t DfxCollectAsyncStack() override;

private:
    ArkWebRefPtr<ArkHiViewDfxAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_HIVIEWDFX_ADAPTER_WRAPPER_H
