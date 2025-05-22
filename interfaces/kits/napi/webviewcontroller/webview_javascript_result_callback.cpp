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

#include <sys/mman.h>
#include <unistd.h>

#include "core/common/container_scope.h"
#include "napi_parse_utils.h"
#include "nweb_napi_scope.h"
#include "native_engine/native_engine.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
namespace {
#define JS_BRIDGE_BINARY_ARGS_COUNT 2

const int MAX_FLOWBUF_DATA_SIZE = 52428800; /* 50MB*/
const int MAX_ENTRIES = 10;
const int HEADER_SIZE = (MAX_ENTRIES * 8);  /* 10 * (int position + int length) */
const int INDEX_SIZE = 2;

// For the sake of the storage API, make this quite large.
const uint32_t MAX_RECURSION_DEPTH = 11;
const uint32_t MAX_DATA_LENGTH = 10000;

std::unordered_map<int32_t, WebviewJavaScriptResultCallBack*> g_webviewJsResultCallbackMap;
std::mutex g_objectMtx;

std::vector<std::string> ParseNapiValue2NwebValue(napi_env env, napi_value& value,
    std::shared_ptr<NWebValue> nwebValue, bool* isObject);
std::vector<std::string> ParseNapiValue2NwebValue(napi_env env, napi_value* value,
    std::shared_ptr<NWebValue> nwebValue, bool* isObject);

class ValueConvertState {
public:
    // Level scope which updates the current depth of some ValueConvertState.
    class Level {
    public:
        explicit Level(ValueConvertState* state) : state_(state)
        {
            if (state_) {
                state_->maxRecursionDepth_--;
            }
        }
        ~Level()
        {
            if (state_) {
                state_->maxRecursionDepth_++;
            }
        }

    private:
        ValueConvertState* state_;
    };

    explicit ValueConvertState() : maxRecursionDepth_(MAX_RECURSION_DEPTH)
    {
    }

    ValueConvertState(const ValueConvertState&) = delete;
    ValueConvertState& operator=(const ValueConvertState&) = delete;

    bool HasReachedMaxRecursionDepth()
    {
        return maxRecursionDepth_ == 0;
    }

private:
    uint32_t maxRecursionDepth_;
};

WebviewJavaScriptResultCallBack* FromNwebID(int32_t nwebId)
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    if (auto it = g_webviewJsResultCallbackMap.find(nwebId); it != g_webviewJsResultCallbackMap.end()) {
        auto js_result_callback = it->second;
        return js_result_callback;
    }
    return nullptr;
}

void StringSplit(std::string str, const char split, std::vector<std::string>& result)
{
    std::istringstream iss(str);
    std::string token;
    while (getline(iss, token, split)) {
        result.push_back(token);
    }
}

void CallH5Function(napi_env env, napi_value* napiArg, std::shared_ptr<NWebValue> nwebValue,
    WebviewJavaScriptResultCallBack::H5Bundle bundle)
{
    WVLOG_D("CallH5Function called");
    bool isObject = false;
    std::vector<std::string> methodNameList;
    methodNameList = ParseNapiValue2NwebValue(env, napiArg, nwebValue, &isObject);
    if (isObject && FromNwebID(bundle.nwebId)) {
        JavaScriptOb::ObjectID returnedObjectId;
        if (FromNwebID(bundle.nwebId)->FindObjectIdInJsTd(env, *napiArg, &returnedObjectId)) {
            FromNwebID(bundle.nwebId)->FindObject(returnedObjectId)->AddHolder(bundle.frameRoutingId);
        } else {
            returnedObjectId = FromNwebID(bundle.nwebId)->AddObject(env, *napiArg, false, bundle.frameRoutingId);
        }

        FromNwebID(bundle.nwebId)->SetUpAnnotateMethods(returnedObjectId, methodNameList);

        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, *napiArg, &valueType);
        if (valueType == napi_function) {
            WVLOG_D("CallH5Function function");
            nwebValue = std::make_shared<NWebValue>();
        } else {
            WVLOG_D("CallH5Function object");
            std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") + std::to_string(returnedObjectId);
            nwebValue = std::make_shared<NWebValue>(bin.c_str(), bin.size());
        }
    }
}

napi_value CallbackFunctionForH5(napi_env env, napi_callback_info info)
{
    WVLOG_D("CallbackFunctionForH5 called");
    napi_escapable_handle_scope scope = nullptr;
    napi_open_escapable_handle_scope(env, &scope);
    napi_value h5CallBackResult = nullptr;
    napi_value thisVar = nullptr;
    size_t argc = 0;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, nullptr, nullptr, nullptr);
    napi_value* napiArgs = nullptr;
    if (argc > 0) {
        WVLOG_D("CallbackFunctionForH5 argc not zero");
        napiArgs = new napi_value[argc];
        if (!napiArgs) {
            WVLOG_D("CallbackFunctionForH5 argc malloc fail");
            napi_get_undefined(env, &h5CallBackResult);
            napi_close_escapable_handle_scope(env, scope);
            return h5CallBackResult;
        }
    }

    napi_get_cb_info(env, info, &argc, napiArgs, &thisVar, &data);
    WebviewJavaScriptResultCallBack::H5Bundle bundle =
        *reinterpret_cast<WebviewJavaScriptResultCallBack::H5Bundle*>(data);

    std::vector<std::shared_ptr<NWebValue>> nwebArgs;
    for (size_t i = 0; i < argc; i++) {
        std::shared_ptr<NWebValue> nwebArg = std::make_shared<NWebValue>(NWebValue::Type::NONE);
        napi_value napiArg = napiArgs[i];
        napi_escape_handle(env, scope, napiArg, &napiArg);
        CallH5Function(env, &napiArg, nwebArg, bundle);
        nwebArgs.push_back(nwebArg);
    }

    if (FromNwebID(bundle.nwebId)) {
        FromNwebID(bundle.nwebId)->CallH5FunctionInternal(env, bundle, nwebArgs);
    }

    if (napiArgs) {
        delete[] napiArgs;
    }

    napi_get_undefined(env, &h5CallBackResult);
    napi_close_escapable_handle_scope(env, scope);
    return h5CallBackResult;
}

napi_value CreateFunctionForH5(
    napi_env env, int32_t nwebId, int32_t frameRoutingId, int32_t h5ObjectId, std::string funcName)
{
    // Create a bundle.
    auto bundle = std::make_unique<OHOS::NWeb::WebviewJavaScriptResultCallBack::H5Bundle>();
    bundle->nwebId = nwebId;
    bundle->frameRoutingId = frameRoutingId;
    bundle->h5Id = h5ObjectId;
    bundle->funcName = funcName;

    napi_value func = nullptr;
    napi_status s = napi_ok;
    s = napi_create_function(env, funcName.c_str(), funcName.size(), CallbackFunctionForH5, bundle.release(), &func);
    if (s != napi_ok) {
        WVLOG_E("CreateFunctionForH5 napi api call fail");
        return nullptr;
    }
    return func;
}

void AddFunctionToObjectForH5(
    napi_env env, OHOS::NWeb::WebviewJavaScriptResultCallBack::H5Bundle& bundle, napi_value obj)
{
    if (!obj || bundle.frameRoutingId < 0) {
        WVLOG_D("AddFunctionToObjectForH5 obj or frame id error");
        return;
    }
    napi_value func = CreateFunctionForH5(env, bundle.nwebId, bundle.frameRoutingId, bundle.h5Id, bundle.funcName);
    if (!func) {
        WVLOG_D("AddFunctionToObjectForH5 create func fail");
        return;
    }
    napi_status s = napi_ok;
    s = napi_set_named_property(env, obj, bundle.funcName.c_str(), func);
    if (s != napi_ok) {
        WVLOG_E("AddFunctionToObjectForH5 napi api call fail");
    }
}

void CreateProxyForH5Object(napi_env env, napi_value* result)
{
    napi_status s = napi_ok;
    s = napi_create_object(env, result);
    if (s != napi_ok) {
        WVLOG_E("CreateProxyFOrH5Object napi api call fail");
    }
}

napi_handle_scope OpenScope(napi_env env)
{
    napi_handle_scope scope = nullptr;
    NAPI_CALL(env, napi_open_handle_scope(env, &scope));
    return scope;
}

void CloseScope(napi_env env, napi_handle_scope scope)
{
    (void)napi_close_handle_scope(env, scope);
}

void CreateUvQueueWorkEnhanced(napi_env env, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* data,
    void (*handler)(napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* data))
{
    uv_loop_s* loop = nullptr;
    NAPI_CALL_RETURN_VOID(env, napi_get_uv_event_loop(env, &loop));
    class WorkData {
    public:
        WorkData() = delete;

        WorkData(napi_env env, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* data,
            void (*handler)(
                napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* data))
            : env_(env), data_(data), handler_(handler)
        {}

        napi_env env_;
        WebviewJavaScriptResultCallBack::NapiJsCallBackParm* data_;
        void (*handler_)(napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* data);
    };

    auto workData = new WorkData(env, data, handler);
    auto work = new uv_work_t;
    work->data = reinterpret_cast<void*>(workData);

    auto callback = [](uv_work_t* work, int status) {
        auto workData = static_cast<WorkData*>(work->data);
        if (!workData) {
            delete work;
            return;
        }

        if (!workData->env_ || !workData->data_ || !workData->handler_) {
            delete workData;
            delete work;
            return;
        }

        napi_env env = workData->env_;
        auto closeScope = [env](napi_handle_scope scope) { CloseScope(env, scope); };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(OpenScope(env), closeScope);

        workData->handler_(workData->env_, static_cast<napi_status>(status), workData->data_);

        delete workData;
        delete work;
    };
    int ret = uv_queue_work_with_qos(
        loop, work, [](uv_work_t* work) {}, callback, uv_qos_user_initiated);
    if (ret != 0) {
        if (workData) {
            delete workData;
            workData = nullptr;
        }
        if (work) {
            delete work;
            work = nullptr;
        }
        return;
    }
}

bool CreateNapiJsCallBackParm(WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*& inParam,
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*& outParam,
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm*& param)
{
    inParam = new (std::nothrow) WebviewJavaScriptResultCallBack::NapiJsCallBackInParm();
    if (inParam == nullptr) {
        WVLOG_D("CreateNapiJsCallBackParm argc malloc fail");
        return false;
    }
    outParam = new (std::nothrow) WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm();
    if (outParam == nullptr) {
        WVLOG_D("CreateNapiJsCallBackParm argc malloc fail");
        delete inParam;
        return false;
    }
    param = new (std::nothrow) WebviewJavaScriptResultCallBack::NapiJsCallBackParm();
    if (param == nullptr) {
        WVLOG_D("CreateNapiJsCallBackParm argc malloc fail");
        delete inParam;
        delete outParam;
        return false;
    }
    return true;
}

void DeleteNapiJsCallBackParm(WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam,
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam,
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param)
{
    if (inParam != nullptr) {
        delete inParam;
        inParam = nullptr;
    }

    if (outParam != nullptr) {
        delete outParam;
        outParam = nullptr;
    }

    if (param != nullptr) {
        delete param;
        param = nullptr;
    }
}

napi_value ParseArrayNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId);
napi_value ParseDictionaryNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId);
napi_value ParseBinNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId);

void ParseDictionaryNapiValue2NwebValue(
    napi_env env, ValueConvertState* state, napi_value& value, std::shared_ptr<NWebValue>& nwebValue, bool* isOject);

bool ParseBasicTypeNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value, napi_value& napiValue)
{
    napi_status s = napi_ok;
    switch (value->GetType()) {
        case NWebValue::Type::INTEGER:
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue INTEGER type");
            s = napi_create_int32(env, value->GetInt(), &napiValue);
            if (s != napi_ok) {
                WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
            }
            break;
        case NWebValue::Type::DOUBLE:
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue DOUBLE type");
            s = napi_create_double(env, value->GetDouble(), &napiValue);
            if (s != napi_ok) {
                WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
            }
            break;
        case NWebValue::Type::BOOLEAN:
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue BOOLEAN type");
            s = napi_get_boolean(env, value->GetBoolean(), &napiValue);
            if (s != napi_ok) {
                WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
            }
            break;
        case NWebValue::Type::STRING:
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue STRING type");
            s = napi_create_string_utf8(env, value->GetString().c_str(), NAPI_AUTO_LENGTH, &napiValue);
            if (s != napi_ok) {
                WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
            }
            break;
        default:
            return false;
    }
    return true;
}

napi_value ParseNwebValue2NapiValueHelper(napi_env env, std::shared_ptr<NWebValue> value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId)
{
    napi_value napiValue = nullptr;
    if (!value) {
        napi_get_undefined(env, &napiValue);
        return napiValue;
    }
    if (ParseBasicTypeNwebValue2NapiValue(env, value, napiValue)) {
        return napiValue;
    }
    switch (value->GetType()) {
        case NWebValue::Type::LIST: {
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue LIST type");
            napiValue = ParseArrayNwebValue2NapiValue(env, value, objectsMap, nwebId, frameId);
            return napiValue;
        }
        case NWebValue::Type::DICTIONARY: {
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue DICTIONARY type");
            napiValue = ParseDictionaryNwebValue2NapiValue(env, value, objectsMap, nwebId, frameId);
            return napiValue;
        }
        case NWebValue::Type::BINARY: {
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue BINARY type");
            napiValue = ParseBinNwebValue2NapiValue(env, value, objectsMap, nwebId, frameId);
            return napiValue;
        }
        case NWebValue::Type::NONE: {
            WVLOG_D("ParseBasicTypeNwebValue2NapiValue NONE type");
            break;
        }
        default:
            WVLOG_E("ParseNwebValue2NapiValueHelper invalid type");
            break;
    }
    napi_get_undefined(env, &napiValue);
    return napiValue;
}

napi_value ParseArrayNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId)
{
    napi_value napiValue = nullptr;
    napi_status s = napi_ok;
    size_t length = value->GetListValueSize();
    s = napi_create_array_with_length(env, length, &napiValue);
    if (s != napi_ok) {
        WVLOG_E("ParseArrayNwebValue2NapiValue napi api call fail");
        return napiValue;
    }
    for (size_t i = 0; i < length; ++i) {
        auto nPtr = std::make_shared<NWebValue>(value->GetListValue(i));
        s = napi_set_element(env, napiValue, i, ParseNwebValue2NapiValueHelper(env, nPtr, objectsMap, nwebId, frameId));
        if (s != napi_ok) {
            WVLOG_E("ParseArrayNwebValue2NapiValue napi api call fail");
        }
    }
    return napiValue;
}

napi_value ParseDictionaryNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId)
{
    napi_value napiValue = nullptr;
    napi_status s = napi_ok;
    s = napi_create_object(env, &napiValue);
    auto dict = value->GetDictionaryValue();
    for (auto& item : dict) {
        auto nValuePtr = std::make_shared<NWebValue>(item.second);
        auto nKeyPtr = std::make_shared<NWebValue>(item.first);
        s = napi_set_property(env, napiValue, ParseNwebValue2NapiValueHelper(env, nKeyPtr, objectsMap, nwebId, frameId),
            ParseNwebValue2NapiValueHelper(env, nValuePtr, objectsMap, nwebId, frameId));
        if (s != napi_ok) {
            WVLOG_E("ParseDictionaryNwebValue2NapiValue napi api call fail");
        }
    }
    return napiValue;
}

napi_value ParseBinNwebValue2NapiValue(napi_env env, const std::shared_ptr<NWebValue>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId)
{
    napi_value napiValue = nullptr;
    napi_get_undefined(env, &napiValue);
    auto buff = value->GetBinaryValue();
    JavaScriptOb::ObjectID objId;
    std::string str(buff);
    std::vector<std::string> strList;
    StringSplit(str, ';', strList);
    if (strList.size() < JS_BRIDGE_BINARY_ARGS_COUNT) {
        napi_get_undefined(env, &napiValue);
        return napiValue;
    }
    std::istringstream ss(strList[1]);
    ss >> objId;
    if (strList[0] == "TYPE_OBJECT_ID") {
        WVLOG_D("ParseNwebValue2NapiValueHelper: TYPE_OBJECT_ID");
        auto iter = objectsMap.find(objId);
        if (iter != objectsMap.end() && iter->second) {
            WVLOG_I("ParseNwebValue2NapiValueHelper: type is "
                    "binary, object is found and object_id == %{public}d",
                objId);
            napiValue = iter->second->GetValue();
        }
        return napiValue;
    } else if (strList[0] == "TYPE_H5_OBJECT_ID") {
        CreateProxyForH5Object(env, &napiValue);
        std::vector<std::string> methodNames;
        methodNames.assign(strList.begin() + JS_BRIDGE_BINARY_ARGS_COUNT, strList.end()); // skip id and type
        WVLOG_D("ParseNwebValue2NapiValueHelper: TYPE_H5_OBJECT_ID");
        for (auto name : methodNames) {
            OHOS::NWeb::WebviewJavaScriptResultCallBack::H5Bundle bundle;
            bundle.nwebId = nwebId;
            bundle.frameRoutingId = frameId;
            bundle.h5Id = objId;
            bundle.funcName = name;
            AddFunctionToObjectForH5(env, bundle, napiValue);
        }
        return napiValue;
    } else if (strList[0] == "TYPE_H5_FUNCTION_ID") {
        WVLOG_D("ParseNwebValue2NapiValueHelper: TYPE_H5_FUNCTION_ID");
        napiValue = CreateFunctionForH5(env, nwebId, frameId, objId, "");
        return napiValue;
    }
    return napiValue;
}

napi_value ParseNwebValue2NapiValue(napi_env env, std::shared_ptr<NWebValue> value,
    WebviewJavaScriptResultCallBack::ObjectMap objectsMap, int32_t nwebId, int32_t frameId)
{
    return ParseNwebValue2NapiValueHelper(env, value, objectsMap, nwebId, frameId);
}

bool ParseBasicTypeNapiValue2NwebValue(napi_env env, napi_value& value,
    std::shared_ptr<NWebValue>& nwebValue, bool* isObject)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    napi_status s = napi_ok;
    switch (valueType) {
        case napi_undefined: // fallthrough
        case napi_null:
            WVLOG_D("ParseBasicTypeNapiValue2NwebValue null or undefined type");
            nwebValue->SetType(NWebValue::Type::NONE);
            break;
        case napi_number: {
            WVLOG_D("ParseBasicTypeNapiValue2NwebValue number type");
            double douVal = 0.0;
            s = napi_get_value_double(env, value, &douVal);
            nwebValue->SetType(NWebValue::Type::DOUBLE);
            nwebValue->SetDouble(douVal);
            break;
        }
        case napi_boolean: {
            WVLOG_D("ParseBasicTypeNapiValue2NwebValue boolean type");
            bool boolVal;
            s = napi_get_value_bool(env, value, &boolVal);
            nwebValue->SetType(NWebValue::Type::BOOLEAN);
            nwebValue->SetBoolean(boolVal);
            break;
        }
        case napi_symbol: // fallthrough
        case napi_string: {
            WVLOG_D("ParseBasicTypeNapiValue2NwebValue string type");
            std::string strVal;
            if (!NapiParseUtils::ParseString(env, value, strVal)) {
                WVLOG_E("ParseBasicTypeNapiValue2NwebValue NapiParseUtils::ParseString "
                        "failed");
            }
            if (strVal == "methodNameListForJsProxy") {
                *isObject = true;
            }
            nwebValue->SetType(NWebValue::Type::STRING);
            nwebValue->SetString(strVal);
            break;
        }
        default: {
            WVLOG_D("ParseBasicTypeNapiValue2NwebValue invalid type");
            return false;
        }
    }
    return true;
}

void ParseNapiValue2NwebValueHelper(
    napi_env env, ValueConvertState* state, napi_value& value,
    std::shared_ptr<NWebValue> nwebValue, bool* isOject)
{
    ValueConvertState::Level stateLevel(state);
    if (state->HasReachedMaxRecursionDepth()) {
        return;
    }
    if (!nwebValue || ParseBasicTypeNapiValue2NwebValue(env, value, nwebValue, isOject)) {
        return;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    napi_status s = napi_ok;
    switch (valueType) {
        case napi_object: {
            bool isArray;
            s = napi_is_array(env, value, &isArray);
            if (s != napi_ok) {
                WVLOG_E("ParseNapiValue2NwebValueHelper napi api call fail");
            }
            if (!isArray) {
                WVLOG_D("ParseNapiValue2NwebValueHelper napi isArray");
                ParseDictionaryNapiValue2NwebValue(env, state, value, nwebValue, isOject);
                break;
            }
            nwebValue->SetType(NWebValue::Type::LIST);
            uint32_t size;
            s = napi_get_array_length(env, value, &size);
            size = std::min(size, MAX_DATA_LENGTH);
            if (s != napi_ok) {
                WVLOG_E("ParseNapiValue2NwebValueHelper napi api call fail");
            }
            for (uint32_t i = 0; i < size; i++) {
                napi_value napiTmp;
                s = napi_get_element(env, value, i, &napiTmp);
                if (s != napi_ok) {
                    WVLOG_E("ParseNapiValue2NwebValueHelper napi api call fail");
                }
                auto nwebTmp = std::make_shared<NWebValue>();
                ParseNapiValue2NwebValueHelper(env, state, napiTmp, nwebTmp, isOject);
                nwebValue->AddListValue(*nwebTmp);
            }
            break;
        }
        default: {
            WVLOG_E("ParseNapiValue2NwebValueHelper invalid type");
        }
    }
}

void ParseDictionaryNapiValue2NwebValue(
    napi_env env, ValueConvertState* state, napi_value& value, std::shared_ptr<NWebValue>& nwebValue, bool* isOject)
{
    napi_status s = napi_ok;
    nwebValue->SetType(NWebValue::Type::DICTIONARY);
    napi_value propertyNames;
    s = napi_get_property_names(env, value, &propertyNames);
    if (s != napi_ok) {
        WVLOG_E("ParseDictionaryNapiValue2NwebValue napi api call fail");
    }
    uint32_t size;
    s = napi_get_array_length(env, propertyNames, &size);
    size = std::min(size, MAX_DATA_LENGTH);
    if (s != napi_ok) {
        WVLOG_E("ParseDictionaryNapiValue2NwebValue napi api call fail");
    }

    for (uint32_t i = 0; i < size; i++) {
        napi_value napiKeyTmp;
        s = napi_get_element(env, propertyNames, i, &napiKeyTmp);
        if (s != napi_ok) {
            WVLOG_E("ParseDictionaryNapiValue2NwebValue napi api call fail");
        }
        bool hasOwnProperty = false;
        s = napi_has_own_property(env, value, napiKeyTmp, &hasOwnProperty);
        if (s != napi_ok) {
            WVLOG_E("ParseDictionaryNapiValue2NwebValue napi api call fail");
        }
        if (!hasOwnProperty) {
            continue;
        }
        napi_value napiValueTmp;
        s = napi_get_property(env, value, napiKeyTmp, &napiValueTmp);
        if (s != napi_ok) {
            WVLOG_E("ParseDictionaryNapiValue2NwebValue napi api call fail");
        }
        auto nwebValueTmp = std::make_shared<NWebValue>();
        auto nwebKeyTmp = std::make_shared<NWebValue>();
        ParseNapiValue2NwebValueHelper(env, state, napiKeyTmp, nwebKeyTmp, isOject);
        ParseNapiValue2NwebValueHelper(env, state, napiValueTmp, nwebValueTmp, isOject);
        nwebValue->AddDictionaryValue(nwebKeyTmp->GetString(), *nwebValueTmp);
    }
}

bool HasAnnotationProperty(napi_env env, napi_value& value)
{
    std::string annotation = "methodNameListForJsProxy";
    napi_status s = napi_ok;
    bool hasProperty = false;
    napi_value napi_str;
    s = napi_create_string_utf8(env, annotation.c_str(), NAPI_AUTO_LENGTH,
                                &napi_str);
    if (s != napi_ok) {
        WVLOG_E("HasAnnotationProperty napi api call fail");
    }
    s = napi_has_own_property(env, value, napi_str, &hasProperty);
    if (s != napi_ok) {
        WVLOG_D("HasAnnotationProperty napi api call fail");
    }
    WVLOG_D(
        "HasAnnotationProperty hasProperty = "
        "%{public}d",
        hasProperty);
    return hasProperty;
}

bool IsCallableObject(napi_env env,
                      napi_value& value,
                      std::vector<std::string>* methodNameList)
{
    std::string annotation = "methodNameListForJsProxy";
    napi_status s = napi_ok;
    napi_value napi_str;
    s = napi_create_string_utf8(env, annotation.c_str(), NAPI_AUTO_LENGTH,
                                &napi_str);
    if (s != napi_ok) {
        WVLOG_E("IsCallableObject napi api call fail");
    }
    if (!HasAnnotationProperty(env, value)) {
        return false;
    }
    napi_value result;
    s = napi_get_property(env, value, napi_str, &result);
    if (s != napi_ok) {
        WVLOG_E("IsCallableObject napi api call fail");
    }
    bool isArray = false;
    s = napi_is_array(env, result, &isArray);
    if (s != napi_ok) {
        WVLOG_E("IsCallableObject napi api call fail");
    }
    if (!isArray) {
        return false;
    }
    uint32_t size;
    s = napi_get_array_length(env, result, &size);
    if (s != napi_ok) {
        WVLOG_E("IsCallableObject napi api call fail");
    }
    for (uint32_t i = 0; i < size; i++) {
        napi_value napiTmp;
        s = napi_get_element(env, result, i, &napiTmp);
        if (s != napi_ok) {
            WVLOG_E("IsCallableObject napi api call fail");
        }
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, napiTmp, &valueType);
        if (valueType != napi_string) {
            continue;
        }
        std::string strVal;
        if (!NapiParseUtils::ParseString(env, napiTmp, strVal)) {
            WVLOG_E("IsCallableObject NapiParseUtils::ParseString failed");
        }
        methodNameList->push_back(strVal);
    }
    return true;
}

std::vector<std::string> ParseNapiValue2NwebValue(napi_env env, napi_value& value,
    std::shared_ptr<NWebValue> nwebValue, bool* isObject)
{
    napi_status s = napi_ok;
    std::vector<std::string> methodNameList;

    s = napi_is_promise(env, value, isObject);
    if (s != napi_ok) {
        WVLOG_E("ParseNapiValue2NwebValue napi api call fail");
    }

    if (*isObject) {
        return std::vector<std::string>{"then", "catch", "finally"};
    }

    if (IsCallableObject(env, value, &methodNameList)) {
        *isObject = true;
        return methodNameList;
    }

    ValueConvertState state;
    bool isObjectForRecursion = false; // FixMe: for Recursion case, now not use
    ParseNapiValue2NwebValueHelper(env, &state, value, nwebValue, &isObjectForRecursion);
    return methodNameList;
}

std::vector<std::string> ParseNapiValue2NwebValue(napi_env env, napi_value* value,
    std::shared_ptr<NWebValue> nwebValue, bool* isObject)
{
    napi_status s = napi_ok;
    std::vector<std::string> methodNameList;

    s = napi_is_promise(env, *value, isObject);
    if (s != napi_ok) {
        WVLOG_E("ParseNapiValue2NwebValue napi api call fail");
    }

    if (*isObject) {
        return std::vector<std::string>{"then", "catch", "finally"};
    }

    if (IsCallableObject(env, *value, &methodNameList)) {
        *isObject = true;
        return methodNameList;
    }

    ValueConvertState state;
    bool isObjectForRecursion = false; // FixMe: for Recursion case, now not use
    ParseNapiValue2NwebValueHelper(env, &state, *value, nwebValue, &isObjectForRecursion);
    return methodNameList;
}
} // namespace

std::shared_ptr<JavaScriptOb> JavaScriptOb::CreateNamed(
    napi_env env, int32_t containerScopeId, napi_value value, size_t refCount)
{
    return std::make_shared<JavaScriptOb>(env, containerScopeId, value, refCount);
}
std::shared_ptr<JavaScriptOb> JavaScriptOb::CreateTransient(
    napi_env env, int32_t containerScopeId, napi_value value, int32_t holder, size_t refCount)
{
    std::set<int32_t> holders;
    holders.insert(holder);
    return std::make_shared<JavaScriptOb>(env, containerScopeId, value, holders, refCount);
}

JavaScriptOb::JavaScriptOb(napi_env env, int32_t containerScopeId, napi_value value, size_t refCount)
    : env_(env), containerScopeId_(containerScopeId), isStrongRef_(refCount != 0), namesCount_(1)
{
    napi_status s = napi_create_reference(env, value, refCount, &objRef_);
    if (s != napi_ok) {
        WVLOG_E("create javascript obj fail");
    }
}

JavaScriptOb::JavaScriptOb(
    napi_env env, int32_t containerScopeId, napi_value value, std::set<int32_t> holders, size_t refCount)
    : env_(env), containerScopeId_(containerScopeId), isStrongRef_(refCount != 0), namesCount_(0), holders_(holders)
{
    std::unique_lock<std::mutex> lock(mutex_);
    napi_status s = napi_create_reference(env, value, refCount, &objRef_);
    if (s != napi_ok) {
        WVLOG_E("create javascript obj fail");
    }
}

WebviewJavaScriptResultCallBack::WebviewJavaScriptResultCallBack(int32_t nwebId)
    : nwebId_(nwebId)
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    g_webviewJsResultCallbackMap.emplace(nwebId, this);
}

WebviewJavaScriptResultCallBack::~WebviewJavaScriptResultCallBack()
{
    std::unique_lock<std::mutex> lk(g_objectMtx);
    g_webviewJsResultCallbackMap.erase(nwebId_);
}

std::shared_ptr<JavaScriptOb> WebviewJavaScriptResultCallBack::FindObject(JavaScriptOb::ObjectID objectId)
{
    auto iter = objects_.find(objectId);
    if (iter != objects_.end()) {
        return iter->second;
    }
    WVLOG_E("WebviewJavaScriptResultCallBack::FindObject Unknown object: objectId = "
            "%{public}d",
        objectId);
    return nullptr;
}

void ProcessObjectCaseInJsTd(
    napi_env env, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param,
    napi_value callResult, std::vector<std::string>& methodNameList)
{
    if (!param) {
        WVLOG_E("WebviewJavaScriptResultCallBack::ProcessObjectCaseInJsTd param null");
        return;
    }

    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);
    auto* outParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*>(param->out);
    JavaScriptOb::ObjectID returnedObjectId;

    if (inParam->webJsResCb->FindObjectIdInJsTd(env, callResult, &returnedObjectId)) {
        inParam->webJsResCb->FindObject(returnedObjectId)->AddHolder(inParam->frameRoutingId);
    } else {
        returnedObjectId = inParam->webJsResCb->AddObject(env, callResult, false, inParam->frameRoutingId);
    }

    inParam->webJsResCb->SetUpAnnotateMethods(returnedObjectId, methodNameList);

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, callResult, &valueType);
    if (valueType == napi_function) {
        WVLOG_D("WebviewJavaScriptResultCallBack::ProcessObjectCaseInJsTd type is function");
        *(static_cast<std::shared_ptr<NWebValue>*>(outParam->ret)) = std::make_shared<NWebValue>();
    } else {
        std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") + std::to_string(returnedObjectId);
        *(static_cast<std::shared_ptr<NWebValue>*>(outParam->ret)) =
            std::make_shared<NWebValue>(bin.c_str(), bin.size());
    }
}

void ExecuteGetJavaScriptResult(
    napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param)
{
    WVLOG_D("WebviewJavaScriptResultCallBack::ExecuteGetJavaScriptResult called");
    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);
    auto* outParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*>(param->out);
    std::shared_ptr<JavaScriptOb> jsObj = inParam->webJsResCb->FindObject(inParam->objId);
    if (!jsObj) {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }
    Ace::ContainerScope containerScope(jsObj->GetContainerScopeId());
    NApiScope scope(env);
    if (!scope.IsVaild()) {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }
    if (jsObj && (jsObj->HasMethod(inParam->methodName))) {
        std::vector<napi_value> argv = {};
        auto nwebArgs = *(static_cast<std::vector<std::shared_ptr<NWebValue>>*>(inParam->data));
        for (std::shared_ptr<NWebValue> input : nwebArgs) {
            argv.push_back(ParseNwebValue2NapiValue(
                env, input, inParam->webJsResCb->GetObjectMap(), inParam->nwebId, inParam->frameRoutingId));
        }
        napi_value callback = jsObj->FindMethod(inParam->methodName);
        if (!callback) {
            WVLOG_E("WebviewJavaScriptResultCallBack::ExecuteGetJavaScriptResult callback null");
        }
        napi_value callResult = nullptr;
        napi_call_function(env, jsObj->GetValue(), callback, argv.size(), &argv[0], &callResult);
        bool isObject = false;
        std::vector<std::string> methodNameList;
        methodNameList = ParseNapiValue2NwebValue(
            env, callResult, *(static_cast<std::shared_ptr<NWebValue>*>(outParam->ret)), &isObject);
        if (isObject) {
            ProcessObjectCaseInJsTd(env, param, callResult, methodNameList);
        }
    }

    std::unique_lock<std::mutex> lock(param->mutex);
    param->ready = true;
    param->condition.notify_all();
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResultSelf(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName,
    int32_t routingId, int32_t objectId)
{
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return ret;
    }
    Ace::ContainerScope containerScope(jsObj->GetContainerScopeId());
    NApiScope scope(jsObj->GetEnv());
    if (!scope.IsVaild()) {
        return ret;
    }

    WVLOG_D("get javaScript result already in js thread");
    std::vector<napi_value> argv = {};
    for (std::shared_ptr<NWebValue> input : args) {
        argv.push_back(ParseNwebValue2NapiValue(jsObj->GetEnv(), input, GetObjectMap(), GetNWebId(), routingId));
    }
    napi_value callback = jsObj->FindMethod(method);
    if (!callback) {
        WVLOG_E("WebviewJavaScriptResultCallBack::ExecuteGetJavaScriptResult callback null");
    }
    napi_value callResult = nullptr;
    napi_call_function(jsObj->GetEnv(), jsObj->GetValue(), callback, argv.size(), &argv[0], &callResult);
    bool isObject = false;
    std::vector<std::string> methodNameList;
    methodNameList = ParseNapiValue2NwebValue(jsObj->GetEnv(), &callResult, ret, &isObject);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(jsObj->GetEnv(), callResult, &valueType);
    WVLOG_D("get javaScript result already in js thread end");
    if (isObject) {
        JavaScriptOb::ObjectID returnedObjectId;
        if (FindObjectIdInJsTd(jsObj->GetEnv(), callResult, &returnedObjectId)) {
            std::shared_ptr<JavaScriptOb> obj = FindObject(returnedObjectId);
            if (obj) {
                obj->AddHolder(routingId);
            }
        } else {
            returnedObjectId = AddObject(jsObj->GetEnv(), callResult, false, routingId);
        }
        SetUpAnnotateMethods(returnedObjectId, methodNameList);
        if (valueType == napi_function) {
            WVLOG_D("WebviewJavaScriptResultCallBack::GetJavaScriptResultSelf type is function");
            ret = std::make_shared<NWebValue>();
        } else {
            WVLOG_D("WebviewJavaScriptResultCallBack::GetJavaScriptResultSelf type is object");
            std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") + std::to_string(returnedObjectId);
            ret = std::make_shared<NWebValue>(bin.c_str(), bin.size());
        }
    }
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResult(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName,
    int32_t routingId, int32_t objectId)
{
    (void)objName; // to be compatible with older webcotroller, classname may be empty
    WVLOG_D(
        "GetJavaScriptResult objName = %{public}s, method = %{public}s, "
        "routingId = %{public}d, objectId = %{public}d",
        objName.c_str(), method.c_str(), routingId, objectId);
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj || !jsObj->HasMethod(method)) {
        return ret;
    }

    auto engine = reinterpret_cast<NativeEngine*>(jsObj->GetEnv());
    if (engine == nullptr) {
        return ret;
    }

    if (pthread_self() == engine->GetTid()) {
        return GetJavaScriptResultSelf(args, method, objName, routingId, objectId);
    } else {
        WVLOG_D("get javaScript result, not in js thread, post task to js thread");
        return PostGetJavaScriptResultToJsThread(args, method, objName, routingId, objectId);
    }
}

char* WebviewJavaScriptResultCallBack::FlowbufStrAtIndex(void* mem, int flowbufIndex, int* argIndex, int* strLen)
{
    int* header = static_cast<int*>(mem); // Cast the memory block to int* for easier access
    int offset = 0;

    if (flowbufIndex >=  MAX_ENTRIES) {
        *argIndex = -1;
        return nullptr;
    }

    int* entry = header + (flowbufIndex * INDEX_SIZE);
    if (*(entry + 1) == 0) { // Check if length is 0, indicating unused entry
        *argIndex = -1;
        return nullptr;
    }

    int i = 0;
    for (i = 0; i < flowbufIndex; i++) {
        offset += *(header + (i * INDEX_SIZE) + 1);
    }

    *strLen = *(header + (i * INDEX_SIZE) + 1) - 1;

    *argIndex = *entry;

    char* dataSegment = static_cast<char*>(mem) + HEADER_SIZE;
    char* currentString = dataSegment + offset;
    return currentString;
}

bool WebviewJavaScriptResultCallBack::ConstructArgv(void* ashmem,
    std::vector<std::shared_ptr<NWebValue>> args,
    std::vector<napi_value>& argv,
    std::shared_ptr<JavaScriptOb> jsObj,
    int32_t routingId)
{
    int argIndex = -1;
    int currIndex = 0;
    int flowbufIndex = 0;
    int strLen = 0;
    char* flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
    flowbufIndex++;
    while (argIndex == currIndex) {
        napi_value napiValue = nullptr;
        napi_status s = napi_ok;
        s = napi_create_string_utf8(jsObj->GetEnv(), flowbufStr, strLen, &napiValue);
        if (s != napi_ok) {
            WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
            return false;
        }
        argv.push_back(napiValue);
        currIndex++;
        flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
        flowbufIndex++;
    }

    for (std::shared_ptr<NWebValue> input : args) {
        while (argIndex == currIndex) {
            napi_value napiValue = nullptr;
            napi_status s = napi_ok;
            s = napi_create_string_utf8(jsObj->GetEnv(), flowbufStr, strLen, &napiValue);
            if (s != napi_ok) {
                WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
            }
            argv.push_back(napiValue);
            currIndex ++;
            flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
            flowbufIndex++;
        }

        argv.push_back(ParseNwebValue2NapiValue(jsObj->GetEnv(), input, GetObjectMap(), GetNWebId(), routingId));
        currIndex++;
    }

    while (argIndex == currIndex) {
        napi_value napiValue = nullptr;
        napi_status s = napi_ok;
        s = napi_create_string_utf8(jsObj->GetEnv(), flowbufStr, strLen, &napiValue);
        if (s != napi_ok) {
            WVLOG_E("ParseBasicTypeNwebValue2NapiValue napi api call fail");
        }
        argv.push_back(napiValue);
        currIndex++;
        flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
        flowbufIndex++;
    }
    return true;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResultSelfHelper(
    std::shared_ptr<JavaScriptOb> jsObj,
    const std::string& method,
    int32_t routingId,
    napi_handle_scope scope,
    std::vector<napi_value> argv)
{
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    if (!jsObj) {
        WVLOG_E("WebviewJavaScriptResultCallBack::ExecuteGetJavaScriptResult jsObj null");
        return ret;
    }
    napi_value callback = jsObj->FindMethod(method);
    if (!callback) {
        WVLOG_E("WebviewJavaScriptResultCallBack::ExecuteGetJavaScriptResult callback null");
    }
    napi_value callResult = nullptr;
    napi_call_function(jsObj->GetEnv(), jsObj->GetValue(), callback, argv.size(), &argv[0], &callResult);
    bool isObject = false;
    std::vector<std::string> methodNameList;
    methodNameList = ParseNapiValue2NwebValue(jsObj->GetEnv(), &callResult, ret, &isObject);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(jsObj->GetEnv(), callResult, &valueType);
    WVLOG_D("get javaScript result already in js thread end");
    if (!isObject) {
        napi_close_handle_scope(jsObj->GetEnv(), scope);
        return ret;
    }
    JavaScriptOb::ObjectID returnedObjectId;
    if (FindObjectIdInJsTd(jsObj->GetEnv(), callResult, &returnedObjectId) && FindObject(returnedObjectId)) {
        FindObject(returnedObjectId)->AddHolder(routingId);
    } else {
        returnedObjectId = AddObject(jsObj->GetEnv(), callResult, false, routingId);
    }
    SetUpAnnotateMethods(returnedObjectId, methodNameList);
    if (valueType == napi_function) {
        WVLOG_D("WebviewJavaScriptResultCallBack::GetJavaScriptResultSelf type is function");
        ret = std::make_shared<NWebValue>();
    } else {
        WVLOG_D("WebviewJavaScriptResultCallBack::GetJavaScriptResultSelf type is object");
        std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") + std::to_string(returnedObjectId);
        ret = std::make_shared<NWebValue>(bin.c_str(), bin.size());
    }
    napi_close_handle_scope(jsObj->GetEnv(), scope);
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResultSelfFlowbuf(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName, int fd,
    int32_t routingId, int32_t objectId)
{
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return ret;
    }
    NApiScope scope(jsObj->GetEnv());
    if (!scope.IsVaild()) {
        return ret;
    }
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        return ret;
    }
    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, MAX_FLOWBUF_DATA_SIZE + HEADER_SIZE, PROT_READ);
    if (!ashmem) {
        return ret;
    }

    std::vector<napi_value> argv = {};
    if (!ConstructArgv(ashmem, args, argv, jsObj, routingId)) {
    	return ret;
    }
    close(fd);

    ret = GetJavaScriptResultSelfHelper(jsObj, method, routingId, scope.scope_, argv);
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptResultFlowbuf(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName, int fd,
    int32_t routingId, int32_t objectId)
{
    (void)objName; // to be compatible with older webcotroller, classname may be empty
    WVLOG_D("GetJavaScriptResult method = %{public}s", method.c_str());
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj || !jsObj->HasMethod(method)) {
        return ret;
    }

    auto engine = reinterpret_cast<NativeEngine*>(jsObj->GetEnv());
    if (engine == nullptr) {
        return ret;
    }

    if (pthread_self() == engine->GetTid()) {
        return GetJavaScriptResultSelfFlowbuf(args, method, objName, fd, routingId, objectId);
    } else {
        auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
        if (!flowbufferAdapter) {
            return ret;
        }

        auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, MAX_FLOWBUF_DATA_SIZE + HEADER_SIZE, PROT_READ);
        if (!ashmem) {
            return ret;
        }

        int argIndex = -1;
        int flowbufIndex = 0;
        int strLen = 0;
        do {
            char* flowbufStr = FlowbufStrAtIndex(ashmem, flowbufIndex, &argIndex, &strLen);
            if (argIndex == -1) {
                break;
            }
            flowbufIndex++;
            std::string str(flowbufStr);
            std::shared_ptr<NWebValue> insertArg = std::make_shared<NWebValue>(str);
            args.insert(args.begin() + argIndex, insertArg);
        } while (argIndex <= MAX_ENTRIES);

        close(fd);
        WVLOG_D("get javaScript result, not in js thread, post task to js thread");
        return PostGetJavaScriptResultToJsThread(args, method, objName, routingId, objectId);
    }
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::PostGetJavaScriptResultToJsThread(
    std::vector<std::shared_ptr<NWebValue>> args, const std::string& method, const std::string& objName,
    int32_t routingId, int32_t objectId)
{
    // to be compatible with older webcotroller, classname may be empty
    (void)objName;
    WVLOG_D("WebviewJavaScriptResultCallBack::PostGetJavaScriptResultToJsThread method = "
            "%{public}s",
        method.c_str());
    std::shared_ptr<NWebValue> ret = std::make_shared<NWebValue>(NWebValue::Type::NONE);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return ret;
    }
    napi_env env = jsObj->GetEnv();
    WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param = nullptr;
    if (!CreateNapiJsCallBackParm(inParam, outParam, param)) {
        WVLOG_E("WebviewJavaScriptResultCallBack::PostGetJavaScriptResultToJsThread malloc fail");
        return ret;
    }

    inParam->webJsResCb = this;
    inParam->nwebId = GetNWebId();
    inParam->frameRoutingId = routingId;
    inParam->objId = objectId;
    inParam->methodName = method;
    inParam->data = reinterpret_cast<void*>(&args);
    outParam->ret = reinterpret_cast<void*>(&ret);
    param->input = reinterpret_cast<void*>(inParam);
    param->out = reinterpret_cast<void*>(outParam);
    param->env = env;

    CreateUvQueueWorkEnhanced(env, param, ExecuteGetJavaScriptResult);
    {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->condition.wait(lock, [&param] { return param->ready; });
    }
    DeleteNapiJsCallBackParm(inParam, outParam, param);
    return ret;
}

bool WebviewJavaScriptResultCallBack::FindObjectIdInJsTd(
    napi_env env, napi_value object, JavaScriptOb::ObjectID* objectId)
{
    *objectId = static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBVIEWCONTROLLERERROR);
    for (const auto& pair : objects_) {
        bool result = false;
        napi_status s = napi_strict_equals(env, object, pair.second->GetValue(), &result);
        if (s != napi_ok) {
            WVLOG_E("WebviewJavaScriptResultCallBack::FindObjectIdInJsTd fail");
        }
        if (result) {
            *objectId = pair.first;
            return true;
        }
    }
    return false;
}

void ExecuteHasJavaScriptObjectMethods(
    napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param)
{
    if (!param) {
        return;
    }

    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);
    auto* outParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*>(param->out);
    std::shared_ptr<JavaScriptOb> jsObj = inParam->webJsResCb->FindObject(inParam->objId);
    if (!jsObj) {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }
    Ace::ContainerScope containerScope(jsObj->GetContainerScopeId());

    NApiScope scope(env);
    if (scope.scope_) {
        if (jsObj && jsObj->HasMethod(inParam->methodName)) {
            *(static_cast<bool*>(outParam->ret)) = true;
        } else {
            WVLOG_D("WebviewJavaScriptResultCallBack::HasJavaScriptObjectMethods cannot find "
                    "object");
        }
    }

    std::unique_lock<std::mutex> lock(param->mutex);
    param->ready = true;
    param->condition.notify_all();
}

bool WebviewJavaScriptResultCallBack::PostHasJavaScriptObjectMethodsToJsThread(
    int32_t objectId, const std::string& methodName)
{
    bool ret = false;
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return false;
    }
    napi_env env = jsObj->GetEnv();
    WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param = nullptr;
    if (!CreateNapiJsCallBackParm(inParam, outParam, param)) {
        return false;
    }

    inParam->webJsResCb = this;
    inParam->objId = objectId;
    inParam->methodName = methodName;
    outParam->ret = reinterpret_cast<void*>(&ret);
    param->input = reinterpret_cast<void*>(inParam);
    param->out = reinterpret_cast<void*>(outParam);
    param->env = env;

    CreateUvQueueWorkEnhanced(env, param, ExecuteHasJavaScriptObjectMethods);

    {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->condition.wait(lock, [&param] { return param->ready; });
    }
    DeleteNapiJsCallBackParm(inParam, outParam, param);
    return ret;
}

bool WebviewJavaScriptResultCallBack::HasJavaScriptObjectMethods(int32_t objectId, const std::string& methodName)
{
    bool ret = false;
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return false;
    }
    napi_env env = jsObj->GetEnv();
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (engine == nullptr) {
        return ret;
    }
    if (pthread_self() == engine->GetTid()) {
        WVLOG_D(
            "has javaScript object methods already in js thread, objectId = "
            "%{public}d, methodName = %{public}s",
            objectId, methodName.c_str());
        NApiScope scope(env);
        if (!scope.IsVaild()) {
            return ret;
        }

        if (jsObj && jsObj->HasMethod(methodName)) {
            ret = true;
        } else {
            WVLOG_D("WebviewJavaScriptResultCallBack::HasJavaScriptObjectMethods cannot find "
                    "object");
        }

        return ret;
    } else {
        WVLOG_D(
            "has javaScript object methods, not in js thread, post task to js "
            "thread, objectId = "
            "%{public}d, methodName = %{public}s",
            objectId, methodName.c_str());
        return PostHasJavaScriptObjectMethodsToJsThread(objectId, methodName);
    }
}

void ExecuteGetJavaScriptObjectMethods(
    napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param)
{
    if (!param) {
        return;
    }

    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);
    auto* outParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*>(param->out);

    std::shared_ptr<JavaScriptOb> jsObj = inParam->webJsResCb->FindObject(inParam->objId);
    if (!jsObj) {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }
    Ace::ContainerScope containerScope(jsObj->GetContainerScopeId());

    NApiScope scope(env);

    if (scope.scope_) {
        if (jsObj) {
            auto methods = jsObj->GetMethodNames();
            for (auto& method : methods) {
                (*(static_cast<std::shared_ptr<NWebValue>*>(outParam->ret)))->AddListValue(NWebValue(method));
            }
        }
    }

    std::unique_lock<std::mutex> lock(param->mutex);
    param->ready = true;
    param->condition.notify_all();
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::PostGetJavaScriptObjectMethodsToJsThread(int32_t objectId)
{
    auto ret = std::make_shared<NWebValue>(NWebValue::Type::LIST);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return ret;
    }
    napi_env env = jsObj->GetEnv();
    WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param = nullptr;
    if (!CreateNapiJsCallBackParm(inParam, outParam, param)) {
        return ret;
    }

    inParam->webJsResCb = this;
    inParam->objId = objectId;
    outParam->ret = reinterpret_cast<void*>(&ret);
    param->input = reinterpret_cast<void*>(inParam);
    param->out = reinterpret_cast<void*>(outParam);
    param->env = env;

    CreateUvQueueWorkEnhanced(env, param, ExecuteGetJavaScriptObjectMethods);

    {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->condition.wait(lock, [&param] { return param->ready; });
    }
    DeleteNapiJsCallBackParm(inParam, outParam, param);
    return ret;
}

std::shared_ptr<NWebValue> WebviewJavaScriptResultCallBack::GetJavaScriptObjectMethods(int32_t objectId)
{
    auto ret = std::make_shared<NWebValue>(NWebValue::Type::LIST);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return ret;
    }
    napi_env env = jsObj->GetEnv();
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (engine == nullptr) {
        return ret;
    }

    if (pthread_self() == engine->GetTid()) {
        WVLOG_D(
            "get javaScript object methods already in js thread, objectId = "
            "%{public}d",
            objectId);
        NApiScope scope(env);
        if (!scope.IsVaild()) {
            return ret;
        }

        if (jsObj) {
            auto methods = jsObj->GetMethodNames();
            for (auto& method : methods) {
                ret->AddListValue(NWebValue(method));
            }
        }

        return ret;
    } else {
        WVLOG_D(
            "get javaScript object methods, not in js thread, post task to js "
            "thread, objectId = %{public}d",
            objectId);
        return PostGetJavaScriptObjectMethodsToJsThread(objectId);
    }
}

void WebviewJavaScriptResultCallBack::RemoveJavaScriptObjectHolderInJsTd(
    int32_t holder, JavaScriptOb::ObjectID objectId)
{
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (jsObj && !(jsObj->IsNamed())) {
        jsObj->RemoveHolder(holder);
        if (!(jsObj->HasHolders())) {
            // reminder me: object->ToWeakRef(), object is erased so the destructor
            // called
            retainedObjectSet_.erase(jsObj);
            objects_.erase(objects_.find(objectId));
        }
    }
}

void ExecuteRemoveJavaScriptObjectHolder(
    napi_env env, napi_status status, WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param)
{
    if (!param) {
        return;
    }

    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);

    std::shared_ptr<JavaScriptOb> jsObj = inParam->webJsResCb->FindObject(inParam->objId);
    if (!jsObj) {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }
    Ace::ContainerScope containerScope(jsObj->GetContainerScopeId());

    NApiScope scope(env);

    if (scope.scope_) {
        inParam->webJsResCb->RemoveJavaScriptObjectHolderInJsTd(inParam->frameRoutingId, inParam->objId);
    }

    std::unique_lock<std::mutex> lock(param->mutex);
    param->ready = true;
    param->condition.notify_all();
}

void WebviewJavaScriptResultCallBack::PostRemoveJavaScriptObjectHolderToJsThread(
    int32_t holder, JavaScriptOb::ObjectID objectId)
{
    WVLOG_D("WebviewJavaScriptResultCallBack::RemoveJavaScriptObjectHolder called, "
            "objectId = %{public}d",
        objectId);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return;
    }
    napi_env env = jsObj->GetEnv();
    WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackParm* param = nullptr;
    if (!CreateNapiJsCallBackParm(inParam, outParam, param)) {
        return;
    }

    inParam->webJsResCb = this;
    inParam->objId = objectId;
    inParam->frameRoutingId = holder;
    param->input = reinterpret_cast<void*>(inParam);
    param->out = reinterpret_cast<void*>(outParam);
    param->env = env;

    CreateUvQueueWorkEnhanced(env, param, ExecuteRemoveJavaScriptObjectHolder);

    {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->condition.wait(lock, [&param] { return param->ready; });
    }
    DeleteNapiJsCallBackParm(inParam, outParam, param);
}

void WebviewJavaScriptResultCallBack::RemoveJavaScriptObjectHolder(int32_t holder, JavaScriptOb::ObjectID objectId)
{
    WVLOG_D("WebviewJavaScriptResultCallBack::RemoveJavaScriptObjectHolder called, "
            "objectId = %{public}d",
        objectId);
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return;
    }
    napi_env env = jsObj->GetEnv();
    auto engine = reinterpret_cast<NativeEngine*>(env);
    if (engine == nullptr) {
        return;
    }
    if (pthread_self() == engine->GetTid()) {
        WVLOG_D("remove javaScript object holder already in js thread");
        NApiScope scope(env);
        if (!scope.IsVaild()) {
            return;
        }

        RemoveJavaScriptObjectHolderInJsTd(holder, objectId);

        return;
    } else {
        WVLOG_D("remove javaScript object holder, not in js thread, post task to js thread");
        PostRemoveJavaScriptObjectHolderToJsThread(holder, objectId);
    }
}

void WebviewJavaScriptResultCallBack::RemoveTransientJavaScriptObject()
{
    // remove retainedObjectSet_ and objects_ CreateTransient object
    auto iter = objects_.begin();
    while (iter != objects_.end()) {
        if (!(iter->second->IsNamed())) {
            WVLOG_D("WebviewJavaScriptResultCallBack::RemoveTransientJavaScriptObject "
                    "objectId = %{public}d  is removed",
                (int32_t)iter->first);
            // reminder me: object->ToWeakRef(), object is erased so the destructor called
            retainedObjectSet_.erase(iter->second);
            objects_.erase(iter++);
        } else {
            ++iter;
        }
    }

    // remove retainedObjectSet_ named object but not in objects_
    auto iter1 = retainedObjectSet_.begin();
    while (iter1 != retainedObjectSet_.end()) {
        auto iter2 = objects_.begin();
        bool isHasObj = false;
        while (iter2 != objects_.end()) {
            if (*iter1 == iter2->second) {
                isHasObj = true;
                break;
            }
            ++iter2;
        }
        if (!isHasObj) {
            WVLOG_D("WebviewJavaScriptResultCallBack::RemoveTransientJavaScriptObject "
                    "isHasObj == false");
            retainedObjectSet_.erase(*iter1);
        }
        ++iter1;
    }
}

void WebviewJavaScriptResultCallBack::SetUpAnnotateMethods(
    JavaScriptOb::ObjectID objId, std::vector<std::string>& methodNameList)
{
    // set up annotate(methodNameListForJsProxy) object method
    if (objects_[objId]) {
        objects_[objId]->SetMethods(methodNameList);
    }
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBack::AddObject(
    napi_env env, const napi_value& object, bool methodName, int32_t holder)
{
    JavaScriptOb::ObjectID objectId;
    {
        int32_t containerScopeId = Ace::ContainerScope::CurrentId();
        auto new_object = methodName ? JavaScriptOb::CreateNamed(env, containerScopeId, object)
                                     : JavaScriptOb::CreateTransient(env, containerScopeId, object, holder);
        objectId = nextObjectId_++;
        WVLOG_D(
            "WebviewJavaScriptResultCallBack::AddObject objectId = "
            "%{public}d",
            static_cast<int32_t>(objectId));
        objects_[objectId] = new_object;
        retainedObjectSet_.insert(new_object);
    }
    return objectId;
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBack::AddNamedObject(
    napi_env env, napi_value& obj, const std::string& objName)
{
    JavaScriptOb::ObjectID objectId;
    NamedObjectMap::iterator iter = namedObjects_.find(objName);
    bool methodName = FindObjectIdInJsTd(env, obj, &objectId);
    if (methodName && iter != namedObjects_.end() && iter->second == objectId) {
        // Nothing to do.
        WVLOG_D(
            "WebviewJavaScriptResultCallBack::AddNamedObject obj and "
            "objName(%{public}s) "
            "already exist",
            objName.c_str());
        return objectId;
    }
    if (iter != namedObjects_.end()) {
        RemoveNamedObject(iter->first);
    }
    if (methodName) {
        objects_[objectId]->AddName();
    } else {
        objectId = AddObject(env, obj, true, 0);
    }
    namedObjects_[objName] = objectId;
    return objectId;
}

std::unordered_map<std::string, std::shared_ptr<JavaScriptOb>> WebviewJavaScriptResultCallBack::GetNamedObjects()
{
    // Get named objects
    std::unordered_map<std::string, std::shared_ptr<JavaScriptOb>> ret;
    for (auto iter = namedObjects_.begin(); iter != namedObjects_.end(); iter++) {
        if (objects_.find(iter->second) != objects_.end()) {
            ret.emplace(iter->first, objects_[iter->second]);
        }
    }
    return ret;
}

WebviewJavaScriptResultCallBack::ObjectMap WebviewJavaScriptResultCallBack::GetObjectMap()
{
    return objects_;
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBack::RegisterJavaScriptProxy(
    RegisterJavaScriptProxyParam& param)
{
    JavaScriptOb::ObjectID objId = AddNamedObject(param.env, param.obj, param.objName);
    // set up named object method
    if (namedObjects_.find(param.objName) != namedObjects_.end() && objects_[namedObjects_[param.objName]]) {
        std::shared_ptr<OHOS::NWeb::JavaScriptOb> jsObj = objects_[namedObjects_[param.objName]];
        jsObj->SetMethods(param.syncMethodList);
        jsObj->SetAsyncMethods(param.asyncMethodList);
        jsObj->SetPermission(param.permission);
    }
    for (auto& item : param.syncMethodList) {
        WVLOG_D(
            "WebviewJavaScriptResultCallBack::RegisterJavaScriptProxy called, "
            "objectId = %{public}d, objName = %{public}s, method = %{public}s",
            static_cast<int32_t>(objId), param.objName.c_str(), item.c_str());
    }
    return objId;
}

bool WebviewJavaScriptResultCallBack::RemoveNamedObject(const std::string& name)
{
    WVLOG_D("WebviewJavaScriptResultCallBack::RemoveNamedObject called, "
            "name = %{public}s",
        name.c_str());
    NamedObjectMap::iterator iter = namedObjects_.find(name);
    if (iter == namedObjects_.end()) {
        return false;
    }
    if (objects_[iter->second]) {
        objects_[iter->second]->RemoveName();
    }
    namedObjects_.erase(iter);
    return true;
}

bool WebviewJavaScriptResultCallBack::DeleteJavaScriptRegister(const std::string& objName)
{
    return RemoveNamedObject(objName);
}

void WebviewJavaScriptResultCallBack::CallH5FunctionInternal(
    napi_env env, H5Bundle& bundle, std::vector<std::shared_ptr<NWebValue>> nwebArgs)
{
    if (bundle.nwebId != GetNWebId()) {
        WVLOG_E("WebviewJavaScriptResultCallBack::CallH5FunctionInternal nweb id not equal");
        return;
    }
    auto nweb_ptr = NWebHelper::Instance().GetNWeb(nwebId_);
    if (!nweb_ptr) {
        WVLOG_E("WebviewJavaScriptResultCallBack::CallH5FunctionInternal nweb_ptr null");
        return;
    }

    nweb_ptr->CallH5Function(bundle.frameRoutingId, bundle.h5Id, bundle.funcName, nwebArgs);
    WVLOG_D("WebviewJavaScriptResultCallBack::CallH5FunctionInternal end");
}

void WebviewJavaScriptResultCallBack::UpdateInstanceId(int32_t newId)
{
    for (const auto& [nwebId, obj] : objects_) {
        obj->SetContainerScopeId(newId);
    }
}

} // namespace OHOS::NWeb
