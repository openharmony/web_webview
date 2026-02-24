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

#ifndef NWEB_VALUE_H
#define NWEB_VALUE_H

#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "nweb_export.h"

namespace OHOS::NWeb {

union data_union {
    int n;
    double f;
    bool b;

    data_union() {}

    data_union(int value) : n(value) {}

    data_union(double value) : f(value) {}

    data_union(bool value) : b(value) {}
};

class OHOS_NWEB_EXPORT NWebValue {
public:
    enum class Type : unsigned char {
        NONE = 0,
        BOOLEAN,
        INTEGER,
        DOUBLE,
        STRING,
        BINARY,
        DICTIONARY,
        LIST,
        ERROR,
        STRINGARRAY,
        BOOLEANARRAY,
        DOUBLEARRAY,
        INT64ARRAY
    };

    NWebValue() {}

    explicit NWebValue(Type type) : type_(type) {}

    explicit NWebValue(const int& value) : type_(Type::INTEGER), data_(value) {}

    explicit NWebValue(const double& value) : type_(Type::DOUBLE), data_(value) {}

    explicit NWebValue(const bool& value) : type_(Type::BOOLEAN), data_(value) {}

    explicit NWebValue(const std::string& value) : type_(Type::STRING), str_(value) {}

    NWebValue(const char* data, size_t len) : type_(Type::BINARY), str_(data, len) {}

    explicit NWebValue(const std::vector<NWebValue>& value) : type_(Type::LIST), list_value_(value.begin(), value.end())
    {}

    explicit NWebValue(const std::map<std::string, NWebValue>& value)
        : type_(Type::DICTIONARY), dictionary_value_(value)
    {}

    explicit NWebValue(const NWebValue& value) : type_(value.type_)
    {
        switch (type_) {
            case Type::NONE:
                break;
            case Type::BOOLEAN:
                data_.b = value.data_.b;
                break;
            case Type::INTEGER:
                data_.n = value.data_.n;
                break;
            case Type::DOUBLE:
                data_.f = value.data_.f;
                break;
            case Type::STRING:
                str_ = value.str_;
                break;
            case Type::BINARY:
                str_ = value.str_;
                break;
            case Type::LIST:
                list_value_ = value.list_value_;
                break;
            case Type::DICTIONARY:
                dictionary_value_ = value.dictionary_value_;
                break;
            default:
                break;
        }
    }

    NWebValue(std::vector<NWebValue>&& value) : type_(Type::LIST)
    {
        std::swap(list_value_, value);
    }

    NWebValue(std::map<std::string, NWebValue>&& value) : type_(Type::DICTIONARY)
    {
        std::swap(dictionary_value_, value);
    }

    NWebValue(NWebValue&& value)
    {
        *this = std::move(value);
    }

    ~NWebValue() = default;

    NWebValue& operator=(const NWebValue& value)
    {
        SetType(value.type_);
        switch (type_) {
            case Type::NONE:
                break;
            case Type::BOOLEAN:
                data_.b = value.data_.b;
                break;
            case Type::INTEGER:
                data_.n = value.data_.n;
                break;
            case Type::DOUBLE:
                data_.f = value.data_.f;
                break;
            case Type::STRING:
                str_ = value.str_;
                break;
            case Type::BINARY:
                str_ = value.str_;
                break;
            case Type::LIST:
                list_value_ = value.list_value_;
                break;
            case Type::DICTIONARY:
                dictionary_value_ = value.dictionary_value_;
                break;
            default:
                std::cout << "error: Invalid type" << std::endl;
                break;
        }
        return *this;
    }

    NWebValue& operator=(NWebValue&& value)
    {
        std::swap(type_, value.type_);
        switch (type_) {
            case Type::NONE:
                break;
            case Type::BOOLEAN:
                std::swap(data_.b, value.data_.b);
                break;
            case Type::INTEGER:
                std::swap(data_.n, value.data_.n);
                break;
            case Type::DOUBLE:
                std::swap(data_.f, value.data_.f);
                break;
            case Type::STRING:
                std::swap(str_, value.str_);
                break;
            case Type::BINARY:
                std::swap(str_, value.str_);
                break;
            case Type::LIST:
                std::swap(list_value_, value.list_value_);
                break;
            case Type::DICTIONARY:
                std::swap(dictionary_value_, value.dictionary_value_);
                break;
            default:
                std::cout << "error: Invalid type" << std::endl;
                break;
        }
        return *this;
    }

    bool operator==(NWebValue& oVal)
    {
        if (type_ != oVal.type_)
            return false;
        switch (type_) {
            case Type::NONE:
                return false;
            case Type::BOOLEAN:
                return data_.b == oVal.data_.b;
            case Type::INTEGER:
                return data_.n == oVal.data_.n;
            case Type::DOUBLE:
                return data_.f == oVal.data_.f;
            case Type::STRING:
                return str_ == oVal.str_;
            case Type::LIST:
                if ((*this).list_value_.size() != oVal.list_value_.size())
                    return false;
                for (size_t i = 0; i < list_value_.size(); ++i) {
                    NWebValue& lVal = oVal.list_value_[i];
                    NWebValue& rVal = (*this).list_value_[i];
                    if (!(lVal == rVal)) {
                        return false;
                    }
                }
                return true;
            case Type::DICTIONARY:
                if ((*this).dictionary_value_.size() != oVal.dictionary_value_.size())
                    return false;
                for (auto item : dictionary_value_) {
                    NWebValue& lVal = oVal.dictionary_value_[item.first];
                    NWebValue& rVal = (*this).dictionary_value_[item.first];
                    if (!(lVal == rVal)) {
                        return false;
                    }
                }
                return true;
            case Type::BINARY:
                return str_ == oVal.str_;
            default:
                std::cout << "error: Invalid type" << std::endl;
                return false;
        }
        return false;
    }

    bool IsNone()
    {
        return GetType() == Type::NONE;
    }

    bool IsBoolean()
    {
        return GetType() == Type::BOOLEAN;
    }

    bool IsString()
    {
        return GetType() == Type::STRING;
    }

    bool IsDouble()
    {
        return GetType() == Type::DOUBLE;
    }

    bool IsINTEGER()
    {
        return GetType() == Type::INTEGER;
    }

    bool IsList()
    {
        return GetType() == Type::LIST;
    }

    bool IsDictionary()
    {
        return GetType() == Type::DICTIONARY;
    }

    bool IsBinary()
    {
        return GetType() == Type::BINARY;
    }

    bool GetBoolean()
    {
        validateType(Type::BOOLEAN);
        return data_.b;
    }

    void SetBoolean(bool b)
    {
        validateType(Type::BOOLEAN);
        data_.b = b;
    }

    void SetString(std::string str)
    {
        validateType(Type::STRING);
        str_ = str;
    }

    std::string GetString()
    {
        validateType(Type::STRING);
        return str_;
    }

    void SetDouble(double dou)
    {
        validateType(Type::DOUBLE);
        data_.f = dou;
    }

    double GetDouble()
    {
        validateType(Type::DOUBLE);
        return data_.f;
    }

    void SetInt(int num)
    {
        validateType(Type::INTEGER);
        data_.n = num;
    }

    int GetInt()
    {
        validateType(Type::INTEGER);
        return data_.n;
    }

    size_t GetListValueSize()
    {
        validateType(Type::LIST);
        return list_value_.size();
    }

    std::vector<NWebValue> GetListValue()
    {
        validateType(Type::LIST);
        return list_value_;
    }

    NWebValue& GetListValue(unsigned int index)
    {
        validateType(Type::LIST);
        if (index >= list_value_.size()) {
            std::cout << "error: index larger than size()" << std::endl;
        }
        return list_value_[index];
    }

    void AddListValue(const NWebValue& value)
    {
        validateType(Type::LIST);
        SetType(Type::LIST);
        list_value_.push_back(value);
    }

    void deleteListValue()
    {
        validateType(Type::LIST);
        SetType(Type::LIST);
        list_value_.pop_back();
    }

    size_t GetDictionaryValueSize()
    {
        validateType(Type::DICTIONARY);
        return dictionary_value_.size();
    }

    std::vector<std::string> GetDictionaryValueKeys()
    {
        validateType(Type::DICTIONARY);
        std::vector<std::string> ret;
        for (auto& item : dictionary_value_) {
            ret.push_back(item.first);
        }
        return ret;
    }

    bool HasDictionaryValueKey(std::string& key)
    {
        validateType(Type::DICTIONARY);
        return dictionary_value_.count(key) == 1;
    }

    std::map<std::string, NWebValue> GetDictionaryValue()
    {
        validateType(Type::DICTIONARY);
        return dictionary_value_;
    }

    NWebValue& GetDictionaryValue(std::string& key)
    {
        validateType(Type::DICTIONARY);
        return dictionary_value_[key];
    }

    void AddDictionaryValue(std::string key, NWebValue& value)
    {
        validateType(Type::DICTIONARY);
        dictionary_value_[key] = value;
    }

    void DeleteDictionaryValue(std::string& key)
    {
        validateType(Type::DICTIONARY);
        dictionary_value_.erase(key);
    }

    size_t GetBinaryValueSize()
    {
        validateType(Type::BINARY);
        return str_.size();
    }

    const char* GetBinaryValue()
    {
        validateType(Type::BINARY);
        return (const char*)str_.c_str();
    }

    void SetJsonString(std::string json_string)
    {
        str_json_ = json_string;
    }

    std::string GetJsonString()
    {
        return str_json_;
    }

    Type GetType()
    {
        return type_;
    }

    void SetType(Type type)
    {
        type_ = type;
    }

    void validateType(Type type) const
    {
        if (type_ != Type::NONE && type_ != type) {
            std::cout << "error: Invalid type" << std::endl;
        }
    }

    int error_ = 0;

private:
    Type type_ = Type::NONE;
    data_union data_;
    std::string str_;
    std::string str_json_;
    std::map<std::string, NWebValue> dictionary_value_;
    std::vector<NWebValue> list_value_;
};

} // namespace OHOS::NWeb

#endif // NWEB_VALUE_H
