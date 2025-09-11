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

namespace OHOS::NWeb {
using namespace NWebError;
namespace {
const char* ANI_JSMESSAGEEXT_CLASS_NAME = "@ohos.web.webview.webview.JsMessageExt";
}
thread_local ani_ref g_jsMsgExtClassRef;

const std::string TASK_ID = "javascript";
constexpr int32_t CALLBACK_PARAM_LENGTH = 2;

WebviewJavaScriptExecuteCallback::WebviewJavaScriptExecuteCallback(
    ani_env* env, ani_ref callback, ani_resolver resolver, bool extention)
    : vm_(nullptr), callbackRef_(nullptr), resolver_(resolver), extention_(extention)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::WebviewJavaScriptExecuteCallback");
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
    if (callback) {
        env->GlobalReference_Create(callback, &callbackRef_);
    }
}

WebviewJavaScriptExecuteCallback::~WebviewJavaScriptExecuteCallback()
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::~WebviewJavaScriptExecuteCallback");
    ani_env* env = GetEnv();
    if (env && callbackRef_) {
        env->GlobalReference_Delete(callbackRef_);
    } else {
        WVLOG_E("global ref not delete");
    }
}

ani_object WrapStsError(ani_env* env, const std::string& msg)
{
    ani_class cls {};
    ani_method method {};
    ani_object obj = nullptr;
    ani_status status = ANI_ERROR;
    if (!env) {
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

    if ((status = env->FindClass("escompat.Error", &cls)) != ANI_OK) {
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "C{std.core.String}C{escompat.ErrorOptions}:", &method)) !=
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
    if ((status = env->FindClass("@ohos.base.BusinessError", &cls)) != ANI_OK) {
        return nullptr;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", "iC{escompat.Error}:", &ctor)) != ANI_OK) {
        return nullptr;
    }
    ani_object error = WrapStsError(env, msg);
    if (!error) {
        WVLOG_E("error is nullptr");
        return nullptr;
    }
    ani_object obj = nullptr;
    if ((status = env->Object_New(cls, ctor, &obj, code, error)) != ANI_OK) {
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
            WVLOG_E("TriggerJsCallback extention is false");
            resultRef[1] = OHOS::NWeb::AniParseUtils::ConvertNWebToAniValue(env, result);
        } else {
            WebJsMessageExt* webJsMessageExt = new (std::nothrow) WebJsMessageExt(result);
            if (!webJsMessageExt) {
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

static void UvAfterWorkCbPromise(
    WebviewJavaScriptExecuteCallback* jsObj, std::shared_ptr<NWebMessage> result, std::vector<ani_ref>& resultRef)
{
    WVLOG_D("enter UvAfterWorkCbPromise");
    ani_env* env;
    if (!jsObj || !(env = jsObj->GetEnv())) {
        WVLOG_E("env or jsObj is nullptr");
        return;
    }
    ani_status status;
    resultRef[0] = CreateStsError(env, static_cast<ani_int>(NWebError::INVALID_RESOURCE), result->GetErrMsg());
    if (!(jsObj->GetExtention())) {
        WVLOG_E("TriggerJsCallback extention is false");
        resultRef[1] = OHOS::NWeb::AniParseUtils::ConvertNWebToAniValue(env, result);
    } else {
        WebJsMessageExt* webJsMessageExt = new (std::nothrow) WebJsMessageExt(result);
        if (!webJsMessageExt) {
            WVLOG_E("new WebJsMessageExt failed.");
            return;
        }
        ani_object jsMessageObj = {};
        if (!AniParseUtils::CreateObjectVoid(env, ANI_JSMESSAGEEXT_CLASS_NAME, jsMessageObj)) {
            WVLOG_E("create jsMessageObj error");
            delete webJsMessageExt;
            webJsMessageExt = nullptr;
            return;
        }
        if (!AniParseUtils::Wrap(
                env, jsMessageObj, ANI_JSMESSAGEEXT_CLASS_NAME, reinterpret_cast<ani_long>(webJsMessageExt))) {
            delete webJsMessageExt;
            webJsMessageExt = nullptr;
            return;
        }
        resultRef[1] = static_cast<ani_ref>(jsMessageObj);
    }

    if (result->GetType() == NWebValue::Type::STRING && result->GetString().empty()) {
        if ((status = env->PromiseResolver_Reject(jsObj->GetResolver(), reinterpret_cast<ani_error>(resultRef[0]))) !=
            ANI_OK) {
            WVLOG_E("UvAfterWorkCbPromise error in reject promise");
        }
    } else {
        if ((status = env->PromiseResolver_Resolve(jsObj->GetResolver(), resultRef[1])) != ANI_OK) {
            WVLOG_E("UvAfterWorkCbPromise error in resolve promise");
        }
    }
}

void TriggerJsCallback(WebviewJavaScriptExecuteCallback* jsObj, std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::TriggerJsCallback");
    if (!jsObj) {
        WVLOG_E("jsObj is null.");
        return;
    }

    ani_env* env = jsObj->GetEnv();
    if (!env) {
        WVLOG_E("jsObj->GetEnv is null.");
        return;
    }
    ani_status status = ANI_OK;
    std::vector<ani_ref> resultRef(CALLBACK_PARAM_LENGTH);
    if (jsObj->GetCallBackRef()) {
        if (CreateArgs(jsObj, result, resultRef)) {
            ani_ref fnReturnVal;
            if ((status = jsObj->GetEnv()->FunctionalObject_Call(static_cast<ani_fn_object>(jsObj->GetCallBackRef()),
                     resultRef.size(), resultRef.data(), &fnReturnVal)) != ANI_OK) {
                WVLOG_E("TriggerJsCallback FunctionalObject_Call Failed status : %{public}d!", status);
            }
        } else {
            WVLOG_E("create args error");
            return;
        }
        return;
    }

    if (jsObj->GetResolver()) {
        UvAfterWorkCbPromise(jsObj, result, resultRef);
        return;
    }
}

void WebviewJavaScriptExecuteCallback::OnReceiveValue(std::shared_ptr<NWebMessage> result)
{
    WVLOG_D("WebviewJavaScriptExecuteCallback::OnReceiveValue");
    std::weak_ptr<WebviewJavaScriptExecuteCallback> weakThis = weak_from_this();
    auto ensureMainHandler = [weakThis, result]() {
        if (auto sharedThis = weakThis.lock()) {
            TriggerJsCallback(sharedThis.get(), result);
        } else {
            WVLOG_E("object destory before task execution");
            return;
        }
    };

    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }

    if (!mainHandler_) {
        WVLOG_E("mainHandler_ is null.");
        return;
    }

    mainHandler_->PostTask(std::move(ensureMainHandler), TASK_ID, 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
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
    WVLOG_D("JsMessageExt GetType start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_E("Get webJsMessageExt failed.");
        AniBusinessError::ThrowErrorByErrCode(env, INIT_ERROR);
        return nullptr;
    }
    ani_int typeMode = 0;
    ani_enum enumType;
    if ((env->FindEnum("@ohos.web.webview.webview.JsMessageType", &enumType)) != ANI_OK) {
        WVLOG_E("FindEnum failed");
        return nullptr;
    }
    typeMode = static_cast<ani_int>(webJsMessageExt->GetType());
    WVLOG_I("webJsMessageExt mode = %{public}d", static_cast<int32_t>(typeMode));
    ani_enum_item mode;
    env->Enum_GetEnumItemByIndex(enumType, typeMode, &mode);
    return mode;
}

static ani_string GetString(ani_env* env, ani_object object)
{
    WVLOG_D("GetString webJsMessageExt start");
    ani_string result = nullptr;
    if (!env) {
        WVLOG_E("env is nullptr");
        return result;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return result;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::STRING)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return result;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (!message) {
        WVLOG_E("message failed.");
        return result;
    }
    std::string msgStr = message->GetString();
    env->String_NewUTF8(msgStr.c_str(), msgStr.length(), &result);
    WVLOG_I("GetString msgStr = %{public}s", msgStr.c_str());
    return result;
}

static ani_object GetNumber(ani_env* env, ani_object object)
{
    WVLOG_D("GetNumber webJsMessageExt start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return nullptr;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::NUMBER)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (!message) {
        WVLOG_E("message failed.");
        return nullptr;
    }

    return static_cast<ani_object>(AniParseUtils::ConvertNWebToAniValue(env, message));
}

static ani_boolean GetBoolean(ani_env* env, ani_object object)
{
    WVLOG_D("GetBoolean webJsMessageExt start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return ANI_FALSE;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return ANI_FALSE;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::BOOLEAN)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return ANI_FALSE;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (!message) {
        WVLOG_E("message failed.");
        return ANI_FALSE;
    }
    bool value = message->GetBoolean();
    WVLOG_I("GetString  value = %{public}d", value);
    return static_cast<ani_boolean>(value);
}

static ani_object GetArrayBuffer(ani_env* env, ani_object object)
{
    WVLOG_D("GetArrayBuffer webJsMessageExt start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return nullptr;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAYBUFFER)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    auto message = webJsMessageExt->GetJsMsgResult();
    if (!message) {
        WVLOG_E("message failed.");
        return nullptr;
    }
    std::vector<uint8_t> msgArr = message->GetBinary();
    void* arrayData = nullptr;
    ani_arraybuffer arraybuffer;
    ani_status status = env->CreateArrayBuffer(msgArr.size(), &arrayData, &arraybuffer);
    if (status != ANI_OK) {
        WVLOG_E("Create arraybuffer failed");
        return nullptr;
    }
    for (size_t i = 0; i < msgArr.size(); ++i) {
        *(uint8_t*)((uint8_t*)arrayData + i) = msgArr[i];
    }
    return arraybuffer;
}

static ani_object GetArray(ani_env* env, ani_object object)
{
    WVLOG_D("GetArray webJsMessageExt start");
    if (!env) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    WebJsMessageExt* webJsMessageExt = reinterpret_cast<WebJsMessageExt*>(AniParseUtils::Unwrap(env, object));
    if (!webJsMessageExt) {
        WVLOG_E("unwrap webJsMessageExt failed.");
        return nullptr;
    }

    if (webJsMessageExt->GetType() != static_cast<int32_t>(JsMessageType::ARRAY)) {
        AniBusinessError::ThrowErrorByErrCode(env, TYPE_NOT_MATCH_WITCH_VALUE);
        return nullptr;
    }

    ani_array arr = nullptr;
    auto message = webJsMessageExt->GetJsMsgResult();
    if (!message) {
        WVLOG_E("message failed.");
        return arr;
    }
    NWebValue::Type type = message->GetType();
    if (type == NWebValue::Type::STRINGARRAY) {
        arr = static_cast<ani_array>(OHOS::NWeb::ConvertToAniHandlerOfStringArr(env, message));
    } else if (type == NWebValue::Type::BOOLEANARRAY) {
        arr = static_cast<ani_array>(OHOS::NWeb::ConvertToAniHandlerOfBooleanArr(env, message));
    } else if (type == NWebValue::Type::DOUBLEARRAY) {
        arr = static_cast<ani_array>(OHOS::NWeb::ConvertToAniHandlerOfDoubleArr(env, message));
    } else if (type == NWebValue::Type::INT64ARRAY) {
        arr = static_cast<ani_array>(OHOS::NWeb::ConvertToAniHandlerOfInt64Arr(env, message));
    }
    return arr;
}

ani_status StsJsMessageExtInit(ani_env* env)
{
    if (!env) {
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