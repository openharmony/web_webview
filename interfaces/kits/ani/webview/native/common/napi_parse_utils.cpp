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

#include <sys/mman.h>
#include <unistd.h>
#include <regex>

#include "nweb.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "securec.h"

#define MAX_FLOWBUF_DATA_SIZE 52428800 /* 50 MB */

namespace OHOS {
namespace NWeb {
namespace {
bool ConvertToNapiHandlerOfString(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::string msgStr = src->GetString();
    napi_create_string_utf8(env, msgStr.c_str(), msgStr.length(), &dst);
    return true;
}

bool ConvertToNapiHandlerOfBinary(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::vector<uint8_t> msgArr = src->GetBinary();
    void *arrayData = nullptr;
    napi_status status = napi_create_arraybuffer(env, msgArr.size(), &arrayData, &dst);
    if (status != napi_ok) {
        WVLOG_E("Create arraybuffer failed");
        return false;
    }
    for (size_t i = 0; i < msgArr.size(); ++i) {
        *(uint8_t*)((uint8_t*)arrayData + i) = msgArr[i];
    }
    return true;
}

bool ConvertToNapiHandlerOfBoolean(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    bool value = src->GetBoolean();
    napi_get_boolean(env, value, &dst);
    return true;
}

bool ConvertToNapiHandlerOfInteger(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    int64_t value = src->GetInt64();
    napi_create_int64(env, value, &dst);
    return true;
}

bool ConvertToNapiHandlerOfDouble(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    double value = src->GetDouble();
    napi_create_double(env, value, &dst);
    return true;
}

bool ConvertToNapiHandlerOfError(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::string errorName = src->GetErrName();
    std::string errorMsg = src->GetErrName() + ": " + src->GetErrMsg();
    napi_value name = nullptr;
    napi_value message = nullptr;
    napi_create_string_utf8(env, errorName.c_str(), errorName.length(), &name);
    napi_create_string_utf8(env, errorMsg.c_str(), errorMsg.length(), &message);
    napi_create_error(env, name, message, &dst);
    return true;
}

bool ConvertToNapiHandlerOfStringArray(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::vector<std::string> values = src->GetStringArray();
    napi_create_array(env, &dst);
    bool isArray = false;
    if (napi_is_array(env, dst, &isArray) != napi_ok || !isArray) {
        WVLOG_E("Create array failed");
        return false;
    }

    int32_t index = 0;
    for (auto value : values) {
        napi_value element = nullptr;
        napi_create_string_utf8(env, value.c_str(), value.length(), &element);
        napi_set_element(env, dst, index++, element);
    }
    return true;
}

bool ConvertToNapiHandlerOfBooleanArray(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::vector<bool> values = src->GetBooleanArray();
    napi_create_array(env, &dst);
    bool isArray = false;
    if (napi_is_array(env, dst, &isArray) != napi_ok || !isArray) {
        WVLOG_E("Create array failed");
        return false;
    }

    int32_t index = 0;
    for (auto value : values) {
        napi_value element = nullptr;
        napi_get_boolean(env, value, &element);
        napi_set_element(env, dst, index++, element);
    }
    return true;
}

bool ConvertToNapiHandlerOfDoubleArray(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::vector<double> values = src->GetDoubleArray();
    napi_create_array(env, &dst);
    bool isArray = false;
    if (napi_is_array(env, dst, &isArray) != napi_ok || !isArray) {
        WVLOG_E("Create array failed");
        return false;
    }

    int32_t index = 0;
    for (auto value : values) {
        napi_value element = nullptr;
        napi_create_double(env, value, &element);
        napi_set_element(env, dst, index++, element);
    }
    return true;
}

bool ConvertToNapiHandlerOfInt64Array(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    std::vector<int64_t> values = src->GetInt64Array();
    napi_create_array(env, &dst);
    bool isArray = false;
    if (napi_is_array(env, dst, &isArray) != napi_ok || !isArray) {
        WVLOG_E("Create array failed");
        return false;
    }

    int32_t index = 0;
    for (auto value : values) {
        napi_value element = nullptr;
        napi_create_int64(env, value, &element);
        napi_set_element(env, dst, index++, element);
    }
    return true;
}
} // namespace

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

bool NapiParseUtils::ParseUint32(napi_env env, napi_value argv, uint32_t& outValue)
{
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_number) {
        return false;
    }

    uint32_t number = 0;
    napi_get_value_uint32(env, argv, &number);
    outValue = number;

    return true;
}

bool NapiParseUtils::ParseUint64(napi_env env, napi_value argv, uint64_t& outValue, bool *lossless)
{
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_bigint) {
        return false;
    }

    uint64_t number = 0;
    napi_get_value_bigint_uint64(env, argv, &number, lossless);
    outValue = number;

    return true;
}

bool NapiParseUtils::ParseInt64(napi_env env, napi_value argv, int64_t& outValue)
{
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_number) {
        return false;
    }

    int64_t number = 0;
    napi_get_value_int64(env, argv, &number);
    outValue = number;

    return true;
}

bool NapiParseUtils::ParseString(napi_env env, napi_value argv, std::string& outValue)
{
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        WVLOG_E("Not a valid napi string");
        return false;
    }
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    if (bufferSize + 1 > UINT_MAX) {
        WVLOG_E("String length is too long");
        return false;
    }
    size_t jsStringLength = 0;
    outValue.resize(bufferSize);
    napi_get_value_string_utf8(env, argv, outValue.data(), bufferSize + 1, &jsStringLength);
    if (jsStringLength != bufferSize) {
        WVLOG_E("The length values obtained twice are different");
        return false;
    }
    return true;
}

bool NapiParseUtils::ParseArrayBuffer(napi_env env, napi_value argv, std::string& outValue)
{
    bool isArrayBuffer = false;
    if (napi_ok != napi_is_arraybuffer(env, argv, &isArrayBuffer) || !isArrayBuffer) {
        WVLOG_E("Not a valid napi ArrayBuffer");
        return false;
    }

    char *arrBuf = nullptr;
    size_t byteLength = 0;
    napi_get_arraybuffer_info(env, argv, (void**)&arrBuf, &byteLength);
    if (!arrBuf) {
        WVLOG_E("Get arrayBuffer info failed");
        return false;
    }
    outValue = std::string(arrBuf, byteLength);
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

bool NapiParseUtils::ParseInt64Array(napi_env env, napi_value argv, std::vector<int64_t>& outValue)
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

        int64_t value;
        if (ParseInt64(env, item, value)) {
            outValue.push_back(value);
        }
    }

    return true;
}

bool NapiParseUtils::ParseBooleanArray(napi_env env, napi_value argv, std::vector<bool>& outValue)
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

        bool value;
        if (ParseBoolean(env, item, value)) {
            outValue.push_back(value);
        }
    }

    return true;
}

bool NapiParseUtils::ParseDoubleArray(napi_env env, napi_value argv, std::vector<double>& outValue)
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

        double value;
        if (ParseDouble(env, item, value)) {
            outValue.push_back(value);
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

bool NapiParseUtils::ParseDouble(napi_env env, napi_value argv, double& outValue)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_number) {
        return false;
    }

    double value;
    napi_get_value_double(env, argv, &value);
    outValue = value;
    return true;
}

//static
bool NapiParseUtils::ConvertNWebToNapiValue(napi_env env, std::shared_ptr<NWebMessage> src, napi_value& dst)
{
    if (!src) {
        WVLOG_E("src is nullptr");
        return false;
    }
    NWebValue::Type type = src->GetType();
    using ConvertNWebToNapiValueHandler = std::function<bool(napi_env, std::shared_ptr<NWebMessage>, napi_value&)>;
    static const std::unordered_map<NWebValue::Type, ConvertNWebToNapiValueHandler> functionMap = {
        { NWebValue::Type::STRING, ConvertToNapiHandlerOfString },
        { NWebValue::Type::BINARY, ConvertToNapiHandlerOfBinary },
        { NWebValue::Type::BOOLEAN, ConvertToNapiHandlerOfBoolean },
        { NWebValue::Type::INTEGER, ConvertToNapiHandlerOfInteger },
        { NWebValue::Type::DOUBLE, ConvertToNapiHandlerOfDouble },
        { NWebValue::Type::ERROR, ConvertToNapiHandlerOfError },
        { NWebValue::Type::STRINGARRAY, ConvertToNapiHandlerOfStringArray },
        { NWebValue::Type::BOOLEANARRAY, ConvertToNapiHandlerOfBooleanArray },
        { NWebValue::Type::DOUBLEARRAY, ConvertToNapiHandlerOfDoubleArray },
        { NWebValue::Type::INT64ARRAY, ConvertToNapiHandlerOfInt64Array }
    };
    auto it = functionMap.find(type);
    if (it == functionMap.end()) {
        WVLOG_E("This type not support");
        std::string msgStr = "This type not support";
        napi_create_string_utf8(env, msgStr.c_str(), msgStr.length(), &dst);
        return true;
    }
    return it->second(env, src, dst);
}

int32_t GetNumFromString(const std::string &input)
{
    int32_t value;

    try {
        value = std::stoi(input);
    } catch (std::out_of_range&) {
        WVLOG_E("value trans failed: out of range");
        value = 0;
    }

    return value;
}

bool NapiParseUtils::ParseJsLengthStringToInt(const std::string &input, PixelUnit &type, int32_t &value)
{
    return false;
}

ErrCode NapiParseUtils::ConstructStringFlowbuf(napi_env env, napi_value argv, int& fd, size_t& scriptLength)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        WVLOG_E("Not a valid napi string");
        return NWebError::PARAM_CHECK_ERROR;
    }

    napi_get_value_string_utf8(env, argv, nullptr, 0, &scriptLength);
    if (scriptLength + 1 > MAX_FLOWBUF_DATA_SIZE) {
        WVLOG_E("String length is too long");
        return NWebError::PARAM_CHECK_ERROR;
    }

    // get ashmem
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        WVLOG_E("Create flowbuffer adapter failed");
        return NWebError::NEW_OOM;
    }
    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength + 1, PROT_READ | PROT_WRITE, fd);
    if (!ashmem) {
        return NWebError::NEW_OOM;
    }

    // write to ashmem
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, static_cast<char*>(ashmem), scriptLength + 1, &jsStringLength);
    if (jsStringLength != scriptLength) {
        close(fd);
        WVLOG_E("Write js string failed, the length values are different");
        return NWebError::PARAM_CHECK_ERROR;
    }
    return NWebError::NO_ERROR;
}

ErrCode NapiParseUtils::ConstructArrayBufFlowbuf(napi_env env, napi_value argv, int& fd, size_t& scriptLength)
{
    bool isArrayBuffer = false;
    if (napi_ok != napi_is_arraybuffer(env, argv, &isArrayBuffer) || !isArrayBuffer) {
        WVLOG_E("Not a valid napi ArrayBuffer");
        return NWebError::PARAM_CHECK_ERROR;
    }

    char *arrBuf = nullptr;
    napi_get_arraybuffer_info(env, argv, (void**)&arrBuf, &scriptLength);
    if (!arrBuf) {
        WVLOG_E("Get arrayBuffer info failed");
        return NWebError::PARAM_CHECK_ERROR;
    }

    if (scriptLength + 1 > MAX_FLOWBUF_DATA_SIZE) {
        WVLOG_E("Arraybuffer length is too long");
        return NWebError::PARAM_CHECK_ERROR;
    }

    // get ashmem
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        WVLOG_E("Create flowbuffer adapter failed");
        return NWebError::NEW_OOM;
    }
    auto ashmem = flowbufferAdapter->CreateAshmem(scriptLength + 1, PROT_READ | PROT_WRITE, fd);
    if (!ashmem) {
        return NWebError::NEW_OOM;
    }

    // write to ashmem
    if (memcpy_s(ashmem, scriptLength + 1, arrBuf, scriptLength) != EOK) {
        WVLOG_E("ConstructArrayBufFlowbuf, memory copy failed");
        return NWebError::NEW_OOM;
    }
    static_cast<char*>(ashmem)[scriptLength] = '\0';
    return NWebError::NO_ERROR;
}
} // namespace NWeb
} // namespace OHOS
