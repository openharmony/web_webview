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
#include <memory>
#include <vector>
#include <array>
#include <cstring>

using namespace testing;
using namespace testing::ext;
using ani_status = int32_t;
using ani_boolean = uint8_t;
using ani_int = int32_t;
using ani_long = long;
using ani_ref = void*;
using ani_object = void*;
using ani_class = void*;
using ani_method = void*;
using ani_field = void*;
using ani_resolver = void*;
using ani_error = void*;
using ani_string = void*;

#define ANI_OK 0
#define ANI_ERROR -1
#define ANI_TRUE 1
#define ANI_FALSE 0
#define ANI_ALREADY_BINDED 1

struct AniEnv {
    ani_status findClassRet = ANI_OK;
    ani_status bindMethodsRet = ANI_OK;
    std::vector<ani_status> bindMethodsSeq;
    ani_status findMethodRet = ANI_OK;
    ani_status findFieldRet = ANI_OK;
    ani_status objectNewRet = ANI_OK;
    ani_status getFieldLongRet = ANI_OK;
    ani_status getFieldByNameLongRet = ANI_OK;
    ani_status setPropertyByNameRefRet = ANI_OK;
    ani_status callMethodVoidRet = ANI_OK;
    ani_status promiseNewRet = ANI_OK;
    ani_status promiseRejectRet = ANI_OK;
    ani_status promiseResolveRet = ANI_OK;
    ani_status globalRefCreateRet = ANI_OK;
    ani_status getPropertyByNameRefRet = ANI_OK;
    ani_status refIsUndefinedRet = ANI_OK;
    ani_boolean refIsUndefinedResult = ANI_FALSE;

    ani_long nativeContextFieldValue = 0;
    bool newObjectValid = true;

    int findClassCount = 0;
    int bindMethodsCount = 0;
    int findMethodCount = 0;
    int findFieldCount = 0;
    int objectNewCount = 0;
    int getFieldLongCount = 0;
    int promiseNewCount = 0;

    ani_status FindClass(const char*, ani_class* cls)
    {
        findClassCount++;
        if (findClassRet == ANI_OK) {
            *cls = reinterpret_cast<ani_class>(this);
        }
        return findClassRet;
    }
    ani_status ClassBindNativeMethods(ani_class, void*, size_t)
    {
        bindMethodsCount++;
        if (!bindMethodsSeq.empty()) {
            ani_status s = bindMethodsSeq.front();
            bindMethodsSeq.erase(bindMethodsSeq.begin());
            return s;
        }
        return bindMethodsRet;
    }
    ani_status ClassFindMethod(ani_class, const char*, const char*, ani_method* m)
    {
        findMethodCount++;
        if (findMethodRet == ANI_OK) {
            *m = reinterpret_cast<ani_method>(this);
        }
        return findMethodRet;
    }
    ani_status ClassFindField(ani_class, const char*, ani_field* f)
    {
        findFieldCount++;
        if (findFieldRet == ANI_OK) {
            *f = reinterpret_cast<ani_field>(this);
        }
        return findFieldRet;
    }
    ani_status ObjectNew(ani_class, ani_method, ani_object* obj, ani_long)
    {
        objectNewCount++;
        if (objectNewRet == ANI_OK && newObjectValid) {
            *obj = reinterpret_cast<ani_object>(this);
        }
        return objectNewRet;
    }
    ani_status ObjectGetFieldLong(ani_object, ani_field, ani_long* val)
    {
        getFieldLongCount++;
        if (getFieldLongRet == ANI_OK) {
            *val = nativeContextFieldValue;
        }
        return getFieldLongRet;
    }
    ani_status ObjectGetFieldByNameLong(ani_object, const char*, ani_long* val)
    {
        if (getFieldByNameLongRet == ANI_OK) {
            *val = nativeContextFieldValue;
        }
        return getFieldByNameLongRet;
    }
    ani_status ObjectSetPropertyByNameRef(ani_object, const char*, ani_ref)
    {
        return setPropertyByNameRefRet;
    }
    ani_status ObjectCallMethodVoid(ani_object, ani_method, ani_int)
    {
        return callMethodVoidRet;
    }
    ani_status ObjectGetPropertyByNameRef(ani_object, const char*, ani_ref* ref)
    {
        if (getPropertyByNameRefRet == ANI_OK) {
            *ref = reinterpret_cast<ani_ref>(this);
        }
        return getPropertyByNameRefRet;
    }
    ani_status PromiseNew(ani_resolver* resolver, ani_object* promise)
    {
        promiseNewCount++;
        if (promiseNewRet == ANI_OK) {
            *resolver = reinterpret_cast<ani_resolver>(this);
            *promise = reinterpret_cast<ani_object>(this);
        }
        return promiseNewRet;
    }
    ani_status PromiseResolverReject(ani_resolver, ani_error)
    {
        return promiseRejectRet;
    }
    ani_status PromiseResolverResolve(ani_resolver, ani_object)
    {
        return promiseResolveRet;
    }
    ani_status GlobalReferenceCreate(ani_ref, ani_ref* ref)
    {
        if (globalRefCreateRet == ANI_OK) {
            *ref = reinterpret_cast<ani_ref>(this);
        }
        return globalRefCreateRet;
    }
    ani_status ReferenceIsUndefined(ani_object, ani_boolean* result)
    {
        if (refIsUndefinedRet == ANI_OK) {
            *result = refIsUndefinedResult;
        }
        return refIsUndefinedRet;
    }
};

namespace OHOS::NWeb {

class WebNativeMessagingExtensionContext {
public:
    virtual ~WebNativeMessagingExtensionContext() = default;
    virtual int32_t StartAbility() { return 0; }
    virtual int32_t TerminateSelf() { return 0; }
    virtual void* GetAbilityInfo() { return nullptr; }
};

static const char* CLEANER_CLASS_NAME =
    "@ohos.web.WebNativeMessagingExtensionContext.Cleaner";
static const char* WEB_NATIVE_MESSAGING_EXTENSION_CONTEXT_CLASS_NAME =
    "@ohos.web.WebNativeMessagingExtensionContext.WebNativeMessagingExtensionContext";

}

namespace OHOS::NWeb {

bool BindNativeMethods(ani_env* env, ani_class& cls)
{
    if (env == nullptr) {
        return false;
    }
    ani_status status = env->ClassBindNativeMethods(cls, nullptr, 0);
    if (status != ANI_OK && status != ANI_ALREADY_BINDED) {
        return false;
    }
    ani_class cleanerCls = nullptr;
    status = env->FindClass(CLEANER_CLASS_NAME, &cleanerCls);
    if (status != ANI_OK || cleanerCls == nullptr) {
        return false;
    }
    status = env->ClassBindNativeMethods(cleanerCls, nullptr, 0);
    if (status != ANI_OK && status != ANI_ALREADY_BINDED) {
        return false;
    }
    return true;
}

ani_ref CreateEtsWebNativeMessagingExtensionContext(
    ani_env* env, std::shared_ptr<WebNativeMessagingExtensionContext> context)
{
    if (env == nullptr) {
        return nullptr;
    }
    ani_class cls = nullptr;
    if (env->FindClass(WEB_NATIVE_MESSAGING_EXTENSION_CONTEXT_CLASS_NAME, &cls) != ANI_OK || !cls) {
        return nullptr;
    }
    if (!BindNativeMethods(env, cls)) {
        return nullptr;
    }
    ani_method method = nullptr;
    if (env->ClassFindMethod(cls, "<ctor>", "l:", &method) != ANI_OK || method == nullptr) {
        return nullptr;
    }
    if (!context) {
        return nullptr;
    }
    ani_object contextObj = nullptr;
    if (env->ObjectNew(cls, method, &contextObj, 0L) != ANI_OK || contextObj == nullptr) {
        return nullptr;
    }
    return contextObj;
}

}

class EtsWebNativeMessagingExtensionContextTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

namespace OHOS::NWeb {


HWTEST_F(EtsWebNativeMessagingExtensionContextTest, BindNativeMethods_NullEnv, TestSize.Level1)
{
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(nullptr, cls));
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, BindNativeMethods_Success, TestSize.Level1)
{
    ani_env env;
    ani_class cls = nullptr;
    EXPECT_TRUE(BindNativeMethods(&env, cls));
    EXPECT_EQ(env.bindMethodsCount, 2);
    EXPECT_EQ(env.findClassCount, 1);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, BindNativeMethods_MainBindFails, TestSize.Level1)
{
    ani_env env;
    env.bindMethodsRet = ANI_ERROR;
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(&env, cls));
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, BindNativeMethods_MainBindAlreadyBinded, TestSize.Level1)
{
    ani_env env;
    env.bindMethodsRet = ANI_ALREADY_BINDED;
    ani_class cls = nullptr;
    EXPECT_TRUE(BindNativeMethods(&env, cls));
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, BindNativeMethods_FindCleanerFails, TestSize.Level1)
{
    ani_env env;
    env.findClassRet = ANI_ERROR;
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(&env, cls));
    EXPECT_EQ(env.bindMethodsCount, 1);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, BindNativeMethods_CleanerBindFails, TestSize.Level1)
{
    ani_env env;
    env.bindMethodsSeq = { ANI_OK, ANI_ERROR };
    ani_class cls = nullptr;
    EXPECT_FALSE(BindNativeMethods(&env, cls));
    EXPECT_EQ(env.bindMethodsCount, 2);
    EXPECT_EQ(env.findClassCount, 1);
}


HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_NullEnv, TestSize.Level1)
{
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(nullptr, ctx), nullptr);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_FindClassFails, TestSize.Level1)
{
    ani_env env;
    env.findClassRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(&env, ctx), nullptr);
    EXPECT_EQ(env.findClassCount, 1);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_BindNativeMethodsFails, TestSize.Level1)
{
    ani_env env;
    env.bindMethodsRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(&env, ctx), nullptr);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_FindMethodFails, TestSize.Level1)
{
    ani_env env;
    env.findMethodRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(&env, ctx), nullptr);
    EXPECT_EQ(env.findMethodCount, 1);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_NullContext, TestSize.Level1)
{
    ani_env env;
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(&env, nullptr), nullptr);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_ObjectNewFails, TestSize.Level1)
{
    ani_env env;
    env.objectNewRet = ANI_ERROR;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(&env, ctx), nullptr);
    EXPECT_EQ(env.objectNewCount, 1);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_ObjectNewInvalid, TestSize.Level1)
{
    ani_env env;
    env.newObjectValid = false;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    EXPECT_EQ(CreateEtsWebNativeMessagingExtensionContext(&env, ctx), nullptr);
}

HWTEST_F(EtsWebNativeMessagingExtensionContextTest, CreateContext_Success, TestSize.Level1)
{
    ani_env env;
    auto ctx = std::make_shared<WebNativeMessagingExtensionContext>();
    ani_ref result = CreateEtsWebNativeMessagingExtensionContext(&env, ctx);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(env.findClassCount, 1);
    EXPECT_EQ(env.findMethodCount, 1);
    EXPECT_EQ(env.objectNewCount, 1);
}

}
