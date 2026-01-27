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

class IMFCursorInfoAdapterMock : public IMFCursorInfoAdapter {
public:
    IMFCursorInfoAdapterMock() = default;

    ~IMFCursorInfoAdapterMock() override = default;

    double GetLeft() override
    {
        return left_;
    }

    double GetTop() override
    {
        return top_;
    }

    double GetWidth() override
    {
        return width_;
    }

    double GetHeight() override
    {
        return height_;
    }

    void SetLeft(double left)
    {
        left_ = left;
    }

    void SetTop(double top)
    {
        top_ = top;
    }

    void SetWidth(double width)
    {
        width_ = width;
    }

    void SetHeight(double height)
    {
        height_ = height;
    }

private:
    double left_ = 0.0;
    double top_ = 0.0;
    double width_ = 0.0;
    double height_  = 0.0;
};

class IMFInputAttributeAdapterMock : public IMFInputAttributeAdapter {
public:
    IMFInputAttributeAdapterMock() = default;

    ~IMFInputAttributeAdapterMock() override = default;

    int32_t GetInputPattern() override
    {
        return 0;
    }

    int32_t GetEnterKeyType() override
    {
        return 0;
    }
};

class IMFSelectionRangeAdapterMock : public IMFSelectionRangeAdapter {
public:
    IMFSelectionRangeAdapterMock() = default;

    ~IMFSelectionRangeAdapterMock() override = default;

    int32_t GetStart() override
    {
        return 0;
    }

    int32_t GetEnd() override
    {
        return 0;
    }
};

class IMFTextConfigTest : public IMFTextConfigAdapter {
public:
    IMFTextConfigTest() = default;
    ~IMFTextConfigTest() override = default;

    std::shared_ptr<IMFInputAttributeAdapter> GetInputAttribute()
    {
        auto mock = std::make_shared<IMFInputAttributeAdapterMock>();
        return mock;
    }

    std::shared_ptr<IMFCursorInfoAdapter> GetCursorInfo()
    {
        auto mock = std::make_shared<IMFCursorInfoAdapterMock>();
        return mock;
    }

    std::shared_ptr<IMFSelectionRangeAdapter> GetSelectionRange()
    {
        auto mock = std::make_shared<IMFSelectionRangeAdapterMock>();
        return mock;
    }

    uint32_t GetWindowId()
    {
        WVLOG_I("test GetWindowId");
        return 0;
    }

    double GetPositionY()
    {
        WVLOG_I("test GetPositionY");
        return 0.0;
    }

    double GetHeight()
    {
        WVLOG_I("test GetHeight");
        return 0.0;
    }
};

class IMFTextListenerTest : public IMFTextListenerAdapter {
public:
    IMFTextListenerTest() = default;
    ~IMFTextListenerTest() override = default;
    void InsertText(const std::u16string& text) override
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
    void SendKeyboardStatus(const IMFAdapterKeyboardStatus& adapterKeyboardStatus) override
    {
        WVLOG_I("test SendKeyboardStatus");
        isSendKeyboardStatus_ = true;
    }
    void SendFunctionKey(std::shared_ptr<IMFAdapterFunctionKeyAdapter> functionKey) override
    {
        WVLOG_I("test SendFunctionKey");
        isSendFunctionKey_ = true;
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
    int32_t GetTextIndexAtCursor() override
    {
        WVLOG_I("test GetTextIndexAtCursor");
        isGetTextIndexAtCursor_ = true;
        return 0;
    }
    std::u16string GetLeftTextOfCursor(int32_t number) override
    {
        WVLOG_I("test GetLeftTextOfCursor");
        isGetLeftTextOfCursor_ = true;
        return u"";
    }
    std::u16string GetRightTextOfCursor(int32_t number) override
    {
        WVLOG_I("test GetRightTextOfCursor");
        isGetRightTextOfCursor_ = true;
        return u"";
    }
    int32_t SetPreviewText(const std::u16string& text, int32_t start, int32_t end) override
    {
        WVLOG_I("test SetPreviewText");
        isSetPreviewText_ = true;
        return 0;
    }
    void FinishTextPreview() override
    {
        WVLOG_I("test FinishTextPreview");
        isFinishTextPreview_ = true;
    }
    void SetNeedUnderLine(bool isNeedUnderline) override
    {
        WVLOG_I("test SetNeedUnderLine");
        isSetNeedUnderLine_ = true;
    }
    void KeyboardUpperRightCornerHide() override
    {
        WVLOG_I("test KeyboardUpperRightCornerHide");
        isKeyboardUpperRightCornerHide_ = true;
    }
    void WebSetImeShow(bool visible) override
    {
        WVLOG_I("test WebSetImeShow");
        isWebSetImeShow_ = true;
    }
    bool VerifyAllSuccess()
    {
        return isInsertText_ && isDeleteForward_ && isDeleteBackward_ && isSendKeyEventFromInputMethod_ &&
               isSendKeyboardStatus_ && isSendFunctionKey_ && isSetKeyboardStatus_ && isMoveCursor_ &&
               isHandleSetSelection_ && isHandleExtendAction_ && isHandleSelect_ && isGetTextIndexAtCursor_ &&
               isGetLeftTextOfCursor_ && isGetRightTextOfCursor_ && isSetPreviewText_ && isFinishTextPreview_ &&
               isSetNeedUnderLine_ && isKeyboardUpperRightCornerHide_ && isWebSetImeShow_;
    }
    bool VerifyFunctionKeySuccess()
    {
        return isSendFunctionKey_;
    }
private:
    bool isInsertText_ = false;
    bool isDeleteForward_ = false;
    bool isDeleteBackward_ = false;
    bool isSendKeyEventFromInputMethod_ = false;
    bool isSendKeyboardStatus_ = false;
    bool isSendFunctionKey_ = false;
    bool isSetKeyboardStatus_ = false;
    bool isMoveCursor_ = false;
    bool isHandleSetSelection_ = false;
    bool isHandleExtendAction_ = false;
    bool isHandleSelect_ = false;
    bool isGetTextIndexAtCursor_ = false;
    bool isGetLeftTextOfCursor_ = false;
    bool isGetRightTextOfCursor_ = false;
    bool isSetPreviewText_ = false;
    bool isFinishTextPreview_ = false;
    bool isSetNeedUnderLine_ = false;
    bool isKeyboardUpperRightCornerHide_ = false;
    bool isWebSetImeShow_ = false;
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
    EXPECT_FALSE(res);
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
    EXPECT_FALSE(res);
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
    EXPECT_NE(g_imf, nullptr);
    g_imf->OnCursorUpdate(nullptr);
    std::u16string text;
    g_imf->OnSelectionChange(text, 0, 0);
    g_imf->Close();
    auto mock = std::make_shared<IMFCursorInfoAdapterMock>();
    g_imf->OnCursorUpdate(mock);
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
    MiscServices::FunctionKey functionKey;
    listenerTest->InsertText(text);
    listenerTest->DeleteForward(0);
    listenerTest->DeleteBackward(0);
    listenerTest->SendKeyEventFromInputMethod(event);
    listenerTest->SendKeyboardStatus(MiscServices::KeyboardStatus::SHOW);
    listenerTest->SendFunctionKey(functionKey);
    listenerTest->SetKeyboardStatus(true);

    listenerTest->MoveCursor(MiscServices::Direction::NONE);
    listenerTest->MoveCursor(MiscServices::Direction::UP);
    listenerTest->MoveCursor(MiscServices::Direction::DOWN);
    listenerTest->MoveCursor(MiscServices::Direction::LEFT);
    listenerTest->MoveCursor(MiscServices::Direction::RIGHT);
    listenerTest->HandleSetSelection(0, 0);
    listenerTest->HandleExtendAction(0);
    listenerTest->HandleSelect(0, 0);
    listenerTest->GetTextIndexAtCursor();
    listenerTest->GetLeftTextOfCursor(0);
    listenerTest->GetRightTextOfCursor(0);
    MiscServices::Range range { 0, 0 };
    listenerTest->SetPreviewText(text, range);
    listenerTest->FinishTextPreview();
    const std::unordered_map<std::string, MiscServices::PrivateDataValue> privateCommand;
    listenerTest->ReceivePrivateCommand(privateCommand);
}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_006.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_006, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto listenerTest = std::make_shared<IMFTextListenerAdapterImpl>(listener);
    MiscServices::FunctionKey functionKey;
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::UNSPECIFIED);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::NONE);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::GO);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::SEARCH);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::SEND);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::NEXT);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::DONE);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::PREVIOUS);
    listenerTest->SendFunctionKey(functionKey);
    functionKey.SetEnterKeyType(MiscServices::EnterKeyType::NEW_LINE);
    listenerTest->SendFunctionKey(functionKey);
    EXPECT_EQ(listener->VerifyFunctionKeySuccess(), true);
}

/**
 * @tc.name: NWebIMFAdapterTest_InsertText_007.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_InsertText_007, TestSize.Level1)
{
    auto listenerTest = std::make_shared<IMFTextListenerAdapterImpl>(nullptr);
    EXPECT_NE(listenerTest, nullptr);
    std::u16string text;
    MiscServices::KeyEvent event;
    MiscServices::FunctionKey functionKey;
    listenerTest->InsertText(text);
    listenerTest->DeleteForward(0);
    listenerTest->DeleteBackward(0);
    listenerTest->SendKeyEventFromInputMethod(event);
    listenerTest->SendKeyboardStatus(MiscServices::KeyboardStatus::SHOW);
    listenerTest->SendFunctionKey(functionKey);
    listenerTest->SetKeyboardStatus(true);
    listenerTest->MoveCursor(MiscServices::Direction::NONE);
    listenerTest->HandleSetSelection(0, 0);
    listenerTest->HandleExtendAction(0);
    listenerTest->HandleSelect(0, 0);
    listenerTest->GetTextIndexAtCursor();
    listenerTest->GetLeftTextOfCursor(0);
    listenerTest->GetRightTextOfCursor(0);
    MiscServices::Range range { 0, 0 };
    listenerTest->SetPreviewText(text, range);
    listenerTest->FinishTextPreview();
    const std::unordered_map<std::string, MiscServices::PrivateDataValue> privateCommand;
    listenerTest->ReceivePrivateCommand(privateCommand);
}

/**
 * @tc.name: NWebIMFAdapterTest_InsertText_008.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_InsertText_008, TestSize.Level1)
{
    bool result = g_imf->Attach(nullptr, true, nullptr, false);
    EXPECT_FALSE(result);
    auto listener = std::make_shared<IMFTextListenerTest>();
    EXPECT_NE(listener, nullptr);
    result = g_imf->Attach(nullptr, true, nullptr, false);
    EXPECT_FALSE(result);
    result = g_imf->Attach(nullptr, true, nullptr, false);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NWebIMFAdapterTest_IMFAdapterImpl_009.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_009, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto listenerTest = std::make_shared<IMFTextListenerAdapterImpl>(listener);
    std::unordered_map<std::string, MiscServices::PrivateDataValue> privateCommand;
    privateCommand = { { "test", "test" } };
    listenerTest->ReceivePrivateCommand(privateCommand);
    privateCommand = { { "previewTextStyle", "underline" } };
    listenerTest->ReceivePrivateCommand(privateCommand);
    privateCommand = { { "com.autofill.params.userName", "test" } };
    listenerTest->ReceivePrivateCommand(privateCommand);
    privateCommand = { { "com.autofill.params.otherAccount", true } };
    listenerTest->ReceivePrivateCommand(privateCommand);
}

/**
 * @tc.name: NWebIMFAdapterTest_InsertText_010.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_InsertText_010, TestSize.Level1)
{
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto config = std::make_shared<IMFTextConfigTest>();
    bool result = g_imf->Attach(listener, true, nullptr, false);
    EXPECT_FALSE(result);

    result = g_imf->Attach(listener, true, config, false);
    EXPECT_FALSE(result);
    EXPECT_NE(g_imf->textListener_, nullptr);
    g_imf->HideTextInput();
}

/**
 * @tc.name: NWebIMFAdapterTest_InsertText_011.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_InsertText_011, TestSize.Level1)
{
    EXPECT_NE(g_imf, nullptr);
    std::string commandKey = "autofill.cancel";
    std::string commandValue = "{\"userName\":\"test\",\"hasAccount\":\"test\"}";
    g_imf->SendPrivateCommand(commandKey, commandValue);
    std::string commandValueEmpty = "";
    g_imf->SendPrivateCommand(commandKey, commandValueEmpty);
    commandKey = "test";
    g_imf->SendPrivateCommand(commandKey, commandValue);
}

/**
 * @tc.name: NWebIMFAdapterTest_InsertText_012.
 * @tc.desc: IMF adapter unittest.
 * @tc.type: FUNC.
 * @tc.require:
 */
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_InsertText_012, TestSize.Level1)
{
    int32_t requestKeyboardReasonNone = 0;
    bool result = g_imf->AttachWithRequestKeyboardReason(nullptr, true, nullptr, false, requestKeyboardReasonNone);
    EXPECT_FALSE(result);
    auto listener = std::make_shared<IMFTextListenerTest>();
    EXPECT_NE(listener, nullptr);
    result = g_imf->AttachWithRequestKeyboardReason(nullptr, true, nullptr, false, requestKeyboardReasonNone);
    EXPECT_FALSE(result);
    result = g_imf->AttachWithRequestKeyboardReason(nullptr, true, nullptr, false, requestKeyboardReasonNone);
    EXPECT_FALSE(result);
    auto config = std::make_shared<IMFTextConfigTest>();
    result = g_imf->AttachWithRequestKeyboardReason(listener, true, nullptr, false, requestKeyboardReasonNone);
    EXPECT_FALSE(result);
    result = g_imf->AttachWithRequestKeyboardReason(listener, true, config, false, requestKeyboardReasonNone);
    EXPECT_FALSE(result);
    result = g_imf->AttachWithRequestKeyboardReason(listener, true, config, true, requestKeyboardReasonNone);
    EXPECT_FALSE(result);
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_013.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_013, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto listenerTest = std::make_shared<IMFTextListenerAdapterImpl>(listener);
    MiscServices::PanelStatusInfo info;
    const MiscServices::PanelStatusInfo info1 = info;
    listenerTest->NotifyPanelStatusInfo(info1);
    info.trigger = MiscServices::Trigger::IME_APP;
    const MiscServices::PanelStatusInfo info2 = info;
    listenerTest->NotifyPanelStatusInfo(info2);
    auto listenerTest2 = std::make_shared<IMFTextListenerAdapterImpl>(nullptr);
    listenerTest2->NotifyPanelStatusInfo(info2);
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_014.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_014, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto config = std::make_shared<IMFTextConfigTest>();
    bool result = g_imf->AttachParamsCheck(nullptr, true, config, false);
    EXPECT_FALSE(result);
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_015.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_015, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    bool result = g_imf->AttachParamsCheck(listener, true, nullptr, false);
    EXPECT_FALSE(result);
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_016.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_016, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto config = std::make_shared<IMFTextConfigTest>();
    bool result = g_imf->AttachParamsCheck(listener, true, config, false);
    EXPECT_TRUE(result);
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_017.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_017, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto listener = std::make_shared<IMFTextListenerTest>();
    auto config = std::make_shared<IMFTextConfigTest>();
    auto adapter = std::make_shared<IMFAdapterImpl>();
    adapter->textListener_ = new (std::nothrow) IMFTextListenerAdapterImpl(listener);
    bool result = g_imf->AttachParamsCheck(listener, true, config, true);
    EXPECT_TRUE(result);
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_018.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_018, TestSize.Level1)
{
    std::unordered_map<std::string, std::variant<std::string, bool, int32_t>> resultMap;
    std::string validJson = R"({"userName":"testUser","hasAccount":"true"})";
    std::string invalidJson = R"({"userName":,"hasAccount":"false"})";
    std::string emptyJson = "{}";
    std::string nullJson = "null";
    std::string mallocFailedJson = "a";

    EXPECT_TRUE(g_imf->ParseFillContentJsonValue(validJson, resultMap));

    std::string jsonMissingUserName = R"({"hasAccount":"false"})";
    resultMap.clear();
    EXPECT_TRUE(g_imf->ParseFillContentJsonValue(jsonMissingUserName, resultMap));
    EXPECT_EQ(resultMap.find("userName"), resultMap.end());

    std::string jsonMissingHasAccount = R"({"userName":"user1"})";
    resultMap.clear();
    EXPECT_TRUE(g_imf->ParseFillContentJsonValue(jsonMissingHasAccount, resultMap));
    EXPECT_EQ(resultMap.find("hasAccount"), resultMap.end());

    resultMap.clear();
    EXPECT_FALSE(g_imf->ParseFillContentJsonValue(invalidJson, resultMap));
    EXPECT_TRUE(resultMap.empty());

    resultMap.clear();
    EXPECT_TRUE(g_imf->ParseFillContentJsonValue(emptyJson, resultMap));
    EXPECT_TRUE(resultMap.empty());

    resultMap.clear();
    EXPECT_FALSE(g_imf->ParseFillContentJsonValue(nullJson, resultMap));
    EXPECT_TRUE(resultMap.empty());

    resultMap.clear();
    EXPECT_FALSE(g_imf->ParseFillContentJsonValue(mallocFailedJson, resultMap));
    EXPECT_TRUE(resultMap.empty());

    std::string jsonNullUserName = R"({"userName":"null","hasAccount":"true"})";
    resultMap.clear();
    EXPECT_TRUE(g_imf->ParseFillContentJsonValue(jsonNullUserName, resultMap));
    EXPECT_EQ(resultMap.find("userName"), resultMap.end());

    std::string jsonNullHasAccount = R"({"userName":"null","hasAccount":"true"})";
    resultMap.clear();
    EXPECT_TRUE(g_imf->ParseFillContentJsonValue(jsonNullHasAccount, resultMap));
    EXPECT_EQ(resultMap.find("hasAccount"), resultMap.end());
}

/**
@tc.name: NWebIMFAdapterTest_IMFAdapterImpl_019.
@tc.desc: IMF adapter unittest.
@tc.type: FUNC.
@tc.require:
*/
HWTEST_F(NWebIMFAdapterTest, NWebIMFAdapterTest_IMFAdapterImpl_019, TestSize.Level1)
{
    auto imf_adapter = OhosAdapterHelper::GetInstance().CreateMMIAdapter();
    EXPECT_NE(imf_adapter, nullptr);
    auto mockCursorInfo = std::make_shared<IMFCursorInfoAdapterMock>();
    const float testLeft = 100.0f;
    const float testTop = 200.0f;
    const float testWidth = 10.0f;
    const float testHeight = 20.0f;

    mockCursorInfo->SetLeft(testLeft);
    mockCursorInfo->SetTop(testTop);
    mockCursorInfo->SetWidth(testWidth);
    mockCursorInfo->SetHeight(testHeight);

    g_imf->OnCursorUpdate(mockCursorInfo);
    EXPECT_TRUE(true);

    auto zeroCursorInfo = std::make_shared<IMFCursorInfoAdapterMock>();
    zeroCursorInfo->SetLeft(0.0f);
    zeroCursorInfo->SetTop(0.0f);
    zeroCursorInfo->SetWidth(0.0f);
    zeroCursorInfo->SetHeight(0.0f);
    g_imf->OnCursorUpdate(zeroCursorInfo);
    EXPECT_TRUE(true);

    auto negativeCursorInfo = std::make_shared<IMFCursorInfoAdapterMock>();
    negativeCursorInfo->SetLeft(-10.0f);
    negativeCursorInfo->SetTop(-10.0f);
    negativeCursorInfo->SetWidth(-10.0f);
    negativeCursorInfo->SetHeight(-10.0f);
    g_imf->OnCursorUpdate(negativeCursorInfo);
    EXPECT_TRUE(true);

    auto failControllerCursorInfo = std::make_shared<IMFCursorInfoAdapterMock>();
    failControllerCursorInfo->SetLeft(10.0f);
    failControllerCursorInfo->SetTop(10.0f);
    failControllerCursorInfo->SetWidth(10.0f);
    failControllerCursorInfo->SetHeight(10.0f);
    g_imf->OnCursorUpdate(failControllerCursorInfo);
    EXPECT_TRUE(true);

    auto abnormalCursorInfo = std::make_shared<IMFCursorInfoAdapterMock>();
    abnormalCursorInfo->SetLeft(std::numeric_limits<float>::quiet_NaN());
    abnormalCursorInfo->SetTop(std::numeric_limits<float>::infinity());
    abnormalCursorInfo->SetWidth(-std::numeric_limits<float>::max());
    abnormalCursorInfo->SetHeight(-std::numeric_limits<float>::max());
    g_imf->OnCursorUpdate(abnormalCursorInfo);
    EXPECT_TRUE(true);
}
} // namespace OHOS::NWeb
