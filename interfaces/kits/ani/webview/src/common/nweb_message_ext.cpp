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

#include "nweb_message_ext.h"
#include "securec.h"

namespace OHOS::NWeb {

std::shared_ptr<NWebMessage> ConvertNwebHap2NwebMessage(std::shared_ptr<NWebHapValue> hap)
{
    std::shared_ptr<NWebMessage> message = std::make_shared<NWebMessage>();
    if (!hap) {
        return message;
    }
    auto type = hap->GetType();
    switch (type) {
        case NWebHapValue::Type::INTEGER: {
            message->SetType(NWebValue::Type::INTEGER);
            message->SetInt64(hap->GetInt());
            break;
        }
        case NWebHapValue::Type::DOUBLE: {
            message->SetType(NWebValue::Type::DOUBLE);
            message->SetDouble(hap->GetDouble());
            break;
        }
        case NWebHapValue::Type::BOOLEAN: {
            message->SetType(NWebValue::Type::BOOLEAN);
            message->SetBoolean(hap->GetBool());
            break;
        }
        case NWebHapValue::Type::STRING: {
            message->SetType(NWebValue::Type::STRING);
            message->SetString(hap->GetString());
            break;
        }
        case NWebHapValue::Type::LIST: {
            auto list = hap->GetListValue();
            size_t len = list.size();
            std::vector<std::string> string_arr;
            std::vector<bool> bool_arr;
            std::vector<double> double_arr;
            std::vector<int64_t> int64_arr;
            NWebValue::Type elem_type = NWebValue::Type::NONE;
            for (size_t i = 0; i < len; i++) {
                if (!list[i]) {
                    continue;
                }
                if (list[i]->GetType() == NWebHapValue::Type::STRING) {
                    elem_type = NWebValue::Type::STRING;
                    string_arr.push_back(list[i]->GetString());
                } else if (list[i]->GetType() == NWebHapValue::Type::BOOLEAN) {
                    elem_type = NWebValue::Type::BOOLEAN;
                    bool_arr.push_back(list[i]->GetBool());
                } else if (list[i]->GetType() == NWebHapValue::Type::DOUBLE) {
                    elem_type = NWebValue::Type::DOUBLE;
                    double_arr.push_back(list[i]->GetDouble());
                } else if (list[i]->GetType() == NWebHapValue::Type::INTEGER) {
                    elem_type = NWebValue::Type::INTEGER;
                    int64_arr.push_back(list[i]->GetInt());
                }
            }
            if (elem_type == NWebValue::Type::STRING) {
                message->SetType(NWebValue::Type::STRINGARRAY);
                message->SetStringArray(string_arr);
            } else if (elem_type == NWebValue::Type::BOOLEAN) {
                message->SetType(NWebValue::Type::BOOLEANARRAY);
                message->SetBooleanArray(bool_arr);
            } else if (elem_type == NWebValue::Type::DOUBLE) {
                message->SetType(NWebValue::Type::DOUBLEARRAY);
                message->SetDoubleArray(double_arr);
            } else if (elem_type == NWebValue::Type::INTEGER) {
                message->SetType(NWebValue::Type::INT64ARRAY);
                message->SetInt64Array(int64_arr);
            }
            break;
        }
        case NWebHapValue::Type::DICTIONARY: {
            std::map<std::string, NWebValue> map;
            auto dict = hap->GetDictValue();
            message->SetType(NWebValue::Type::ERROR);
            std::string strErrName;
            std::string strErrMsg;
            if (dict.find("Error.name") != dict.end() && dict["Error.name"]) {
                strErrName = dict["Error.name"]->GetString();
            }
            if (dict.find("Error.message") != dict.end() && dict["Error.message"]) {
                strErrMsg = dict["Error.message"]->GetString();
            }
            message->SetErrName(strErrName);
            message->SetErrMsg(strErrMsg);
            break;
        }
        case NWebHapValue::Type::BINARY: {
            auto length = 0;
            auto buff = hap->GetBinary(length);
            std::vector<uint8_t> arr(length);
            memcpy_s(&arr[0], length, buff, length);
            message->SetType(NWebValue::Type::BINARY);
            message->SetBinary(arr);
            break;
        }
        default: {
            break;
        }
    }
    message->SetErrorDescription(hap->GetErrorDescription());
    return message;
}

} // namespace OHOS::NWeb
