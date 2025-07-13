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
#include <cstdarg>
#include "securec.h"

namespace {
// error message
const std::string PARAM_CHECK_ERROR_MSG = "Invalid input parameter";
const std::string INIT_ERROR_MSG = "Init error. The WebviewController must be associated with a Web component";
const std::string INVALID_URL_MSG = "Invalid url";
const std::string INVALID_RESOURCE_MSG = "Invalid resource path or file type";
const std::string FUNCTION_NOT_ENABLE_MSG = "Function not enabled.";
const std::string INVALID_COOKIE_VALUE_MSG = "Invalid cookie value";
const std::string CAN_NOT_REGISTER_MESSAGE_EVENT_MSG = "Fail to register a message event for the port.";
const std::string CANNOT_DEL_JAVA_SCRIPT_PROXY_MSG = "Failed to delete JavaScriptProxy because it does not exist.";
const std::string CAN_NOT_POST_MESSAGE_MSG = "Failed to post messages through the port.";
const std::string INVALID_ORIGIN_MSG = "Invalid origin";
const std::string NO_WEBSTORAGE_ORIGIN_MSG = "Invalid web storage origin";
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
const std::string UNKNOWN_ERROR_MSG = "Unknown error message.";
}

namespace OHOS {
namespace ParamCheckErrorMsgTemplate {
    const char* TYPE_ERROR = "BusinessError 401: Parameter error. The type of '%s' must be %s.";
    const char* TYPE_ALL_STRING = "BusinessError 401: Parameter error. The type of params must be string.";
    const char* TYPE_ALL_INT = "BusinessError 401: Parameter error. The type of params must be int.";
    const char* PARAM_TYEPS_ERROR = "BusinessError 401: Parameter error. The type of params is error.";
    const char* PARAM_NUMBERS_ERROR_ONE = "BusinessError 401: Parameter error. The number of params must be %s.";
    const char* PARAM_NUMBERS_ERROR_TWO = "BusinessError 401: Parameter error. The number of params must be %s or %s.";
    const char* PARAM_NUMBERS_ERROR_THREE =
        "BusinessError 401: Parameter error. The number of params must be %s or %s or %s.";
    const char* PARAM_NOT_NULL = "BusinessError 401: Parameter error. The type of '%s' can not be ignored.";
    const char* PARAM_NOT_NULL_TWO =
        "BusinessError 401: Parameter error. The type of '%s' and '%s' can not be ignored.";
    const char* PARAM_TYPE_INVALID = "BusinessError 401: Parameter error. The type of '%s' is invalid.";
    const char* PARAM_DETAIL_ERROR_MSG = "BusinessError 401: Parameter error. detail: %s.";
}
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
};

std::string GetErrMsgByErrCode(ErrCode code)
{
    auto it = g_errCodeMsgMap.find(code);
    if (it != g_errCodeMsgMap.end()) {
        return it->second;
    }
    return UNKNOWN_ERROR_MSG;
}
std::string FormatString(const char *errorMsgTemplate, ...)
{
    char sbuf[256];
    va_list args;
    va_start(args, errorMsgTemplate);
    if (vsnprintf_s(sbuf, sizeof(sbuf), sizeof(sbuf) - 1, errorMsgTemplate, args) < 0) {
        va_end(args);
        return "";
    }
    va_end(args);
    return sbuf;
}
} // namespace NWebError
} // namespace OHOS
