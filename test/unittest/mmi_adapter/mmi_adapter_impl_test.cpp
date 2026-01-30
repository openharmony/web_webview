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

#include <array>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "key_event.h"
#include "mmi_adapter.h"
#include "ohos_adapter_helper.h"

#define private public
#include "mmi_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

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
    void OnDeviceAdded(int32_t deviceId, const std::string& type) override {};
    void OnDeviceRemoved(int32_t deviceId, const std::string& type) override {};
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
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_001, TestSize.Level1)
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
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_002, TestSize.Level1)
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
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_003, TestSize.Level1)
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
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_004, TestSize.Level1)
{
    auto listener = std::make_shared<MMIListenerTest>();
    EXPECT_NE(listener, nullptr);
    auto listenerTest = std::make_shared<MMIListenerAdapterImpl>(listener);
    EXPECT_NE(listenerTest, nullptr);
    listenerTest->OnDeviceAdded(1, "add");
    listenerTest->OnDeviceRemoved(1, "remove");
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_005.
 * @tc.desc: MMI adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5OZZ8
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_005, TestSize.Level1)
{
    auto listener = std::make_shared<MMIListenerTest>();
    int32_t ret = g_mmi->RegisterDevListener("change", nullptr);
    EXPECT_EQ(ret, RESULT_ERROR);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_006.
 * @tc.desc: MMI adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:I5OZZ8
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_006, TestSize.Level1)
{
    auto mmi_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(mmi_adapter, nullptr);
    auto listener = std::make_shared<MMIListenerTest>();
    auto listenerTest = std::make_shared<MMIListenerAdapterImpl>(listener);
    listenerTest->listener_ = nullptr;
    listenerTest->OnDeviceAdded(1, "add");
    listenerTest->OnDeviceRemoved(1, "remove");

    const char* code = g_mmi->KeyCodeToString(MMI::KeyEvent::KEYCODE_UNKNOWN);
    EXPECT_NE(code, nullptr);
    int32_t result = g_mmi->RegisterMMIInputListener(nullptr);
    EXPECT_EQ(result, -1);
    std::shared_ptr<MMIInputListenerAdapter> eventCallback = std::make_shared<MockMMIInputListenerAdapter>();
    result = g_mmi->RegisterMMIInputListener(std::move(eventCallback));
    EXPECT_NE(result, -1);
    g_mmi->UnregisterMMIInputListener(MMI::KeyEvent::KEYCODE_UNKNOWN);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_007.
 * @tc.desc: MMIInputListenerAdapterImpl.
 * @tc.type: FUNC.
 * @tc.require:I5OZZ8
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIInputListenerAdapterImpl_007, TestSize.Level1)
{
    std::shared_ptr<MMIInputListenerAdapter> listener = std::make_shared<MockMMIInputListenerAdapter>();
    MMIInputListenerAdapterImpl listenerAdapter(listener);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    std::shared_ptr<MMI::AxisEvent> axisEvent = nullptr;
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    listenerAdapter.OnInputEvent(keyEvent);
    listenerAdapter.OnInputEvent(pointerEvent);
    listenerAdapter.OnInputEvent(axisEvent);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    listenerAdapter.OnInputEvent(keyEvent);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_CANCEL);
    listenerAdapter.OnInputEvent(keyEvent);

    MMIInputListenerAdapterImpl listenerAdapterImpl(nullptr);
    listenerAdapterImpl.OnInputEvent(keyEvent);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_008.
 * @tc.desc: MMI adapter GetMaxTouchPoints unittest.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_008, TestSize.Level1)
{
    int32_t ret = g_mmi->GetMaxTouchPoints();
    EXPECT_GE(ret, 0);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_009.
 * @tc.desc: MMI adapter GetDeviceInfo with invalid deviceId.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_009, TestSize.Level1)
{
    std::shared_ptr<MMIDeviceInfoAdapterMock> info = std::make_shared<MMIDeviceInfoAdapterMock>();
    EXPECT_NE(info, nullptr);

    int32_t ret = g_mmi->GetDeviceInfo(99999, info);
    EXPECT_TRUE(ret == RESULT_OK || ret == RESULT_ERROR);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIListenerAdapterImpl_010.
 * @tc.desc: MMIListenerAdapterImpl destructor and null listener test.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIListenerAdapterImpl_010, TestSize.Level1)
{
    auto listenerTest = std::make_shared<MMIListenerAdapterImpl>(nullptr);
    EXPECT_NE(listenerTest, nullptr);

    listenerTest->OnDeviceAdded(1, "add");
    listenerTest->OnDeviceRemoved(1, "remove");

    listenerTest.reset();
    EXPECT_EQ(listenerTest, nullptr);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIInputListenerAdapterImpl_011.
 * @tc.desc: MMIInputListenerAdapterImpl destructor and null listener test.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIInputListenerAdapterImpl_011, TestSize.Level1)
{
    auto listenerAdapterImpl = std::make_shared<MMIInputListenerAdapterImpl>(nullptr);
    EXPECT_NE(listenerAdapterImpl, nullptr);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);

    listenerAdapterImpl->OnInputEvent(keyEvent);

    listenerAdapterImpl.reset();
    EXPECT_EQ(listenerAdapterImpl, nullptr);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIInputListenerAdapterImpl_012.
 * @tc.desc: MMIInputListenerAdapterImpl OnInputEvent with valid listener.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIInputListenerAdapterImpl_012, TestSize.Level1)
{
    auto listener = std::make_shared<MockMMIInputListenerAdapter>();
    MMIInputListenerAdapterImpl listenerAdapter(listener);

    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_A);
    listenerAdapter.OnInputEvent(keyEvent);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_B);
    listenerAdapter.OnInputEvent(keyEvent);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_CANCEL);
    listenerAdapter.OnInputEvent(keyEvent);

    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UNKNOWN);
    listenerAdapter.OnInputEvent(keyEvent);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_013.
 * @tc.desc: MMI adapter KeyCodeToString with various key codes.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_013, TestSize.Level1)
{
    const char* code1 = g_mmi->KeyCodeToString(MMI::KeyEvent::KEYCODE_A);
    EXPECT_NE(code1, nullptr);

    const char* code2 = g_mmi->KeyCodeToString(MMI::KeyEvent::KEYCODE_0);
    EXPECT_NE(code2, nullptr);

    const char* code3 = g_mmi->KeyCodeToString(MMI::KeyEvent::KEYCODE_ENTER);
    EXPECT_NE(code3, nullptr);

    const char* code4 = g_mmi->KeyCodeToString(MMI::KeyEvent::KEYCODE_BACK);
    EXPECT_NE(code4, nullptr);

    const char* code5 = g_mmi->KeyCodeToString(999999);
    EXPECT_NE(code5, nullptr);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_014.
 * @tc.desc: MMI adapter GetKeyboardType with different device IDs.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_014, TestSize.Level1)
{
    int32_t type;
    int32_t ret1 = g_mmi->GetKeyboardType(0, type);
    EXPECT_EQ(ret1, RESULT_OK);

    int32_t ret2 = g_mmi->GetKeyboardType(1, type);
    EXPECT_EQ(ret2, RESULT_OK);

    int32_t ret3 = g_mmi->GetKeyboardType(-1, type);
    EXPECT_TRUE(ret3 == RESULT_OK || ret3 == RESULT_ERROR);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_015.
 * @tc.desc: MMI adapter GetDeviceIds and iterate.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_015, TestSize.Level1)
{
    std::vector<int32_t> ids;
    int32_t ret = g_mmi->GetDeviceIds(ids);
    EXPECT_EQ(ret, RESULT_OK);

    for (const auto& id : ids) {
        EXPECT_GE(id, 0);
    }
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_016.
 * @tc.desc: MMI adapter RegisterMMIInputListener flow.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_016, TestSize.Level1)
{
    std::shared_ptr<MMIInputListenerAdapter> listener = std::make_shared<MockMMIInputListenerAdapter>();

    int32_t monitorId = g_mmi->RegisterMMIInputListener(listener);
    EXPECT_NE(monitorId, -1);

    g_mmi->UnregisterMMIInputListener(monitorId);

    std::shared_ptr<MMIInputListenerAdapter> listener2 = std::make_shared<MockMMIInputListenerAdapter>();
    int32_t monitorId2 = g_mmi->RegisterMMIInputListener(listener2);
    EXPECT_NE(monitorId2, -1);

    g_mmi->UnregisterMMIInputListener(monitorId2);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_017.
 * @tc.desc: MMI adapter GetDeviceInfo and verify all fields.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_017, TestSize.Level1)
{
    std::shared_ptr<MMIDeviceInfoAdapterMock> info = std::make_shared<MMIDeviceInfoAdapterMock>();
    EXPECT_NE(info, nullptr);

    EXPECT_CALL(*info, SetId(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetType(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetBus(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetVersion(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetProduct(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetVendor(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetName(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetPhys(_)).Times(AtLeast(0));
    EXPECT_CALL(*info, SetUniq(_)).Times(AtLeast(0));

    int32_t ret = g_mmi->GetDeviceInfo(0, info);
    EXPECT_EQ(ret, RESULT_OK);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_018.
 * @tc.desc: MMI adapter RegisterDevListener with different types.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_018, TestSize.Level1)
{
    auto listener = std::make_shared<MMIListenerTest>();

    int32_t ret1 = g_mmi->RegisterDevListener("change", listener);
    EXPECT_EQ(ret1, RESULT_OK);
    g_mmi->UnregisterDevListener("change");

    int32_t ret2 = g_mmi->RegisterDevListener("all", listener);
    EXPECT_TRUE(ret2 == RESULT_OK || ret2 == RESULT_ERROR);
    if (ret2 == RESULT_OK) {
        g_mmi->UnregisterDevListener("all");
    }
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_019.
 * @tc.desc: MMI adapter multiple listener registration and unregistration.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_019, TestSize.Level1)
{
    auto listener1 = std::make_shared<MMIListenerTest>();
    auto listener2 = std::make_shared<MMIListenerTest>();

    int32_t ret1 = g_mmi->RegisterDevListener("change", listener1);
    EXPECT_EQ(ret1, RESULT_OK);

    int32_t ret2 = g_mmi->RegisterDevListener("change", listener2);
    EXPECT_EQ(ret2, RESULT_OK);

    g_mmi->UnregisterDevListener("change");
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_020.
 * @tc.desc: MMI adapter OnInputEvent with null KeyEvent.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_020, TestSize.Level1)
{
    auto listener = std::make_shared<MockMMIInputListenerAdapter>();
    MMIInputListenerAdapterImpl listenerAdapter(listener);

    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    listenerAdapter.OnInputEvent(keyEvent);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_021.
 * @tc.desc: MMI adapter GetMaxTouchPoints multiple calls.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_021, TestSize.Level1)
{
    int32_t ret1 = g_mmi->GetMaxTouchPoints();
    EXPECT_GE(ret1, 0);

    int32_t ret2 = g_mmi->GetMaxTouchPoints();
    EXPECT_GE(ret2, 0);

    EXPECT_EQ(ret1, ret2);
}

/**
 * @tc.name: NWebMMIAdapterTest_MMIAdapterImpl_022.
 * @tc.desc: MMI listener OnDeviceAdded and OnDeviceRemoved with valid listener.
 * @tc.type: FUNC.
 * @tc.require:IC5TR0
 */
HWTEST_F(NWebMMIAdapterTest, NWebMMIAdapterTest_MMIAdapterImpl_022, TestSize.Level1)
{
    class TestMMIListener : public MMIListenerAdapter {
    public:
        int32_t lastDeviceId = -1;
        std::string lastType;
        int addedCount = 0;
        int removedCount = 0;

        void OnDeviceAdded(int32_t deviceId, const std::string& type) override
        {
            lastDeviceId = deviceId;
            lastType = type;
            addedCount++;
        }

        void OnDeviceRemoved(int32_t deviceId, const std::string& type) override
        {
            lastDeviceId = deviceId;
            lastType = type;
            removedCount++;
        }
    };

    auto testListener = std::make_shared<TestMMIListener>();
    auto listenerAdapter = std::make_shared<MMIListenerAdapterImpl>(testListener);

    listenerAdapter->OnDeviceAdded(100, "test_type");
    EXPECT_EQ(testListener->lastDeviceId, 100);
    EXPECT_EQ(testListener->lastType, "test_type");
    EXPECT_EQ(testListener->addedCount, 1);

    listenerAdapter->OnDeviceRemoved(200, "remove_type");
    EXPECT_EQ(testListener->lastDeviceId, 200);
    EXPECT_EQ(testListener->lastType, "remove_type");
    EXPECT_EQ(testListener->removedCount, 1);
}
} // namespace OHOS::NWeb
