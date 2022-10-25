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

#include "napi_parse_utils.h"

namespace {
constexpr int MAX_STRING_LENGTH = 40960;
}

namespace OHOS {
namespace NWeb {
napi_value NapiParseUtils::CreateEnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value arg = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &arg, nullptr);
    return arg;
}

napi_value NapiParseUtils::ToInt32Value(napi_env env, int32_t number)
{
    napi_value result = nullptr;
    napi_create_int32(env, number, &result);
    return result;
}

bool NapiParseUtils::ParseInt32(napi_env env, napi_value argv, int32_t& outValue)
{
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_number) {
        return false;
    }

    int32_t number = 0;
    napi_get_value_int32(env, argv, &number);
    outValue = number;

    return true;
}

bool NapiParseUtils::ParseString(napi_env env, napi_value argv, std::string& outValue)
{
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        return false;
    }
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    if (bufferSize > MAX_STRING_LENGTH) {
        return false;
    }
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, stringValue, bufferSize + 1, &jsStringLength);
    if (jsStringLength != bufferSize) {
        return false;
    }
    outValue = stringValue;
    return true;
}

bool NapiParseUtils::ParseBoolean(napi_env env, napi_value argv, bool& outValue)
{
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_boolean) {
        return false;
    }

    bool boolValue;
    napi_get_value_bool(env, argv, &boolValue);
    outValue = boolValue;
    return true;
}

bool NapiParseUtils::ParseStringArray(napi_env env, napi_value argv, std::vector<std::string>& outValue)
{
    bool isArray = false;
    napi_is_array(env, argv, &isArray);
    if (!isArray) {
        return false;
    }
    
    uint32_t arrLen = 0;
    napi_get_array_length(env, argv, &arrLen);
    for (uint32_t i = 0; i < arrLen; ++i) {
        napi_value item = nullptr;
        napi_get_element(env, argv, i, &item);

        std::string str;
        if (ParseString(env, item, str)) {
            outValue.push_back(str);
        }
    }

    return true;
}

bool NapiParseUtils::ParseFloat(napi_env env, napi_value argv, float& outValue)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_number) {
        return false;
    }

    double value;
    napi_get_value_double(env, argv, &value);
    outValue = static_cast<float>(value);
    return true;
}
} // namespace NWeb
} // namespace OHOS