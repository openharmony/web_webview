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

#ifndef NWEB_WEB_MESSAGE_H
#define NWEB_WEB_MESSAGE_H

#include <vector>

#include "nweb_export.h"
#include "nweb_value.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebMessage : public NWebValue {
public:
    NWebMessage() {}

    explicit NWebMessage(NWebValue::Type type) : NWebValue(type) {}

    ~NWebMessage() = default;

    void SetBinary(std::vector<uint8_t>& binary_data)
    {
        binary_data_.reserve(binary_data.size());
        binary_data_ = binary_data;
    }

    std::vector<uint8_t> GetBinary()
    {
        return binary_data_;
    }

    std::string GetErrName()
    {
        return err_name_;
    }

    std::string GetErrMsg()
    {
        return err_msg_;
    }

    int64_t GetInt64()
    {
        return value_;
    }

    void SetErrName(std::string name)
    {
        err_name_ = name;
    }

    void SetErrMsg(std::string msg)
    {
        err_msg_ = msg;
    }

    void SetInt64(int64_t value)
    {
        value_ = value;
    }

    std::vector<std::string> GetStringArray()
    {
        return string_arr_;
    }

    void SetStringArray(std::vector<std::string> string_arr)
    {
        string_arr_ = string_arr;
    }

    std::vector<bool> GetBooleanArray()
    {
        return bool_arr_;
    }

    void SetBooleanArray(std::vector<bool> bool_arr)
    {
        bool_arr_ = bool_arr;
    }

    std::vector<double> GetDoubleArray()
    {
        return double_arr_;
    }

    void SetDoubleArray(std::vector<double> double_arr)
    {
        double_arr_ = double_arr;
    }

    std::vector<int64_t> GetInt64Array()
    {
        return int64_arr_;
    }

    void SetInt64Array(std::vector<int64_t> int64_arr)
    {
        int64_arr_ = int64_arr;
    }

    std::string GetErrorDescription()
    {
        return error_description_;
    }

    void SetErrorDescription(const std::string& description)
    {
        error_description_ = description;
    }

private:
    std::vector<uint8_t> binary_data_;
    std::string err_name_;
    std::string err_msg_;
    std::string error_description_;
    int64_t value_ = -1;
    std::vector<std::string> string_arr_;
    std::vector<bool> bool_arr_;
    std::vector<double> double_arr_;
    std::vector<int64_t> int64_arr_;
};

} // namespace OHOS::NWeb

#endif // NWEB_WEB_MESSAGE_H
