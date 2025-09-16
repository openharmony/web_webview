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

#include "js_web_native_messaging_extension_context.h"

#include "js_extension_context.h"
#include "js_error_utils.h"
#include "napi_common_ability.h"
#include "web_native_messaging_common.h"
#include "web_native_messaging_log.h"

namespace OHOS {
namespace NWeb {
using namespace AbilityRuntime;
namespace {
constexpr int32_t ARGC_ZERO = 0;
constexpr int32_t ARGC_ONE = 1;
constexpr int32_t INDEX_ZERO = 0;
constexpr int32_t INDEX_ONE = 1;

napi_value CreateNMJsError(napi_env env, int32_t err)
{
    std::string ErrorMesg = "";
    int32_t jsError = NativeMessageError::NativeCodeToJsCode(err, ErrorMesg);
    return AbilityRuntime::CreateJsError(env, jsError, ErrorMesg.c_str());
}

class JsWebNativeMessagingExtensionContext final {
public:
    explicit JsWebNativeMessagingExtensionContext(const std::shared_ptr<WebNativeMessagingExtensionContext>& context)
        : context_(context)
    {}
    ~JsWebNativeMessagingExtensionContext() = default;

    static void Finalizer(napi_env env, void* data, void* hint)
    {
        JsWebNativeMessagingExtensionContext* context = static_cast<JsWebNativeMessagingExtensionContext*>(data);
        if (context == nullptr) {
            WNMLOG_E("context is null");
            return;
        }
        std::unique_ptr<JsWebNativeMessagingExtensionContext> ptr(context);
    }

    static napi_value StartAbility(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, JsWebNativeMessagingExtensionContext, OnStartAbility);
    }

    static napi_value TerminateSelf(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, JsWebNativeMessagingExtensionContext, OnTerminateSelf);
    }

    static napi_value StopNativeConnection(napi_env env, napi_callback_info info)
    {
        GET_NAPI_INFO_AND_CALL(env, info, JsWebNativeMessagingExtensionContext, OnStopNativeConnection);
    }

private:
    std::weak_ptr<WebNativeMessagingExtensionContext> context_;

    bool CheckType(const napi_env& env, const napi_value& value, const napi_valuetype& type)
    {
        napi_valuetype valuetype = napi_undefined;
        napi_typeof(env, value, &valuetype);
        if (valuetype != type) {
            WNMLOG_E("value type dismatch, [%{public}d]->[%{public}d]", valuetype, type);
            return false;
        }
        return true;
    }

    bool ParseInt32(const napi_env& env, const napi_value& value, int32_t& result)
    {
        if (!CheckType(env, value, napi_number)) {
            return false;
        }
        if (napi_get_value_int32(env, value, &result) != napi_ok) {
            WNMLOG_E("Cannot get value int32");
            return false;
        }
        return true;
    }

    napi_value OnStopNativeConnection(napi_env env, NapiCallbackInfo& info)
    {
        WNMLOG_I("OnStopNativeConnection");
        if (info.argc == ARGC_ZERO) {
            WNMLOG_E("invalid arg");
            ThrowTooFewParametersError(env);
            return CreateJsUndefined(env);
        }

        int32_t connectionId;
        if (!ParseInt32(env, info.argv[0], connectionId)) {
            ThrowInvalidParamError(env, "Parse param connectionId failed, type must be number");
            return CreateJsUndefined(env);
        }

        auto innerErrCode = std::make_shared<ErrCode>(ERR_OK);
        NapiAsyncTask::ExecuteCallback execute = [connectionId, weak = context_, innerErrCode]() {
            auto context = weak.lock();
            if (!context) {
                WNMLOG_E("context is null");
                *innerErrCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
                return;
            }
            *innerErrCode = context->StopNativeConnection(connectionId);
        };
        NapiAsyncTask::CompleteCallback complete =
            [innerErrCode](napi_env env, NapiAsyncTask& task, int32_t status) {
                if (*innerErrCode == ERR_OK) {
                    task.Resolve(env, CreateJsUndefined(env));
                } else if (NativeMessageError::IsNativeMessagingErr(*innerErrCode)) {
                    task.Reject(env, CreateNMJsError(env, *innerErrCode));
                } else {
                    task.Reject(env, CreateJsErrorByNativeErr(env, *innerErrCode));
                }
            };

        napi_value lastParam = nullptr;
        napi_value result = nullptr;
        NapiAsyncTask::ScheduleHighQos("JsWebNativeMessagingExtensionContext::OnStopNativeConnection",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    napi_value OnTerminateSelf(napi_env env, NapiCallbackInfo& info)
    {
        WNMLOG_I("OnTerminateSelf");
        auto innerErrCode = std::make_shared<ErrCode>(ERR_OK);
        NapiAsyncTask::ExecuteCallback execute = [weak = context_, innerErrCode]() {
            auto context = weak.lock();
            if (!context) {
                WNMLOG_E("context is null");
                *innerErrCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
                return;
            }
            *innerErrCode = context->TerminateSelf();
        };
        NapiAsyncTask::CompleteCallback complete =
            [innerErrCode](napi_env env, NapiAsyncTask& task, int32_t status) {
                if (*innerErrCode == ERR_OK) {
                    task.Resolve(env, CreateJsUndefined(env));
                } else {
                    task.Reject(env, CreateJsErrorByNativeErr(env, *innerErrCode));
                }
            };

        napi_value lastParam = nullptr;
        napi_value result = nullptr;
        NapiAsyncTask::ScheduleHighQos("JsWebNativeMessagingExtensionContext::TerminateSelf",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }

    bool CheckStartAbilityInputParam(napi_env env, NapiCallbackInfo& info,
        AAFwk::Want& want, AAFwk::StartOptions& startOptions, size_t& unwrapArgc) const
    {
        if (info.argc < ARGC_ONE) {
            return false;
        }
        unwrapArgc = ARGC_ZERO;
        if (!AppExecFwk::UnwrapWant(env, info.argv[INDEX_ZERO], want)) {
            return false;
        }
        ++unwrapArgc;
        if (info.argc > ARGC_ONE && CheckTypeForNapiValue(env, info.argv[INDEX_ONE], napi_object)) {
            AppExecFwk::UnwrapStartOptions(env, info.argv[INDEX_ONE], startOptions);
            unwrapArgc++;
        }
        return true;
    }

    napi_value OnStartAbility(napi_env env, NapiCallbackInfo& info)
    {
        WNMLOG_I("OnStartAbility");
        size_t unwrapArgc = 0;
        AAFwk::Want want;
        AAFwk::StartOptions startOptions;
        if (info.argc == ARGC_ZERO) {
            WNMLOG_E("invalid arg");
            ThrowTooFewParametersError(env);
            return CreateJsUndefined(env);
        }
        if (!CheckStartAbilityInputParam(env, info, want, startOptions, unwrapArgc)) {
            WNMLOG_E("Failed, input param type invalid");
            ThrowInvalidParamError(env, "Parse param want failed, type must be Want");
            return CreateJsUndefined(env);
        }

        auto innerErrCode = std::make_shared<ErrCode>(ERR_OK);
        NapiAsyncTask::ExecuteCallback execute =
            [weak = context_, want, startOptions, unwrapArgc, innerErrCode]() {
            auto context = weak.lock();
            if (!context) {
                WNMLOG_E("context is null");
                *innerErrCode = static_cast<int32_t>(AbilityErrorCode::ERROR_CODE_INVALID_CONTEXT);
                return;
            }
            *innerErrCode = context->StartAbility(want, startOptions);
        };
        NapiAsyncTask::CompleteCallback complete =
            [innerErrCode](napi_env env, NapiAsyncTask& task, int32_t status) {
            if (*innerErrCode == ERR_OK) {
                task.ResolveWithNoError(env, CreateJsUndefined(env));
            } else if (NativeMessageError::IsNativeMessagingErr(*innerErrCode)) {
                task.Reject(env, CreateNMJsError(env, *innerErrCode));
            } else {
                task.Reject(env, CreateJsErrorByNativeErr(env, *innerErrCode));
            }
        };

        napi_value lastParam = nullptr;
        napi_value result = nullptr;
        NapiAsyncTask::ScheduleHighQos("JsWebNativeMessagingExtensionContext::OnStartAbility",
            env, CreateAsyncTaskWithLastParam(env, lastParam, std::move(execute), std::move(complete), &result));
        return result;
    }
};
} // namespace

napi_value CreateJsWebNativeMessagingExtensionContext(napi_env env,
    std::shared_ptr<WebNativeMessagingExtensionContext> context)
{
    napi_value objValue = AbilityRuntime::CreateJsExtensionContext(env, context);
    std::unique_ptr<JsWebNativeMessagingExtensionContext> jsContext =
        std::make_unique<JsWebNativeMessagingExtensionContext>(context);
    napi_status status =
        napi_wrap(env, objValue, jsContext.release(),
        JsWebNativeMessagingExtensionContext::Finalizer, nullptr, nullptr);
    if (status != napi_ok) {
        WNMLOG_E("napi_wrap failed");
        return nullptr;
    }

    const char *moduleName = "JsWebNativeMessagingExtensionContext";
    BindNativeFunction(
        env, objValue, "startAbility", moduleName, JsWebNativeMessagingExtensionContext::StartAbility);
    BindNativeFunction(
        env, objValue, "terminateSelf", moduleName, JsWebNativeMessagingExtensionContext::TerminateSelf);
    BindNativeFunction(
        env, objValue, "stopNativeConnection",
        moduleName, JsWebNativeMessagingExtensionContext::StopNativeConnection);
    return objValue;
}
} // namespace NWeb
} // namespace OHOS
