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
#include "qos/qos.h"

#include "nweb_log.h"

namespace OHOS::NWeb {

QoS_Level ConvertQosLevel(QosLevelAdapter level)
{
    switch (level) {
        case QosLevelAdapter::NWEB_QOS_BACKGROUND:
            return QoS_Level::QOS_BACKGROUND;
        case QosLevelAdapter::NWEB_QOS_UTILITY:
            return QoS_Level::QOS_UTILITY;
        case QosLevelAdapter::NWEB_QOS_DEFAULT:
            return QoS_Level::QOS_DEFAULT;
        case QosLevelAdapter::NWEB_QOS_USER_INITIATED:
            return QoS_Level::QOS_USER_INITIATED;
        case QosLevelAdapter::NWEB_QOS_DEADLINE_REQUEST:
            return QoS_Level::QOS_DEADLINE_REQUEST;
        case QosLevelAdapter::NWEB_QOS_USER_INTERACTIVE:
            return QoS_Level::QOS_USER_INTERACTIVE;
        default:
            return QoS_Level::QOS_DEFAULT;
    }
}

// static
QosManagerAdapterImpl& QosManagerAdapterImpl::GetInstance()
{
    static QosManagerAdapterImpl instance;
    return instance;
}

int32_t QosManagerAdapterImpl::SetThreadQoS(QosLevelAdapter level)
{
    QoS_Level qosLevel = ConvertQosLevel(level);
    int32_t ret = OH_QoS_SetThreadQoS(qosLevel);
    if (ret < 0) {
        WVLOG_D("Set thread qos failed, ret: %{public}d", ret);
    }
    return ret;
}

int32_t QosManagerAdapterImpl::GetThreadQoS(QosLevelAdapter *level)
{
    int32_t ret = OH_QoS_GetThreadQoS(level);
    if (ret < 0) {
        WVLOG_D("Get thread qos failed, ret: %{public}d", ret);
    }
    return ret;
}
} // namespace OHOS::NWeb
