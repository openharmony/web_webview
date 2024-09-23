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

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "nweb_log.h"
#include "ohos_adapter_helper.h"

#define private public
#include "event_handler_adapter_impl.h"

using namespace OHOS;

namespace OHOS::NWeb {

class EventHandlerAdapterImplTest : public testing::Test {
public:
    EventHandlerAdapterImplTest() {}

    ~EventHandlerAdapterImplTest() override {}

    void SetUp() override {}

    void TearDown() override {}
};

class EventHandlerFDListenerTest : public EventHandlerFDListenerAdapter {
public:
    EventHandlerFDListenerTest() = default;
    ~EventHandlerFDListenerTest() override = default;
    void OnReadable(int32_t fileDescriptor) override
    {
        WVLOG_I("test OnReadable");
        isReadable_ = true;
    }
    bool VerifySuccess()
    {
        return isReadable_;
    }

private:
    bool isReadable_ = false;
};

/**
 * @tc.name: EventHandlerAdapterImplTest_EventHandlerAdapterImpl_001.
 * @tc.desc: EventHandlerAdapter adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(EventHandlerAdapterImplTest, EventHandlerAdapterImplTest_EventHandlerAdapterImpl_001)
{
    int32_t fd = -1;
    auto eventHandlerTest = std::make_unique<EventHandlerAdapterImpl>();
    ASSERT_NE(eventHandlerTest, nullptr);
    EXPECT_NE(eventHandlerTest->loop_, nullptr);
    bool res = eventHandlerTest->AddFileDescriptorListener(fd, EventHandlerAdapter::INPUT_EVENT, nullptr);
    EXPECT_FALSE(res);

    auto listener = std::make_shared<EventHandlerFDListenerTest>();
    res = eventHandlerTest->AddFileDescriptorListener(fd, EventHandlerAdapter::INPUT_EVENT, listener);
    EXPECT_FALSE(res);
    eventHandlerTest->RemoveFileDescriptorListener(fd);

    EXPECT_FALSE(listener->VerifySuccess());
}

/**
 * @tc.name: EventHandlerAdapterImplTest_EventHandlerAdapterImpl_002.
 * @tc.desc: EventHandlerAdapter adapter unittest.
 * @tc.type: FUNC.
 */
TEST_F(EventHandlerAdapterImplTest, EventHandlerAdapterImplTest_EventHandlerAdapterImpl_002)
{
    int32_t fd = -1;
    auto eventHandlerTest = std::make_unique<EventHandlerAdapterImpl>();
    ASSERT_NE(eventHandlerTest, nullptr);
    ASSERT_NE(eventHandlerTest->loop_, nullptr);

    auto listener = std::make_shared<EventHandlerFDListenerTest>();
    bool res = eventHandlerTest->AddFileDescriptorListener(fd, EventHandlerAdapter::INPUT_EVENT, listener);
    EXPECT_FALSE(res);
    eventHandlerTest->RemoveFileDescriptorListener(fd);

    auto listenerTest = std::make_shared<EventHandlerFDListenerAdapterImpl>(listener, fd);
    listenerTest->GetFDListener()->OnReadable(fd);
    EXPECT_TRUE(listener->VerifySuccess());
}
} // namespace OHOS::NWeb
