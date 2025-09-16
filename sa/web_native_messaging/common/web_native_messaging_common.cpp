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

 #include "web_native_messaging_common.h"

namespace OHOS::NWeb::NativeMessageError {
namespace {
constexpr int32_t JS_PERMISSION_ERROR = 201;
constexpr int32_t JS_CONTEXT_NOT_EXIST = 16000011;
constexpr int32_t JS_INTERNAL_ERROR = 16000050;
const std::string JS_PERMISSION_ERROR_MSG = "Permission denied.";
const std::string JS_CONTEXT_NOT_EXIST_MSG = "This connection is not exist.";
const std::string JS_INTERNAL_ERROR_MSG = "Internal error, code is ";
}

bool IsNativeMessagingErr(int32_t errCode)
{
    return (errCode > MIN_CN_ERROR && errCode <= 0);
}

int32_t NativeCodeToJsCode(int32_t ret, std::string& errorMsg)
{
    int32_t errorCode;
    switch (ret) {
        case ConnectNativeRet::PERMISSION_CHECK_ERROR:
            errorCode = JS_PERMISSION_ERROR;
            errorMsg = JS_PERMISSION_ERROR_MSG;
            break;
        case ConnectNativeRet::CONNECTION_NOT_EXIST:
            errorCode = JS_CONTEXT_NOT_EXIST;
            errorMsg = JS_CONTEXT_NOT_EXIST_MSG;
            break;
        default:
            errorCode = JS_INTERNAL_ERROR;
            errorMsg = JS_INTERNAL_ERROR_MSG + std::to_string(ret);
            break;
    }
    return errorCode;
}
} // namespace OHOS::NWeb::NativeMessageError
