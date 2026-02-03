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
#include <unordered_map>
#include "hilog/log.h"

#define private public
#include "nweb.h"
#include "nweb_helper.h"
#include "nweb_config_helper.h"
#include "nweb_adapter_helper.h"
#include "nweb_create_window.h"
#include "nweb_c_api.h"
#include "nweb_init_params.h"
#include "application_context.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::Rosen;
using namespace OHOS::AbilityRuntime;

namespace OHOS {
namespace {
const int DEFAULT_WIDTH = 2560;
const int DEFAULT_HEIGHT = 1396;
const int32_t MAX_WIDTH = 7681;
const int32_t LTPO_STRATEGY = 1;
const uint32_t MAX_URLS_COUNT = 100;
const std::string INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.arkwebcore";
std::shared_ptr<AbilityRuntime::ApplicationContext> g_applicationContext = nullptr;
} // namespace

namespace {
    std::string g_errlog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        g_errlog += msg;
    }
}

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

    void SetRenderProcessMode(RenderProcessMode mode)
    {
        process_mode_ = mode;
    }

    RenderProcessMode GetRenderProcessMode()
    {
        return process_mode_;
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

    void EnablePrivateNetworkAccess(bool enable) {}

    bool IsPrivateNetworkAccessEnabled()
    {
        return false;
    }

    void SetSocketIdleTimeout(int32_t timeout) {}

    void LibraryLoaded(std::shared_ptr<NWebEngineInitArgs> init_args, bool lazy) {}

    MOCK_METHOD(void, SetWebDebuggingAccessAndPort,
        (bool isEnableDebug, int32_t port), (override));

private:
    RenderProcessMode process_mode_ = RenderProcessMode::SINGLE_MODE;
};

void NwebHelperTest::SetUpTestCase(void)
{}

void NwebHelperTest::TearDownTestCase(void)
{}

void NwebHelperTest::SetUp(void)
{}

void NwebHelperTest::TearDown(void)
{}

std::unordered_map<std::string, std::string> g_argsMap;

/**
 * @tc.name: NWebHelper_GetWebStorage_002
 * @tc.desc: GetWebStorage.
 * @tc.type: FUNC
 * @tc.require: AR000GGHJ8
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetWebStorage_002, TestSize.Level1)
{
    auto web_storage = NWebHelper::Instance().GetWebStorage();
    EXPECT_EQ(web_storage, nullptr);
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    auto web_storage1 = NWebHelper::Instance().GetWebStorage();
    EXPECT_EQ(web_storage1, nullptr);
    NWebAdapterHelper::Instance().ReadConfigIfNeeded();
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
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    auto dataBase = NWebHelper::Instance().GetDataBase();
    EXPECT_EQ(dataBase, nullptr);

    std::shared_ptr<NWebCookieManager> cook = NWebHelper::Instance().GetCookieManager();
    EXPECT_EQ(cook, nullptr);
    NWebHelper::Instance().initFlag_ = true;
    EXPECT_EQ(NWebHelper::Instance().GetCookieManager(), nullptr);
    NWebHelper::Instance().initFlag_ = false;
    
    // Test NWebAdapterHelper::CreateNWeb
    void *enhanceSurfaceInfo = nullptr;
    int32_t temp = 1;
    std::shared_ptr<NWeb> nweb =
        NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
        DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    enhanceSurfaceInfo = static_cast<void *>(&temp);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, MAX_WIDTH);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    MAX_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    NWebHelper::Instance().nwebEngine_ = nullptr;
    nweb = NWebAdapterHelper::Instance().CreateNWeb(enhanceSurfaceInfo, GetInitArgs(),
                                                    DEFAULT_WIDTH, DEFAULT_HEIGHT);
    EXPECT_EQ(nweb, nullptr);
    EXPECT_EQ(NWebHelper::Instance().GetDataBase(), nullptr);
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
    NWebHelper::Instance().SetBundlePath(INSTALLATION_DIR);
    NWebHelper::Instance().SetAutoPreconnect(true);
    bool result = NWebHelper::Instance().InitAndRun(false);
    EXPECT_FALSE(result);
    NWebHelper::Instance().SetConnectionTimeout(1);
    NWebHelper::Instance().LoadWebEngine(true, false);

    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().PrefetchResource(nullptr, {}, "web_test", 0);
    NWebHelper::Instance().ClearPrefetchedResource({"web_test"});
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
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP();
    }
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    bool result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    static WebDownloadDelegateCallback *downloadCallback;
    WebDownloader_CreateDownloadDelegateCallback(&downloadCallback);
    EXPECT_EQ(downloadCallback, nullptr);
    OnDownloadBeforeStart fun = [] (NWebDownloadItem *downloadItem, WebBeforeDownloadCallbackWrapper *wrapper) {};
    WebDownloader_SetDownloadBeforeStart(downloadCallback, fun);
    WebDownloadManager_PutDownloadCallback(downloadCallback);
    OnDownloadDidUpdate didUpdate = [] (NWebDownloadItem *downloadItem, WebDownloadItemCallbackWrapper *wrapper) {};
    WebDownloader_SetDownloadDidUpdate(downloadCallback, didUpdate);
    NWebDownloadItem *downloadItem = nullptr;
    WebDownloadItem_CreateWebDownloadItem(&downloadItem);
    EXPECT_EQ(downloadItem, nullptr);
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
    EXPECT_EQ(receivedBytes, 0);
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
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP();
    }
    NWebHelper::Instance().nwebEngine_ = std::make_shared<MockNWebEngine>();
    bool result = NWebHelper::Instance().LoadNWebSDK();
    EXPECT_TRUE(result);
    NWebDownloadItem *downloadItem = nullptr;
    WebDownloadItem_CreateWebDownloadItem(&downloadItem);
    EXPECT_EQ(downloadItem, nullptr);
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
    EXPECT_EQ(nWebId, -1);
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
    EXPECT_EQ(guid, nullptr);
    int64_t totalBytes = WebDownloadItem_TotalBytes(downloadItem);
    EXPECT_EQ(totalBytes, 0);
    int64_t receivedBytes = WebDownloadItem_ReceivedBytes(downloadItem);
    EXPECT_EQ(receivedBytes, 0);
    char* fullPath = WebDownloadItem_FullPath(downloadItem);
    EXPECT_EQ(fullPath, nullptr);
    char* url = WebDownloadItem_Url(downloadItem);
    EXPECT_EQ(url, nullptr);
    char* eTag = WebDownloadItem_ETag(downloadItem);
    EXPECT_EQ(eTag, nullptr);
    char* mimeType = WebDownloadItem_MimeType(downloadItem);
    EXPECT_EQ(mimeType, nullptr);
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
    if (!Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP();
    }
    NWebHelper::Instance().nwebEngine_ = nullptr;
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebHelper::Instance().GetNWeb(1);
    EXPECT_EQ(nweb, nullptr);
    NWebHelper::Instance().SetWebTag(1, "webtag");
    std::shared_ptr<NWebDOHConfigImpl> config = std::make_shared<NWebDOHConfigImpl>();
    NWebHelper::Instance().SetHttpDns(config);
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
    NWebHelper::Instance().SetScrollbarMode(ScrollbarMode::OVERLAY_LAYOUT_SCROLLBAR);
    NWebHelper::Instance().SetSiteIsolationMode(SiteIsolationMode::PARTIAL);
    NWebHelper::Instance().GetSiteIsolationMode();

    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().SetHttpDns(config);
    NWebHelper::Instance().PrepareForPageLoad("web_test", true, 0);
    NWebHelper::Instance().WarmupServiceWorker("web_test");
    NWebHelper::Instance().GetDataBase();
    NWebHelper::Instance().SetConnectionTimeout(1);
    NWebHelper::Instance().AddIntelligentTrackingPreventionBypassingList(hosts);
    NWebHelper::Instance().RemoveIntelligentTrackingPreventionBypassingList(hosts);
    NWebHelper::Instance().ClearIntelligentTrackingPreventionBypassingList();
    NWebHelper::Instance().GetDefaultUserAgent();
    NWebHelper::Instance().SetAppCustomUserAgent("web_test");
    NWebHelper::Instance().SetUserAgentForHosts("web_test", hosts);
    NWebHelper::Instance().PauseAllTimers();
    NWebHelper::Instance().ResumeAllTimers();
    NWebHelper::Instance().SetWebDestroyMode(WebDestroyMode::NORMAL_MODE);
    NWebHelper::Instance().SetSiteIsolationMode(SiteIsolationMode::PARTIAL);
    NWebHelper::Instance().GetSiteIsolationMode();
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
    NWebHelper::Instance().LoadWebEngine(true, false);
    bool result = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_TRUE(result);
    NWebHelper::Instance().SetWebTag(1, "webtag");
    NWebHelper::Instance().SetWebDebuggingAccess(true);
    NWebHelper::Instance().SetWebDebuggingAccessAndPort(true, 80);
    NWebHelper::Instance().SaveSchemeVector("web_test", 8080);
    result = NWebHelper::Instance().HasLoadWebEngine();
    EXPECT_TRUE(result);
    result = NWebHelper::Instance().RegisterCustomSchemes();
    EXPECT_FALSE(result);
}

/**
 * @tc.name: NWebHelper_LoadWebEngine_009
 * @tc.desc: LoadWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_LoadWebEngine_009, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebHelper::Instance().GetNWeb(1);
    EXPECT_EQ(nweb, nullptr);
    NWebHelper::Instance().SetWebTag(1, "webtag");
    NWebHelper::Instance().SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode::DISABLE_AUTO_KEYBOARD_ON_ACTIVE);

    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().SetSoftKeyboardBehaviorMode(WebSoftKeyboardBehaviorMode::DISABLE_AUTO_KEYBOARD_ON_ACTIVE);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
    NWebHelper::Instance().LoadWebEngine(true, false);
    bool result = NWebHelper::Instance().GetWebEngine(true);
    EXPECT_TRUE(result);
    NWebHelper::Instance().SetWebTag(1, "webtag");
    result = NWebHelper::Instance().HasLoadWebEngine();
    EXPECT_TRUE(result);
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
    NWebHelper::Instance().EnableWholeWebPageDrawing();
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
    NWebHelper::Instance().bundlePath_ = "";
    std::shared_ptr<NWebCreateInfoImpl> create_info = std::make_shared<NWebCreateInfoImpl>();
    std::shared_ptr<NWeb> nweb = NWebHelper::Instance().CreateNWeb(create_info);
    EXPECT_EQ(nweb, nullptr);
    nweb = NWebHelper::Instance().GetNWeb(1);
    EXPECT_EQ(nweb, nullptr);
    auto manager = NWebHelper::Instance().GetAdsBlockManager();
    EXPECT_EQ(manager, nullptr);
    auto nwebengineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebengineMock;
    auto manager1 = NWebHelper::Instance().GetAdsBlockManager();
    EXPECT_EQ(manager1, nullptr);
    NWebHelper::Instance().nwebEngine_ = nullptr;
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
    EXPECT_EQ(NWebHelper::Instance().GetNWeb(nweb_id), nullptr);
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
 * @tc.name: NWebHelper_InitWebEngine
 * @tc.desc: InitWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_InitWebEngine, TestSize.Level1)
{
    NWebHelper::Instance().initFlag_ = true;
    bool resultBool = NWebHelper::Instance().InitWebEngine();
    EXPECT_TRUE(resultBool);
    NWebHelper::Instance().initFlag_ = false;
    resultBool = NWebHelper::Instance().InitWebEngine();
    EXPECT_FALSE(resultBool);
    ApplicationContextMock *contextMock = new ApplicationContextMock();
    ASSERT_NE(contextMock, nullptr);
    g_applicationContext.reset(contextMock);
    resultBool = NWebHelper::Instance().InitWebEngine();
    EXPECT_FALSE(resultBool);
}

/**
 * @tc.name: NWebHelper_SetProxyOverride
 * @tc.desc: SetProxyOverride.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetProxyOverride, TestSize.Level1)
{
    std::vector<std::string> proxyUrls = {"http://127.0.0.1:8080"};
    std::vector<std::string> proxySchemeFilters = {"http", "https"};
    std::vector<std::string> bypassRules = {"localhost", "127.0.0.1"};
    bool reverseBypass = false;
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    NWebHelper::Instance().SetProxyOverride(proxyUrls, proxySchemeFilters, bypassRules, reverseBypass, nullptr);
    NWebHelper::Instance().RemoveProxyOverride(nullptr);
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().initFlag_ = true;
    NWebHelper::Instance().RemoveProxyOverride(nullptr);
}

/**
 * @tc.name: NWebHelper_RemoveAllCache
 * @tc.desc: RemoveAllCache.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_RemoveAllCache, TestSize.Level1)
{
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().initFlag_ = true;
    bool includeDiskFiles = true;
    NWebHelper::Instance().RemoveAllCache(includeDiskFiles);
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().bundlePath_ = "";
    NWebHelper::Instance().RemoveAllCache(includeDiskFiles);
}

/**
 * @tc.name: NWebHelper_GetBundleName_001
 * @tc.desc: GetBundleName.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetBundleName_001, TestSize.Level1)
{
    std::string bundleName = NWebAdapterHelper::Instance().GetBundleName();
    EXPECT_TRUE(bundleName.empty());
    NWebConfigHelper::Instance().SetBundleName("testBundleName");
    bundleName = NWebAdapterHelper::Instance().GetBundleName();
    EXPECT_FALSE(bundleName.empty());
}

/**
 * @tc.name: SetWebDebuggingAccess
 * @tc.desc: SetWebDebuggingAccess and SetWebDebuggingAccessAndPort.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, SetWebDebuggingAccess, TestSize.Level1)
{
    NWebHelper::Instance().initFlag_ = false;
    NWebHelper::Instance().nwebEngine_ = nullptr;

    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    bool isEnableDebug = true;
    int32_t port = 8888;
    EXPECT_CALL(*nwebEngineMock, SetWebDebuggingAccessAndPort(isEnableDebug, port)).Times(1);

    NWebHelper::Instance().SetWebDebuggingAccess(isEnableDebug);
    NWebHelper::Instance().SetWebDebuggingAccessAndPort(isEnableDebug, port);

    NWebHelper::Instance().initFlag_ = true;
    NWebHelper::Instance().SetWebDebuggingAccess(isEnableDebug);
    NWebHelper::Instance().SetWebDebuggingAccessAndPort(isEnableDebug, port);

    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().SetWebDebuggingAccess(isEnableDebug);
    NWebHelper::Instance().SetWebDebuggingAccessAndPort(isEnableDebug, port);
}

/**
 * @tc.name: NWebHelper_SetBlanklessLoadingCacheCapacity_001
 * @tc.desc: test SetBlanklessLoadingCacheCapacity.
 * @tc.type: FUNC
 * @tc.require: ICACAG
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetBlanklessLoadingCacheCapacity_001, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(-1);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(0);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(10);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(30);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(100);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(1000);
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);
}

/**
 * @tc.name: NWebHelper_SetBlanklessLoadingCacheCapacity_002
 * @tc.desc: test SetBlanklessLoadingCacheCapacity.
 * @tc.type: FUNC
 * @tc.require: ICACAG
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetBlanklessLoadingCacheCapacity_002, TestSize.Level1)
{
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(-1);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(0);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(10);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(30);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(100);
    NWebHelper::Instance().SetBlanklessLoadingCacheCapacity(1000);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
}

/**
 * @tc.name: NWebHelper_ClearBlanklessLoadingCache_001
 * @tc.desc: test ClearBlanklessLoadingCache.
 * @tc.type: FUNC
 * @tc.require: ICACAG
 */
HWTEST_F(NwebHelperTest, NWebHelper_ClearBlanklessLoadingCache_001, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    std::vector<std::string> blankLessLoadingCache;
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    blankLessLoadingCache.push_back("www.ClearBlanklessLoadingCache1.com");
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    blankLessLoadingCache.push_back("www.ClearBlanklessLoadingCache2.com");
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    for (uint32_t idx = 0; idx < MAX_URLS_COUNT; idx++) {
        blankLessLoadingCache.push_back("www.ClearBlanklessLoadingCache3.com");
    }
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);
}

/**
 * @tc.name: NWebHelper_ClearBlanklessLoadingCache_002
 * @tc.desc: test ClearBlanklessLoadingCache.
 * @tc.type: FUNC
 * @tc.require: ICACAG
 */
HWTEST_F(NwebHelperTest, NWebHelper_ClearBlanklessLoadingCache_002, TestSize.Level1)
{
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    std::vector<std::string> blankLessLoadingCache;
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    blankLessLoadingCache.push_back("www.ClearBlanklessLoadingCache1.com");
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    blankLessLoadingCache.push_back("www.ClearBlanklessLoadingCache2.com");
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    for (uint32_t idx = 0; idx < MAX_URLS_COUNT; idx++) {
        blankLessLoadingCache.push_back("www.ClearBlanklessLoadingCache3.com");
    }
    NWebHelper::Instance().ClearBlanklessLoadingCache(blankLessLoadingCache);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
}

/**
 * @tc.name: NWebHelper_CheckBlankOptEnable_001
 * @tc.desc: test CheckBlankOptEnable.
 * @tc.type: FUNC
 * @tc.require: ICACAG
 */
HWTEST_F(NwebHelperTest, NWebHelper_CheckBlankOptEnable_001, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    EXPECT_EQ(NWebHelper::Instance().CheckBlankOptEnable("example", 0), "");
}

/**
 * @tc.name: NWebHelper_CheckBlankOptEnable_002
 * @tc.desc: test CheckBlankOptEnable.
 * @tc.type: FUNC
 * @tc.require: ICACAG
 */
HWTEST_F(NwebHelperTest, NWebHelper_CheckBlankOptEnable_002, TestSize.Level1)
{
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    EXPECT_EQ(NWebHelper::Instance().CheckBlankOptEnable("example", 0), "");
}

/**
 * @tc.name: NWebHelper_EnablePrivateNetworkAccess_001
 * @tc.desc: EnablePrivateNetworkAccess.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_EnablePrivateNetworkAccess_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    g_errlog.clear();
    LOG_SetCallback(MyLogCallback);
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().EnablePrivateNetworkAccess(true);
    EXPECT_TRUE(g_errlog.find("web engine is nullptr") != std::string::npos);

    auto nwebengineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebengineMock;
    NWebHelper::Instance().EnablePrivateNetworkAccess(true);
    NWebHelper::Instance().EnablePrivateNetworkAccess(false);
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: NWebHelper_IsPrivateNetworkAccessEnabled_001
 * @tc.desc: IsPrivateNetworkAccessEnabled.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_IsPrivateNetworkAccessEnabled_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    g_errlog.clear();
    LOG_SetCallback(MyLogCallback);
    NWebHelper::Instance().nwebEngine_ = nullptr;
    NWebHelper::Instance().IsPrivateNetworkAccessEnabled();
    EXPECT_TRUE(g_errlog.find("web engine is nullptr") != std::string::npos);

    auto nwebengineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebengineMock;
    NWebHelper::Instance().IsPrivateNetworkAccessEnabled();
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: NWebHelper_SetAutoPreconnect_001
 * @tc.desc: SetAutoPreconnect.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetAutoPreconnect_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    NWebHelper::Instance().SetAutoPreconnect(true);
    EXPECT_EQ(NWebHelper::Instance().autoPreconnectEnabled_, true);
    NWebHelper::Instance().SetAutoPreconnect(false);
    EXPECT_EQ(NWebHelper::Instance().autoPreconnectEnabled_, false);
}

/**
 * @tc.name: NWebHelper_IsAutoPreconnectEnabled_001
 * @tc.desc: IsAutoPreconnectEnabled.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_IsAutoPreconnectEnabled_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    NWebHelper::Instance().autoPreconnectEnabled_ = true;
    EXPECT_EQ(NWebHelper::Instance().IsAutoPreconnectEnabled(), true);
    NWebHelper::Instance().autoPreconnectEnabled_ = false;
    EXPECT_EQ(NWebHelper::Instance().IsAutoPreconnectEnabled(), false);
}

/**
 * @tc.name: NWebHelper_SetSocketIdleTimeout_001
 * @tc.desc: SetSocketIdleTimeout.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetSocketIdleTimeout_001, TestSize.Level1)
{
    NWebHelper::Instance().nwebEngine_ = nullptr;
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().SetSocketIdleTimeout(100);
    EXPECT_EQ(NWebHelper::Instance().socketIdleTimeout_, 100);
}

/**
 * @tc.name: NWebHelper_SetSocketIdleTimeout_002
 * @tc.desc: SetSocketIdleTimeout.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetSocketIdleTimeout_002, TestSize.Level1)
{
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().SetSocketIdleTimeout(200);
    EXPECT_EQ(NWebHelper::Instance().socketIdleTimeout_, 100);
}

/**
 * @tc.name: NWebHelper_GetSocketIdleTimeout_001
 * @tc.desc: GetSocketIdleTimeout.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetSocketIdleTimeout_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);

    NWebHelper::Instance().SetSocketIdleTimeout(100);
    EXPECT_EQ(NWebHelper::Instance().GetSocketIdleTimeout(), 100);
}

/**
 * @tc.name: NWebHelper_CreateNWeb
 * @tc.desc: CreateNWeb
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_CreateNWeb, TestSize.Level1)
{
    g_errlog.clear();
    LOG_SetCallback(MyLogCallback); 
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "webTestSurfaceName";
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<Surface> surPtr = surfaceNode->GetSurface();
    ASSERT_NE(surPtr, nullptr);
    ASSERT_NE(GetInitArgs(), nullptr);
    uint32_t width = 7880;
    uint32_t height = 7880;
    std::shared_ptr<NWeb> nweb = 
        NWebAdapterHelper::Instance().CreateNWeb(surPtr, GetInitArgs(), width, height);
    EXPECT_TRUE(g_errlog.find("is invalid") != std::string::npos);
    EXPECT_EQ(nweb, nullptr);

    g_errlog.clear();
    width = 780;
    nweb = NWebAdapterHelper::Instance().CreateNWeb(surPtr, GetInitArgs(), width, height);
    EXPECT_TRUE(g_errlog.find("is invalid") != std::string::npos);
    EXPECT_EQ(nweb, nullptr);

    g_errlog.clear();
    width = 7880;
    height = 780;
    nweb = NWebAdapterHelper::Instance().CreateNWeb(surPtr, GetInitArgs(), width, height);
    EXPECT_TRUE(g_errlog.find("is invalid") != std::string::npos);
    EXPECT_EQ(nweb, nullptr);

    g_errlog.clear();
    bool ret = NWebHelper::Instance().autoPreconnectEnabled_;
    width = 780;
    height = 780;
    NWebHelper::Instance().autoPreconnectEnabled_ = false;
    nweb = NWebAdapterHelper::Instance().CreateNWeb(surPtr, GetInitArgs(), width, height);
    EXPECT_FALSE(g_errlog.find("is invalid") != std::string::npos);

    g_errlog.clear();
    NWebHelper::Instance().autoPreconnectEnabled_ = true;
    nweb = NWebAdapterHelper::Instance().CreateNWeb(surPtr, GetInitArgs(), width, height);
    EXPECT_FALSE(g_errlog.find("is invalid") != std::string::npos);

    NWebHelper::Instance().autoPreconnectEnabled_ = ret;
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: NWebHelper_SetLazyInitializeWebEngine_001
 * @tc.desc: SetLazyInitializeWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetLazyInitializeWebEngine_001, TestSize.Level1)
{
    NWebHelper::Instance().initWebEngine_ = true;
    EXPECT_EQ(NWebHelper::Instance().initWebEngine_, true);

    NWebHelper::Instance().SetLazyInitializeWebEngine(true);
    EXPECT_EQ(NWebHelper::Instance().lazyInitializeWebEngine_, false);
}

/**
 * @tc.name: NWebHelper_SetLazyInitializeWebEngine_002
 * @tc.desc: SetLazyInitializeWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetLazyInitializeWebEngine_002, TestSize.Level1)
{
    NWebHelper::Instance().initWebEngine_ = false;
    EXPECT_EQ(NWebHelper::Instance().initWebEngine_, false);
    NWebHelper::Instance().nwebEngine_ = nullptr;
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().SetLazyInitializeWebEngine(true);
    EXPECT_EQ(NWebHelper::Instance().lazyInitializeWebEngine_, true);
}

/**
 * @tc.name: NWebHelper_SetLazyInitializeWebEngine_003
 * @tc.desc: SetLazyInitializeWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetLazyInitializeWebEngine_003, TestSize.Level1)
{
    NWebHelper::Instance().initWebEngine_ = false;
    EXPECT_EQ(NWebHelper::Instance().initWebEngine_, false);
    auto nwebEngineMock = std::make_shared<MockNWebEngine>();
    NWebHelper::Instance().nwebEngine_ = nwebEngineMock;
    EXPECT_NE(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().SetLazyInitializeWebEngine(false);
    EXPECT_EQ(NWebHelper::Instance().lazyInitializeWebEngine_, true);
}

/**
 * @tc.name: NWebHelper_IsLazyInitializeWebEngine_001
 * @tc.desc: IsLazyInitializeWebEngine.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_IsLazyInitializeWebEngine_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    auto nwebHelper = NWebHelper::Instance().GetNWeb(nweb_id);
    EXPECT_EQ(nwebHelper, nullptr);
    NWebHelper::Instance().nwebEngine_ = nullptr;
    EXPECT_EQ(NWebHelper::Instance().nwebEngine_, nullptr);

    NWebHelper::Instance().SetLazyInitializeWebEngine(false);
    EXPECT_EQ(NWebHelper::Instance().IsLazyInitializeWebEngine(), false);
}

/**
 * @tc.name: NWebHelper_SetNWebActiveStatus_001
 * @tc.desc: SetNWebActiveStatus.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetNWebActiveStatus_001, TestSize.Level1)
{
    int32_t nweb_id = 1;

    NWebHelper::Instance().SetNWebActiveStatus(nweb_id, true);
    EXPECT_EQ(NWebHelper::Instance().GetNWebActiveStatus(nweb_id), true);
}

/**
 * @tc.name: NWebHelper_SetNWebActiveStatus_002
 * @tc.desc: SetNWebActiveStatus.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_SetNWebActiveStatus_002, TestSize.Level1)
{
    int32_t nweb_id = 1;

    NWebHelper::Instance().SetNWebActiveStatus(nweb_id, true);
    EXPECT_EQ(NWebHelper::Instance().GetNWebActiveStatus(nweb_id), true);

    NWebHelper::Instance().SetNWebActiveStatus(nweb_id, false);
    EXPECT_EQ(NWebHelper::Instance().GetNWebActiveStatus(nweb_id), false);
}

/**
 * @tc.name: NWebHelper_GetNWebActiveStatus_001
 * @tc.desc: GetNWebActiveStatus.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_GetNWebActiveStatus_001, TestSize.Level1)
{
    int32_t nweb_id = 1;

    EXPECT_EQ(NWebHelper::Instance().GetNWebActiveStatus(nweb_id), false);

    NWebHelper::Instance().SetNWebActiveStatus(nweb_id, true);
    EXPECT_EQ(NWebHelper::Instance().GetNWebActiveStatus(nweb_id), true);
    NWebHelper::Instance().RemoveNWebActiveStatus(nweb_id);
}

/**
 * @tc.name: NWebHelper_RemoveNWebActiveStatus_001
 * @tc.desc: RemoveNWebActiveStatus.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_RemoveNWebActiveStatus_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    NWebHelper::Instance().SetNWebActiveStatus(nweb_id, true);
    NWebHelper::Instance().RemoveNWebActiveStatus(nweb_id);
    EXPECT_EQ(NWebHelper::Instance().GetNWebActiveStatus(nweb_id), false);
}

/**
 * @tc.name: NWebHelper_DumpArkWebInfo_001
 * @tc.desc: DumpArkWebInfo.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NwebHelperTest, NWebHelper_DumpArkWebInfo_001, TestSize.Level1)
{
    std::string result;
    NWebHelper::Instance().nwebEngine_ = nullptr;
    result = NWebHelper::Instance().DumpArkWebInfo("");
    EXPECT_NE(result.size(), 0);

    result.clear();
    g_errlog.clear();
    LOG_SetCallback(MyLogCallback); 
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "webTestSurfaceName";
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    ASSERT_NE(surfaceNode, nullptr);
    sptr<Surface> surPtr = surfaceNode->GetSurface();
    ASSERT_NE(surPtr, nullptr);
    ASSERT_NE(GetInitArgs(), nullptr);
    uint32_t width = 7880;
    uint32_t height = 7880;
    std::shared_ptr<NWeb> nweb = 
        NWebAdapterHelper::Instance().CreateNWeb(surPtr, GetInitArgs(), width, height);
    EXPECT_TRUE(g_errlog.find("is invalid") != std::string::npos);
    EXPECT_EQ(nweb, nullptr);

    result = NWebHelper::Instance().DumpArkWebInfo("");
    EXPECT_NE(result.size(), 0);
}
} // namespace OHOS::NWeb
}
