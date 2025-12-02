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
    
    static void StartAbility(ani_env* env,
        ani_object obj, ani_object want, ani_object options)
    {
        auto etsExtensionContext = GetEtsAbilityContext(env, obj);
        if (etsExtensionContext == nullptr) {
            WNMLOG_E("null etsExtensionContext");
            return ;
        }
        etsExtensionContext->OnStartAbility(env, obj, want, options);
    }

    static void TerminateSelf(ani_env* env, ani_object obj)
    {
        auto etsExtensionContext = GetEtsAbilityContext(env, obj);
        if (etsExtensionContext == nullptr) {
            WNMLOG_E("null etsExtensionContext");
            return ;
        }
        etsExtensionContext->OnTerminateSelf(env, obj);
    }
private:
    std::weak_ptr<WebNativeMessagingExtensionContext> context_;

    static ETSWebNativeMessagingExtensionContext *GetEtsAbilityContext(
        ani_env *env, ani_object aniObj)
    {
        WNMLOG_D("GetEtsAbilityContext");
        ani_class cls = nullptr;
        ani_long nativeContextLong;
        ani_field contextField = nullptr;
        ani_status status = ANI_ERROR;
        if (env == nullptr) {
            WNMLOG_E("null env");
            return nullptr;
        }
        if ((status = env->FindClass(
            WEB_NATIVE_MESSAGING_EXTENSION_CONTEXT_CLASS_NAME, &cls)) != ANI_OK) {
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
            return ;
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
        AAFwk::Want& want, AAFwk::StartOptions& startOptions) const
    {
        if (!AppExecFwk::UnwrapWant(env, aniWant, want)) {
            WNMLOG_E("Parse param want failed");
            return false;
        }
        ani_boolean isUndefined = ANI_TRUE;
        env->Reference_IsUndefined(options, &isUndefined);
        if (isUndefined == ANI_FALSE) {
            if (!AppExecFwk::UnwrapStartOptions(env, options, startOptions)) {
                WNMLOG_E("Parse param options failed");
            }
        }
        return true;
    }

    void OnStartAbility(ani_env* env, ani_object obj, ani_object aniWant, ani_object options)
    {
        WNMLOG_D("OnStartAbility");
        if (env == nullptr) {
            WVLOG_E("env is nullptr");
            return ;
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
        cls == nullptr) {
        WNMLOG_E("Failed to find class, status : %{public}d", status);
        return nullptr;
    }
    if (!BindNativeMethods(env, cls)) {
        WNMLOG_E("Failed to BindNativeMethods");
        return nullptr;
    }
    if ((status = env->Class_FindMethod(cls, "<ctor>", "J:V", &method)) != ANI_OK ||
            method == nullptr) {
        WNMLOG_E("Failed to find constructor, status : %{public}d", status);
        return nullptr;
    }
    std::unique_ptr<ETSWebNativeMessagingExtensionContext> workContext =
            std::make_unique<ETSWebNativeMessagingExtensionContext>(context);
    if (workContext == nullptr) {
        WNMLOG_E("Failed to create etsServiceExtensionContext");
        return nullptr;
    }
    auto serviceContextPtr = new std::weak_ptr<WebNativeMessagingExtensionContext>
                                (workContext->GetAbilityContext());
    if ((status = env->Object_New(
        cls, method, &contextObj, (ani_long)workContext.release())) != ANI_OK ||
        contextObj == nullptr) {
        WNMLOG_E("Failed to create object, status : %{public}d", status);
        if (serviceContextPtr != nullptr) {
            delete serviceContextPtr;
            serviceContextPtr = nullptr;
        }
        return nullptr;
    }
    if (!ContextUtil::SetNativeContextLong(env, contextObj, (ani_long)(serviceContextPtr))) {
        WNMLOG_E("Failed to setNativeContextLong ");
        if (serviceContextPtr != nullptr) {
            delete serviceContextPtr;
            serviceContextPtr = nullptr;
        }
        return nullptr;
    }
    ContextUtil::CreateEtsBaseContext(env, cls, contextObj, context);
    CreateEtsExtensionContext(env, cls, contextObj, context, context->GetAbilityInfo());
    return contextObj;
}
} // namespace NWeb
} // namespace OHOS