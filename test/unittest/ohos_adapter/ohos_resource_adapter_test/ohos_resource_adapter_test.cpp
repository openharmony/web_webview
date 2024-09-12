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
#include <cstdint>
#include <securec.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "ohos_resource_adapter_impl.h"
#include "application_context.h"
#undef private

#include "extractor.h"
#include "zip_file_reader.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::AbilityBase;
using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace {
constexpr uint32_t MODULE_NAME_SIZE = 32;
const std::string NWEB_HAP_PATH = "/system/app/com.ohos.nweb/NWeb.hap";
const std::string NWEB_HAP_PATH_1 = "/system/app/NWeb/NWeb.hap";
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
}

namespace AbilityRuntime {
std::shared_ptr<ApplicationContext> Context::GetApplicationContext()
{
    return g_applicationContext;
}
} // namespace AbilityRuntime

namespace NWeb {
class OhosResourceAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void OhosResourceAdapterTest::SetUpTestCase(void)
{}

void OhosResourceAdapterTest::TearDownTestCase(void)
{}

void OhosResourceAdapterTest::SetUp(void)
{}

void OhosResourceAdapterTest::TearDown(void)
{}

class ApplicationContextMock : public ApplicationContext {
public:
    MOCK_CONST_METHOD0(GetResourceManager, std::shared_ptr<Global::Resource::ResourceManager>());
    MOCK_METHOD2(CreateModuleContext, std::shared_ptr<Context>(const std::string &, const std::string &));
};

class ExtractorMock : public Extractor{
public:
    explicit ExtractorMock() : Extractor("web_test") {}
    ~ExtractorMock() override = default;
    MOCK_METHOD0(Init, bool());
    MOCK_METHOD0(IsStageModel, bool());
    MOCK_METHOD3(ExtractToBufByName, bool(const std::string &, std::unique_ptr<uint8_t[]> &, size_t &));
};

/**
 * @tc.name: OhosResourceAdapterTest_Init_001
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosResourceAdapterTest, OhosResourceAdapterTest_Init_001, TestSize.Level1)
{
    std::string hapPath = "/system/app/com.ohos.nweb/NWeb.hap";
    auto adapterImpl = std::make_shared<OhosResourceAdapterImpl>(hapPath);
    EXPECT_NE(adapterImpl, nullptr);
    hapPath.clear();
    adapterImpl->Init(hapPath);
}

/**
 * @tc.name: OhosResourceAdapterTest_GetRawFileData_002
 * @tc.desc: GetRawFileData.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosResourceAdapterTest, OhosResourceAdapterTest_GetRawFileData_002, TestSize.Level1)
{
    std::string hapPath = "/system/app/com.ohos.nweb/NWeb.hap";
    OhosResourceAdapterImpl adapterImpl(hapPath);
    std::unique_ptr<uint8_t[]> dest;
    std::string rawFile = "test_web";
    size_t len = rawFile.size();
    uint8_t* data;
    bool result = adapterImpl.GetRawFileData(rawFile, len, &data, true);
    EXPECT_FALSE(result);
    std::shared_ptr<Extractor> extractor = std::make_shared<Extractor>(rawFile);
    result = adapterImpl.GetRawFileData(extractor, rawFile, len, dest);
    EXPECT_FALSE(result);
    result = adapterImpl.GetRawFileData(nullptr, rawFile, len, dest);
    EXPECT_FALSE(result);
    auto mock = std::make_shared<ExtractorMock>();
    EXPECT_CALL(*mock, IsStageModel())
        .WillRepeatedly(::testing::Return(false));
    adapterImpl.GetRawFileData(mock, rawFile, len, dest);
    std::shared_ptr<OhosFileMapper> fileMapper = adapterImpl.GetRawFileMapper(rawFile, true);
    EXPECT_EQ(fileMapper, nullptr);
    result = adapterImpl.IsRawFileExist(rawFile, true);
    EXPECT_FALSE(result);
    uint16_t date;
    uint16_t time;
    result = adapterImpl.GetRawFileLastModTime(rawFile, date, time, true);
    EXPECT_FALSE(result);
    result = adapterImpl.GetRawFileLastModTime(rawFile, date, time, false);
    EXPECT_FALSE(result);
    time_t times;
    result = adapterImpl.GetRawFileLastModTime(rawFile, times, true);
    EXPECT_FALSE(result);
    result = adapterImpl.GetRawFileLastModTime(rawFile, times, false);
    EXPECT_FALSE(result);
    result = adapterImpl.HasEntry(extractor, rawFile);
    EXPECT_FALSE(result);
    result = adapterImpl.HasEntry(nullptr, rawFile);
    EXPECT_FALSE(result);
    std::shared_ptr<OhosFileMapper> mapper = adapterImpl.GetRawFileMapper(nullptr, rawFile);
    EXPECT_EQ(mapper, nullptr);
    mapper = adapterImpl.GetRawFileMapper(extractor, rawFile);
    EXPECT_EQ(mapper, nullptr);
    adapterImpl.sysExtractor_.reset();
    result = adapterImpl.GetRawFileLastModTime(rawFile, date, time, true);
    EXPECT_FALSE(result);
    result = adapterImpl.GetRawFileLastModTime(rawFile, times, true);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: OhosResourceAdapterTest_OhosFileMapperImpl_003
 * @tc.desc: OhosFileMapperImpl.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosResourceAdapterTest, OhosResourceAdapterTest_OhosFileMapperImpl_003, TestSize.Level1)
{
    std::string hapPath = "";
    if (access(NWEB_HAP_PATH.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH;
    }
    if (access(NWEB_HAP_PATH_1.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH_1;
    }
    std::shared_ptr<Extractor> extractor = std::make_shared<Extractor>(hapPath);
    EXPECT_NE(extractor, nullptr);
    std::shared_ptr<OHOS::AbilityBase::ZipFileReader> fileReader =
        OHOS::AbilityBase::ZipFileReader::CreateZipFileReader(hapPath);
    EXPECT_NE(fileReader, nullptr);
    std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap = std::make_unique<OHOS::AbilityBase::FileMapper>();
    EXPECT_NE(fileMap, nullptr);
    fileMap->CreateFileMapper(fileReader, hapPath, 0, hapPath.size(), true);
    OhosFileMapperImpl apperImpl(std::move(fileMap), extractor);
    int32_t result = apperImpl.GetFd();
    EXPECT_EQ(result, -1);
    result = apperImpl.GetOffset();
    EXPECT_NE(result, -1);
    std::string fileNmae = apperImpl.GetFileName();
    EXPECT_NE(fileNmae, "");
    bool isCompressed = apperImpl.IsCompressed();
    EXPECT_TRUE(isCompressed);
    void* data = apperImpl.GetDataPtr();
    EXPECT_NE(data, nullptr);
    size_t dataLen = apperImpl.GetDataLen();
    EXPECT_NE(dataLen, 0);
    uint8_t* dest;
    isCompressed = apperImpl.UnzipData(&dest, dataLen);
    EXPECT_FALSE(isCompressed);
    apperImpl.extractor_.reset();
    isCompressed = apperImpl.UnzipData(&dest, dataLen);
    EXPECT_FALSE(isCompressed);
}

/**
 * @tc.name: OhosResourceAdapterTest_ParseModuleName_004
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosResourceAdapterTest, OhosResourceAdapterTest_ParseModuleName_004, TestSize.Level1)
{
    std::string hapPath = "";
    if (access(NWEB_HAP_PATH.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH;
    }
    if (access(NWEB_HAP_PATH_1.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH_1;
    }
    OhosResourceAdapterImpl adapterImpl(hapPath);
    bool newCreate = false;
    std::shared_ptr<Extractor> extractor = ExtractorUtil::GetExtractor(hapPath, newCreate);
    EXPECT_NE(extractor, nullptr);
    std::string result = adapterImpl.GetModuleName(nullptr, 0);
    EXPECT_EQ(result, "");
    char *configStr = static_cast<char*>(malloc(MODULE_NAME_SIZE));
    EXPECT_NE(configStr, nullptr);
    (void)memset_s(configStr, MODULE_NAME_SIZE, 0, MODULE_NAME_SIZE);
    result = adapterImpl.GetModuleName(configStr, MODULE_NAME_SIZE);
    EXPECT_EQ(result, "");
    if (memcpy_s(configStr, MODULE_NAME_SIZE, "\"moduleName\"", sizeof("\"moduleName\"")) != EOK) {
        EXPECT_FALSE(true);
    };
    result = adapterImpl.GetModuleName(configStr, MODULE_NAME_SIZE);
    EXPECT_EQ(result, "");
    uint32_t len = strlen(configStr);
    if (memcpy_s(configStr + len, MODULE_NAME_SIZE - len, "\"", sizeof("\"")) != EOK) {
        EXPECT_FALSE(true);
    };
    result = adapterImpl.GetModuleName(configStr, MODULE_NAME_SIZE);
    EXPECT_EQ(result, "");
    (void)memset_s(configStr, MODULE_NAME_SIZE, 0, MODULE_NAME_SIZE);
    if (memcpy_s(configStr, MODULE_NAME_SIZE, "\"moduleName\"\"test\"", sizeof("\"moduleName\"\"test\"")) != EOK) {
        EXPECT_FALSE(true);
    };
    result = adapterImpl.GetModuleName(configStr, MODULE_NAME_SIZE);
    EXPECT_NE(result, "");
    result = adapterImpl.ParseModuleName(extractor);
    EXPECT_EQ(result, "");
    result = adapterImpl.ParseModuleName(nullptr);
    EXPECT_EQ(result, "");
    free(configStr);
    configStr = nullptr;
    auto mock = std::make_shared<ExtractorMock>();
    EXPECT_CALL(*mock, ExtractToBufByName(::testing::_, ::testing::_, ::testing::_))
        .WillRepeatedly(::testing::Return(true));
    adapterImpl.ParseModuleName(mock);
}

/**
 * @tc.name: OhosResourceAdapterTest_GetResourceMgr_005
 * @tc.desc: GetResourceMgr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosResourceAdapterTest, OhosResourceAdapterTest_GetResourceMgr_005, TestSize.Level1)
{
    std::string hapPath = "";
    if (access(NWEB_HAP_PATH.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH;
    }
    if (access(NWEB_HAP_PATH_1.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH_1;
    }
    OhosResourceAdapterImpl adapterImpl(hapPath);
    uint8_t* dest;
    std::string rawFile = "test";
    size_t len = rawFile.size();
    bool result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/bundleName";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/bundleName:";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/bundleName:test/";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/bundleName:test/moduleName:";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/bundleName:test/moduleName:web/";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    rawFile = "resources/rawfile/bundleName:test/moduleName:web/webtest";
    len = rawFile.size();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
    g_applicationContext = std::make_shared<ApplicationContextMock>();
    result = adapterImpl.GetRawFileData(rawFile, len, &dest, false);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: OhosResourceAdapterTest_GetResourceString_006
 * @tc.desc: GetResourceString.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosResourceAdapterTest, OhosResourceAdapterTest_GetResourceString_006, TestSize.Level1)
{
    std::string hapPath = "";
    if (access(NWEB_HAP_PATH.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH;
    }
    if (access(NWEB_HAP_PATH_1.c_str(), F_OK) == 0) {
        hapPath = NWEB_HAP_PATH_1;
    }
    OhosResourceAdapterImpl adapterImpl(hapPath);
    std::string res;

    bool result = adapterImpl.GetResourceString("", "", 1, res);
    EXPECT_FALSE(result);

    result = adapterImpl.GetResourceString("test", "web", 1, res);
    EXPECT_FALSE(result);

    result = adapterImpl.GetResourceString("test", "web/webtest", 1, res);
    EXPECT_FALSE(result);
}
}
} // namespace NWeb
