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

#ifndef OHOS_NWEB_WEB_NATIVE_MESSAGING_COMMON_H
#define OHOS_NWEB_WEB_NATIVE_MESSAGING_COMMON_H

#include <string>

namespace OHOS::NWeb {
enum ConnectNativeRet : int32_t {
    SUCCESS = 0,
    PERMISSION_CHECK_ERROR = -1,
    CONTEXT_ERROR = -2,
    WANT_FORMAT_ERROR = -3,
    CONNECTION_NOT_EXIST = -4,
    MEMORY_ERROR = -5,
    CALLBACK_ERROR = -6,
    IPC_ERROR = -7,
    SERVICE_INIT_ERROR = -8,
    CONNECTION_ID_EXIST = -9,
    REQUEST_SIZE_TOO_LARGE = -10,
    CONNECT_STATUS_ERROR = -11,
    ABILITY_CONNECTION_ERROR = -12,
    SERVICE_DIED_ERROR = -13,
    MIN_CN_ERROR = -1000,
};

struct ConnectionNativeInfo {
    int32_t connectionId;
    std::string bundleName;
    std::string extensionOrigin;
    int32_t extensionPid;
};

namespace NativeMessageError {
bool IsNativeMessagingErr(int32_t errCode);
int32_t NativeCodeToJsCode(int32_t ret, std::string& errorMsg);
}

// move it to samgr later
#define SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID 8610
} // namespace OHOS::NWeb
#endif // OHOS_NWEB_WEB_NATIVE_MESSAGING_COMMON_H
