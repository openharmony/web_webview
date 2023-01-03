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

#ifndef OHOS_INIT_WEB_ADAPTER_IMPL_H
#define OHOS_INIT_WEB_ADAPTER_IMPL_H

#include "ohos_init_web_adapter.h"

namespace OHOS::NWeb {
class OhosInitWebAdapterImpl : public OhosInitWebAdapter {
public:
    OhosInitWebAdapterImpl() = default;

    ~OhosInitWebAdapterImpl() override = default;

    WebRunInitedCallback* GetRunWebInitedCallback() override;

    void SetRunWebInitedCallback(WebRunInitedCallback* &&callback) override;

    static WebRunInitedCallback* webRunInitedCallback_;
};
}  // namespace OHOS::NWeb

#endif  // OHOS_INIT_WEB_ADAPTER_IMPL_H