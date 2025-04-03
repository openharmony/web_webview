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
#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>
#include <mutex>

#define private public
#include "ability_manager_client.h"
#include "iservice_registry.h"
#include "migration_manager_adapter_impl.h"
#include "system_ability.h"
#include "system_ability_definition.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {

class MigrationListenerAdapterTest : public MigrationListenerAdapter {
public:
    MigrationListenerAdapterTest() = default;

    virtual ~MigrationListenerAdapterTest() = default;

    void OnMigrationReply(int32_t errorCode, int32_t succussCount, const std::vector<int32_t>& errorIndex,
        const std::vector<int32_t>& codeList) override
    {}
};

class MigrationManagerAdapterlmplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void MigrationManagerAdapterlmplTest::SetUpTestCase(void) {}

void MigrationManagerAdapterlmplTest::TearDownTestCase(void) {}

void MigrationManagerAdapterlmplTest::SetUp(void) {}

void MigrationManagerAdapterlmplTest::TearDown(void) {}

/**
 * @tc.name: MigrationManagerAdapterlmplTest_001
 * @tc.desc: MigrationListenAdapter Func
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(MigrationManagerAdapterlmplTest, MigrationManagerAdapterlmplTest_001, TestSize.Level1)
{
    std::shared_ptr<MigrationListenerAdapter> listener = std::make_shared<MigrationListenerAdapterTest>();
    std::unique_ptr<MigrationManagerAdapterImpl::MigrationListenerAdapterImpl> migrationListener =
        std::make_unique<MigrationManagerAdapterImpl::MigrationListenerAdapterImpl>(listener);
    std::vector<int32_t> vec(0);
    migrationListener->SetJsonData("JsonTest");
    migrationListener->SetInterfaceToken("InterfaceTokenTest");
    migrationListener->OnMigrationReply(0, 0, vec, vec);
    EXPECT_NE(migrationListener, nullptr);
}

/**
 * @tc.name: MigrationManagerAdapterlmplTest_002
 * @tc.desc: MigrationManagerAdapterlmpl SendMigrationRequest.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MigrationManagerAdapterlmplTest, MigrationManagerAdapterlmplTest_002, TestSize.Level1)
{
    std::shared_ptr<MigrationManagerAdapterImpl> migrationManagermpl = std::make_shared<MigrationManagerAdapterImpl>();
    std::shared_ptr<MigrationListenerAdapter> listener = std::make_shared<MigrationListenerAdapterTest>();
    EXPECT_NE(migrationManagermpl, nullptr);
    EXPECT_NE(listener, nullptr);
    EXPECT_EQ(migrationManagermpl->RegisterMigrationListener(listener), 0);
    migrationManagermpl->SetMigrationParam("", "", "");
    EXPECT_FALSE(migrationManagermpl->SendMigrationRequest(std::make_shared<std::string>("")));
}

/**
 *
 * @tc.name: MigrationManagerAdapterlmplTest_003
 * @tc.desc: MigrationManagerAdapterlmpl OnAbilityConnectDone
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(MigrationManagerAdapterlmplTest, MigrationManagerAdapterlmplTest_003, TestSize.Level1)
{
    std::shared_ptr<MigrationManagerAdapterImpl> migrationManagermpl = std::make_shared<MigrationManagerAdapterImpl>();
    std::shared_ptr<MigrationListenerAdapter> listener = std::make_shared<MigrationListenerAdapterTest>();
    std::unique_ptr<MigrationManagerAdapterImpl::MigrationListenerAdapterImpl> migrationListener =
        std::make_unique<MigrationManagerAdapterImpl::MigrationListenerAdapterImpl>(listener);
    EXPECT_EQ(migrationManagermpl->RegisterMigrationListener(listener), 0);

    const AppExecFwk::ElementName elementName("aaa", "bbb", "ccc");
    OHOS::sptr<OHOS::IRemoteObject> sp(nullptr);
    migrationListener->OnAbilityConnectDone(elementName, sp, 0);

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sp = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    EXPECT_NE(sp, nullptr);
    migrationListener->OnAbilityConnectDone(elementName, sp, 0);
    EXPECT_TRUE(migrationManagermpl->SendMigrationRequest(std::make_shared<std::string>("")));

    migrationListener->OnAbilityDisconnectDone(elementName, 0);
    EXPECT_FALSE(migrationManagermpl->SendMigrationRequest(std::make_shared<std::string>("")));
}

} // namespace NWeb
} // namespace OHOS