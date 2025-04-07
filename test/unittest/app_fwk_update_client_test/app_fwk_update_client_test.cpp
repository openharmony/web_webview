/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#define private public
#define protected public
#include "app_fwk_update_client.h"
#include "app_fwk_update_service.h"
#include "app_fwk_update_load_callback.h"
#undef protected
#undef private
#include "iservice_registry.h"
#include "system_ability_definition.h"

using namespace testing::ext;

namespace OHOS::NWeb {
class AppFwkUpdateClientTest : public testing::Test {
public:
    AppFwkUpdateClientTest() {}
    virtual ~AppFwkUpdateClientTest() {}

    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override;
    sptr<IRemoteObject> GetAppFwkUpdateRemoteObject(AppFwkUpdateClient &appClient);
};

void AppFwkUpdateClientTest::TearDown()
{
    sptr<ISystemAbilityManager> samgr =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr != nullptr) {
        (void)samgr->UnloadSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
    }
}

sptr<IRemoteObject> AppFwkUpdateClientTest::GetAppFwkUpdateRemoteObject(AppFwkUpdateClient &appClient)
{
    (void)appClient.GetFwkUpdateProxy();
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        return nullptr;
    }
    return systemAbilityMgr->CheckSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
}


/**
 * @tc.name: AppFwkUpdateClientTest_001
 * @tc.desc: invoke app_fwk_update service in client program.
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_001, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    EXPECT_NE(appFwkUpdateClient.appFwkUpdateDiedRecipient_, nullptr);
    auto &clent_1 = AppFwkUpdateClient::GetInstance();
    auto &clent_2 = AppFwkUpdateClient::GetInstance();
    EXPECT_EQ(&client_1, &client_2);
    EXPECT_EQ(client_1.appFwkUpdateDiedRecipient_, client_2.appFwkUpdateDiedRecipient_);

}

/**
 * @tc.name: AppFwkUpdateClientTest_002
 * @tc.desc: LoadSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, loadCallback)
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_002, TestSize.Level0)
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_NE(systemAbilityMgr, nullptr);

    sptr<AppFwkUpdateLoadCallback> loadCallback = new (std::nothrow) AppFwkUpdateLoadCallback();
    EXPECT_NE(loadCallback, nullptr);

    auto ret = systemAbilityMgr->LoadSystemAbility(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, loadCallback);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: AppFwkUpdateClientTest_003
 * @tc.desc: appFwkUpdateClient.LoadFwkService()
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_003, TestSize.Level0)
{
    AppFwkUpdateClient& appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    appFwkUpdateClient.SetFwkUpdate(nullptr);
    EXPECT_EQ(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    bool retLoad = appFwkUpdateClient.LoadFwkService();
    EXPECT_TRUE(retLoad);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: AppFwkUpdateClientTest_004
 * @tc.desc: appFwkUpdateClient.VerifyPackageInstall()
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_004, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    int32_t ret = appFwkUpdateClient.VerifyPackageInstall("com.ohos.nweb", "");
    EXPECT_EQ(ret, -1);
}
/**
 * @tc.name: AppFwkUpdateClientTest_005
 * @tc.desc: appFwkUpdateClient.GetFwkUpdateProxy()
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_005, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    sptr<IRemoteObject> remoteObject = GetAppFwkUpdateRemoteObject(appFwkUpdateClient);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdateProxy(), nullptr);

    sptr<IAppFwkUpdateService> fwkUpdateProxy_check = iface_cast<IAppFwkUpdateService>(remoteObject);
    EXPECT_NE(fwkUpdateProxy_check, nullptr);
}

/**
 * @tc.name: AppFwkUpdateClientTest_006
 * @tc.desc: appFwkUpdateClient.loadSaFinished_
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_006, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    auto aotCompilerProxy = appFwkUpdateClient.GetFwkUpdateProxy();
    EXPECT_TRUE(appFwkUpdateClient.loadSaFinished_);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: AppFwkUpdateClientTest_007
 * @tc.desc: appFwkUpdateClient.AppFwkUpdateOnRemoteDied()
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_007, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    sptr<IRemoteObject> remoteObject = GetAppFwkUpdateRemoteObject(appFwkUpdateClient);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    wptr<IRemoteObject> remoteObject_weak = remoteObject;
    appFwkUpdateClient.AppFwkUpdateOnRemoteDied(remoteObject_weak);
    EXPECT_EQ(appFwkUpdateClient.GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: AppFwkUpdateClientTest_008
 * @tc.desc: callback.OnLoadSystemAbilitySuccess()
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_008, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    AppFwkUpdateLoadCallback callback;
    sptr<IRemoteObject> remoteObject = GetAppFwkUpdateRemoteObject(appFwkUpdateClient);
    appFwkUpdateClient.SetFwkUpdate(nullptr);

    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, nullptr);
    EXPECT_EQ(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID - 1, remoteObject);
    EXPECT_EQ(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID + 1, remoteObject);
    EXPECT_EQ(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, remoteObject);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: AppFwkUpdateClientTest_009
 * @tc.desc: callback.OnLoadSystemAbilityFail()
 * @tc.type: Func
 * @tc.require: 
*/
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClientTest_009, TestSize.Level0)
{
    AppFwkUpdateClient &appFwkUpdateClient = AppFwkUpdateClient::GetInstance();
    AppFwkUpdateLoadCallback callback;
    sptr<IRemoteObject> remoteObject = GetAppFwkUpdateRemoteObject(appFwkUpdateClient);
    appFwkUpdateClient.SetFwkUpdate(remoteObject);

    callback.OnLoadSystemAbilityFail(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID - 1);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilityFail(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID + 1);
    EXPECT_NE(appFwkUpdateClient.GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilityFail(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
    EXPECT_EQ(appFwkUpdateClient.GetFwkUpdate(), nullptr);
}
} // namespace OHOS::ArkCompiler
