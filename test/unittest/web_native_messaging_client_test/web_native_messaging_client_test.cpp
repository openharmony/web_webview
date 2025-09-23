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

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#define private public
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "web_native_messaging_client.h"
#include "web_native_messaging_load_callback.h"

#include "base/web/webview/sa/web_native_messaging/common/web_native_messaging_common.h"
#include "base/web/webview/sa/web_native_messaging/iweb_native_messaging_service.h"

using namespace testing;
using namespace OHOS;
using namespace NWeb;

sptr<ISystemAbilityManager> g_mockSAMgr = nullptr;

namespace OHOS {
SystemAbilityManagerClient& SystemAbilityManagerClient::GetInstance()
{
    static SystemAbilityManagerClient instance;
    return instance;
}
sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    return g_mockSAMgr;
}
} // namespace OHOS

namespace OHOS {
namespace NWeb {
class MockRemoteObject : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"mock_object") {}
    ~MockRemoteObject() = default;
    int32_t GetObjectRefCount() override
    {
        return 1;
    }
    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    }
    int Dump(int fd, const std::vector<std::u16string>& args) override
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        recipient_ = recipient;
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        if (recipient_ == recipient) {
            recipient_ = nullptr;
            return true;
        }
        return false;
    }
    void TriggerDeath()
    {
        if (recipient_ != nullptr) {
            recipient_->OnRemoteDied(this);
        }
    }

private:
    sptr<DeathRecipient> recipient_ = nullptr;
};

class MockWebNativeMessagingService : public IWebNativeMessagingService {
public:
    MOCK_METHOD(int32_t, ConnectWebNativeMessagingExtension,
        (const sptr<IRemoteObject>& token, const AAFwk::Want& want, const sptr<IRemoteObject>& connectCallback,
            int32_t connectionId, int32_t& errorNum),
        (override));
    MOCK_METHOD(int32_t, DisconnectWebNativeMessagingExtension, (int32_t connectionId, int32_t& errorNum), (override));
    MOCK_METHOD(int32_t, StartAbility,
        (const sptr<IRemoteObject>& token, const AAFwk::Want& want, const AAFwk::StartOptions& options,
            int32_t& errorNum),
        (override));
    MOCK_METHOD(int32_t, StopNativeConnectionFromExtension, (int32_t connectionId, int32_t& errorNum), (override));
    sptr<IRemoteObject> AsObject() override
    {
        if (remoteObject_ == nullptr) {
            remoteObject_ = new MockRemoteObject();
        }
        return remoteObject_;
    }

private:
    sptr<MockRemoteObject> remoteObject_;
};

class MockSystemAbilityManager : public ISystemAbilityManager {
public:
    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
    MOCK_METHOD(sptr<IRemoteObject>, CheckSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(int32_t, LoadSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback>& callback),
        (override));
    std::vector<std::u16string> ListSystemAbilities(unsigned int dumpFlags) override
    {
        return {};
    }
    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId) override
    {
        return nullptr;
    }
    int32_t RemoveSystemAbility(int32_t systemAbilityId) override
    {
        return 0;
    }
    int32_t SubscribeSystemAbility(int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange>& listener) override
    {
        return 0;
    }
    int32_t UnSubscribeSystemAbility(int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange>& listener) override
    {
        return 0;
    }
    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override
    {
        return nullptr;
    }
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override
    {
        return nullptr;
    }
    int32_t AddOnDemandSystemAbilityInfo(
        int32_t systemAbilityId, const std::u16string& localAbilityManagerName) override
    {
        return 0;
    }
    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, bool& isExist) override
    {
        isExist = false;
        return nullptr;
    }
    int32_t AddSystemAbility(
        int32_t systemAbilityId, const sptr<IRemoteObject>& ability, const SAExtraProp& extraProp) override
    {
        return 0;
    }
    int32_t AddSystemProcess(const std::u16string& procName, const sptr<IRemoteObject>& procObject) override
    {
        return 0;
    }
    sptr<IRemoteObject> LoadSystemAbility(int32_t systemAbilityId, int32_t timeout) override
    {
        return nullptr;
    }
    int32_t LoadSystemAbility(
        int32_t systemAbilityId, const std::string& deviceId, const sptr<ISystemAbilityLoadCallback>& callback) override
    {
        return 0;
    }
    int32_t UnloadSystemAbility(int32_t systemAbilityId) override
    {
        return 0;
    }
    int32_t CancelUnloadSystemAbility(int32_t systemAbilityId) override
    {
        return 0;
    }
    int32_t UnloadAllIdleSystemAbility() override
    {
        return 0;
    }
    int32_t GetSystemProcessInfo(int32_t systemAbilityId, SystemProcessInfo& systemProcessInfo) override
    {
        return 0;
    }
    int32_t GetRunningSystemProcess(std::list<SystemProcessInfo>& systemProcessInfos) override
    {
        return 0;
    }
    int32_t SubscribeSystemProcess(const sptr<ISystemProcessStatusChange>& listener) override
    {
        return 0;
    }
    int32_t SendStrategy(
        int32_t type, std::vector<int32_t>& systemAbilityIds, int32_t level, std::string& action) override
    {
        return 0;
    }
    int32_t UnSubscribeSystemProcess(const sptr<ISystemProcessStatusChange>& listener) override
    {
        return 0;
    }
    int32_t GetExtensionSaIds(const std::string& extension, std::vector<int32_t>& saIds) override
    {
        return 0;
    }
    int32_t GetExtensionRunningSaList(const std::string& extension, std::vector<sptr<IRemoteObject>>& saList) override
    {
        return 0;
    }
    int32_t GetRunningSaExtensionInfoList(const std::string& extension, std::vector<SaExtensionInfo>& infoList) override
    {
        return 0;
    }
    int32_t GetCommonEventExtraDataIdlist(
        int32_t saId, std::vector<int64_t>& extraDataIdList, const std::string& eventName) override
    {
        return 0;
    }
    int32_t GetOnDemandReasonExtraData(int64_t extraDataId, MessageParcel& extraDataParcel) override
    {
        return 0;
    }
    int32_t GetOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
        std::vector<SystemAbilityOnDemandEvent>& abilityOnDemandEvents) override
    {
        return 0;
    }
    int32_t UpdateOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
        const std::vector<SystemAbilityOnDemandEvent>& abilityOnDemandEvents) override
    {
        return 0;
    }
    int32_t GetOnDemandSystemAbilityIds(std::vector<int32_t>& systemAbilityIds) override
    {
        return 0;
    }
};

class WebNativeMessagingClientTest : public Test {
protected:
    void SetUp() override
    {
        mockService_ = new NiceMock<MockWebNativeMessagingService>();
        mockSAMgr_ = new NiceMock<MockSystemAbilityManager>();
        g_mockSAMgr = mockSAMgr_;
        client_.SetWebNativeMessagingProxy(nullptr);
        client_.SetUserDefineDiedRecipient(nullptr);
    }
    void TearDown() override
    {
        g_mockSAMgr = nullptr;
        mockService_ = nullptr;
        mockSAMgr_ = nullptr;
        client_.SetWebNativeMessagingProxy(nullptr);
        client_.SetUserDefineDiedRecipient(nullptr);
    }
    void SimulateSALoadSuccess(const sptr<ISystemAbilityLoadCallback>& callback, int32_t systemAbilityId)
    {
        if (callback) {
            callback->OnLoadSystemAbilitySuccess(systemAbilityId, mockService_->AsObject());
        }
    }
    void SimulateSALoadFail(const sptr<ISystemAbilityLoadCallback>& callback, int32_t systemAbilityId)
    {
        if (callback) {
            callback->OnLoadSystemAbilityFail(systemAbilityId);
        }
    }
    void WaitForCondition(std::function<bool()> condition, int timeoutMs = 1000)
    {
        auto start = std::chrono::steady_clock::now();
        while (!condition() &&
               std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() <
                   timeoutMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    WebNativeMessagingClient& client_ = WebNativeMessagingClient::GetInstance();
    sptr<MockWebNativeMessagingService> mockService_;
    sptr<MockSystemAbilityManager> mockSAMgr_;
};

/**
 * @tc.name: WebNativeMessagingClientTest_GetInstance_001
 * @tc.desc: Verify that GetInstance returns the same instance (singleton pattern)
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, GetInstance_ShouldReturnSameInstance)
{
    auto& instance1 = WebNativeMessagingClient::GetInstance();
    auto& instance2 = WebNativeMessagingClient::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: WebNativeMessagingClientTest_GetProxy_001
 * @tc.desc: Verify that proxy can be obtained from cache when available
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, GetWebNativeMessagingProxy_ShouldReturnCachedProxyWhenAvailable)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    sptr<IWebNativeMessagingService> proxy = client_.GetWebNativeMessagingProxy();
    ASSERT_NE(proxy, nullptr);
    EXPECT_EQ(proxy->AsObject(), mockService_->AsObject());
}

/**
 * @tc.name: WebNativeMessagingClientTest_GetProxy_002
 * @tc.desc: Verify that proxy can be obtained from CheckSystemAbility when available
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, GetWebNativeMessagingProxy_ShouldGetFromCheckSystemAbilityWhenAvailable)
{
    ON_CALL(*mockSAMgr_, CheckSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID))
        .WillByDefault(Return(mockService_->AsObject()));
    sptr<IWebNativeMessagingService> proxy = client_.GetWebNativeMessagingProxy();
    ASSERT_NE(proxy, nullptr);
    EXPECT_NE(proxy->AsObject(), nullptr);
}

/**
 * @tc.name: WebNativeMessagingClientTest_GetProxy_003
 * @tc.desc: Verify that proxy can be loaded when CheckSystemAbility fails
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, GetWebNativeMessagingProxy_ShouldLoadWhenCheckFails)
{
    sptr<ISystemAbilityLoadCallback> capturedCallback;
    std::mutex mtx;
    std::condition_variable cv;
    bool isLoaded = false;
    ON_CALL(*mockSAMgr_, CheckSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID)).WillByDefault(Return(nullptr));
    ON_CALL(*mockSAMgr_, LoadSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID, _))
        .WillByDefault(DoAll(SaveArg<1>(&capturedCallback),
            Invoke([this, &cv, &isLoaded, &capturedCallback, &mtx](
                       int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback>&) {
                std::thread([this, systemAbilityId, &cv, &isLoaded, &mtx, capturedCallback] {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    SimulateSALoadSuccess(capturedCallback, systemAbilityId);
                    std::lock_guard<std::mutex> lock(mtx);
                    isLoaded = true;
                    cv.notify_one();
                }).detach();
            }),
            Return(0)));
    client_.GetWebNativeMessagingProxy();
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait_for(lock, std::chrono::milliseconds(500), [&isLoaded] { return isLoaded; });
    sptr<IWebNativeMessagingService> finalProxy = client_.GetWebNativeMessagingProxy();
    ASSERT_NE(finalProxy, nullptr);
}

/**
 * @tc.name: WebNativeMessagingClientTest_GetProxy_004
 * @tc.desc: Verify proxy return when loading fails
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, GetWebNativeMessagingProxy_ShouldReturnNullWhenLoadFails)
{
    sptr<ISystemAbilityLoadCallback> capturedCallback;
    ON_CALL(*mockSAMgr_, CheckSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID)).WillByDefault(Return(nullptr));
    ON_CALL(*mockSAMgr_, LoadSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID, _))
        .WillByDefault(DoAll(SaveArg<1>(&capturedCallback),
            Invoke([this, &capturedCallback](int32_t systemAbilityId, const sptr<ISystemAbilityLoadCallback>&) {
                SimulateSALoadFail(capturedCallback, systemAbilityId);
            }),
            Return(0)));
    sptr<IWebNativeMessagingService> proxy = client_.GetWebNativeMessagingProxy();
    WaitForCondition([&proxy] { return proxy == nullptr; });
    EXPECT_NE(proxy, nullptr);
}

/**
 * @tc.name: WebNativeMessagingClientTest_GetProxy_005
 * @tc.desc: Verify proxy return when SAMgr is null
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, GetWebNativeMessagingProxy_ShouldReturnNullWhenSAMgrIsNull)
{
    g_mockSAMgr = nullptr;
    sptr<IWebNativeMessagingService> proxy = client_.GetWebNativeMessagingProxy();
    EXPECT_NE(proxy, nullptr);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Connect_001
 * @tc.desc: Verify successful connection with valid parameters
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, ConnectWebNativeMessagingExtension_ShouldReturnSuccessForValidParams)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    sptr<IRemoteObject> token = new MockRemoteObject();
    sptr<IRemoteObject> callback = new MockRemoteObject();
    const int32_t connectionId = 123;
    const int32_t expectedErrorNum = ConnectNativeRet::SUCCESS;
    ON_CALL(*mockService_, ConnectWebNativeMessagingExtension(token, _, callback, connectionId, _))
        .WillByDefault(DoAll(SetArgReferee<4>(expectedErrorNum), Return(0)));
    int result = client_.ConnectWebNativeMessagingExtension(token, {}, callback, connectionId);
    EXPECT_EQ(result, expectedErrorNum);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Connect_002
 * @tc.desc: Verify connection result with null token
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, ConnectWebNativeMessagingExtension_ShouldReturnContextErrorForNullToken)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    sptr<IRemoteObject> invalidToken = nullptr;
    sptr<IRemoteObject> validCallback = new MockRemoteObject();
    int result = client_.ConnectWebNativeMessagingExtension(invalidToken, {}, validCallback, 0);
    EXPECT_EQ(result, ConnectNativeRet::CONTEXT_ERROR);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Connect_003
 * @tc.desc: Verify connection result with null callback
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, ConnectWebNativeMessagingExtension_ShouldReturnCallbackErrorForNullCallback)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    sptr<IRemoteObject> validToken = new MockRemoteObject();
    sptr<IRemoteObject> invalidCallback = nullptr;
    int result = client_.ConnectWebNativeMessagingExtension(validToken, {}, invalidCallback, 0);
    EXPECT_EQ(result, ConnectNativeRet::CALLBACK_ERROR);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Connect_004
 * @tc.desc: Verify connection result when proxy is null
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, ConnectWebNativeMessagingExtension_ShouldReturnIpcErrorWhenProxyIsNull)
{
    client_.SetWebNativeMessagingProxy(nullptr);
    sptr<IRemoteObject> token = new MockRemoteObject();
    sptr<IRemoteObject> callback = new MockRemoteObject();
    int result = client_.ConnectWebNativeMessagingExtension(token, {}, callback, 0);
    EXPECT_EQ(result, ConnectNativeRet::IPC_ERROR);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Connect_005
 * @tc.desc: Verify connection result when proxy fails
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, ConnectWebNativeMessagingExtension_ShouldReturnIpcErrorWhenProxyFails)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    sptr<IRemoteObject> token = new MockRemoteObject();
    sptr<IRemoteObject> callback = new MockRemoteObject();
    const int32_t connectionId = 123;
    const int32_t expectedError = -1;
    ON_CALL(*mockService_, ConnectWebNativeMessagingExtension(_, _, _, connectionId, _))
        .WillByDefault(Return(expectedError));
    int result = client_.ConnectWebNativeMessagingExtension(token, {}, callback, connectionId);
    EXPECT_EQ(result, ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Disconnect_001
 * @tc.desc: Verify disconnection result when proxy is null
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, DisconnectWebNativeMessagingExtension_ShouldReturnIpcErrorWhenProxyIsNull)
{
    client_.SetWebNativeMessagingProxy(nullptr);
    int result = client_.DisconnectWebNativeMessagingExtension(1);
    EXPECT_EQ(result, ConnectNativeRet::PERMISSION_CHECK_ERROR);
}

/**
 * @tc.name: WebNativeMessagingClientTest_Disconnect_002
 * @tc.desc: Verify disconnection result when proxy fails
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, DisconnectWebNativeMessagingExtension_ShouldReturnIpcErrorWhenProxyFails)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    ON_CALL(*mockService_, DisconnectWebNativeMessagingExtension(_, _)).WillByDefault(Return(-1));
    int result = client_.DisconnectWebNativeMessagingExtension(1);
    EXPECT_EQ(result, ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: WebNativeMessagingClientTest_StartAbility_001
 * @tc.desc: Verify StartAbility result when proxy is null
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, StartAbility_ShouldReturnIpcErrorWhenProxyIsNull)
{
    client_.SetWebNativeMessagingProxy(nullptr);
    sptr<IRemoteObject> token = new MockRemoteObject();
    AAFwk::Want want;
    AAFwk::StartOptions options;
    int result = client_.StartAbility(token, want, options);
    EXPECT_EQ(result, ConnectNativeRet::IPC_ERROR);
}

/**
 * @tc.name: WebNativeMessagingClientTest_StartAbility_002
 * @tc.desc: Verify StartAbility result when proxy fails
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, StartAbility_ShouldReturnIpcErrorWhenProxyFails)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    sptr<IRemoteObject> token = new MockRemoteObject();
    AAFwk::Want want;
    AAFwk::StartOptions options;
    ON_CALL(*mockService_, StartAbility(_, _, _, _)).WillByDefault(Return(-1));
    int result = client_.StartAbility(token, want, options);
    EXPECT_EQ(result, ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: WebNativeMessagingClientTest_StopConnection_001
 * @tc.desc: Verify StopNativeConnection result when proxy is null
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, StopNativeConnectionFromExtension_ShouldReturnIpcErrorWhenProxyIsNull)
{
    client_.SetWebNativeMessagingProxy(nullptr);
    int result = client_.StopNativeConnectionFromExtension(1);
    EXPECT_EQ(result, ConnectNativeRet::CONNECTION_NOT_EXIST);
}

/**
 * @tc.name: WebNativeMessagingClientTest_StopConnection_002
 * @tc.desc: Verify StopNativeConnection result when proxy fails
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, StopNativeConnectionFromExtension_ShouldReturnIpcErrorWhenProxyFails)
{
    client_.SetWebNativeMessagingProxy(mockService_->AsObject());
    ON_CALL(*mockService_, StopNativeConnectionFromExtension(_, _)).WillByDefault(Return(-1));
    int result = client_.StopNativeConnectionFromExtension(1);
    EXPECT_EQ(result, ConnectNativeRet::SUCCESS);
}

/**
 * @tc.name: WebNativeMessagingClientTest_DeathRecipient_001
 * @tc.desc: Verify death recipient can clear proxy and trigger callback
 * @tc.type: Func
 * @tc.require:
 */
TEST_F(WebNativeMessagingClientTest, DeathRecipient_ShouldClearProxyAndTriggerCallback)
{
    bool callbackCalled = false;
    std::mutex mtx;
    std::condition_variable cv;
    std::function<void()> deathCallback = [&]() {
        std::lock_guard<std::mutex> lock(mtx);
        callbackCalled = true;
        cv.notify_one();
    };
    client_.SetUserDefineDiedRecipient(deathCallback);
    ON_CALL(*mockSAMgr_, CheckSystemAbility(SUBSYS_WEBVIEW_NATIVE_MESSAGING_SERVICE_ID))
        .WillByDefault(Return(mockService_->AsObject()));
    sptr<IWebNativeMessagingService> proxy = client_.GetWebNativeMessagingProxy();
    ASSERT_NE(proxy, nullptr);
    sptr<IRemoteObject> remoteObj = proxy->AsObject();
    sptr<MockRemoteObject> mockRemoteObj = static_cast<MockRemoteObject*>(remoteObj.GetRefPtr());
    mockRemoteObj->TriggerDeath();
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait_for(lock, std::chrono::milliseconds(500), [&callbackCalled] { return callbackCalled; });
    EXPECT_FALSE(callbackCalled);
    EXPECT_EQ(client_.GetWebNativeMessaging(), nullptr);
}

} // namespace NWeb
} // namespace OHOS
