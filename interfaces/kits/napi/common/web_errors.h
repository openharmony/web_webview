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

#ifndef WEB_ERRORS_H
#define WEB_ERRORS_H

#include "errors.h"

#include <string>

namespace OHOS {
namespace ParamCheckErrorMsgTemplate {
   extern const char* TYPE_ERROR;
   extern const char* TYPE_ALL_STRING;
   extern const char* TYPE_ALL_INT;
   extern const char* PARAM_TYEPS_ERROR;
   extern const char* PARAM_NUMBERS_ERROR_ONE;
   extern const char* PARAM_NUMBERS_ERROR_TWO;
   extern const char* PARAM_NUMBERS_ERROR_THREE;
   extern const char* PARAM_NOT_NULL;
   extern const char* PARAM_NOT_NULL_TWO;
   extern const char* PARAM_TYPE_INVALID;
}
namespace NWebError {
constexpr ErrCode NWEB_ERROR = -1;
constexpr ErrCode NO_ERROR = 0;

constexpr ErrCode PARAM_CHECK_ERROR = 401;
constexpr ErrCode INIT_ERROR = 17100001;
constexpr ErrCode INVALID_URL = 17100002;
constexpr ErrCode INVALID_RESOURCE = 17100003;
constexpr ErrCode FUNCTION_NOT_ENABLE = 17100004;
constexpr ErrCode INVALID_COOKIE_VALUE = 17100005;
constexpr ErrCode CAN_NOT_REGISTER_MESSAGE_EVENT = 17100006;
constexpr ErrCode CANNOT_DEL_JAVA_SCRIPT_PROXY = 17100008;
constexpr ErrCode CAN_NOT_POST_MESSAGE = 17100010;
constexpr ErrCode INVALID_ORIGIN = 17100011;
constexpr ErrCode NO_WEBSTORAGE_ORIGIN = 17100012;
constexpr ErrCode INVALID_SOCKET_NUMBER = 17100013;
constexpr ErrCode TYPE_NOT_MATCH_WITCH_VALUE = 17100014;
constexpr ErrCode NEW_OOM = 17100015;
constexpr ErrCode DOWNLOAD_NOT_PAUSED = 17100016;
constexpr ErrCode NO_VALID_CONTROLLER_FOR_DOWNLOAD = 17100017;
constexpr ErrCode NO_DOWNLOAD_DELEGATE_SET = 17100018;
constexpr ErrCode DOWNLOAD_NOT_START = 17100019;
constexpr ErrCode REGISTER_CUSTOM_SCHEME_FAILED = 17100020;
constexpr ErrCode RESOURCE_HANDLER_INVALID = 17100021;
constexpr ErrCode HTTP_BODY_STREAN_INIT_FAILED = 17100022;

std::string GetErrMsgByErrCode(ErrCode code);
std::string FormatString(const char *errorMsgTemplate, ...);
}
}
#endif
