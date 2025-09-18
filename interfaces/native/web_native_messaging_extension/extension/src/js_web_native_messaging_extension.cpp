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

#include "js_web_native_messaging_extension.h"

#include "ability_handler.h"
#include "js_runtime_utils.h"
#include "js_web_native_messaging_extension_context.h"
#include "web_native_messaging_extension_stub_impl.h"

namespace OHOS {
namespace NWeb {
namespace {
constexpr int32_t ARGC_ONE = 1;
constexpr int32_t INDEX_ZERO = 0;
} // namespace

using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;

napi_value AttachWebNativeMessagingExtensionContext(napi_env env, void* value, void*)
{
    if (value == nullptr) {
        WNMLOG_E("null value");
        return nullptr;
    }
    auto ptr = reinterpret_cast<std::weak_ptr<WebNativeMessagingExtensionContext>*>(value)->lock();
    if (ptr == nullptr) {
        WNMLOG_E("null ptr");
        return nullptr;
    }
    napi_value object = CreateJsWebNativeMessagingExtensionContext(env, ptr);
    auto sysModule = JsRuntime::LoadSystemModuleByEngine(env, "web.webNativeMessagingExtensionContext", &object, 1);
    if (sysModule == nullptr) {
        WNMLOG_E("null sysModule");
        return nullptr;
    }
    auto contextObj = sysModule->GetNapiValue();
    napi_coerce_to_native_binding_object(
        env, contextObj, DetachCallbackFunc, AttachWebNativeMessagingExtensionContext, value, nullptr);
    auto workContext = new (std::nothrow) std::weak_ptr<WebNativeMessagingExtensionContext>(ptr);
    auto res = napi_wrap(
        env, contextObj, workContext,
        [](napi_env, void* data, void*) {
            WNMLOG_I("Finalizer for weak_ptr app service extension context is called");
            delete static_cast<std::weak_ptr<WebNativeMessagingExtensionContext>*>(data);
        },
        nullptr, nullptr);
    if (res != napi_ok && workContext != nullptr) {
        WNMLOG_E("napi_wrap failed");
        delete workContext;
        return nullptr;
    }
    return contextObj;
}

JsWebNativeMessagingExtension* JsWebNativeMessagingExtension::Create(const std::unique_ptr<Runtime>& runtime)
{
    return new JsWebNativeMessagingExtension(static_cast<JsRuntime&>(*runtime));
}

JsWebNativeMessagingExtension::JsWebNativeMessagingExtension(JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}

JsWebNativeMessagingExtension::~JsWebNativeMessagingExtension()
{
    auto context = GetContext();
    if (context) {
        context->Unbind();
    }

    jsRuntime_.FreeNativeReference(std::move(jsObj_));
    jsRuntime_.FreeNativeReference(std::move(shellContextRef_));
}

void JsWebNativeMessagingExtension::Init(const std::shared_ptr<AbilityLocalRecord>& record,
    const std::shared_ptr<OHOSApplication>& application, std::shared_ptr<AbilityHandler>& handler,
    const sptr<IRemoteObject>& token)
{
    WebNativeMessagingExtension::Init(record, application, handler, token);

    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        WNMLOG_E("get srcPath failed");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    AbilityRuntime::HandleScope handleScope(jsRuntime_);
    auto env = jsRuntime_.GetNapiEnv();

    jsObj_ = jsRuntime_.LoadModule(moduleName, srcPath, abilityInfo_->hapPath,
        abilityInfo_->compileMode == CompileMode::ES_MODULE, false, abilityInfo_->srcEntrance);
    if (jsObj_ == nullptr) {
        WNMLOG_E("null jsObj_");
        return;
    }

    napi_value obj = jsObj_->GetNapiValue();
    if (!CheckTypeForNapiValue(env, obj, napi_object)) {
        WNMLOG_E("get JsWebNativeMessagingExtension obj failed");
        return;
    }

    BindContext(env, obj);
}

void JsWebNativeMessagingExtension::BindContext(napi_env env, napi_value obj)
{
    auto context = GetContext();
    if (context == nullptr) {
        WNMLOG_E("null context");
        return;
    }
    napi_value contextObj = CreateJsWebNativeMessagingExtensionContext(env, context);
    shellContextRef_ =
        JsRuntime::LoadSystemModuleByEngine(env, "web.webNativeMessagingExtensionContext", &contextObj, ARGC_ONE);
    if (shellContextRef_ == nullptr) {
        WNMLOG_E("null shellContextRef");
        return;
    }
    contextObj = shellContextRef_->GetNapiValue();
    if (!CheckTypeForNapiValue(env, contextObj, napi_object)) {
        WNMLOG_E("get context native obj failed");
        return;
    }
    auto workContext = new (std::nothrow) std::weak_ptr<WebNativeMessagingExtensionContext>(context);
    napi_coerce_to_native_binding_object(
        env, contextObj, DetachCallbackFunc, AttachWebNativeMessagingExtensionContext, workContext, nullptr);
    context->Bind(jsRuntime_, shellContextRef_.get());
    napi_set_named_property(env, obj, "context", contextObj);

    auto res = napi_wrap(
        env, contextObj, workContext,
        [](napi_env, void* data, void*) {
            delete static_cast<std::weak_ptr<WebNativeMessagingExtensionContext>*>(data);
        }, nullptr, nullptr);
    if (res != napi_ok && workContext != nullptr) {
        WNMLOG_E("napi_wrap failed:%{public}d", res);
        delete workContext;
        return;
    }
    WNMLOG_I("end BindContext");
}

sptr<IRemoteObject> JsWebNativeMessagingExtension::OnConnect(const AAFwk::Want& want)
{
    Extension::OnConnect(want);
    if (providerRemoteObject_ == nullptr) {
        sptr<WebNativeMessagingExtensionStubImpl> remoteObject = new (std::nothrow)
            WebNativeMessagingExtensionStubImpl(std::static_pointer_cast<JsWebNativeMessagingExtension>(
                shared_from_this()));
        if (remoteObject != nullptr) {
            providerRemoteObject_ = remoteObject->AsObject();
        }
    }
    return providerRemoteObject_;
}

napi_value ConnInfoToJs(napi_env env, const WNMEConnectionInfo& params)
{
    napi_value connInfoJs = nullptr;
    NAPI_CALL(env, napi_create_object(env, &connInfoJs));

    napi_value connectionIdJs;
    napi_create_int32(env, params.connectionId, &connectionIdJs);
    NAPI_CALL(env, napi_set_named_property(env, connInfoJs, "connectionId", connectionIdJs));

    napi_value bundleNameJs;
    NAPI_CALL(env, napi_create_string_utf8(env, params.bundleName.c_str(), NAPI_AUTO_LENGTH, &bundleNameJs));
    NAPI_CALL(env, napi_set_named_property(env, connInfoJs, "bundleName", bundleNameJs));

    napi_value extensionOriginJs;
    NAPI_CALL(env, napi_create_string_utf8(env, params.extensionOrigin.c_str(), NAPI_AUTO_LENGTH, &extensionOriginJs));
    NAPI_CALL(env, napi_set_named_property(env, connInfoJs, "extensionOrigin", extensionOriginJs));

    napi_value fdReadJs;
    napi_create_int32(env, params.fdRead, &fdReadJs);
    NAPI_CALL(env, napi_set_named_property(env, connInfoJs, "fdRead", fdReadJs));

    napi_value fdWriteJs;
    napi_create_int32(env, params.fdWrite, &fdWriteJs);
    NAPI_CALL(env, napi_set_named_property(env, connInfoJs, "fdWrite", fdWriteJs));
    return connInfoJs;
}

bool CheckValueType(const napi_env& env, const napi_value& value, const napi_valuetype type)
{
    if (value == nullptr) {
        return false;
    }
    napi_valuetype valuetype;
    NAPI_CALL_BASE(env, napi_typeof(env, value, &valuetype), false);
    return valuetype == type;
}

void JsWebNativeMessagingExtension::InvokeCallback(const char* methodName, const WNMEConnectionInfo& params)
{
    AbilityRuntime::HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_value abilityObj = jsObj_->GetNapiValue();
    napi_value method;
    napi_get_named_property(env, abilityObj, methodName, &method);
    if (method== nullptr) {
        WNMLOG_E("no find %{public}s", methodName);
        return;
    }
    if (!CheckValueType(env, method, napi_valuetype::napi_function)) {
        WNMLOG_E("wrong type %{public}s", methodName);
        return;
    }
    napi_value arg[ARGC_ONE];
    arg[INDEX_ZERO] = ConnInfoToJs(env, params);

    napi_status status = napi_call_function(env, abilityObj, method, ARGC_ONE, arg, nullptr);
    if (status != napi_ok) {
        WNMLOG_E("call js func failed: %{public}d", status);
    }

    if (std::string(methodName) == std::string("onConnectNative")) {
        connmgr_.AddConnection(params);
    } else {
        connmgr_.RemoveConnection(params);
    }
    return;
}

int32_t JsWebNativeMessagingExtension::InvokeCallbackInMainThread(
    const std::string& methodName, WNMEConnectionInfo& params)
{
    if (handler_ == nullptr) {
        WNMLOG_E("handler_ is nullptr");
        return -1;
    }
    auto jsServiceExtension = std::static_pointer_cast<JsWebNativeMessagingExtension>(shared_from_this());
    auto task = [jsServiceExtension, methodName, data = params]() {
        if (jsServiceExtension) {
            jsServiceExtension->InvokeCallback(methodName.c_str(), data);
        }
    };
    handler_->PostTask(task, "JsWebNativeMessagingExtension::" + methodName);
    return 0;
}

int32_t JsWebNativeMessagingExtension::ConnectNative(WNMEConnectionInfo& connection)
{
    return InvokeCallbackInMainThread("onConnectNative", connection);
}

int32_t JsWebNativeMessagingExtension::DisconnectNative(WNMEConnectionInfo& connection)
{
    return InvokeCallbackInMainThread("onDisconnectNative", connection);
}

void JsWebNativeMessagingExtension::OnDestroy()
{
    AbilityRuntime::HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_value abilityObj = jsObj_->GetNapiValue();
    napi_value method;
    const std::string methodName = "onDestroy";
    napi_get_named_property(env, abilityObj, methodName.c_str(), &method);
    if (method== nullptr) {
        WNMLOG_E("no find %{public}s", methodName.c_str());
        return;
    }
    if (!CheckValueType(env, method, napi_valuetype::napi_function)) {
        WNMLOG_E("wrong type %{public}s", methodName.c_str());
        return;
    }

    napi_status status = napi_call_function(env, abilityObj, method, 0, nullptr, nullptr);
    if (status != napi_ok) {
        WNMLOG_E("call js func failed: %{public}d", status);
    }
    return;
}

void JsWebNativeMessagingExtension::OnStop()
{
    OnDestroy();
}

void JsWebNativeMessagingExtension::GetSrcPath(std::string& srcPath)
{
    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
}

} // namespace NWeb
} // namespace OHOS
