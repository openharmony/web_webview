/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "connection_native_info_parcel.h"

#include "securec.h"
#include "web_native_messaging_log.h"

namespace OHOS::NWeb {
bool ConnectionNativeInfoParcel::Marshalling(Parcel& out) const
{
    if (!(out.WriteInt32(connectionNativeInfo_.connectionId)) ||
        !(out.WriteString(connectionNativeInfo_.bundleName)) ||
        !(out.WriteString(connectionNativeInfo_.extensionOrigin)) ||
        !(out.WriteInt32(connectionNativeInfo_.extensionPid))) {
        WNMLOG_E("Write connect native info fail");
        return false;
    }
    return true;
}

ConnectionNativeInfoParcel* ConnectionNativeInfoParcel::Unmarshalling(Parcel& in)
{
    ConnectionNativeInfoParcel* infoParcel = new (std::nothrow) ConnectionNativeInfoParcel();
    if (infoParcel == nullptr) {
        return nullptr;
    }

    ConnectionNativeInfo info = {};
    int32_t connectionId;
    if (!in.ReadInt32(connectionId)) {
        WNMLOG_E("connectionId is invalid");
        delete infoParcel;
        return nullptr;
    }
    info.connectionId = connectionId;

    std::string bundleName = in.ReadString();
    if (bundleName.empty()) {
        WNMLOG_E("bundleName is invalid");
        delete infoParcel;
        return nullptr;
    }
    info.bundleName = bundleName;

    std::string extensionOrigin = in.ReadString();
    if (extensionOrigin.empty()) {
        WNMLOG_E("extensionOrigin is invalid");
        delete infoParcel;
        return nullptr;
    }
    info.extensionOrigin = extensionOrigin;

    int32_t extensionPid;
    if (!in.ReadInt32(extensionPid)) {
        WNMLOG_E("extensionPid is invalid");
        delete infoParcel;
        return nullptr;
    }
    info.extensionPid = extensionPid;
    infoParcel->connectionNativeInfo_ = info;
    return infoParcel;
}
} // namespace OHOS::NWeb
