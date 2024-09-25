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

#include <array>
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "mmi_adapter.h"
#include "ohos_adapter_helper.h"

#define private public
#include "mmi_adapter_impl.h"

using namespace testing;

namespace OHOS::NWeb {
namespace {
const int RESULT_OK = 0;
const int RESULT_ERROR = -1;
std::shared_ptr<MMIAdapterImpl> g_mmi;
} // namespace

class MMIDeviceInfoAdapterMock : public MMIDeviceInfoAdapter {
public:
    MMIDeviceInfoAdapterMock() = default;
    MOCK_METHOD0(GetId, int32_t());
    MOCK_METHOD0(GetType, int32_t());
    MOCK_METHOD0(GetBus, int32_t());
    MOCK_METHOD0(GetVersion, int32_t());
    MOCK_METHOD0(GetProduct, int32_t());
    MOCK_METHOD0(GetVendor, int32_t());
    MOCK_METHOD0(GetName, std::string());
    MOCK_METHOD0(GetPhys, std::string());
    MOCK_METHOD0(GetUniq, std::string());
    MOCK_METHOD1(SetId, void(int32_t));
    MOCK_METHOD1(SetType, void(int32_t));
    MOCK_METHOD1(SetBus, void(int32_t));
    MOCK_METHOD1(SetVersion, void(int32_t));
    MOCK_METHOD1(SetProduct, void(int32_t));
    MOCK_METHOD1(SetVendor, void(int32_t));
    MOCK_METHOD1(SetName, void(std::string));
    MOCK_METHOD1(SetPhys, void(std::string));
    MOCK_METHOD1(SetUniq, void(std::string));
};

class NWebMMIAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class MMIListenerTest : public MMIListenerAdapter {
public:
    MMIListenerTest() = default;
    virtual ~MMIListenerTest() = default;
    void OnDeviceAdded(int32_t deviceId, const std::string& type) override {}
    void OnDeviceRemoved(int32_t deviceId, const std::string& type) override {}
};

void NWebMMIAdapterTest::SetUpTestCase(void)
{
    g_mmi = std::make_shared<MMIAdapterImpl>();
    ASSERT_NE(g_mmi, nullptr);
}

void NWebMMIAdapterTest::TearDownTestCase(void) {}

void NWebMMIAdapterTest::SetUp(void) {}

void NWebMMIAdapterTest::TearDown(void) {}

class MockMMIInputListenerAdapter : public MMIInputListenerAdapter {
public:
    MockMMIInputListenerAdapter() = default;
    void OnInputEvent(int32_t keyCode, int32_t keyAction) {}
};

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_001.
 * @tc.desc: MMI adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5P001
 */
TEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_001)
{
    auto listener = std::make_shared<MMIListenerTest>();
    int32_t ret = g_mmi->RegisterDevListener("change", listener);
    EXPECT_EQ(ret, RESULT_OK);

    ret = g_mmi->UnregisterDevListener("change");
    EXPECT_EQ(ret, RESULT_OK);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_002.
 * @tc.desc: MMI adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5P001
 */
TEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_002)
{
    int32_t type;
    int32_t ret = g_mmi->GetKeyboardType(1, type);
    EXPECT_EQ(ret, RESULT_OK);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_003.
 * @tc.desc: MMI adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5P001
 */
TEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_003)
{
    std::vector<int32_t> devList;
    int32_t ret = g_mmi->GetDeviceIds(devList);
    EXPECT_EQ(ret, RESULT_OK);

    std::shared_ptr<MMIDeviceInfoAdapterMock> info = std::make_shared<MMIDeviceInfoAdapterMock>();
    EXPECT_NE(info, nullptr);
    ret = g_mmi->GetDeviceInfo(0, info);
    EXPECT_EQ(ret, RESULT_OK);
    ret = g_mmi->GetDeviceInfo(0, nullptr);
    EXPECT_NE(ret, RESULT_OK);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_004.
 * @tc.desc: MMI adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5OZZ8
 */
TEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_004)
{
    auto listener = std::make_shared<MMIListenerTest>();
    int32_t ret = g_mmi->RegisterDevListener("change", nullptr);
    EXPECT_EQ(ret, RESULT_ERROR);
}
} // namespace OHOS::NWeb
