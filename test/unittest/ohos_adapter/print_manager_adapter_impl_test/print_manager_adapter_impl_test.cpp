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

#include <cstring>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "print_manager_adapter_impl.h"
#include "print_manager_adapter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {

namespace {
const char *TESTFILE_PATH = "/data/test/unittestfile";
}

class PrintManagerAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PrintManagerAdapterImplTest::SetUpTestCase(void)
{}

void PrintManagerAdapterImplTest::TearDownTestCase(void)
{}

void PrintManagerAdapterImplTest::SetUp(void)
{}

void PrintManagerAdapterImplTest::TearDown(void)
{}

class PrintDocumentAdapterImplMock : public PrintDocumentAdapterAdapter {
public:
    PrintDocumentAdapterImplMock() = default;
    ~PrintDocumentAdapterImplMock() override = default;

    void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::shared_ptr<PrintWriteResultCallbackAdapter> callback) override {}

    void OnJobStateChanged(const std::string& jobId, uint32_t state) override {}
};

class MockPrintWriteResultCallbackAdapter : public PrintWriteResultCallbackAdapter {
    public:
        MockPrintWriteResultCallbackAdapter() = default;
        void WriteResultCallback(std::string jobId, uint32_t code) {}
};

/**
 * @tc.name: PrintManagerAdapterImplTest_InitParamSet_001
 * @tc.desc: Init.
 * @tc.type: FUNC
 * @tc.require: AR000I7I57
 */
HWTEST_F(PrintManagerAdapterImplTest, PrintManagerAdapterImplTest_InitParamSet_001, TestSize.Level1)
{
#if defined(NWEB_PRINT_ENABLE)
    std::vector<std::string> fileList = { TESTFILE_PATH };
    uint32_t fd = open(TESTFILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd >= 0) {
        std::vector<uint32_t> fdList = { fd };
        std::string taskId;
        PrintManagerAdapterImpl::GetInstance().StartPrint(fileList, fdList, taskId);
        std::shared_ptr<PrintDocumentAdapterAdapter> printDocumentAdapterImpl;
        PrintAttributesAdapter printAttributesAdapter;
        PrintManagerAdapterImpl::GetInstance().Print("webPrintTestJob", printDocumentAdapterImpl,
            printAttributesAdapter);
        void* token = nullptr;
        PrintManagerAdapterImpl::GetInstance().Print("webPrintTestJob", printDocumentAdapterImpl,
            printAttributesAdapter, token);
        std::shared_ptr<PrintDocumentAdapterAdapter> mock = std::make_shared<PrintDocumentAdapterImplMock>();
        EXPECT_NE(mock, nullptr);
        PrintDocumentAdapterImpl documentAdapter(mock);
        std::string jobId = "abc";
        OHOS::Print::PrintAttributes oldAttrs = OHOS::Print::PrintAttributes();
        OHOS::Print::PrintAttributes newAttrs = OHOS::Print::PrintAttributes();
        auto writeResultCallback = [](std::string str, uint32_t index){};
        uint32_t state = 1;

        EXPECT_NE(documentAdapter.cb_, nullptr);
        documentAdapter.onStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, writeResultCallback);
        documentAdapter.onJobStateChanged(jobId, state);
        documentAdapter.cb_ = nullptr;
        documentAdapter.onStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, writeResultCallback);
        documentAdapter.onJobStateChanged(jobId, state);
        close(fd);
    }
#endif
}
}
} // namespace NWeb
