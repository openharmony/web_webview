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
class ShareRelroEnabledTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static bool isArkwebEngineAccessible;
};

bool ShareRelroEnabledTest::isArkwebEngineAccessible = true;

void ShareRelroEnabledTest::SetUpTestCase(void)
{
    std::string arkwebLibPath = GetArkwebLibPath().c_str();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/libarkweb_engine.so";
    std::error_code ec;
    isArkwebEngineAccessible = std::filesystem::exists(arkwebEngineLibPath, ec);
    if (ec) {
        GTEST_LOG_(INFO) << "Error checking file:" << ec.message() << "\n";
        isArkwebEngineAccessible = false;
    }
}

void ShareRelroEnabledTest::TearDownTestCase(void)
{}

void ShareRelroEnabledTest::SetUp(void)
{}

void ShareRelroEnabledTest::TearDown(void)
{}

/**
 * @tc.name: ShareRelroEnabledTest_NeedShareRelro_001
 * @tc.desc: Test the NeedShareRelro when web.shareRelro.enabled is true.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_NeedShareRelro_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    EXPECT_EQ(NeedShareRelro(), isArkwebEngineAccessible);
}

/**
 * @tc.name: ShareRelroEnabledTest_ReserveAddressSpace_001
 * @tc.desc: Test the ReserveAddressSpace when mmap failed.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_ReserveAddressSpace_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    SetMmapMockValue(MAP_FAILED); // mock mmap return MAP_FAILED
    EXPECT_EQ(ReserveAddressSpace(), false);
}

/**
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_001
 * @tc.desc: Test the CreateRelroFile when g_reservedAddress is nullptr
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    // g_reservedAddress is nullptr
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
 * @tc.name: ShareRelroEnabledTest_LoadWithRelroFile_001
 * @tc.desc: Test the LoadWithRelroFile when g_reservedAddress is nullptr
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_LoadWithRelroFile_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    // g_reservedAddress is nullptr
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
 * @tc.name: ShareRelroEnabledTest_ReserveAddressSpace_002
 * @tc.desc: Test the ReserveAddressSpace when mmap return is not MAP_FAILED.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_ReserveAddressSpace_002, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    static int mockValue; // mock mmap return value is not MAP_FAILED
    SetMmapMockValue(&mockValue);
    EXPECT_EQ(ReserveAddressSpace(), isArkwebEngineAccessible);
}
/**
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_002
 * @tc.desc: Test the CreateRelroFile when unlink failed and errno is ENOMEM
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_002, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    int invalidRetValue = -1; // -1 is invalid ret value
    SetUnlinkMockValue(invalidRetValue);
    errno = ENOMEM;
    SetOpenMockValue(invalidRetValue);
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
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_003
 * @tc.desc: Test the CreateRelroFile when unlink failed and errno is ENOENT
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_003, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    int invalidRetValue = -1; // -1 is invalid ret value
    SetUnlinkMockValue(invalidRetValue);
    errno = ENOENT;
    SetOpenMockValue(invalidRetValue);
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
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_004
 * @tc.desc: Test the CreateRelroFile when unlink success and errno is ENOENT
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_004, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    SetUnlinkMockValue(0); // 0 is valid ret value
    errno = ENOENT;
    SetOpenMockValue(-1); // -1 is invalid ret value
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
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_005
 * @tc.desc: Test the CreateRelroFile when unlink success and errno is ENOMEM
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_005, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    SetUnlinkMockValue(0); // 0 is valid ret value
    errno = ENOMEM;
    SetOpenMockValue(-1); // -1 is invalid ret value
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
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_006
 * @tc.desc: Test the CreateRelroFile when open success and dlopen_ns_ext failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_006, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    int validMockValue = 0; // 0 is invalid ret value
    SetUnlinkMockValue(validMockValue);
    errno = ENOENT;
    SetOpenMockValue(validMockValue);
    SetDlopenNsExtMockValue(nullptr);
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
 * @tc.name: ShareRelroEnabledTest_CreateRelroFile_007
 * @tc.desc: Test the CreateRelroFile when dlopen_ns_ext success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFile_007, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    int validMockValue = 0; // 0 is invalid ret value
    SetUnlinkMockValue(validMockValue);
    errno = ENOENT;
    SetOpenMockValue(validMockValue);
    SetDlopenNsExtMockValue(&validMockValue);
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    if (isArkwebEngineAccessible) {
        EXPECT_NE(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);
    } else {
        EXPECT_EQ(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);
    }
    SetDlopenNsExtMockValue(nullptr);
}

/**
 * @tc.name: ShareRelroEnabledTest_CreateRelroFileInSubProc_001
 * @tc.desc: Test the CreateRelroFileInSubProc when CreateRelroFile success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_CreateRelroFileInSubProc_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    int validMockValue = 0; // 0 is invalid ret value
    SetPrctlMockValue(validMockValue);
    SetGidMockValue(validMockValue);
    SetUidMockValue(validMockValue);
    SetRestoreconRecurseMockValue(validMockValue);
    SetUnlinkMockValue(validMockValue);
    errno = ENOENT;
    SetOpenMockValue(validMockValue);
    SetDlopenNsExtMockValue(&validMockValue);
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    if (isArkwebEngineAccessible) {
        EXPECT_NE(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    } else {
        EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    }
    SetDlopenNsExtMockValue(nullptr);
}

/**
 * @tc.name: ShareRelroEnabledTest_LoadWithRelroFile_002
 * @tc.desc: Test the LoadWithRelroFile when open failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_LoadWithRelroFile_002, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    SetOpenMockValue(-1); // -1 is invalid ret value
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
 * @tc.name: ShareRelroEnabledTest_LoadWithRelroFile_003
 * @tc.desc: Test the LoadWithRelroFile when open success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroEnabledTest, ShareRelroEnabledTest_LoadWithRelroFile_003, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    int validMockValue = 0; // 0 is valid ret value
    SetOpenMockValue(validMockValue);
    SetDlopenNsExtMockValue(&validMockValue);
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    if (isArkwebEngineAccessible) {
        EXPECT_NE(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    } else {
        EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    }
    SetDlopenNsExtMockValue(nullptr);
}
} // namespace OHOS::NWeb