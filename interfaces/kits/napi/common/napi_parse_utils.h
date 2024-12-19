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

#ifndef NAPI_PARSE_UTILS_H
#define NAPI_PARSE_UTILS_H

#include <vector>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "nweb.h"
#include "nweb_web_message.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
constexpr int INTEGER_ZERO = 0;
constexpr int INTEGER_ONE = 1;
constexpr int INTEGER_TWO = 2;
constexpr int INTEGER_THREE = 3;
constexpr int INTEGER_FOUR = 4;
constexpr int INTEGER_FIVE = 5;
constexpr int MAX_CUSTOM_SCHEME_NAME_LENGTH = 32;
constexpr int MAX_CUSTOM_SCHEME_SIZE = 10;
constexpr int MAX_STRING_TO_INT32_LENGTH = 10;

class NapiParseUtils {
public:
    static napi_value CreateEnumConstructor(napi_env env, napi_callback_info info);
    static napi_value ToInt32Value(napi_env env, int32_t number);
    static bool ParseUint32(napi_env env, napi_value argv, uint32_t& outValue);
    static bool ParseInt32(napi_env env, napi_value argv, int32_t& outValue);
    static bool ParseUint64(napi_env env, napi_value argv, uint64_t& outValue, bool *lossless);
    static bool ParseInt64(napi_env env, napi_value argv, int64_t& outValue);
    static bool ParseDouble(napi_env env, napi_value argv, double& outValue);
    static bool ParseString(napi_env env, napi_value argv, std::string& outValue);
    static bool ParseArrayBuffer(napi_env env, napi_value argv, std::string& outValue);
    static bool ParseBoolean(napi_env env, napi_value argv, bool& outValue);
    static bool ParseStringArray(napi_env env, napi_value argv, std::vector<std::string>& outValue);
    static bool ParseBooleanArray(napi_env env, napi_value argv, std::vector<bool>& outValue);
    static bool ParseDoubleArray(napi_env env, napi_value argv, std::vector<double>& outValue);
    static bool ParseInt64Array(napi_env env, napi_value argv, std::vector<int64_t>& outValue);
    static bool ParseFloat(napi_env env, napi_value argv, float& outValue);
    static bool ConvertNWebToNapiValue(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst);
    static ErrCode ConstructStringFlowbuf(napi_env env, napi_value argv, int& fd, size_t& scriptLength);
    static ErrCode ConstructArrayBufFlowbuf(napi_env env, napi_value argv, int& fd, size_t& scriptLength);
    static bool ParseJsLengthStringToInt(const std::string& input, PixelUnit& type, int32_t& value);
};
} // namespace NWeb
} // namespace OHOS
#endif