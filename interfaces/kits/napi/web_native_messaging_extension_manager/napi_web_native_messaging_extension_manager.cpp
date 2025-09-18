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

#include "napi_web_native_messaging_extension_manager.h"

#include <cstdint>
#include <map>
#include <mutex>
#include <uv.h>

#include "ability.h"
#include "business_error.h"
#include "context.h"
#include "napi_base_context.h"
#include "napi_common_want.h"
#include "nweb_napi_scope.h"
#include "napi_parse_utils.h"
#include "securec.h"
#include "system_properties_adapter_impl.h"
#include "web_extension_connection_callback.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {

namespace {
std::recursive_mutex gConnectsLock_;
int32_t g_serialNumber = 1;
static std::map<int32_t, sptr<WebExtensionConnectionCallback>> g_connects;
}

static void RemoveConnection(int32_t connectId)
{
    std::lock_guard<std::recursive_mutex> lock(gConnectsLock_);
    auto item = g_connects.find(connectId);
    if (item != g_connects.end()) {
        WNMLOG_D("remove connection %{public}d ok", connectId);
        g_connects.erase(item);
    } else {
        WNMLOG_I("remove connection %{public}d not exist", connectId);
    }
}

static int32_t InsertConnection(sptr<WebExtensionConnectionCallback> connection)
{
    std::lock_guard<std::recursive_mutex> lock(gConnectsLock_);
    if (connection == nullptr) {
        WNMLOG_E("insert null connection");
        return -1;
    }
    int32_t connectId = g_serialNumber;
    g_connects.emplace(connectId, connection);
    if (g_serialNumber < INT32_MAX) {
        g_serialNumber++;
    } else {
        g_serialNumber = 1;
    }
    connection->connectionId_ = connectId;
    WNMLOG_D("insert connection %{public}d", connectId);
    return connectId;
}

CommonAsyncContext::CommonAsyncContext(napi_env napiEnv)
{
    env = napiEnv;
}

CommonAsyncContext::~CommonAsyncContext()
{
    if (callbackRef) {
        WNMLOG_D("~CommonAsyncContext delete callbackRef");
        napi_delete_reference(env, callbackRef);
        callbackRef = nullptr;
    }
    if (work) {
        WNMLOG_D("~CommonAsyncContext delete work");
        napi_delete_async_work(env, work);
        work = nullptr;
    }
}

static napi_value FillJsConnectionNativeInfo(napi_env env, const ConnectionNativeInfo& info)
{
    napi_value jsInfoObj = nullptr;
    napi_create_object(env, &jsInfoObj);

    napi_value jsConnectId = {0};
    napi_create_int32(env, info.connectionId, &jsConnectId);

    napi_value jsBundleName = {0};
    napi_create_string_utf8(env, info.bundleName.c_str(), info.bundleName.length(), &jsBundleName);

    napi_value jsExtensionOrigin = {0};
    napi_create_string_utf8(env, info.extensionOrigin.c_str(), info.extensionOrigin.length(), &jsExtensionOrigin);

    napi_value jsExtensionPid = {0};
    napi_create_int32(env, info.extensionPid, &jsExtensionPid);

    napi_set_named_property(env, jsInfoObj, "connectionId", jsConnectId);
    napi_set_named_property(env, jsInfoObj, "bundleName", jsBundleName);
    napi_set_named_property(env, jsInfoObj, "extensionOrigin", jsExtensionOrigin);
    napi_set_named_property(env, jsInfoObj, "extensionPid", jsExtensionPid);
    return jsInfoObj;
}

static void TransformOnFailedErrorCode(int32_t innerErrorCode, int32_t& jsCode, std::string& errMsg)
{
    switch (innerErrorCode) {
        case ConnectNativeRet::PERMISSION_CHECK_ERROR:
            jsCode = static_cast<int32_t>(NmErrorCode::PERMISSION_DENY);
            errMsg = NmErrorMsg::PERMISSION_DENY_ERR_MSG;
            break;
        case ConnectNativeRet::WANT_FORMAT_ERROR:
            jsCode = static_cast<int32_t>(NmErrorCode::WANT_CONTENT_ERROR);
            errMsg = NmErrorMsg::WANT_CONTENT_ERR_MSG;
            break;
        default:
            jsCode = static_cast<int32_t>(NmErrorCode::INNER_ERROR);
            errMsg = NmErrorMsg::INNER_ERR_MSG + std::to_string(innerErrorCode);
            break;
    }
}

static bool InvokeConnectNativeCallback(NapiExtensionConnectionCallbackParam* param)
{
    NApiScope scope(param->env_);
    if (!scope.IsVaild()) {
        WNMLOG_E("scope is null");
        return false;
    }
    if (!param) {
        WNMLOG_E("param is invalid");
        return false;
    }
    int32_t paramNum = INTEGER_ONE;
    napi_value result[INTEGER_TWO] = {0};
    napi_value onCallbackFunc = nullptr;
    switch (param->type_) {
        case ConnectCallbackType::ON_CONNECT_TYPE: {
            result[INTEGER_ZERO] = FillJsConnectionNativeInfo(param->env_, param->result_);
            napi_get_reference_value(param->env_, param->callback_.onConnectMethodRef, &onCallbackFunc);
            break;
        }
        case ConnectCallbackType::ON_DISCONNECT_TYPE: {
            RemoveConnection(param->result_.connectionId);
            result[INTEGER_ZERO] = FillJsConnectionNativeInfo(param->env_, param->result_);
            napi_get_reference_value(param->env_, param->callback_.onDisconnectMethodRef, &onCallbackFunc);
            break;
        }
        case ConnectCallbackType::ON_FAILED_TYPE: {
            RemoveConnection(param->result_.connectionId);
            paramNum = INTEGER_TWO;
            int32_t jsErrorCode;
            std::string JsErrorMsg;
            TransformOnFailedErrorCode(param->errorNum_, jsErrorCode, JsErrorMsg);
            napi_create_int32(param->env_, jsErrorCode, &result[INTEGER_ZERO]);
            napi_create_string_utf8(param->env_, JsErrorMsg.c_str(), NAPI_AUTO_LENGTH, &result[INTEGER_ONE]);
            napi_get_reference_value(param->env_, param->callback_.onFailedMethodRef, &onCallbackFunc);
            break;
        }
        default: {
            WNMLOG_E("type %{public}d is invalid", param->type_);
            return false;
        }
    }
    napi_value placeHodler = nullptr;
    napi_call_function(param->env_, nullptr, onCallbackFunc, paramNum, &result[INTEGER_ZERO], &placeHodler);
    return true;
}

static void UvInvokeConnectNativeCallback(uv_work_t* work, int status)
{
    if (work == nullptr) {
        WNMLOG_E("uv work is null");
        return;
    }
    auto* data = reinterpret_cast<NapiExtensionConnectionCallbackParam*>(work->data);
    if (data == nullptr) {
        WNMLOG_E("extension connection callback param is null");
        delete work;
        return;
    }
    InvokeConnectNativeCallback(data);
    delete data;
    delete work;
}

void NapiExtensionConnectionCallback::DoJsExtensionConnectCallback(
    ConnectionNativeInfo& info, int32_t errorNum, ConnectCallbackType type)
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    auto engine = reinterpret_cast<NativeEngine*>(env_);
    if (loop == nullptr) {
        WNMLOG_E("get uv event loop failed");
        return;
    }

    auto param = std::make_unique<NapiExtensionConnectionCallbackParam>();
    if (param == nullptr) {
        WNMLOG_E("new NapiExtensionConnectionCallbackParam failed");
        return;
    }

    param->type_ = type;
    param->env_ = env_;
    param->callback_ = jsCallback_;
    param->result_ = info;
    param->errorNum_ = errorNum;
    param->caller = shared_from_this();
    if (pthread_self() == engine->GetTid()) {
        InvokeConnectNativeCallback(param.get());
    } else {
        std::unique_ptr<uv_work_t> work = std::make_unique<uv_work_t>();
        if (work == nullptr) {
            WNMLOG_E("new uv work failed");
            return;
        }

        work->data = reinterpret_cast<void*>(param.get());
        int ret = uv_queue_work_with_qos(
            loop, work.get(), [](uv_work_t* work) {}, UvInvokeConnectNativeCallback, uv_qos_user_initiated);
        if (ret == 0) {
            work.release();
            param.release();
        }
    }
}

static void DoDeleteJsCall(DeleteJsCallParam* param)
{
    if (!param) {
        return;
    }
    auto env = param->env_;
    auto jsCallback = param->jsCallback_;
    if (jsCallback.onConnectMethodRef) {
        napi_delete_reference(env, jsCallback.onConnectMethodRef);
    }
    if (jsCallback.onDisconnectMethodRef) {
        napi_delete_reference(env, jsCallback.onDisconnectMethodRef);
    }
    if (jsCallback.onFailedMethodRef) {
        napi_delete_reference(env, jsCallback.onFailedMethodRef);
    }
}

static void UvInvokeDeleteJsCall(uv_work_t* work, int status)
{
    if (work == nullptr) {
        WNMLOG_E("uv work is null");
        return;
    }
    auto* data = reinterpret_cast<DeleteJsCallParam*>(work->data);
    if (data == nullptr) {
        WNMLOG_E("delete connection callback param is null");
        delete work;
        return;
    }
    DoDeleteJsCall(data);
    delete data;
    delete work;
}

void NapiExtensionConnectionCallback::DeleteJsCallInJsThread()
{
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(env_, &loop);
    auto engine = reinterpret_cast<NativeEngine*>(env_);
    if (loop == nullptr) {
        WNMLOG_E("get uv event loop failed");
        return;
    }

    auto param = std::make_unique<DeleteJsCallParam>();
    if (param == nullptr) {
        WNMLOG_E("new NapiExtensionConnectionCallbackParam failed");
        return;
    }

    param->env_ = env_;
    param->jsCallback_ = jsCallback_;
    if (pthread_self() == engine->GetTid()) {
        DoDeleteJsCall(param.get());
    } else {
        std::unique_ptr<uv_work_t> work = std::make_unique<uv_work_t>();
        if (work == nullptr) {
            WNMLOG_E("new uv work failed");
            return;
        }

        work->data = reinterpret_cast<void*>(param.get());
        int ret = uv_queue_work_with_qos(
            loop, work.get(), [](uv_work_t* work) {}, UvInvokeDeleteJsCall, uv_qos_user_initiated);
        if (ret == 0) {
            work.release();
            param.release();
        }
    }
}

void NapiExtensionConnectionCallback::OnExtensionConnect(ConnectionNativeInfo& info)
{
    DoJsExtensionConnectCallback(info, 0, ConnectCallbackType::ON_CONNECT_TYPE);
}

void NapiExtensionConnectionCallback::OnExtensionDisconnect(ConnectionNativeInfo& info)
{
    DoJsExtensionConnectCallback(info, 0, ConnectCallbackType::ON_DISCONNECT_TYPE);
}

void NapiExtensionConnectionCallback::OnExtensionFailed(int32_t connectionId, int32_t errorNum)
{
    ConnectionNativeInfo info;
    info.connectionId = connectionId;
    DoJsExtensionConnectCallback(info, errorNum, ConnectCallbackType::ON_FAILED_TYPE);
}

static bool ParseConnectionCallbackFunction(napi_env env, napi_value preArgs,
    const std::string& funcName, napi_ref& methodRef)
{
    napi_value methodVal = nullptr;
    napi_get_named_property(env, preArgs, funcName.c_str(), &methodVal);

    if (!methodVal) {
        return false;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, methodVal, &valueType);
    if (valueType != napi_function) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            NWebError::FormatString(ParamCheckErrorMsgTemplate::TYPE_ERROR, "callback", "function"));
        return false;
    }

    napi_create_reference(env, methodVal, INTEGER_ONE, &methodRef);
    return true;
}

static sptr<WebExtensionConnectionCallback> ParseNapiExtensionConnectionCallback(napi_env env, napi_value preArgs)
{
    JsConnectCallback jsCallback;
    if (!ParseConnectionCallbackFunction(env, preArgs, "onConnect", jsCallback.onConnectMethodRef)) {
        return nullptr;
    }
    if (!ParseConnectionCallbackFunction(env, preArgs, "onDisconnect", jsCallback.onDisconnectMethodRef)) {
        napi_delete_reference(env, jsCallback.onConnectMethodRef);
        return nullptr;
    }
    if (!ParseConnectionCallbackFunction(env, preArgs, "onFailed", jsCallback.onFailedMethodRef)) {
        napi_delete_reference(env, jsCallback.onConnectMethodRef);
        napi_delete_reference(env, jsCallback.onDisconnectMethodRef);
        return nullptr;
    }

    auto callback = std::make_shared<NapiExtensionConnectionCallback>(env, jsCallback);
    if (!callback) {
        napi_delete_reference(env, jsCallback.onConnectMethodRef);
        napi_delete_reference(env, jsCallback.onDisconnectMethodRef);
        napi_delete_reference(env, jsCallback.onFailedMethodRef);
        return nullptr;
    }
    auto callbackRemote = new (std::nothrow) WebExtensionConnectionCallback(callback);
    if (!callbackRemote) {
        WNMLOG_E("new WebExtensionConnectionCallback failed");
        return nullptr;
    }
    return callbackRemote;
}

void NapiWebNativeMessagingExtensionManager::ConnectNativeExcute(napi_env env, void* data)
{
    WNMLOG_D("connectNative excute");
    auto asyncContext = reinterpret_cast<ConnectNativeAsyncContext*>(data);
    if (asyncContext == nullptr) {
        WNMLOG_E("asyncContext is null");
        return;
    }
    asyncContext->errCode = WebNativeMessagingClient::GetInstance().ConnectWebNativeMessagingExtension(
        asyncContext->token, asyncContext->want, asyncContext->connectCallback,
        asyncContext->connectId);
}

void NapiWebNativeMessagingExtensionManager::ConnectNativeComplete(napi_env env, napi_status status, void* data)
{
    WNMLOG_D("connectNative complete");
    auto asyncContext = reinterpret_cast<ConnectNativeAsyncContext*>(data);
    if (asyncContext == nullptr || !asyncContext->connectCallback) {
        WNMLOG_E("asyncContext is invalid");
        return;
    }

    std::unique_ptr<ConnectNativeAsyncContext> asyncContextPtr { asyncContext };
    if (asyncContext->errCode != 0) {
        asyncContext->connectCallback->OnFailed(asyncContext->errCode);
    }
}

static bool ParseUiAbilityContextToken(napi_env env, napi_value param, sptr<IRemoteObject>& token)
{
    bool stageMode = false;
    napi_status status = AbilityRuntime::IsStageContext(env, param, stageMode);
    if (status != napi_ok || !stageMode) {
        WNMLOG_E("not stage mode");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "Parse param context failed, must be a context of stageMode.");
        return false;
    }
    auto context = AbilityRuntime::GetStageModeContext(env, param);
    if (context == nullptr) {
        WNMLOG_E("parse param context is null");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "Parse param context failed, must not be nullptr.");
        return false;
    }
    auto uiAbilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
    if (uiAbilityContext == nullptr) {
        WNMLOG_E("parse param context is not abilityContext");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "Parse param context failed, must be UIAbilityContext.");
        return false;
    }

    token = uiAbilityContext->GetToken();
    return true;
}

napi_value NapiWebNativeMessagingExtensionManager::ConnectNative(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_create_int32(env, -1, &result);
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() != ProductDeviceType::DEVICE_TYPE_2IN1) {
        WNMLOG_E("ConnectNative: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }
    size_t argc = INTEGER_THREE;
    napi_value argv[INTEGER_THREE] = {0};
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != INTEGER_THREE) {
        WNMLOG_E("BusinessError: 401. Arg count must be 3.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR, "Arg count must be 3");
        return result;
    }
    auto* asyncContext = new (std::nothrow) ConnectNativeAsyncContext(env);
    if (asyncContext == nullptr) {
        WNMLOG_E("insufficient memory for asyncContext.");
        return result;
    }
    std::unique_ptr<ConnectNativeAsyncContext> asyncContextPtr { asyncContext };
    if (!ParseUiAbilityContextToken(env, argv[INTEGER_ZERO], asyncContext->token)) {
        return result;
    }
    if (!AppExecFwk::UnwrapWant(env, argv[INTEGER_ONE], asyncContext->want)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "Parse param want failed, must be a want");
        return result;
    }

    asyncContext->connectCallback = ParseNapiExtensionConnectionCallback(env, argv[INTEGER_TWO]);
    if (asyncContext->connectCallback == nullptr) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "Parse param callback failed.");
        return result;
    }

    asyncContext->connectId = InsertConnection(asyncContext->connectCallback);
    napi_create_int32(env, asyncContext->connectId, &result);

    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "ConnectNative", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, ConnectNativeExcute, ConnectNativeComplete,
        static_cast<void*>(asyncContext), &(asyncContext->work)));
    NAPI_CALL(env, napi_queue_async_work_with_qos(env, asyncContext->work, napi_qos_user_initiated));
    asyncContextPtr.release();
    return result;
}

void NapiWebNativeMessagingExtensionManager::DisconnectNativeExcute(napi_env env, void* data)
{
    WNMLOG_D("disconnect native excute");
    auto asyncContext = reinterpret_cast<DisconnectNativeAsyncContext*>(data);
    if (asyncContext == nullptr) {
        WNMLOG_E("asyncContext is nullptr");
        return;
    }
    asyncContext->errCode = WebNativeMessagingClient::GetInstance().DisconnectWebNativeMessagingExtension(
        asyncContext->connectId);
}

void NapiWebNativeMessagingExtensionManager::DisconnectNativeComplete(napi_env env, napi_status status, void* data)
{
    WNMLOG_D("disconnect native complete");
    auto asyncContext = reinterpret_cast<DisconnectNativeAsyncContext*>(data);
    if (asyncContext == nullptr) {
        WNMLOG_E("asyncContext is nullptr");
        return;
    }

    std::unique_ptr<DisconnectNativeAsyncContext> asyncContextPtr { asyncContext };
    napi_value result;
    NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result));
    if (asyncContext->errCode != 0) {
        std::string ErrorMesg = "";
        int32_t jsError = NativeMessageError::NativeCodeToJsCode(asyncContext->errCode, ErrorMesg);
        napi_value businessError = NWebError::BusinessError::CreateError(env, jsError, ErrorMesg);
        NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncContextPtr->deferred, businessError));
    } else {
        NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncContextPtr->deferred, result));
    }
}

napi_value NapiWebNativeMessagingExtensionManager::DisconnectNative(napi_env env, napi_callback_info info)
{
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() != ProductDeviceType::DEVICE_TYPE_2IN1) {
        WNMLOG_E("DisconnectNative: Capability not supported.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::CAPABILITY_NOT_SUPPORTED_ERROR);
        return nullptr;
    }

    size_t argc = INTEGER_ONE;
    napi_value argv[INTEGER_ONE] = {0};
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (argc != INTEGER_ONE) {
        WNMLOG_E("BusinessError: 401. Arg count must be 1.");
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR, "Arg count must be 1");
        return nullptr;
    }

    auto* asyncContext = new (std::nothrow) DisconnectNativeAsyncContext(env);
    if (asyncContext == nullptr) {
        WNMLOG_E("insufficient memory for asyncContext.");
        return nullptr;
    }
    std::unique_ptr<DisconnectNativeAsyncContext> asyncContextPtr { asyncContext };

    if (!NapiParseUtils::ParseInt32(env, argv[INTEGER_ZERO], asyncContext->connectId) ||
        (asyncContext->connectId < 0)) {
        NWebError::BusinessError::ThrowErrorByErrcode(env, NWebError::PARAM_CHECK_ERROR,
            "The type of 'connectionId' must be int and must be >= 0");
        return nullptr;
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &asyncContext->deferred, &result));
    napi_value resource = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, "DisconnectNative", NAPI_AUTO_LENGTH, &resource));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resource, DisconnectNativeExcute, DisconnectNativeComplete,
        static_cast<void*>(asyncContext), &(asyncContext->work)));
    NAPI_CALL(env, napi_queue_async_work_with_qos(env, asyncContext->work, napi_qos_user_initiated));
    asyncContextPtr.release();
    return result;
}

void NapiWebNativeMessagingExtensionManager::Init(napi_env env, napi_value exports)
{
    WebNativeMessagingClient::GetInstance().SetUserDefineDiedRecipient([](){
        std::lock_guard<std::recursive_mutex> lock(gConnectsLock_);
        for (auto iter = g_connects.begin(); iter != g_connects.end();) {
            WNMLOG_I("remove connection %{public}d because of service death", iter->first);
            if (iter->second) {
                iter->second->OnFailed(ConnectNativeRet::SERVICE_DIED_ERROR);
            }
            iter = g_connects.erase(iter);
        }
    });

    napi_value nmErrorCode = nullptr;
    napi_property_descriptor nmErorrCodeProperties[] = {
        DECLARE_NAPI_STATIC_PROPERTY("INNER_ERROR", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(NmErrorCode::INNER_ERROR))),
        DECLARE_NAPI_STATIC_PROPERTY("WANT_CONTENT_ERROR", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(NmErrorCode::WANT_CONTENT_ERROR))),
        DECLARE_NAPI_STATIC_PROPERTY("PERMISSION_DENY", NapiParseUtils::ToInt32Value(env,
            static_cast<int32_t>(NmErrorCode::PERMISSION_DENY))),
    };
    napi_define_class(env, NATIVE_MESSAGING_ERROR_CODE_ENUM_NAME.c_str(),
        NATIVE_MESSAGING_ERROR_CODE_ENUM_NAME.length(),
        NapiParseUtils::CreateEnumConstructor, nullptr, sizeof(nmErorrCodeProperties) /
        sizeof(nmErorrCodeProperties[0]), nmErorrCodeProperties, &nmErrorCode);
    napi_set_named_property(env, exports, NATIVE_MESSAGING_ERROR_CODE_ENUM_NAME.c_str(), nmErrorCode);
}
} // namespace NWeb
} // namespace OHOS
