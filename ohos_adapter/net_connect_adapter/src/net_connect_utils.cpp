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

#include "net_connect_utils.h"

namespace OHOS::NWeb {
NetConnectType NetConnectUtils::ConvertToConnectTypeInner(const Telephony_RadioTechnology &subtype)
{
    switch (subtype) {
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_IWLAN:
            return NetConnectType::CONNECTION_UNKNOWN;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_GSM:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_1XRTT:
            return NetConnectType::CONNECTION_2G;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_WCDMA:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPA:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPAP:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_TD_SCDMA:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EVDO:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EHRPD:
            return NetConnectType::CONNECTION_3G;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE_CA:
            return NetConnectType::CONNECTION_4G;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_NR:
            return NetConnectType::CONNECTION_5G;
        default:
            return NetConnectType::CONNECTION_UNKNOWN;
    }
}

NetConnectSubtype NetConnectUtils::ConvertToConnectsubtype(const Telephony_RadioTechnology &subtype)
{
    switch (subtype) {
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_IWLAN:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_WCDMA:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_TD_SCDMA:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EVDO:
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_NR:
            return NetConnectSubtype::SUBTYPE_UNKNOWN;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_GSM:
            return NetConnectSubtype::SUBTYPE_GSM;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_1XRTT:
            return NetConnectSubtype::SUBTYPE_1XRTT;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPA:
            return NetConnectSubtype::SUBTYPE_HSPA;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_HSPAP:
            return NetConnectSubtype::SUBTYPE_HSPAP;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_EHRPD:
            return NetConnectSubtype::SUBTYPE_EHRPD;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE:
            return NetConnectSubtype::SUBTYPE_LTE;
        case Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_LTE_CA:
            return NetConnectSubtype::SUBTYPE_LTE_ADVANCED;
        default:
            return NetConnectSubtype::SUBTYPE_UNKNOWN;
    }
}

NetConnectType NetConnectUtils::ConvertToConnectType(const NetConn_NetBearerType &netBearType,
                                                     const Telephony_RadioTechnology &subtype)
{
    switch (netBearType) {
        case NETCONN_BEARER_CELLULAR:
            return ConvertToConnectTypeInner(subtype);
        case NETCONN_BEARER_WIFI:
            return NetConnectType::CONNECTION_WIFI;
        case NETCONN_BEARER_BLUETOOTH:
            return NetConnectType::CONNECTION_BLUETOOTH;
        case NETCONN_BEARER_ETHERNET:
            return NetConnectType::CONNECTION_ETHERNET;
        case NETCONN_BEARER_VPN:
            return NetConnectType::CONNECTION_UNKNOWN;
        default:
            return NetConnectType::CONNECTION_UNKNOWN;
    }
}

std::string NetConnectUtils::ConnectTypeToString(const NetConnectType &type)
{
    switch (type) {
        case NetConnectType::CONNECTION_UNKNOWN:
            return "unknown";
        case NetConnectType::CONNECTION_ETHERNET:
            return "ethernet";
        case NetConnectType::CONNECTION_WIFI:
            return "wifi";
        case NetConnectType::CONNECTION_2G:
            return "2g";
        case NetConnectType::CONNECTION_3G:
            return "3g";
        case NetConnectType::CONNECTION_4G:
            return "4g";
        case NetConnectType::CONNECTION_NONE:
            return "none";
        case NetConnectType::CONNECTION_BLUETOOTH:
            return "bluetooth";
        case NetConnectType::CONNECTION_5G:
            return "5g";
    }
}
}  // namespace OHOS::NWeb