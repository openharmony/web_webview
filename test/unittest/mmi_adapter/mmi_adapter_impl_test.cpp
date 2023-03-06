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

#include "ohos_adapter_helper.h"
#include "mmi_adapter.h"
#include "key_event.h"

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
    void OnDeviceAdded(int32_t deviceId, const std::string &type) override {};
    void OnDeviceRemoved(int32_t deviceId, const std::string &type) override {};
};

void NWebMMIAdapterTest::SetUpTestCase(void)
{
    g_mmi = std::make_shared<MMIAdapterImpl>();
    ASSERT_NE(g_mmi, nullptr);
}

void NWebMMIAdapterTest::TearDownTestCase(void) {}

void NWebMMIAdapterTest::SetUp(void) {}

void NWebMMIAdapterTest::TearDown(void) {}

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
    auto callback = [](int32_t type) {};
    int32_t ret = g_mmi->GetKeyboardType(1, callback);
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
    auto callback = [](std::vector<int32_t>& devList) {};
    int32_t ret = g_mmi->GetDeviceIds(callback);
    EXPECT_EQ(ret, RESULT_OK);
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
    auto listenerTest = std::make_shared<MMIListenerAdapterImpl>(listener);
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

    const char *code = g_mmi->KeyCodeToString(MMI::KeyEvent::KEYCODE_UNKNOWN);
    EXPECT_NE(code, nullptr);
    int32_t result = g_mmi->RegisterMMIInputListener(nullptr);
    EXPECT_EQ(result, -1);
    InputEventCallback eventCallback = [](int32_t, int32_t) {};
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
    InputEventCallback listener = [](int32_t, int32_t) {};
    MMIInputListenerAdapterImpl listenerAdapter(listener);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
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
}  // namespace OHOS::NWeb