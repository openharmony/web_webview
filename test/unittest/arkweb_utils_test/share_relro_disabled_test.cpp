/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "mock_system_func.h"
#include "arkweb_utils.cpp"
#include "parameters.h"
using namespace testing::ext;
namespace OHOS::ArkWeb {
class ShareRelroDisabledTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ShareRelroDisabledTest::SetUpTestCase(void)
{}

void ShareRelroDisabledTest::TearDownTestCase(void)
{}

void ShareRelroDisabledTest::SetUp(void)
{}

void ShareRelroDisabledTest::TearDown(void)
{}

/**
 * @tc.name: ShareRelroDisabledTest_NeedShareRelro_001
 * @tc.desc: Test the NeedShareRelro when web.shareRelro.enabled is false.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_NeedShareRelro_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    EXPECT_EQ(NeedShareRelro(), false);
}

/**
 * @tc.name: ShareRelroDisabledTest_ReserveAddressSpace_001
 * @tc.desc: Test the ReserveAddressSpace when web.shareRelro.enabled is false.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_ReserveAddressSpace_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    EXPECT_EQ(ReserveAddressSpace(), false);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFile_001
 * @tc.desc: Test the CreateRelroFile when web.shareRelro.enabled is false
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFile_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_001
 * @tc.desc: Test the CreateRelroFileInSubProc when prctl failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    SetPrctlMockValue(-1); // -1 invlaid prctl mock value
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_002
 * @tc.desc: Test the CreateRelroFileInSubProc when setuid and setgid both failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_002, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    SetPrctlMockValue(0); // 0 vlaid mock value
    int invalidMockValue = -1; // -1 invlaid mock value
    SetUidMockValue(invalidMockValue);
    SetGidMockValue(invalidMockValue);
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_003
 * @tc.desc: Test the CreateRelroFileInSubProc when setuid success and setgid failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_003, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    int validMockValue = 0; // 0 vlaid mock value
    SetPrctlMockValue(validMockValue);
    SetUidMockValue(validMockValue);
    SetGidMockValue(-1); // -1 invlaid mock value
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_004
 * @tc.desc: Test the CreateRelroFileInSubProc when setgid success and setuid failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_004, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    int validMockValue = 0; // 0 vlaid mock value
    SetPrctlMockValue(validMockValue);
    SetGidMockValue(validMockValue);
    SetUidMockValue(-1); // -1 invlaid mock value
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_005
 * @tc.desc: Test the CreateRelroFileInSubProc when RestoreconRecurse failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_005, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    int validMockValue = 0; // 0 vlaid mock value
    SetPrctlMockValue(validMockValue);
    SetGidMockValue(validMockValue);
    SetUidMockValue(validMockValue);
    SetRestoreconRecurseMockValue(-1); // -1 invlaid mock value
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_006
 * @tc.desc: Test the CreateRelroFileInSubProc when CreateRelroFile failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_006, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    int validMockValue = 0; // 0 vlaid mock value
    SetPrctlMockValue(validMockValue);
    SetGidMockValue(validMockValue);
    SetUidMockValue(validMockValue);
    SetRestoreconRecurseMockValue(validMockValue);
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_CreateRelroFileInSubProc_007
 * @tc.desc: Test the CreateRelroFileInSubProc when fork failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_CreateRelroFileInSubProc_007, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    SetForkMockValue(-1); // -1 invliad pid
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    SetForkMockValue(1); // 1 invliad pid
    CreateRelroFileInSubProc();
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroDisabledTest_LoadWithRelroFile_001
 * @tc.desc: Test the LoadWithRelroFile when web.shareRelro.enabled is false
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroDisabledTest, ShareRelroDisabledTest_LoadWithRelroFile_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}
} // namespace OHOS::NWeb