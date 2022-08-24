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
#include "nweb_input_event_consumer.h"
#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;

namespace OHOS::NWeb {
namespace {
const bool RESULT_OK = true;
const bool RESULT_FAIL = false;
const int32_t POINTER_EVENT = 12;
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
    bool result = false;
    NWebHelper::Instance().SetBundlePath(MOCK_INSTALLATION_DIR);
    if (!NWebAdapterHelper::Instance().Init(false)) {
        return;
    }
    sptr<OHOS::Rosen::Window> window = CreateWindow();
    if (window == nullptr) {
        return;
    }
    g_nweb = NWebAdapterHelper::Instance().CreateNWeb(window.GetRefPtr(), GetInitArgs());
    if (g_nweb != nullptr) {
        result = true;
    }
    EXPECT_EQ(RESULT_FAIL, result);
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
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebInputEventTest, NWebInputEvent_NWebInputEventConsumer_001, TestSize.Level1)
{
    bool result = false;
    g_input = std::make_shared<NWebInputEventConsumer>(g_nweb);
    if (g_input != nullptr) {
        result = true;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebInputEvent_OnInputEvent_002.
 * @tc.desc: Test the OnInputEvent.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebInputEventTest, NWebInputEvent_OnInputEvent_002, TestSize.Level1)
{
    bool result;
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    for (int32_t i = 0; i <= POINTER_EVENT; i++) {
        event->SetPointerAction(i);
        result = g_input->OnInputEvent(event);
        EXPECT_EQ(RESULT_OK, result);
    }

    std::shared_ptr<MMI::KeyEvent> tmp = MMI::KeyEvent::Create();
    result = g_input->OnInputEvent(tmp);
    EXPECT_EQ(RESULT_OK, result);

    std::shared_ptr<MMI::AxisEvent> axisevent = MMI::AxisEvent::Create();
    result = g_input->OnInputEvent(axisevent);
    EXPECT_EQ(RESULT_FAIL, result);
}

/**
 * @tc.name: NWebInputEvent_OnInputEvent_003.
 * @tc.desc: Test the OnInputEvent unusually.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebInputEventTest, NWebInputEvent_OnInputEvent_003, TestSize.Level1)
{
    bool result;
    int32_t reset = -1;
    int32_t keyCode = 2;
    std::shared_ptr<MMI::PointerEvent> event = MMI::PointerEvent::Create();
    event->SetPointerId(reset);
    result = g_input->OnInputEvent(event);
    EXPECT_EQ(RESULT_OK, result);

    event = nullptr;
    result = g_input->OnInputEvent(event);
    EXPECT_EQ(RESULT_OK, result);

    std::shared_ptr<MMI::KeyEvent> tmp = MMI::KeyEvent::Create();
    tmp->SetKeyCode(keyCode);
    result = g_input->OnInputEvent(tmp);
    EXPECT_EQ(RESULT_OK, result);

    keyCode = 0;
    tmp->SetKeyCode(keyCode);
    result = g_input->OnInputEvent(tmp);
    EXPECT_EQ(RESULT_OK, result);

    tmp = nullptr;
    result = g_input->OnInputEvent(tmp);
    EXPECT_EQ(RESULT_OK, result);
}
}