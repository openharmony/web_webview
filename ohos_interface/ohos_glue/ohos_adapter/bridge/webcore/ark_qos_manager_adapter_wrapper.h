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

#ifndef ARK_QOS_MANAGER_ADAPTER_WRAPPER_H
#define ARK_QOS_MANAGER_ADAPTER_WRAPPER_H
#pragma once

#include "adapter_base.h"
#include "ohos_adapter/include/ark_qos_manager_adapter.h"
#include "qos_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkQosManagerAdapterWrapper : public OHOS::NWeb::QosManagerAdapter {
public:
    explicit ArkQosManagerAdapterWrapper(ArkWebRefPtr<ArkQosManagerAdapter>);

    int32_t SetThreadQoS(NWeb::QosLevelAdapter level) override;

    int32_t GetThreadQoS(NWeb::QosLevelAdapter *level) override;

private:
    ArkWebRefPtr<ArkQosManagerAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_QOS_MANAGER_ADAPTER_WRAPPER_H
