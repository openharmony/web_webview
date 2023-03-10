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
#include <surface.h>

#define private public
#include "nweb_adapter_helper.h"
#include "nweb_create_window.h"
#include "nweb_helper.h"
#undef private

#include "ohos_adapter_helper.h"
#include "window.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::MMI;
using namespace OHOS::Rosen;
namespace OHOS::NWeb {
namespace {
const std::string MOCK_INSTALLATION_DIR = "/data/app/el1/bundle/public/com.ohos.nweb";
}
class OhosAdapterHelperTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void OhosAdapterHelperTest::SetUpTestCase(void)
{}

void OhosAdapterHelperTest::TearDownTestCase(void)
{}

void OhosAdapterHelperTest::SetUp(void)
{}

void OhosAdapterHelperTest::TearDown(void)
{}

/**
 * @tc.name: OhosAdapterHelper_GetCookieManager_001.
 * @tc.desc: Test the GetCookieManager.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosAdapterHelperTest, OhosAdapterHelper_GetCookieManager_001, TestSize.Level1)
{
    int32_t nweb_id = 1;
    NWebHelper &helper = NWebHelper::Instance();
    helper.SetBundlePath(MOCK_INSTALLATION_DIR);
    helper.Init(false);
    auto cook = helper.GetCookieManager();
    EXPECT_NE(cook, nullptr);
    auto base = helper.GetDataBase();
    EXPECT_NE(base, nullptr);
    auto storage = helper.GetWebStorage();
    EXPECT_NE(storage, nullptr);
    auto nweb = helper.GetNWeb(nweb_id);
    EXPECT_EQ(nweb.lock(), nullptr);
}

/**
 * @tc.name: OhosAdapterHelper_GetInstance_002.
 * @tc.desc: Test the GetInstance.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosAdapterHelperTest, OhosAdapterHelper_GetInstance_002, TestSize.Level1)
{
    OhosAdapterHelper &helper = OhosAdapterHelper::GetInstance();
    std::unique_ptr<AafwkAppMgrClientAdapter> client = helper.CreateAafwkAdapter();
    EXPECT_NE(client, nullptr);
    std::unique_ptr<PowerMgrClientAdapter> powerMgr = helper.CreatePowerMgrClientAdapter();
    EXPECT_NE(powerMgr, nullptr);
    std::unique_ptr<DisplayManagerAdapter> display = helper.CreateDisplayMgrAdapter();
    EXPECT_NE(display, nullptr);
    std::unique_ptr<BatteryMgrClientAdapter> battery = helper.CreateBatteryClientAdapter();
    EXPECT_NE(battery, nullptr);
    helper.GetOhosWebDataBaseAdapterInstance();
    std::unique_ptr<NetConnectAdapter> connect = helper.CreateNetConnectAdapter();
    EXPECT_NE(connect, nullptr);
    helper.GetPasteBoard();
    std::unique_ptr<AudioRendererAdapter> audioRender = helper.CreateAudioRendererAdapter();
    EXPECT_NE(audioRender, nullptr);
    helper.GetAudioSystemManager();
    helper.GetWebPermissionDataBaseInstance();
    std::unique_ptr<MMIAdapter> mmiAdapter = helper.CreateMMIAdapter();
    EXPECT_NE(mmiAdapter, nullptr);
    sptr<Surface> surface = nullptr;
    NWebInitArgs initArgs;
    uint32_t width = 1;
    uint32_t height = 1;
    auto nweb = NWebAdapterHelper::Instance().CreateNWeb(surface, initArgs, width, height);
    EXPECT_EQ(nweb, nullptr);
}

/**
 * @tc.name: OhosAdapterHelper_CreateNWeb_003.
 * @tc.desc: Test the CreateNWeb.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosAdapterHelperTest, OhosAdapterHelper_CreateNWeb_003, TestSize.Level1)
{
    sptr<OHOS::Rosen::Window> window = nullptr;
    NWebAdapterHelper &helper = NWebAdapterHelper::Instance();
    helper.CreateNWeb(window.GetRefPtr(), GetInitArgs());
}

/**
 * @tc.name: OhosAdapterHelper_GetDataBase_004.
 * @tc.desc: Test the GetDataBase.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(OhosAdapterHelperTest, OhosAdapterHelper_GetDataBase_004, TestSize.Level1)
{
    int32_t nweb_id = 1;
    NWebHelper &helper = NWebHelper::Instance();
    NWebCreateInfo create_info;
    helper.LoadLib(true);
    helper.libHandleWebEngine_ = nullptr;
    helper.LoadLib(true);
    helper.bundlePath_ = "";
    helper.LoadLib(true);
    helper.libHandleWebEngine_ = nullptr;
    helper.LoadLib(true);
    helper.LoadLib(true);
    helper.libHandleWebEngine_ = nullptr;
    auto webview = helper.CreateNWeb(create_info);
    EXPECT_EQ(webview, nullptr);
    auto cook = helper.GetCookieManager();
    EXPECT_EQ(cook, nullptr);
    auto base = helper.GetDataBase();
    EXPECT_EQ(base, nullptr);
    auto storage = helper.GetWebStorage();
    EXPECT_EQ(storage, nullptr);
    auto nweb = helper.GetNWeb(nweb_id);
    EXPECT_EQ(nweb.lock(), nullptr);
    helper.UnloadLib();
}
}
