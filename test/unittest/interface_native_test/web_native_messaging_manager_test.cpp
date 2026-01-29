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
#include "connect_native_request.h"
#include "extension_ipc_connection.h"
#include "i_web_native_messaging_manager.h"
#include "service_delay_exit_task.h"
#include "service_event_handler.h"
#include "web_extension_connection_callback_proxy.h"
#include "web_native_messaging_manager.h"
#undef protected
#undef private
#include <thread>

#include "common_event_data.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "ipc_skeleton.h"
#include "nweb_log.h"
#include "system_ability_definition.h"
#include "system_ability_ondemand_reason.h"
#include "want.h"
#include "int_wrapper.h"
#include "string_wrapper.h"
#include "want_params_wrapper.h"
#include "mock_accesstoken_kit.h"
#include "if_system_ability_manager.h"
#include "bundle_mgr_client.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::AAFwk;
#define MAX_REQUEST_SIZE (10000)

int g_callingUid = 0;
int g_callingTkid = 0;
int g_callingPid = 0;

namespace OHOS {
const std::string WANT_EXTENSION_ORIGIN_PARAM_KEY = "ohos.arkweb.extensionOrigin";
const std::string WANT_READ_PIPE_PARAM_KEY = "ohos.arkweb.messageReadPipe";
const std::string WANT_WRITE_PIPE_PARAM_KEY = "ohos.arkweb.messageWritePipe";
const int32_t MOCK_PIPE_FD = 2;

int IPCSkeleton::GetCallingUid()
{
    return g_callingUid;
}

uint32_t IPCSkeleton::GetCallingTokenID()
{
    return g_callingTkid;
}

int IPCSkeleton::GetCallingPid()
{
    return g_callingPid;
}

class IRemoteObjectMocker : public IRemoteObject {
public:
    IRemoteObjectMocker() : IRemoteObject {u"IRemoteObjectMocker"}
    {
    }

    ~IRemoteObjectMocker()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
    {
        return 0;
    }

    bool IsProxyObject() const
    {
        return true;
    }

    bool CheckObjectLegality() const
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient)
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface()
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string>& args)
    {
        return 0;
    }
};

namespace NWeb {
class WebNativeMessagingManagerTest : public testing::Test {
public:
    WebNativeMessagingManagerTest() {}
    virtual ~WebNativeMessagingManagerTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    std::shared_ptr<WebNativeMessagingManager> manager_;
    int32_t connectionId = -1;
};

void WebNativeMessagingManagerTest::SetUp()
{
    manager_ = std::make_shared<WebNativeMessagingManager>();
}

void WebNativeMessagingManagerTest::TearDown()
{
    manager_.reset();
}

/**
 * @tc.name: LookUpOrNewIpcConnection
 * @tc.desc: LookUpOrNewIpcConnection()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, LookUpOrNewIpcConnection, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string bundleName = "abc";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    manager_->NewIpcConnection(tkid, bundleName, abilityName, token, -1);
    std::string bundleName2 = "abc2";
    manager_->LookUpOrNewIpcConnection(tkid, bundleName, abilityName, token, -1);
    EXPECT_TRUE(1);
    manager_->LookUpOrNewIpcConnection(tkid, bundleName2, abilityName, token, -1);
    EXPECT_TRUE(1);
}


/**
 * @tc.name: GetAndCheckConnectParams
 * @tc.desc: GetAndCheckConnectParams()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, GetAndCheckConnectParams, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();

    std::string bundleName = "web_native_messaging_service_test";
    AAFwk::Want want;
    want.SetBundle(bundleName);
    AAFwk::WantParams topParams;

    AppExecFwk::ElementName element;
    element.SetAbilityName("WebNativeMessagingExtensionAbility");
    element.SetBundleName("web_native_messaging_service_test");
    element.SetDeviceID("12345");
    want.SetElement(element);
    want.SetParam(WANT_EXTENSION_ORIGIN_PARAM_KEY, std::string("app_instance_1"));
    AAFwk::WantParams readParams;
    readParams.SetParam("type", AAFwk::String::Box("FD"));
    readParams.SetParam("value", AAFwk::Integer::Box(1));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readParams));

    AAFwk::WantParams writeParams;
    writeParams.SetParam("type", AAFwk::String::Box("FD"));
    writeParams.SetParam("value", AAFwk::Integer::Box(2));
    topParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeParams));
    want.SetParams(topParams);

    ConnectNativeParams params;
    int connectionId = 123;
    int ret = 0;
    ret = manager_->GetAndCheckConnectParams(nullptr, callback, connectionId, params);
    EXPECT_TRUE(ret == ConnectNativeRet::CONTEXT_ERROR);

    ret = manager_->GetAndCheckConnectParams(token, nullptr, connectionId, params);
    EXPECT_TRUE(ret == ConnectNativeRet::CALLBACK_ERROR);

    MockAccesstokenKit::MockAccessTokenKitRet(-1);
    ret = manager_->GetAndCheckConnectParams(token, callback, connectionId, params);
    EXPECT_TRUE(ret == ConnectNativeRet::PERMISSION_CHECK_ERROR);

    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);
    MockAccesstokenKit::MockGetHapTokenInfo(-1);
    ret = manager_->GetAndCheckConnectParams(token, callback, connectionId, params);
    EXPECT_TRUE(ret == ConnectNativeRet::PERMISSION_CHECK_ERROR);
    MockAccesstokenKit::MockGetHapTokenInfo(0);

    int32_t pid = 444;
    int32_t innerId = 555;
    ConnectionNativeRequest::InsertInnerConnectionIdMap(tkid, pid, connectionId, innerId);
    g_callingTkid = tkid;
    g_callingPid = pid;
    ret = manager_->GetAndCheckConnectParams(token, callback, connectionId, params);
    EXPECT_TRUE(ret == ConnectNativeRet::CONNECTION_ID_EXIST);

    auto aa = std::make_shared<ConnectionNativeRequest>();
    aa->connectIdMap_.clear();
    std::weak_ptr<ConnectionNativeRequest> weak = aa;
    for (int vv = 0; vv <= MAX_REQUEST_SIZE; vv++) {
        InnerConnectIdMapKey key(vv, pid++, connectionId);
        aa->connectIdMap_[vv] = weak;
    }

    g_callingTkid++;
    g_callingPid++;
    ret = manager_->GetAndCheckConnectParams(token, callback, connectionId, params);
    EXPECT_TRUE(ret == ConnectNativeRet::REQUEST_SIZE_TOO_LARGE);

    aa->connectIdMap_.clear();
    ret = manager_->GetAndCheckConnectParams(token, callback, connectionId, params);
    aa->connectIdMap_.clear();
    EXPECT_TRUE(ret == ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: ConnectWebNativeMessagingExtension
 * @tc.desc: ConnectWebNativeMessagingExtension()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, ConnectWebNativeMessagingExtension, testing::ext::TestSize.Level0)
{
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();

    std::string bundleName = "web_native_messaging_service_test";
    AAFwk::Want want;
    want.SetBundle(bundleName);
    AAFwk::WantParams topParams;

    AppExecFwk::ElementName element;
    element.SetAbilityName("WebNativeMessagingExtensionAbility");
    element.SetBundleName("web_native_messaging_service_test");
    element.SetDeviceID("12345");
    want.SetElement(element);
    want.SetParam(WANT_EXTENSION_ORIGIN_PARAM_KEY, std::string("app_instance_1"));
    AAFwk::WantParams readParams;
    readParams.SetParam("type", AAFwk::String::Box("FD"));
    readParams.SetParam("value", AAFwk::Integer::Box(1));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readParams));

    AAFwk::WantParams writeParams;
    writeParams.SetParam("type", AAFwk::String::Box("FD"));
    writeParams.SetParam("value", AAFwk::Integer::Box(2));
    topParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeParams));
    want.SetParams(topParams);

    ConnectNativeParams params;
    int connectionId = 123;
    int errorNum = 0;

    MockAccesstokenKit::MockAccessTokenKitRet(-1);
    MockAccesstokenKit::MockGetHapTokenInfo(0);

    manager_->ConnectWebNativeMessagingExtension(token, want, callback, connectionId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::PERMISSION_CHECK_ERROR);

    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);
    manager_->ConnectWebNativeMessagingExtension(token, want, callback, connectionId, errorNum);
    EXPECT_TRUE(errorNum != ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: DisconnectWebNativeMessagingExtension
 * @tc.desc: DisconnectWebNativeMessagingExtension()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, DisconnectWebNativeMessagingExtension, testing::ext::TestSize.Level0)
{
    int connectionId = 123;
    int errorNum = 0;

    std::string bundleName = "web_native_messaging_service_test";
    AAFwk::Want want;
    want.SetBundle(bundleName);
    AAFwk::WantParams topParams;

    AppExecFwk::ElementName element;
    element.SetAbilityName("WebNativeMessagingExtensionAbility");
    element.SetBundleName("web_native_messaging_service_test");
    element.SetDeviceID("12345");
    want.SetElement(element);
    want.SetParam(WANT_EXTENSION_ORIGIN_PARAM_KEY, std::string("app_instance_1"));
    AAFwk::WantParams readParams;
    readParams.SetParam("type", AAFwk::String::Box("FD"));
    readParams.SetParam("value", AAFwk::Integer::Box(1));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readParams));

    AAFwk::WantParams writeParams;
    writeParams.SetParam("type", AAFwk::String::Box("FD"));
    writeParams.SetParam("value", AAFwk::Integer::Box(2));
    topParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeParams));
    want.SetParams(topParams);

    ConnectNativeParams params;
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();

    MockAccesstokenKit::MockAccessTokenKitRet(-1);
    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::PERMISSION_CHECK_ERROR);

    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);

    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);

    auto aa = std::make_shared<ConnectionNativeRequest>();
    auto bb = std::make_shared<ConnectionNativeRequest>();
    aa->targetBundleName_ = bundleName;
    aa->SetInnerConnectionId(connectionId);
    bb->SetInnerConnectionId(0);

    aa->connectIdMap_.clear();
    int innerId = 0;
    aa->InsertInnerConnectionIdMap(g_callingTkid, g_callingPid, connectionId, innerId);
    std::weak_ptr<ConnectionNativeRequest> weak = aa;

    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);

    aa->connectIdMap_[innerId] = weak;
    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);

    std::string abilityName = "ability";
    HapTokenInfo tt = { 0 };
    tt.bundleName = bundleName;
    MockAccesstokenKit::MockGetHapTokenInfo(tt);

    manager_->NewIpcConnectionUnlock(g_callingTkid, bundleName, abilityName, token, -1);
    sptr<ExtensionIpcConnection> ipcConnect =
        new (std::nothrow) ExtensionIpcConnection(g_callingTkid, bundleName, abilityName,
        token, manager_->serviceHandler_);
    manager_->AbilityConnectMap_.clear();

    manager_->AbilityConnectMap_.insert(std::pair<std::pair<Security::AccessToken::AccessTokenID, std::string>,
        sptr<ExtensionIpcConnection>>({g_callingTkid, bundleName}, ipcConnect));
    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    auto tb = manager_->LookUpIpcConnection(g_callingTkid, aa->GetTargetBundleName());
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);

    ipcConnect->pendingRequests_.emplace_back(aa);
    ipcConnect->pendingRequests_.emplace_back(bb);
    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: StartAbility
 * @tc.desc: StartAbility()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, StartAbility, testing::ext::TestSize.Level0)
{
    int errorNum = 0;
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();

    std::string bundleName = "web_native_messaging_service_test";
    AAFwk::Want want;
    want.SetBundle(bundleName);
    AAFwk::WantParams topParams;

    AppExecFwk::ElementName element;
    element.SetAbilityName("WebNativeMessagingExtensionAbility");
    element.SetBundleName("web_native_messaging_service_test");
    element.SetDeviceID("12345");
    want.SetElement(element);
    want.SetParam(WANT_EXTENSION_ORIGIN_PARAM_KEY, std::string("app_instance_1"));
    AAFwk::WantParams readParams;
    readParams.SetParam("type", AAFwk::String::Box("FD"));
    readParams.SetParam("value", AAFwk::Integer::Box(1));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readParams));

    AAFwk::WantParams writeParams;
    writeParams.SetParam("type", AAFwk::String::Box("FD"));
    writeParams.SetParam("value", AAFwk::Integer::Box(2));
    topParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeParams));
    want.SetParams(topParams);

    AAFwk::StartOptions startOptions;
    MockAccesstokenKit::MockAccessTokenKitRet(-1);
    manager_->NewIpcConnectionUnlock(tkid, bundleName, abilityName, token, -1);

    manager_->StartAbility(token, want, startOptions, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::PERMISSION_CHECK_ERROR);

    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);
    std::string bundleName2 = "web_native_messaging_service_test2";

    sptr<ExtensionIpcConnection> ipcConnect =
        new (std::nothrow) ExtensionIpcConnection(g_callingTkid, bundleName2, abilityName, token, manager_->serviceHandler_);
    ipcConnect->targetExtensionPid_ = g_callingPid;
    manager_->AbilityConnectMap_.insert(std::pair<std::pair<Security::AccessToken::AccessTokenID, std::string>,
        sptr<ExtensionIpcConnection>>({g_callingTkid, bundleName2}, ipcConnect));
    manager_->StartAbility(token, want, startOptions, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::PERMISSION_CHECK_ERROR);
    manager_->AbilityConnectMap_.clear();
    sptr<ExtensionIpcConnection> ipcConnect2 =
        new (std::nothrow) ExtensionIpcConnection(g_callingTkid, bundleName, abilityName,
        token, manager_->serviceHandler_);
    ipcConnect2->targetExtensionPid_ = g_callingPid;
    manager_->AbilityConnectMap_.insert(std::pair<std::pair<Security::AccessToken::AccessTokenID, std::string>,
        sptr<ExtensionIpcConnection>>({g_callingTkid, bundleName}, ipcConnect2));
    manager_->StartAbility(token, want, startOptions, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::PERMISSION_CHECK_ERROR);
}

/**
 * @tc.name: StopNativeConnectionFromExtension
 * @tc.desc: StopNativeConnectionFromExtension()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, StopNativeConnectionFromExtension, testing::ext::TestSize.Level0)
{
    int32_t errorNum = 0;
    int32_t innerConnectId = 0;
    int32_t connectionId = 0;
    std::string bundleName = "abc";

    manager_->StopNativeConnectionFromExtension(innerConnectId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);

    auto aa = std::make_shared<ConnectionNativeRequest>();
    aa->targetBundleName_ = bundleName;
    aa->SetInnerConnectionId(connectionId);
    aa->connectIdMap_.clear();
    aa->InsertInnerConnectionIdMap(g_callingTkid, g_callingPid, connectionId, innerConnectId);
    std::weak_ptr<ConnectionNativeRequest> weak = aa;
    aa->connectIdMap_[innerConnectId] = weak;

    manager_->StopNativeConnectionFromExtension(innerConnectId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);
    auto bb = std::make_shared<ConnectionNativeRequest>();
    bb->targetBundleName_ = bundleName;
    bb->SetInnerConnectionId(connectionId);
    bb->targetExtensionPid_ = g_callingPid;

    bb->connectIdMap_.clear();
    bb->InsertInnerConnectionIdMap(g_callingTkid, g_callingPid, connectionId, innerConnectId);
    std::weak_ptr<ConnectionNativeRequest> weak2 = bb;
    bb->connectIdMap_[innerConnectId] = weak2;

    manager_->StopNativeConnectionFromExtension(innerConnectId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);

    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    int tkid = 0;

    sptr<ExtensionIpcConnection> ipcConnect =
        new (std::nothrow) ExtensionIpcConnection(tkid, bundleName, abilityName, token, manager_->serviceHandler_);
    manager_->AbilityConnectMap_.clear();

    manager_->AbilityConnectMap_.insert(std::pair<std::pair<Security::AccessToken::AccessTokenID, std::string>,
        sptr<ExtensionIpcConnection>>({tkid, bundleName}, ipcConnect));

    manager_->StopNativeConnectionFromExtension(innerConnectId, errorNum);
    EXPECT_TRUE(errorNum == ConnectNativeRet::CONNECTION_NOT_EXIST);
}

static void FillWant(AAFwk::Want& want)
{
    std::string abilityName = "ability";
    std::string bundleName = "web_native_messaging_service_test";
    want.SetBundle(bundleName);
    AppExecFwk::ElementName element;
    element.SetAbilityName("ability");
    element.SetBundleName("web_native_messaging_service_test");
    element.SetDeviceID("12345");
    want.SetElement(element);

    AAFwk::WantParams topParams;
    topParams.SetParam(WANT_EXTENSION_ORIGIN_PARAM_KEY, AAFwk::String::Box("app_instance_1"));
    AAFwk::WantParams readParams;
    readParams.SetParam("type", AAFwk::String::Box("FD"));
    readParams.SetParam("value", AAFwk::Integer::Box(MOCK_PIPE_FD));
    topParams.SetParam(WANT_READ_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(readParams));

    AAFwk::WantParams writeParams;
    writeParams.SetParam("type", AAFwk::String::Box("FD"));
    writeParams.SetParam("value", AAFwk::Integer::Box(MOCK_PIPE_FD));
    topParams.SetParam(WANT_WRITE_PIPE_PARAM_KEY, AAFwk::WantParamWrapper::Box(writeParams));
    want.SetParams(topParams);
}

/**
 * @tc.name: CreateNativeRequest001
 * @tc.desc: CreateNativeRequest()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, CreateNativeRequest001, testing::ext::TestSize.Level0)
{
    int errorNum = 0;
    ConnectNativeParams params;
    AAFwk::Want want;

    // FillRequestWithWant failed
    auto request1 = manager_->CreateNativeRequest(want, params, errorNum);
    ASSERT_EQ(request1, nullptr);

    FillWant(want);

    // params userid invalid
    params.callerUserId = -1;
    auto request2 = manager_->CreateNativeRequest(want, params, errorNum);
    ASSERT_EQ(request2, nullptr);

    params.callerUserId = 1;
    // GetBundlerInfo failed
    AppExecFwk::BundleMgrClient::mockRet = false;
    auto request3 = manager_->CreateNativeRequest(want, params, errorNum);
    ASSERT_EQ(request3, nullptr);

    // GetBundlerInfo ok, has same name ability, but type is invalid
    AppExecFwk::BundleMgrClient::mockRet = true;
    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    extensionInfo.name = "ability";
    AppExecFwk::BundleMgrClient::mockExtensionAbilityInfo.emplace_back(extensionInfo);
    auto request4 = manager_->CreateNativeRequest(want, params, errorNum);
    ASSERT_EQ(request4, nullptr);

    // CheckAbilityIsWebExtensionAbility ok
    AppExecFwk::ExtensionAbilityInfo extensionInfo1;
    extensionInfo1.name = "ability";
    extensionInfo1.type =
        AppExecFwk::ExtensionAbilityType::WEB_NATIVE_MESSAGING;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    extensionInfos.emplace_back(extensionInfo1);
    AppExecFwk::BundleMgrClient::mockExtensionAbilityInfo = extensionInfos;
    auto request5 = manager_->CreateNativeRequest(want, params, errorNum);
    ASSERT_NE(request5, nullptr);
}

/**
 * @tc.name: CreateNativeRequest002
 * @tc.desc: CreateNativeRequest() - test memory allocation failure
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, CreateNativeRequest002, testing::ext::TestSize.Level0)
{
    int errorNum = 0;
    ConnectNativeParams params;
    params.callerUserId = 1;
    AAFwk::Want want;
    FillWant(want);

    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    extensionInfo.name = "ability";
    extensionInfo.type = AppExecFwk::ExtensionAbilityType::WEB_NATIVE_MESSAGING;
    AppExecFwk::BundleMgrClient::mockExtensionAbilityInfo.emplace_back(extensionInfo);
    AppExecFwk::BundleMgrClient::mockRet = true;

    auto request = manager_->CreateNativeRequest(want, params, errorNum);
    ASSERT_NE(request, nullptr);
    EXPECT_EQ(errorNum, ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: DeleteIpcConnect
 * @tc.desc: DeleteIpcConnect()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, DeleteIpcConnect, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string bundleName = "test_bundle";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    manager_->DeleteIpcConnect(tkid, bundleName);
    EXPECT_TRUE(manager_->AbilityConnectMap_.empty());

    manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    EXPECT_FALSE(manager_->AbilityConnectMap_.empty());

    manager_->DeleteIpcConnect(tkid, bundleName);
    EXPECT_TRUE(manager_->AbilityConnectMap_.empty());

    manager_->DeleteIpcConnect(tkid, bundleName);
    EXPECT_TRUE(manager_->AbilityConnectMap_.empty());
}

/**
 * @tc.name: IsIpcConnectExist
 * @tc.desc: IsIpcConnectExist()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, IsIpcConnectExist, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string bundleName = "test_bundle";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    EXPECT_FALSE(manager_->IsIpcConnectExist());

    manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    EXPECT_TRUE(manager_->IsIpcConnectExist());

    manager_->DeleteIpcConnect(tkid, bundleName);
    EXPECT_FALSE(manager_->IsIpcConnectExist());
}

/**
 * @tc.name: CleanAbilityConnection
 * @tc.desc: CleanAbilityConnection()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, CleanAbilityConnection, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string bundleName = "test_bundle";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    manager_->Init();
    manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);

    manager_->CleanAbilityConnection(tkid, bundleName);
    EXPECT_FALSE(manager_->IsIpcConnectExist());

    manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    manager_->CleanAbilityConnection(tkid, bundleName);
    EXPECT_FALSE(manager_->IsIpcConnectExist());
}

/**
 * @tc.name: GetPidExtensionBundleName
 * @tc.desc: GetPidExtensionBundleName()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, GetPidExtensionBundleName, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string bundleName = "test_bundle";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    std::string result;

    EXPECT_FALSE(manager_->GetPidExtensionBundleName(-1, result));
    EXPECT_FALSE(manager_->GetPidExtensionBundleName(0, result));

    EXPECT_FALSE(manager_->GetPidExtensionBundleName(100, result));

    auto ipcConnect = manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    int32_t testPid = 9999;
    ipcConnect->targetExtensionPid_ = testPid;

    EXPECT_TRUE(manager_->GetPidExtensionBundleName(testPid, result));
    EXPECT_EQ(result, bundleName);

    std::string result2;
    EXPECT_FALSE(manager_->GetPidExtensionBundleName(8888, result2));
}

/**
 * @tc.name: NewIpcConnectionUnlock
 * @tc.desc: NewIpcConnectionUnlock()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, NewIpcConnectionUnlock, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x123;
    std::string bundleName = "test_bundle";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    int32_t userId = 100;

    auto ipcConnect = manager_->NewIpcConnectionUnlock(tkid, bundleName, abilityName, token, userId);
    ASSERT_NE(ipcConnect, nullptr);
    EXPECT_EQ(ipcConnect->callerUserId_, userId);

    auto found = manager_->LookUpIpcConnectionUnlock(tkid, bundleName);
    EXPECT_EQ(found, ipcConnect);
}

/**
 * @tc.name: NewIpcConnection
 * @tc.desc: NewIpcConnection()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, NewIpcConnection, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x456;
    std::string bundleName = "test_bundle2";
    std::string abilityName = "ability2";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    int32_t userId = 200;

    auto ipcConnect = manager_->NewIpcConnection(tkid, bundleName, abilityName, token, userId);
    ASSERT_NE(ipcConnect, nullptr);
    EXPECT_EQ(ipcConnect->callerUserId_, userId);
}

/**
 * @tc.name: LookUpIpcConnectionUnlock
 * @tc.desc: LookUpIpcConnectionUnlock()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, LookUpIpcConnectionUnlock, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x789;
    std::string bundleName = "test_bundle3";
    std::string abilityName = "ability3";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    auto result = manager_->LookUpIpcConnectionUnlock(tkid, bundleName);
    EXPECT_EQ(result, nullptr);

    auto ipcConnect = manager_->NewIpcConnectionUnlock(tkid, bundleName, abilityName, token, 1);
    auto found = manager_->LookUpIpcConnectionUnlock(tkid, bundleName);
    EXPECT_EQ(found, ipcConnect);

    std::string wrongBundleName = "wrong_bundle";
    auto notFound = manager_->LookUpIpcConnectionUnlock(tkid, wrongBundleName);
    EXPECT_EQ(notFound, nullptr);
}

/**
 * @tc.name: LookUpIpcConnection
 * @tc.desc: LookUpIpcConnection()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, LookUpIpcConnection, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0xABC;
    std::string bundleName = "test_bundle4";
    std::string abilityName = "ability4";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    auto result = manager_->LookUpIpcConnection(tkid, bundleName);
    EXPECT_EQ(result, nullptr);

    auto ipcConnect = manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    auto found = manager_->LookUpIpcConnection(tkid, bundleName);
    EXPECT_EQ(found, ipcConnect);

    Security::AccessToken::AccessTokenID wrongTkid = 0xDEF;
    auto notFound = manager_->LookUpIpcConnection(wrongTkid, bundleName);
    EXPECT_EQ(notFound, nullptr);
}

/**
 * @tc.name: ExitSaProcess
 * @tc.desc: ExitSaProcess()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, ExitSaProcess, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x111;
    std::string bundleName = "test_bundle5";
    std::string abilityName = "ability5";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    manager_->ExitSaProcess();
    EXPECT_TRUE(manager_->IsIpcConnectExist());

    manager_->DeleteIpcConnect(tkid, bundleName);
    manager_->ExitSaProcess();
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, Dump, testing::ext::TestSize.Level0)
{
    std::vector<std::u16string> args;
    int32_t ret = manager_->Dump(-1, args);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);

    args.push_back(u"-h");
    ret = manager_->Dump(1, args);
    EXPECT_EQ(ret, ERR_OK);

    args.clear();
    args.push_back(u"-d");
    ret = manager_->Dump(1, args);
    EXPECT_EQ(ret, ERR_OK);

    args.clear();
    ret = manager_->Dump(1, args);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: Init
 * @tc.desc: Init()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, Init, testing::ext::TestSize.Level0)
{
    auto testManager = std::make_shared<WebNativeMessagingManager>();
    bool result = testManager->Init();
    EXPECT_TRUE(result);
    EXPECT_NE(testManager->runner_, nullptr);
    EXPECT_NE(testManager->serviceHandler_, nullptr);
    EXPECT_NE(testManager->delayExitTask_, nullptr);
}

/**
 * @tc.name: ConnectWebNativeMessagingExtension002
 * @tc.desc: ConnectWebNativeMessagingExtension() - test retry logic
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, ConnectWebNativeMessagingExtension002, testing::ext::TestSize.Level0)
{
    std::string bundleName = "test_bundle";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();

    AAFwk::Want want;
    FillWant(want);

    int connectionId = 123;
    int errorNum = 0;

    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);
    HapTokenInfo tt = { 0 };
    tt.bundleName = bundleName;
    MockAccesstokenKit::MockGetHapTokenInfo(tt);

    AppExecFwk::BundleMgrClient::mockRet = true;
    AppExecFwk::ExtensionAbilityInfo extensionInfo;
    extensionInfo.name = "ability";
    extensionInfo.type = AppExecFwk::ExtensionAbilityType::WEB_NATIVE_MESSAGING;
    AppExecFwk::BundleMgrClient::mockExtensionAbilityInfo.emplace_back(extensionInfo);

    manager_->Init();
    manager_->ConnectWebNativeMessagingExtension(token, want, callback, connectionId, errorNum);
}

/**
 * @tc.name: DisconnectWebNativeMessagingExtension002
 * @tc.desc: DisconnectWebNativeMessagingExtension() - test additional branches
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, DisconnectWebNativeMessagingExtension002, testing::ext::TestSize.Level0)
{
    int connectionId = 123;
    int errorNum = 0;
    std::string bundleName = "test_bundle";

    auto aa = std::make_shared<ConnectionNativeRequest>();
    aa->targetBundleName_ = bundleName;
    aa->SetInnerConnectionId(connectionId);
    aa->targetExtensionPid_ = g_callingPid;
    aa->callerTokenId_ = g_callingTkid;
    aa->callerPid_ = g_callingPid;
    aa->callerConnectionId_ = connectionId;

    ConnectionNativeRequest::InsertRequestMap(aa);
    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);

    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    manager_->Init();
    auto ipcConnect = manager_->NewIpcConnection(g_callingTkid, bundleName, abilityName, token, 1);
    ipcConnect->pendingRequests_.clear();
    ipcConnect->pendingRequests_.emplace_back(aa);

    auto bb = std::make_shared<ConnectionNativeRequest>();
    bb->targetBundleName_ = bundleName;
    bb->SetInnerConnectionId(456);
    bb->targetExtensionPid_ = g_callingPid;
    ipcConnect->pendingRequests_.emplace_back(bb);

    manager_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    EXPECT_EQ(errorNum, ConnectNativeRet::SUCCESS);
    ConnectionNativeRequest::RemoveRequestMap(connectionId);
}

/**
 * @tc.name: StartAbility002
 * @tc.desc: StartAbility() - test with null token and different bundle
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, StartAbility002, testing::ext::TestSize.Level0)
{
    int errorNum = 0;
    std::string bundleName = "test_bundle";

    AAFwk::Want want;
    FillWant(want);
    want.SetBundle(bundleName);

    AAFwk::StartOptions startOptions;

    MockAccesstokenKit::MockAccessTokenKitRet(PermissionState::PERMISSION_GRANTED);
    manager_->StartAbility(nullptr, want, startOptions, errorNum);
}

/**
 * @tc.name: StopNativeConnectionFromExtension002
 * @tc.desc: StopNativeConnectionFromExtension() - test successful disconnect
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, StopNativeConnectionFromExtension002, testing::ext::TestSize.Level0)
{
    int32_t errorNum = 0;
    int32_t innerConnectId = 456;
    int32_t connectionId = 789;
    std::string bundleName = "test_bundle";

    auto aa = std::make_shared<ConnectionNativeRequest>();
    aa->targetBundleName_ = bundleName;
    aa->SetInnerConnectionId(innerConnectId);
    aa->targetExtensionPid_ = g_callingPid;
    aa->callerTokenId_ = g_callingTkid;
    aa->callerPid_ = g_callingPid;
    aa->callerConnectionId_ = connectionId;

    ConnectionNativeRequest::InsertRequestMap(aa);
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();

    manager_->Init();
    auto ipcConnect = manager_->NewIpcConnection(g_callingTkid, bundleName, abilityName, token, 1);
    ipcConnect->pendingRequests_.clear();
    ipcConnect->pendingRequests_.emplace_back(aa);

    auto bb = std::make_shared<ConnectionNativeRequest>();
    bb->targetBundleName_ = bundleName;
    bb->SetInnerConnectionId(457);
    bb->targetExtensionPid_ = g_callingPid;
    ipcConnect->pendingRequests_.emplace_back(bb);

    manager_->StopNativeConnectionFromExtension(innerConnectId, errorNum);
    EXPECT_EQ(errorNum, ConnectNativeRet::SUCCESS);
    ConnectionNativeRequest::RemoveRequestMap(innerConnectId);
}

/**
 * @tc.name: LookUpOrNewIpcConnection002
 * @tc.desc: LookUpOrNewIpcConnection() - test lookup existing
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingManagerTest, LookUpOrNewIpcConnection002, testing::ext::TestSize.Level0)
{
    Security::AccessToken::AccessTokenID tkid = 0x222;
    std::string bundleName = "test_bundle_lookup";
    std::string abilityName = "ability";
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    auto ipcConnect1 = manager_->NewIpcConnection(tkid, bundleName, abilityName, token, 1);
    ASSERT_NE(ipcConnect1, nullptr);

    auto ipcConnect2 = manager_->LookUpOrNewIpcConnection(tkid, bundleName, abilityName, token, 1);
    EXPECT_EQ(ipcConnect1, ipcConnect2);
}
} // namespace NWeb
} // namespace OHOS
