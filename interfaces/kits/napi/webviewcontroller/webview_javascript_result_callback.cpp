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

#include "webview_javascript_result_callback.h"

#include "napi_parse_utils.h"
#include "nweb_log.h"

namespace OHOS::NWeb {

WebviewJavaScriptResultCallBack::~WebviewJavaScriptResultCallBack()
{
    for (auto it1 = objectMap_.begin(); it1 != objectMap_.end(); ++it1) {
        for (auto it2 = it1->second.methodMap.begin(); it2 != it1->second.methodMap.end(); ++it2) {
            napi_delete_reference(it1->second.env, it2->second);
        }
    }
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResult(
    std::vector<std::shared_ptr<NWebValue>> args,
    const std::string& method,
    const std::string& objName)
{
    WVLOG_D("WebviewJavaScriptResultCallBack::GetJavaScriptResult method = %{public}s, objName = %{public}s",
        method.c_str(), objName.c_str());
    std::vector<napi_value> argv = {};
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);

    if (objectMap_.find(objName) == objectMap_.end()) {
        return ret;
    }
    JavaScriptObj jsObj = objectMap_[objName];
    if (jsObj.methodMap.find(method) == jsObj.methodMap.end()) {
        return ret;
    }
    for (std::shared_ptr<NWebValue> input: args) {
        ParseNwebValue2NapiValue(jsObj.env, input, argv);
    }

    napi_value callback = nullptr;
    napi_value callResult = nullptr;
    napi_get_reference_value(jsObj.env, jsObj.methodMap[method], &callback);
    napi_call_function(jsObj.env, nullptr, callback, argv.size(), &argv[0], &callResult);

    ParseNapiValue2NwebValue(jsObj.env, callResult, ret);
    return ret;
}

void WebviewJavaScriptResultCallBack::RegisterJavaScriptProxy(napi_env env, napi_value obj,
    const std::string& objName, const std::vector<std::string>& methodList)
{
    if (objectMap_.find(objName) != objectMap_.end()) {
        WVLOG_I("object already exists, objName = %{public}s", objName.c_str());
        return;
    }

    JavaScriptObj jsObj;
    jsObj.env = env;
    for (uint32_t i = 0; i < methodList.size(); i++) {
        std::string methodName = methodList[i];
        bool hasFunc = false;
        napi_value result = nullptr;
        napi_valuetype valueType = napi_undefined;
        napi_ref callback = nullptr;

        napi_has_named_property(env, obj, methodName.c_str(), &hasFunc);
        if (!hasFunc) {
            continue;
        }
        napi_get_named_property(env, obj, methodName.c_str(), &result);
        napi_typeof(env, result, &valueType);
        if (valueType != napi_function) {
            continue;
        }
        napi_create_reference(env, result, 1, &callback);
        jsObj.methodMap[methodName] = callback;
    }

    objectMap_[objName] = jsObj;
}

bool WebviewJavaScriptResultCallBack::DeleteJavaScriptRegister(const std::string& objName)
{
    if (objectMap_.find(objName) == objectMap_.end()) {
        return false;
    }

    for (auto it = objectMap_[objName].methodMap.begin(); it != objectMap_[objName].methodMap.end(); ++it) {
        napi_delete_reference(objectMap_[objName].env, it->second);
    }
    objectMap_.erase(objName);
    return true;
}

void WebviewJavaScriptResultCallBack::ParseNwebValue2NapiValue(
    napi_env env,
    std::shared_ptr<NWebValue> value,
    std::vector<napi_value>& argv)
{
    napi_value napiValue = nullptr;

    switch (value->GetType()) {
        case NWebValue::Type::INTEGER:
            napi_create_int32(env, value->GetInt(), &napiValue);
            argv.push_back(napiValue);
            break;
        case NWebValue::Type::DOUBLE:
            napi_create_double(env, value->GetDouble(), &napiValue);
            argv.push_back(napiValue);
            break;
        case NWebValue::Type::BOOLEAN:
            napi_get_boolean(env, value->GetBoolean(), &napiValue);
            argv.push_back(napiValue);
            break;
        case NWebValue::Type::STRING:
            napi_create_string_utf8(env, value->GetString().c_str(), NAPI_AUTO_LENGTH, &napiValue);
            argv.push_back(napiValue);
            break;
        case NWebValue::Type::NONE:
        default:
            WVLOG_E("ParseNwebValue2NapiValue invalid type");
            break;
    }
}

void WebviewJavaScriptResultCallBack::ParseNapiValue2NwebValue(
    napi_env env, napi_value value,
    std::shared_ptr<NWebValue> nwebValue)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);

    switch (valueType) {
        case napi_number: {
            double douVal = 0.0;
            napi_get_value_double(env, value, &douVal);
            nwebValue->SetType(NWebValue::Type::DOUBLE);
            nwebValue->SetDouble(douVal);
            break;
        }
        case napi_boolean: {
            bool boolVal;
            napi_get_value_bool(env, value, &boolVal);
            nwebValue->SetType(NWebValue::Type::BOOLEAN);
            nwebValue->SetBoolean(boolVal);
            break;
        }
        case napi_string: {
            std::string strVal;
            if (!NapiParseUtils::ParseString(env, value, strVal)) {
                return;
            }
            nwebValue->SetType(NWebValue::Type::STRING);
            nwebValue->SetString(strVal);
            break;
        }
        default: {
            WVLOG_E("ParseNapiValue2NwebValue invalid type");
            break;
        }
    }
}

}