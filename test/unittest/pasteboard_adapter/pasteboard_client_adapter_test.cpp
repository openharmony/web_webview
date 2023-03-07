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
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "pasteboard_client_adapter_impl.h"
#undef private

#include "paste_data.h"
#include "paste_data_record.h"
#include "pasteboard_client_adapter.h"
#include "ohos_adapter_helper.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MiscServices;
using namespace OHOS::Media;

namespace OHOS::NWeb {
namespace {
const int RESULT_OK = 0;
const bool TRUE_OK = true;
const std::string g_mimeType = "data";
std::shared_ptr<std::string> g_htmlText;
std::shared_ptr<std::string> g_plainText;
std::shared_ptr<PasteDataRecordAdapterImpl> g_paster;
std::shared_ptr<PasteDataRecordAdapterImpl> g_pasternull;
std::shared_ptr<PasteDataRecordAdapterImpl> g_datarecord;
std::shared_ptr<PasteDataRecordAdapterImpl> g_datarecordnull;
std::shared_ptr<PasteDataAdapterImpl> g_dataAdapter;
std::shared_ptr<PasteDataAdapterImpl> g_dataAdapterNull;
} // namespace

class NWebPasteboardAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NWebPasteboardAdapterTest::SetUpTestCase(void)
{
    int result = 0;
    g_dataAdapterNull = std::make_shared<PasteDataAdapterImpl>();
    if (g_dataAdapterNull == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_dataAdapterNull->data_ = nullptr;

    std::shared_ptr<PasteDataRecord> record = std::make_shared<PasteDataRecord>();
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_pasternull = std::make_shared<PasteDataRecordAdapterImpl>(record);
    if (g_pasternull == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_pasternull->record_ = nullptr;

    std::string mimeType = "pixelMap";
    g_datarecordnull = std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
    if (g_datarecordnull == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_datarecordnull->builder_ = nullptr;
    g_datarecordnull->record_ = nullptr;

    result = 0;
    g_datarecord = std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
    if (g_datarecord == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

void NWebPasteboardAdapterTest::TearDownTestCase(void)
{}

void NWebPasteboardAdapterTest::SetUp(void)
{}

void NWebPasteboardAdapterTest::TearDown(void)
{}

class MockPasteData : public PasteData {
public:
    MOCK_METHOD1(GetRecordAt, std::shared_ptr<PasteDataRecord>(std::size_t));
    MOCK_METHOD0(GetRecordCount, std::size_t());
    MOCK_METHOD1(Encode, bool(std::vector<uint8_t> &));
    MOCK_METHOD1(Decode, bool(const std::vector<uint8_t> &));
};

class MockPasteDataRecord : public PasteDataRecord {
public:
    MOCK_METHOD0(GetPixelMap, std::shared_ptr<PixelMap>());
    MOCK_METHOD1(Encode, bool(std::vector<uint8_t> &));
    MOCK_METHOD1(Decode, bool(const std::vector<uint8_t> &));
};

class MockPasteboardObserver : public PasteboardObserverAdapter {
public:
    MockPasteboardObserver() = default;
    void OnPasteboardChanged() override {}
};

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataRecordAdapterImpl_001.
 * @tc.desc: Test the PasteDataRecordAdapterImpl.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataRecordAdapterImpl_001, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<PasteDataRecord> record = std::make_shared<PasteDataRecord>();
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<PasteDataRecordAdapterImpl> pasterimpl = std::make_shared<PasteDataRecordAdapterImpl>(record);
    if (pasterimpl == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataRecordAdapterImpl_002.
 * @tc.desc: Test the PasteDataRecordAdapterImpl.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataRecordAdapterImpl_002, TestSize.Level1)
{
    int result = 0;
    g_htmlText = std::make_shared<std::string>("htmlText");
    if (g_htmlText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_plainText = std::make_shared<std::string>("plainText");
    if (g_plainText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_paster = std::make_shared<PasteDataRecordAdapterImpl>(g_mimeType, g_htmlText, g_plainText);
    if (g_paster == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_NewRecord_003.
 * @tc.desc: Test the NewRecord.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_NewRecord_003, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<PasteDataRecordAdapter> record =
        PasteDataRecordAdapter::NewRecord(g_mimeType, g_htmlText, g_plainText);
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetMimeType_004.
 * @tc.desc: Test the GetMimeType.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetMimeType_004, TestSize.Level1)
{
    int ret = 0;
    std::string mimeType = g_paster->GetMimeType();
    if (mimeType.empty()) {
        ret = -1;
    }
    EXPECT_EQ(RESULT_OK, ret);
    mimeType = g_pasternull->GetMimeType();
    if (mimeType.empty()) {
        ret = -1;
    }
    EXPECT_NE(RESULT_OK, ret);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetHtmlText_005.
 * @tc.desc: Test the GetHtmlText.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetHtmlText_005, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<std::string> htmlText = g_paster->GetHtmlText();
    if (htmlText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<std::string> html = g_pasternull->GetHtmlText();
    if (html == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetPlainText_006.
 * @tc.desc: Test the GetPlainText.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPlainText_006, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<std::string> plainText = g_paster->GetPlainText();
    if (plainText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<std::string> plain = g_pasternull->GetPlainText();
    if (plain == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetRecord_007.
 * @tc.desc: Test the GetRecord.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecord_007, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<PasteDataRecord> record = g_paster->GetRecord();
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataAdapterImpl_008.
 * @tc.desc: Test the PasteDataAdapterImpl.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataAdapterImpl_008, TestSize.Level1)
{
    int result = 0;
    g_dataAdapter = std::make_shared<PasteDataAdapterImpl>();
    if (g_dataAdapter == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataAdapterImpl_009.
 * @tc.desc: Test the PasteDataAdapterImpl.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataAdapterImpl_009, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<PasteData> data = std::make_shared<PasteData>();
    if (data == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<PasteDataAdapterImpl> dataAdapter = std::make_shared<PasteDataAdapterImpl>(data);
    if (dataAdapter == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_AddHtmlRecord_010.
 * @tc.desc: Test the AddHtmlRecord.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_AddHtmlRecord_010, TestSize.Level1)
{
    std::string htmlName = "test";
    g_dataAdapter->AddHtmlRecord(htmlName);
    g_dataAdapterNull->AddHtmlRecord(htmlName);
}

/**
 * @tc.name: NWebPasteboardAdapter_AddTextRecord_011.
 * @tc.desc: Test the AddTextRecord.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_AddTextRecord_011, TestSize.Level1)
{
    std::string htmlName = "test";
    g_dataAdapter->AddTextRecord(htmlName);
    g_dataAdapterNull->AddTextRecord(htmlName);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetMimeTypes_012.
 * @tc.desc: Test the GetMimeTypes.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetMimeTypes_012, TestSize.Level1)
{
    int result = 0;
    std::vector<std::string> mimeTypes = g_dataAdapter->GetMimeTypes();
    if (mimeTypes.empty()) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::vector<std::string> types = g_dataAdapterNull->GetMimeTypes();
    if (types.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetPrimaryHtml_013.
 * @tc.desc: Test the GetPrimaryHtml.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPrimaryHtml_013, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<std::string> primaryHtml = g_dataAdapter->GetPrimaryHtml();
    if (primaryHtml == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<std::string> primary = g_dataAdapterNull->GetPrimaryHtml();
    if (primary == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetPrimaryText_014.
 * @tc.desc: Test the GetPrimaryText.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPrimaryText_014, TestSize.Level1)
{
    int result = 0;
    std::size_t index = 0;
    std::shared_ptr<std::string> primaryText = g_dataAdapter->GetPrimaryText();
    if (primaryText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    std::shared_ptr<PasteDataRecordAdapter> record = g_dataAdapter->GetRecordAt(index);
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    PasteRecordList recordList = g_dataAdapter->AllRecords();
    if (recordList.empty()) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    std::shared_ptr<std::string> primary = g_dataAdapterNull->GetPrimaryText();
    if (primary == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetPrimaryMimeType_015.
 * @tc.desc: Test the GetPrimaryMimeType.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPrimaryMimeType_015, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<std::string> primaryMimeType = g_dataAdapter->GetPrimaryMimeType();
    if (primaryMimeType == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<std::string> primary = g_dataAdapterNull->GetPrimaryMimeType();
    if (primary == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetRecordAt_016.
 * @tc.desc: Test the GetRecordAt.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BB
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecordAt_016, TestSize.Level1)
{
    int result = 0;
    std::size_t index = 1;
    std::shared_ptr<PasteDataRecordAdapter> record = g_dataAdapterNull->GetRecordAt(index);
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetRecordAt_017.
 * @tc.desc: Test the GetRecordAt.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecordAt_017, TestSize.Level1)
{
    int result = 0;
    std::size_t index = 0;
    MockPasteData *mock1 = new MockPasteData();
    g_dataAdapterNull->data_.reset((PasteData *)mock1);
    std::shared_ptr<PasteDataRecordAdapter> str = g_dataAdapterNull->GetRecordAt(index);
    if (str == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    g_dataAdapterNull->data_ = nullptr;
}

/**
 * @tc.name: NWebPasteboardAdapter_GetRecordCount_018.
 * @tc.desc: Test the GetRecordCount.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecordCount_018, TestSize.Level1)
{
    int result = 0;
    std::size_t record = g_dataAdapterNull->GetRecordCount();
    if (record != 0) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    MockPasteData *mock2 = new MockPasteData();
    g_dataAdapterNull->data_.reset((PasteData *)mock2);
    std::size_t count = g_dataAdapterNull->GetRecordCount();
    EXPECT_EQ(RESULT_OK, count);
    g_dataAdapterNull->data_ = nullptr;
}

/**
 * @tc.name: NWebPasteboardAdapter_AllRecords_019.
 * @tc.desc: Test the AllRecords.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_AllRecords_019, TestSize.Level1)
{
    int result = 0;
    PasteRecordList record = g_dataAdapterNull->AllRecords();
    if (record.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
    MockPasteData *mock = new MockPasteData();
    g_dataAdapterNull->data_.reset((PasteData *)mock);
    PasteRecordList str = g_dataAdapterNull->AllRecords();
    if (str.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetInstance_020.
 * @tc.desc: Test the GetInstance.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetInstance_020, TestSize.Level1)
{
    PasteBoardClientAdapterImpl::GetInstance();
}

/**
 * @tc.name: NWebPasteboardAdapter_SetPasteData_021.
 * @tc.desc: Test the SetPasteData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetPasteData_021, TestSize.Level1)
{
    int result = 0;
    PasteRecordList data;
    std::shared_ptr<PasteDataRecordAdapter> record = PasteDataRecordAdapter::NewRecord("text/html");
    if (record == nullptr) {
        result = -1;
    }
    std::shared_ptr<std::string> pasteData = std::make_shared<std::string>("test");
    record->SetHtmlText(pasteData);
    data.push_back(record);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetPasteData_022.
 * @tc.desc: Test the GetPasteData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPasteData_022, TestSize.Level1)
{
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    PasteRecordList data;
    bool result = PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data);
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_HasPasteData_023.
 * @tc.desc: Test the HasPasteData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_HasPasteData_023, TestSize.Level1)
{
    bool result = PasteBoardClientAdapterImpl::GetInstance().HasPasteData();
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataRecordAdapterImpl_024.
 * @tc.desc: Test the PasteDataRecordAdapterImpl.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataRecordAdapterImpl_024, TestSize.Level1)
{
    int result = 0;
    std::string mimeType = "test";
    std::shared_ptr<PasteDataRecordAdapterImpl> datarecord = std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
    if (datarecord == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_NewRecord_025.
 * @tc.desc: Test the NewRecord.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_NewRecord_025, TestSize.Level1)
{
    int result = 0;
    std::string mimeType = "test";
    std::shared_ptr<PasteDataRecordAdapter> record = PasteDataRecordAdapter::NewRecord(mimeType);
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_SetHtmlText_026.
 * @tc.desc: Test the SetHtmlText.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetHtmlText_026, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<std::string> htmlText = std::make_shared<std::string>("test");
    if (htmlText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    bool reset = g_datarecordnull->SetHtmlText(htmlText);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetHtmlText(htmlText);
    EXPECT_EQ(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_SetPlainText_027.
 * @tc.desc: Test the SetPlainText.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetPlainText_027, TestSize.Level1)
{
    int result = 0;
    std::shared_ptr<std::string> plainText = std::make_shared<std::string>("test");
    if (plainText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    bool reset = g_datarecordnull->SetPlainText(plainText);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetPlainText(plainText);
    EXPECT_EQ(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_ImageToClipboardAlphaType_028.
 * @tc.desc: Test the ImageToClipboardAlphaType.
 * @tc.type: FUNC
 * @tc.require:issueI5O4B5
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_ImageToClipboardAlphaType_028, TestSize.Level1)
{
    Media::ImageInfo imgInfo;
    imgInfo.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    ClipBoardImageAlphaType result = g_datarecord->ImageToClipboardAlphaType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN);

    imgInfo.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    result = g_datarecord->ImageToClipboardAlphaType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE);

    imgInfo.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
    result = g_datarecord->ImageToClipboardAlphaType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED);

    imgInfo.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_UNPREMUL;
    result = g_datarecord->ImageToClipboardAlphaType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN);
}

/**
 * @tc.name: NWebPasteboardAdapter_ImageToClipboardColorType_029.
 * @tc.desc: Test the ImageToClipboardColorType.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_ImageToClipboardColorType_029, TestSize.Level1)
{
    Media::ImageInfo imgInfo;
    imgInfo.pixelFormat = Media::PixelFormat::RGBA_8888;
    ClipBoardImageColorType result = g_datarecord->ImageToClipboardColorType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageColorType::COLOR_TYPE_RGBA_8888);

    imgInfo.pixelFormat = Media::PixelFormat::BGRA_8888;
    result = g_datarecord->ImageToClipboardColorType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageColorType::COLOR_TYPE_BGRA_8888);

    imgInfo.pixelFormat = Media::PixelFormat::RGBA_F16;
    result = g_datarecord->ImageToClipboardColorType(imgInfo);
    EXPECT_EQ(result, ClipBoardImageColorType::COLOR_TYPE_UNKNOWN);
}

/**
 * @tc.name: NWebPasteboardAdapter_ClipboardToImageAlphaType_030.
 * @tc.desc: Test the ClipboardToImageAlphaType.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_ClipboardToImageAlphaType_030, TestSize.Level1)
{
    ClipBoardImageAlphaType alphaType = ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    Media::AlphaType result = g_datarecord->ClipboardToImageAlphaType(alphaType);
    EXPECT_EQ(result, Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN);
    
    alphaType = ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
    result = g_datarecord->ClipboardToImageAlphaType(alphaType);
    EXPECT_EQ(result, Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE);

    alphaType = ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
    result = g_datarecord->ClipboardToImageAlphaType(alphaType);
    EXPECT_EQ(result, Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL);

    alphaType = ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED;
    result = g_datarecord->ClipboardToImageAlphaType(alphaType);
    EXPECT_EQ(result, Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN);
}

/**
 * @tc.name: NWebPasteboardAdapter_ClipboardToImageColorType_031.
 * @tc.desc: Test the ClipboardToImageColorType.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_ClipboardToImageColorType_031, TestSize.Level1)
{
    ClipBoardImageColorType colorType = ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
    Media::PixelFormat result = g_datarecord->ClipboardToImageColorType(colorType);
    EXPECT_EQ(result, Media::PixelFormat::RGBA_8888);

    colorType = ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
    result = g_datarecord->ClipboardToImageColorType(colorType);
    EXPECT_EQ(result, Media::PixelFormat::BGRA_8888);

    colorType = ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
    result = g_datarecord->ClipboardToImageColorType(colorType);
    EXPECT_EQ(result, Media::PixelFormat::UNKNOWN);
}

/**
 * @tc.name: NWebPasteboardAdapter_SetImgData_032.
 * @tc.desc: Test the SetImgData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetImgData_032, TestSize.Level1)
{
    uint32_t storage[][5] = {
        {0xCA, 0xDA, 0xCA, 0xC9, 0xA3},
        {0xAC, 0xA8, 0x89, 0x47, 0x87},
        {0x4B, 0x25, 0x25, 0x25, 0x46},
        {0x90, 0x1, 0x25, 0x41, 0x33},
        {0x75, 0x55, 0x44, 0x20, 0x00},
    };
    ClipBoardImageData *image = new ClipBoardImageData;
    image->colorType = ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
    image->alphaType = ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    image->data = storage[0];
    image->dataSize = sizeof(storage);
    image->rowBytes = 5;
    image->width = 5;
    image->height = 5;
    std::shared_ptr<ClipBoardImageData> imageData(image);
    bool reset = g_datarecord->SetImgData(imageData);
    EXPECT_EQ(TRUE_OK, reset);
    reset = g_datarecordnull->SetImgData(imageData);
    EXPECT_NE(TRUE_OK, reset);
    imageData->dataSize = 1;
    reset = g_datarecordnull->SetImgData(imageData);
    EXPECT_NE(TRUE_OK, reset);
    imageData = nullptr;
    reset = g_datarecordnull->SetImgData(imageData);
    EXPECT_NE(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetImgData_033.
 * @tc.desc: Test the GetImgData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetImgData_033, TestSize.Level1)
{
    ClipBoardImageData image;
    bool reset = g_datarecordnull->GetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_paster->GetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetImgData_034.
 * @tc.desc: Test the GetImgData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetImgData_034, TestSize.Level1)
{
    ClipBoardImageData image;
    bool reset = g_datarecord->GetImgData(image);
    EXPECT_EQ(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_Clear_035.
 * @tc.desc: Test the Clear.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_Clear_035, TestSize.Level1)
{
    uint32_t bufferSize = 20;
    if (g_datarecord->imgBuffer_ == nullptr) {
        g_datarecord->imgBuffer_ = static_cast<uint8_t *>(calloc(static_cast<size_t>(bufferSize), sizeof(uint8_t)));
    }
    g_datarecord->Clear();
}

/**
 * @tc.name: NWebPasteboardAdapter_Clear_036.
 * @tc.desc: Test the Clear.
 * @tc.type: FUNC
 * @tc.require:issueI5O4AZ
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_Clear_036, TestSize.Level1)
{
    PasteRecordList data;
    data.clear();
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data);
    PasteBoardClientAdapterImpl::GetInstance().Clear();
}

/**
 * @tc.name: NWebPasteboardAdapter_SetUri_037.
 * @tc.desc: Test the SetUri.
 * @tc.type: FUNC
 * @tc.require:issueI5QA3D
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetUri_037, TestSize.Level1)
{
    int result = 0;
    std::string emptyTestUri = "";
    std::string testUri = "/data/test/path";
    EXPECT_EQ(RESULT_OK, result);
    bool reset = g_datarecordnull->SetUri(testUri);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetUri(emptyTestUri);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetUri(testUri);
    EXPECT_EQ(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_SetCustomData_038.
 * @tc.desc: Test the SetCustomData.
 * @tc.type: FUNC
 * @tc.require:issueI5QA3D
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetCustomData_038, TestSize.Level1)
{
    int result = 0;
    PasteCustomData emptyTestData;
    std::string format = "format";
    vector<uint8_t> data = {0, 1, 2};
    PasteCustomData testData;
    testData.insert(std::make_pair(format, data));
    EXPECT_EQ(RESULT_OK, result);
    bool reset = g_datarecordnull->SetCustomData(testData);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetCustomData(emptyTestData);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetCustomData(testData);
    EXPECT_EQ(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetUri_039.
 * @tc.desc: Test the GetUri.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetUri_039, TestSize.Level1)
{
    int result = 0;
    g_datarecord->SetUri("/data/test/path");
    std::shared_ptr<std::string> uri = g_datarecord->GetUri();
    if (uri == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_datarecord->record_ = g_datarecord->builder_->SetUri(nullptr).Build();
    uri = g_datarecord->GetUri();
    if (uri == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    uri = g_pasternull->GetUri();
    if (uri == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetCustomData_040.
 * @tc.desc: Test the GetCustomData.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetCustomData_040, TestSize.Level1)
{
    int result = 0;
    std::string format = "format";
    vector<uint8_t> data = {0, 1, 2};
    PasteCustomData testData;
    g_datarecord->SetCustomData(testData);
    std::shared_ptr<PasteCustomData> customData = g_datarecord->GetCustomData();
    if (customData == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    g_datarecord->record_ = g_datarecord->builder_->SetCustomData(nullptr).Build();
    customData = g_datarecord->GetCustomData();
    if (customData == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    customData = g_pasternull->GetCustomData();
    if (customData == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_OpenRemoteUri_041.
 * @tc.desc: Test the OpenRemoteUri.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_OpenRemoteUri_041, TestSize.Level1)
{
    std::string testUri = "datashare:////#fdFromBinder=155";
    int32_t fd = PasteBoardClientAdapterImpl::GetInstance().OpenRemoteUri(testUri);
    EXPECT_EQ(155, fd);
    std::string testInvalidUri = "www.example.com";
    fd = PasteBoardClientAdapterImpl::GetInstance().OpenRemoteUri(testInvalidUri);
    EXPECT_EQ(-1, fd);
}

/**
 * @tc.name: PasteBoardClientAdapterImpl_GetTokenId_042.
 * @tc.desc: Test the GetTokenId.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, PasteBoardClientAdapterImpl_GetTokenId_042, TestSize.Level1)
{
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().GetTokenId());
}

/**
 * @tc.name: PasteBoardClientAdapterImpl_IsLocalPaste_043.
 * @tc.desc: Test the IsLocalPaste.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, PasteBoardClientAdapterImpl_IsLocalPaste_043, TestSize.Level1)
{
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    EXPECT_EQ(false, PasteBoardClientAdapterImpl::GetInstance().IsLocalPaste());
}

/**
 * @tc.name: PasteboardObserverAdapter_OnPasteboardChanged_044.
 * @tc.desc: Test the CreatePasteboardObserver.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, PasteboardObserverAdapter_OnPasteboardChanged_044, TestSize.Level1)
{
    std::shared_ptr<PasteboardObserverAdapter> observer =
        std::make_shared<MockPasteboardObserver>();
    PasteboardObserverAdapterImpl observerImpl(observer);
    observerImpl.OnPasteboardChanged();
    observerImpl.observer_ = nullptr;
    observerImpl.OnPasteboardChanged();
}

/**
 * @tc.name: PasteBoardClientAdapterImpl_AddPasteboardChangedObserver_045.
 * @tc.desc: Test the AddPasteboardChangedObserver.
 * @tc.type: FUNC
 * @tc.require:issueI5O4BN
 */
HWTEST_F(NWebPasteboardAdapterTest, PasteBoardClientAdapterImpl_AddPasteboardChangedObserver_045, TestSize.Level1)
{
    std::shared_ptr<PasteboardObserverAdapter> observer =
        std::make_shared<MockPasteboardObserver>();
    std::shared_ptr<PasteboardObserverAdapter> observerInvalid =
        std::make_shared<MockPasteboardObserver>();
    PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(observer);
    PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(nullptr);
    EXPECT_EQ(1, PasteBoardClientAdapterImpl::GetInstance().reg_.size());
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(observer);
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().reg_.size());
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(observerInvalid);
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().reg_.size());
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(nullptr);
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().reg_.size());
}
}