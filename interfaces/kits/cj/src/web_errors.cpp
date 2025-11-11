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

#include "web_errors.h"

#include <string>
#include <unordered_map>

namespace {
// error message
const std::string PARAM_CHECK_ERROR_MSG = "Invalid input parameter";
const std::string INIT_ERROR_MSG = "Init error. The WebviewController must be associated with a Web component";
const std::string INVALID_URL_MSG = "URL error. Possible causes:1.No valid cookie found for the specified URL. "
    "2.The webpage corresponding to the URL is invalid, or the URL length exceeds 2*1024*1024.";
const std::string INVALID_RESOURCE_MSG = "Invalid resource path or file type";
const std::string FUNCTION_NOT_ENABLE_MSG = "Function not enabled.";
const std::string INVALID_COOKIE_VALUE_MSG = "The provided cookie value is invalid. It must follow the format "
    "specified in RFC 6265.";
const std::string CAN_NOT_REGISTER_MESSAGE_EVENT_MSG = "Fail to register a message event for the port.";
const std::string CANNOT_DEL_JAVA_SCRIPT_PROXY_MSG = "Failed to delete JavaScriptProxy because it does not exist.";
const std::string CAN_NOT_POST_MESSAGE_MSG = "Failed to post messages through the port.";
const std::string INVALID_ORIGIN_MSG = "Invalid origin.The origin format must follow defined in RFC 6454.";
const std::string NO_WEBSTORAGE_ORIGIN_MSG = "Invalid web storage origin.The web storage origin is empty.";
const std::string INVALID_SOCKET_NUMBER_MSG = "The number of sockets to be preconnected is invalid.";
const std::string TYPE_NOT_MATCH_WITCH_VALUE_MSG = "The type and value of the message do not match.";
const std::string NEW_OOM_MSG = "Memory allocation failed.";
const std::string DOWNLOAD_NOT_PAUSED_MSG = "The download task is not paused.";
const std::string NO_VALID_CONTROLLER_FOR_DOWNLOAD_MSG = "No valid WebviewController is associated.";
const std::string NO_DOWNLOAD_DELEGATE_SET_MSG = "No WebDownloadDelegate has been set yet.";
const std::string DOWNLOAD_NOT_START_MSG = "The download task is not started yet.";
const std::string REGISTER_CUSTOM_SCHEME_FAILED_MSG = "Failed to register custom schemes.";
const std::string HTTP_BODY_STREAN_INIT_FAILED_MSG = "Failed to initialize the HTTP body stream.";
const std::string RESOURCE_HANDLER_INVALID_MSG = "The resource handler is invalid.";
const std::string HTTP_AUTH_MALLOC_FAILED_MSG = "Failed to malloc string memory to get HttpAuth.";
const std::string UNKNOWN_ERROR_MSG = "Unknown error message.";
}

namespace OHOS {
namespace NWebError {
std::unordered_map<ErrCode, std::string> g_errCodeMsgMap = {
    {PARAM_CHECK_ERROR, PARAM_CHECK_ERROR_MSG},
    {INIT_ERROR, INIT_ERROR_MSG},
    {INVALID_URL, INVALID_URL_MSG},
    {INVALID_RESOURCE, INVALID_RESOURCE_MSG},
    {FUNCTION_NOT_ENABLE, FUNCTION_NOT_ENABLE_MSG},
    {INVALID_COOKIE_VALUE, INVALID_COOKIE_VALUE_MSG},
    {CAN_NOT_REGISTER_MESSAGE_EVENT, CAN_NOT_REGISTER_MESSAGE_EVENT_MSG},
    {CANNOT_DEL_JAVA_SCRIPT_PROXY, CANNOT_DEL_JAVA_SCRIPT_PROXY_MSG},
    {CAN_NOT_POST_MESSAGE, CAN_NOT_POST_MESSAGE_MSG},
    {INVALID_ORIGIN, INVALID_ORIGIN_MSG},
    {NO_WEBSTORAGE_ORIGIN, NO_WEBSTORAGE_ORIGIN_MSG},
    {INVALID_SOCKET_NUMBER, INVALID_SOCKET_NUMBER_MSG},
    {TYPE_NOT_MATCH_WITCH_VALUE, TYPE_NOT_MATCH_WITCH_VALUE_MSG},
    {NEW_OOM, NEW_OOM_MSG},
    {DOWNLOAD_NOT_PAUSED, DOWNLOAD_NOT_PAUSED_MSG},
    {NO_VALID_CONTROLLER_FOR_DOWNLOAD, NO_VALID_CONTROLLER_FOR_DOWNLOAD_MSG},
    {NO_DOWNLOAD_DELEGATE_SET, NO_DOWNLOAD_DELEGATE_SET_MSG},
    {DOWNLOAD_NOT_START, DOWNLOAD_NOT_START_MSG},
    {REGISTER_CUSTOM_SCHEME_FAILED, REGISTER_CUSTOM_SCHEME_FAILED_MSG},
    {HTTP_BODY_STREAN_INIT_FAILED, HTTP_BODY_STREAN_INIT_FAILED_MSG},
    {RESOURCE_HANDLER_INVALID, RESOURCE_HANDLER_INVALID_MSG},
    {HTTP_AUTH_MALLOC_FAILED, HTTP_AUTH_MALLOC_FAILED_MSG},
};

std::string GetErrMsgByErrCode(ErrCode code)
{
    auto it = g_errCodeMsgMap.find(code);
    if (it != g_errCodeMsgMap.end()) {
        return it->second;
    }
    return UNKNOWN_ERROR_MSG;
}
} // namespace NWebError
} // namespace OHOS
