/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <securec.h>
#include <ui/rs_surface_node.h>
#include <unordered_map>

#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/application_context.h"
#include "nweb.h"
#include "ohos_adapter_helper.h"
#include "sqlite_database_utils.h"
#include "window.h"

#define private public
#include "ohos_web_permission_data_base_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AbilityRuntime;
using namespace OHOS::NativeRdb;

namespace OHOS {
namespace {
const std::string TEST_ORIGIN = "test_origin";
const std::string NO_EXIST_ORIGIN = "no_exist_origin";
const std::string WEB_PATH = "/web";
const int32_t TIME_SECONDS_THREE = 3;
const int32_t DATABASE_DIR_PERMISSION = 0700;
std::shared_ptr<NWeb::OhosWebPermissionDataBaseAdapter> g_dataBaseNull = nullptr;
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
} // namespace

namespace AbilityRuntime {
std::shared_ptr<ApplicationContext> Context::GetApplicationContext()
{
    return g_applicationContext;
}
} // namespace AbilityRuntime

namespace NWeb {
class PermissionDataBaseAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static void InitPermissionDataBase(void);
};

class ApplicationContextMock : public ApplicationContext {
public:
    MOCK_CONST_METHOD0(GetBundleName, std::string());
    MOCK_METHOD0(GetCacheDir, std::string());
};

void PermissionDataBaseAdapterTest::InitPermissionDataBase(void)
{
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    dataBase.ClearAllPermission(WebPermissionType::GEOLOCATION);
    dataBase.SetPermissionByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION, true);
}

void PermissionDataBaseAdapterTest::SetUpTestCase(void)
{
    bool result = true;
    ApplicationContextMock *contextMock = new ApplicationContextMock();
    ASSERT_NE(contextMock, nullptr);
    EXPECT_CALL(*contextMock, GetBundleName())
        .Times(1)
        .WillRepeatedly(::testing::Return("com.example.testapplication"));
    EXPECT_CALL(*contextMock, GetCacheDir())
        .Times(TIME_SECONDS_THREE)
        .WillRepeatedly(::testing::Return("/data"));

    g_applicationContext.reset(contextMock);
    g_dataBaseNull.reset(new OhosWebPermissionDataBaseAdapterImpl());
    std::string databaseDir = g_applicationContext->GetCacheDir() + WEB_PATH;
    if (access(databaseDir.c_str(), F_OK) != 0) {
        result = mkdir(databaseDir.c_str(), DATABASE_DIR_PERMISSION) == 0;
        EXPECT_TRUE(result);
    }
    InitPermissionDataBase();
    g_applicationContext.reset();
    g_dataBaseNull.reset(new OhosWebPermissionDataBaseAdapterImpl());
    if (g_dataBaseNull == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);
}

void PermissionDataBaseAdapterTest::TearDownTestCase(void)
{
    system("rm -rf /data/web");
}

void PermissionDataBaseAdapterTest::SetUp(void)
{}

void PermissionDataBaseAdapterTest::TearDown(void)
{}

/**
 * @tc.name: PermissionDataBaseAdapterTest_GetInstance_001
 * @tc.desc: GetInstance.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_GetInstance_001, TestSize.Level1)
{
    bool result = true;
    OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_ExistPermissionByOrigin_002
 * @tc.desc: ExistPermissionByOrigin.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_ExistPermissionByOrigin_002, TestSize.Level1)
{
    bool result = true;
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    result = dataBase.ExistPermissionByOrigin("", WebPermissionType::GEOLOCATION);
    EXPECT_FALSE(result);
    result = dataBase.ExistPermissionByOrigin(NO_EXIST_ORIGIN, WebPermissionType::GEOLOCATION);
    EXPECT_FALSE(result);
    result = dataBase.ExistPermissionByOrigin(TEST_ORIGIN, WebPermissionType::NONE_TYPE);
    EXPECT_FALSE(result);
    result = dataBase.ExistPermissionByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION);
    EXPECT_TRUE(result);
    if (g_dataBaseNull) {
        result = g_dataBaseNull->ExistPermissionByOrigin(TEST_ORIGIN, WebPermissionType::NONE_TYPE);
        EXPECT_FALSE(result);
        result = g_dataBaseNull->ExistPermissionByOrigin("", WebPermissionType::NONE_TYPE);
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_GetPermissionResultByOrigin_003
 * @tc.desc: GetPermissionResultByOrigin.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_GetPermissionResultByOrigin_003, TestSize.Level1)
{
    bool result = true;
    bool state = false;
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    result = dataBase.GetPermissionResultByOrigin("", WebPermissionType::GEOLOCATION, state);
    EXPECT_FALSE(result);
    result = dataBase.GetPermissionResultByOrigin(NO_EXIST_ORIGIN, WebPermissionType::GEOLOCATION, state);
    EXPECT_FALSE(result);
    result = dataBase.GetPermissionResultByOrigin(TEST_ORIGIN, WebPermissionType::NONE_TYPE, state);
    EXPECT_FALSE(result);
    result = dataBase.GetPermissionResultByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION, state);
    EXPECT_TRUE(result);
    if (g_dataBaseNull) {
        result = g_dataBaseNull->GetPermissionResultByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION, state);
        EXPECT_FALSE(result);
        result = g_dataBaseNull->GetPermissionResultByOrigin("", WebPermissionType::GEOLOCATION, state);
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_SetPermissionByOrigin_004
 * @tc.desc: SetPermissionByOrigin.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_SetPermissionByOrigin_004, TestSize.Level1)
{
    bool result = true;
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    dataBase.SetPermissionByOrigin("", WebPermissionType::GEOLOCATION, false);
    dataBase.SetPermissionByOrigin(NO_EXIST_ORIGIN, WebPermissionType::GEOLOCATION, false);
    dataBase.SetPermissionByOrigin(TEST_ORIGIN, WebPermissionType::NONE_TYPE, false);
    dataBase.SetPermissionByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION, false);
    if (g_dataBaseNull) {
        g_dataBaseNull->SetPermissionByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION, false);
        g_dataBaseNull->SetPermissionByOrigin("", WebPermissionType::GEOLOCATION, false);
    }
    EXPECT_TRUE(result);
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_ClearPermissionByOrigin_005
 * @tc.desc: ClearPermissionByOrigin.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_ClearPermissionByOrigin_005, TestSize.Level1)
{
    bool result = true;
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    dataBase.ClearPermissionByOrigin("", WebPermissionType::GEOLOCATION);
    dataBase.ClearPermissionByOrigin(NO_EXIST_ORIGIN, WebPermissionType::GEOLOCATION);
    dataBase.ClearPermissionByOrigin(TEST_ORIGIN, WebPermissionType::NONE_TYPE);
    dataBase.ClearPermissionByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION);
    if (g_dataBaseNull) {
        g_dataBaseNull->ClearPermissionByOrigin(TEST_ORIGIN, WebPermissionType::GEOLOCATION);
        g_dataBaseNull->ClearPermissionByOrigin("", WebPermissionType::GEOLOCATION);
    }
    EXPECT_TRUE(result);
    InitPermissionDataBase();
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_ClearAllPermission_006
 * @tc.desc: ClearAllPermission.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_ClearAllPermission_006, TestSize.Level1)
{
    bool result = true;
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    dataBase.ClearAllPermission(WebPermissionType::NONE_TYPE);
    dataBase.ClearAllPermission(WebPermissionType::GEOLOCATION);
    if (g_dataBaseNull) {
        g_dataBaseNull->ClearAllPermission(WebPermissionType::GEOLOCATION);
    }
    EXPECT_TRUE(result);
    InitPermissionDataBase();
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_GetOriginsByPermission_007
 * @tc.desc: GetOriginsByPermission.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_GetOriginsByPermission_007, TestSize.Level1)
{
    bool result = true;
    std::vector<std::string> origins;
    auto& dataBase = OhosWebPermissionDataBaseAdapterImpl::GetInstance();
    dataBase.GetOriginsByPermission(WebPermissionType::NONE_TYPE, origins);
    dataBase.GetOriginsByPermission(WebPermissionType::GEOLOCATION, origins);
    if (g_dataBaseNull) {
        g_dataBaseNull->GetOriginsByPermission(WebPermissionType::GEOLOCATION, origins);
    }
    EXPECT_TRUE(result);
}

/**
 * @tc.name: PermissionDataBaseAdapterTest_CallBack_008
 * @tc.desc: CallBack.
 * @tc.type: FUNC
 * @tc.require: issueI5LW79
 */
HWTEST_F(PermissionDataBaseAdapterTest, PermissionDataBaseAdapterTest_CallBack_008, TestSize.Level1)
{
    bool result = true;
    std::string name = "web_permission_test.db";
    std::string bundleName = "com.example.testapplication1";
    std::string databaseDir = "/data";
    int32_t errorCode = E_OK;
    std::string realPath = SqliteDatabaseUtils::GetDefaultDatabasePath(databaseDir, name, errorCode);
    RdbStoreConfig config("");
    config.SetPath(std::move(realPath));
    config.SetBundleName(bundleName);
    config.SetName(std::move(name));
    config.SetArea(1);

    errorCode = NativeRdb::E_OK;
    PermissionDataBaseRdbOpenCallBack callBack;
    auto rdbStore = RdbHelper::GetRdbStore(config, 1, callBack, errorCode);
    if (rdbStore == nullptr) {
        result = false;
    } else {
        callBack.OnCreate(*(rdbStore.get()));
        callBack.OnUpgrade(*(rdbStore.get()), 1, 1);
    }
    EXPECT_TRUE(result);
}
} // namespace NWeb
} // namespace OHOS