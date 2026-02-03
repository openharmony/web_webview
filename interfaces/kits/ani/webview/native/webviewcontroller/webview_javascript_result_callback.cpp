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
#include "native_engine/native_engine.h"
#include "nweb_helper.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "web_errors.h"

namespace OHOS::NWeb {
namespace {
#define JS_BRIDGE_BINARY_ARGS_COUNT 2

const int MAX_FLOWBUF_DATA_SIZE = 52428800; /* 50MB*/
const int MAX_ENTRIES = 10;
const int HEADER_SIZE = (MAX_ENTRIES * 8);  /* 10 * (int position + int length) */
const int INDEX_SIZE = 2;
const std::string TASK_ID = "javascriptcallback";
constexpr int32_t REFERENCES_MAX_NUMBER = 16;

// For the sake of the storage API, make this quite large.
const uint32_t MAX_RECURSION_DEPTH = 11;
const uint32_t MAX_DATA_LENGTH = 10000;

std::unordered_map<int32_t, WebviewJavaScriptResultCallBack*> g_webviewJsResultCallbackMap;
std::mutex g_objectMtx;

std::vector<std::string> ParseNapiValue2NwebValue(napi_env env, napi_value& value,
    std::shared_ptr<NWebValue> nwebValue, bool* isObject);
std::vector<std::string> ParseNapiValue2NwebValue(napi_env env, napi_value* value,
    std::shared_ptr<NWebValue> nwebValue, bool* isObject);

std::vector<std::string> ParseAniValue2NwebValue(ani_env* env, ani_object* value,
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

template<typename T>
ani_object ParseNwebValue2AniValueV2(ani_env* env, std::shared_ptr<T> value,
    WebviewJavaScriptResultCallBack::ObjectMap objectsMap, int32_t nwebId, int32_t frameId, int32_t id);

template<typename T>
std::vector<std::string> ParseAniValue2NwebValueV2(
    ani_env* env, ani_object* value, std::shared_ptr<T> nwebValue, bool* isObject);

void ParseAniValue2NwebValueHelper(
    ani_env* env, ValueConvertState* state, ani_object& value, std::shared_ptr<NWebValue> nwebValue, bool* isOject);

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

void CallH5FunctionV2(ani_env* env, std::shared_ptr<NWebValue> nwebValue, ani_object object,
    WebviewJavaScriptResultCallBack::H5Bundle bundle)
{
    WVLOG_D("CallH5FunctionV2 called");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    bool isObject = false;
    std::vector<std::string> methodNameList;
    methodNameList = ParseAniValue2NwebValue(env, &object, nwebValue, &isObject);
    auto fromNwebID = FromNwebID(bundle.nwebId);
    if (isObject && fromNwebID) {
        JavaScriptOb::ObjectID returnedObjectId;
        if (fromNwebID->FindObjectIdInJsTd(env, object, &returnedObjectId)) {
            fromNwebID->FindObject(returnedObjectId)->AddHolder(bundle.frameRoutingId);
        } else {
            returnedObjectId = fromNwebID->AddObject(env, object, false, bundle.frameRoutingId);
        }

        fromNwebID->SetUpAnnotateMethods(returnedObjectId, methodNameList);
    }
}

ani_ref CallbackFunctionForH5V2(ani_env* env, ani_object object, void* data, ani_double argv, ani_array args)
{
    WebviewJavaScriptResultCallBack::H5Bundle bundle =
        *reinterpret_cast<WebviewJavaScriptResultCallBack::H5Bundle*>(data);
    ani_status s = ANI_OK;
    std::vector<std::shared_ptr<NWebValue>> nwebArgs;
    ani_size size = 0;

    if (!env || argv <= 0) {
        WVLOG_E("CallbackFunctionForH5V2 env is null");
        return nullptr;
    }

    ani_boolean isArray = ANI_FALSE;
    ani_class cls;
    s = env->FindClass("std.core.Array;", &cls);
    if (s != ANI_OK || !cls) {
        WVLOG_E("CallbackFunctionForH5V2 Array class fail");
        return nullptr;
    }
    s = env->Object_InstanceOf(static_cast<ani_object>(args), cls, &isArray);
    if (s != ANI_OK || !isArray) {
        WVLOG_E("CallbackFunctionForH5V2 args must be array");
        return nullptr;
    }
    s = env->Array_GetLength(args, &size);
    if (s != ANI_OK) {
        WVLOG_E("CallbackFunctionForH5V2 array length fail");
        size = 0;
    }

    uint32_t arraySize = uint32_t(size);
    ani_array argsRef = static_cast<ani_array>(args);
    for (uint32_t i = 0; i < arraySize; i++) {
        ani_ref aniTmp {};
        s = env->Array_Get(argsRef, i, &aniTmp);
        if (s != ANI_OK) {
            WVLOG_E("CallbackFunctionForH5V2 get element call fail");
            continue;
        }

        ani_object object = static_cast<ani_object>(aniTmp);
        std::shared_ptr<NWebValue> nwebArg = std::make_shared<NWebValue>(NWebValue::Type::NONE);
        CallH5FunctionV2(env, nwebArg, object, bundle);
        nwebArgs.push_back(nwebArg);
    }

    if (FromNwebID(bundle.nwebId)) {
        FromNwebID(bundle.nwebId)->CallH5FunctionInternal(env, bundle, nwebArgs);
    }
    return nullptr;
}

ani_object CreateFunctionForH5(ani_env* env, int32_t nwebId, int32_t frameRoutingId, int32_t h5ObjectId,
    std::string funcName, ani_class jsProxyCallbackCls, ani_object callObj)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    // Create a bundle.
    auto bundle = std::make_unique<OHOS::NWeb::WebviewJavaScriptResultCallBack::H5Bundle>();
    bundle->nwebId = nwebId;
    bundle->frameRoutingId = frameRoutingId;
    bundle->h5Id = h5ObjectId;
    bundle->funcName = funcName;

    ani_status status = ANI_OK;
    ani_method method {};
    status = env->Class_FindMethod(jsProxyCallbackCls, "getCallback", nullptr, &method);
    if (status != ANI_OK) {
        WVLOG_E("CreateFunctionForH5 FindMethod call fail");
        return nullptr;
    }
    ani_ref callFuc {};
    status = env->Object_CallMethod_Ref(callObj, method, &callFuc, reinterpret_cast<void*>(bundle.release()));
    if (status != ANI_OK) {
        WVLOG_E("CreateFunctionForH5 CallMethod fail");
        return nullptr;
    }
    return static_cast<ani_object>(callFuc);
}

void AddFunctionToObjectForH5(
    ani_env* env, OHOS::NWeb::WebviewJavaScriptResultCallBack::H5Bundle& bundle, ani_class callbackCls, ani_object obj)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!obj || bundle.frameRoutingId < 0) {
        WVLOG_E("AddFunctionToObjectForH5 obj or frame id error");
        return;
    }
    ani_object func =
        CreateFunctionForH5(env, bundle.nwebId, bundle.frameRoutingId, bundle.h5Id, bundle.funcName, callbackCls, obj);
    if (!func) {
        WVLOG_E("AddFunctionToObjectForH5 create func fail");
        return;
    }
}

ani_class CreateProxyForH5Object(ani_env* env, ani_object* result)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_status status = ANI_OK;
    constexpr const char* className = "@ohos.web.webview.webview.jsProxyCallbackWrapper";
    ani_class jsProxyCallbackCls = nullptr;
    status = env->FindClass(className, &jsProxyCallbackCls);
    if (status != ANI_OK || !jsProxyCallbackCls) {
        WVLOG_E("CreateProxyForH5Object find class failed, status: %{public}d", status);
        return nullptr;
    }
    std::array methods = {
        ani_native_function { "callNativeFunc", nullptr, reinterpret_cast<void*>(CallbackFunctionForH5V2) },
    };
    status = env->Class_BindNativeMethods(jsProxyCallbackCls, methods.data(), methods.size());
    if (status != ANI_OK) {
        WVLOG_E("CreateProxyForH5Object BindNativeMethods fail");
        return nullptr;
    }
    ani_method ctor {};
    status = env->Class_FindMethod(jsProxyCallbackCls, "<ctor>", nullptr, &ctor);
    if (status != ANI_OK) {
        WVLOG_E("CreateProxyForH5Object FindMethod call fail");
        return nullptr;
    }
    ani_object callObj {};
    status = env->Object_New(jsProxyCallbackCls, ctor, &callObj, nullptr);
    if (status != ANI_OK) {
        WVLOG_E("CreateProxyForH5Object create object call fail");
        return nullptr;
    }
    *result = callObj;
    return jsProxyCallbackCls;
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
    int ret = uv_queue_work_with_qos_internal(
        loop, work, [](uv_work_t* work) {}, callback, uv_qos_user_initiated,
        "WebviewCreateUvQueueWorkEnhanced_ani");
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

ani_ref AniCreateMapObject(ani_env* env, WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t id)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto iter = objectsMap.find(id);
    if (iter != objectsMap.end()) {
        std::shared_ptr<JavaScriptOb> jsObj = iter->second;

        ani_ref callResult = nullptr;
        ani_object webviewObj = static_cast<ani_object>(jsObj->GetWebviewObject());
        ani_status s = env->Object_CallMethodByName_Ref(webviewObj, "getReferenceMap", nullptr, &callResult);
        if (ANI_OK != s || !callResult) {
            WVLOG_E("AniCreateMapObject call method fail,status: %{public}d", s);
        }
        return callResult;
    }
    return nullptr;
}

void AniSetFieldForMapObject(ani_env* env, WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, ani_object target,
    ani_string name, ani_object value, int32_t id)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    auto iter = objectsMap.find(id);
    if (iter != objectsMap.end()) {
        std::shared_ptr<JavaScriptOb> jsObj = iter->second;

        ani_ref callResult;
        ani_object webviewObj = static_cast<ani_object>(jsObj->GetWebviewObject());
        ani_status s = env->Object_CallMethodByName_Ref(
            webviewObj, "setFieldForObject", nullptr, &callResult, target, name, value);
        if (ANI_OK != s || !callResult) {
            WVLOG_E("AnisetFieldForMapObject call method fail,status: %{public}d", s);
        }
        return;
    }
    return;
}

bool AniIsInt(ani_env* env, ani_object& object)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class intClass;
    if (env->FindClass("std.core.Int;", &intClass) != ANI_OK) {
        WVLOG_E("ParseInt32 failed - invalid FindClass int type");
        return false;
    }
    ani_boolean isInt;
    if (env->Object_InstanceOf(object, intClass, &isInt) != ANI_OK || isInt != ANI_TRUE) {
        WVLOG_E("ParseInt32 failed - invalid int type");
        return false;
    }
    return isInt;
}

bool AniIsArray(ani_env* env, ani_object arrayObject)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_class cls;
    ani_boolean isArray = ANI_FALSE;
    ani_status status = env->FindClass("std.core.Array;", &cls);
    if (status != ANI_OK) {
        WVLOG_E("Failed to find class std.core.Array;");
        return false;
    }
    status = env->Object_InstanceOf(arrayObject, cls, &isArray);
    if (status != ANI_OK) {
        WVLOG_E("Failed to find check instance of array");
        return false;
    }
    if (!isArray) {
        WVLOG_E("object must be array");
        return false;
    }
    return true;
}

bool AniGetElement(ani_env* env, ani_array value, uint32_t i, ani_ref* aniTmp)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_status s = env->Array_Get(value, i, aniTmp);
    if (s != ANI_OK) {
        WVLOG_E("get element call fail");
        return false;
    }
    ani_boolean isNull;
    s = env->Reference_IsNull(*aniTmp, &isNull);
    if (s != ANI_OK) {
        WVLOG_E("element is null check fail");
        return false;
    }
    if (isNull) {
        WVLOG_E("element is null");
        return false;
    }
    return true;
}

uint32_t AniGetArrayLength(ani_env* env, ani_array value)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return 0;
    }
    ani_size size = 0;
    ani_status s = env->Array_GetLength(value, &size);
    if (s != ANI_OK) {
        WVLOG_E("array length fail");
    }
    return uint32_t(size);
}

ani_array ConvertAniArrayFromVecterObject(ani_env* env, std::vector<ani_object>& list)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined failed");
        return nullptr;
    }
    ani_array arrayResult;
    if (ANI_OK != env->Array_New(list.size(), undefinedRef, &arrayResult)) {
        WVLOG_E("ConvertArrayOfVecterObject  new array ref error.");
        return nullptr;
    }
    int i = 0;
    for (auto& item : list) {
        ani_object objValue = item;
        ani_ref refValue = static_cast<ani_ref>(objValue);
        if (ANI_OK != env->Array_Set(arrayResult, i++, refValue)) {
            WVLOG_E("ConvertArrayOfVecterObject array set fail");
        }
    }
    return arrayResult;
}

void ProcessObjectCaseInJsTdV2(ani_env* env, WebviewJavaScriptResultCallBack::AniJsCallBackParm* param,
    ani_object callResult, std::vector<std::string>& methodNameList, std::shared_ptr<NWebHapValue> result)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!param) {
        WVLOG_E("ProcessObjectCaseInJsTdV2 param null");
        return;
    }

    JavaScriptOb::ObjectID returnedObjectId;
    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);
    if (inParam->webJsResCb->FindObjectIdInJsTd(env, callResult, &returnedObjectId)) {
        inParam->webJsResCb->FindObject(returnedObjectId)->AddHolder(inParam->frameRoutingId);
    } else {
        returnedObjectId = inParam->webJsResCb->AddObject(env, callResult, false, inParam->frameRoutingId);
    }

    inParam->webJsResCb->SetUpAnnotateMethods(returnedObjectId, methodNameList);
    bool isFunction = AniParseUtils::IsFunction(env, callResult);
    if (isFunction) {
        WVLOG_D("ProcessObjectCaseInJsTd type is function");
    } else {
        WVLOG_D("ProcessObjectCaseInJsTd type is TYPE_OBJECT_ID");
        std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") + std::to_string(returnedObjectId);
        result->SetType(NWebHapValue::Type::BINARY);
        result->SetBinary(bin.size(), bin.c_str());
    }
}

void ExecuteGetJavaScriptResultV2(
    ani_env* env, ani_status status, WebviewJavaScriptResultCallBack::AniJsCallBackParm* param)
{
    WVLOG_D("ExecuteGetJavaScriptResultV2 called");
    auto* inParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*>(param->input);
    auto* outParam = static_cast<WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*>(param->out);
    std::shared_ptr<JavaScriptOb> jsObj = inParam->webJsResCb->FindObject(inParam->objId);
    if (!jsObj) {
        WVLOG_E("ExecuteGetJavaScriptResultV2 obj is nullptr ");
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }
    if (!env) {
        WVLOG_E("ExecuteGetJavaScriptResultV2 env is nullptr");
        std::unique_lock<std::mutex> lock(param->mutex);
        param->ready = true;
        param->condition.notify_all();
        return;
    }

    Ace::ContainerScope containerScope(jsObj->GetContainerScopeId());
    ani_size nr_refs = REFERENCES_MAX_NUMBER;
    env->CreateLocalScope(nr_refs);

    ani_string methodName = AniParseUtils::StringToAniStr(env, inParam->methodName);
    if (jsObj && (jsObj->AniHasMethod(inParam->methodName))) {
        ani_ref callResult = nullptr;
        ani_object nameObj = static_cast<ani_object>(jsObj->GetAniValue());
        ani_object webviewObj = static_cast<ani_object>(jsObj->GetWebviewObject());
        auto argv = *(static_cast<std::vector<ani_object>*>(inParam->data));
        ani_array argvRef = ConvertAniArrayFromVecterObject(env, argv);
        ani_status s = env->Object_CallMethodByName_Ref(
            webviewObj, "jsProxyInvokeMethod", nullptr, &callResult, nameObj, methodName, argvRef);
        if (ANI_OK != s || !callResult) {
            WVLOG_E("ExecuteGetJavaScriptResultV2 call method return null");
        }

        bool isObject = false;
        std::vector<std::string> methodNameList;
        auto result = *(static_cast<std::shared_ptr<NWebHapValue>*>(outParam->ret));
        ani_object callObj = static_cast<ani_object>(callResult);

        ani_boolean isUndefined = true;
        if ((s = env->Reference_IsUndefined(callObj, &isUndefined)) != ANI_OK) {
            WVLOG_E("ExecuteGetJavaScriptResultV2 callResult Undefine");
        }
        if (!isUndefined) {
            methodNameList = ParseAniValue2NwebValueV2(env, &callObj, result, &isObject);
            if (isObject) {
                ProcessObjectCaseInJsTdV2(env, param, callObj, methodNameList, result);
            }
        }
    }
    env->DestroyLocalScope();
    std::unique_lock<std::mutex> lock(param->mutex);
    param->ready = true;
    param->condition.notify_all();
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

bool CreateNapiJsCallBackParm(WebviewJavaScriptResultCallBack::NapiJsCallBackInParm*& inParam,
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm*& outParam,
    WebviewJavaScriptResultCallBack::AniJsCallBackParm*& param)
{
    inParam = new (std::nothrow) WebviewJavaScriptResultCallBack::NapiJsCallBackInParm();
    if (inParam == nullptr) {
        WVLOG_E("CreateNapiJsCallBackParm argc malloc fail");
        return false;
    }
    outParam = new (std::nothrow) WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm();
    if (outParam == nullptr) {
        WVLOG_E("CreateNapiJsCallBackParm argc malloc fail");
        delete inParam;
        return false;
    }
    param = new (std::nothrow) WebviewJavaScriptResultCallBack::AniJsCallBackParm();
    if (param == nullptr) {
        WVLOG_E("CreateAniJsCallBackParm argc malloc fail");
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

void DeleteNapiJsCallBackParm(WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam,
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam,
    WebviewJavaScriptResultCallBack::AniJsCallBackParm* param)
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

bool HasAnnotationProperty(ani_env* env, ani_object& value)
{
    std::string annotation = "methodNameListForJsProxy";
    ani_status s = ANI_OK;
    ani_boolean hasProperty = false;
    if (!env) {
        return false;
    }
    s = env->Object_GetPropertyByName_Boolean(value, annotation.c_str(), &hasProperty);
    if (s != ANI_OK) {
        WVLOG_E("HasAnnotationProperty not find");
    }
    WVLOG_D("HasAnnotationProperty hasProperty = "
            "%{public}d",
        hasProperty);
    if (!hasProperty) {
        return false;
    }
    return true;
}

bool IsCallableObject(ani_env* env, ani_object& value, std::vector<std::string>* methodNameList)
{
    std::string annotation = "methodNameListForJsProxy";
    ani_status s = ANI_OK;
    if (!env) {
        return false;
    }
    if (!HasAnnotationProperty(env, value)) {
        WVLOG_E("IsCallableObject HasAnnotationProperty false ");
        return false;
    }
    ani_object result;
    ani_ref ref_result;
    s = env->Object_GetFieldByName_Ref(value, annotation.c_str(), &ref_result);
    if (s != ANI_OK) {
        WVLOG_E("IsCallableObject get property call fail");
    }
    result = static_cast<ani_object>(ref_result);
    bool isArray = AniIsArray(env, result);
    if (!isArray) {
        WVLOG_E("IsCallableObject AniIsArray false ");
        return false;
    }

    ani_array arrayResult = static_cast<ani_array>(ref_result);
    ani_array arrayRefResult = static_cast<ani_array>(arrayResult);
    uint32_t size = AniGetArrayLength(env, arrayResult);
    for (uint32_t i = 0; i < size; i++) {
        ani_ref aniTmp;
        bool isNull = AniGetElement(env, arrayRefResult, i, &aniTmp);
        if (!isNull) {
            WVLOG_E("IsCallableObject array element null fail");
            continue;
        }

        ani_object objResult = static_cast<ani_object>(aniTmp);
        bool isString = AniParseUtils::IsString(env, objResult);
        if (!isString) {
            WVLOG_E("IsCallableObject IsString fail ");
            continue;
        }
        std::string strVal;
        if (!AniParseUtils::ParseString(env, aniTmp, strVal)) {
            WVLOG_E("IsCallableObject ParseString faile");
        }
        methodNameList->push_back(strVal);
    }
    return true;
}

bool ParseBasicTypeAniValue2NwebValue(
    ani_env* env, ani_object& value, std::shared_ptr<NWebValue>& nwebValue, bool* isObject)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_ref aniTmp = static_cast<ani_ref>(value);
    bool isDouble = AniParseUtils::IsDouble(env, value);
    if (isDouble) {
        double douVal = 0.0;
        if (!AniParseUtils::ParseDouble(env, aniTmp, douVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValue AniParseUtils::ParseDouble "
                    "failed");
            return false;
        }
        nwebValue->SetType(NWebValue::Type::DOUBLE);
        nwebValue->SetDouble(douVal);
        return true;
    }
    bool isBoolean = AniParseUtils::IsBoolean(env, value);
    if (isBoolean) {
        bool boolVal = false;
        if (!AniParseUtils::ParseBoolean(env, aniTmp, boolVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValue AniParseUtils::ParseBoolean "
                    "failed");
            return false;
        }
        nwebValue->SetType(NWebValue::Type::BOOLEAN);
        nwebValue->SetBoolean(boolVal);
        return true;
    }
    bool isString = AniParseUtils::IsString(env, value);
    if (isString) {
        std::string strVal = "";
        if (!AniParseUtils::ParseString(env, aniTmp, strVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValue AniParseUtils::ParseString "
                    "failed");
            return false;
        }
        if (strVal == "methodNameListForJsProxy") {
            *isObject = true;
        }
        nwebValue->SetType(NWebValue::Type::STRING);
        nwebValue->SetString(strVal);
        return true;
    }
    WVLOG_D("ParseBasicTypeAniValue2NwebValue null or undefined type");
    nwebValue->SetType(NWebValue::Type::NONE);
    return false;
}

void ParseDictionaryAniValue2NwebValue(
    ani_env* env, ValueConvertState* state, ani_object& value, std::shared_ptr<NWebValue>& nwebValue, bool* isOject)
{
    ani_status s = ANI_OK;
    ani_double arrayLength;
    ani_array arrayRef;
    if (!env || !state) {
        return;
    }

    nwebValue->SetType(NWebValue::Type::DICTIONARY);

    bool isArray = AniIsArray(env, value);
    if (!isArray) {
        WVLOG_E("IsCallableObject AniIsArray false");
        return;
    }
    arrayRef = static_cast<ani_array>(value);

    s = env->Object_GetPropertyByName_Double(value, "length", &arrayLength);
    if (s != ANI_OK) {
        WVLOG_E("ParseDictionaryAniValue2NwebValue Object_GetPropertyByName_Double call fail");
        return;
    }
    uint32_t size = static_cast<uint32_t>(arrayLength);

    size = std::min(size, MAX_DATA_LENGTH);
    for (uint32_t i = 0; i < size; i++) {
        ani_ref arrayItem = nullptr;
        s = env->Array_Get(arrayRef, i, &arrayItem);
        if (s != ANI_OK) {
            WVLOG_E("ParseDictionaryAniValue2NwebValue array call fail");
            continue;
        }
        ani_object element = static_cast<ani_object>(arrayItem);
        auto nwebValueTmp = std::make_shared<NWebValue>();
        ParseAniValue2NwebValueHelper(env, state, element, nwebValueTmp, isOject);
        nwebValue->AddDictionaryValue(nwebValueTmp->GetString(), *nwebValueTmp);
    }
}

void ParseAniValue2NwebValueHelper(
    ani_env* env, ValueConvertState* state, ani_object& value, std::shared_ptr<NWebValue> nwebValue, bool* isOject)
{
    if (!env || !state) {
        return;
    }
    ValueConvertState::Level stateLevel(state);
    if (state->HasReachedMaxRecursionDepth()) {
        return;
    }
    if (!nwebValue || ParseBasicTypeAniValue2NwebValue(env, value, nwebValue, isOject)) {
        WVLOG_E("ParseAniValue2NwebValueHelper ParseBasicType");
        return;
    }

    bool isObject = AniParseUtils::IsObject(env, value);
    if (isObject) {
        bool isArray = AniIsArray(env, value);
        if (!isArray) {
            WVLOG_E("ParseAniValue2NwebValueHelper isArray false");
            ParseDictionaryAniValue2NwebValue(env, state, value, nwebValue, isOject);
            return;
        }
        nwebValue->SetType(NWebValue::Type::LIST);
        ani_array arrayValue = static_cast<ani_array>(value);
        ani_array arrayValueRef = static_cast<ani_array>(arrayValue);
        uint32_t size = AniGetArrayLength(env, arrayValue);
        size = std::min(size, MAX_DATA_LENGTH);
        for (uint32_t i = 0; i < size; i++) {
            ani_ref aniTmp;
            bool isNull = AniGetElement(env, arrayValueRef, i, &aniTmp);
            if (!isNull) {
                WVLOG_E("ParseAniValue2NwebValueHelper element is null fail");
                continue;
            }
            if (aniTmp == nullptr) {
                WVLOG_E("ParseAniValue2NwebValueHelper element is null");
                continue;
            }
            ani_object element = static_cast<ani_object>(aniTmp);
            auto nwebTmp = std::make_shared<NWebValue>();
            ParseAniValue2NwebValueHelper(env, state, element, nwebTmp, isOject);
            nwebValue->AddListValue(*nwebTmp);
        }
    }
}

std::vector<std::string> ParseAniValue2NwebValue(
    ani_env* env, ani_object* value, std::shared_ptr<NWebValue> nwebValue, bool* isObject)
{
    std::vector<std::string> methodNameList;

    *isObject = false;
    if (IsCallableObject(env, *value, &methodNameList)) {
        *isObject = true;
        return methodNameList;
    }
    ValueConvertState state;
    bool isObjectForRecursion = false; // FixMe: for Recursion case, now not use
    ParseAniValue2NwebValueHelper(env, &state, *value, nwebValue, &isObjectForRecursion);
    return methodNameList;
}

template<typename T>
bool ParseBasicTypeAniValue2NwebValueV2(ani_env* env, ani_object& value, std::shared_ptr<T>& nwebValue, bool* isObject)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    ani_ref aniTmp = static_cast<ani_ref>(value);
    bool isInt = AniIsInt(env, value);
    if (isInt) {
        int32_t int32Val = 0;
        if (!AniParseUtils::ParseInt32(env, aniTmp, int32Val)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValueV2 AniParseUtils::ParseInt32 "
                    "failed");
            return false;
        }
        nwebValue->SetType(T::Type::INTEGER);
        nwebValue->SetInt(int32Val);
        return true;
    }
    bool isInteger = AniParseUtils::IsInteger(env, value);
    if (isInteger) {
        int64_t intVal = 0;
        if (!AniParseUtils::ParseInt64(env, aniTmp, intVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValueV2 AniParseUtils::ParseInt64 "
                    "failed");
            return false;
        }
        nwebValue->SetType(T::Type::INTEGER);
        nwebValue->SetInt(intVal);
        return true;
    }
    bool isDouble = AniParseUtils::IsDouble(env, value);
    if (isDouble) {
        double douVal = 0.0;
        if (!AniParseUtils::ParseDouble(env, aniTmp, douVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValueV2 AniParseUtils::ParseDouble "
                    "failed");
            return false;
        }
        nwebValue->SetType(T::Type::DOUBLE);
        nwebValue->SetDouble(douVal);
        return true;
    }

    bool isBoolean = AniParseUtils::IsBoolean(env, value);
    if (isBoolean) {
        bool boolVal = false;
        if (!AniParseUtils::ParseBoolean(env, aniTmp, boolVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValueV2 AniParseUtils::ParseDouble "
                    "failed");
            return false;
        }
        nwebValue->SetType(T::Type::BOOLEAN);
        nwebValue->SetBool(boolVal);
        return true;
    }

    bool isString = AniParseUtils::IsString(env, value);
    if (isString) {
        std::string strVal = "";
        if (!AniParseUtils::ParseString(env, aniTmp, strVal)) {
            WVLOG_E("ParseBasicTypeAniValue2NwebValueV2 AniParseUtils::ParseString "
                    "failed");
            return false;
        }
        if (strVal == "methodNameListForJsProxy") {
            *isObject = true;
        }
        nwebValue->SetType(T::Type::STRING);
        nwebValue->SetString(strVal);
        return true;
    }

    nwebValue->SetType(T::Type::NONE);
    return false;
}

template<typename T>
void ParseAniValue2NwebValueHelperV2(
    ani_env* env, ValueConvertState* state, ani_object& value, std::shared_ptr<T> nwebValue, bool* isObject)
{
    if (!env || !state) {
        return;
    }
    ValueConvertState::Level stateLevel(state);
    if (state->HasReachedMaxRecursionDepth()) {
        WVLOG_E("ParseAniValue2NwebValueHelperV2 state false ");
        return;
    }
    ani_boolean isUndefined = true;
    if ((env->Reference_IsUndefined(value, &isUndefined)) != ANI_OK) {
        WVLOG_E("ParseAniValue2NwebValueHelperV2 call Undefine fail");
    }
    if (isUndefined) {
        WVLOG_E("ParseAniValue2NwebValueHelperV2 result is undefine, return");
        return;
    }
    if (!nwebValue || ParseBasicTypeAniValue2NwebValueV2(env, value, nwebValue, isObject)) {
        WVLOG_E("ParseAniValue2NwebValueHelperV2 ParseBasicType ");
        return;
    }
    bool isObj = AniParseUtils::IsObject(env, value);
    if (isObj) {
        bool isArray = AniIsArray(env, value);
        if (!isArray) {
            WVLOG_E("ParseAniValue2NwebValueHelperV2 isArray false");
            ParseDictionaryAniValue2NwebValueV2(env, state, value, nwebValue, isObject);
            return;
        }
        nwebValue->SetType(T::Type::LIST);
        ani_array arrayValue = static_cast<ani_array>(value);
        ani_array arrayValueRef = static_cast<ani_array>(arrayValue);
        uint32_t size = AniGetArrayLength(env, arrayValue);
        size = std::min(size, MAX_DATA_LENGTH);
        for (uint32_t i = 0; i < size; i++) {
            ani_ref aniTmp;
            bool isElement = AniGetElement(env, arrayValueRef, i, &aniTmp);
            if (!isElement) {
                WVLOG_E("/ParseAniValue2NwebValueHelperV2 element get fail");
                continue;
            }
            ani_object element = static_cast<ani_object>(aniTmp);
            if (!element) {
                WVLOG_E("/ParseAniValue2NwebValueHelperV2 element is null fail");
                continue;
            }
            auto child = nwebValue->NewChildValue();
            ParseAniValue2NwebValueHelperV2(env, state, element, child, isObject);
            nwebValue->SaveListChildValue();
        }
        return;
    }
    WVLOG_E("ParseAniValue2NwebValueHelper invalid type");
    return;
}

template<typename T>
void ParseDictionaryAniValue2NwebValueV2(
    ani_env* env, ValueConvertState* state, ani_object& value, std::shared_ptr<T>& nwebValue, bool* isOject)
{
    ani_status s = ANI_OK;
    if (!env || !state) {
        return;
    }
    nwebValue->SetType(T::Type::DICTIONARY);
    ani_class cls;
    s = env->FindClass("std.core.Object;", &cls);
    if (s != ANI_OK) {
        WVLOG_E("Object FindClass status: %{public}d", s);
        return;
    }
    ani_static_method getMethod = nullptr;
    if (env->Class_FindStaticMethod(cls, "entries", "C{std.core.Object}:C{std.core.Array}", &getMethod) != ANI_OK) {
        WVLOG_E("ParseDictionaryAniValue2NwebValueV2 Class_FindStaticMethod fail.. ");
        return;
    }
    ani_ref result; // an array of key/values
    s = env->Class_CallStaticMethod_Ref(cls, getMethod, &result, value);
    if (s != ANI_OK) {
        WVLOG_E("ParseDictionaryAniValue2NwebValueV2 CallStaticMethod fail ");
        return;
    }

    ani_size arrayLength;
    ani_array arrayRef {};
    arrayRef = static_cast<ani_array>(result);
    if (env->Array_GetLength(arrayRef, &arrayLength) != ANI_OK) {
        WVLOG_E("get array length failed");
        return;
    }
    uint32_t size = static_cast<uint32_t>(arrayLength);
    size = std::min(size, MAX_DATA_LENGTH);
    for (uint32_t i = 0; i < size; i++) {
        ani_ref arrayItem = nullptr;
        s = env->Array_Get(arrayRef, i, &arrayItem);
        if (s != ANI_OK) {
            WVLOG_E("ParseDictionaryAniValue2NwebValue array call fail");
            continue;
        }
        ani_tuple_value element = static_cast<ani_tuple_value>(arrayItem);
        ani_ref keyResult;
        s = env->TupleValue_GetItem_Ref(element, 0, &keyResult);
        if (s != ANI_OK) {
            WVLOG_E("ParseDictionaryAniValue2NwebValueV2 TupleValue key fail ");
            continue;
        }
        ani_ref valueResult;
        s = env->TupleValue_GetItem_Ref(element, 1, &valueResult);
        if (s != ANI_OK) {
            WVLOG_E("ParseDictionaryAniValue2NwebValueV2 TupleValue value fail ");
            continue;
        }
        auto nwebKeyTmp = std::make_shared<NWebValue>();
        ani_object keyTmp = static_cast<ani_object>(keyResult);
        ParseAniValue2NwebValueHelper(env, state, keyTmp, nwebKeyTmp, isOject);
        auto nwebValueTmp = nwebValue->NewChildValue();
        ani_object valueTmp = static_cast<ani_object>(valueResult);
        ParseAniValue2NwebValueHelperV2(env, state, valueTmp, nwebValueTmp, isOject);
        nwebValue->SaveDictChildValue(nwebKeyTmp->GetString());
    }
}

template<typename T>
std::vector<std::string> ParseAniValue2NwebValueV2(
    ani_env* env, ani_object* value, std::shared_ptr<T> nwebValue, bool* isObject)
{
    std::vector<std::string> methodNameList;
    if (!env) {
        WVLOG_E("ParseAniValue2NwebValueV2 env is null");
        return std::vector<std::string> { "then", "catch", "finally" };
    }
    if (IsCallableObject(env, *value, &methodNameList)) {
        *isObject = true;
        return methodNameList;
    }

    ValueConvertState state;
    bool isObjectForRecursion = false; // FixMe: for Recursion case, now not use
    ParseAniValue2NwebValueHelperV2(env, &state, *value, nwebValue, &isObjectForRecursion);
    return methodNameList;
}

// Map
template<typename T>
ani_object ParseDictionaryNwebValue2AniValueV2(ani_env* env, const std::shared_ptr<T>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId, int32_t id)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto dict = value->GetDictValue();

    // create Map
    ani_ref mapObj = AniCreateMapObject(env, objectsMap, id);
    ani_object clsMapObj = static_cast<ani_object>(mapObj);

    for (auto& item : dict) {
        ani_string keyObj = AniParseUtils::StringToAniStr(env, item.first);
        ani_object valueObj = ParseNwebValue2AniValueV2(env, item.second, objectsMap, nwebId, frameId, id);
        if (valueObj == nullptr || keyObj == nullptr) {
            WVLOG_E("ParseDictionaryNwebValue2AniValueV2 value Obj null");
            continue;
        }
        AniSetFieldForMapObject(env, objectsMap, clsMapObj, keyObj, valueObj, id);
    }
    return clsMapObj;
}

template<typename T>
ani_array ParseArrayNwebValue2AniValueV2(ani_env* env, const std::shared_ptr<T>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId, int32_t id)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    auto list = value->GetListValue();
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        WVLOG_E("GetUndefined failed");
        return nullptr;
    }
    ani_array arrayResult;
    if (ANI_OK != env->Array_New(list.size(), undefinedRef, &arrayResult)) {
        WVLOG_E("ParseArrayNwebValue2AniValueV2  new array ref error.");
        return nullptr;
    }
    int i = 0;
    for (auto& item : list) {
        ani_object objValue = ParseNwebValue2AniValueV2(env, item, objectsMap, nwebId, frameId, id);
        ani_ref refValue = static_cast<ani_ref>(objValue);
        if (ANI_OK != env->Array_Set(arrayResult, i++, refValue)) {
            WVLOG_E("ParseArrayNwebValue2AniValueV2 array set fail");
        }
    }
    return arrayResult;
}

template<typename T>
bool ParseBasicTypeNwebValue2AniValueV2(ani_env* env, const std::shared_ptr<T>& value, ani_object* aniValue)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    auto type = value->GetType();
    WVLOG_D("ParseBasicTypeNwebValue2AniValueV2 type is %{public}hhu", type);
    bool boolCreate = true;
    switch (type) {
        case T::Type::INTEGER:
            *aniValue = AniParseUtils::CreateInt(env, value->GetInt());
            break;
        case T::Type::DOUBLE:
            *aniValue = AniParseUtils::CreateDouble(env, value->GetDouble());
            break;
        case T::Type::BOOLEAN: {
            ani_object obj = nullptr;
            boolCreate = AniParseUtils::CreateBoolean(env, value->GetBool(), obj);
            *aniValue = obj;
            break;
        }
        case T::Type::STRING:
            *aniValue = AniParseUtils::StringToAniStr(env, value->GetString());
            break;
        default:
            return false;
    }
    return boolCreate;
}

template<typename T>
ani_ref ParseBinNwebValue2AniValueV2(ani_env* env, const std::shared_ptr<T>& value,
    WebviewJavaScriptResultCallBack::ObjectMap& objectsMap, int32_t nwebId, int32_t frameId)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_ref aniValue = nullptr;
    if (env->GetUndefined(&aniValue) != ANI_OK) {
        return nullptr;
    }
    int length = 0;
    auto buff = value->GetBinary(length);
    JavaScriptOb::ObjectID objId;
    std::string str(buff);
    std::vector<std::string> strList;
    StringSplit(str, ';', strList);
    if (strList.size() < JS_BRIDGE_BINARY_ARGS_COUNT) {
        return aniValue;
    }

    std::istringstream ss(strList[1]);
    ss >> objId;
    if (strList[0] == "TYPE_OBJECT_ID") {
        WVLOG_D("ParseBinNwebValue2AniValueV2: TYPE_OBJECT_ID");
        auto iter = objectsMap.find(objId);
        if (iter != objectsMap.end() && iter->second) {
            WVLOG_E("ParseBinNwebValue2AniValueV2: type is "
                    "binary, object is found and object_id == %{public}d",
                objId);
            aniValue = iter->second->GetAniValue();
        }
        return aniValue;
    } else if (strList[0] == "TYPE_H5_OBJECT_ID") {
        ani_object objValue = nullptr;
        ani_class jsCallbackCls = CreateProxyForH5Object(env, &objValue);
        if (!jsCallbackCls) {
            return nullptr;
        }
        std::vector<std::string> methodNames;
        methodNames.assign(strList.begin() + JS_BRIDGE_BINARY_ARGS_COUNT, strList.end()); // skip id and type
        WVLOG_D("ParseBinNwebValue2AniValueV2: TYPE_H5_OBJECT_ID");
        for (auto name : methodNames) {
            OHOS::NWeb::WebviewJavaScriptResultCallBack::H5Bundle bundle;
            bundle.nwebId = nwebId;
            bundle.frameRoutingId = frameId;
            bundle.h5Id = objId;
            bundle.funcName = name;
            AddFunctionToObjectForH5(env, bundle, jsCallbackCls, objValue);
        }
        aniValue = static_cast<ani_ref>(objValue);
        return aniValue;
    } else if (strList[0] == "TYPE_H5_FUNCTION_ID") {
        WVLOG_D("ParseBinNwebValue2AniValueV2: TYPE_H5_FUNCTION_ID");
        ani_object objValue = nullptr;
        ani_class jsCallbackCls = CreateProxyForH5Object(env, &objValue);
        aniValue = CreateFunctionForH5(env, nwebId, frameId, objId, "", jsCallbackCls, objValue);
        return aniValue;
    }
    return aniValue;
}

template<typename T>
ani_object ParseNwebValue2AniValueV2(ani_env* env, std::shared_ptr<T> value,
    WebviewJavaScriptResultCallBack::ObjectMap objectsMap, int32_t nwebId, int32_t frameId, int32_t id)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_object aniValue = nullptr;
    if (!value) {
        WVLOG_E("ParseNwebValue2AniValueV2 value null");
        ani_ref refValue = static_cast<ani_ref>(aniValue);
        env->GetUndefined(&refValue);
        return aniValue;
    }

    if (ParseBasicTypeNwebValue2AniValueV2(env, value, &aniValue)) {
        return aniValue;
    }
    switch (value->GetType()) {
        case T::Type::LIST: {
            WVLOG_D("ParseNwebValue2AniValueV2 LIST type");
            aniValue = ParseArrayNwebValue2AniValueV2(env, value, objectsMap, nwebId, frameId, id);
            return aniValue;
        }
        case T::Type::DICTIONARY: {
            WVLOG_D("ParseNwebValue2AniValueV2 DICTIONARY type");
            aniValue = ParseDictionaryNwebValue2AniValueV2(env, value, objectsMap, nwebId, frameId, id);
            return aniValue;
        }
        case T::Type::BINARY: {
            WVLOG_D("ParseNwebValue2AniValueV2 BINARY type");
            ani_ref objValue = ParseBinNwebValue2AniValueV2(env, value, objectsMap, nwebId, frameId);
            aniValue = static_cast<ani_object>(objValue);
            return aniValue;
        }
        case T::Type::NONE: {
            WVLOG_D("ParseNwebValue2AniValueV2 NONE type");
            break;
        }
        default:
            WVLOG_E("ParseNwebValue2AniValueV2 invalid type");
            break;
    }
    return aniValue;
}
} // namespace
void JavaScriptOb::AniSetUpMethods()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (isMethodsSetup_) {
        return;
    }
    ani_object obj = static_cast<ani_object>(GetAniValue());
    ani_env* env = GetAniEnv();
    if (env == nullptr) {
        WVLOG_E("env null");
        return;
    }
    ani_class cls;
    ani_status s = env->FindClass("std.core.Object;", &cls);
    if (s != ANI_OK) {
        WVLOG_E("AniSetUpMethods Object FindClass status: %{public}d", s);
        return;
    }
    ani_static_method getMethod = nullptr;
    if (env->Class_FindStaticMethod(cls, "entries", "C{std.core.Object}:C{std.core.Array}", &getMethod) != ANI_OK) {
        WVLOG_E("AniSetUpMethods Class_FindStaticMethod fail.. ");
        return;
    }
    ani_ref result; // an array of key/values
    s = env->Class_CallStaticMethod_Ref(cls, getMethod, &result, obj);
    if (s != ANI_OK) {
        WVLOG_E("AniSetUpMethods CallStaticMethod fail ");
        return;
    }
    ani_size arrayLength;
    ani_array arrayRef {};
    arrayRef = static_cast<ani_array>(result);
    if (env->Array_GetLength(arrayRef, &arrayLength) != ANI_OK) {
        WVLOG_E("get array length failed");
        return;
    }
    uint32_t size = static_cast<uint32_t>(arrayLength);
    size = std::min(size, MAX_DATA_LENGTH);
    for (uint32_t i = 0; i < size; i++) {
        ani_ref arrayItem = nullptr;
        s = env->Array_Get(arrayRef, i, &arrayItem);
        if (s != ANI_OK) {
            WVLOG_E("AniSetUpMethods array call fail");
            continue;
        }
        ani_tuple_value element = static_cast<ani_tuple_value>(arrayItem);
        ani_ref keyResult;
        s = env->TupleValue_GetItem_Ref(element, 0, &keyResult);
        if (s != ANI_OK) {
            WVLOG_E("PAniSetUpMethods TupleValue key fail ");
            continue;
        }
        bool isFunction = AniParseUtils::IsFunction(env, static_cast<ani_object>(keyResult));
        if (!isFunction) {
            continue;
        }
        std::string methodName;
        if (AniParseUtils::ParseString(env, keyResult, methodName)) {
            methods_.push_back(methodName);
        }
    }
    isMethodsSetup_ = true;
}

std::shared_ptr<JavaScriptOb> JavaScriptOb::CreateNamed(
    napi_env env, int32_t containerScopeId, napi_value value, size_t refCount)
{
    return std::make_shared<JavaScriptOb>(env, containerScopeId, value, refCount);
}

std::shared_ptr<JavaScriptOb> JavaScriptOb::CreateNamed(
    ani_env *env, int32_t containerScopeId, ani_object value, size_t refCount)
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

std::shared_ptr<JavaScriptOb> JavaScriptOb::CreateTransient(
    ani_env *env, int32_t containerScopeId, ani_object value, int32_t holder, size_t refCount)
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

JavaScriptOb::JavaScriptOb(ani_env *env, int32_t containerScopeId, ani_object value, size_t refCount)
    : vm_(nullptr), containerScopeId_(containerScopeId), isStrongRef_(refCount != 0), namesCount_(1)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (env->GetVM(&vm_) != ANI_OK) {
        WVLOG_E("get vm from env error");
        return;
    }

    if (env->GlobalReference_Create(value, &aniObjRef_) != ANI_OK) {
        WVLOG_E("create reference javascript obj fail");
        return;
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

JavaScriptOb::JavaScriptOb(
    ani_env *env, int32_t containerScopeId, ani_object value, std::set<int32_t> holders, size_t refCount)
    : vm_(nullptr), containerScopeId_(containerScopeId), isStrongRef_(refCount != 0), namesCount_(0), holders_(holders)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }

    if (env->GetVM(&vm_) != ANI_OK) {
        WVLOG_E("get vm from env error");
        return;
    }

    if (env->GlobalReference_Create(value, &aniObjRef_) != ANI_OK) {
        WVLOG_E("create reference javascript obj fail");
        return;
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
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
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

    napi_close_handle_scope(env, scope);
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
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(jsObj->GetEnv(), &scope);
    if (scope == nullptr) {
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
    }
    napi_close_handle_scope(jsObj->GetEnv(), scope);
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
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(jsObj->GetEnv(), &scope);
    if (scope == nullptr) {
        return ret;
    }
    auto flowbufferAdapter = OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
    if (!flowbufferAdapter) {
        napi_close_handle_scope(jsObj->GetEnv(), scope);
        return ret;
    }
    auto ashmem = flowbufferAdapter->CreateAshmemWithFd(fd, MAX_FLOWBUF_DATA_SIZE + HEADER_SIZE, PROT_READ);
    if (!ashmem) {
        napi_close_handle_scope(jsObj->GetEnv(), scope);
        return ret;
    }

    std::vector<napi_value> argv = {};
    if (!ConstructArgv(ashmem, args, argv, jsObj, routingId)) {
        napi_close_handle_scope(jsObj->GetEnv(), scope);
    	return ret;
    }
    close(fd);

    ret = GetJavaScriptResultSelfHelper(jsObj, method, routingId, scope, argv);
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

bool WebviewJavaScriptResultCallBack::FindObjectIdInJsTd(
    ani_env* env, ani_object object, JavaScriptOb::ObjectID* objectId)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return false;
    }
    WVLOG_I("unorder_map objects_ size = %{public}d", static_cast<int>(objects_.size()));
    *objectId = static_cast<JavaScriptOb::ObjectID>(JavaScriptOb::JavaScriptObjIdErrorCode::WEBVIEWCONTROLLERERROR);
    for (const auto& pair : objects_) {
        ani_boolean isEquals = ANI_FALSE;
        if (env->Reference_StrictEquals(object, pair.second->GetAniValue(), &isEquals) != ANI_OK && !isEquals) {
            WVLOG_E("WebviewJavaScriptResultCallBack::FindObjectIdInJsTd fail");
        }
        WVLOG_D("WebviewJavaScriptResultCallBack::FindObjectIdInJsTd isEqual : %{public}d", isEquals);
        if (isEquals) {
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

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope) {
        if (jsObj && jsObj->HasMethod(inParam->methodName)) {
            *(static_cast<bool*>(outParam->ret)) = true;
        } else {
            WVLOG_D("WebviewJavaScriptResultCallBack::HasJavaScriptObjectMethods cannot find "
                    "object");
        }
        napi_close_handle_scope(env, scope);
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
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return ret;
        }

        if (jsObj && jsObj->HasMethod(methodName)) {
            ret = true;
        } else {
            WVLOG_D("WebviewJavaScriptResultCallBack::HasJavaScriptObjectMethods cannot find "
                    "object");
        }

        napi_close_handle_scope(env, scope);
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

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);

    if (scope) {
        if (jsObj) {
            auto methods = jsObj->GetMethodNames();
            for (auto& method : methods) {
                (*(static_cast<std::shared_ptr<NWebValue>*>(outParam->ret)))->AddListValue(NWebValue(method));
            }
        }
        napi_close_handle_scope(env, scope);
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
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return ret;
        }

        if (jsObj) {
            auto methods = jsObj->GetMethodNames();
            for (auto& method : methods) {
                ret->AddListValue(NWebValue(method));
            }
        }

        napi_close_handle_scope(env, scope);
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

void WebviewJavaScriptResultCallBack::CreateUvQueueWork(ani_env* env,
    WebviewJavaScriptResultCallBack::AniJsCallBackParm* data,
    void (*handler)(ani_env* env, ani_status status, WebviewJavaScriptResultCallBack::AniJsCallBackParm* data))
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    std::shared_ptr<WebviewJavaScriptResultCallBack> weakThis = shared_from_this();
    class WorkData {
    public:
        WorkData() = delete;

        WorkData(ani_vm* vm, WebviewJavaScriptResultCallBack::AniJsCallBackParm* data,
            void (*handler)(ani_env* env, ani_status status, WebviewJavaScriptResultCallBack::AniJsCallBackParm* data))
            : vm_(vm), data_(data), handler_(handler)
        {}

        ani_vm* vm_;
        WebviewJavaScriptResultCallBack::AniJsCallBackParm* data_;
        void (*handler_)(ani_env* env, ani_status status, WebviewJavaScriptResultCallBack::AniJsCallBackParm* data);
    };

    ani_vm* vm;
    ani_status s = env->GetVM(&vm);
    if (ANI_OK != s) {
        WVLOG_E("CreateUvQueueWork getVM status=%{public}d", s);
        return;
    }
    auto workData = new WorkData(vm, data, handler);
    void* dataPointer = reinterpret_cast<void*>(workData);
    ani_status status = ANI_OK;

    auto callback = [weakThis, dataPointer, status]() {
        auto work = static_cast<WorkData*>(dataPointer);
        if (auto sharedThis = weakThis) {
            if (!work) {
                WVLOG_E("CreateUvQueueWork work data null");
                return;
            }
            if (!work->vm_ || !work->data_ || !work->handler_) {
                WVLOG_E("CreateUvQueueWork work handler null");
                delete work;
                return;
            }
            ani_env* env = nullptr;
            ani_options aniArgs { 0, nullptr };
            ani_status s = work->vm_->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &env);
            if (ANI_OK != s) {
                WVLOG_E("CreateUvQueueWork AttachCurrentThread fail");
                delete work;
                return;
            }
            work->handler_(env, static_cast<ani_status>(status), work->data_);
            if (s == ANI_OK) {
                work->vm_->DetachCurrentThread();
            }
        }
        delete work;
    };
    if (mainHandler_) {
        mainHandler_->PostTask(std::move(callback), TASK_ID, 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    }
}

void WebviewJavaScriptResultCallBack::PostGetJavaScriptResultToJsThreadV2(std::vector<ani_object>& argv,
    const std::string& method, int32_t routingId, int32_t objectId, std::shared_ptr<NWebHapValue> result)
{
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        return;
    }

    WebviewJavaScriptResultCallBack::NapiJsCallBackInParm* inParam = nullptr;
    WebviewJavaScriptResultCallBack::NapiJsCallBackOutParm* outParam = nullptr;
    WebviewJavaScriptResultCallBack::AniJsCallBackParm* param = nullptr;
    if (!CreateNapiJsCallBackParm(inParam, outParam, param)) {
        WVLOG_E("WebviewJavaScriptResultCallBack::PostGetJavaScriptResultToJsThreadV2 malloc fail");
        return;
    }

    inParam->webJsResCb = this;
    inParam->objId = objectId;
    inParam->nwebId = GetNWebId();
    inParam->methodName = method;
    inParam->frameRoutingId = routingId;
    inParam->data = reinterpret_cast<void*>(&argv);
    outParam->ret = reinterpret_cast<void*>(&result);

    param->env = jsObj->GetAniEnv();
    param->out = reinterpret_cast<void*>(outParam);
    param->input = reinterpret_cast<void*>(inParam);

    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::Create(TASK_ID);
        if (!runner) {
            DeleteNapiJsCallBackParm(inParam, outParam, param);
            return;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }

    if (mainHandler_ == nullptr) {
        WVLOG_E("WebviewJavaScriptResultCallBack::PostGetJavaScriptResultToJsThreadV2 mainHandler is null.");
        DeleteNapiJsCallBackParm(inParam, outParam, param);
        return;
    }
    CreateUvQueueWork(param->env, param, ExecuteGetJavaScriptResultV2);
    {
        std::unique_lock<std::mutex> lock(param->mutex);
        param->condition.wait(lock, [&param] { return param->ready; });
    }
    DeleteNapiJsCallBackParm(inParam, outParam, param);
}

void WebviewJavaScriptResultCallBack::GetJavaScriptResultV2(
    const std::vector<std::shared_ptr<NWebHapValue>>& args, const std::string& method,
    const std::string& objectName, int32_t routingId, int32_t objectId,
    std::shared_ptr<NWebHapValue> result)
{
    (void)objectName; // to be compatible with older webcotroller, classname may be empty
    WVLOG_D("GetJavaScriptResultV2 method = %{public}s,routingId = %{public}d, objectId = %{public}d", method.c_str(),
        routingId, objectId);

    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj || !jsObj->AniHasMethod(method)) {
        WVLOG_E("get javaScript result, jsobj null");
        return;
    }

    auto engine = reinterpret_cast<NativeEngine*>(jsObj->GetAniEnv());
    if (engine == nullptr) {
        WVLOG_E("get javaScript result, engine is null");
        return;
    }

    if (pthread_self() == engine->GetTid()) {
        GetJavaScriptResultSelfV2(args, method, routingId, objectId, result);
    } else {
        WVLOG_D("get javaScript result, not in js thread, post task to js thread");
        auto nwebId = GetNWebId();
        ani_env* env = jsObj->GetAniEnv();
        std::vector<ani_object> argv = {};
        for (auto& input : args) {
            argv.push_back(ParseNwebValue2AniValueV2(env, input, GetObjectMap(), nwebId, routingId, objectId));
        }
        PostGetJavaScriptResultToJsThreadV2(argv, method, routingId, objectId, result);
    }
}

void WebviewJavaScriptResultCallBack::GetJavaScriptResultSelfV2(const std::vector<std::shared_ptr<NWebHapValue>>& args,
    const std::string& method, int32_t routingId, int32_t objectId, std::shared_ptr<NWebHapValue> result)
{
    std::shared_ptr<JavaScriptOb> jsObj = FindObject(objectId);
    if (!jsObj) {
        WVLOG_E("GetJavaScriptResultSelfV2 jsObj null");
        return;
    }

    WVLOG_D("get javaScript result already in js thread");
    std::vector<ani_object> argv = {};
    for (auto& input : args) {
        argv.push_back(ParseNwebValue2AniValueV2(jsObj->GetAniEnv(), input, GetObjectMap(), GetNWebId(),
                                                 routingId, objectId));
    }
    ani_object callback = jsObj->AniFindMethod(method);
    if (!callback) {
        WVLOG_E("GetJavaScriptResultSelfV2 callback null");
    }

    ani_env* env = jsObj->GetAniEnv();
    if (env == nullptr) {
        WVLOG_E("env null");
        return;
    }
    ani_object callResult = nullptr;
    ani_status status = ANI_OK;
    ani_ref resultVal;
    ani_array arrayRef = ConvertAniArrayFromVecterObject(env, argv);
    ani_ref argvRef = static_cast<ani_ref>(arrayRef);
    if ((status = env->FunctionalObject_Call(static_cast<ani_fn_object>(callback),
                                             argv.size(), &argvRef, &resultVal)) != ANI_OK) {
        WVLOG_E("GetJavaScriptResultSelfV2 call Failed status : %{public}d!", status);
        return;
    }

    callResult = static_cast<ani_object>(resultVal);
    bool isObject = false;
    std::vector<std::string> methodNameList =
        ParseAniValue2NwebValueV2(jsObj->GetAniEnv(), &callResult, result, &isObject);

    bool isFunction = AniParseUtils::IsFunction(jsObj->GetAniEnv(), callResult);

    WVLOG_D("get javaScript result already in js thread end");
    if (isObject) {
        JavaScriptOb::ObjectID returnedObjectId;
        if (FindObjectIdInJsTd(jsObj->GetAniEnv(), callResult, &returnedObjectId)) {
            FindObject(returnedObjectId)->AddHolder(routingId);
        } else {
            returnedObjectId = AddObject(jsObj->GetAniEnv(), callResult, false, routingId);
        }

        SetUpAnnotateMethods(returnedObjectId, methodNameList);
        if (isFunction) {
            WVLOG_D("GetJavaScriptResultSelfV2 type is function");
        } else {
            WVLOG_D("GetJavaScriptResultSelfV2 type is object");
            std::string bin = std::string("TYPE_OBJECT_ID") + std::string(";") + std::to_string(returnedObjectId);
            result->SetType(NWebHapValue::Type::BINARY);
            result->SetBinary(bin.size(), bin.c_str());
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

    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);

    if (scope) {
        inParam->webJsResCb->RemoveJavaScriptObjectHolderInJsTd(inParam->frameRoutingId, inParam->objId);
        napi_close_handle_scope(env, scope);
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
        napi_handle_scope scope = nullptr;
        napi_open_handle_scope(env, &scope);
        if (scope == nullptr) {
            return;
        }

        RemoveJavaScriptObjectHolderInJsTd(holder, objectId);

        napi_close_handle_scope(env, scope);
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
        WVLOG_D("WebviewJavaScriptResultCallBack::AddObject objectId = %{public}d", static_cast<int32_t>(objectId));
        objects_[objectId] = new_object;
        retainedObjectSet_.insert(new_object);
    }
    return objectId;
}

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBack::AddObject(
    ani_env* env, const ani_object& object, bool methodName, int32_t holder)
{
    JavaScriptOb::ObjectID objectId;
    {
        int32_t containerScopeId = Ace::ContainerScope::CurrentId();
        auto new_object = methodName ? JavaScriptOb::CreateNamed(env, containerScopeId, object)
                                     : JavaScriptOb::CreateTransient(env, containerScopeId, object, holder);
        objectId = nextObjectId_++;
        WVLOG_D("WebviewJavaScriptResultCallBack::AddObject objectId = %{public}d", static_cast<int32_t>(objectId));
        objects_[objectId] = new_object;
        retainedObjectSet_.insert(new_object);
    }
    WVLOG_D("WebviewJavaScriptResultCallBack::AddObject objectId : %{public}d", objectId);
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

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBack::AddNamedObject(
    ani_env* env, ani_object& obj, const std::string& objName)
{
    WVLOG_I("enter WebviewJavaScriptResultCallBack::AddNamedObject");
    JavaScriptOb::ObjectID objectId;
    NamedObjectMap::iterator iter = namedObjects_.find(objName);
    bool methodName = FindObjectIdInJsTd(env, obj, &objectId);
    if (methodName && iter != namedObjects_.end() && iter->second == objectId) {
        // Nothing to do.
        WVLOG_D("WebviewJavaScriptResultCallBack::AddNamedObject obj and objName(%{public}s) already exist",
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
    WVLOG_I("WebviewJavaScriptResultCallBack::AddNamedObject : %{public}d", objectId);
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

JavaScriptOb::ObjectID WebviewJavaScriptResultCallBack::RegisterJavaScriptProxy(AniRegisterJavaScriptProxyParam& param)
{
    WVLOG_I("enter WebviewJavaScriptResultCallBack::RegisterJavaScriptProxy");
    JavaScriptOb::ObjectID objId = AddNamedObject(param.env, param.obj, param.objName);
    // set up named object method
    if (namedObjects_.find(param.objName) != namedObjects_.end() && objects_[namedObjects_[param.objName]]) {
        std::shared_ptr<OHOS::NWeb::JavaScriptOb> jsObj = objects_[namedObjects_[param.objName]];
        jsObj->SetMethods(param.syncMethodList);
        jsObj->SetAsyncMethods(param.asyncMethodList);
        jsObj->SetPermission(param.permission);
        jsObj->SetWebviewObject(param.webviewObj);
    }
    for (auto& item : param.syncMethodList) {
        WVLOG_D("WebviewJavaScriptResultCallBack::RegisterJavaScriptProxy called, "
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
        WVLOG_I("delete object find by name");
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

void WebviewJavaScriptResultCallBack::GetJavaScriptResultFlowbufV2(
    const std::vector<std::shared_ptr<NWebHapValue>>& args, const std::string& method,
    const std::string& objectName, int fd, int32_t routingId, int32_t objectId,
    std::shared_ptr<NWebHapValue> result)
{
}

void WebviewJavaScriptResultCallBack::GetJavaScriptObjectMethodsV2(
    int32_t objectId, std::shared_ptr<NWebHapValue> result)
{
}


void WebviewJavaScriptResultCallBack::CallH5FunctionInternal(
    ani_env* env, H5Bundle& bundle, std::vector<std::shared_ptr<NWebValue>> nwebArgs)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
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
} // namespace OHOS::NWeb
