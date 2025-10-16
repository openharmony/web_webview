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

#ifndef WEB_NATIVE_MESSAGING_EXTENSION_CONNECT_INFO_H
#define WEB_NATIVE_MESSAGING_EXTENSION_CONNECT_INFO_H

#include <string>
#include <cstdint>
#include "message_parcel.h"

namespace OHOS {
namespace NWeb {
struct WNMEConnectionInfo {
    int32_t connectionId = -1;
    std::string bundleName = "";
    std::string extensionOrigin = "";
    int32_t fdRead = -1;
    int32_t fdWrite = -1;
};

struct WNMEConnectionInfoParcel {
    bool Marshalling(MessageParcel& parcel);
    static bool Unmarshalling(MessageParcel& in, WNMEConnectionInfo& conn_info);

    WNMEConnectionInfo conn_info_;
};
} // namespace NWeb
} // namespace OHOS
#endif // WEB_NATIVE_MESSAGING_EXTENSION_CONNECT_INFO_H