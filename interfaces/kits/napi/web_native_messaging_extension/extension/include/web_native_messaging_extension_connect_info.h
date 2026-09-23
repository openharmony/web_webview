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
#include <stdio.h>
#include "message_parcel.h"

namespace OHOS {
namespace NWeb {
// fd sanitizer owner tag，SA 侧与扩展进程侧统一使用该 tag 管理 WNME 链路的 fd 所有权。
// 等价于 fdsan_create_owner_tag(FDSAN_OWNER_TYPE_FILE, 0xD004500)（OH musl 公式为
// (type << 56) | tag）：type 占高 8 位便于 fdsan 日志识别为 FILE*，value 对齐 WNM
// hilog domain（LOG_WEB_NATIVE_MESSAGING_DOMAIN 0xD004500）。
// fdsan_create_owner_tag 是 libc 运行时函数，无法在 constexpr 中调用，故按公式编译期计算。
constexpr uint64_t WNM_FD_OWNER_TAG =
    (static_cast<uint64_t>(FDSAN_OWNER_TYPE_FILE) << 56) | 0xD004500;

// 接收 IPC 传递的 fd 后调用，接管该 fd 的 fdsan 所有权（跨进程接收的 fd 在本进程内无 tag）。
inline void WnmExchangeFdOwnerTag(int fd)
{
    if (fd >= 0) {
        fdsan_exchange_owner_tag(fd, 0, WNM_FD_OWNER_TAG);
    }
}

// 以 WNM_FD_OWNER_TAG 关闭 fd（fdsan 校验所有权，拦截 double-close 与误关他人 fd），并置 -1 防止二次关闭。
inline void WnmCloseFdWithTag(int& fd)
{
    if (fd >= 0) {
        fdsan_close_with_tag(fd, WNM_FD_OWNER_TAG);
        fd = -1;
    }
}

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