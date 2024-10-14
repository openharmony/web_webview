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

#include <cstring>
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#define private public
#include "pasteboard_client_adapter_impl.h"
#undef private

#define FIVE 5

#include "ohos_adapter_helper.h"
#include "pasteboard_client_adapter.h"

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
protected:
    NWebPasteboardAdapterTest() {}

    ~NWebPasteboardAdapterTest() override {}

    void SetUp();

    void TearDown() override {}
};

void NWebPasteboardAdapterTest::SetUp() {
    int result = 0;
    g_dataAdapterNull = std::make_shared<PasteDataAdapterImpl>();
    result = (g_dataAdapterNull == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
    g_dataAdapterNull->data_ = nullptr;

    result = 0;
    OH_UdmfRecord* record = OH_UdmfRecord_Create();
    result = (record == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
    
    result = 0;
    g_pasternull = std::make_shared<PasteDataRecordAdapterImpl>(record);
    result = (g_pasternull == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
    g_pasternull->record_ = nullptr;

    result = 0;
    std::string mimeType = "pixelMap";
    g_datarecordnull = std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
    result = (g_datarecordnull == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
    g_datarecordnull->record_ = nullptr;

    result = 0;
    g_datarecord = std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
    result = (g_datarecord == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);

    result = 0;
    g_htmlText = std::make_shared<std::string>("htmlText");
    result = (g_htmlText == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    g_plainText = std::make_shared<std::string>("plainText");
    result = (g_plainText == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    g_paster = std::make_shared<PasteDataRecordAdapterImpl>(g_mimeType, g_htmlText, g_plainText);
    result = (g_paster == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);

    result = 0;
    g_dataAdapter = std::make_shared<PasteDataAdapterImpl>();
    result = (g_dataAdapter == nullptr) ? -1 : 0;
    EXPECT_EQ(RESULT_OK, result);
}

class MockPasteboardObserver : public PasteboardObserverAdapter {
public:
    MockPasteboardObserver() = default;
    void OnPasteboardChanged() override {}
};

class MockClipBoardImageDataAdapter : public ClipBoardImageDataAdapter {
public:
    MockClipBoardImageDataAdapter() = default;

    ClipBoardImageColorType GetColorType() override
    {
        return colorType;
    }

    ClipBoardImageAlphaType GetAlphaType() override
    {
        return alphaType;
    }

    uint32_t* GetData() override
    {
        return data;
    }

    size_t GetDataSize() override
    {
        return dataSize;
    }

    size_t GetRowBytes() override
    {
        return rowBytes;
    }

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    void SetColorType(ClipBoardImageColorType color) override
    {
        colorType = color;
    }

    void SetAlphaType(ClipBoardImageAlphaType alpha) override
    {
        alphaType = alpha;
    }

    void SetData(uint32_t* d) override
    {
        data = d;
    }

    void SetDataSize(size_t size) override
    {
        dataSize = size;
    }

    void SetRowBytes(size_t r) override
    {
        rowBytes = r;
    }

    void SetWidth(int32_t w) override
    {
        width = w;
    }

    void SetHeight(int32_t h) override
    {
        height = h;
    }

    ClipBoardImageColorType colorType;
    ClipBoardImageAlphaType alphaType;
    uint32_t* data;
    size_t dataSize;
    size_t rowBytes;
    int32_t width;
    int32_t height;
};

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataRecordAdapterImpl_001.
 * @tc.desc: Test the PasteDataRecordAdapterImpl.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataRecordAdapterImpl_001)
{
    int result = 0;
    OH_UdmfRecord* record = OH_UdmfRecord_Create();
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    
    result = 0;
    std::shared_ptr<PasteDataRecordAdapterImpl> pasterimpl = std::make_shared<PasteDataRecordAdapterImpl>(record);
    if (pasterimpl == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    pasterimpl->GetImgData(nullptr);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataRecordAdapterImpl_002.
 * @tc.desc: Test the PasteDataRecordAdapterImpl.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataRecordAdapterImpl_002)
{
    int result = 0;
    g_htmlText = std::make_shared<std::string>("htmlText");
    if (g_htmlText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    g_plainText = std::make_shared<std::string>("plainText");
    if (g_plainText == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_NewRecord_003)
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetMimeType_004)
{
    int ret = 0;
    std::string mimeType = g_paster->GetMimeType();
    if (mimeType.empty()) {
        ret = -1;
    }
    //mimeType must be empty, so ret still should be 0
    ret = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetHtmlText_005)
{
    int result = 0;
    std::shared_ptr<std::string> htmlText = g_paster->GetHtmlText();
    if (htmlText == nullptr) {
        result = -1;
    }
    //htmlText must be empty, so result still should be 0
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPlainText_006)
{
    int result = 0;
    std::shared_ptr<std::string> plainText = g_paster->GetPlainText();
    if (plainText == nullptr) {
        result = -1;
    }
    //plainText must be empty, so result still should be 0
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecord_007)
{
    int result = 0;
    OH_UdmfRecord* record = g_paster->GetRecord();
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataAdapterImpl_008.
 * @tc.desc: Test the PasteDataAdapterImpl.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataAdapterImpl_008)
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataAdapterImpl_009)
{
    int result = 0;
    OH_UdmfData* data = OH_UdmfData_Create();
    if (data == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_AddHtmlRecord_010)
{
    std::string htmlName = "test";
    EXPECT_NE(g_dataAdapter, nullptr);
    g_dataAdapter->AddHtmlRecord(htmlName);
    g_dataAdapterNull->AddHtmlRecord(htmlName);
}

/**
 * @tc.name: NWebPasteboardAdapter_AddTextRecord_011.
 * @tc.desc: Test the AddTextRecord.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_AddTextRecord_011)
{
    std::string textName = "test";
    EXPECT_NE(g_dataAdapter, nullptr);
    g_dataAdapter->AddTextRecord(textName);
    g_dataAdapterNull->AddTextRecord(textName);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetMimeTypes_012.
 * @tc.desc: Test the GetMimeTypes.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetMimeTypes_012)
{
    int result = 0;
    std::vector<std::string> mimeTypes = g_dataAdapter->GetMimeTypes();
    if (mimeTypes.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPrimaryHtml_013)
{
    int result = 0;
    std::shared_ptr<std::string> primaryHtml = g_dataAdapter->GetPrimaryHtml();
    if (primaryHtml == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPrimaryText_014)
{
    int result = 0;
    std::size_t index = 0;
    std::shared_ptr<std::string> primaryText = g_dataAdapter->GetPrimaryText();
    if (primaryText == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
    std::shared_ptr<PasteDataRecordAdapter> record = g_dataAdapter->GetRecordAt(index);
    if (record == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
    PasteRecordVector recordVector = g_dataAdapter->AllRecords();
    if (recordVector.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPrimaryMimeType_015)
{
    int result = 0;
    std::shared_ptr<std::string> primaryMimeType = g_dataAdapter->GetPrimaryMimeType();
    if (primaryMimeType == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecordAt_016)
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecordAt_017)
{
    int result = 0;
    std::size_t index = 0;
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
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetRecordCount_018)
{
    int result = 0;
    std::size_t record = g_dataAdapterNull->GetRecordCount();
    if (record != 0) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    std::size_t count = g_dataAdapterNull->GetRecordCount();
    EXPECT_EQ(RESULT_OK, count);
    g_dataAdapterNull->data_ = nullptr;
}

/**
 * @tc.name: NWebPasteboardAdapter_AllRecords_019.
 * @tc.desc: Test the AllRecords.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_AllRecords_019)
{
    int result = 0;
    PasteRecordVector record = g_dataAdapterNull->AllRecords();
    if (record.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
    PasteRecordVector str = g_dataAdapterNull->AllRecords();
    if (str.empty()) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_SetPasteData_020.
 * @tc.desc: Test the SetPasteData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetPasteData_020)
{
    PasteRecordVector data;
    std::shared_ptr<PasteDataRecordAdapter> record = PasteDataRecordAdapter::NewRecord("text/html");
    EXPECT_NE(record, nullptr);
    std::shared_ptr<std::string> pasteData = std::make_shared<std::string>("test");
    EXPECT_NE(pasteData, nullptr);
    record->SetHtmlText(pasteData);
    data.push_back(record);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data, CopyOptionMode::NONE);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetPasteData_021.
 * @tc.desc: Test the GetPasteData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetPasteData_021)
{
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    PasteRecordVector data;
    bool result = PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data);
    EXPECT_EQ(false, result);
    std::shared_ptr<PasteDataRecordAdapter> adapter = std::make_shared<PasteDataRecordAdapterImpl>("test");
    data.push_back(adapter);
    result = PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data);
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_HasPasteData_022.
 * @tc.desc: Test the HasPasteData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_HasPasteData_022)
{
    bool result = PasteBoardClientAdapterImpl::GetInstance().HasPasteData();
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_PasteDataRecordAdapterImpl_023.
 * @tc.desc: Test the PasteDataRecordAdapterImpl.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_PasteDataRecordAdapterImpl_023)
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
 * @tc.name: NWebPasteboardAdapter_NewRecord_024.
 * @tc.desc: Test the NewRecord.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_NewRecord_024)
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
 * @tc.name: NWebPasteboardAdapter_SetHtmlText_025.
 * @tc.desc: Test the SetHtmlText.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetHtmlText_025)
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
 * @tc.name: NWebPasteboardAdapter_SetPlainText_026.
 * @tc.desc: Test the SetPlainText.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetPlainText_026)
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
 * @tc.name: NWebPasteboardAdapter_SetImgData_031.
 * @tc.desc: Test the SetImgData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetImgData_031)
{
    uint32_t storage[][5] = {
        { 0xCA, 0xDA, 0xCA, 0xC9, 0xA3 },
        { 0xAC, 0xA8, 0x89, 0x47, 0x87 },
        { 0x4B, 0x25, 0x25, 0x25, 0x46 },
        { 0x90, 0x1, 0x25, 0x41, 0x33 },
        { 0x75, 0x55, 0x44, 0x20, 0x00 },
    };

    std::shared_ptr<MockClipBoardImageDataAdapter> image = std::make_shared<MockClipBoardImageDataAdapter>();
    image->SetColorType(ClipBoardImageColorType::COLOR_TYPE_BGRA_8888);
    image->SetAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN);
    image->SetData(storage[0]);
    image->SetDataSize(sizeof(storage));
    image->SetRowBytes(FIVE);
    image->SetWidth(FIVE);
    image->SetHeight(FIVE);
    bool reset = g_datarecord->SetImgData(image);
    EXPECT_EQ(TRUE_OK, reset);
    reset = g_datarecordnull->SetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
    image->SetDataSize(1);
    reset = g_datarecordnull->SetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
    image = nullptr;
    reset = g_datarecordnull->SetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetImgData_032.
 * @tc.desc: Test the GetImgData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetImgData_032)
{
    std::shared_ptr<MockClipBoardImageDataAdapter> image = std::make_shared<MockClipBoardImageDataAdapter>();
    bool reset = g_datarecordnull->GetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_paster->GetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetImgData_033.
 * @tc.desc: Test the GetImgData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetImgData_033)
{
    std::shared_ptr<MockClipBoardImageDataAdapter> image = std::make_shared<MockClipBoardImageDataAdapter>();
    bool reset = g_datarecord->GetImgData(image);
    EXPECT_NE(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_Clear_034.
 * @tc.desc: Test the Clear.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_Clear_034)
{
    uint32_t bufferSize = 20;
    EXPECT_NE(g_datarecord, nullptr);
    if (g_datarecord->imgBuffer_ == nullptr) {
        g_datarecord->imgBuffer_ = static_cast<uint8_t*>(calloc(static_cast<size_t>(bufferSize), sizeof(uint8_t)));
    }
    g_datarecord->Clear();
}

/**
 * @tc.name: NWebPasteboardAdapter_Clear_035.
 * @tc.desc: Test the Clear.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_Clear_035)
{
    PasteRecordVector data;
    data.clear();
    EXPECT_EQ(data.size(), 0);
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data);
    PasteBoardClientAdapterImpl::GetInstance().Clear();
}

/**
 * @tc.name: NWebPasteboardAdapter_SetUri_036.
 * @tc.desc: Test the SetUri.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetUri_036)
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
 * @tc.name: NWebPasteboardAdapter_SetCustomData_037.
 * @tc.desc: Test the SetCustomData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_SetCustomData_037)
{
    int result = 0;
    PasteCustomData emptyTestData;
    std::string format = "format";
    std::vector<uint8_t> data_ = { 0, 1, 2 };
    PasteCustomData testData;
    testData.insert(std::make_pair(format, data_));
    EXPECT_EQ(RESULT_OK, result);
    bool reset = g_datarecordnull->SetCustomData(testData);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetCustomData(emptyTestData);
    EXPECT_NE(TRUE_OK, reset);
    reset = g_datarecord->SetCustomData(testData);
    EXPECT_EQ(TRUE_OK, reset);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetUri_038.
 * @tc.desc: Test the GetUri.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetUri_038)
{
    int result = 0;
    g_datarecord->SetUri("/data/test/path");
    std::shared_ptr<std::string> uri = g_datarecord->GetUri();
    if (uri == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    g_datarecord->SetUri("");
    uri = g_datarecord->GetUri();
    if (uri == nullptr) {
        result = -1;
    }
    EXPECT_EQ(RESULT_OK, result);
    result = 0;
    uri = g_pasternull->GetUri();
    if (uri == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_GetCustomData_039.
 * @tc.desc: Test the GetCustomData.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_GetCustomData_039)
{
    int result = 0;
    std::string format = "format";
    std::vector<uint8_t> data_ = { 0, 1, 2 };
    PasteCustomData testData;
    g_datarecord->SetCustomData(testData);
    std::shared_ptr<PasteCustomData> customData = g_datarecord->GetCustomData();
    if (customData == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
    g_datarecord->SetCustomData(testData);
    customData = g_datarecord->GetCustomData();
    if (customData == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
    result = 0;
    customData = g_pasternull->GetCustomData();
    if (customData == nullptr) {
        result = -1;
    }
    EXPECT_NE(RESULT_OK, result);
}

/**
 * @tc.name: NWebPasteboardAdapter_OpenRemoteUri_040.
 * @tc.desc: Test the OpenRemoteUri.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, NWebPasteboardAdapter_OpenRemoteUri_040)
{
    std::string testUri = "datashare:////#fdFromBinder=155";
    int32_t fd = PasteBoardClientAdapterImpl::GetInstance().OpenRemoteUri(testUri);
    EXPECT_EQ(-1, fd);
    std::string testInvalidUri = "www.example.com";
    fd = PasteBoardClientAdapterImpl::GetInstance().OpenRemoteUri(testInvalidUri);
    EXPECT_EQ(-1, fd);
}

/**
 * @tc.name: PasteBoardClientAdapterImpl_GetTokenId_041.
 * @tc.desc: Test the GetTokenId.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, PasteBoardClientAdapterImpl_GetTokenId_041)
{
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().GetTokenId());
}

/**
 * @tc.name: PasteBoardClientAdapterImpl_IsLocalPaste_042.
 * @tc.desc: Test the IsLocalPaste.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, PasteBoardClientAdapterImpl_IsLocalPaste_042)
{
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    EXPECT_EQ(false, PasteBoardClientAdapterImpl::GetInstance().IsLocalPaste());
}

/**
 * @tc.name: PasteboardObserverAdapter_OnPasteboardChanged_043.
 * @tc.desc: Test the CreatePasteboardObserver.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, PasteboardObserverAdapter_OnPasteboardChanged_043)
{
    std::shared_ptr<PasteboardObserverAdapter> observer = std::make_shared<MockPasteboardObserver>();
    EXPECT_NE(observer, nullptr);
    observer->OnPasteboardChanged();
}

/**
 * @tc.name: PasteBoardClientAdapterImpl_AddPasteboardChangedObserver_044.
 * @tc.desc: Test the AddPasteboardChangedObserver.
 * @tc.type: FUNC
 */
TEST_F(NWebPasteboardAdapterTest, PasteBoardClientAdapterImpl_AddPasteboardChangedObserver_044)
{
    std::shared_ptr<PasteboardObserverAdapter> observer = std::make_shared<MockPasteboardObserver>();
    std::shared_ptr<PasteboardObserverAdapter> observerInvalid = std::make_shared<MockPasteboardObserver>();
    int32_t id = PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(observer);
    PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(nullptr);
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().reg_.size());
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(id);
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().reg_.size());
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(-1);
    EXPECT_EQ(0, PasteBoardClientAdapterImpl::GetInstance().reg_.size());

    Udmf_ShareOption option =
        PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::IN_APP);
    EXPECT_EQ(option, Udmf_ShareOption::SHARE_OPTIONS_IN_APP);
    option = PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::LOCAL_DEVICE);
    EXPECT_EQ(option, Udmf_ShareOption::SHARE_OPTIONS_CROSS_APP);
    option = PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::CROSS_DEVICE);
    EXPECT_EQ(option, Udmf_ShareOption::SHARE_OPTIONS_INVALID);
    option = PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::NONE);
    EXPECT_EQ(option, Udmf_ShareOption::SHARE_OPTIONS_INVALID);
}
} // namespace OHOS::NWeb