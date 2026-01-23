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

#if (!defined(webview_arm64) || defined(ASAN_DETECTOR))
const std::string PERSIST_ARKWEBCORE_PACKAGE_NAME = "persist.arkwebcore.package_name";
const std::string SANDBOX_REAL_PATH = "/data/storage/el1/bundle/arkwebcore";
#endif

namespace OHOS::ArkWeb {
class ShareRelroTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static bool shareRelroEnabled_;
    static std::string bundleName_;
    static bool isArkwebEngineAccessible_;
};

bool ShareRelroTest::shareRelroEnabled_ = false;
std::string ShareRelroTest::bundleName_ = "";
bool ShareRelroTest::isArkwebEngineAccessible_ = true;

void ShareRelroTest::SetUpTestCase(void)
{
    shareRelroEnabled_ = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    bundleName_ = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    std::string arkwebLibPath = GetArkwebLibPath().c_str();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/libarkweb_engine.so";
    std::error_code ec;
    isArkwebEngineAccessible_ = std::filesystem::exists(arkwebEngineLibPath, ec);
    if (ec) {
        GTEST_LOG_(INFO) << "Error checking file:" << ec.message() << "\n";
        isArkwebEngineAccessible_ = false;
    }
}

void ShareRelroTest::TearDownTestCase(void)
{
    OHOS::system::SetParameter("web.shareRelro.enabled", shareRelroEnabled_ ? "true" : "false");
    OHOS::system::SetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, bundleName_);
}

void ShareRelroTest::SetUp(void)
{}

void ShareRelroTest::TearDown(void)
{}

/**
 * @tc.name: ShareRelroTest_CheckTargetExists001
 * @tc.desc: Test the CheckTargetExists when ec is not null.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CheckTargetExists001, TestSize.Level1)
{
    std::filesystem::path p = "/data/test/|\0ut"; // invalid path format
    std::error_code ec;
    EXPECT_EQ(CheckTargetExists(p), false);
}

/**
 * @tc.name: ShareRelroTest_CheckTargetExists002
 * @tc.desc: Test the CheckTargetExists when ec is null.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CheckTargetExists002, TestSize.Level1)
{
    std::string arkwebLibPath = GetArkwebLibPath().c_str();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/libarkweb_engine.so";
    EXPECT_EQ(CheckTargetExists(arkwebEngineLibPath), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_CreateArkWebSandboxPathIfNeed001
 * @tc.desc: Test the CreateArkWebSandboxPathIfNeed when ArkWebSandboxPath exists.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateArkWebSandboxPathIfNeed001, TestSize.Level1)
{
    EXPECT_EQ(OHOS::system::SetParameter("web.shareRelro.enabled", "false"), true);
    EXPECT_EQ(OHOS::system::GetBoolParameter("web.shareRelro.enabled", false), false);
    CreateArkWebSandboxPathIfNeed();
    std::string arkwebLibPath = GetArkwebLibPath().c_str();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/libarkweb_engine.so";
    EXPECT_EQ(CheckTargetExists(arkwebEngineLibPath), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_CreateArkWebSandboxPathIfNeed002
 * @tc.desc: Test the CreateArkWebSandboxPathIfNeed when ArkWebSandboxPath dose not exist.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateArkWebSandboxPathIfNeed002, TestSize.Level1)
{
    EXPECT_EQ(OHOS::system::SetParameter("web.shareRelro.enabled", "true"), true);
    EXPECT_EQ(OHOS::system::GetBoolParameter("web.shareRelro.enabled", false), true);
    CreateArkWebSandboxPathIfNeed();
    std::string arkwebLibPath = GetArkwebLibPath().c_str();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/libarkweb_engine.so";
    EXPECT_EQ(CheckTargetExists(arkwebEngineLibPath), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_MountArkwebEngineLib001
 * @tc.desc: Test the MountArkwebEngineLib when bundle name is empty.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_MountArkwebEngineLib001, TestSize.Level1)
{
    EXPECT_EQ(OHOS::system::SetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, ""), true);
    const std::string bundleName = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    EXPECT_EQ(bundleName.empty(), true);
    MountArkwebEngineLib();
    std::string arkwebLibPath = GetArkwebLibPath();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/" + ARK_WEB_ENGINE_LIB_NAME;
    EXPECT_EQ(CheckTargetExists(arkwebEngineLibPath), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_MountArkwebEngineLib002
 * @tc.desc: Test the MountArkwebEngineLib when mount failed.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_MountArkwebEngineLib002, TestSize.Level1)
{
    EXPECT_EQ(OHOS::system::SetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "test"), true);
    const std::string bundleName = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    EXPECT_EQ(bundleName.empty(), false);
    SetMountMockValue(1); // 1 invalid mount mock value
    MountArkwebEngineLib();
    std::string arkwebLibPath = GetArkwebLibPath();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/" + ARK_WEB_ENGINE_LIB_NAME;
    EXPECT_EQ(CheckTargetExists(arkwebEngineLibPath), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_MountArkwebEngineLib003
 * @tc.desc: Test the MountArkwebEngineLib when mount success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_MountArkwebEngineLib003, TestSize.Level1)
{
    EXPECT_EQ(OHOS::system::SetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "test"), true);
    const std::string bundleName = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    EXPECT_EQ(bundleName.empty(), false);
    SetMountMockValue(0); // 0 valid mount mock value
    MountArkwebEngineLib();
    std::string arkwebLibPath = GetArkwebLibPath();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/" + ARK_WEB_ENGINE_LIB_NAME;
    EXPECT_EQ(CheckTargetExists(arkwebEngineLibPath), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_NeedShareRelro_001
 * @tc.desc: Test the NeedShareRelro when web.shareRelro.enabled is false.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_NeedShareRelro_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    EXPECT_EQ(NeedShareRelro(), false);
}

/**
 * @tc.name: ShareRelroTest_NeedShareRelro_002
 * @tc.desc: Test the NeedShareRelro when web.shareRelro.enabled is true.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_NeedShareRelro_002, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);
    EXPECT_EQ(OHOS::system::SetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, ""), true);
    const std::string bundleName = OHOS::system::GetParameter(PERSIST_ARKWEBCORE_PACKAGE_NAME, "");
    EXPECT_EQ(bundleName.empty(), true);

    std::string arkwebLibPath = GetArkwebLibPath();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/" + ARK_WEB_ENGINE_LIB_NAME;
    EXPECT_EQ(NeedShareRelro(), isArkwebEngineAccessible_);
}

/**
 * @tc.name: ShareRelroTest_ReserveAddressSpace_001
 * @tc.desc: Test the ReserveAddressSpace when web.shareRelro.enabled is false.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_ReserveAddressSpace_001, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "false");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, false);
    EXPECT_EQ(ReserveAddressSpace(), false);
}

/**
 * @tc.name: ShareRelroTest_ReserveAddressSpace_002
 * @tc.desc: Test the ReserveAddressSpace when mmap failed.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_ReserveAddressSpace_002, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);

    std::string arkwebLibPath = GetArkwebLibPath();
    std::filesystem::path arkwebEngineLibPath = arkwebLibPath + "/" + ARK_WEB_ENGINE_LIB_NAME;
    SetMmapMockValue(MAP_FAILED); // mock mmap return MAP_FAILED
    EXPECT_EQ(ReserveAddressSpace(), false);
}

/**
 * @tc.name: ShareRelroTest_CreateRelroFile_001
 * @tc.desc: Test the CreateRelroFile when g_reservedAddress is nullptr
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_001, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);

    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = true;
    EXPECT_EQ(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);

    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = false;
    EXPECT_EQ(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);

    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
    // g_reservedAddress is nullptr
    EXPECT_EQ(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroTest_LoadWithRelroFile_001
 * @tc.desc: Test the LoadWithRelroFile when g_reservedAddress is nullptr
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_LoadWithRelroFile_001, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);

    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = true;
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);

    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = false;
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);

    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
    // g_reservedAddress is nullptr
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroTest_ReserveAddressSpace_003
 * @tc.desc: Test the ReserveAddressSpace when mmap return is not MAP_FAILED.
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_ReserveAddressSpace_003, TestSize.Level1)
{
    bool ret = OHOS::system::SetParameter("web.shareRelro.enabled", "true");
    EXPECT_EQ(ret, true);
    bool isShareRelroEnabled = OHOS::system::GetBoolParameter("web.shareRelro.enabled", false);
    EXPECT_EQ(isShareRelroEnabled, true);

    static int mockValue; // mock mmap return value is not MAP_FAILED
    SetMmapMockValue(&mockValue);
    EXPECT_EQ(ReserveAddressSpace(), isArkwebEngineAccessible_);
    g_reservedAddress = &mockValue;
}
/**
 * @tc.name: ShareRelroTest_CreateRelroFile_002
 * @tc.desc: Test the CreateRelroFile when unlink failed and errno is ENOMEM
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_002, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
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
 * @tc.name: ShareRelroTest_CreateRelroFile_003
 * @tc.desc: Test the CreateRelroFile when unlink failed and errno is ENOENT
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_003, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
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
 * @tc.name: ShareRelroTest_CreateRelroFile_004
 * @tc.desc: Test the CreateRelroFile when unlink success and errno is ENOENT
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_004, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
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
 * @tc.name: ShareRelroTest_CreateRelroFile_005
 * @tc.desc: Test the CreateRelroFile when unlink success and errno is ENOMEM
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_005, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
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
 * @tc.name: ShareRelroTest_CreateRelroFile_006
 * @tc.desc: Test the CreateRelroFile when open success and dlopen_ns_ext failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_006, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
    int validMockValue = 0; // 0 is valid ret value
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
 * @tc.name: ShareRelroTest_CreateRelroFile_007
 * @tc.desc: Test the CreateRelroFile when dlopen_ns_ext success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFile_007, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
    int validMockValue = 0; // 0 is valid ret value
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
    EXPECT_NE(CreateRelroFile(arkWebEngineLibName, &dlns), nullptr);
    SetDlopenNsExtMockValue(nullptr);
}

/**
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_001
 * @tc.desc: Test the CreateRelroFileInSubProc when fork failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_001, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    SetForkMockValue(-1); // -1 invalid pid
    CreateRelroFileInSubProc();
    std::string arkWebEngineLibName = "libarkweb_engine.so";
    Dl_namespace dlns;
    Dl_namespace ndkns;
    dlns_init(&dlns, GetArkwebNameSpace().c_str());
    dlns_create(&dlns, GetArkwebLibPath().c_str());
    dlns_get("ndk", &ndkns);
    dlns_inherit(&dlns, &ndkns, "allow_all_shared_libs");
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);

    SetForkMockValue(1); // 1 invalid pid
    CreateRelroFileInSubProc();
    EXPECT_EQ(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
}

/**
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_002
 * @tc.desc: Test the CreateRelroFileInSubProc when prctl failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_002, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    SetForkMockValue(0); // 0 valid pid
    SetPrctlMockValue(-1); // -1 invalid prctl mock value
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
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_003
 * @tc.desc: Test the CreateRelroFileInSubProc when setuid and setgid both failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_003, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    SetForkMockValue(0); // 0 valid pid
    SetPrctlMockValue(0); // 0 valid mock value
    int invalidMockValue = -1; // -1 invalid mock value
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
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_004
 * @tc.desc: Test the CreateRelroFileInSubProc when setuid success and setgid failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_004, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    int validMockValue = 0; // 0 valid mock value
    SetForkMockValue(validMockValue);
    SetPrctlMockValue(validMockValue);
    SetUidMockValue(validMockValue);
    SetGidMockValue(-1); // -1 invalid mock value
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
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_005
 * @tc.desc: Test the CreateRelroFileInSubProc when setgid success and setuid failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_005, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    int validMockValue = 0; // 0 valid mock value
    SetForkMockValue(validMockValue);
    SetPrctlMockValue(validMockValue);
    SetGidMockValue(validMockValue);
    SetUidMockValue(-1); // -1 invalid mock value
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
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_006
 * @tc.desc: Test the CreateRelroFileInSubProc when RestoreconRecurse failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_006, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    int validMockValue = 0; // 0 valid mock value
    SetForkMockValue(validMockValue);
    SetPrctlMockValue(validMockValue);
    SetGidMockValue(validMockValue);
    SetUidMockValue(validMockValue);
    SetRestoreconRecurseMockValue(-1); // -1 invalid mock value
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
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_007
 * @tc.desc: Test the CreateRelroFileInSubProc when CreateRelroFile failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_007, TestSize.Level1)
{
    g_shareRelroEnabled = false;
    g_arkwebEngineAccessible = false;
    int validMockValue = 0; // 0 valid mock value
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
 * @tc.name: ShareRelroTest_CreateRelroFileInSubProc_008
 * @tc.desc: Test the CreateRelroFileInSubProc when CreateRelroFile success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_CreateRelroFileInSubProc_008, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
    int validMockValue = 0; // 0 valid mock value
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
    EXPECT_NE(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    SetDlopenNsExtMockValue(nullptr);
}

/**
 * @tc.name: ShareRelroTest_LoadWithRelroFile_002
 * @tc.desc: Test the LoadWithRelroFile when open failed
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_LoadWithRelroFile_002, TestSize.Level1)
{
    g_shareRelroEnabled = true;
    g_arkwebEngineAccessible = true;
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
 * @tc.name: ShareRelroTest_LoadWithRelroFile_003
 * @tc.desc: Test the LoadWithRelroFile when open success
 * @tc.type: Func
 * @tc.require:
 */
HWTEST_F(ShareRelroTest, ShareRelroTest_LoadWithRelroFile_003, TestSize.Level1)
{
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
    EXPECT_NE(LoadWithRelroFile(arkWebEngineLibName, &dlns), nullptr);
    SetDlopenNsExtMockValue(nullptr);
}
} // namespace OHOS::NWeb