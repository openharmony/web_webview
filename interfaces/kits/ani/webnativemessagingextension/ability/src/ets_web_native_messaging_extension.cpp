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

#include "ets_web_native_messaging_extension.h"

#include "ability_handler.h"
#include "ani_common_configuration.h"
#include "ani_common_want.h"
#include "ets_web_native_messaging_extension_context.h"
#include "nweb_log.h"
#include "web_native_messaging_extension_stub_impl.h"

namespace OHOS {
namespace NWeb {
namespace {
constexpr const char* ON_DESTROY_METHOD_NAME = "onDestroy";
constexpr const char* WEB_NATIVE_MESSAGING_EXTENSION_CLASS_NAME =
    "@ohos.web.WebNativeMessagingExtensionAbility.WebNativeMessagingExtensionAbility";
constexpr const char* ANI_CLASS_CONNECTION_INFO =
    "@ohos.web.WebNativeMessagingExtensionAbility.ConnectionInfoInner";
constexpr const char* CONNECTION_INFO_SIGNATUR =
    "C{@ohos.web.WebNativeMessagingExtensionAbility.ConnectionInfo}:";
} // namespace

using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;

ETSWebNativeMessagingExtension* ETSWebNativeMessagingExtension::Create(
    const std::unique_ptr<Runtime>& runtime)
{
    return new ETSWebNativeMessagingExtension(static_cast<ETSRuntime&>(*runtime));
}

ETSWebNativeMessagingExtension::ETSWebNativeMessagingExtension(ETSRuntime& etsRuntime)
    :etsRuntime_(etsRuntime) {}
ETSWebNativeMessagingExtension::~ETSWebNativeMessagingExtension()
{
    auto env = etsRuntime_.GetAniEnv();
    if (env == nullptr) {
        WNMLOG_E("env null");
        return;
    }
    if (etsObj_ == nullptr) {
        WNMLOG_E("etsObj_ null");
        return;
    }
    if (etsObj_->aniRef) {
        env->GlobalReference_Delete(etsObj_->aniRef);
    }
}

void ETSWebNativeMessagingExtension::Init(const std::shared_ptr<AbilityLocalRecord>& record,
    const std::shared_ptr<OHOSApplication>& application, std::shared_ptr<AbilityHandler>& handler,
    const sptr<IRemoteObject>& token)
{
    WebNativeMessagingExtension::Init(record, application, handler, token);
    if (record == nullptr) {
        WNMLOG_E("AbilityLocalRecord null");
        return;
    }
    auto abilityInfo = record->GetAbilityInfo();
    if (abilityInfo == nullptr) {
        WNMLOG_E("abilityInfo null");
        return;
    }
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        WNMLOG_E("get srcPath failed");
        return;
    }
    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    WNMLOG_D("moduleName:%{public}s,srcPath:%{public}s,"
        "compileMode :%{public}d",
        moduleName.c_str(), srcPath.c_str(), abilityInfo_->compileMode);

    etsObj_ = etsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
        abilityInfo_->compileMode == CompileMode::ES_MODULE, false, abilityInfo_->srcEntrance);
    if (etsObj_ == nullptr) {
        WNMLOG_E("null etsObj_");
        return;
    }
    auto env = etsRuntime_.GetAniEnv();
    BindContext(env, etsObj_->aniObj);
}

void ETSWebNativeMessagingExtension::BindContext(ani_env* env, ani_object obj)
{
    if (env == nullptr) {
        WNMLOG_E("null env");
        return;
    }
    if (etsObj_ == nullptr) {
        WNMLOG_E("null etsObj_");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        WNMLOG_E("null context");
        return;
    }
    ani_ref contextObj = CreateEtsWebNativeMessagingExtensionContext(env, context);
    if (contextObj == nullptr) {
        WNMLOG_E("Create context obj error");
        return;
    }
    ani_ref contextRef = nullptr;
    ani_field field = nullptr;
    ani_status status = ANI_ERROR;
    if ((status = env->GlobalReference_Create(contextObj, &contextRef)) != ANI_OK) {
        WNMLOG_E("GlobalReference_Create failed, status : %{public}d", status);
        return;
    }
    if ((status = env->Class_FindField(etsObj_->aniCls, "context", &field)) != ANI_OK) {
        WNMLOG_E("Class_FindField failed, status : %{public}d", status);
        env->GlobalReference_Delete(contextRef);
        return;
    }
    if ((status = env->Object_SetField_Ref(
        etsObj_->aniObj, field, contextRef)) != ANI_OK) {
        WNMLOG_E("Object_SetField_Ref failed, status : %{public}d", status);
        env->GlobalReference_Delete(contextRef);
        return;
    }
}

sptr<IRemoteObject> ETSWebNativeMessagingExtension::OnConnect(const AAFwk::Want& want)
{
    Extension::OnConnect(want);
    if (providerRemoteObject_ == nullptr) {
        sptr<WebNativeMessagingExtensionStubImpl> remoteObject = new (std::nothrow)
            WebNativeMessagingExtensionStubImpl(
            std::static_pointer_cast<ETSWebNativeMessagingExtension>(
            shared_from_this()));
        if (remoteObject != nullptr) {
            providerRemoteObject_ = remoteObject->AsObject();
        }
    }
    return providerRemoteObject_;
}

bool CreateObject(ani_env *env, const char *className, ani_object& object)
{
    ani_class cls;
    ani_status status = env->FindClass(className, &cls);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s class failed, status: %{public}d", className, status);
        return false;
    }
    ani_method ctor;
    if ((status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) != ANI_OK) {
        WVLOG_E("get %{public}s ctor method failed, status: %{public}d", className, status);
        return false;
    }
    if ((status = env->Object_New(cls, ctor, &object)) != ANI_OK) {
        WVLOG_E("new %{public}s failed, status: %{public}d", className, status);
        return false;
    }
    return true;
}

ani_object ConnInfoToAni(ani_env* env, const WNMEConnectionInfo& params)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }

    ani_object connInfoAni = {};
    if (!CreateObject(env, ANI_CLASS_CONNECTION_INFO, connInfoAni)) {
        WVLOG_E("CreateConnectionInfo FAILED");
        return connInfoAni;
    }
    ani_int connectionIdAni =  static_cast<ani_int>(params.connectionId);
    env->Object_SetPropertyByName_Int(connInfoAni, "connectionId", connectionIdAni);
    ani_string bundleNameAni = nullptr;
    if (env->String_NewUTF8(
        params.bundleName.c_str(), params.bundleName.size(), &bundleNameAni) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(connInfoAni, "bundleName", bundleNameAni);
    }
    ani_string extensionOriginAni = nullptr;
    if (env->String_NewUTF8(
        params.extensionOrigin.c_str(), params.extensionOrigin.size(),
        &extensionOriginAni) == ANI_OK) {
        env->Object_SetPropertyByName_Ref(connInfoAni, "extensionOrigin", extensionOriginAni);
    }
    ani_int fdReadAni = static_cast<ani_int>(params.fdRead);
    env->Object_SetPropertyByName_Int(connInfoAni, "fdRead", fdReadAni);
    ani_int fdWriteAni = static_cast<ani_int>(params.fdWrite);
    env->Object_SetPropertyByName_Int(connInfoAni, "fdWrite", fdWriteAni);
    return connInfoAni;
}

bool FindMethod(ani_env* env,
    ani_method& method, const char* methodName, const char *signature)
{
    ani_class cls = nullptr;
    ani_status status = env->FindClass(
        WEB_NATIVE_MESSAGING_EXTENSION_CLASS_NAME, &cls);
    if (status != ANI_OK || !cls) {
        WVLOG_E("find %{public}s class failed, status: %{public}d",
                WEB_NATIVE_MESSAGING_EXTENSION_CLASS_NAME, status);
        return false;
    }

    status = env->Class_FindMethod(cls, methodName, signature, &method);
    if (status != ANI_OK) {
        WVLOG_E("find %{public}s method failed, status: %{public}d",
                methodName, status);
        return false;
    }
    return true;
}

void ETSWebNativeMessagingExtension::InvokeCallback(
    const char* methodName, const WNMEConnectionInfo& params)
{
    auto env = etsRuntime_.GetAniEnv();
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!etsObj_) {
        WVLOG_E("etsObj_ is nullptr");
        return;
    }
    ani_object object = etsObj_->aniObj;
    ani_method method = {};
    if (!FindMethod(env, method, methodName, CONNECTION_INFO_SIGNATUR)) {
        WVLOG_E("find method failed");
        return;
    }
    ani_object info = ConnInfoToAni(env, params);
    ani_status status = env->Object_CallMethod_Void(object, method, info);
    if (status != ANI_OK) {
        WNMLOG_E("call func failed status: %{public}d", status);
    }
    if (std::string(methodName) == std::string("onConnectNative")) {
        connmgr_.AddConnection(params);
    } else {
        connmgr_.RemoveConnection(params);
    }
    return;
}

int32_t ETSWebNativeMessagingExtension::InvokeCallbackInMainThread(
    const std::string& methodName, WNMEConnectionInfo& params)
{
    if (handler_ == nullptr) {
        WNMLOG_E("handler_ is nullptr");
        return -1;
    }
    auto etsServiceExtension =
        std::static_pointer_cast<ETSWebNativeMessagingExtension>(shared_from_this());
    auto task = [etsServiceExtension, methodName, data = params]() {
        if (etsServiceExtension) {
            etsServiceExtension->InvokeCallback(methodName.c_str(), data);
        }
    };
    handler_->PostTask(task, "ETSWebNativeMessagingExtension::" + methodName);
    return 0;
}

int32_t ETSWebNativeMessagingExtension::ConnectNative(WNMEConnectionInfo& connection)
{
    return InvokeCallbackInMainThread("onConnectNative", connection);
}

int32_t ETSWebNativeMessagingExtension::DisconnectNative(WNMEConnectionInfo& connection)
{
    return InvokeCallbackInMainThread("onDisconnectNative", connection);
}

void ETSWebNativeMessagingExtension::OnDestroy()
{
    auto env = etsRuntime_.GetAniEnv();
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return;
    }
    if (!etsObj_) {
        WVLOG_E("etsObj_ is nullptr");
        return;
    }
    ani_object object = etsObj_->aniObj;
    ani_method method = {};
    if (!FindMethod(env, method, ON_DESTROY_METHOD_NAME, ":V")) {
        WVLOG_E("find method failed");
        return;
    }
    env->Object_CallMethod_Void(object, method);
}

void ETSWebNativeMessagingExtension::OnStop()
{
    OnDestroy();
}

void ETSWebNativeMessagingExtension::GetSrcPath(std::string& srcPath)
{
    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        auto pos = srcPath.rfind(".");
        if (pos != std::string::npos) {
            srcPath.erase(pos);
            srcPath.append(".abc");
        }
    }
}
} // namespace NWeb
} // namespace OHOS