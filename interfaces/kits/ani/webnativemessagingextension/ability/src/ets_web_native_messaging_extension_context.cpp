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

#include "ets_web_native_messaging_extension_context.h"

#include "ability.h"
#include "ani_base_context.h"
#include "ets_error_utils.h"
#include "ani_common_start_options.h"
#include "ani_common_want.h"
#include "ani_business_error.h"
#include "ets_context_utils.h"
#include "ets_extension_context.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"
#include "nweb_log.h"
#include "web_errors.h"

namespace OHOS {
namespace NWeb {
using namespace NWebError;
using namespace AbilityRuntime;
namespace {

static const char* CLEANER_CLASS_NAME =
    "@ohos.web.WebNativeMessagingExtensionContext.Cleaner";
static const char* WEB_NATIVE_MESSAGING_EXTENSION_CONTEXT_CLASS_NAME =
    "@ohos.web.WebNativeMessagingExtensionContext.WebNativeMessagingExtensionContext";
static constexpr const char *ABILITY_RESULT_CLASS_NAME = "ability.abilityResult.AbilityResultInner";

class ETSWebNativeMessagingExtensionContext final {
public:
    explicit ETSWebNativeMessagingExtensionContext(
        const std::shared_ptr<WebNativeMessagingExtensionContext>& context)
        : context_(context)
    {}

    ~ETSWebNativeMessagingExtensionContext() = default;

    static void Finalizer(ani_env *env, ani_object obj)
    {
        if (env == nullptr) {
            WNMLOG_E("null env");
            return;
        }
        ani_long nativeEtsContextPtr;
        if (env->Object_GetFieldByName_Long(
            obj, "nativeEtsContext", &nativeEtsContextPtr) != ANI_OK) {
            WNMLOG_E("Failed to get nativeEtsContext");
            return;
        }
        if (nativeEtsContextPtr != 0) {
            delete reinterpret_cast<ETSWebNativeMessagingExtensionContext *>(nativeEtsContextPtr);
        }
    }

    std::weak_ptr<WebNativeMessagingExtensionContext> GetAbilityContext()
    {
        return context_;
    }

    static void StartAbility(ani_env* env, ani_object obj, ani_object want, ani_object options)
    {
        auto etsExtensionContext = GetEtsAbilityContext(env, obj);
        if (etsExtensionContext == nullptr) {
            WNMLOG_E("etsExtensionContext is null");
            return;
        }
        etsExtensionContext->OnStartAbility(env, obj, want, options);
    }

    static void TerminateSelf(ani_env* env, ani_object obj)
    {
        auto etsExtensionContext = GetEtsAbilityContext(env, obj);
        if (etsExtensionContext == nullptr) {
            WNMLOG_E("etsExtensionContext is null");
            return;
        }
        etsExtensionContext->OnTerminateSelf(env, obj);
    }

    static void StopNativeConnection(ani_env* env, ani_object obj, ani_int connectionId)
    {
        auto etsExtensionContext = GetEtsAbilityContext(env, obj);
        if (etsExtensionContext == nullptr) {
            WNMLOG_E("etsExtensionContext is null");
            return;
        }
        etsExtensionContext->OnStopNativeConnection(env, obj, connectionId);
    }

    static ani_object StartAbilityForResult(ani_env* env, ani_object obj, ani_object want, ani_object options)
    {
        auto etsExtensionContext = GetEtsAbilityContext(env, obj);
        if (etsExtensionContext == nullptr) {
            WNMLOG_E("etsExtensionContext is null");
            return nullptr;
        }
        return etsExtensionContext->OnStartAbilityForResult(env, obj, want, options);
    }
private:
    std::weak_ptr<WebNativeMessagingExtensionContext> context_;

    static ETSWebNativeMessagingExtensionContext *GetEtsAbilityContext(
        ani_env *env, ani_object aniObj)
    {
        WNMLOG_I("GetEtsAbilityContext");
        ani_class cls = nullptr;
        ani_long nativeContextLong;
        ani_field contextField = nullptr;
        ani_status status = ANI_ERROR;
        if (env == nullptr) {
            WNMLOG_E("null env");
            return nullptr;
        }
        if ((status = env->FindClass(
            WEB_NATIVE_MESSAGING_EXTENSION_CONTEXT_CLASS_NAME, &cls)) != ANI_OK || !cls) {
            WNMLOG_E("Failed to find class, status : %{public}d", status);
            return nullptr;
        }
        if ((status = env->Class_FindField(cls, "nativeEtsContext", &contextField)) != ANI_OK) {
            WNMLOG_E("Failed to find filed, status : %{public}d", status);
            return nullptr;
        }
        if ((status = env->Object_GetField_Long(aniObj, contextField, &nativeContextLong)) != ANI_OK) {
            WNMLOG_E("Failed to get filed, status : %{public}d", status);
            return nullptr;
        }

        return reinterpret_cast<ETSWebNativeMessagingExtensionContext *>(nativeContextLong);
    }

    void OnTerminateSelf(ani_env* env, ani_object obj)
    {
        WNMLOG_D("OnTerminateSelf");
        if (env == nullptr) {
            WVLOG_E("env is nullptr");
            return;
        }
        auto innerErrCode = std::make_shared<ErrCode>(ERR_OK);
        auto context = context_.lock();
        if (!context) {
            WNMLOG_E("context is null");
            *innerErrCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
        } else {
            *innerErrCode = context->TerminateSelf();
        }
        if (*innerErrCode != ERR_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, *innerErrCode);
        }
    }

    bool CheckStartAbilityInputParam(ani_env* env, ani_object aniWant, ani_object options,
        AAFwk::Want& want, AAFwk::StartOptions& startOptions)
    {
        if (!AppExecFwk::UnwrapWant(env, aniWant, want)) {
            WNMLOG_E("parse want failed");
            EtsErrorUtil::ThrowInvalidParamError(env, "Parse param want failed, want must be Want.");
            return false;
        }
        if (options) {
            AppExecFwk::UnwrapStartOptions(env, options, startOptions);
            UnwrapCompletionHandlerInStartOptions(env, options, startOptions);
        }
        return true;
    }

    void OnStartAbility(ani_env* env, ani_object obj, ani_object aniWant, ani_object options)
    {
        WNMLOG_D("OnStartAbility");
        if (env == nullptr) {
            WVLOG_E("env is nullptr");
            return;
        }
        AAFwk::Want want;
        AAFwk::StartOptions startOptions;

        if (!CheckStartAbilityInputParam(env, aniWant, options, want, startOptions)) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                                         "Parse param want failed, must be a want");
            return;
        }
        auto innerErrCode = std::make_shared<ErrCode>(ERR_OK);
        auto context = context_.lock();
        if (!context) {
            WNMLOG_E("context is null");
            *innerErrCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
        } else {
            *innerErrCode = context->StartAbility(want, startOptions);
        }

        if (*innerErrCode != ERR_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, *innerErrCode);
        }
    }

    void OnStopNativeConnection(ani_env* env, ani_object obj, ani_int connectionId)
    {
        WNMLOG_D("OnStopNativeConnection");
        if (env == nullptr) {
            WVLOG_E("env is nullptr");
            return;
        }

        int32_t connId = static_cast<int32_t>(connectionId);

        auto innerErrCode = std::make_shared<ErrCode>(ERR_OK);
        auto context = context_.lock();
        if (!context) {
            WNMLOG_E("context is null");
            *innerErrCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
        } else {
            *innerErrCode = context->StopNativeConnection(connId);
        }

        if (*innerErrCode != ERR_OK) {
            AniBusinessError::ThrowErrorByErrCode(env, *innerErrCode);
        }
    }

    bool RejectWithError(ani_env* env, ani_resolver resolver, int32_t errCode)
    {
        int32_t jsErrorCode = errCode;
        std::string errorMsg;
        if (NativeMessageError::IsNativeMessagingErr(errCode)) {
            jsErrorCode = NativeMessageError::NativeCodeToJsCode(errCode, errorMsg);
        } else {
            errorMsg = GetErrMsgByErrCode(errCode);
        }
        ani_ref jsResult = AniBusinessError::CreateError(env, jsErrorCode, errorMsg);
        if (jsResult == nullptr) {
            return false;
        }
        ani_status status = env->PromiseResolver_Reject(resolver, static_cast<ani_error>(jsResult));
        return status == ANI_OK;
    }

    bool CreatePromiseAndCheckContext(ani_env* env, ani_resolver& resolver, ani_object& promise,
        std::shared_ptr<WebNativeMessagingExtensionContext>& context)
    {
        ani_status status = env->Promise_New(&resolver, &promise);
        if (status != ANI_OK) {
            WNMLOG_E("Promise_New failed, status: %{public}d", status);
            return false;
        }

        context = context_.lock();
        if (!context) {
            WNMLOG_E("[WNWN_SAFR] context is null");
            int32_t errCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
            if (!RejectWithError(env, resolver, errCode)) {
                return false;
            }
            return true;
        }
        return true;
    }

    RuntimeTask CreateRuntimeTask(ani_env* env, ani_resolver resolver)
    {
        return [env, resolver, this](int resultCode, const AAFwk::Want& resultWant, bool isInner) {
            WNMLOG_I("RuntimeTask, resultCode: %{public}d, isInner: %{public}d", resultCode, isInner);
            if (isInner) {
                WNMLOG_E("Inner error, rejecting with error code: %{public}d", resultCode);
                RejectWithError(env, resolver, resultCode);
                return;
            }
            ani_object abilityResultObj = WrapAbilityResult(env, resultCode, resultWant);
            ani_status status = env->PromiseResolver_Resolve(resolver, abilityResultObj);
            if (status != ANI_OK) {
                WNMLOG_E("PromiseResolver_Resolve failed, status: %{public}d", status);
            }
        };
    }

    bool ExecuteStartAbilityForResult(ani_env* env, std::shared_ptr<WebNativeMessagingExtensionContext> context,
        const AAFwk::Want& want, const AAFwk::StartOptions& startOptions, ani_resolver resolver)
    {
        int requestCode = context->GenerateCurRequestCode();
        AAFwk::Want modifiedWant = want;
        modifiedWant.SetParam(AAFwk::Want::PARAM_RESV_FOR_RESULT, true);

        RuntimeTask task = CreateRuntimeTask(env, resolver);
        ErrCode err = context->StartAbilityForResult(modifiedWant, startOptions, requestCode, std::move(task));
        if (err != ERR_OK) {
            WNMLOG_E("StartAbilityForResult failed: %{public}d", err);
            if (!RejectWithError(env, resolver, err)) {
                return false;
            }
        } else {
            WNMLOG_I("StartAbilityForResult succeeded, callback registered");
        }
        return true;
    }

    bool CreateAbilityResultObject(ani_env* env, int resultCode, ani_object& resultObj)
    {
        ani_class resultCls = nullptr;
        ani_status status = env->FindClass("ability.abilityResult.AbilityResultInner", &resultCls);
        if (status != ANI_OK || resultCls == nullptr) {
            WNMLOG_E("Failed to find AbilityResult class");
            return false;
        }
        ani_method resultCtor = nullptr;
        status = env->Class_FindMethod(resultCls, "<ctor>", nullptr, &resultCtor);
        if (status != ANI_OK || resultCtor == nullptr) {
            WNMLOG_E("Failed to find AbilityResult constructor");
            return false;
        }
        status = env->Object_New(resultCls, resultCtor, &resultObj);
        if (status != ANI_OK || resultObj == nullptr) {
            WNMLOG_E("Failed to create AbilityResult object");
            return false;
        }
        ani_object resultCodeObj = nullptr;
        status = env->Class_FindMethod(resultCls, "<ctor>", "I:", &resultCtor);
        if (status != ANI_OK || resultCtor == nullptr) {
            WNMLOG_E("Failed to find Integer constructor");
            return false;
        }
        status = env->Object_New(resultCls, resultCtor, &resultCodeObj, resultCode);
        if (status != ANI_OK || resultCodeObj == nullptr) {
            WNMLOG_E("[WNM_SAF] Failed to create resultCode object");
            return false;
        }
        status = env->Object_SetPropertyByName_Ref(resultObj, "resultCode", resultCodeObj);
        if (status != ANI_OK) {
            WNMLOG_E("Failed to set resultCode property");
            return false;
        }
        return true;
    }

    bool SetWantProperty(ani_env *env, ani_object resultObj, const AAFwk::Want &want)
    {
        ani_status status = env->Object_SetPropertyByName_Ref(resultObj, "want", AppExecFwk::WrapWant(env, want));
        if (status != ANI_OK) {
            WNMLOG_E("Object_SetPropertyByName_Ref status: %{public}d", status);
            return false;
        }
        return true;
    }

    void UnwrapCompletionHandlerInStartOptions(ani_env *env, ani_object param, AAFwk::StartOptions &options)
    {
        WNMLOG_D("UnwrapCompletionHandlerInStartOptions called");
        if (env == nullptr) {
            WNMLOG_E("env null");
            return;
        }
        auto context = context_.lock();
        if (!context) {
            WNMLOG_E("null context");
            return;
        }
        ani_status status = ANI_ERROR;
        ani_ref completionHandler;
        if ((status = env->Object_GetPropertyByName_Ref(param, "completionHandler", &completionHandler)) != ANI_OK) {
            WNMLOG_E("null completionHandler");
            return;
        }
        ani_ref refCompletionHandler = nullptr;
        if (env->GlobalReference_Create(completionHandler, &refCompletionHandler) != ANI_OK || !refCompletionHandler) {
            WNMLOG_E("Failed to create global ref for completionHandler");
            return;
        }
        OnRequestResult onRequestSucc;
        OnRequestResult onRequestFail;
        AppExecFwk::CreateOnRequestResultCallback(env, refCompletionHandler, "onRequestSuccess", onRequestSucc);
        AppExecFwk::CreateOnRequestResultCallback(env, refCompletionHandler, "onRequestFailure", onRequestFail);
        uint64_t time = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
                                                  .count());
        std::string requestId = std::to_string(time);
        options.requestId_ = requestId;
    }

    ani_object WrapAbilityResult(ani_env *env, int32_t resultCode, const AAFwk::Want &want)
    {
        if (env == nullptr) {
            WNMLOG_E("null env");
            return nullptr;
        }
        ani_class cls = nullptr;
        ani_status status = env->FindClass(ABILITY_RESULT_CLASS_NAME, &cls);
        if (status != ANI_OK) {
            WNMLOG_E("FindClass status: %{public}d", status);
            return nullptr;
        }
        ani_method ctor = nullptr;
        if ((status = env->Class_FindMethod(cls, "<ctor>", nullptr, &ctor)) != ANI_OK) {
            WNMLOG_E("Class_FindMethod status: %{public}d", status);
            return nullptr;
        }
        ani_object resultObj = nullptr;
        if ((status = env->Object_New(cls, ctor, &resultObj)) != ANI_OK) {
            WNMLOG_E("Object_New status: %{public}d", status);
            return nullptr;
        }
        ani_method resultCodeSetter = nullptr;
        if ((status = env->Class_FindMethod(cls, "<set>resultCode", nullptr, &resultCodeSetter)) != ANI_OK) {
            WNMLOG_E("Class_FindMethod status: %{public}d", status);
            return nullptr;
        }
        if ((status = env->Object_CallMethod_Void(resultObj, resultCodeSetter, resultCode)) != ANI_OK) {
            WNMLOG_E("Object_CallMethod_Void status: %{public}d", status);
            return nullptr;
        }
        ani_method wantSetter = nullptr;
        if ((status = env->Class_FindMethod(cls, "<set>want", nullptr, &wantSetter)) != ANI_OK) {
            WNMLOG_E("Class_FindMethod status: %{public}d", status);
            return nullptr;
        }
        ani_object wantObj = AppExecFwk::WrapWant(env, want);
        if ((status = env->Object_CallMethod_Void(resultObj, wantSetter, wantObj)) != ANI_OK) {
            WNMLOG_E("Object_CallMethod_Void status: %{public}d", status);
            return nullptr;
        }
        return resultObj;
    }
    ani_object OnStartAbilityForResult(ani_env* env, ani_object obj, ani_object wantObj, ani_object startOptionsObj)
    {
        if (env == nullptr) {
            WVLOG_E("env is nullptr");
            return nullptr;
        }
        AAFwk::Want want;
        AAFwk::StartOptions startOptions;
        if (!CheckStartAbilityInputParam(env, wantObj, startOptionsObj, want, startOptions)) {
            AniBusinessError::ThrowError(env, PARAM_CHECK_ERROR,
                                         "Parse param want failed, must be a want");
            return nullptr;
        }

        ani_resolver resolver {};
        ani_object promise = nullptr;
        std::shared_ptr<WebNativeMessagingExtensionContext> context;
        if (!CreatePromiseAndCheckContext(env, resolver, promise, context)) {
            return nullptr;
        }
        if (context == nullptr) {
            return promise;
        }

        if (!ExecuteStartAbilityForResult(env, context, want, startOptions, resolver)) {
            return nullptr;
        }
        return promise;
    }
};
} // namespace

bool BindNativeMethods(ani_env *env, ani_class &cls)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return ANI_ERROR;
    }
    ani_status status = ANI_ERROR;
    std::array functions = {
        ani_native_function { "startAbilitySync", nullptr,
            reinterpret_cast<void *>(ETSWebNativeMessagingExtensionContext::StartAbility) },
        ani_native_function { "terminateSelfSync", nullptr,
            reinterpret_cast<void *>(ETSWebNativeMessagingExtensionContext::TerminateSelf) },
        ani_native_function { "stopNativeConnectionSync", nullptr,
            reinterpret_cast<void *>(ETSWebNativeMessagingExtensionContext::StopNativeConnection) },
        ani_native_function { "startAbilityForResultSync", nullptr,
            reinterpret_cast<void *>(ETSWebNativeMessagingExtensionContext::StartAbilityForResult) },
    };
    if ((status = env->Class_BindNativeMethods(cls, functions.data(), functions.size())) != ANI_OK
            && status != ANI_ALREADY_BINDED) {
        WNMLOG_E("bind method status : %{public}d", status);
        return false;
    }
    ani_class cleanerCls = nullptr;
    status = env->FindClass(CLEANER_CLASS_NAME, &cleanerCls);
    if (status != ANI_OK || cleanerCls == nullptr) {
        WNMLOG_E("Failed to find class, status : %{public}d", status);
        return false;
    }
    std::array CleanerMethods = {
        ani_native_function {
            "clean", nullptr, reinterpret_cast<void *>(ETSWebNativeMessagingExtensionContext::Finalizer)},
    };
    if ((status = env->Class_BindNativeMethods(
        cleanerCls, CleanerMethods.data(), CleanerMethods.size())) != ANI_OK &&
        status != ANI_ALREADY_BINDED) {
        WNMLOG_E("bind method status : %{public}d", status);
        return false;
    }
    return true;
}

ani_ref CreateEtsWebNativeMessagingExtensionContext(ani_env* env,
    std::shared_ptr<WebNativeMessagingExtensionContext> context)
{
    if (env == nullptr) {
        WVLOG_E("env is nullptr");
        return nullptr;
    }
    ani_class cls = nullptr;
    ani_status status = ANI_ERROR;
    ani_method method = nullptr;
    ani_object contextObj = nullptr;

    if ((status = env->FindClass(
        WEB_NATIVE_MESSAGING_EXTENSION_CONTEXT_CLASS_NAME, &cls)) != ANI_OK ||
        !cls) {
        WNMLOG_E("Failed to find class, status : %{public}d", status);
        return nullptr;
    }
    if (!BindNativeMethods(env, cls)) {
        WNMLOG_E("Failed to BindNativeMethods");
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "l:", &method)) != ANI_OK ||
            method == nullptr) {
        WNMLOG_E("Failed to find constructor, status : %{public}d", status);
        return nullptr;
    }
    auto workContext = std::make_unique<ETSWebNativeMessagingExtensionContext>(context);
    if (workContext == nullptr) {
        WNMLOG_E("Failed to create etsServiceExtensionContext");
        return nullptr;
    }
    auto serviceContextPtr = std::make_unique<std::weak_ptr<WebNativeMessagingExtensionContext>>(
                                workContext->GetAbilityContext());
    if ((status = env->Object_New(
        cls, method, &contextObj, (ani_long)workContext.get())) != ANI_OK ||
        contextObj == nullptr) {
        WNMLOG_E("Failed to create object, status : %{public}d", status);
        return nullptr;
    }
    workContext.release();
    if (!ContextUtil::SetNativeContextLong(env, contextObj, (ani_long)(serviceContextPtr.get()))) {
        WNMLOG_E("Failed to setNativeContextLong ");
        return nullptr;
    }
    serviceContextPtr.release();
    ContextUtil::CreateEtsBaseContext(env, cls, contextObj, context);
    CreateEtsExtensionContext(env, cls, contextObj, context, context->GetAbilityInfo());
    return contextObj;
}
} // namespace NWeb
} // namespace OHOS