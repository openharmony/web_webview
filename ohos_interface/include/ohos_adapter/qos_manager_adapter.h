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

#ifndef QOS_MANAGER_ADAPTER_H
#define QOS_MANAGER_ADAPTER_H

#include <cstdint>

namespace OHOS::NWeb {
enum class QosLevelAdapter : int {
    /**
     * @brief Means the QoS level is background.
     */
    NWEB_QOS_BACKGROUND = 0,

    /**
     * @brief Means the QoS level is utility.
     */
    NWEB_QOS_UTILITY,

    /**
     * @brief Means the QoS level is default.
     */
    NWEB_QOS_DEFAULT,

    /**
     * @brief Means the QoS level is user-initiated.
     */
    NWEB_QOS_USER_INITIATED,

    /**
     * @brief Means the QoS level is user-request.
     */
    NWEB_QOS_DEADLINE_REQUEST,

    /**
     * @brief Means the QoS level is user-interactive.
     */
    NWEB_QOS_USER_INTERACTIVE,
};

class QosManagerAdapter {
public:
    QosManagerAdapter() = default;

    virtual ~QosManagerAdapter() = default;

    virtual int32_t SetThreadQoS(QosLevelAdapter level) = 0;

    virtual int32_t GetThreadQoS(QosLevelAdapter *level) = 0;
};

} // namespace OHOS::NWeb

#endif // QOS_MANAGER_ADAPTER_H
