/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "nweb_log.h"
#include "ohos_adapter_helper.h"

#define private public
#include "imf_adapter_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
namespace {
std::shared_ptr<IMFAdapterImpl> g_imf = nullptr;
} // namespace

class NWebIMFAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class IMFTextListenerTest : public IMFTextListenerAdapter {
public:
    IMFTextListenerTest() = default;
    ~IMFTextListenerTest() override = default;
    void InsertText(const std::u16string &text) override
    {
        WVLOG_I("test InsertText");
        isInsertText_ = true;
    }
    void DeleteForward(int32_t length) override
    {
        WVLOG_I("test DeleteForward");
        isDeleteForward_ = true;
    }
    void DeleteBackward(int32_t length) override
    {
        WVLOG_I("test DeleteBackward");
        isDeleteBackward_ = true;
    }
    void SendKeyEventFromInputMethod() override
    {
        WVLOG_I("test SendKeyEventFromInputMethod");
        isSendKeyEventFromInputMethod_ = true;
    }
    void SendKeyboardInfo(const IMFAdapterKeyboardInfo &info) override
    {
        WVLOG_I("test SendKeyboardInfo");
        isSendKeyboardInfo_ = true;
    }
    void SetKeyboardStatus(bool status) override
    {
        WVLOG_I("test SetKeyboardStatus");
        isSetKeyboardStatus_ = true;
    }
    void MoveCursor(const IMFAdapterDirection direction) override
    {
        WVLOG_I("test MoveCursor");
        isMoveCursor_ = true;
    }
    void HandleSetSelection(int32_t start, int32_t end) override
    {
        WVLOG_I("test HandleSetSelection");
        isHandleSetSelection_ = true;
    }
    void HandleExtendAction(int32_t action) override
    {
        WVLOG_I("test HandleExtendAction");
        isHandleExtendAction_ = true;
    }
    void HandleSelect(int32_t keyCode, int32_t cursorMoveSkip) override
    {
        WVLOG_I("test HandleSelect");
        isHandleSelect_ = true;
    }
    bool VerifyAllSuccess()
    {
        return isInsertText_ && isDeleteForward_ && isDeleteBackward_ && isSendKeyEventFromInputMethod_ &&
               isSendKeyboardInfo_ && isSetKeyboardStatus_ && isMoveCursor_ && isHandleSetSelection_ &&
               isHandleExtendAction_ && isHandleSelect_;
    }

private:
    bool isInsertText_ = false;
    bool isDeleteForward_ = false;
    bool isDeleteBackward_ = false;
    bool isSendKeyEventFromInputMethod_ = false;
    bool isSendKeyboardInfo_ = false;
    bool isSetKeyboardStatus_ = false;
    bool isMoveCursor_ = false;
    bool isHandleSetSelection_ = false;
    bool isHandleExtendAction_ = false;
    bool isHandleSelect_ = false;
};

void NWebIMFAdapterTest::SetUpTestCase(void)
{
    g_imf = std::make_unique<IMFAdapterImpl>();
    ASSERT_NE(g_imf, nullptr);
}

void NWebIMFAdapterTest::TearDownTestCase(void) {}

void NWebIMFAdapterTest::SetUp(void) {}

void NWebIMFAdapterTest::TearDown(void) {}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_001.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_001, TestSize.Level1)
{
    bool res = g_imf->Attach(nullptr, false);
    EXPECT_EQ(g_imf->textListener_, nullptr);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_002.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_002, TestSize.Level1)
{
    auto listener = std::make_shared<IMFTextListenerTest>();
    bool res = g_imf->Attach(listener, true);
    EXPECT_TRUE(res);
    EXPECT_NE(g_imf->textListener_, nullptr);
    g_imf->HideTextInput();
}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_003.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_003, TestSize.Level1)
{
    auto listener = std::make_shared<IMFTextListenerTest>();
    bool res = g_imf->Attach(listener, false);
    EXPECT_TRUE(res);
    EXPECT_NE(g_imf->textListener_, nullptr);
    g_imf->ShowCurrentInput(IMFAdapterTextInputType::TEXT);
    g_imf->ShowCurrentInput(IMFAdapterTextInputType::NUMBER);
    g_imf->HideTextInput();
}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_004.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_004, TestSize.Level1)
{
    IMFAdapterCursorInfo cursorInfo;
    g_imf->OnCursorUpdate(cursorInfo);
    std::u16string text;
    g_imf->OnSelectionChange(text, 0, 0);
    g_imf->Close();
}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_005.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_005, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto listenerTest = std::make_shared<IMFTextListenerAdapterImpl>(listener);
    std::u16string text;
    MiscServices::KeyEvent event;
    MiscServices::KeyboardInfo info;
    listenerTest->InsertText(text);
    listenerTest->DeleteForward(0);
    listenerTest->DeleteBackward(0);
    listenerTest->SendKeyEventFromInputMethod(event);
    listenerTest->SendKeyboardInfo(info);
    info.SetKeyboardStatus(static_cast<int32_t>(MiscServices::KeyboardStatus::SHOW));
    listenerTest->SendKeyboardInfo(info);
    info.SetKeyboardStatus(static_cast<int32_t>(MiscServices::KeyboardStatus::HIDE));
    info.SetFunctionKey(static_cast<int32_t>(MiscServices::FunctionKey::CONFIRM));
    listenerTest->SendKeyboardInfo(info);
    listenerTest->SetKeyboardStatus(true);
    listenerTest->MoveCursor(MiscServices::Direction::NONE);
    listenerTest->MoveCursor(MiscServices::Direction::UP);
    listenerTest->MoveCursor(MiscServices::Direction::DOWN);
    listenerTest->MoveCursor(MiscServices::Direction::LEFT);
    listenerTest->MoveCursor(MiscServices::Direction::RIGHT);
    listenerTest->HandleSetSelection(0, 0);
    listenerTest->HandleExtendAction(0);
    listenerTest->HandleSelect(0, 0);
    EXPECT_EQ(listener->VerifyAllSuccess(), true);
}

/**
 * @tc.name: NWebIMFAdapterTest_InsertText_006.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_InsertText_006, TestSize.Level1)
{
    auto listenerTest = std::make_shared<IMFTextListenerAdapterImpl>(nullptr);
    EXPECT_NE(listenerTest, nullptr);
    std::u16string text;
    MiscServices::KeyEvent event;
    MiscServices::KeyboardInfo info;
    listenerTest->InsertText(text);
    listenerTest->DeleteForward(0);
    listenerTest->DeleteBackward(0);
    listenerTest->SendKeyEventFromInputMethod(event);
    listenerTest->SendKeyboardInfo(info);
    listenerTest->SetKeyboardStatus(true);
    listenerTest->MoveCursor(MiscServices::Direction::NONE);
    listenerTest->HandleSetSelection(0, 0);
    listenerTest->HandleExtendAction(0);
    listenerTest->HandleSelect(0, 0);
}
} // namespace OHOS::NWeb