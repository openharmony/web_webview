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

#include "web_native_messaging_extension_connect_info.h"

#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
namespace{
const int IGNORE_FD = 0;
const int FORCE_FD = 1;
}

bool WNMEConnectionInfoParcel::Marshalling(MessageParcel &out)
{
    if (!(out.WriteInt32(conn_info_.connectionId))) {
        WNMLOG_E("fail write");
        return false;
    }
    if (!(out.WriteString(conn_info_.bundleName))) {
        WNMLOG_E("fail write");
        return false;
    }
    if (!(out.WriteString(conn_info_.extensionOrigin))) {
        WNMLOG_E("fail write");
        return false;
    }
    if ((conn_info_.fdRead == -1) || (conn_info_.fdWrite == -1)) {
        out.WriteInt32(IGNORE_FD);
        return true;
    } else {
        out.WriteInt32(FORCE_FD);
    }
    if (!(out.WriteFileDescriptor(conn_info_.fdRead))) {
        WNMLOG_E("fail write");
        return false;
    }
    if (!(out.WriteFileDescriptor(conn_info_.fdWrite))) {
        WNMLOG_E("fail write");
        return false;
    }
    return true;
}

bool WNMEConnectionInfoParcel::Unmarshalling(MessageParcel& in, WNMEConnectionInfo& conn_info)
{
    if (!(in.ReadInt32(conn_info.connectionId))) {
        WNMLOG_E("fail read");
        return false;
    }
    if (!(in.ReadString(conn_info.bundleName))) {
        WNMLOG_E("fail read");
        return false;
    }
    if (!(in.ReadString(conn_info.extensionOrigin))) {
        WNMLOG_E("fail read");
        return false;
    }
    int ignore = FORCE_FD;
    if (!in.ReadInt32(ignore)) {
        WNMLOG_E("fail read");
        return false;
    }
    if (ignore == IGNORE_FD) {
        conn_info.fdRead = -1;
        conn_info.fdWrite = -1;
        return true;
    }

    conn_info.fdRead = in.ReadFileDescriptor();
    if (conn_info.fdRead < 0) {
        WNMLOG_E("fail read");
        return false;
    }
    conn_info.fdWrite = in.ReadFileDescriptor();
    if (conn_info.fdWrite < 0) {
        WNMLOG_E("fail read");
        return false;
    }
    return true;
}
} // namespace NWeb
} // namespace OHOS
