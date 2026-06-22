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

#include <gtest/gtest.h>
#include <cstring>
#include <functional>
#include <string>
#include <memory>
#include <array>
#include <vector>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <ani.h>

#include "test_mock_ets_context_deps.h"
#include "ets_web_native_messaging_extension_context.cpp"

using namespace testing;
using namespace testing::ext;
namespace OHOS::NWeb {

static ani_env g_mockEnv;
static ani_env* MakeMockEnv()
{
    g_mockEnv = ani_env{};
    return &g_mockEnv;
}

struct EtsCtxHolder {
    std::shared_ptr<WebNativeMessagingExtensionContext> wnme;
    std::unique_ptr<ETSWebNativeMessagingExtensionContext> ets;
    EtsCtxHolder() : wnme(std::make_shared<WebNativeMessagingExtensionContext>()),
                     ets(std::make_unique<ETSWebNativeMessagingExtensionContext>(wnme)) {}
    void InstallAsNativeField()
    {
        g_findClassRet = ANI_OK;
        g_findFieldRet = ANI_OK;
        g_getFieldLongRet = ANI_OK;
        g_nativeContextFieldValue = reinterpret_cast<ani_long>(ets.get());
    }
};

class EtsMessagingContextRealTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ResetAniMockState();
        WebNativeMessagingExtensionContext::Reset();
        NWebError::AniBusinessError::Reset();
        AbilityRuntime::ContextUtil::setNativeContextLongRet = true;
        AbilityRuntime::ContextUtil::createEtsBaseContextCount = 0;
        AbilityRuntime::EtsErrorUtil::throwInvalidParamCount = 0;
        g_createEtsExtensionContextCount = 0;
        AppExecFwk::g_unwrapWantRet = true;
        NativeMessageError::g_isNativeMessagingErr = false;
        NWebError::AniBusinessError::createErrorRet = reinterpret_cast<ani_ref>(0x1);
    }
};

HWTEST_F(EtsMessagingContextRealTest, BindNativeMethods_NullEnv, TestSize.Level1)
{
    ani_class cls = nullptr;
    int findClassBefore = g_findClassCount;
    int bindBefore = g_bindMethodsCount;
    EXPECT_TRUE(BindNativeMethods(nullptr, cls));
    EXPECT_EQ(g_findClassCount, findClassBefore);
    EXPECT_EQ(g_bindMethodsCount, bindBefore);
}

HWTEST_F(EtsMessagingContextRealTest, BindNativeMethods_Success, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    ani_class cls = nullptr;
    EXPECT_TRUE(BindNativeMethods(env, cls));
    EXPECT_EQ(g_bindMethodsCount, 2);
    EXPECT_EQ(g_findClassCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, BindNativeMethods_MainBindFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_bindMethodsRet = ANI_ERROR;
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(env, cls));
    EXPECT_EQ(g_bindMethodsCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, BindNativeMethods_MainBindAlreadyBinded, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_bindMethodsRet = ANI_ALREADY_BINDED;
    ani_class cls = nullptr;
    EXPECT_TRUE(BindNativeMethods(env, cls));
}

HWTEST_F(EtsMessagingContextRealTest, BindNativeMethods_FindCleanerFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_findClassRet = ANI_ERROR;
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(env, cls));
    EXPECT_EQ(g_bindMethodsCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, BindNativeMethods_CleanerBindFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_bindMethodsSeq = { ANI_OK, ANI_ERROR };
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(env, cls));
    EXPECT_EQ(g_bindMethodsCount, 2);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_NullEnv, TestSize.Level1)
{
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    int findClassBefore = g_findClassCount;
    int objNewBefore = g_objectNewCount;
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(nullptr, ctx), nullptr);
    EXPECT_EQ(g_findClassCount, findClassBefore);
    EXPECT_EQ(g_objectNewCount, objNewBefore);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_FindClassFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_findClassRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(env, ctx), nullptr);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_BindNativeMethodsFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_bindMethodsRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(env, ctx), nullptr);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_FindMethodFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_findMethodRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(env, ctx), nullptr);
    EXPECT_EQ(g_findMethodCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_ObjectNewFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_objectNewRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(env, ctx), nullptr);
    EXPECT_EQ(g_objectNewCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_SetNativeContextLongFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    AbilityRuntime::ContextUtil::setNativeContextLongRet = false;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(env, ctx), nullptr);
    EXPECT_EQ(g_objectNewCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, CreateContext_Success, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    ani_ref result = CreateEtsWebNativeMessagingExtensionContext(env, ctx);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(g_findClassCount, 2);
    EXPECT_EQ(g_findMethodCount, 1);
    EXPECT_EQ(g_objectNewCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_GetEtsCtxNull_EnvNull, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    int findBefore = g_findClassCount;
    int bindBefore = g_bindMethodsCount;
    ETSWebNativeMessagingExtensionContext::StartAbility(nullptr, nullptr, nullptr, nullptr);
    (void)env;
    EXPECT_EQ(g_findClassCount, findBefore);
    EXPECT_EQ(g_bindMethodsCount, bindBefore);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_GetEtsCtxNull_FindClassFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_findClassRet = ANI_ERROR;
    int bindBefore = g_bindMethodsCount;
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nullptr);
    EXPECT_EQ(g_findClassCount, 1);
    EXPECT_EQ(g_bindMethodsCount, bindBefore);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_GetEtsCtxNull_FindFieldFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_findFieldRet = ANI_ERROR;
    int findBefore = g_findClassCount;
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nullptr);
    EXPECT_EQ(g_findClassCount, findBefore + 1);
    EXPECT_EQ(g_findFieldRet, ANI_ERROR);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_Success, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nullptr);
    EXPECT_GT(WebNativeMessagingExtensionContext::startAbilityRet, -1);
    (void)h;
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_UnwrapWantFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    AppExecFwk::g_unwrapWantRet = false;
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nullptr);
    EXPECT_GT(AbilityRuntime::EtsErrorUtil::throwInvalidParamCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_WithOptionsCoversCompletionHandler, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    ani_object nonNullOptions = reinterpret_cast<ani_object>(0x1);
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nonNullOptions);
    EXPECT_TRUE(AppExecFwk::g_unwrapStartOptionsCall);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_WithOptionsCompletionHandlerGetFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    g_getPropertyRefRet = ANI_ERROR;
    ani_object nonNullOptions = reinterpret_cast<ani_object>(0x1);
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nonNullOptions);
    EXPECT_TRUE(AppExecFwk::g_unwrapStartOptionsCall);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_ContextExpired, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    h.wnme.reset();
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nullptr);
    EXPECT_GT(NWebError::AniBusinessError::throwErrorByErrCodeCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_StartAbilityReturnsError, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    WebNativeMessagingExtensionContext::startAbilityRet = -1;
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nullptr);
    EXPECT_GT(NWebError::AniBusinessError::throwErrorByErrCodeCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, TerminateSelf_Success, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    int throwBefore = NWebError::AniBusinessError::throwErrorByErrCodeCount;
    ETSWebNativeMessagingExtensionContext::TerminateSelf(env, nullptr);
    EXPECT_EQ(NWebError::AniBusinessError::throwErrorByErrCodeCount, throwBefore);
    EXPECT_EQ(WebNativeMessagingExtensionContext::terminateRet, ERR_OK);
}

HWTEST_F(EtsMessagingContextRealTest, TerminateSelf_ContextExpired, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    h.wnme.reset();
    ETSWebNativeMessagingExtensionContext::TerminateSelf(env, nullptr);
    EXPECT_GT(NWebError::AniBusinessError::throwErrorByErrCodeCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, StopNativeConnection_Success, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    int throwBefore = NWebError::AniBusinessError::throwErrorByErrCodeCount;
    ETSWebNativeMessagingExtensionContext::StopNativeConnection(env, nullptr, 42);
    EXPECT_EQ(NWebError::AniBusinessError::throwErrorByErrCodeCount, throwBefore);
    EXPECT_EQ(WebNativeMessagingExtensionContext::stopConnRet, ERR_OK);
}

HWTEST_F(EtsMessagingContextRealTest, StopNativeConnection_StopReturnsError, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    WebNativeMessagingExtensionContext::stopConnRet = -4;
    ETSWebNativeMessagingExtensionContext::StopNativeConnection(env, nullptr, 42);
    EXPECT_GT(NWebError::AniBusinessError::throwErrorByErrCodeCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_PromiseNewFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    g_promiseNewRet = ANI_ERROR;
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_EQ(r, nullptr);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_ContextExpired, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    h.wnme.reset();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_NE(r, nullptr);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_StartAbilityForResultError, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    WebNativeMessagingExtensionContext::startForResultRet = -1;
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_NE(r, nullptr);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_Success_ThenResolveTask, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(0, w, false);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_Success_ThenInnerErrorTask, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(1001, w, true);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_InnerError_IsNativeMessagingErr, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    NativeMessageError::g_isNativeMessagingErr = true;
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(99001, w, true);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_UnwrapWantFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    AppExecFwk::g_unwrapWantRet = false;
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    EXPECT_EQ(r, nullptr);
}

HWTEST_F(EtsMessagingContextRealTest, Finalizer_NullEnv, TestSize.Level1)
{
    int fieldBefore = g_getFieldLongCount;
    int findBefore = g_findClassCount;
    ETSWebNativeMessagingExtensionContext::Finalizer(nullptr, nullptr);
    EXPECT_EQ(g_getFieldLongCount, fieldBefore);
    EXPECT_EQ(g_findClassCount, findBefore);
    EXPECT_EQ(g_nativeContextFieldValue, 0);
}

HWTEST_F(EtsMessagingContextRealTest, Finalizer_GetFieldFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    g_getFieldLongRet = ANI_ERROR;
    int fieldBefore = g_getFieldLongCount;
    ETSWebNativeMessagingExtensionContext::Finalizer(env, nullptr);
    EXPECT_EQ(g_getFieldLongCount, fieldBefore + 1);
    EXPECT_EQ(g_nativeContextFieldValue, 0);
}

HWTEST_F(EtsMessagingContextRealTest, Finalizer_DeletesPtr, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    auto wnme = std::make_shared<WebNativeMessagingExtensionContext>();
    auto* raw = new ETSWebNativeMessagingExtensionContext(wnme);
    g_nativeContextFieldValue = reinterpret_cast<ani_long>(raw);
    ETSWebNativeMessagingExtensionContext::Finalizer(env, nullptr);
    EXPECT_EQ(g_getFieldLongCount, 1);
}

HWTEST_F(EtsMessagingContextRealTest, WrapAbilityResult_FindClassFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    ASSERT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    g_findClassRet = ANI_ERROR;
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(0, w, false);
}

HWTEST_F(EtsMessagingContextRealTest, WrapAbilityResult_FindMethodFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    ASSERT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    g_findMethodRet = ANI_ERROR;
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(0, w, false);
}

HWTEST_F(EtsMessagingContextRealTest, WrapAbilityResult_ObjectNewFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    ASSERT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    g_objectNewRet = ANI_ERROR;
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(0, w, false);
}

HWTEST_F(EtsMessagingContextRealTest, WrapAbilityResult_CallMethodVoidFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    ASSERT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    g_callMethodVoidRet = ANI_ERROR;
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(0, w, false);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_WithOptionsContextExpired, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    h.wnme.reset();
    ani_object nonNullOptions = reinterpret_cast<ani_object>(0x1);
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nonNullOptions);
    EXPECT_GT(NWebError::AniBusinessError::throwErrorByErrCodeCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbility_WithOptionsGlobalRefFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    g_globalRefCreateRet = ANI_ERROR;
    ani_object nonNullOptions = reinterpret_cast<ani_object>(0x1);
    ETSWebNativeMessagingExtensionContext::StartAbility(env, nullptr, nullptr, nonNullOptions);
    EXPECT_EQ(NWebError::AniBusinessError::throwErrorByErrCodeCount, 0);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_InnerError_CreateErrorNull, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    NWebError::AniBusinessError::createErrorRet = nullptr;
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    ASSERT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(1001, w, true);
}

HWTEST_F(EtsMessagingContextRealTest, StartAbilityForResult_ResolveTask_ResolveFails, TestSize.Level1)
{
    ani_env* env = MakeMockEnv();
    EtsCtxHolder h;
    h.InstallAsNativeField();
    auto r = ETSWebNativeMessagingExtensionContext::StartAbilityForResult(env, nullptr, nullptr, nullptr);
    ASSERT_NE(r, nullptr);
    ASSERT_NE(WebNativeMessagingExtensionContext::lastTask, nullptr);
    g_promiseResolveRet = ANI_ERROR;
    AAFwk::Want w;
    WebNativeMessagingExtensionContext::lastTask(0, w, false);
}

}
