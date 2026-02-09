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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <cstdint>
#include <cstddef>

#include "ani.h"
#include "test_mock_webview_controller.h"
#include "test_mock_ani_business_error.h"
#include "test_mock_ani_parse_utils.h"
#include "web_errors.h"

#include "ani_webview_controller.cpp"

class AniWebviewControllerTest : public testing::Test {
protected:
    // Mock Functions
    static ani_status MockFindEnum(ani_env* env, const char* enum_descriptor, ani_enum* result)
    {
        if (g_findEnumShouldFail) {
            return ANI_ERROR;
        }
        return ANI_OK;
    }

    static ani_status MockEnum_GetEnumItemByIndex(ani_env* env, ani_enum enm, ani_size index, ani_enum_item* result)
    {
        if (g_enumGetItemShouldFail) {
            return ANI_ERROR;
        }
        *result = reinterpret_cast<ani_enum_item>(GetMockObject(POOL_INDEX_ENUM_ITEM));
        return ANI_OK;
    }

    static ani_status MockReference_IsUndefined(ani_env* env, ani_ref ref, ani_boolean* result)
    {
        if (g_referenceIsUndefinedShouldFail) {
            return ANI_ERROR;
        }
        *result = g_referenceIsUndefinedResult;
        return ANI_OK;
    }

    static ani_status MockPromise_New(ani_env* env, ani_resolver* result_resolver, ani_object* result_promise)
    {
        if (g_promiseNewShouldFail) {
            return ANI_ERROR;
        }
        *result_resolver = reinterpret_cast<ani_resolver>(GetMockObject(POOL_INDEX_RESOLVER));
        *result_promise = reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_PROMISE));
        return ANI_OK;
    }

    static ani_status MockEnumItem_GetValue_Int(ani_env* env, ani_enum_item enum_item, ani_int* result)
    {
        if (g_enumItemGetValueShouldFail) {
            return ANI_ERROR;
        }
        *result = g_enumItemGetValueResult;
        return ANI_OK;
    }

    static ani_status MockCreateArrayBuffer(ani_env* env, size_t length, void** data_result,
        ani_arraybuffer* arraybuffer_result)
    {
        if (g_createArrayBufferShouldFail) {
            return ANI_ERROR;
        }
        static char buffer[ENV_BUFFER_SIZE];
        *data_result = buffer;
        *arraybuffer_result = reinterpret_cast<ani_arraybuffer>(GetMockObject(POOL_INDEX_ARRAY_BUFFER));
        return ANI_OK;
    }

    static constexpr size_t VTABLE_SIZE = 400;
    static constexpr size_t ENV_BUFFER_SIZE = 1024;
    static constexpr int MOCK_POOL_SIZE = 64;

    // vtable offset enumeration
    enum VTableOffset {
        VT_FIND_ENUM = 16,                    // FindEnum offset
        VT_REFERENCE_IS_UNDEFINED = 39,      // Reference_IsUndefined offset
        VT_ENUM_GET_ENUM_ITEM_BY_INDEX = 86, // Enum_GetEnumItemByIndex offset
        VT_ENUMITEM_GET_VALUE_INT = 88,      // EnumItem_GetValue_Int offset
        VT_CREATE_ARRAY_BUFFER = 384,        // CreateArrayBuffer offset
        VT_PROMISE_NEW = 386                 // Promise_New offset
    };

    // Address pool index enumeration
    enum MockPoolIndex {
        POOL_INDEX_ENV = 0,           // env pointer
        POOL_INDEX_CALLBACK = 1,      // callback object
        POOL_INDEX_MODE = 2,          // mode enum item
        POOL_INDEX_ANI_CLASS = 3,     // aniClass object
        POOL_INDEX_ENUM_ITEM = 4,     // ani_enum_item (Mock return value)
        POOL_INDEX_RESOLVER = 5,      // ani_resolver (Mock return value)
        POOL_INDEX_PROMISE = 6,       // ani_object promise (Mock return value)
        POOL_INDEX_ARRAY_BUFFER = 7,  // ani_arraybuffer (Mock return value)
        POOL_INDEX_NULL_CONTROLLER = 8 // controller (nullptr scenario)
    };

    static constexpr ani_int TEST_WEB_ID = 100;     // test WebId
    static constexpr ani_int INVALID_WEB_ID = -1;   // invalid WebId (error return value)
    static constexpr ani_int TEST_TIMEOUT_MS = 5000; // test timeout (milliseconds)

    // Static vtable and related variables
    static void* g_fakeVtable[VTABLE_SIZE];
    static bool g_vtableInitialized;
    static char g_envBuffer[ENV_BUFFER_SIZE];

    static bool g_findEnumShouldFail;
    static bool g_enumGetItemShouldFail;
    static bool g_referenceIsUndefinedShouldFail;
    static ani_boolean g_referenceIsUndefinedResult;
    static bool g_promiseNewShouldFail;
    static bool g_enumItemGetValueShouldFail;
    static ani_int g_enumItemGetValueResult;
    static bool g_createArrayBufferShouldFail;

    // Mock object address pool - each element has unique address
    static char g_mockObjectPool[MOCK_POOL_SIZE];

    // Get fake object pointer from pool
    static ani_object GetMockObject(int index)
    {
        if (index < 0 || index >= MOCK_POOL_SIZE) {
            return nullptr;
        }
        return reinterpret_cast<ani_object>(&g_mockObjectPool[index]);
    }

    // Get fake env pointer (using address pool)
    static ani_env* GetMockEnv()
    {
        return reinterpret_cast<ani_env*>(GetMockObject(POOL_INDEX_ENV));
    }

    // Create Mock ani_env
    static ani_env* CreateMockAniEnv()
    {
        if (!g_vtableInitialized) {
            (void)memset_s(g_fakeVtable, sizeof(g_fakeVtable), 0, sizeof(g_fakeVtable));
            // Set vtable function pointers
            g_fakeVtable[VT_FIND_ENUM] = (void*)MockFindEnum;
            g_fakeVtable[VT_REFERENCE_IS_UNDEFINED] = (void*)MockReference_IsUndefined;
            g_fakeVtable[VT_ENUM_GET_ENUM_ITEM_BY_INDEX] = (void*)MockEnum_GetEnumItemByIndex;
            g_fakeVtable[VT_ENUMITEM_GET_VALUE_INT] = (void*)MockEnumItem_GetValue_Int;
            g_fakeVtable[VT_CREATE_ARRAY_BUFFER] = (void*)MockCreateArrayBuffer;
            g_fakeVtable[VT_PROMISE_NEW] = (void*)MockPromise_New;
            g_vtableInitialized = true;
        }

        (void)memset_s(g_envBuffer, ENV_BUFFER_SIZE, 0, ENV_BUFFER_SIZE);
        ani_env* env = reinterpret_cast<ani_env*>(g_envBuffer);
        *(const void**)env = g_fakeVtable;  // Set c_api pointer
        return env;
    }

    void SetUp() override
    {
        testController_ = new OHOS::NWeb::WebviewController("test_tag");
        testController_->SetInitialized(true);
        OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;
        OHOS::NWeb::AniBusinessError::Reset();
        // Reset Mock control variables to default values
        g_findEnumShouldFail = false;
        g_enumGetItemShouldFail = false;
        g_referenceIsUndefinedShouldFail = false;
        g_referenceIsUndefinedResult = ANI_FALSE;  // Ensure default is not undefined
        g_promiseNewShouldFail = false;
        g_enumItemGetValueShouldFail = false;
        g_enumItemGetValueResult = 0;
        g_createArrayBufferShouldFail = false;
        OHOS::NWeb::AniParseUtils::g_isFunctionResult = true;
        OHOS::NWeb::WebviewController::g_terminateRenderProcessShouldFail = false;
    }

    void TearDown() override
    {
        delete testController_;
        testController_ = nullptr;
        OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    }

    OHOS::NWeb::WebviewController* testController_;

    bool WasErrorThrown() const
    {
        return OHOS::NWeb::AniBusinessError::g_errorWasThrown;
    }

    int32_t GetLastErrorCode() const
    {
        return OHOS::NWeb::AniBusinessError::g_lastErrorCode;
    }

    void SetControllerInitialized(bool initialized)
    {
        if (testController_) {
            testController_->SetInitialized(initialized);
        }
    }
};

void* AniWebviewControllerTest::g_fakeVtable[AniWebviewControllerTest::VTABLE_SIZE] = {nullptr};
bool AniWebviewControllerTest::g_vtableInitialized = false;
char AniWebviewControllerTest::g_envBuffer[AniWebviewControllerTest::ENV_BUFFER_SIZE] = {0};
bool AniWebviewControllerTest::g_findEnumShouldFail = false;
bool AniWebviewControllerTest::g_enumGetItemShouldFail = false;
bool AniWebviewControllerTest::g_referenceIsUndefinedShouldFail = false;
ani_boolean AniWebviewControllerTest::g_referenceIsUndefinedResult = ANI_FALSE;
bool AniWebviewControllerTest::g_promiseNewShouldFail = false;
bool AniWebviewControllerTest::g_enumItemGetValueShouldFail = false;
ani_int AniWebviewControllerTest::g_enumItemGetValueResult = 0;
bool AniWebviewControllerTest::g_createArrayBufferShouldFail = false;
char AniWebviewControllerTest::g_mockObjectPool[AniWebviewControllerTest::MOCK_POOL_SIZE] = {0};

using OHOS::NWeb::OnActive;
using OHOS::NWeb::OnInactive;
using OHOS::NWebError::INIT_ERROR;
using OHOS::NWeb::GetWebId;
using OHOS::NWeb::InnerGetWebId;
using OHOS::NWeb::GetAttachState;
using OHOS::NWeb::OnControllerAttachStateChange;
using OHOS::NWeb::OffControllerAttachStateChange;
using OHOS::NWeb::WaitForAttachedPromise;
using OHOS::NWeb::SetWebDetach;
using OHOS::NWeb::SetRenderProcessMode;
using OHOS::NWeb::GetRenderProcessMode;
using OHOS::NWeb::InitializeWebEngine;
using OHOS::NWeb::PauseAllTimers;
using OHOS::NWeb::ResumeAllTimers;
using OHOS::NWeb::SerializeWebState;
using OHOS::NWeb::TerminateRenderProcess;
using OHOS::NWebError::PARAM_CHECK_ERROR;

/**
 * @tc.name: OnActive_Success
 * @tc.desc: OnActive.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OnActive_Success)
{
    ani_env* env = GetMockEnv();
    OnActive(env, reinterpret_cast<ani_object>(testController_));

    EXPECT_FALSE(WasErrorThrown());
    EXPECT_TRUE(testController_->WasOnActiveCalled());
}

/**
 * @tc.name: OnActive_ErrorScenarios
 * @tc.desc: OnActive.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OnActive_ErrorScenarios)
{
    // env is nullptr
    OnActive(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniBusinessError::Reset();

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = GetMockEnv();
    OnActive(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // controller not initialized
    SetControllerInitialized(false);
    env = GetMockEnv();
    OnActive(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    EXPECT_FALSE(testController_->WasOnActiveCalled());
    OHOS::NWeb::AniBusinessError::Reset();
    SetControllerInitialized(true);
}

/**
 * @tc.name: OnInactive_Success
 * @tc.desc: OnInactive.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OnInactive_Success)
{
    ani_env* env = GetMockEnv();
    OnInactive(env, reinterpret_cast<ani_object>(testController_));

    EXPECT_FALSE(WasErrorThrown());
    EXPECT_TRUE(testController_->WasOnInactiveCalled());
}

/**
 * @tc.name: OnInactive_ErrorScenarios
 * @tc.desc: OnInactive.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OnInactive_ErrorScenarios)
{
    // env is nullptr
    OnInactive(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniBusinessError::Reset();

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = GetMockEnv();
    OnInactive(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // controller not initialized
    SetControllerInitialized(false);
    env = GetMockEnv();
    OnInactive(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    EXPECT_FALSE(testController_->WasOnInactiveCalled());
    OHOS::NWeb::AniBusinessError::Reset();
    SetControllerInitialized(true);
}

/**
 * @tc.name: GetWebId_Success
 * @tc.desc: GetWebId.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetWebId_Success)
{
    testController_->SetWebId(TEST_WEB_ID);
    ani_env* env = GetMockEnv();
    ani_int result = GetWebId(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, TEST_WEB_ID);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: GetWebId_ErrorScenarios
 * @tc.desc: GetWebId.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetWebId_ErrorScenarios)
{
    // env is nullptr
    ani_int result = GetWebId(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, INVALID_WEB_ID);

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = GetMockEnv();
    result = GetWebId(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_EQ(result, INVALID_WEB_ID);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // controller not initialized
    SetControllerInitialized(false);
    result = GetWebId(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, INVALID_WEB_ID);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    SetControllerInitialized(true);
}

/**
 * @tc.name: InnerGetWebId_Success
 * @tc.desc: InnerGetWebId.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, InnerGetWebId_Success)
{
    testController_->SetWebId(TEST_WEB_ID);
    ani_env* env = GetMockEnv();
    ani_int result = InnerGetWebId(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, TEST_WEB_ID);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: InnerGetWebId_ErrorScenarios
 * @tc.desc: InnerGetWebId.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, InnerGetWebId_ErrorScenarios)
{
    // env is nullptr
    ani_int result = InnerGetWebId(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, INVALID_WEB_ID);

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = GetMockEnv();
    result = InnerGetWebId(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_EQ(result, INVALID_WEB_ID);
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // controller not initialized
    SetControllerInitialized(false);
    result = InnerGetWebId(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, INVALID_WEB_ID);
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniBusinessError::Reset();
    SetControllerInitialized(true);
}

/**
 * @tc.name: GetAttachState_Success
 * @tc.desc: GetAttachState.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetAttachState_Success)
{
    ani_env* env = CreateMockAniEnv();
    GetAttachState(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: GetAttachState_ControllerIsNull
 * @tc.desc: GetAttachState.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetAttachState_ControllerIsNull)
{
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = CreateMockAniEnv();
    ani_enum_item result = GetAttachState(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_EQ(result, nullptr);
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;
}

/**
 * @tc.name: GetAttachState_ErrorScenarios
 * @tc.desc: GetAttachState.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetAttachState_ErrorScenarios)
{
    // env is nullptr
    ani_enum_item result = GetAttachState(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);

    // FindEnum fails
    g_findEnumShouldFail = true;
    ani_env* env = CreateMockAniEnv();
    result = GetAttachState(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);
    g_findEnumShouldFail = false;

    // Enum_GetEnumItemByIndex fails
    g_enumGetItemShouldFail = true;
    result = GetAttachState(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);
    g_enumGetItemShouldFail = false;
}

/**
 * @tc.name: OnControllerAttachStateChange_Success
 * @tc.desc: OnControllerAttachStateChange.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OnControllerAttachStateChange_Success)
{
    // Set Mock return value: callback is function, not undefined
    g_referenceIsUndefinedResult = ANI_FALSE;
    OHOS::NWeb::AniParseUtils::g_isFunctionResult = true;

    ani_env* env = CreateMockAniEnv();
    ani_object callback = GetMockObject(POOL_INDEX_CALLBACK);
    OnControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: OnControllerAttachStateChange_ErrorScenarios
 * @tc.desc: OnControllerAttachStateChange.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OnControllerAttachStateChange_ErrorScenarios)
{
    ani_object callback = GetMockObject(POOL_INDEX_CALLBACK);

    // env is nullptr
    OnControllerAttachStateChange(nullptr, reinterpret_cast<ani_object>(testController_), callback);
    // Function returns directly, no exception thrown
    EXPECT_FALSE(WasErrorThrown());

    // Reference_IsUndefined fails
    g_referenceIsUndefinedShouldFail = true;
    ani_env* env = CreateMockAniEnv();
    OnControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), PARAM_CHECK_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    g_referenceIsUndefinedShouldFail = false;

    // callback is undefined
    g_referenceIsUndefinedResult = ANI_TRUE;
    env = CreateMockAniEnv();
    OnControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), PARAM_CHECK_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    g_referenceIsUndefinedResult = ANI_FALSE;

    // callback type error (not function)
    OHOS::NWeb::AniParseUtils::g_isFunctionResult = false;
    env = CreateMockAniEnv();
    OnControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), PARAM_CHECK_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_isFunctionResult = true;

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    env = CreateMockAniEnv();
    OnControllerAttachStateChange(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)),
        callback);
    // Function returns directly, no exception thrown
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;
}

/**
 * @tc.name: OffControllerAttachStateChange_Success
 * @tc.desc: OffControllerAttachStateChange.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OffControllerAttachStateChange_Success)
{
    // Set Mock return value: callback is function, not undefined
    g_referenceIsUndefinedResult = ANI_FALSE;
    OHOS::NWeb::AniParseUtils::g_isFunctionResult = true;

    ani_env* env = CreateMockAniEnv();
    ani_object callback = GetMockObject(POOL_INDEX_CALLBACK);
    OffControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: OffControllerAttachStateChange_ErrorScenarios
 * @tc.desc: OffControllerAttachStateChange.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, OffControllerAttachStateChange_ErrorScenarios)
{
    ani_object callback = GetMockObject(POOL_INDEX_CALLBACK);

    // env is nullptr
    OffControllerAttachStateChange(nullptr, reinterpret_cast<ani_object>(testController_), callback);
    // Function returns directly, no exception thrown
    EXPECT_FALSE(WasErrorThrown());

    // Reference_IsUndefined fails
    g_referenceIsUndefinedShouldFail = true;
    ani_env* env = CreateMockAniEnv();
    OffControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), PARAM_CHECK_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    g_referenceIsUndefinedShouldFail = false;

    // callback is undefined (valid case, should not throw error)
    g_referenceIsUndefinedResult = ANI_TRUE;
    env = CreateMockAniEnv();
    OffControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    // undefined callback is valid
    EXPECT_FALSE(WasErrorThrown());
    g_referenceIsUndefinedResult = ANI_FALSE;

    // callback is neither undefined nor function (type error)
    g_referenceIsUndefinedResult = ANI_FALSE;
    OHOS::NWeb::AniParseUtils::g_isFunctionResult = false;
    env = CreateMockAniEnv();
    OffControllerAttachStateChange(env, reinterpret_cast<ani_object>(testController_), callback);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), PARAM_CHECK_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_isFunctionResult = true;

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    env = CreateMockAniEnv();
    OffControllerAttachStateChange(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)),
        callback);
    // Function returns directly, no exception thrown
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;
}

/**
 * @tc.name: WaitForAttachedPromise_Success
 * @tc.desc: WaitForAttachedPromise.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, WaitForAttachedPromise_Success)
{
    ani_env* env = CreateMockAniEnv();
    ani_int timeout = TEST_TIMEOUT_MS;
    ani_object result = WaitForAttachedPromise(env, reinterpret_cast<ani_object>(testController_), timeout);
    // Promise object should be created
    EXPECT_NE(result, nullptr);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: WaitForAttachedPromise_ErrorScenarios
 * @tc.desc: WaitForAttachedPromise.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, WaitForAttachedPromise_ErrorScenarios)
{
    ani_int timeout = TEST_TIMEOUT_MS;

    // env is nullptr
    ani_object result = WaitForAttachedPromise(nullptr, reinterpret_cast<ani_object>(testController_), timeout);
    EXPECT_EQ(result, nullptr);

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = CreateMockAniEnv();
    result = WaitForAttachedPromise(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)),
        timeout);
    EXPECT_EQ(result, nullptr);
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // Promise_New fails
    g_promiseNewShouldFail = true;
    env = CreateMockAniEnv();
    result = WaitForAttachedPromise(env, reinterpret_cast<ani_object>(testController_), timeout);
    EXPECT_EQ(result, nullptr);
    g_promiseNewShouldFail = false;
}

/**
 * @tc.name: SetWebDetach_Success
 * @tc.desc: SetWebDetach.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, SetWebDetach_Success)
{
    ani_env* env = GetMockEnv();
    ani_int nwebId = 100;
    SetWebDetach(env, reinterpret_cast<ani_object>(testController_), nwebId);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: SetWebDetach_ErrorScenarios
 * @tc.desc: SetWebDetach.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, SetWebDetach_ErrorScenarios)
{
    ani_int nwebId = 100;

    // env is nullptr
    SetWebDetach(nullptr, reinterpret_cast<ani_object>(testController_), nwebId);
    EXPECT_FALSE(WasErrorThrown());

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = GetMockEnv();
    SetWebDetach(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)), nwebId);
    EXPECT_FALSE(WasErrorThrown());
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;
}

/**
 * @tc.name: SetRenderProcessMode_Success
 * @tc.desc: SetRenderProcessMode.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, SetRenderProcessMode_Success)
{
    // Set Mock return value: mode is not undefined, EnumItem_GetValue_Int succeeds
    g_referenceIsUndefinedResult = ANI_FALSE;
    // RenderProcessMode enum value
    g_enumItemGetValueResult = 1;

    ani_env* env = CreateMockAniEnv();
    ani_enum_item mode = reinterpret_cast<ani_enum_item>(GetMockObject(POOL_INDEX_MODE));
    SetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_), mode);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: SetRenderProcessMode_ErrorScenarios
 * @tc.desc: SetRenderProcessMode.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, SetRenderProcessMode_ErrorScenarios)
{
    ani_enum_item mode = reinterpret_cast<ani_enum_item>(GetMockObject(POOL_INDEX_MODE));

    // env is nullptr
    SetRenderProcessMode(nullptr, reinterpret_cast<ani_object>(testController_), mode);
    // Function returns directly, no exception thrown
    EXPECT_FALSE(WasErrorThrown());

    // mode is undefined (valid case, return directly)
    g_referenceIsUndefinedResult = ANI_TRUE;
    ani_env* env = CreateMockAniEnv();
    SetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_), nullptr);
    EXPECT_FALSE(WasErrorThrown());
    g_referenceIsUndefinedResult = ANI_FALSE;

    // Reference_IsUndefined fails
    g_referenceIsUndefinedShouldFail = true;
    env = CreateMockAniEnv();
    SetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_), mode);
    // Function returns directly, no exception thrown
    EXPECT_FALSE(WasErrorThrown());
    g_referenceIsUndefinedShouldFail = false;

    // EnumItem_GetValue_Int fails
    g_enumItemGetValueShouldFail = true;
    env = CreateMockAniEnv();
    SetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_), mode);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), PARAM_CHECK_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    g_enumItemGetValueShouldFail = false;
}

/**
 * @tc.name: GetRenderProcessMode_Success
 * @tc.desc: GetRenderProcessMode.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetRenderProcessMode_Success)
{
    ani_env* env = CreateMockAniEnv();
    ani_enum_item result = GetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_));
    // Should return valid enum_item
    EXPECT_NE(result, nullptr);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: GetRenderProcessMode_ErrorScenarios
 * @tc.desc: GetRenderProcessMode.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, GetRenderProcessMode_ErrorScenarios)
{
    // env is nullptr
    ani_enum_item result = GetRenderProcessMode(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);

    // FindEnum fails
    g_findEnumShouldFail = true;
    ani_env* env = CreateMockAniEnv();
    result = GetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);
    g_findEnumShouldFail = false;

    // Enum_GetEnumItemByIndex fails
    g_enumGetItemShouldFail = true;
    env = CreateMockAniEnv();
    result = GetRenderProcessMode(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);
    g_enumGetItemShouldFail = false;
}

/**
 * @tc.name: InitializeWebEngine_Success
 * @tc.desc: InitializeWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, InitializeWebEngine_Success)
{
    ani_env* env = GetMockEnv();
    ani_object aniClass = GetMockObject(POOL_INDEX_ANI_CLASS);
    InitializeWebEngine(env, aniClass);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: InitializeWebEngine_EnvIsNull
 * @tc.desc: InitializeWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, InitializeWebEngine_EnvIsNull)
{
    ani_object aniClass = GetMockObject(POOL_INDEX_ANI_CLASS);
    InitializeWebEngine(nullptr, aniClass);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: PauseAllTimers_Success
 * @tc.desc: PauseAllTimers.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, PauseAllTimers_Success)
{
    ani_env* env = GetMockEnv();
    PauseAllTimers(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: PauseAllTimers_EnvIsNull
 * @tc.desc: PauseAllTimers.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, PauseAllTimers_EnvIsNull)
{
    PauseAllTimers(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: ResumeAllTimers_Success
 * @tc.desc: ResumeAllTimers.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, ResumeAllTimers_Success)
{
    ani_env* env = GetMockEnv();
    ResumeAllTimers(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: ResumeAllTimers_EnvIsNull
 * @tc.desc: ResumeAllTimers.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, ResumeAllTimers_EnvIsNull)
{
    ResumeAllTimers(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: SerializeWebState_Success
 * @tc.desc: SerializeWebState.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, SerializeWebState_Success)
{
    ani_env* env = CreateMockAniEnv();
    ani_object result = SerializeWebState(env, reinterpret_cast<ani_object>(testController_));
    // Should return ArrayBuffer object
    EXPECT_NE(result, nullptr);
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: SerializeWebState_ErrorScenarios
 * @tc.desc: SerializeWebState.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, SerializeWebState_ErrorScenarios)
{
    // env is nullptr
    ani_object result = SerializeWebState(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = CreateMockAniEnv();
    result = SerializeWebState(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_EQ(result, nullptr);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // controller not initialized
    SetControllerInitialized(false);
    env = CreateMockAniEnv();
    result = SerializeWebState(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    SetControllerInitialized(true);

    // CreateArrayBuffer fails
    g_createArrayBufferShouldFail = true;
    env = CreateMockAniEnv();
    result = SerializeWebState(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, nullptr);
    g_createArrayBufferShouldFail = false;
}

/**
 * @tc.name: TerminateRenderProcess_Success
 * @tc.desc: TerminateRenderProcess.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, TerminateRenderProcess_Success)
{
    ani_env* env = GetMockEnv();
    TerminateRenderProcess(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_FALSE(WasErrorThrown());
}

/**
 * @tc.name: TerminateRenderProcess_ErrorScenarios
 * @tc.desc: TerminateRenderProcess.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(AniWebviewControllerTest, TerminateRenderProcess_ErrorScenarios)
{
    // env is nullptr
    ani_boolean result = TerminateRenderProcess(nullptr, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, ANI_FALSE);

    // controller is nullptr
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = nullptr;
    ani_env* env = GetMockEnv();
    result = TerminateRenderProcess(env, reinterpret_cast<ani_object>(GetMockObject(POOL_INDEX_NULL_CONTROLLER)));
    EXPECT_EQ(result, ANI_FALSE);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    OHOS::NWeb::AniParseUtils::g_testControllerPtr = testController_;

    // controller not initialized
    SetControllerInitialized(false);
    result = TerminateRenderProcess(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, ANI_FALSE);
    EXPECT_TRUE(WasErrorThrown());
    EXPECT_EQ(GetLastErrorCode(), INIT_ERROR);
    OHOS::NWeb::AniBusinessError::Reset();
    SetControllerInitialized(true);

    // TerminateRenderProcess fails
    OHOS::NWeb::WebviewController::g_terminateRenderProcessShouldFail = true;
    env = GetMockEnv();
    result = TerminateRenderProcess(env, reinterpret_cast<ani_object>(testController_));
    EXPECT_EQ(result, ANI_FALSE);
    OHOS::NWeb::WebviewController::g_terminateRenderProcessShouldFail = false;
}
