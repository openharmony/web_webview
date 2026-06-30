/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef TEST_MOCK_ETS_CONTEXT_DEPS_H
#define TEST_MOCK_ETS_CONTEXT_DEPS_H

#include <ani.h>
#include <functional>
#include <string>

using ErrCode = int;
constexpr ErrCode ERR_OK = 0;

namespace OHOS {
namespace NWebError {
constexpr ErrCode PARAM_CHECK_ERROR = 401;

enum class AbilityErrorCode {
    ERROR_CODE_INVALID_CONTEXT = 16000002,
};

inline std::string GetErrMsgByErrCode(ErrCode code) { return std::string("mock_err_") + std::to_string(code); }

class AniBusinessError {
public:
    static ani_status ThrowError(ani_env* env, int32_t errorCode, const std::string& errorMessage)
    {
        throwErrorCount++;
        lastErrorCode = errorCode;
        return ANI_OK;
    }
    static ani_status ThrowErrorByErrCode(ani_env* env, int32_t errorCode)
    {
        throwErrorByErrCodeCount++;
        lastErrorCode = errorCode;
        return ANI_OK;
    }
    static ani_ref CreateError(ani_env* env, int32_t err) { return createErrorRet; }
    static ani_ref CreateError(ani_env* env, int32_t err, const std::string& errorMsg) { return createErrorRet; }

    inline static int throwErrorCount = 0;
    inline static int throwErrorByErrCodeCount = 0;
    inline static int32_t lastErrorCode = 0;
    inline static ani_ref createErrorRet = nullptr;
    static void Reset()
    {
        throwErrorCount = 0;
        throwErrorByErrCodeCount = 0;
        lastErrorCode = 0;
        createErrorRet = nullptr;
    }
};
} // namespace NWebError

namespace AAFwk {
class Want {
public:
    static const char* paramResvForResult;
    void SetParam(const std::string& key, bool value)
    {
        if (key == paramResvForResult) {
            forResult_ = value;
        }
    }
    bool forResult_ = false;
};
inline const char* Want::paramResvForResult = "ohos.aafwk.param.caller.forResult";

class StartOptions {
public:
    std::string requestId_;
};
} // namespace AAFwk

namespace NWeb {

using RuntimeTask = std::function<void(int resultCode, const AAFwk::Want& resultWant, bool isInner)>;
struct OnRequestResult {};

class WebNativeMessagingExtensionContext {
public:
    virtual ~WebNativeMessagingExtensionContext() = default;
    virtual int32_t StartAbility(const AAFwk::Want& want, const AAFwk::StartOptions& options)
    {
        return startAbilityRet;
    }
    virtual int32_t TerminateSelf() { return terminateRet; }
    virtual int32_t StopNativeConnection(int32_t connId) { return stopConnRet; }
    virtual int GenerateCurRequestCode() { return requestCode; }
    virtual ErrCode StartAbilityForResult(const AAFwk::Want& want, const AAFwk::StartOptions& options, int requestCode,
                                          RuntimeTask task)
    {
        lastTask = std::move(task);
        return startForResultRet;
    }
    virtual void* GetAbilityInfo() { return abilityInfo; }

    inline static int32_t startAbilityRet = ERR_OK;
    inline static int32_t terminateRet = ERR_OK;
    inline static int32_t stopConnRet = ERR_OK;
    inline static int requestCode = 0;
    inline static ErrCode startForResultRet = ERR_OK;
    inline static void* abilityInfo = nullptr;
    inline static RuntimeTask lastTask = nullptr;
    static void Reset()
    {
        startAbilityRet = ERR_OK;
        terminateRet = ERR_OK;
        stopConnRet = ERR_OK;
        requestCode = 0;
        startForResultRet = ERR_OK;
        abilityInfo = nullptr;
        lastTask = nullptr;
    }
};

namespace AppExecFwk {
inline bool g_unwrapWantRet = true;
inline bool g_unwrapStartOptionsCall = false;
inline ani_ref g_wrapWantRet = nullptr;

inline bool UnwrapWant(ani_env* env, ani_object aniWant, AAFwk::Want& want)
{
    if (g_unwrapWantRet) {
        want.forResult_ = false;
    }
    return g_unwrapWantRet;
}
inline void UnwrapStartOptions(ani_env* env, ani_object options, AAFwk::StartOptions& startOptions)
{
    g_unwrapStartOptionsCall = true;
}
inline ani_object WrapWant(ani_env* env, const AAFwk::Want& want)
{
    return static_cast<ani_object>(g_wrapWantRet);
}
inline void CreateOnRequestResultCallback(ani_env* env, ani_ref handler, const char* name, OnRequestResult& cb) {}
} // namespace AppExecFwk

namespace NativeMessageError {
inline bool g_isNativeMessagingErr = false;
inline int32_t g_nativeCodeToJsCodeRet = 0;
inline std::string g_nativeCodeToJsCodeMsg = "mock_js_msg";

inline bool IsNativeMessagingErr(int32_t errCode) { return g_isNativeMessagingErr; }
inline int32_t NativeCodeToJsCode(int32_t ret, std::string& errorMsg)
{
    errorMsg = g_nativeCodeToJsCodeMsg;
    return g_nativeCodeToJsCodeRet;
}
} // namespace NativeMessageError

namespace AbilityRuntime {
class ContextUtil {
public:
    inline static bool setNativeContextLongRet = true;
    inline static int createEtsBaseContextCount = 0;
    static bool SetNativeContextLong(ani_env* env, ani_object obj, ani_long ptr)
    {
        return setNativeContextLongRet;
    }
    static void CreateEtsBaseContext(ani_env* env, ani_class cls, ani_object obj,
                                    std::shared_ptr<WebNativeMessagingExtensionContext> context)
    {
        createEtsBaseContextCount++;
    }
};

class EtsErrorUtil {
public:
    inline static int throwInvalidParamCount = 0;
    static void ThrowInvalidParamError(ani_env* env, const std::string& msg) { throwInvalidParamCount++; }
};
} // namespace AbilityRuntime

inline int g_createEtsExtensionContextCount = 0;
inline void CreateEtsExtensionContext(ani_env* env, ani_class cls, ani_object obj,
                                      std::shared_ptr<WebNativeMessagingExtensionContext> context, void* abilityInfo)
{
    g_createEtsExtensionContextCount++;
}

} // namespace NWeb
} // namespace OHOS

#define WNMLOG_E(fmt, ...) (void)0
#define WNMLOG_I(fmt, ...) (void)0
#define WNMLOG_D(fmt, ...) (void)0
#define WVLOG_E(fmt, ...) (void)0
#define WVLOG_I(fmt, ...) (void)0
#define WVLOG_D(fmt, ...) (void)0

#endif // TEST_MOCK_ETS_CONTEXT_DEPS_H
