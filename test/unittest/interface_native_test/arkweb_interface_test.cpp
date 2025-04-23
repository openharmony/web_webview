/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <securec.h>

#define private public
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_config_helper.h"
#include "nweb_adapter_helper.h"
#include "base/web/webview/interfaces/native/arkweb_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace NWeb {
class MockNWebEngine : public OHOS::NWeb::NWebEngine {
public:
    std::shared_ptr<NWeb> CreateNWeb(std::shared_ptr<NWebCreateInfo> create_info)
    {
        return nullptr;
    }

    std::shared_ptr<NWeb> GetNWeb(int32_t nweb_id)
    {
        return nullptr;
    }

    std::shared_ptr<NWebDataBase> GetDataBase()
    {
        return nullptr;
    }

    std::shared_ptr<NWebWebStorage> GetWebStorage()
    {
        return nullptr;
    }

    std::shared_ptr<NWebCookieManager> GetCookieManager()
    {
        return nullptr;
    }

    std::shared_ptr<NWebDownloadManager> GetDownloadManager()
    {
        return nullptr;
    }

    void SetWebTag(int32_t nweb_id, const char* web_tag) {}

    void InitializeWebEngine(std::shared_ptr<NWebEngineInitArgs> init_args) {}

    void PrepareForPageLoad(const std::string& url, bool preconnectable, int32_t num_sockets) {}

    void SetWebDebuggingAccess(bool isEnableDebug) {}

    void AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) {}

    void RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>& hosts) {}
    void ClearIntelligentTrackingPreventionBypassingList() {}

    std::string GetDefaultUserAgent()
    {
        return "";
    }

    void PauseAllTimers() {}

    void ResumeAllTimers() {}

    void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>& pre_args,
        const std::map<std::string, std::string>& additional_http_headers, const std::string& cache_key,
        const uint32_t& cache_valid_time)
    {}

    void SetRenderProcessMode(RenderProcessMode mode) {}

    RenderProcessMode GetRenderProcessMode()
    {
        return RenderProcessMode::SINGLE_MODE;
    }

    void ClearPrefetchedResource(const std::vector<std::string>& cache_key_list) {}

    void WarmupServiceWorker(const std::string& url) {}

    void SetHostIP(const std::string& hostName, const std::string& address, int32_t aliveTime) {}

    void ClearHostIP(const std::string& hostName) {}

    void SetAppCustomUserAgent(const std::string& userAgent) {}

    void SetUserAgentForHosts(const std::string& userAgent, const std::vector<std::string>& hosts) {}

    void SetWholeWebDrawing() {}

    std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager()
    {
        return nullptr;
    }

    void EnableBackForwardCache(bool nativeEmbed, bool mediaTakeOver) {}

    void RemoveAllCache(bool include_disk_files) {}
};

class OHArkWebInterfaceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void OHArkWebInterfaceTest::SetUpTestCase(void)
{}

void OHArkWebInterfaceTest::TearDownTestCase(void)
{}

void OHArkWebInterfaceTest::SetUp(void)
{}

void OHArkWebInterfaceTest::TearDown(void)
{}

/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_001
 * @tc.desc  : Test OH_ArkWeb_GetNativeAPI->LoadComponentAPI
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_001, TestSize.Level1) {
    ArkWeb_NativeAPIVariantKind type = ARKWEB_NATIVE_COMPONENT;
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;

    ArkWeb_AnyNativeAPI* result = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result, nullptr);

    ArkWeb_AnyNativeAPI* result1 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result1, nullptr);

    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    ArkWeb_AnyNativeAPI* result2 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_EQ(result2, nullptr);
}

/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_002
 * @tc.desc  : Test OH_ArkWeb_GetNativeAPI->LoadControllerAPI
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_002, TestSize.Level1) {
    ArkWeb_NativeAPIVariantKind type = ARKWEB_NATIVE_CONTROLLER;
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;

    ArkWeb_AnyNativeAPI* result = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result, nullptr);

    ArkWeb_AnyNativeAPI* result1 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result1, nullptr);

    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    ArkWeb_AnyNativeAPI* result2 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_EQ(result2, nullptr);
}

/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_003
 * @tc.desc  : Test OH_ArkWeb_GetNativeAPI->LoadWebMessagePortAPI
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_003, TestSize.Level1) {
    ArkWeb_NativeAPIVariantKind type = ARKWEB_NATIVE_WEB_MESSAGE_PORT;
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;

    ArkWeb_AnyNativeAPI* result = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result, nullptr);

    ArkWeb_AnyNativeAPI* result1 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result1, nullptr);

    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    ArkWeb_AnyNativeAPI* result2 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_EQ(result2, nullptr);
}

/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_004
 * @tc.desc  : Test OH_ArkWeb_GetNativeAPI->LoadWebMessageAPI
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_004, TestSize.Level1) {
    ArkWeb_NativeAPIVariantKind type = ARKWEB_NATIVE_WEB_MESSAGE;
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;

    ArkWeb_AnyNativeAPI* result = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result, nullptr);

    ArkWeb_AnyNativeAPI* result1 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result1, nullptr);

    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    ArkWeb_AnyNativeAPI* result2 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_EQ(result2, nullptr);
}


/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_005
 * @tc.desc  : Test OH_ArkWeb_GetNativeAPI->LoadCookieManagerAPI
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_005, TestSize.Level1) {
    ArkWeb_NativeAPIVariantKind type = ARKWEB_NATIVE_COOKIE_MANAGER;
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().initFlag_ = true;

    ArkWeb_AnyNativeAPI* result = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result, nullptr);

    ArkWeb_AnyNativeAPI* result1 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result1, nullptr);

    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    ArkWeb_AnyNativeAPI* result2 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_EQ(result2, nullptr);
}

/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_006
 * @tc.desc  : Test OH_ArkWeb_GetNativeAPI->LoadJavaScriptValueAPI
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_GetNativeAPI_006, TestSize.Level1) {
    ArkWeb_NativeAPIVariantKind type = ARKWEB_NATIVE_JAVASCRIPT_VALUE;
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;

    ArkWeb_AnyNativeAPI* result = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result, nullptr);

    ArkWeb_AnyNativeAPI* result1 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_NE(result1, nullptr);

    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    ArkWeb_AnyNativeAPI* result2 = OH_ArkWeb_GetNativeAPI(type);
    EXPECT_EQ(result2, nullptr);
}

/**
 * @tc.name  : OHArkWebInterfaceTest_OH_ArkWeb_RegisterScrollCallback
 * @tc.desc  : Test OH_ArkWeb_RegisterScrollCallback
 */
HWTEST_F(OHArkWebInterfaceTest, OHArkWebInterfaceTest_OH_ArkWeb_RegisterScrollCallback, TestSize.Level1) {
    const char* webTag = "testWebTag";
    void* userData = nullptr;
    ArkWeb_OnScrollCallback callback = [](const char* webTag, void* userData, double x, double y) {};
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    bool result = OH_ArkWeb_RegisterScrollCallback(webTag, callback, userData);
    EXPECT_EQ(result, false);
    result = OH_ArkWeb_RegisterScrollCallback(webTag, callback, userData);
    EXPECT_EQ(result, false);
    OH_ArkWeb_DestroyGlobalPointersForTest();
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    result = OH_ArkWeb_RegisterScrollCallback(webTag, callback, userData);
    EXPECT_EQ(result, false);
}

}
}