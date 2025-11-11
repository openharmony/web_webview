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

#include "ani_web_native_messaging_extension_manager.h"

#include <cstdint>
#include <map>
#include <mutex>
#include <uv.h>

#include "ability.h"
#include "ani.h"
#include "ani_base_context.h"
#include "ani_business_error.h"
#include "ani_common_want.h"
#include "ani_parse_utils.h"
#include "context.h"
#include "napi_parse_utils.h"
#include "nweb_log.h"
#include "system_properties_adapter_impl.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_common.h"
#include "../../../../nativecommon/web_errors.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
namespace {
constexpr const char* WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_CLASS_NAME =
    "@ohos.web.webNativeMessagingExtensionManager.webNativeMessagingExtensionManager";
constexpr const char*  ANI_CLASS_CONNECTION_NATIVE_INFO =
    "@ohos.web.webNativeMessagingExtensionManager.webNativeMessagingExtensionManager.ConnectionNativeInfoInner";
constexpr const char*  NM_ERROR_CODE =
    "@ohos.web.webNativeMessagingExtensionManager.webNativeMessagingExtensionManager.NmErrorCode";
constexpr const char*  WEB_NATIVE_MESSAGING_EXTENSION_CALLBACK =
    "@ohos.web.webNativeMessagingExtensionManager."
    "webNativeMessagingExtensionManager.WebExtensionConnectionCallbackInner";
constexpr const char* CONNECTION_NATIVE_INFO_SIGNATUR =
    "C{@ohos.web.webNativeMessagingExtensionManager.webNativeMessagingExtensionManager.ConnectionNativeInfo}:";
constexpr const char* ON_FAILED_SIGNATUR =
    "E{@ohos.web.webNativeMessagingExtensionManager.webNativeMessagingExtensionManager.NmErrorCode}C{std.core.String}:";
constexpr const char* METHOD_ON_CONNECT = "onConnect";
constexpr const char* METHOD_ON_DISCONNECT = "onDisconnect";
constexpr const char* METHOD_ON_FAILED = "onFailed";
constexpr int32_t INDEX_ZERO = 0;
constexpr int32_t INDEX_ONE = 1;
constexpr int32_t INDEX_TWO = 2;
std::recursive_mutex g_connectsLock;
int32_t g_serialNumber = 1;
static std::map<int32_t, sptr<WebExtensionConnectionCallback>> g_connects;
bool g_initDiedRecipient = false;
}

const std::string TASK_ID_DELETE = "delete";
const std::string TASK_ID_CONNECT = "connect";

static void RemoveConnection(int32_t connectId)
{
    std::lock_guard<std::recursive_mutex> lock(g_connectsLock);
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
    std::lock_guard<std::recursive_mutex> lock(g_connectsLock);
    if (!g_initDiedRecipient) {
        auto diedRecipient = []() {
            std::lock_guard<std::recursive_mutex> lock(g_connectsLock);
            for (auto iter = g_connects.begin(); iter != g_connects.end();) {
                WNMLOG_I("remove connection %{public}d because of service death", iter->first);
                if (iter->second) {
                    iter->second->OnFailed(ConnectNativeRet::SERVICE_DIED_ERROR);
                }
                iter = g_connects.erase(iter);
            }
        };
        WebNativeMessagingClient::GetInstance().SetUserDefineDiedRecipient(diedRecipient);
        g_initDiedRecipient = true;
    }
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

static ani_object FillConnectionNativeInfo(ani_env* env, const ConnectionNativeInfo& info)
{
    ani_object infoObj = {};
    if (!env) {
        WNMLOG_E("env is nullptr");
        return infoObj;
    }
    if (!AniParseUtils::CreateObjectVoid(env, ANI_CLASS_CONNECTION_NATIVE_INFO, infoObj)) {
        WVLOG_E("CreateConnectionNativeInfo FAILED");
        return infoObj;
    }

    ani_int connectId =  static_cast<ani_int>(info.connectionId);
    ani_int extensionPid =  static_cast<ani_int>(info.extensionPid);

    env->Object_SetPropertyByName_Int(infoObj, "connectionId", connectId);
    env->Object_SetPropertyByName_Int(infoObj, "extensionPid", extensionPid);
    ani_string bundleName = nullptr;
    if (env->String_NewUTF8(info.bundleName.c_str(),
        info.bundleName.size(), &bundleName) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(infoObj, "bundleName", bundleName);
    }
    ani_string extensionOrigin = nullptr;
    if (env->String_NewUTF8(info.extensionOrigin.c_str(),
        info.extensionOrigin.size(), &extensionOrigin) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(infoObj, "extensionOrigin", extensionOrigin);
    }
    return infoObj;
}

static void TransformOnFailedErrorCode(
    int32_t innerErrorCode, int32_t& codeIndex, std::string& errMsg)
{
    switch (innerErrorCode) {
        case ConnectNativeRet::PERMISSION_CHECK_ERROR:
            codeIndex = INDEX_ZERO;
            errMsg = NmErrorMsg::PERMISSION_DENY_ERR_MSG;
            break;
        case ConnectNativeRet::WANT_FORMAT_ERROR:
            codeIndex = INDEX_ONE;
            errMsg = NmErrorMsg::WANT_CONTENT_ERR_MSG;
            break;
        default:
            codeIndex = INDEX_TWO;
            errMsg = NmErrorMsg::INNER_ERR_MSG + std::to_string(innerErrorCode);
            break;
    }
}

void CallETSConnectionMethod(ani_env* env, ani_ref callback,
    const char* methodName, ani_object resultObj)
{
    if (!env) {
        WNMLOG_E("env is nullptr");
        return;
    }
    ani_class cls {};
    ani_status status = env->FindClass(WEB_NATIVE_MESSAGING_EXTENSION_CALLBACK, &cls);
    if (status != ANI_OK) {
        WNMLOG_E("FindClass %{public}s error, status: %{public}d",
            WEB_NATIVE_MESSAGING_EXTENSION_CALLBACK, status);
        return;
    }
    ani_method method {};
    if ((status = env->Class_FindMethod(cls, methodName,
        CONNECTION_NATIVE_INFO_SIGNATUR, &method)) != ANI_OK) {
        WNMLOG_E("Class_FindMethod %{public}s error, status: %{public}d",
            methodName, status);
        return;
    }
    if ((status = env->Object_CallMethod_Void(
        reinterpret_cast<ani_object>(callback), method, resultObj)) != ANI_OK) {
        WNMLOG_E("Object_CallMethod_Void error, status: %{public}d", status);
    }
}
void CallETSOnFailedMethod(ani_env* env, ani_ref callback,
    ani_string errorMsg, ani_enum_item codeEnum)
{
    if (!env) {
        WNMLOG_E("env is nullptr");
        return;
    }
    ani_class cls {};
    ani_status status = env->FindClass(WEB_NATIVE_MESSAGING_EXTENSION_CALLBACK, &cls);
    if (status != ANI_OK) {
        WNMLOG_E("FindClass %{public}s error, status: %{public}d",
            WEB_NATIVE_MESSAGING_EXTENSION_CALLBACK, status);
        return;
    }
    ani_method method {};
    if ((status = env->Class_FindMethod(cls,
        METHOD_ON_FAILED, ON_FAILED_SIGNATUR, &method)) != ANI_OK) {
        WNMLOG_E("Class_FindMethod %{public}s error, status: %{public}d",
                 METHOD_ON_FAILED, status);
        return;
    }
    if ((status = env->Object_CallMethod_Void(
        reinterpret_cast<ani_object>(callback), method,
        static_cast<ani_object>(codeEnum), errorMsg)) != ANI_OK) {
        WNMLOG_E("Object_CallMethod_Void error, status: %{public}d", status);
    }
}

void InvokeConnectNativeCallback(std::shared_ptr<AniExtensionConnectionCallbackParam> param,
    std::shared_ptr<AniExtensionConnectionCallback> object)
{
    if (!param) {
        WNMLOG_E("param is invalid");
        return;
    }
    if (!object) {
        WNMLOG_E("object is invalid");
        return;
    }
    ani_env* env = object->GetEnv();
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    ani_object resultObj;
    switch (param->type_) {
        case ConnectCallbackType::ON_CONNECT_TYPE: {
            resultObj = FillConnectionNativeInfo(env, param->result_);
            CallETSConnectionMethod(env, param->aniRef_, METHOD_ON_CONNECT, resultObj);
            break;
        }
        case ConnectCallbackType::ON_DISCONNECT_TYPE: {
            RemoveConnection(param->result_.connectionId);
            resultObj = FillConnectionNativeInfo(env, param->result_);
            CallETSConnectionMethod(env, param->aniRef_, METHOD_ON_DISCONNECT, resultObj);
            break;
        }
        case ConnectCallbackType::ON_FAILED_TYPE: {
            RemoveConnection(param->result_.connectionId);
            int32_t errorCodeIndex;
            std::string errorMsg;
            TransformOnFailedErrorCode(param->errorNum_, errorCodeIndex, errorMsg);
            ani_enum_item codeEnum;
            AniParseUtils::GetEnumItemByIndex(env, NM_ERROR_CODE, errorCodeIndex, codeEnum);
            ani_string aniErrorMsg = nullptr;
            if (auto status = env->String_NewUTF8(errorMsg.c_str(), errorMsg.size(), &aniErrorMsg) != ANI_OK) {
                WNMLOG_E("String_NewUTF8 error status is %{public}d", status);
                return;
            }
            CallETSOnFailedMethod(env, param->aniRef_, aniErrorMsg, codeEnum);
            break;
        }
        default: {
            WNMLOG_E("type %{public}d is invalid", param->type_);
            return;
        }
    }
    return;
}

AniExtensionConnectionCallback::AniExtensionConnectionCallback(ani_env* env) : vm_(nullptr)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
}

void AniExtensionConnectionCallback::DoExtensionConnectCallback(
    ConnectionNativeInfo& info, int32_t errorNum, ConnectCallbackType type)
{
    if (!callback_) {
        WVLOG_E("callback is nullptr");
        return;
    }
    auto param = std::make_shared<AniExtensionConnectionCallbackParam>();
    if (param == nullptr) {
        WNMLOG_E("new AniExtensionConnectionCallbackParam failed");
        return;
    }
    param->type_ = type;
    param->aniRef_ = callback_->aniRef;
    param->result_ = info;
    param->errorNum_ = errorNum;

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
    auto connection = shared_from_this();
    auto task = [param, connection] () { InvokeConnectNativeCallback(param, connection); };
    bool postResult = false;
    postResult = mainHandler_->PostTask(
        std::move(task), TASK_ID_CONNECT, 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    if (postResult) {
        WVLOG_D("PostTask success");
    } else {
        WVLOG_E("PostTask failed");
    }
}

static void TriggerDeleteCall(std::shared_ptr<AniExtensionConnectionCallback> obj)
{
    WVLOG_D("AniExtensionConnectionCallback::TriggerDeleteCall");
    if (!obj) {
        WVLOG_E("obj is null.");
        return;
    }

    ani_env* env = obj->GetEnv();
    if (!env) {
        WVLOG_E("obj->GetEnv is null.");
        return;
    }
    auto callback = obj->GetCallBack();
    if (callback && callback->aniRef) {
        env->GlobalReference_Delete(callback->aniRef);
    }
}

void AniExtensionConnectionCallback::DeleteCallInThread()
{
    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner =
            OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            return;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }

    if (!mainHandler_) {
        WVLOG_E("mainHandler_ is null.");
        return;
    }
    std::shared_ptr<AniExtensionConnectionCallback> sharedThis = shared_from_this();
    auto task = [sharedThis] () { TriggerDeleteCall(sharedThis); };
    bool postResult = false;
    postResult = mainHandler_->PostTask(
        std::move(task), TASK_ID_DELETE, 0, OHOS::AppExecFwk::EventQueue::Priority::HIGH, {});
    if (postResult) {
        WVLOG_D("PostTask success");
    } else {
        WVLOG_E("PostTask failed");
    }
}

bool AniExtensionConnectionCallback::Register(ani_object object)
{
    ani_env *env = GetEnv();
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return false;
    }
    if (object == nullptr) {
        WVLOG_E("null object");
        return false;
    }
    callback_ = std::make_shared<AppExecFwk::ETSNativeReference>();

    ani_ref objRef = nullptr;
    ani_status status = ANI_ERROR;
    status = env->GlobalReference_Create(object, &objRef);
    if (status != ANI_OK) {
        WVLOG_E("GlobalReference_Create failed status: %{public}d", status);
        return false;
    }

    callback_->aniObj = object;
    callback_->aniRef = objRef;
    return true;
}

void AniExtensionConnectionCallback::OnExtensionConnect(ConnectionNativeInfo& info)
{
    DoExtensionConnectCallback(info, 0, ConnectCallbackType::ON_CONNECT_TYPE);
}

void AniExtensionConnectionCallback::OnExtensionDisconnect(ConnectionNativeInfo& info)
{
    DoExtensionConnectCallback(info, 0, ConnectCallbackType::ON_DISCONNECT_TYPE);
}

void AniExtensionConnectionCallback::OnExtensionFailed(int32_t connectionId, int32_t errorNum)
{
    ConnectionNativeInfo info;
    info.connectionId = connectionId;
    DoExtensionConnectCallback(info, errorNum, ConnectCallbackType::ON_FAILED_TYPE);
}

CommonAsyncContext::CommonAsyncContext(ani_env* env) : vm_(nullptr)
{
    if (!env) {
        WVLOG_E("env is nullptr");
        return;
    }
    env->GetVM(&vm_);
}

CommonAsyncContext::~CommonAsyncContext()
{
    ani_env* env = nullptr;
    if (vm_) {
        vm_->GetEnv(ANI_VERSION_1, &env);
    }
    if (env && callbackRef) {
        WNMLOG_D("~CommonAsyncContext delete callbackRef");
        env->GlobalReference_Delete(callbackRef);
        callbackRef = nullptr;
    }
}

static sptr<WebExtensionConnectionCallback> ParseAniExtensionConnectionCallback(
    ani_env *env, ani_object info)
{
    auto callback = std::make_shared<AniExtensionConnectionCallback>(env);
    bool isRegister = callback->Register(info);
    if (!isRegister) {
        WNMLOG_E("AniExtensionConnectionCallback  Register failed");
        return nullptr;
    }
    auto callbackRemote = new (std::nothrow) WebExtensionConnectionCallback(callback);
    if (!callbackRemote) {
        WNMLOG_E("new WebExtensionConnectionCallback failed");
        return nullptr;
    }
    return callbackRemote;
}

void ConnectNativeExcute(ani_env* env, ConnectNativeAsyncContext* asyncContext)
{
    WNMLOG_D("connectNative excute");
    if (asyncContext == nullptr) {
        WNMLOG_E("asyncContext is null");
        return;
    }
    asyncContext->errCode = WebNativeMessagingClient::GetInstance().ConnectWebNativeMessagingExtension(
        asyncContext->token, asyncContext->want, asyncContext->connectCallback,
        asyncContext->connectId);
}

void ConnectNativeComplete(ani_env* env, ConnectNativeAsyncContext* asyncContext)
{
    WNMLOG_D("connectNative complete");
    if (asyncContext == nullptr || asyncContext->connectCallback == nullptr) {
        WNMLOG_E("asyncContext is invalid");
        return;
    }
    if (asyncContext->errCode != 0) {
        asyncContext->connectCallback->OnFailed(asyncContext->errCode);
    }
}

static bool ParseUiAbilityContextToken(
    ani_env* env, ani_object contextObject, sptr<IRemoteObject>& token)
{
    ani_boolean stageMode = false;
    ani_status status = AbilityRuntime::IsStageContext(env, contextObject, stageMode);
    if (status != ANI_OK || !stageMode) {
        WNMLOG_E("not stage mode");
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "Parse param context failed, must be a context of stageMode.");
        return false;
    }
    auto context = AbilityRuntime::GetStageModeContext(env, contextObject);
    if (context == nullptr) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "Parse param context failed, must not be nullptr.");
        return false;
    }

    auto uiAbilityContext =
        AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context);
    if (uiAbilityContext == nullptr) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "Parse param context failed, must be UIAbilityContext.");
        return false;
    }
    token = uiAbilityContext->GetToken();
    return true;
}

static ani_int ConnectNative(ani_env* env,
                             ani_object context, ani_object want, ani_object callback)
{
    int32_t result = -1;
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return result;
    }
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() !=
             ProductDeviceType::DEVICE_TYPE_2IN1) {
        WNMLOG_E("ConnectNative: Capability not supported.");
        AniBusinessError::ThrowErrorByErrCode(env, CAPABILITY_NOT_SUPPORTED_ERROR);
        return result;
    }

    auto* asyncContext = new (std::nothrow) ConnectNativeAsyncContext(env);
    if (asyncContext == nullptr) {
        WNMLOG_E("insufficient memory for asyncContext.");
        return result;
    }
    std::unique_ptr<ConnectNativeAsyncContext> asyncContextPtr { asyncContext };

    if (!ParseUiAbilityContextToken(env, context, asyncContext->token)) {
        WVLOG_E("ParseUiAbilityContextToken context is nullptr");
        return result;
    }

    if (!AppExecFwk::UnwrapWant(env, want, asyncContext->want)) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "Parse param want failed, must be a want");
        return result;
    }
    asyncContext->connectCallback = ParseAniExtensionConnectionCallback(env, callback);
    if (asyncContext->connectCallback == nullptr) {
        WVLOG_E("ParseAniExtensionConnectionCallback connectCallback == nullptr");
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "Parse param callback failed.");
        return result;
    }
    asyncContext->connectId = InsertConnection(asyncContext->connectCallback);
    result = asyncContext->connectId;
    ConnectNativeExcute(env, asyncContext);
    ConnectNativeComplete(env, asyncContext);
    asyncContextPtr.release();
    return result;
}

static void DisconnectNativeSync(ani_env* env, ani_int aniConnectId)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (SystemPropertiesAdapterImpl::GetInstance().GetProductDeviceType() != ProductDeviceType::DEVICE_TYPE_2IN1) {
        WNMLOG_E("DisconnectNative: Capability not supported.");
        AniBusinessError::ThrowErrorByErrCode(env, CAPABILITY_NOT_SUPPORTED_ERROR);
        return;
    }

    int32_t connectId = static_cast<int32_t>(aniConnectId);
    if (connectId < 0) {
        AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
            "The type of 'connectionId' must be int and must be >= 0");
        return ;
    }
    int32_t errCode = WebNativeMessagingClient::GetInstance().DisconnectWebNativeMessagingExtension(connectId);
    if (errCode != 0) {
        std::string ErrorMesg = "";
        int32_t error = NativeMessageError::NativeCodeToJsCode(errCode, ErrorMesg);
        AniBusinessError::ThrowError(env, error, ErrorMesg);
    }
}

ani_status StsExtensionManagerInit(ani_env* env)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_namespace ns;
    ani_status status = env->FindNamespace(WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_CLASS_NAME, &ns);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s FindNamespace failed, status: %{public}d",
            WEB_NATIVE_MESSAGING_EXTENSION_MANAGER_CLASS_NAME, status);
        return ANI_ERROR;
    }
    std::array methodArray = {
        ani_native_function { "connectNative", nullptr, reinterpret_cast<void *>(ConnectNative) },
        ani_native_function { "disconnectNativeSync", nullptr, reinterpret_cast<void *>(DisconnectNativeSync) },
    };
    status = env->Namespace_BindNativeFunctions(ns, methodArray.data(), methodArray.size());
    if (status != ANI_OK) {
        WVLOG_E("webNativeMessagingManager Namespace_BindNativeFunctions failed status: %{public}d", status);
    }
    return ANI_OK;
}
extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    WVLOG_I("webNativeMessagingExtensionManager in ANI_Constructor");
    if (vm == nullptr || result == nullptr) {
        WVLOG_E("null vm or result");
        return ANI_INVALID_ARGS;
    }

    ani_env *env = nullptr;
    ani_status status = ANI_ERROR;
    status = vm->GetEnv(ANI_VERSION_1, &env);
    if (status != ANI_OK) {
        WVLOG_E("GetEnv failed, status=%{public}d", status);
        return ANI_NOT_FOUND;
    }
    StsExtensionManagerInit(env);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
}
}