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

#include <cstring>
#include <gtest/gtest.h>
#include <securec.h>
#include <ui/rs_surface_node.h>
#include <unordered_map>

#include "key_event.h"
#include "nweb_create_window.h"

#define private public
#include "nweb_input_event_consumer.h"
#include "nweb_helper.h"
#undef private

#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "pointer_event.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<NWeb> g_nweb;
std::shared_ptr<NWebInputEventConsumer> g_input;
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
} // namespace

class NWebInputEventTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NWebInputEventTest::SetUpTestCase(void)
{
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_TRUE(result);
}

void NWebInputEventTest::TearDownTestCase(void)
{}

void NWebInputEventTest::SetUp(void)
{}

void NWebInputEventTest::TearDown(void)
{}

/**
 * @tc.name: NWebInputEvent_NWebInputEventConsumer_001.
 * @tc.desc: Test the NWebInputEventConsumer.
 * @tc.type: FUNC
 * @tc.require:issueI5NXG9
 */
HWTEST_F(NWebInputEventTest, NWebInputEvent_NWebInputEventConsumer_001, TestSize.Level1)
{
    g_input = std::make_shared<NWebInputEventConsumer>(g_nweb);
    EXPECT_NE(g_input, nullptr);
}

/**
 * @tc.name: NWebInputEvent_OnInputEvent_002.
 * @tc.desc: Test the OnInputEvent.
 * @tc.type: FUNC
 * @tc.require:issueI5OURV
 */
HWTEST_F(NWebInputEventTest, NWebInputEvent_OnInputEvent_002, TestSize.Level1)
{
    const int32_t POINTER_EVENT = 11;
    const int32_t POINTERID = 1;
    bool result;
    PointerEvent::PointerItem pointerItem;
    int32_t keyCode = MMI::KeyEvent::KEYCODE_BACK;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    EXPECT_NE(keyEvent, nullptr);
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    EXPECT_NE(event, nullptr);
    result = g_input->OnInputEvent(event);
    EXPECT_TRUE(result);
    result = g_input->OnInputEvent(keyEvent);
    EXPECT_TRUE(result);

    std::shared_ptr<NWeb> mock = std::make_shared<NWebMock>();
    EXPECT_NE(mock, nullptr);
    std::shared_ptr<NWebInputEventConsumer> input = std::make_shared<NWebInputEventConsumer>(mock);
    EXPECT_NE(input, nullptr);
    result = input->OnInputEvent(event);
    EXPECT_TRUE(result);
    pointerItem.SetPointerId(POINTERID);
    event->SetPointerId(POINTERID);
    event->AddPointerItem(pointerItem);
    for (int32_t i = 0; i <= POINTER_EVENT; i++) {
        event->SetPointerAction(i);
        result = g_input->OnInputEvent(event);
        EXPECT_TRUE(result);
    }

    result = input->OnInputEvent(keyEvent);
    EXPECT_TRUE(result);
    std::shared_ptr<MMI::AxisEvent> axisevent = MMI::AxisEvent::Create();
    result = g_input->OnInputEvent(axisevent);
    EXPECT_FALSE(result);
    keyEvent->SetKeyCode(keyCode);
    result = input->OnInputEvent(keyEvent);
    EXPECT_TRUE(result);

    keyEvent = nullptr;
    event = nullptr;
    input->DispatchKeyEvent(keyEvent);
    input->DispatchPointerEvent(event);
    result = input->OnInputEvent(event);
    EXPECT_FALSE(result);
    result = input->OnInputEvent(keyEvent);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NWebInputEvent_CreateNWeb_003.
 * @tc.desc: Test the CreateNWeb.
 * @tc.type: FUNC
 * @tc.require:issueI5NXG9
 */
HWTEST_F(NWebInputEventTest, NWebInputEvent_CreateNWeb_003, TestSize.Level1)
{
    sptr<OHOS::Rosen::Window> window = CreateWindow();
    EXPECT_NE(window, nullptr);
    NWebHelper::Instance().libHandleWebEngine_ = nullptr;
    g_nweb = NWebAdapterHelper::Instance().CreateNWeb(window.GetRefPtr(), GetInitArgs());
    EXPECT_EQ(g_nweb, nullptr);
}
}
