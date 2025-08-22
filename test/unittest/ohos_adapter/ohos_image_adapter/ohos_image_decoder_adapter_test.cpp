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
#include <cstdint>
#include <securec.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "pixel_map.h"
#include "directory_ex.h"
#include "image_source.h"
#include "image_utils.h"
#include "ohos_image_decoder_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {

namespace NWeb {
namespace {

static const std::string IMAGE_FILE_PATH = "/data/local/tmp/image/test.heic";

bool ReadFileToBuffer(const std::string &filePath, uint8_t *buffer, size_t bufferSize)
{
    std::string realPath;
    if (!OHOS::PathToRealPath(filePath, realPath)) {
        return false;
    }

    if (buffer == nullptr) {
        return false;
    }

    FILE *fp = fopen(realPath.c_str(), "rb");
    if (fp == nullptr) {
        return false;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return false;
    }
    size_t fileSize = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return false;
    }
    if (bufferSize < fileSize) {
        fclose(fp);
        return false;
    }
    size_t retSize = fread(buffer, 1, fileSize, fp);
    if (retSize != fileSize) {
        fclose(fp);
        return false;
    }
    int ret = fclose(fp);
    if (ret != 0) {
        return true;
    }
    return true;
}

bool TestInitImage(OhosImageDecoderAdapterImpl &imageDecoderAdapterImpl, uint8_t *buffer)
{
    size_t bufferSize = 0;
    bool boolRes = Media::ImageUtils::GetFileSize(IMAGE_FILE_PATH, bufferSize);
    if (!boolRes) {
        return false;
    }

    buffer = static_cast<uint8_t *>(malloc(bufferSize));
    if (buffer == nullptr) {
        return false;
    }

    boolRes = ReadFileToBuffer(IMAGE_FILE_PATH, buffer, bufferSize);
    if (!boolRes) {
        free(buffer);
        return false;
    }

    boolRes = imageDecoderAdapterImpl.ParseImageInfo(buffer, bufferSize);
    if (!boolRes) {
        free(buffer);
        return false;
    }
    return true;
}

bool TestDecodeImage(OhosImageDecoderAdapterImpl &imageDecoderAdapterImpl, uint8_t *buffer, AllocatorType type)
{
    size_t bufferSize = 0;
    bool boolRes = Media::ImageUtils::GetFileSize(IMAGE_FILE_PATH, bufferSize);
    if (!boolRes) {
        return false;
    }

    buffer = static_cast<uint8_t *>(malloc(bufferSize));
    if (buffer == nullptr) {
        return false;
    }

    boolRes = ReadFileToBuffer(IMAGE_FILE_PATH, buffer, bufferSize);
    if (!boolRes) {
        free(buffer);
        return false;
    }

    boolRes = imageDecoderAdapterImpl.Decode(buffer, bufferSize, type, false);
    if (!boolRes) {
        free(buffer);
        return false;
    }
    return true;
}
} // namespace

class OhosImageAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void OhosImageAdapterTest::SetUpTestCase(void)
{}

void OhosImageAdapterTest::TearDownTestCase(void)
{}

void OhosImageAdapterTest::SetUp(void)
{}

void OhosImageAdapterTest::TearDown(void)
{}

/**
 * @tc.name: OhosImageAdapterTest_ParseImageInfo_001
 * @tc.desc: ParseImageInfo.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_ParseImageInfo_001, TestSize.Level1)
{
    uint8_t data[32] = {0};
    uint32_t size = sizeof(data);
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    bool result = imageDecoderAdapterImpl.ParseImageInfo(nullptr, size);
    EXPECT_FALSE(result);

    result = imageDecoderAdapterImpl.ParseImageInfo(data, 0);
    EXPECT_FALSE(result);

    result = imageDecoderAdapterImpl.ParseImageInfo(data, size);
    EXPECT_FALSE(result);

    size_t bufferSize = 0;
    bool boolRes = Media::ImageUtils::GetFileSize(IMAGE_FILE_PATH, bufferSize);
    EXPECT_TRUE(boolRes);

    uint8_t *buffer = static_cast<uint8_t *>(malloc(bufferSize));
    EXPECT_NE(buffer, nullptr);

    boolRes = ReadFileToBuffer(IMAGE_FILE_PATH, buffer, bufferSize);
    EXPECT_TRUE(boolRes);

    boolRes = imageDecoderAdapterImpl.ParseImageInfo(buffer, (uint32_t)bufferSize);
    EXPECT_TRUE(boolRes);

    free(buffer);
}

/**
 * @tc.name: OhosImageAdapterTest_GetEncodedFormat_002
 * @tc.desc: GetEncodedFormat.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetEncodedFormat_002, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    bool ret = TestInitImage(imageDecoderAdapterImpl, buffer);
    EXPECT_TRUE(ret);
    std::string format = imageDecoderAdapterImpl.GetEncodedFormat();
    EXPECT_NE(format, "");

    free(buffer);
}

/**
 * @tc.name: OhosImageAdapterTest_GetImageWidth_003
 * @tc.desc: GetImageWidth.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetImageWidth_003, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    bool ret = TestInitImage(imageDecoderAdapterImpl, buffer);
    EXPECT_TRUE(ret);
    int32_t width = imageDecoderAdapterImpl.GetImageWidth();
    EXPECT_NE(width, 0);

    free(buffer);
}

/**
 * @tc.name: OhosImageAdapterTest_GetImageHeight_004
 * @tc.desc: GetImageHeight.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetImageHeight_004, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    bool ret = TestInitImage(imageDecoderAdapterImpl, buffer);
    EXPECT_TRUE(ret);
    int32_t heigth = imageDecoderAdapterImpl.GetImageHeight();
    EXPECT_NE(heigth, 0);

    free(buffer);
}

/**
 * @tc.name: OhosImageAdapterTest_DecodeToPixelMap_005
 * @tc.desc: DecodeToPixelMap.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_DecodeToPixelMap_005, TestSize.Level1)
{
    size_t bufferSize = 0;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    bool ret = imageDecoderAdapterImpl.DecodeToPixelMap(nullptr, bufferSize);
    EXPECT_FALSE(ret);

    bool boolRes = Media::ImageUtils::GetFileSize(IMAGE_FILE_PATH, bufferSize);

    uint8_t *buffer = static_cast<uint8_t *>(malloc(bufferSize));

    boolRes = ReadFileToBuffer(IMAGE_FILE_PATH, buffer, bufferSize);
    EXPECT_TRUE(boolRes);

    ret = imageDecoderAdapterImpl.DecodeToPixelMap(buffer, bufferSize);
    EXPECT_TRUE(ret);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_GetFd_006
 * @tc.desc: GetFd.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetFd_006, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    int32_t fd = imageDecoderAdapterImpl.GetFd();
    EXPECT_EQ(fd, -1);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kDmaAlloc);
    EXPECT_TRUE(ret);

    fd = imageDecoderAdapterImpl.GetFd();
    EXPECT_NE(fd, -1);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    fd = imageDecoderAdapterImpl.GetFd();
    EXPECT_EQ(fd, -1);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_GetStride_007
 * @tc.desc: GetStride.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetStride_007, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    int32_t stride = imageDecoderAdapterImpl.GetStride();
    EXPECT_EQ(stride, 0);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kDmaAlloc);
    EXPECT_TRUE(ret);

    stride = imageDecoderAdapterImpl.GetStride();
    EXPECT_NE(stride, 0);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    stride = imageDecoderAdapterImpl.GetStride();
    EXPECT_EQ(stride, 0);

    free(buffer);
    buffer = nullptr;

    imageDecoderAdapterImpl.ReleasePixelMap();

    ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kShareMemAlloc);
    EXPECT_TRUE(ret);

    stride = imageDecoderAdapterImpl.GetStride();
    EXPECT_NE(stride, 0);

    pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_GetOffset_008
 * @tc.desc: GetOffset.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetOffset_008, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    int32_t offset = imageDecoderAdapterImpl.GetOffset();
    EXPECT_EQ(offset, 0);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kDmaAlloc);
    EXPECT_TRUE(ret);

    offset = imageDecoderAdapterImpl.GetOffset();
    EXPECT_EQ(offset, 0);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    offset = imageDecoderAdapterImpl.GetOffset();
    EXPECT_EQ(offset, 0);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_GetSize_009
 * @tc.desc: GetSize.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetSize_009, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    uint64_t size = imageDecoderAdapterImpl.GetSize();
    EXPECT_EQ(size, 0);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kDmaAlloc);
    EXPECT_TRUE(ret);

    size = imageDecoderAdapterImpl.GetSize();
    EXPECT_NE(size, 0);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    size = imageDecoderAdapterImpl.GetSize();
    EXPECT_EQ(size, 0);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_GetNativeWindowBuffer_010
 * @tc.desc: GetNativeWindowBuffer.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetNativeWindowBuffer_010, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    void* windowBuffer = imageDecoderAdapterImpl.GetNativeWindowBuffer();
    EXPECT_EQ(windowBuffer, nullptr);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kDmaAlloc);
    EXPECT_TRUE(ret);

    windowBuffer = imageDecoderAdapterImpl.GetNativeWindowBuffer();
    EXPECT_NE(windowBuffer, nullptr);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    windowBuffer = imageDecoderAdapterImpl.GetNativeWindowBuffer();
    EXPECT_NE(windowBuffer, nullptr);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}


/**
 * @tc.name: OhosImageAdapterTest_GetPlanesCount_011
 * @tc.desc: GetPlanesCount.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetPlanesCount_011, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    int32_t planesCount = imageDecoderAdapterImpl.GetPlanesCount();
    EXPECT_EQ(planesCount, 0);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kDmaAlloc);
    EXPECT_TRUE(ret);

    planesCount = imageDecoderAdapterImpl.GetPlanesCount();
    EXPECT_EQ(planesCount, 0);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    planesCount = imageDecoderAdapterImpl.GetPlanesCount();
    EXPECT_EQ(planesCount, 0);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_Decode_012
 * @tc.desc: Decode.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_Decode_012, TestSize.Level1)
{
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;
    uint8_t data[32] = {0};
    uint32_t size = sizeof(data);
    bool ret = imageDecoderAdapterImpl.Decode(data, size, AllocatorType::kDmaAlloc, false);
    EXPECT_FALSE(ret);

    ret = imageDecoderAdapterImpl.Decode(nullptr, size, AllocatorType::kDmaAlloc, false);
    EXPECT_FALSE(ret);

    ret = imageDecoderAdapterImpl.Decode(data, 0, AllocatorType::kDmaAlloc, false);
    EXPECT_FALSE(ret);

    size_t bufferSize = 0;
    bool boolRes = Media::ImageUtils::GetFileSize(IMAGE_FILE_PATH, bufferSize);
    uint8_t *buffer = static_cast<uint8_t *>(malloc(bufferSize));
    boolRes = ReadFileToBuffer(IMAGE_FILE_PATH, buffer, bufferSize);
    EXPECT_TRUE(boolRes);

    ret = imageDecoderAdapterImpl.Decode(buffer, bufferSize, AllocatorType::kDmaAlloc, true);
    EXPECT_TRUE(ret);

    ret = imageDecoderAdapterImpl.Decode(buffer, bufferSize, AllocatorType::kDmaAlloc, false);
    EXPECT_TRUE(ret);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_GetDecodeData_013
 * @tc.desc: GetDecodeData.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_GetDecodeData_013, TestSize.Level1)
{
    uint8_t *buffer = nullptr;
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;

    void* data = imageDecoderAdapterImpl.GetDecodeData();
    EXPECT_EQ(data, nullptr);

    bool ret = TestDecodeImage(imageDecoderAdapterImpl, buffer, AllocatorType::kShareMemAlloc);
    EXPECT_TRUE(ret);

    data = imageDecoderAdapterImpl.GetDecodeData();
    EXPECT_NE(data, nullptr);

    auto* pixelMap = imageDecoderAdapterImpl.GetPixelMap();
    pixelMap->FreePixelMap();
    data = imageDecoderAdapterImpl.GetDecodeData();
    EXPECT_EQ(data, nullptr);

    free(buffer);

    imageDecoderAdapterImpl.ReleasePixelMap();
}

/**
 * @tc.name: OhosImageAdapterTest_DecodeByPath_014
 * @tc.desc: DecodeByPath.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosImageAdapterTest, OhosImageAdapterTest_DecodeByPath_014, TestSize.Level1)
{
    OhosImageDecoderAdapterImpl imageDecoderAdapterImpl;
    bool ret = imageDecoderAdapterImpl.DecodeByPath(IMAGE_FILE_PATH, AllocatorType::kDmaAlloc);
    EXPECT_FALSE(ret);
}

} // namespace NWeb
} // namespace OHOS
