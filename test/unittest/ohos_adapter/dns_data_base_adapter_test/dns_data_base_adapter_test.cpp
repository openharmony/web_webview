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

#include "foundation/ability/ability_runtime/interfaces/kits/native/appkit/ability_runtime/context/application_context.h"
#include "ohos_adapter_helper.h"
#include "sqlite_database_utils.h"

#define private public
#include "ohos_web_dns_data_base_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AbilityRuntime;
using namespace OHOS::NativeRdb;

namespace OHOS {
namespace {
const std::string WEB_DNS_DATABASE_FILE = "web_dns.db";
const std::string WEB_PATH = "/web";
const int32_t TIME_SECONDS_THREE = 3;
const int32_t DATABASE_DIR_PERMISSION = 0700;
std::shared_ptr<NWeb::OhosWebDnsDataBaseAdapter> g_dataBaseNull = nullptr;
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
} // namespace

namespace AbilityRuntime {
std::shared_ptr<ApplicationContext> Context::GetApplicationContext()
{
    return g_applicationContext;
}
} // namespace AbilityRuntime

namespace NWeb {
class DnsDataBaseAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static void InitDnsDataBase(void);
};

class ApplicationContextMock : public ApplicationContext {
public:
    MOCK_CONST_METHOD0(GetBundleName, std::string());
    MOCK_METHOD0(GetCacheDir, std::string());
};

void DnsDataBaseAdapterTest::InitDnsDataBase(void)
{
    OhosWebDnsDataBaseAdapterImpl::GetInstance();
}

void DnsDataBaseAdapterTest::SetUpTestCase(void)
{
    bool result = true;
    g_dataBaseNull.reset(new OhosWebDnsDataBaseAdapterImpl());
    if (g_dataBaseNull == nullptr) {
        result = false;
    }
    EXPECT_TRUE(result);

    InitDnsDataBase();

    ApplicationContextMock *contextMock = new ApplicationContextMock();
    ASSERT_NE(contextMock, nullptr);
    EXPECT_CALL(*contextMock, GetBundleName())
        .Times(1)
        .WillRepeatedly(::testing::Return("com.example.testapplication"));
    EXPECT_CALL(*contextMock, GetCacheDir())
        .Times(TIME_SECONDS_THREE)
        .WillRepeatedly(::testing::Return("/data"));
    g_applicationContext.reset(contextMock);
    InitDnsDataBase();

    std::string databaseDir = g_applicationContext->GetCacheDir() + WEB_PATH;
    if (access(databaseDir.c_str(), F_OK) != 0) {
        result = mkdir(databaseDir.c_str(), DATABASE_DIR_PERMISSION) == 0;
        EXPECT_TRUE(result);
    }
    InitDnsDataBase();
}

void DnsDataBaseAdapterTest::TearDownTestCase(void)
{
    system("rm -rf /data/web");
    g_applicationContext.reset();
}

void DnsDataBaseAdapterTest::SetUp(void)
{}

void DnsDataBaseAdapterTest::TearDown(void)
{}

/**
 * @tc.name: DnsDataBaseAdapterTest_GetInstance_001
 * @tc.desc: GetInstance.
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(DnsDataBaseAdapterTest, DnsDataBaseAdapterTest_GetInstance_001, TestSize.Level1)
{
    OhosWebDnsDataBaseAdapterImpl::GetInstance();
}

/**
 * @tc.name: DnsDataBaseAdapterTest_ExistHostname_002
 * @tc.desc: ExistHostname.
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(DnsDataBaseAdapterTest, DnsDataBaseAdapterTest_ExistHostname_002, TestSize.Level1)
{
    bool result = true;
    auto& dataBase = OhosWebDnsDataBaseAdapterImpl::GetInstance();
    result = dataBase.ExistHostname("");
    EXPECT_FALSE(result);

    result = dataBase.ExistHostname("hostname1");
    EXPECT_FALSE(result);

    dataBase.InsertHostname("hostname0");
    result = dataBase.ExistHostname("hostname1");
    EXPECT_FALSE(result);
    result = dataBase.ExistHostname("hostname0");
    EXPECT_TRUE(result);

    if (g_dataBaseNull) {
        result = g_dataBaseNull->ExistHostname("hostname0");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: DnsDataBaseAdapterTest_InsertHostname_003
 * @tc.desc: InsertHostname.
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(DnsDataBaseAdapterTest, DnsDataBaseAdapterTest_InsertHostname_003, TestSize.Level1)
{
    bool result = true;
    auto& dataBase = OhosWebDnsDataBaseAdapterImpl::GetInstance();
    dataBase.InsertHostname("");
    result = dataBase.ExistHostname("");
    EXPECT_FALSE(result);

    dataBase.InsertHostname("hostname2");
    result = dataBase.ExistHostname("hostname2");
    EXPECT_TRUE(result);

    if (g_dataBaseNull) {
        g_dataBaseNull->InsertHostname("hostname1");
        result = g_dataBaseNull->ExistHostname("hostname1");
        EXPECT_FALSE(result);
    }
}

/**
 * @tc.name: DnsDataBaseAdapterTest_GetHostnames_004
 * @tc.desc: GetHostnames.
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(DnsDataBaseAdapterTest, DnsDataBaseAdapterTest_GetHostnames_004, TestSize.Level1)
{
    std::vector<std::string> hostInfo;
    auto& dataBase = OhosWebDnsDataBaseAdapterImpl::GetInstance();
    dataBase.ClearAllHostname();
    dataBase.GetHostnames(hostInfo);
    EXPECT_TRUE(hostInfo.empty());
    hostInfo.clear();

    dataBase.InsertHostname("hostname0");
    dataBase.GetHostnames(hostInfo);
    EXPECT_FALSE(hostInfo.empty());
    hostInfo.clear();

    if (g_dataBaseNull) {
        g_dataBaseNull->GetHostnames(hostInfo);
        EXPECT_TRUE(hostInfo.empty());
    }
}

/**
 * @tc.name: DnsDataBaseAdapterTest_ClearAllHostname_005
 * @tc.desc: ClearAllHostname.
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(DnsDataBaseAdapterTest, DnsDataBaseAdapterTest_ClearAllHostname_005, TestSize.Level1)
{
    std::vector<std::string> hostInfo;
    auto& dataBase = OhosWebDnsDataBaseAdapterImpl::GetInstance();
    dataBase.InsertHostname("hostname0");
    dataBase.GetHostnames(hostInfo);
    EXPECT_FALSE(hostInfo.empty());
    hostInfo.clear();

    dataBase.ClearAllHostname();
    dataBase.GetHostnames(hostInfo);
    EXPECT_TRUE(hostInfo.empty());

    if (g_dataBaseNull) {
        g_dataBaseNull->ClearAllHostname();
    }
}

/**
 * @tc.name: DnsDataBaseAdapterTest_CallBack_006
 * @tc.desc: CallBack.
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(DnsDataBaseAdapterTest, DnsDataBaseAdapterTest_CallBack_006, TestSize.Level1)
{
    bool result = true;
    std::string name = WEB_DNS_DATABASE_FILE;
    std::string bundleName = "com.example.testapplication1";
    std::string databaseDir = "/data/web";
    int32_t errorCode = E_OK;
    std::string realPath = SqliteDatabaseUtils::GetDefaultDatabasePath(databaseDir, name, errorCode);
    RdbStoreConfig config("");
    config.SetPath(std::move(realPath));
    config.SetBundleName(bundleName);
    config.SetName(std::move(name));
    config.SetArea(1);

    errorCode = NativeRdb::E_OK;
    DnsDataBaseRdbOpenCallBack callBack;
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