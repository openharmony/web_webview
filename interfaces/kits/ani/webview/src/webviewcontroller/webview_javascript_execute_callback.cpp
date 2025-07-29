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

#include "webview_javascript_execute_callback.h"

#include <memory>

#include "ani.h"
#include "ani_business_error.h"
#include "ani_parse_utils.h"
#include "business_error.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "nweb_web_message.h"
#include "web_errors.h"

#include "base/web/webview/interfaces/kits/ani/webview/src/common/ani_parse_utils.h"

namespace OHOS::NWeb {
using namespace NWebError;
namespace {
const char* ANI_JSMESSAGEEXT_CLASS_NAME = "L@ohos/web/webview/webview/JsMessageExt;";
}
thread_local ani_ref g_jsMsgExtClassRef;

const std::string TASK_ID = "javascript";
constexpr int32_t CALLBACK_PARAM_LENGTH = 2;

WebviewJavaScriptExecuteCallback::WebviewJavaScriptExecuteCallback(
    ani_env* env, ani_ref callback, ani_resolver deferred, bool extention)
    : vm_(nullptr), callbackRef_(nullptr), deferred_(deferred), extention_(extention)
{
    WVLOG_I("WebviewJavaScriptExecuteCallback::WebviewJavaScriptExecuteCallback");
    if (!env) {
        WVLOG_I("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
    if (callback) {
        env->GlobalReference_Create(callback, &callbackRef_);
    }
}

WebviewJavaScriptExecuteCallback::~WebviewJavaScriptExecuteCallback()
{
    WVLOG_I("WebviewJavaScriptExecuteCallback::~WebviewJavaScriptExecuteCallback");
    ani_env* env = GetEnv();
    if (callbackRef_) {
        env->GlobalReference_Delete(callbackRef_);
    }
}

ani_object WrapStsError(ani_env* env, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (env == nullptr) {
        return nullptr;
    }

    ani_string aniMsg = nullptr;
    if ((status = env->String_NewUTF8(msg.c_str(), msg.size(), &aniMsg)) != ANI_OK) {
        WVLOG_E("error string is null status : %{public}d", status);
        return nullptr;
    }

    ani_ref undefRef;
    if ((status = env->GetUndefined(&undefRef)) != ANI_OK) {
        return nullptr;
    }

    if ((status = env->FindClass("Lescompat/Error;", &cls)) != ANI_OK) {
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "Lstd/core/String;Lescompat/ErrorOptions;:V", &method)) !=
        ANI_OK) {
        return nullptr;
    }

    if ((status = env->Object_New(cls, method, &obj, aniMsg, undefRef)) != ANI_OK) {
        return nullptr;
    }
    return obj;
}

ani_ref CreateStsError(ani_env* env, ani_int code, const std::string& msg)
{
    ani_class cls;
    ani_status status = ANI_OK;
    if ((status = env->FindClass("L@ohos/base/BusinessError;", &cls)) != ANI_OK) {
        return nullptr;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", "DLescompat/Error;:V", &ctor)) != ANI_OK) {
        return nullptr;
    }
    ani_object error = WrapStsError(env, msg);
    if (error == nullptr) {
        WVLOG_E("error is nullptr");
        return nullptr;
    }
    ani_object obj = nullptr;
    ani_double dCode(code);
    if ((status = env->Object_New(cls, ctor, &obj, dCode, error)) != ANI_OK) {
        WVLOG_E("object_new error status : %{public}d", status);
        return nullptr;
    }
    return reinterpret_cast<ani_ref>(obj);
}

bool CreateArgs(
    WebviewJavaScriptExecuteCallback* jsObj, std::shared_ptr<NWebMessage> result, std::vector<ani_ref>& resultRef)
{
    ani_env* env = jsObj->GetEnv();
    if (!env || !jsObj) {
        WVLOG_E("env or jsObj is nullptr");
        return false;
    }
    ani_status status;
    if (result->GetType() == NWebValue::Type::STRING && result->GetString().empty()) {
        resultRef[0] = CreateStsError(env, static_cast<ani_int>(NWebError::INVALID_RESOURCE), result->GetErrMsg());
        if ((status = env->GetNull(&resultRef[1])) != ANI_OK) {
            WVLOG_E("create null resultRef[1] error");
            return false;
        }
    } else {
        WVLOG_I("get string from result : %{public}s", result->GetString().c_str());
        if ((status = env->GetNull(&resultRef[0])) != ANI_OK) {
            WVLOG_E("create null resultRef[0] error");
            return false;
        }
        if (!(jsObj->GetExtention())) {
            WVLOG_I("TriggerJsCallback extention is false");
            resultRef[1] = OHOS::NWeb::AniParseUtils::ConvertNWebToAniValue(env, result);
        } else {
            WVLOG_I("TriggerJsCallback extention is true");
            WebJsMessageExt* webJsMessageExt = new (std::nothrow) WebJsMessageExt(result);
            if (webJsMessageExt == nullptr) {
                WVLOG_E("new WebJsMessageExt failed.");
                return false;
            }
            ani_object jsMessageObj = {};
            if (!AniParseUtils::CreateObjectVoid(env, ANI_JSMESSAGEEXT_CLASS_NAME, jsMessageObj)) {
                WVLOG_E("create jsMessageObj error");
                delete webJsMessageExt;
                webJsMessageExt = nullptr;
                return false;
            }
            if (!AniParseUtils::Wrap(
                    env, jsMessageObj, ANI_JSMESSAGEEXT_CLASS_NAME, reinterpret_cast<ani_long>(webJsMessageExt))) {
                delete webJsMessageExt;
                webJsMessageExt = nullptr;
                return false;
            }
            resultRef[1] = static_cast<ani_ref>(jsMessageObj);
        }
    }
    return true;
}

void TriggerJsCallback(WebviewJavaScriptExecuteCallback* jsObj, std::shared_ptr<NWebMessage> result)
{
    WVLOG_I("WebviewJavaScriptExecuteCallback::TriggerJsCallback");
    if (jsObj == nullptr) {
        WVLOG_E("jsObj is null.");
        return;
    }

    ani_env* env = jsObj->GetEnv();
    if (env == nullptr) {
        WVLOG_E("jsObj->GetEnv is null.");
        return;
    }
    ani_status status = ANI_OK;
    ani_boolean errorExists;
    jsObj->GetEnv()->ExistUnhandledError(&errorExists);

    // callback result format: [Error, PixelMap]
    std::vector<ani_ref> resultRef(CALLBACK_PARAM_LENGTH);
    env->GetUndefined(&resultRef[0]);
    env->GetUndefined(&resultRef[1]);
    if (CreateArgs(jsObj, result, resultRef)) {
        if (jsObj->GetCallBackRef()) {
            // callback
            ani_ref fnReturnVal;
            if ((status = jsObj->GetEnv()->FunctionalObject_Call(static_cast<ani_fn_object>(jsObj->GetCallBackRef()),
                     resultRef.size(), resultRef.data(), &fnReturnVal)) != ANI_OK) {
                WVLOG_E("TriggerJsCallback FunctionalObject_Call Failed status : %{public}d!", status);
            } else {
                WVLOG_I("TriggerJsCallback FunctionalObject_Call Success!");
            }
        }
    } else {
        WVLOG_E("create args error");
        return;
    }
    return;
}

void WebviewJavaScriptExecuteCallback::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WVLOG_I("WebviewJavaScriptExecuteCallback::OnReceiveValue");
    if (!callbackRef_) {
        WVLOG_E("callbackRef_ is null.");
        return;
    }

    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }

    if (mainHandler_ == nullptr) {
        WVLOG_E("mainHandler_ is null.");
        return;
    }

    auto task = [this, result]() { TriggerJsCallback(this, result); };

    mainHandler_->PostTask(task, TASK_ID);
}

int32_t WebJsMessageExt::ConvertToJsType(NWebValue::Type type)
{
    JsMessageType jsMessageType = JsMessageType::NOTSUPPORT;
    switch (type) {
        case NWebValue::Type::STRING:
            jsMessageType = JsMessageType::STRING;
            break;
        case NWebValue::Type::INTEGER:
        case NWebValue::Type::DOUBLE:
            jsMessageType = JsMessageType::NUMBER;
            break;
        case NWebValue::Type::BOOLEAN:
            jsMessageType = JsMessageType::BOOLEAN;
            break;
        case NWebValue::Type::BINARY:
            jsMessageType = JsMessageType::ARRAYBUFFER;
            break;
        case NWebValue::Type::STRINGARRAY:
        case NWebValue::Type::BOOLEANARRAY:
        case NWebValue::Type::DOUBLEARRAY:
        case NWebValue::Type::INT64ARRAY:
            jsMessageType = JsMessageType::ARRAY;
            break;
        default:
            jsMessageType = JsMessageType::NOTSUPPORT;
            break;
    }
    return static_cast<int32_t>(jsMessageType);
}

int32_t WebJsMessageExt::GetType()
{
    if (value_) {
        return ConvertToJsType(value_->GetType());
    }
    return static_cast<int32_t>(JsMessageType::NOTSUPPORT);
}

std::string WebJsMessageExt::GetString()
{
    if (value_) {
        return value_->GetString();
    }
    return "";
}

double WebJsMessageExt::GetNumber()
{
    if (value_) {
        return value_->GetDouble();
    }
    return 0;
}

bool WebJsMessageExt::GetBoolean()
{
    if (value_) {
        return value_->GetBoolean();
    }
    return false;
}

static ani_enum_item GetType(ani_env* env, ani_object object)
{
    WVLOG_I("JsMessageExt GetType start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_I("webJsMessageExt GetType.");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    ani_int typeMode = 0;
    ani_enum enumType;
    env->FindEnum("L@ohos/web/webview/webview/JsMessageType;", &enumType);
    typeMode = static_cast<ani_int>(webJsMessageExt->GetType());
    WVLOG_I("webJsMessageExt mode = %{public}d", static_cast<int32_t>(typeMode));
    ani_enum_item mode;
    env->Enum_GetEnumItemByIndex(enumType, typeMode, &mode);
    return mode;
}

static ani_string GetString(ani_env* env, ani_object object)
{
    WVLOG_I("GetString webJsMessageExt start");
    ani_string result = nullptr;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::STRING)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (message == nullptr) {
        WVLOG_E("message failed.");
        return result;
    }
    std::string msgStr = message->GetString();
    env->String_NewUTF8(msgStr.c_str(), msgStr.length(), &result);
    WVLOG_I("GetString msgStr = %{public}s", msgStr.c_str());
    return result;
}

static ani_double GetNumber(ani_env* env, ani_object object)
{
    WVLOG_I("GetNumber webJsMessageExt start");
    ani_double result = 0.0;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::NUMBER)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (message == nullptr) {
        WVLOG_E("message failed.");
        return result;
    }
    NWebValue::Type type = message->GetType();
    if (type == NWebValue::Type::INTEGER) {
        int64_t value = message->GetInt64();
        result = static_cast<ani_long>(value);
        WVLOG_I("GetString value = %{public}d", static_cast<int>(value));
    }
    if (type == NWebValue::Type::DOUBLE) {
        int value = message->GetDouble();
        result = static_cast<ani_double>(value);
        WVLOG_I("GetString value = %{public}d", value);
    }
    return result;
}

static ani_boolean GetBoolean(ani_env* env, ani_object object)
{
    WVLOG_I("GetBoolean webJsMessageExt start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return ANI_FALSE;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::BOOLEAN)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return ANI_FALSE;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (message == nullptr) {
        WVLOG_E("message failed.");
        return ANI_FALSE;
    }
    bool value = message->GetBoolean();
    WVLOG_I("GetString  value = %{public}d", value);
    return static_cast<ani_boolean>(value);
}

static ani_object GetArrayBuffer(ani_env* env, ani_object object)
{
    WVLOG_I("GetArrayBuffer webJsMessageExt start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return nullptr;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAYBUFFER)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (message == nullptr) {
        WVLOG_E("message failed.");
        return nullptr;
    }
    std::vector<uint8_t> msgArr = message->GetBinary();
    void* arrayData = nullptr;
    ani_arraybuffer arraybuffer;
    ani_status status = env->CreateArrayBuffer(msgArr.size(), &arrayData, &arraybuffer);
    if (status != ANI_OK) {
        WVLOG_E("Create arraybuffer failed");
        return arraybuffer;
    }
    for (size_t i = 0; i < msgArr.size(); ++i) {
        *(uint8_t*)((uint8_t*)arrayData + i) = msgArr[i];
    }
    return arraybuffer;
}

static ani_object GetArray(ani_env* env, ani_object object)
{
    WVLOG_I("GetArray webJsMessageExt start");
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (webJsMessageExt == nullptr) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return nullptr;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAY)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    ani_array_ref arr = nullptr;
    auto message = webJsMessageExt->GetJsMsgResult();
    if (message == nullptr) {
        WVLOG_E("message failed.");
        return arr;
    }
    NWebValue::Type type = message->GetType();
    if (type == NWebValue::Type::STRINGARRAY) {
        arr = static_cast<ani_array_ref>(OHOS::NWeb::ConvertToAniHandlerOfStringArr(env, message));
    }
    if (type == NWebValue::Type::BOOLEANARRAY) {
        arr = static_cast<ani_array_ref>(OHOS::NWeb::ConvertToAniHandlerOfBooleanArr(env, message));
    }
    if (type == NWebValue::Type::DOUBLEARRAY) {
        arr = static_cast<ani_array_ref>(OHOS::NWeb::ConvertToAniHandlerOfDoubleArr(env, message));
    }
    if (type == NWebValue::Type::INT64ARRAY) {
        arr = static_cast<ani_array_ref>(OHOS::NWeb::ConvertToAniHandlerOfInt64Arr(env, message));
    }
    return arr;
}

ani_status StsJsMessageExtInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_class jsMessageExtCls = nullptr;
    ani_status status = env->FindClass(ANI_JSMESSAGEEXT_CLASS_NAME, &jsMessageExtCls);
    if (status != ANI_OK || !jsMessageExtCls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", ANI_JSMESSAGEEXT_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array allMethods = {
        ani_native_function { "getType", nullptr, reinterpret_cast<void*>(GetType) },
        ani_native_function { "getString", nullptr, reinterpret_cast<void*>(GetString) },
        ani_native_function { "getNumber", nullptr, reinterpret_cast<void*>(GetNumber) },
        ani_native_function { "getBoolean", nullptr, reinterpret_cast<void*>(GetBoolean) },
        ani_native_function { "getArrayBuffer", nullptr, reinterpret_cast<void*>(GetArrayBuffer) },
        ani_native_function { "getArray", nullptr, reinterpret_cast<void*>(GetArray) },
    };

    status = env->Class_BindNativeMethods(jsMessageExtCls, allMethods.data(), allMethods.size());
    if (status != ANI_OK) {
        WVLOG_E("Class_BindNativeMethods failed status: %{public}d", status);
        return ANI_ERROR;
    }
    return ANI_OK;
}
} // namespace OHOS::NWeb