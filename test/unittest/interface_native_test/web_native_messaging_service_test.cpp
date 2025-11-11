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
#include "web_native_messaging_service.h"
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

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
int g_callingUid = 0;


namespace OHOS {
const std::string WANT_EXTENSION_ORIGIN_PARAM_KEY = "ohos.arkweb.extensionOrigin";
const std::string WANT_READ_PIPE_PARAM_KEY = "ohos.arkweb.messageReadPipe";
const std::string WANT_WRITE_PIPE_PARAM_KEY = "ohos.arkweb.messageWritePipe";

int IPCSkeleton::GetCallingUid()
{
    return g_callingUid;
}
class MockSystemAbilityOnDemandReason : public OHOS::SystemAbilityOnDemandReason {
public:
    MOCK_METHOD(std::string, GetName, (), (const));
    MOCK_METHOD(std::string, GetValue, (), (const));
    MOCK_METHOD(bool, HasExtraData, (), (const));
    MOCK_METHOD(OHOS::OnDemandReasonExtraData, GetExtraData, (), (const));
    MOCK_METHOD(OHOS::OnDemandReasonId, GetReasonId, (), (const));
};

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

class MockCommonEventSubscriber : public OHOS::EventFwk::CommonEventSubscriber {
public:
    MockCommonEventSubscriber() = default;
    ~MockCommonEventSubscriber() = default;
    MOCK_METHOD1(OnReceiveEvent, void(const OHOS::EventFwk::CommonEventData&));
};

namespace system {
bool SetParameter(const std::string& key, const std::string& value)
{
    if (value == "") {
        return false;
    }
    return true;
}
} // namespace system

bool SystemAbility::Publish(sptr<IRemoteObject> systemAbility)
{
    systemAbility.ForceSetRefPtr(nullptr);
    return true;
}

namespace NWeb {
class WebNativeMessagingServiceTest : public testing::Test {
public:
    WebNativeMessagingServiceTest() {}
    virtual ~WebNativeMessagingServiceTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    std::shared_ptr<WebNativeMessagingService> service_;
    int32_t connectionId = -1;
};

void WebNativeMessagingServiceTest::SetUp()
{
    service_ = std::make_shared<WebNativeMessagingService>(0, true);
}

void WebNativeMessagingServiceTest::TearDown()
{
    service_.reset();
}

class MockPackageCallback {
public:
    void OnPackageChangedEvent(const std::string& bundleName, const std::string& hapPath)
    {
        lastBundleName = bundleName;
        lastHapPath = hapPath;
        callCount++;
    }

    std::string lastBundleName;
    std::string lastHapPath;
    int callCount = 0;
};

/**
 * @tc.name: StartAbility_001
 * @tc.desc: StartAbility()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingServiceTest, StartAbility_001, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> token;
    AAFwk::Want want;
    std::string bundleName = "web_native_messaging_service_test";
    want.SetBundle(bundleName);

    AppExecFwk::ElementName element;
    element.SetAbilityName("WebNativeMessagingExtensionAbility");
    element.SetBundleName("web_native_messaging_service_test");
    element.SetDeviceID("12345");
    want.SetElement(element);
    want.SetType("12345");

    AAFwk::StartOptions startOptions;
    int32_t errorNum = 0;

    service_->StartAbility(token, want, startOptions, errorNum);
    ASSERT_NE(errorNum, ERR_OK);

    service_->manager_ = nullptr;
    service_->StartAbility(token, want, startOptions, errorNum);
    ASSERT_NE(errorNum, ERR_OK);
}

/**
 * @tc.name: ConnectWebNativeMessagingExtension
 * @tc.desc: ConnectWebNativeMessagingExtension()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingServiceTest, ConnectWebNativeMessagingExtension, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();
    AAFwk::Want want;
    std::string bundleName = "web_native_messaging_service_test";
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
    int32_t errorNum = 0;

    int32_t connectionId = 1;
    service_->ConnectWebNativeMessagingExtension(token, want, callback, connectionId, errorNum);
    ASSERT_NE(errorNum, ERR_OK);

    service_->manager_ = nullptr;
    service_->ConnectWebNativeMessagingExtension(token, want, callback, connectionId, errorNum);
    ASSERT_NE(errorNum, ERR_OK);
}

/**
 * @tc.name: DisconnectWebNativeMessagingExtension
 * @tc.desc: DisconnectWebNativeMessagingExtension()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingServiceTest, DisconnectWebNativeMessagingExtension, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();
    AAFwk::Want want;
    std::string bundleName = "web_native_messaging_service_test";
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
    int32_t errorNum = 0;

    int32_t connectionId = 1;
    service_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    ASSERT_NE(errorNum, ERR_OK);

    service_->manager_ = nullptr;
    service_->DisconnectWebNativeMessagingExtension(connectionId, errorNum);
    ASSERT_NE(errorNum, ERR_OK);
}

/**
 * @tc.name: StopNativeConnectionFromExtension
 * @tc.desc: StopNativeConnectionFromExtension()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingServiceTest, StopNativeConnectionFromExtension, testing::ext::TestSize.Level0)
{
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    sptr<IRemoteObject> callback = new IRemoteObjectMocker();
    AAFwk::Want want;
    std::string bundleName = "web_native_messaging_service_test";
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
    int32_t errorNum = 0;

    int32_t connectionId = 1;
    service_->StopNativeConnectionFromExtension(connectionId, errorNum);
    ASSERT_NE(errorNum, ERR_OK);

    service_->manager_ = nullptr;
    service_->StopNativeConnectionFromExtension(connectionId, errorNum);
    ASSERT_NE(errorNum, ERR_OK);
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingServiceTest, Dump, testing::ext::TestSize.Level0)
{
    int32_t errorNum = 0;
    std::vector<std::u16string> args = { u"abc", u"bcd"};

    int fd = 1;
    service_->manager_ = nullptr;
    errorNum = service_->Dump(fd, args);
    EXPECT_TRUE(errorNum == ERR_INVALID_VALUE);

    service_->manager_ = std::make_shared<WebNativeMessagingManager>();
    service_->manager_->Init();
    fd = -1;
    errorNum = service_->Dump(fd, args);
    EXPECT_TRUE(errorNum == ERR_INVALID_VALUE);
    fd = 1;
    errorNum = service_->Dump(fd, args);
    EXPECT_TRUE(errorNum == ERR_OK);
}

/**
 * @tc.name: OnStart
 * @tc.desc: OnStart()
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(WebNativeMessagingServiceTest, OnStart, testing::ext::TestSize.Level0)
{
    std::shared_ptr<MockSystemAbilityOnDemandReason> reason = std::make_shared<MockSystemAbilityOnDemandReason>();

    service_->registerToService_ = false;
    service_->OnStart(*reason);
    EXPECT_TRUE(1);

    service_->registerToService_ = true;
    service_->OnStart(*reason);
    EXPECT_TRUE(1);
}
} // namespace NWeb
} // namespace OHOS
