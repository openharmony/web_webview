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
constexpr ErrCode CANNOT_ZOOM_IN_OR_ZOOM_OUT = 17100009;
constexpr ErrCode CAN_NOT_POST_MESSAGE = 17100010;
constexpr ErrCode INVALID_ORIGIN = 17100011;
constexpr ErrCode NO_WEBSTORAGE_ORIGIN = 17100012;
constexpr ErrCode NEW_OOM = 17100013;

std::string GetErrMsgByErrCode(ErrCode code);
}
}
#endif