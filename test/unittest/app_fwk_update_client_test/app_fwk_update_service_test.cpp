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

#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#define private public
#define protected public
#include "app_fwk_update_client.h"
#include "app_fwk_update_load_callback.h"
#include "app_fwk_update_service.h"
#undef protected
#undef private
#include <thread>

#include "appspawn.h"
#include "bundle_mgr_client.h"
#include "common_event_data.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "ipc_skeleton.h"
#include "nweb_log.h"
#include "system_ability_definition.h"
#include "system_ability_ondemand_reason.h"
#include "want.h"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;

int g_parameter = 0;
int g_setParameter = 0;
int g_intParameter = 0;
int g_boolParameter = 0;
namespace OHOS {
void SetCallingUidValue(int value);

class MockSystemAbilityOnDemandReason : public OHOS::SystemAbilityOnDemandReason {
public:
    MOCK_METHOD(std::string, GetName, (), (const));
    MOCK_METHOD(std::string, GetValue, (), (const));
    MOCK_METHOD(bool, HasExtraData, (), (const));
    MOCK_METHOD(OHOS::OnDemandReasonExtraData, GetExtraData, (), (const));
    MOCK_METHOD(OHOS::OnDemandReasonId, GetReasonId, (), (const));
};

class MockAppSpawnFunctions {
public:
    MOCK_METHOD(int, AppSpawnClientInit, (const char*, AppSpawnClientHandle*));
    MOCK_METHOD(int, AppSpawnReqMsgCreate, (AppSpawnMsgType, const char*, AppSpawnReqMsgHandle*));
    MOCK_METHOD(int, AppSpawnClientSendMsg, (AppSpawnClientHandle, AppSpawnReqMsgHandle, AppSpawnResult*));
    MOCK_METHOD(int, AppSpawnClientDestroy, (AppSpawnClientHandle));
};

static MockAppSpawnFunctions* g_mockAppSpawnFunctions = nullptr;

extern "C" {
int AppSpawnClientInit(const char* name, AppSpawnClientHandle* handle)
{
    return g_mockAppSpawnFunctions->AppSpawnClientInit(name, handle);
}

int AppSpawnReqMsgCreate(AppSpawnMsgType msgType, const char* bundleName, AppSpawnReqMsgHandle* handle)
{
    return g_mockAppSpawnFunctions->AppSpawnReqMsgCreate(msgType, bundleName, handle);
}

int AppSpawnClientSendMsg(AppSpawnClientHandle clientHandle, AppSpawnReqMsgHandle reqHandle, AppSpawnResult* result)
{
    return g_mockAppSpawnFunctions->AppSpawnClientSendMsg(clientHandle, reqHandle, result);
}

int AppSpawnClientDestroy(AppSpawnClientHandle clientHandle)
{
    return g_mockAppSpawnFunctions->AppSpawnClientDestroy(clientHandle);
}
}

class MockCommonEventManager {
public:
    static bool SubscribeCommonEvent(const std::shared_ptr<OHOS::EventFwk::CommonEventSubscriber>& subscriber)
    {
        return GetInstance().subscribeResult_;
    }

    static void setSubscribeResult(bool result)
    {
        GetInstance().subscribeResult_ = result;
    }

private:
    static MockCommonEventManager& GetInstance()
    {
        static MockCommonEventManager instance;
        return instance;
    }

    bool subscribeResult_ = true;
};

class MockCommonEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    MockCommonEventSubscriber() = default;
    ~MockCommonEventSubscriber() = default;
    MOCK_METHOD1(OnReceiveEvent, void(const OHOS::EventFwk::CommonEventData&));
};

namespace system {
bool SetParameter(const std::string& key, const std::string& value)
{
    if (value == "" || g_setParameter == 0) {
        return false;
    }
    return true;
}

std::string GetParameter(const std::string& key, const std::string& def)
{
    if (g_parameter) {
        return "false";
    }
    return "";
}

int GetIntParameter(const std::string& key, int def)
{
    if (g_intParameter) {
        return 1;
    }
    return 0;
}

bool GetBoolParameter(const std::string& key, bool def)
{
    if (g_boolParameter) {
        return true;
    }
    return false;
}
} // namespace system

bool SystemAbility::Publish(sptr<IRemoteObject> systemAbility)
{
    systemAbility.ForceSetRefPtr(nullptr);
    return true;
}

namespace NWeb {
class AppFwkUpdateServiceTest : public testing::Test {
public:
    AppFwkUpdateServiceTest() {}
    virtual ~AppFwkUpdateServiceTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    std::shared_ptr<AppFwkUpdateService> service_;
    std::shared_ptr<MockCommonEventSubscriber> mockSubscriber_;
    std::unique_ptr<MockAppSpawnFunctions> mockAppSpawnFunctions_;
};

void AppFwkUpdateServiceTest::SetUp()
{
    service_ = std::make_shared<AppFwkUpdateService>(0, true);
    mockSubscriber_ = std::make_shared<MockCommonEventSubscriber>();
    mockAppSpawnFunctions_ = std::make_unique<MockAppSpawnFunctions>();
    g_mockAppSpawnFunctions = mockAppSpawnFunctions_.get();
}

void AppFwkUpdateServiceTest::TearDown()
{
    g_mockAppSpawnFunctions = nullptr;
    mockAppSpawnFunctions_.reset();
    mockSubscriber_.reset();
    service_.reset();
    g_parameter = 0;
    g_setParameter = 0;
    g_intParameter = 0;
    g_boolParameter = 0;
    SetCallingUidValue(0);
}

class MockPackageCallback {
public:
    void OnPackageChangedEvent(const std::string& bundleName, const std::string& hapPath)
    {
        lastBundleName_ = bundleName;
        lastHapPath_ = hapPath;
        callCount_++;
    }

    std::string lastBundleName_;
    std::string lastHapPath_;
    int callCount_ = 0;
};

class PackageChangedReceiverTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<PackageChangedReceiver> receiver_;
    std::shared_ptr<MockPackageCallback> callbackMock_;
    std::string testBundleName_ = "com.ohos.nweb";
};

void PackageChangedReceiverTest::SetUpTestCase(void) {}

void PackageChangedReceiverTest::TearDownTestCase(void) {}

void PackageChangedReceiverTest::SetUp()
{
    callbackMock_ = std::make_shared<MockPackageCallback>();

    EventFwk::MatchingSkills skills;
    skills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    EventFwk::CommonEventSubscribeInfo info(skills);

    PackageCommonEventCallback callback;
    callback.OnPackageChangedEvent = [this](const std::string bundleName, const std::string hapPath) {
        callbackMock_->OnPackageChangedEvent(bundleName, hapPath);
    };

    receiver_ = std::make_shared<PackageChangedReceiver>(info, callback);
}

void PackageChangedReceiverTest::TearDown()
{
    receiver_.reset();
    callbackMock_.reset();
}

/**
 * @tc.name: SubscribePackageChangedEvent_001
 * @tc.desc: SubscribePackageChangedEvent()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SubscribePackageChangedEvent_001, testing::ext::TestSize.Level0)
{
    MockCommonEventManager::setSubscribeResult(true);
    service_->SubscribePackageChangedEvent();
    ASSERT_NE(service_->pkgSubscriber_, nullptr);
}

/**
 * @tc.name: SendNWebSpawnMesage_001
 * @tc.desc: SendNWebSpawnMesage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendNWebSpawnMesage_001, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_));

    int result = service_->SendNWebSpawnMesage("com.example.webcore");
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendNWebSpawnMesage_002
 * @tc.desc: SendNWebSpawnMesage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendNWebSpawnMesage_002, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(_, _))
        .WillOnce(Return(-1))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_));

    int result = service_->SendNWebSpawnMesage("com.example.webcore");
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendNWebSpawnMesage_003
 * @tc.desc: SendNWebSpawnMesage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendNWebSpawnMesage_003, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _))
        .WillOnce(Return(-1))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int result = service_->SendNWebSpawnMesage("com.example.webcore");
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendNWebSpawnMesage_004
 * @tc.desc: SendNWebSpawnMesage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendNWebSpawnMesage_004, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(_, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(-1)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int result = service_->SendNWebSpawnMesage("com.example.webcore");
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendNWebSpawnMesage_005
 * @tc.desc: SendNWebSpawnMesage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendNWebSpawnMesage_005, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(_, _)).WillRepeatedly(Return(-1));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(nullptr));

    int result = service_->SendNWebSpawnMesage("com.example.webcore");
    ASSERT_EQ(result, -1);
}

/**
 * @tc.name: SendAppSpawnMessage_001
 * @tc.desc: SendAppSpawnMessage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendAppSpawnMessage_001, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_RESTART_SPAWNER, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int result = service_->SendAppSpawnMessage("com.example.app", MSG_RESTART_SPAWNER);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendAppSpawnMessage_002
 * @tc.desc: SendAppSpawnMessage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendAppSpawnMessage_002, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(Return(-1))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_RESTART_SPAWNER, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int result = service_->SendAppSpawnMessage("com.example.app", MSG_RESTART_SPAWNER);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendAppSpawnMessage_003
 * @tc.desc: SendAppSpawnMessage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendAppSpawnMessage_003, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_RESTART_SPAWNER, _, _))
        .WillOnce(Return(-1))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int result = service_->SendAppSpawnMessage("com.example.app", MSG_RESTART_SPAWNER);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendAppSpawnMessage_004
 * @tc.desc: SendAppSpawnMessage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendAppSpawnMessage_004, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_RESTART_SPAWNER, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(-1)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int result = service_->SendAppSpawnMessage("com.example.app", MSG_RESTART_SPAWNER);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: SendAppSpawnMessage_005
 * @tc.desc: SendAppSpawnMessage()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, SendAppSpawnMessage_005, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _)).WillRepeatedly(Return(-1));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(nullptr)).WillOnce(Return(0));

    int result = service_->SendAppSpawnMessage("com.example.app", MSG_RESTART_SPAWNER);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: VerifyPackageInstall_001
 * @tc.desc: VerifyPackageInstall()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, VerifyPackageInstall_001, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _)).WillRepeatedly(Return(-1));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(nullptr)).WillOnce(Return(0));

    int32_t isSuccess = 0;
    SetCallingUidValue(5523);
    ErrCode errCode = service_->VerifyPackageInstall("com.ohos.arkwebcore", "install_path", isSuccess);
    EXPECT_EQ(errCode, ERR_INVALID_VALUE);
}

/**
 * @tc.name: VerifyPackageInstall_002
 * @tc.desc: VerifyPackageInstall()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, VerifyPackageInstall_002, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_UNLOAD_WEBLIB_IN_APPSPAWN, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));

    int32_t isSuccess = 0;
    SetCallingUidValue(5523);
    g_parameter = 0;
    ErrCode errCode = service_->VerifyPackageInstall("com.ohos.arkwebcore", "", isSuccess);
    EXPECT_EQ(errCode, ERR_OK);
}

/**
 * @tc.name: VerifyPackageInstall_003
 * @tc.desc: VerifyPackageInstall()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, VerifyPackageInstall_003, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_UNLOAD_WEBLIB_IN_APPSPAWN, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));
    int32_t isSuccess = 0;
    SetCallingUidValue(5523);
    g_parameter = 1;
    g_setParameter = 1;
    ErrCode errCode = service_->VerifyPackageInstall("com.ohos.arkwebcore", "", isSuccess);
    EXPECT_EQ(errCode, ERR_INVALID_VALUE);
}

/**
 * @tc.name: VerifyPackageInstall_004
 * @tc.desc: VerifyPackageInstall()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, VerifyPackageInstall_004, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_UNLOAD_WEBLIB_IN_APPSPAWN, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));
    int32_t isSuccess = 0;
    SetCallingUidValue(5523);
    g_parameter = 1;
    g_setParameter = 1;
    ErrCode errCode = service_->VerifyPackageInstall("", "abc", isSuccess);
    EXPECT_EQ(errCode, ERR_INVALID_VALUE);
}

/**
 * @tc.name: VerifyPackageInstall_005
 * @tc.desc: VerifyPackageInstall()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, VerifyPackageInstall_005, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)))
        .WillRepeatedly(Return(-1));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_UNLOAD_WEBLIB_IN_APPSPAWN, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(nullptr)).WillRepeatedly(Return(0));
    int32_t isSuccess = 0;
    SetCallingUidValue(5523);
    g_parameter = 0;
    ErrCode errCode = service_->VerifyPackageInstall("com.ohos.arkwebcore", "arkwebcore.install_path", isSuccess);
    EXPECT_EQ(errCode, ERR_INVALID_VALUE);
}

/**
 * @tc.name: Init_001
 * @tc.desc: Init()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, Init_001, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    bool tmp = service_->Init(*startReason);
    EXPECT_EQ(tmp, false);
}

/**
 * @tc.name: Init_002
 * @tc.desc: Init()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, Init_002, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::INTERFACE_CALL, "", "", 1);
    bool tmp = service_->Init(*startReason);
    EXPECT_EQ(tmp, false);
}

/**
 * @tc.name: Init_003
 * @tc.desc: Init()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, Init_003, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundleName", "com.ohos.arkwebcore" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);
    bool tmp = service_->Init(*startReason);
    EXPECT_EQ(tmp, true);
}

/**
 * @tc.name: Init_004
 * @tc.desc: Init()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, Init_004, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundleName", "com.example.other" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);
    bool tmp = service_->Init(*startReason);
    EXPECT_EQ(tmp, false);
}

/**
 * @tc.name: Init_005
 * @tc.desc: Init()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, Init_005, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundle", "com.example.other" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);
    bool tmp = service_->Init(*startReason);
    EXPECT_EQ(tmp, false);
}

/**
 * @tc.name: OnStart_001
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, OnStart_001, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundleName", "com.ohos.arkwebcore" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);

    service_->registerToService_ = false;
    service_->OnStart(*startReason);
    EXPECT_TRUE(service_->registerToService_);
}

/**
 * @tc.name: OnStart_002
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, OnStart_002, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundleName", "com.ohos.arkwebcore" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);

    service_->registerToService_ = true;
    service_->OnStart(*startReason);
    EXPECT_TRUE(service_->registerToService_);
}

/**
 * @tc.name: OnStart_003
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, OnStart_003, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundleName", "com.example.other" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);

    service_->registerToService_ = false;
    service_->OnStart(*startReason);
    EXPECT_FALSE(service_->registerToService_);
}

/**
 * @tc.name: OnStart_004
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, OnStart_004, testing::ext::TestSize.Level0)
{
    auto startReason = std::make_shared<SystemAbilityOnDemandReason>(OnDemandReasonId::COMMON_EVENT, "", "", 1);
    std::map<std::string, std::string> wantMap = { { "bundleName", "com.example.other" } };
    auto extraData = std::make_shared<OnDemandReasonExtraData>(0, "", wantMap);
    startReason->SetExtraData(*extraData);

    service_->registerToService_ = true;
    service_->OnStart(*startReason);
    EXPECT_TRUE(service_->registerToService_);
}

/**
 * @tc.name: NotifyFWKAfterBmsStart_001
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, NotifyFWKAfterBmsStart_001, testing::ext::TestSize.Level0)
{
    g_parameter = 0;
    EXPECT_EQ(service_->NotifyFWKAfterBmsStart(), ERR_INVALID_VALUE);
}

/**
 * @tc.name: NotifyFWKAfterBmsStart_002
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, NotifyFWKAfterBmsStart_002, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_LOAD_WEBLIB_IN_APPSPAWN, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));
    service_->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, "9527");
    service_->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID - 1, "9527");
    g_parameter = 1;
    g_setParameter = 1;
    EXPECT_EQ(service_->NotifyFWKAfterBmsStart(), ERR_INVALID_VALUE);
}

/**
 * @tc.name: NotifyFWKAfterBmsStart_003
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, NotifyFWKAfterBmsStart_003, testing::ext::TestSize.Level0)
{
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _)).WillRepeatedly(Return(-1));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).Times(0);
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(nullptr)).WillOnce(Return(0));
    g_parameter = 1;
    EXPECT_EQ(service_->NotifyFWKAfterBmsStart(), ERR_INVALID_VALUE);
}

/**
 * @tc.name: NotifyFWKAfterBmsStart_004
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, NotifyFWKAfterBmsStart_004, testing::ext::TestSize.Level0)
{
    AppSpawnClientHandle clientHandle = reinterpret_cast<AppSpawnClientHandle>(1);
    AppSpawnReqMsgHandle reqHandle = reinterpret_cast<AppSpawnReqMsgHandle>(1);

    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientInit(APPSPAWN_SERVER_NAME, _))
        .WillOnce(DoAll(SetArgPointee<1>(clientHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnReqMsgCreate(MSG_LOAD_WEBLIB_IN_APPSPAWN, _, _))
        .WillOnce(DoAll(SetArgPointee<2>(reqHandle), Return(0)));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientSendMsg(_, _, _)).WillOnce(Return(0));
    EXPECT_CALL(*mockAppSpawnFunctions_, AppSpawnClientDestroy(_)).WillOnce(Return(0));
    service_->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, "9527");
    service_->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID - 1, "9527");
    g_parameter = 1;
    g_setParameter = 0;
    EXPECT_EQ(service_->NotifyFWKAfterBmsStart(), ERR_OK);
}

/**
 * @tc.name: NotifyArkWebInstallSuccess_001
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, NotifyArkWebInstallSuccess_001, testing::ext::TestSize.Level0)
{
    g_intParameter = 0;
    g_boolParameter = 0;
    EXPECT_EQ(service_->NotifyArkWebInstallSuccess(), ERR_OK);

    g_intParameter = 1;
    g_boolParameter = 0;
    EXPECT_EQ(service_->NotifyArkWebInstallSuccess(), ERR_OK);
}

/**
 * @tc.name: NotifyArkWebInstallSuccess_002
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, NotifyArkWebInstallSuccess_002, testing::ext::TestSize.Level0)
{
    g_intParameter = 1;
    g_boolParameter = 1;
    g_setParameter = 0;
    EXPECT_EQ(service_->NotifyArkWebInstallSuccess(), ERR_INVALID_VALUE);

    g_setParameter = 1;
    EXPECT_EQ(service_->NotifyArkWebInstallSuccess(), ERR_OK);
}

/**
 * @tc.name: OnStop_001
 * @tc.desc: OnStop()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(AppFwkUpdateServiceTest, OnStop_001, testing::ext::TestSize.Level0)
{
    service_->registerToService_ = true;
    service_->OnStop();
    EXPECT_FALSE(service_->registerToService_);
}

/**
 * @tc.name: OnReceiveEvent_001
 * @tc.desc: OnReceiveEvent()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(PackageChangedReceiverTest, OnReceiveEvent_001, testing::ext::TestSize.Level0)
{
    AAFwk::Want want;
    want.SetAction("");

    EventFwk::CommonEventData eventData(want);

    int initialCallCount = callbackMock_->callCount_;
    receiver_->OnReceiveEvent(eventData);
    EXPECT_EQ(callbackMock_->callCount_, initialCallCount);
}

/**
 * @tc.name: OnReceiveEvent_002
 * @tc.desc: OnReceiveEvent()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(PackageChangedReceiverTest, OnReceiveEvent_002, testing::ext::TestSize.Level0)
{
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    want.SetBundle("com.example.other");

    EventFwk::CommonEventData eventData(want);

    int initialCallCount = callbackMock_->callCount_;
    receiver_->OnReceiveEvent(eventData);
    EXPECT_EQ(callbackMock_->callCount_, initialCallCount);
}

/**
 * @tc.name: OnReceiveEvent_003
 * @tc.desc: OnReceiveEvent()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(PackageChangedReceiverTest, OnReceiveEvent_003, testing::ext::TestSize.Level0)
{
    AAFwk::Want want;
    want.SetAction("OTHER_EVENT");
    want.SetBundle(testBundleName_);

    EventFwk::CommonEventData eventData(want);

    int initialCallCount = callbackMock_->callCount_;
    receiver_->OnReceiveEvent(eventData);
    EXPECT_EQ(callbackMock_->callCount_, initialCallCount);
}

/**
 * @tc.name: OnReceiveEvent_004
 * @tc.desc: OnReceiveEvent()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(PackageChangedReceiverTest, OnReceiveEvent_004, testing::ext::TestSize.Level0)
{
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    want.SetBundle(testBundleName_);

    EventFwk::CommonEventData eventData(want);

    int initialCallCount = callbackMock_->callCount_;
    receiver_->OnReceiveEvent(eventData);
    EXPECT_EQ(callbackMock_->callCount_, initialCallCount);
}

/**
 * @tc.name: OnReceiveEvent_005
 * @tc.desc: OnReceiveEvent()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(PackageChangedReceiverTest, OnReceiveEvent_005, testing::ext::TestSize.Level0)
{
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    want.SetBundle("com.huawei.hmos.arkwebcore");

    EventFwk::CommonEventData eventData(want);

    int initialCallCount = callbackMock_->callCount_;
    receiver_->OnReceiveEvent(eventData);
    EXPECT_GE(callbackMock_->callCount_, initialCallCount);
}
} // namespace NWeb
} // namespace OHOS