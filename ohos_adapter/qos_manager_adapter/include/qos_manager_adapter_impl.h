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

#ifndef QOS_MANAGER_ADAPTER_IMPL_H
#define QOS_MANAGER_ADAPTER_IMPL_H

#include "qos_manager_adapter.h"

namespace OHOS::NWeb {

class QosManagerAdapterImpl : public QosManagerAdapter {
public:
    static QosManagerAdapterImpl& GetInstance();

    ~QosManagerAdapterImpl() override = default;

    int32_t SetThreadQoS(QosLevelAdapter level) override;

    int32_t GetThreadQoS(QosLevelAdapter *level) override;

private:
    QosManagerAdapterImpl() = default;

    QosManagerAdapterImpl(const QosManagerAdapterImpl& other) = delete;

    QosManagerAdapterImpl& operator=(const QosManagerAdapterImpl&) = delete;
};

}  // namespace OHOS::NWeb

#endif  // QOS_MANAGER_ADAPTER_IMPL_H
