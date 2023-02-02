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

void WebviewJavaScriptResultCallBack::UvJsCallbackThreadWoker(uv_work_t *work, int status)
{
    if (work == nullptr) {
        WVLOG_E("uv work is null");
        return;
    }
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm *param =
        reinterpret_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackParm*>(work->data);
    if (param == nullptr) {
        WVLOG_E("NapiJsCallBackParm is null");
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(param->env_, &scope);
    if (scope == nullptr) {
        return;
    }

    std::vector<napi_value> argv = {};
    for (std::shared_ptr<NWebValue> input: param->args_) {
        ParseNwebValue2NapiValue(param->env_, input, argv);
    }

    napi_value callback = nullptr;
    napi_value callResult = nullptr;
    napi_get_reference_value(param->env_, param->callback_, &callback);
    napi_call_function(param->env_, nullptr, callback, argv.size(), &argv[0], &callResult);
    // convert to nweb value
    ParseNapiValue2NwebValue(param->env_, callResult, param->value_);

    std::unique_lock<std::mutex> lock(param->mutex_);
    param->ready_ = true;
    param->condition_.notify_all();
    napi_close_handle_scope(param->env_, scope);
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResult(
    std::vector<std::shared_ptr<NWebValue>> args,
    const std::string& method,
    const std::string& objName)
{
    WVLOG_D("WebviewJavaScriptResultCallBack::GetJavaScriptResult method = %{public}s, objName = %{public}s",
        method.c_str(), objName.c_str());
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);

    if (objectMap_.find(objName) == objectMap_.end()) {
        return ret;
    }
    JavaScriptObj jsObj = objectMap_[objName];
    if (jsObj.methodMap.find(method) == jsObj.methodMap.end()) {
        return ret;
    }

    uv_loop_s *loop = nullptr;
    uv_work_t *work = nullptr;
    napi_get_uv_event_loop(jsObj.env, &loop);
    if (loop == nullptr) {
        WVLOG_E("get uv event loop failed");
        return ret;
    }
    work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        WVLOG_E("new uv work failed");
        return ret;
    }
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm *param =
        new (std::nothrow) WebviewJavaScriptResultCallBack::NapiJsCallBackParm();
    if (param == nullptr) {
        WVLOG_E("new WebMsgPortParam failed");
        delete work;
        return ret;
    }

    param->env_ = jsObj.env;
    param->callback_ = jsObj.methodMap[method];
    param->args_ = args;
    param->value_ = ret;

    work->data = reinterpret_cast<void*>(param);
    uv_queue_work(loop, work, [](uv_work_t *work) {}, UvJsCallbackThreadWoker);
    std::unique_lock<std::mutex> lock(param->mutex_);
    param->condition_.wait(lock, [&param] { return param->ready_; });
    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
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