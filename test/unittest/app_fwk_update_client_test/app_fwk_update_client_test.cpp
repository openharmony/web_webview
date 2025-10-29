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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#define private public
#define protected public
#include "app_fwk_update_client.h"
#include "app_fwk_update_load_callback.h"
#include "app_fwk_update_service.h"
#include "iapp_fwk_update_service.h"
#include "ipc_skeleton.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#undef protected
#undef privateout

namespace OHOS {
int g_callingUid = 0;

class MockAppFwkUpdateService : public NWeb::IAppFwkUpdateService {
public:
    MOCK_METHOD3(VerifyPackageInstall, ErrCode(const std::string&, const std::string&, int32_t&));
    MOCK_METHOD0(NotifyFWKAfterBmsStart, ErrCode());
    MOCK_METHOD0(NotifyArkWebInstallSuccess, ErrCode());
    MOCK_METHOD0(RefPtrCallback, void());
    MOCK_METHOD1(OnFirstStrongRef, void(const void*));
    MOCK_METHOD1(OnLastStrongRef, void(const void*));
    MOCK_METHOD1(OnLastWeakRef, void(const void*));
    MOCK_METHOD1(OnAttemptPromoted, bool(const void*));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};
static sptr<testing::NiceMock<MockAppFwkUpdateService>> g_mockAppFwkUpdateService = nullptr;

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_descriptor") {}
    virtual ~MockIRemoteObject() = default;
    MOCK_METHOD0(GetObjectRefCount, int32_t());
    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));
    MOCK_CONST_METHOD0(IsProxyObject, bool());
    MOCK_CONST_METHOD0(CheckObjectLegality, bool());
    MOCK_METHOD1(AddDeathRecipient, bool(const sptr<DeathRecipient>&));
    MOCK_METHOD1(RemoveDeathRecipient, bool(const sptr<DeathRecipient>&));
    MOCK_CONST_METHOD1(Marshalling, bool(Parcel&));
    MOCK_METHOD0(AsInterface, sptr<IRemoteBroker>());
    MOCK_METHOD2(Dump, int(int, const std::vector<std::u16string>&));
    MOCK_METHOD0(RefPtrCallback, void());
    MOCK_METHOD1(OnFirstStrongRef, void(const void*));
    MOCK_METHOD1(OnLastStrongRef, void(const void*));
    MOCK_METHOD1(OnLastWeakRef, void(const void*));
    MOCK_METHOD1(OnAttemptPromoted, bool(const void*));
};

class MockSystemAbilityManager : public ISystemAbilityManager {
public:
    MOCK_METHOD1(ListSystemAbilities, std::vector<std::u16string>(unsigned int));
    MOCK_METHOD1(GetSystemAbility, sptr<IRemoteObject>(int32_t));
    MOCK_METHOD1(CheckSystemAbilityOne, sptr<IRemoteObject>(int32_t));
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId)
    {
        return CheckSystemAbilityOne(systemAbilityId);
    }
    MOCK_METHOD1(RemoveSystemAbility, int32_t(int32_t));
    MOCK_METHOD2(SubscribeSystemAbility, int32_t(int32_t, const sptr<ISystemAbilityStatusChange>&));
    MOCK_METHOD2(UnSubscribeSystemAbility, int32_t(int32_t, const sptr<ISystemAbilityStatusChange>&));
    MOCK_METHOD2(GetSystemAbility, sptr<IRemoteObject>(int32_t, const std::string&));
    MOCK_METHOD2(CheckSystemAbility, sptr<IRemoteObject>(int32_t, const std::string&));
    MOCK_METHOD2(AddOnDemandSystemAbilityInfo, int32_t(int32_t, const std::u16string&));
    MOCK_METHOD2(CheckSystemAbility, sptr<IRemoteObject>(int32_t, bool&));
    MOCK_METHOD3(AddSystemAbility, int32_t(int32_t, const sptr<IRemoteObject>&, const SAExtraProp&));
    MOCK_METHOD2(AddSystemProcess, int32_t(const std::u16string&, const sptr<IRemoteObject>&));
    MOCK_METHOD2(LoadSystemAbility, sptr<IRemoteObject>(int32_t, int32_t));
    MOCK_METHOD2(LoadSystemAbilityTwo, int32_t(int32_t, const sptr<ISystemAbilityLoadCallback>&));
    MOCK_METHOD3(LoadSystemAbilityThree, int32_t(int32_t, const std::string&, const sptr<ISystemAbilityLoadCallback>&));
    int32_t LoadSystemAbility(int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback>& callback)
    {
        return LoadSystemAbilityTwo(systemAbilityId, callback);
    }
    int32_t LoadSystemAbility(
        int32_t systemAbilityId, const std::string& deviceId, const sptr<ISystemAbilityLoadCallback>& callback)
    {
        return LoadSystemAbilityThree(systemAbilityId, deviceId, callback);
    }
    MOCK_METHOD1(UnloadSystemAbility, int32_t(int32_t));
    MOCK_METHOD1(CancelUnloadSystemAbility, int32_t(int32_t));
    MOCK_METHOD0(UnloadAllIdleSystemAbility, int32_t());
    MOCK_METHOD1(UnloadProcess, int32_t(const std::vector<std::u16string>&));
    MOCK_METHOD1(GetLruIdleSystemAbilityProc, int32_t(std::vector<IdleProcessInfo>&));
    MOCK_METHOD2(GetSystemProcessInfo, int32_t(int32_t, SystemProcessInfo&));
    MOCK_METHOD1(GetRunningSystemProcess, int32_t(std::list<SystemProcessInfo>&));
    MOCK_METHOD1(SubscribeSystemProcess, int32_t(const sptr<ISystemProcessStatusChange>&));
    MOCK_METHOD4(SendStrategy, int32_t(int32_t, std::vector<int32_t>&, int32_t, std::string&));
    MOCK_METHOD1(UnSubscribeSystemProcess, int32_t(const sptr<ISystemProcessStatusChange>&));
    MOCK_METHOD2(GetExtensionSaIds, int32_t(const std::string&, std::vector<int32_t>&));
    MOCK_METHOD2(GetExtensionRunningSaList, int32_t(const std::string&, std::vector<sptr<IRemoteObject>>&));
    MOCK_METHOD1(GetLocalAbilityManagerProxy, sptr<IRemoteObject>(int32_t));
    MOCK_METHOD2(GetRunningSaExtensionInfoList, int32_t(const std::string&, std::vector<SaExtensionInfo>&));
    MOCK_METHOD3(GetCommonEventExtraDataIdlist, int32_t(int32_t, std::vector<int64_t>&, const std::string&));
    MOCK_METHOD2(GetOnDemandReasonExtraData, int32_t(int64_t, MessageParcel&));
    MOCK_METHOD3(GetOnDemandPolicy, int32_t(int32_t, OnDemandPolicyType, std::vector<SystemAbilityOnDemandEvent>&));
    MOCK_METHOD3(
        UpdateOnDemandPolicy, int32_t(int32_t, OnDemandPolicyType, const std::vector<SystemAbilityOnDemandEvent>&));
    MOCK_METHOD1(GetOnDemandSystemAbilityIds, int32_t(std::vector<int32_t>&));
    MOCK_METHOD0(AsObject, sptr<IRemoteObject>());
};

sptr<ISystemAbilityManager> g_mockSystemAbilityManager = nullptr;
SystemAbilityManagerClient& SystemAbilityManagerClient::GetInstance()
{
    static SystemAbilityManagerClient instance;
    return instance;
}
sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    return g_mockSystemAbilityManager;
}
int IPCSkeleton::GetCallingUid()
{
    return g_callingUid;
}
void SetCallingUidValue(int value)
{
    g_callingUid = value;
}

template<>
sptr<NWeb::IAppFwkUpdateService> iface_cast<NWeb::IAppFwkUpdateService>(const sptr<IRemoteObject>& object)
{
    if (object) {
        return sptr<NWeb::IAppFwkUpdateService>(g_mockAppFwkUpdateService.GetRefPtr());
    }
    return nullptr;
}

namespace NWeb {
class AppFwkUpdateClientTest : public testing::Test {
public:
    AppFwkUpdateClientTest() {}
    virtual ~AppFwkUpdateClientTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override;
    void TearDown() override;
    AppFwkUpdateClient* appFwkUpdateClient_ = nullptr;
    AppFwkUpdateClient::AppFwkUpdateDiedRecipient* appFwkUpdateDiedRecipient_ = nullptr;
    sptr<testing::NiceMock<MockIRemoteObject>> mockRemote_ = nullptr;
    sptr<testing::NiceMock<MockIRemoteObject>> mockRemote1_ = nullptr;
    sptr<MockSystemAbilityManager> mockSystemAbilityManager_;
};

void AppFwkUpdateClientTest::SetUp()
{
    appFwkUpdateClient_ = new AppFwkUpdateClient();
    appFwkUpdateDiedRecipient_ = new AppFwkUpdateClient::AppFwkUpdateDiedRecipient();
    mockRemote_ = new testing::NiceMock<MockIRemoteObject>();
    mockRemote1_ = new testing::NiceMock<MockIRemoteObject>();
    g_mockAppFwkUpdateService = new testing::NiceMock<MockAppFwkUpdateService>();
    mockSystemAbilityManager_ = new testing::NiceMock<MockSystemAbilityManager>();
    g_mockSystemAbilityManager = mockSystemAbilityManager_;
}

void AppFwkUpdateClientTest::TearDown()
{
    appFwkUpdateClient_ = nullptr;
    appFwkUpdateDiedRecipient_ = nullptr;
    mockRemote_ = nullptr;
    mockRemote1_ = nullptr;
    g_mockAppFwkUpdateService = nullptr;
    g_mockSystemAbilityManager = nullptr;
    mockSystemAbilityManager_ = nullptr;
    g_callingUid = 0;
}

/**
 * @tc.name: AppFwkUpdateClient_001
 * @tc.desc: AppFwkUpdateClient()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateClient_001, testing::ext::TestSize.Level0)
{
    EXPECT_NE(appFwkUpdateClient_->appFwkUpdateDiedRecipient_, nullptr);
}

/**
 * @tc.name: SetFwkUpdate_001
 * @tc.desc: SetFwkUpdate(const sptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, SetFwkUpdate_001, testing::ext::TestSize.Level0)
{
    appFwkUpdateClient_->SetFwkUpdate(nullptr);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: SetFwkUpdate_002
 * @tc.desc: SetFwkUpdate(const sptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, SetFwkUpdate_002, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->SetFwkUpdate(remoteObject);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: GetFwkUpdateProxy_001
 * @tc.desc: GetFwkUpdateProxy()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, GetFwkUpdateProxy_001, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->SetFwkUpdate(remoteObject);

    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
    sptr<IAppFwkUpdateService> fwkUpdateProxy = appFwkUpdateClient_->GetFwkUpdateProxy();
    EXPECT_EQ(fwkUpdateProxy, appFwkUpdateClient_->GetFwkUpdate());
}

/**
 * @tc.name: GetFwkUpdateProxy_002
 * @tc.desc: GetFwkUpdateProxy()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, GetFwkUpdateProxy_002, testing::ext::TestSize.Level0)
{
    g_mockSystemAbilityManager = nullptr;
    appFwkUpdateClient_->SetFwkUpdate(nullptr);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
    sptr<IAppFwkUpdateService> fwkUpdateProxy = appFwkUpdateClient_->GetFwkUpdateProxy();
    EXPECT_EQ(fwkUpdateProxy, nullptr);
}

/**
 * @tc.name: GetFwkUpdateProxy_003
 * @tc.desc: GetFwkUpdateProxy()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, GetFwkUpdateProxy_003, testing::ext::TestSize.Level0)
{
    appFwkUpdateClient_->SetFwkUpdate(nullptr);

    EXPECT_CALL(*mockSystemAbilityManager_, CheckSystemAbilityOne(testing::_)).WillOnce(testing::Return(mockRemote_));
    sptr<IAppFwkUpdateService> fwkUpdateProxy = appFwkUpdateClient_->GetFwkUpdateProxy();
    EXPECT_NE(fwkUpdateProxy, nullptr);
}

/**
 * @tc.name: GetFwkUpdateProxy_004
 * @tc.desc: GetFwkUpdateProxy()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, GetFwkUpdateProxy_004, testing::ext::TestSize.Level0)
{
    appFwkUpdateClient_->SetFwkUpdate(nullptr);
    EXPECT_CALL(*mockSystemAbilityManager_, CheckSystemAbilityOne(testing::_)).WillOnce(testing::Return(nullptr));
    EXPECT_CALL(*mockSystemAbilityManager_, LoadSystemAbilityTwo(testing::_, testing::_)).WillOnce(testing::Return(-1));
    sptr<IAppFwkUpdateService> fwkUpdateProxy = appFwkUpdateClient_->GetFwkUpdateProxy();
    EXPECT_EQ(fwkUpdateProxy, nullptr);
}

/**
 * @tc.name: GetFwkUpdateProxy_005
 * @tc.desc: GetFwkUpdateProxy()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, GetFwkUpdateProxy_005, testing::ext::TestSize.Level0)
{
    appFwkUpdateClient_->SetFwkUpdate(nullptr);
    EXPECT_CALL(*mockSystemAbilityManager_, CheckSystemAbilityOne(testing::_))
        .WillOnce(testing::Return(nullptr))
        .WillOnce(testing::Return(mockRemote_));
    EXPECT_CALL(*mockSystemAbilityManager_, LoadSystemAbilityTwo(testing::_, testing::_)).WillOnce(testing::Return(0));
    sptr<IAppFwkUpdateService> fwkUpdateProxy = appFwkUpdateClient_->GetFwkUpdateProxy();
    EXPECT_NE(fwkUpdateProxy, nullptr);
}

/**
 * @tc.name: VerifyPackageInstall_001
 * @tc.desc: VerifyPackageInstall(const std::string& bundleName, const std::string& hapPath)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, VerifyPackageInstall_001, testing::ext::TestSize.Level0)
{
    int result = appFwkUpdateClient_->VerifyPackageInstall("com.invalid.bundle", "/path/to/hap");
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: VerifyPackageInstall_002
 * @tc.desc: VerifyPackageInstall(const std::string& bundleName, const std::string& hapPath)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, VerifyPackageInstall_002, testing::ext::TestSize.Level0)
{
    g_callingUid = 5523;
    int result = appFwkUpdateClient_->VerifyPackageInstall("com.invalid.bundle", "/path/to/hap");
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name: VerifyPackageInstall_003
 * @tc.desc: VerifyPackageInstall(const std::string& bundleName, const std::string& hapPath)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, VerifyPackageInstall_003, testing::ext::TestSize.Level0)
{
    g_callingUid = 5523;
    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->SetFwkUpdate(remoteObject);
    int result = appFwkUpdateClient_->VerifyPackageInstall("com.ohos.nweb", "/path/to/hap");
    EXPECT_EQ(result, -1);
}

/**
 * @tc.name: LoadFwkService_001
 * @tc.desc: LoadFwkService()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, LoadFwkService_001, testing::ext::TestSize.Level0)
{
    g_mockSystemAbilityManager = nullptr;
    bool result = appFwkUpdateClient_->LoadFwkService();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: LoadFwkService_002
 * @tc.desc: LoadFwkService()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, LoadFwkService_002, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockSystemAbilityManager_, LoadSystemAbilityTwo(testing::_, testing::_)).WillOnce(testing::Return(-1));
    bool result = appFwkUpdateClient_->LoadFwkService();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: LoadFwkService_003
 * @tc.desc: LoadFwkService()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, LoadFwkService_003, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockSystemAbilityManager_, LoadSystemAbilityTwo(testing::_, testing::_)).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockSystemAbilityManager_, CheckSystemAbilityOne(testing::_)).WillOnce(testing::Return(nullptr));
    bool result = appFwkUpdateClient_->LoadFwkService();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: LoadFwkService_004
 * @tc.desc: LoadFwkService()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, LoadFwkService_004, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockSystemAbilityManager_, LoadSystemAbilityTwo(testing::_, testing::_)).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockSystemAbilityManager_, CheckSystemAbilityOne(testing::_)).WillOnce(testing::Return(mockRemote_));
    bool result = appFwkUpdateClient_->LoadFwkService();
    EXPECT_TRUE(result);
}

/**
 * @tc.name: OnLoadSystemAbilitySuccess_001
 * @tc.desc: OnLoadSystemAbilitySuccess(const sptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, OnLoadSystemAbilitySuccess_001, testing::ext::TestSize.Level0)
{
    appFwkUpdateClient_->appFwkUpdateDiedRecipient_ = nullptr;
    appFwkUpdateClient_->OnLoadSystemAbilitySuccess(nullptr);

    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->appFwkUpdateDiedRecipient_ =
        new (std::nothrow) AppFwkUpdateClient::AppFwkUpdateDiedRecipient();
    testing::Mock::AllowLeak(mockRemote_);
    EXPECT_CALL(*mockRemote_, AddDeathRecipient(testing::_))
        .WillOnce(testing::Return(false))
        .WillOnce(testing::Return(true));

    appFwkUpdateClient_->OnLoadSystemAbilitySuccess(remoteObject);
    appFwkUpdateClient_->OnLoadSystemAbilitySuccess(remoteObject);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: AppFwkUpdateOnRemoteDied_001
 * @tc.desc: AppFwkUpdateOnRemoteDied(const wptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateOnRemoteDied_001, testing::ext::TestSize.Level0)
{
    appFwkUpdateDiedRecipient_->OnRemoteDied(nullptr);

    appFwkUpdateClient_->SetFwkUpdate(nullptr);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
    appFwkUpdateClient_->AppFwkUpdateOnRemoteDied(nullptr);
}

/**
 * @tc.name: AppFwkUpdateOnRemoteDied_002
 * @tc.desc: AppFwkUpdateOnRemoteDied(const wptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateOnRemoteDied_002, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->SetFwkUpdate(remoteObject);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
    appFwkUpdateClient_->AppFwkUpdateOnRemoteDied(nullptr);
}

/**
 * @tc.name: AppFwkUpdateOnRemoteDied_003
 * @tc.desc: AppFwkUpdateOnRemoteDied(const wptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateOnRemoteDied_003, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = mockRemote_;
    wptr<IRemoteObject> weakRemoteObject(remoteObject);
    appFwkUpdateDiedRecipient_->OnRemoteDied(weakRemoteObject);

    appFwkUpdateClient_->SetFwkUpdate(remoteObject);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
    appFwkUpdateClient_->AppFwkUpdateOnRemoteDied(weakRemoteObject);
}

/**
 * @tc.name: AppFwkUpdateOnRemoteDied_004
 * @tc.desc: AppFwkUpdateOnRemoteDied(const wptr<IRemoteObject>& remoteObject)
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, AppFwkUpdateOnRemoteDied_004, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->SetFwkUpdate(remoteObject);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
    testing::Mock::AllowLeak(g_mockAppFwkUpdateService);
    EXPECT_CALL(*g_mockAppFwkUpdateService, AsObject()).WillOnce(testing::Return(remoteObject));
    wptr<IRemoteObject> weakRemoteObject(remoteObject);
    appFwkUpdateClient_->AppFwkUpdateOnRemoteDied(weakRemoteObject);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: CallbackOnLoadSystemAbilitySuccess_001
 * @tc.desc: callback.OnLoadSystemAbilitySuccess()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, CallbackOnLoadSystemAbilitySuccess_001, testing::ext::TestSize.Level0)
{
    AppFwkUpdateLoadCallback callback;
    sptr<IRemoteObject> remoteObject = mockRemote_;
    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID, nullptr);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID - 1, remoteObject);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilitySuccess(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID + 1, remoteObject);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: CallbackOnLoadSystemAbilityFail_001
 * @tc.desc: callback.OnLoadSystemAbilityFail()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, CallbackOnLoadSystemAbilityFail_001, testing::ext::TestSize.Level0)
{
    AppFwkUpdateLoadCallback callback;
    sptr<IRemoteObject> remoteObject = mockRemote_;
    appFwkUpdateClient_->SetFwkUpdate(remoteObject);

    callback.OnLoadSystemAbilityFail(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID - 1);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);

    callback.OnLoadSystemAbilityFail(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID + 1);
    EXPECT_NE(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}

/**
 * @tc.name: CallbackOnLoadSystemAbilityFail_002
 * @tc.desc: callback.OnLoadSystemAbilityFail()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateClientTest, CallbackOnLoadSystemAbilityFail_002, testing::ext::TestSize.Level0)
{
    AppFwkUpdateLoadCallback callback;
    callback.OnLoadSystemAbilityFail(SUBSYS_WEBVIEW_SYS_UPDATE_SERVICE_ID);
    EXPECT_EQ(appFwkUpdateClient_->GetFwkUpdate(), nullptr);
}
} // namespace NWeb
} // namespace OHOS
