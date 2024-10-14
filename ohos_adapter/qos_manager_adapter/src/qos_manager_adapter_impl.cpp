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

#include "qos_manager_adapter_impl.h"
#include <map>
#include "qos/qos.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

std::map<QosLevelAdapter, QoS_Level> kQosLevelAdapterToQoS_LevelMap =
{
    {QosLevelAdapter::NWEB_QOS_BACKGROUND, QoS_Level::QOS_BACKGROUND},
    {QosLevelAdapter::NWEB_QOS_UTILITY, QoS_Level::QOS_UTILITY},
    {QosLevelAdapter::NWEB_QOS_DEFAULT, QoS_Level::QOS_DEFAULT},
    {QosLevelAdapter::NWEB_QOS_USER_INITIATED, QoS_Level::QOS_USER_INITIATED},
    {QosLevelAdapter::NWEB_QOS_DEADLINE_REQUEST, QoS_Level::QOS_DEADLINE_REQUEST},
    {QosLevelAdapter::NWEB_QOS_USER_INTERACTIVE, QoS_Level::QOS_USER_INTERACTIVE},
};

QoS_Level QosLevelAdapterToQosLevel(QosLevelAdapter nwebLevelAdapter)
{
    if (kQosLevelAdapterToQoS_LevelMap.count(nwebLevelAdapter) > 0) {
        return kQosLevelAdapterToQoS_LevelMap[nwebLevelAdapter];
    }
    return QoS_Level::QOS_DEFAULT;
}

QosLevelAdapter QosLevelToQosLevelAdapter(QoS_Level qosLevel)
{
    for (const auto& pair : kQosLevelAdapterToQoS_LevelMap) {
        if (pair.second == qosLevel) {
            return pair.first;
        }
    }
    return QosLevelAdapter::NWEB_QOS_DEFAULT;
}

// static
QosManagerAdapterImpl& QosManagerAdapterImpl::GetInstance()
{
    static QosManagerAdapterImpl instance;
    return instance;
}

int32_t QosManagerAdapterImpl::SetThreadQoS(QosLevelAdapter level)
{
    QoS_Level qosLevel = QosLevelAdapterToQosLevel(level);
    int32_t ret = OH_QoS_SetThreadQoS(qosLevel);
    if (ret < 0) {
        WVLOG_D("Set thread qos failed, ret: %{public}d", ret);
    }
    return ret;
}

int32_t QosManagerAdapterImpl::GetThreadQoS(QosLevelAdapter *level)
{
    QoS_Level qosLevel = QoS_Level::QOS_DEFAULT;
    int32_t ret = OH_QoS_GetThreadQoS(&qosLevel);
    if (ret < 0) {
        WVLOG_D("Get thread qos failed, ret: %{public}d", ret);
    }
    *level = QosLevelToQosLevelAdapter(qosLevel);
    return ret;
}
} // namespace OHOS::NWeb
