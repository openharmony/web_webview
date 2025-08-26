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
#include <gmock/gmock.h>
#include <securec.h>
#include <ui/rs_surface_node.h>
#include <unordered_map>

#define private public
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_config_helper.h"
#include "nweb_adapter_helper.h"
#include "nweb_create_window.h"
#include "nweb_c_api.h"
#include "nweb_init_params.h"
#include "application_context.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace {
sptr<Surface> g_surface = nullptr;
const bool RESULT_OK = true;
const int DEFAULT_WIDTH = 2560;
const int DEFAULT_HEIGHT = 1396;
const int32_t NWEB_MAX_WIDTH = 7681;
const int32_t LTPO_STRATEGY = 1;
const std::string MOCK_NWEB_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.arkwebcore";
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
} // namespace

namespace AbilityRuntime {
std::shared_ptr<ApplicationContext> Context::GetApplicationContext()
{
    return g_applicationContext;
}
} // namespace AbilityRuntime

namespace NWeb {

class NwebHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class ApplicationContextMock : public ApplicationContext {
public:
    MOCK_CONST_METHOD0(GetBaseDir, std::string());
};

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

    void EnableWholeWebPageDrawing() {}

    std::shared_ptr<NWebAdsBlockManager> GetAdsBlockManager()
    {
        return nullptr;
    }

    void EnableBackForwardCache(bool nativeEmbed, bool mediaTakeOver) {}

    void RemoveAllCache(bool include_disk_files) {}
};

void NwebHelperTest::SetUpTestCase(void)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "webTestSurfaceName";
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    EXPECT_NE(surfaceNode, nullptr);
    g_surface = surfaceNode->GetSurface();
    EXPECT_NE(g_surface, nullptr);
}

void NwebHelperTest::TearDownTestCase(void)
{}

void NwebHelperTest::SetUp(void)
{}

void NwebHelperTest::TearDown(void)
{}

std::unordered_map<std::string, std::string> g_argsMap;

/**
 * @tc.name: NWebHelper_SetBundlePath_001
 * @tc.desc: SetBundlePath.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetBundlePath_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    bool result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_FALSE(result);
    NWebHelper::Instance().SetBundlePath(MOCK_NWEB_INSTALLATION_DIR);
    result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
    NWebHelper::Instance().SetHttpDns(config);
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
    NWebHelper::Instance().WarmupServiceWorker("web_test");
    NWebHelper::Instance().PrefetchResource(nullptr, {}, "web_test", 0);
    NWebHelper::Instance().ClearPrefetchedResource({"web_test"});
    NWebAdapterHelper::Instance().ReadConfigIfNeeded();
    std::string bundleName = "LTPODynamicApp";
    EXPECT_FALSE(NWebAdapterHelper::Instance().IsLTPODynamicApp(bundleName));
    int32_t actualValue = NWebAdapterHelper::Instance().GetLTPOStrategy();
    EXPECT_TRUE(actualValue);
    NWebHelper::Instance().EnableBackForwardCache(true, true);
    result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_FALSE(result);
    ApplicationContextMock *contextMock = new ApplicationContextMock();
    ASSERT_NE(contextMock, nullptr);
    g_applicationContext.reset(contextMock);
    EXPECT_CALL(*contextMock, GetBaseDir())
        .Times(1)
        .WillRepeatedly(::testing::Return(""));
    result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_FALSE(result);
    NWebAdapterHelper::Instance().CreateNWeb(g_surface, GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_CALL(*contextMock, GetBaseDir())
        .Times(2)
        .WillRepeatedly(::testing::Return("test_web"));
    result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_TRUE(result);
    NWebAdapterHelper::Instance().CreateNWeb(g_surface, GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    WebDownloadManager_PutDownloadCallback(nullptr);
    g_applicationContext.reset();
}

/**
 * @tc.name: NWebHelper_GetWebStorage_002
 * @tc.desc: GetWebStorage.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetWebStorage_002, TestSize.Level1)
{
    auto web_storage = NWebHelper::Instance().GetWebStorage();
    bool result = false;
    if (web_storage != nullptr) {
        result = true;
    }
    EXPECT_EQ(RESULT_OK, result);
    std::string config = NWebAdapterHelper::Instance().ParsePerfConfig("web", "test");
    EXPECT_TRUE(config.empty());
    NWebConfigHelper::Instance().perfConfig_.emplace("web/test", "web_test");
    config = NWebAdapterHelper::Instance().ParsePerfConfig("web", "test");
    EXPECT_FALSE(config.empty());
}

/**
 * @tc.name: NWebHelper_GetDataBase_003
 * @tc.desc: GetDataBase.
 * @tc.type: FUNC
 * @tc.require:issueI5OESN
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetDataBase_003, TestSize.Level1)
{
    auto dataBase = NWebHelper::Instance().GetDataBase();
    bool result = false;
    if (dataBase != nullptr) {
        result = true;
    }
    EXPECT_EQ(RESULT_OK, result);

    std::shared_ptr<NWebCookieManager> cook = NWebHelper::Instance().GetCookieManager();
    EXPECT_NE(cook, nullptr);

    void *enhanceSurfaceInfo = nullptr;
    int32_t temp = 1;
    std::shared_ptr<NWeb> nweb =
        NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    enhanceSurfaceInfo = static_cast<void *>(&temp);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, NWEB_MAX_WIDTH);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    NWEB_MAX_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
}

/**
 * @tc.name: NWebHelper_GetConfigPath_005
 * @tc.desc: GetConfigPath.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetConfigPath_005, TestSize.Level1)
{
    std::string configFileName = "test";
    std::string figPath = NWebConfigHelper::Instance().GetConfigPath(configFileName);
    EXPECT_FALSE(figPath.empty());
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    NWebAdapterHelper::Instance().ParseConfig(initArgs);
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
    NWebHelper::Instance().WarmupServiceWorker("web_test");
    NWebHelper::Instance().PrefetchResource(nullptr, {}, "web_test", 0);
    NWebHelper::Instance().ClearPrefetchedResource({"web_test"});
    NWebHelper::Instance().bundlePath_.clear();
    NWebHelper::Instance().EnableBackForwardCache(true, true);
    NWebHelper::Instance().SetCustomSchemeCmdLine("single-process");
    bool result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_TRUE(result);
    NWebHelper::Instance().SetConnectionTimeout(1);
    NWebHelper::Instance().LoadWebEngine(true, false);

    // To test SetRenderProcessMode and GetRenderProcessMode.
    NWebHelper::Instance().SetRenderProcessMode(RenderProcessMode::SINGLE_MODE);
    RenderProcessMode render_process_mode =
        NWebHelper::Instance().GetRenderProcessMode();
    EXPECT_EQ(render_process_mode, RenderProcessMode::SINGLE_MODE);
    NWebHelper::Instance().SetRenderProcessMode(RenderProcessMode::MULTIPLE_MODE);
    render_process_mode = NWebHelper::Instance().GetRenderProcessMode();
    EXPECT_EQ(render_process_mode, RenderProcessMode::MULTIPLE_MODE);

    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().SetRenderProcessMode(RenderProcessMode::MULTIPLE_MODE);
    EXPECT_EQ(NWebHelper::Instance().GetRenderProcessMode(), RenderProcessMode::SINGLE_MODE);
}

/**
 * @tc.name: NWebHelper_LoadNWebSDK_006
 * @tc.desc: LoadNWebSDK.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_LoadNWebSDK_006, TestSize.Level1)
{
    std::shared_ptr<NWebCreateInfo> create_info = std::make_shared<NWebCreateInfoImpl>();
    NWebHelper::Instance().SetBundlePath(MOCK_NWEB_INSTALLATION_DIR);
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_EQ(RESULT_OK, result);
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    static WebDownloadDelegateCallback *downloadCallback;
    WebDownloader_CreateDownloadDelegateCallback(&downloadCallback);
    EXPECT_NE(downloadCallback, nullptr);
    OnDownloadBeforeStart fun = [] (NWebDownloadItem *downloadItem, WebBeforeDownloadCallbackWrapper *wrapper) {};
    WebDownloader_SetDownloadBeforeStart(downloadCallback, fun);
    WebDownloadManager_PutDownloadCallback(downloadCallback);
    OnDownloadDidUpdate didUpdate = [] (NWebDownloadItem *downloadItem, WebDownloadItemCallbackWrapper *wrapper) {};
    WebDownloader_SetDownloadDidUpdate(downloadCallback, didUpdate);
    NWebDownloadItem *downloadItem = nullptr;
    WebDownloadItem_CreateWebDownloadItem(&downloadItem);
    EXPECT_NE(downloadItem, nullptr);
    WebDownloader_ResumeDownloadStatic(downloadItem);
    WebDownloader_StartDownload(1, "test_web");
    WebDownload_Continue(nullptr, "test_web");
    WebDownload_CancelBeforeDownload(nullptr);
    WebDownload_PauseBeforeDownload(nullptr);
    WebDownload_ResumeBeforeDownload(nullptr);
    WebDownload_Cancel(nullptr);
    WebDownload_Pause(nullptr);
    WebDownload_Resume(nullptr);
    long itemId = WebDownloadItem_GetDownloadItemId(downloadItem);
    EXPECT_NE(itemId, -1);
    WebDownloadItem_GetState(nullptr);
    NWebDownloadItem *download = nullptr;
    int speed = WebDownloadItem_CurrentSpeed(download);
    EXPECT_EQ(speed, 0);
    int complete = WebDownloadItem_PercentComplete(download);
    EXPECT_EQ(complete, 0);
    WebDownloadItem_SetReceivedBytes(downloadItem, 1);
    WebDownloadItem_TotalBytes(downloadItem);
    int64_t receivedBytes = WebDownloadItem_ReceivedBytes(downloadItem);
    EXPECT_NE(receivedBytes, 0);
    char* originalUrl = WebDownloadItem_OriginalUrl(downloadItem);
    EXPECT_EQ(originalUrl, nullptr);
    char* fileName = WebDownloadItem_SuggestedFileName(downloadItem);
    EXPECT_EQ(fileName, nullptr);
    char* disposition = WebDownloadItem_ContentDisposition(downloadItem);
    EXPECT_EQ(disposition, nullptr);
}

/**
 * @tc.name: NWebHelper_WebDownloadItem_IsPaused_007
 * @tc.desc: WebDownloadItem_IsPaused.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_WebDownloadItem_IsPaused_007, TestSize.Level1)
{
    bool result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    NWebDownloadItem *downloadItem = nullptr;
    WebDownloadItem_CreateWebDownloadItem(&downloadItem);
    EXPECT_NE(downloadItem, nullptr);
    NWebDownloadItem *download = nullptr;
    bool isPaused = WebDownloadItem_IsPaused(download);
    EXPECT_FALSE(isPaused);
    char* method = WebDownloadItem_Method(downloadItem);
    EXPECT_EQ(method, nullptr);
    WebDownloadItem_LastErrorCode(downloadItem);
    char* receivedSlices = WebDownloadItem_ReceivedSlices(downloadItem);
    EXPECT_EQ(receivedSlices, nullptr);
    char* lastModified = WebDownloadItem_LastModified(downloadItem);
    EXPECT_EQ(lastModified, nullptr);
    int nWebId = WebDownloadItem_NWebId(download);
    EXPECT_EQ(nWebId, 0);
    WebDownloadItem_Destroy(downloadItem);
    DestroyBeforeDownloadCallbackWrapper(nullptr);
    DestroyDownloadItemCallbackWrapper(nullptr);
    WebDownloadItem_SetGuid(downloadItem, "test_web");
    WebDownloadItem_SetUrl(downloadItem, "test_web");
    WebDownloadItem_SetFullPath(downloadItem, "test_web");
    WebDownloadItem_SetETag(downloadItem, "test_web");
    WebDownloadItem_SetLastModified(downloadItem, "test_web");
    WebDownloadItem_SetMimeType(downloadItem, "test_web");
    WebDownloadItem_SetReceivedBytes(downloadItem, 1);
    WebDownloadItem_SetTotalBytes(downloadItem, 1);
    WebDownloadItem_SetReceivedSlices(downloadItem, "test_web");
    char* guid = WebDownloadItem_Guid(downloadItem);
    EXPECT_NE(guid, nullptr);
    int64_t totalBytes = WebDownloadItem_TotalBytes(downloadItem);
    EXPECT_NE(totalBytes, 0);
    int64_t receivedBytes = WebDownloadItem_ReceivedBytes(downloadItem);
    EXPECT_NE(receivedBytes, 0);
    char* fullPath = WebDownloadItem_FullPath(downloadItem);
    EXPECT_NE(fullPath, nullptr);
    char* url = WebDownloadItem_Url(downloadItem);
    EXPECT_NE(url, nullptr);
    char* eTag = WebDownloadItem_ETag(downloadItem);
    EXPECT_NE(eTag, nullptr);
    char* mimeType = WebDownloadItem_MimeType(downloadItem);
    EXPECT_NE(mimeType, nullptr);
    long itemId = WebDownloadItem_GetDownloadItemId(downloadItem);
    auto state1 = WebDownload_GetItemState(nWebId, itemId);
    auto state2 = WebDownload_GetItemStateByGuid("test_web");
    WebDownload_GetItemStateByGuidV2("test_web");
    EXPECT_EQ(state1, state2);
}

/**
 * @tc.name: NWebHelper_LoadWebEngine_008
 * @tc.desc: LoadWebEngine.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_LoadWebEngine_008, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebHelper::Instance().GetNWeb(1);
    EXPECT_EQ(nweb, nullptr);
    std::shared_ptr<NWebCookieManager> cook = NWebHelper::Instance().GetCookieManager();
    EXPECT_NE(cook, nullptr);
    auto manager = NWebHelper::Instance().GetAdsBlockManager();
    EXPECT_NE(manager, nullptr);
    std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
    NWebHelper::Instance().SetHttpDns(config);
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
    NWebHelper::Instance().WarmupServiceWorker("web_test");
    NWebHelper::Instance().GetDataBase();
    std::shared_ptr<NWebWebStorage> storage = NWebHelper::Instance().GetWebStorage();
    EXPECT_NE(storage, nullptr);
    NWebHelper::Instance().SetConnectionTimeout(1);
    std::vector<std::string> hosts;
    NWebHelper::Instance().AddIntelligentTrackingPreventionBypassingList(hosts);
    NWebHelper::Instance().RemoveIntelligentTrackingPreventionBypassingList(hosts);
    NWebHelper::Instance().ClearIntelligentTrackingPreventionBypassingList();
    NWebHelper::Instance().GetDefaultUserAgent();
    NWebHelper::Instance().SetAppCustomUserAgent("web_test");
    NWebHelper::Instance().SetUserAgentForHosts("web_test", hosts);
    NWebHelper::Instance().PauseAllTimers();
    NWebHelper::Instance().ResumeAllTimers();
    NWebHelper::Instance().SetWebDestroyMode(WebDestroyMode::NORMAL_MODE);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
    NWebHelper::Instance().LoadWebEngine(true, false);
    bool result = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_TRUE(result);
    result = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_TRUE(result);
    cook = NWebHelper::Instance().GetCookieManager();
    EXPECT_NE(cook, nullptr);
    manager = NWebHelper::Instance().GetAdsBlockManager();
    EXPECT_NE(manager, nullptr);
    NWebHelper::Instance().SetWebTag(1, "webtag");

    NWebHelper::Instance().SetWebTag(1, "webtag");
    NWebHelper::Instance().AddIntelligentTrackingPreventionBypassingList(hosts);
    NWebHelper::Instance().RemoveIntelligentTrackingPreventionBypassingList(hosts);
    NWebHelper::Instance().ClearIntelligentTrackingPreventionBypassingList();
    NWebHelper::Instance().GetDefaultUserAgent();
    NWebHelper::Instance().SetAppCustomUserAgent("web_test");
    NWebHelper::Instance().SetUserAgentForHosts("web_test", hosts);
    NWebHelper::Instance().PauseAllTimers();
    NWebHelper::Instance().ResumeAllTimers();
    NWebHelper::Instance().SetWebDestroyMode(WebDestroyMode::NORMAL_MODE);
    NWebHelper::Instance().SetWebDebuggingAccess(true);
    NWebHelper::Instance().SetWebDebuggingAccessAndPort(true, 80);
    NWebHelper::Instance().SaveSchemeVector("web_test", 8080);
    result = NWebHelper::Instance().HasLoadWebEngine();
    EXPECT_TRUE(result);
    result = NWebHelper::Instance().RegisterCustomSchemes();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NWebHelper_GetPerfConfig_001
 * @tc.desc: GetPerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetPerfConfig_001, TestSize.Level1)
{
    EXPECT_TRUE(NWebAdapterHelper::Instance().GetPerfConfig("test").empty());
    NWebConfigHelper::Instance().ltpoConfig_["test"] = {OHOS::NWeb::FrameRateSetting{0, 0, 0}};
    EXPECT_FALSE(NWebAdapterHelper::Instance().GetPerfConfig("test").empty());
    NWebConfigHelper::Instance().ltpoConfig_.clear();
}

/**
 * @tc.name: NWebHelper_ParseNWebLTPOConfig_001
 * @tc.desc: ParseNWebLTPOConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_ParseNWebLTPOConfig_001, TestSize.Level1)
{
    EXPECT_TRUE(NWebConfigHelper::Instance().ltpoConfig_.empty());
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    NWebAdapterHelper::Instance().ParseConfig(initArgs);
    EXPECT_TRUE(NWebConfigHelper::Instance().ltpoConfig_.empty());
}

/**
 * @tc.name: NWebHelper_SetHostIP_001
 * @tc.desc: SetHostIP.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetHostIP_001, TestSize.Level1)
{
    std::string hostName = "hello";
    std::string address = "world";
    int32_t aliveTime = 0;

    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().SetHostIP(hostName, address, aliveTime);
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);

    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().SetHostIP(hostName, address, aliveTime);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().nwebEngine_ = nullptr;
}

/**
 * @tc.name: NWebHelper_ClearHostIP_001
 * @tc.desc: ClearHostIP.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_ClearHostIP_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    std::string hostName = "name";
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().ClearHostIP(hostName);
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);

    auto nwebengineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebengineMock;
    NWebHelper::Instance().ClearHostIP(hostName);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().nwebEngine_ = nullptr;
}

/**
 * @tc.name: NWebHelper_EnableWholeWebPageDrawing_001
 * @tc.desc: EnableWholeWebPageDrawing.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_EnableWholeWebPageDrawing_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    auto nwebengineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebengineMock;
    NWebHelper::Instance().EnableWholeWebPageDrawing();
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().nwebEngine_ = nullptr;
}

/**
 * @tc.name: NWebHelper_GetAdsBlockManager_001
 * @tc.desc: GetAdsBlockManager.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetAdsBlockManager_001, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebHelper::Instance().GetNWeb(1);
    EXPECT_EQ(nweb, nullptr);
    auto manager = NWebHelper::Instance().GetAdsBlockManager();
    EXPECT_NE(manager, nullptr);
}

/**
 * @tc.name: NWebHelper_TrimMemoryByPressureLevel_001
 * @tc.desc: TrimMemoryByPressureLevel.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_TrimMemoryByPressureLevel_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    int32_t memoryLevel = 1;
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().TrimMemoryByPressureLevel(memoryLevel);
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);

    auto nwebengineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebengineMock;
    NWebHelper::Instance().TrimMemoryByPressureLevel(memoryLevel);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().nwebEngine_ = nullptr;
}

/**
 * @tc.name: NWebHelper_ParseNWebLTPOApp_001
 * @tc.desc: ParseNWebLTPOApp.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_ParseNWebLTPOApp_001, TestSize.Level1)
{
    EXPECT_TRUE(NWebConfigHelper::Instance().ltpoAllowedApps_.empty());
    EXPECT_FALSE(NWebConfigHelper::Instance().IsLTPODynamicApp(""));
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    NWebAdapterHelper::Instance().ParseConfig(initArgs);
    EXPECT_TRUE(NWebConfigHelper::Instance().ltpoAllowedApps_.empty());
    EXPECT_FALSE(NWebConfigHelper::Instance().IsLTPODynamicApp(""));
    EXPECT_FALSE(NWebAdapterHelper::Instance().IsLTPODynamicApp(""));

    NWebConfigHelper::Instance().ltpoStrategy_ = LTPO_STRATEGY;
    NWebAdapterHelper::Instance().ParseConfig(initArgs);
    EXPECT_EQ(NWebConfigHelper::Instance().GetLTPOStrategy(), LTPO_STRATEGY);
    EXPECT_EQ(NWebAdapterHelper::Instance().GetLTPOStrategy(), LTPO_STRATEGY);
}

/**
 * @tc.name: NWebHelper_GetWebEngine_001
 * @tc.desc: GetWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetWebEngine_001, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    g_applicationContext.reset();
    bool result = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_FALSE(result);
    ApplicationContextMock *contextMock = new ApplicationContextMock();
    ASSERT_NE(contextMock, nullptr);
    g_applicationContext.reset(contextMock);
    result = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NWebHelper_RemoveAllCache
 * @tc.desc: RemoveAllCache.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_RemoveAllCache_001, TestSize.Level1)
{
    bool result = NWebAdapterHelper::Instance().Init(false);
    EXPECT_TRUE(result);
    NWebHelper::Instance().LoadWebEngine(true, false);
    bool ret = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_TRUE(ret);

    NWebHelper::Instance().RemoveAllCache(true);
}

} // namespace OHOS::NWeb
}
