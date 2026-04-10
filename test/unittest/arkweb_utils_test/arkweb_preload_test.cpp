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
#include <cstring>
#include <dlfcn.h>
#include <dlfcn_ext.h>
#include <gtest/gtest.h>
#include <string>
#include <sys/sysinfo.h>
#include <unistd.h>

#include "arkweb_preload_common.cpp"
#include "parameters.h"
#include "arkweb_utils.h"

using namespace testing::ext;
#ifdef __cplusplus
extern "C" {
#endif

void *g_dlopenMockValue = reinterpret_cast<void *>(0x1);

void SetDlopenMockValue(void *mockValue)
{
    g_dlopenMockValue = mockValue;
}

void *DlopenMock(const char *filename, int flags)
{
    (void)filename;
    (void)flags;
    return g_dlopenMockValue;
}

void *g_dlopenNsMockValue = reinterpret_cast<void *>(0x1);

void SetDlopenNsMockValue(void *mockValue)
{
    g_dlopenNsMockValue = mockValue;
}

void *DlopenNsMock(Dl_namespace *dlns, const char *libName, int flag)
{
    (void)dlns;
    (void)libName;
    (void)flag;
    return g_dlopenNsMockValue;
}

int g_sysinfoMockValue = 0;

void SetSysinfoMockValue(int mockValue)
{
    g_sysinfoMockValue = mockValue;
}

int SysinfoMock(struct sysinfo *info)
{
    if (g_sysinfoMockValue == 0) {
        info->totalram = 3UL * 1024UL * 1024UL * 1024UL;
        return 0;
    }
    return -1;
}

void DlnsInitMock(Dl_namespace *dlns, const char *name)
{
    (void)dlns;
    (void)name;
}

int DlnsCreateMock(Dl_namespace *dlns, const char *libPath)
{
    (void)dlns;
    (void)libPath;
    return 0;
}

int DlnsGetMock(const char *name, Dl_namespace *dlns)
{
    (void)name;
    (void)dlns;
    return 0;
}

int DlnsInheritMock(Dl_namespace *dlns, Dl_namespace *inherited, const char *sharedLibs)
{
    (void)dlns;
    (void)inherited;
    (void)sharedLibs;
    return 0;
}

#ifdef __cplusplus
} // extern "C"
#endif

namespace OHOS::ArkWeb {

class ArkWebPreloadCommonTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    static std::string savedPackageName;
};

std::string ArkWebPreloadCommonTest::savedPackageName;

void ArkWebPreloadCommonTest::SetUpTestCase(void)
{
    savedPackageName = OHOS::system::GetParameter("persist.arkwebcore.package_name", "");
}

void ArkWebPreloadCommonTest::TearDownTestCase(void)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", savedPackageName);
}

void ArkWebPreloadCommonTest::SetUp(void) {}

void ArkWebPreloadCommonTest::TearDown(void) {}

// ==================== GetArkwebBundleInstallLibPath ====================

/**
 * @brief Empty package name returns empty path
 */
HWTEST_F(ArkWebPreloadCommonTest, GetArkwebBundleInstallLibPath_EmptyPackageName_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "");
    auto res = GetArkwebBundleInstallLibPath();
    EXPECT_EQ(res, "");
}

/**
 * @brief Valid package name with EVERGREEN engine returns correct path
 */
HWTEST_F(ArkWebPreloadCommonTest, GetArkwebBundleInstallLibPath_EvergreenEngine_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", "0");
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    auto res = GetArkwebBundleInstallLibPath();
    EXPECT_FALSE(res.empty());
#if defined(webview_arm64)
    EXPECT_NE(res.find("arm64"), std::string::npos);
#elif defined(webview_x86_64)
    EXPECT_EQ(res, "");
#else
    EXPECT_NE(res.find("arm"), std::string::npos);
    EXPECT_EQ(res.find("arm64"), std::string::npos);
#endif
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

/**
 * @brief Valid package name with LEGACY engine appends "legacy" to bundle name
 */
HWTEST_F(ArkWebPreloadCommonTest, GetArkwebBundleInstallLibPath_LegacyEngine_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(static_cast<int>(ArkWebEngineType::LEGACY)));

    auto res = GetArkwebBundleInstallLibPath();
#if !defined(webview_x86_64)
    EXPECT_NE(res.find("legacy"), std::string::npos);
#endif
    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

// ==================== GetOhosAdptGlueSrcLibPath ====================

/**
 * @brief GetOhosAdptGlueSrcLibPath returns correct path per architecture
 */
HWTEST_F(ArkWebPreloadCommonTest, GetOhosAdptGlueSrcLibPath_ArchPath_001, TestSize.Level1)
{
    auto res = GetOhosAdptGlueSrcLibPath();
#if defined(webview_arm64)
    EXPECT_EQ(res, "/system/lib64/libohos_adapter_glue_source.z.so");
#elif defined(webview_x86_64)
    EXPECT_EQ(res, "");
#else
    EXPECT_EQ(res, "/system/lib/libohos_adapter_glue_source.z.so");
#endif
}

// ==================== GetSysTotalRAMInfo ====================

/**
 * @brief sysinfo success returns positive RAM value
 */
HWTEST_F(ArkWebPreloadCommonTest, GetSysTotalRAMInfo_Success_001, TestSize.Level1)
{
    SetSysinfoMockValue(0); // Simulate success

    int32_t ram = GetSysTotalRAMInfo();
    EXPECT_GT(ram, 0);
}

/**
 * @brief sysinfo failure returns 0
 */
HWTEST_F(ArkWebPreloadCommonTest, GetSysTotalRAMInfo_Fail_001, TestSize.Level1)
{
    SetSysinfoMockValue(-1); // Simulate failure

    int32_t ram = GetSysTotalRAMInfo();
    EXPECT_EQ(ram, 0);

    SetSysinfoMockValue(0); // Restore
}

// ==================== GetRenderPreLoadMode ====================

/**
 * @brief ramSize well below 8GB threshold results in PRELOAD_NO
 */
HWTEST_F(ArkWebPreloadCommonTest, GetRenderPreLoadMode_SmallRAM_001, TestSize.Level1)
{
    int32_t smallRam = 1;
    int mode = GetRenderPreLoadMode(smallRam);
    EXPECT_EQ(mode, 0); // PRELOAD_NO
}

/**
 * @brief ramSize exactly at 8GB threshold results in PRELOAD_NO
 */
HWTEST_F(ArkWebPreloadCommonTest, GetRenderPreLoadMode_Exactly8GB_001, TestSize.Level1)
{
    int32_t thresholdRam = RAM_SIZE_8G * SIZE_KB;
    int mode = GetRenderPreLoadMode(thresholdRam);
    EXPECT_EQ(mode, 0); // PRELOAD_NO (<=)
}

/**
 * @brief ramSize > 8GB reads preload mode from system parameter (mode 1)
 */
HWTEST_F(ArkWebPreloadCommonTest, GetRenderPreLoadMode_LargeRAM_Partial_001, TestSize.Level1)
{
    int32_t largeRam = RAM_SIZE_8G * SIZE_KB + 1;
    int savedPreloadMode = OHOS::system::GetIntParameter("const.startup.nwebspawn.preloadMode", 0);
    OHOS::system::SetParameter("const.startup.nwebspawn.preloadMode", "1");

    int mode = GetRenderPreLoadMode(largeRam);
    EXPECT_EQ(mode, 1); // PRELOAD_PARTIAL

    OHOS::system::SetParameter("const.startup.nwebspawn.preloadMode", std::to_string(savedPreloadMode));
}

/**
 * @brief ramSize > 8GB with param=2 returns PRELOAD_FULL
 */
HWTEST_F(ArkWebPreloadCommonTest, GetRenderPreLoadMode_LargeRAM_Full_001, TestSize.Level1)
{
    int32_t largeRam = RAM_SIZE_8G * SIZE_KB + 1;
    int savedPreloadMode = OHOS::system::GetIntParameter("const.startup.nwebspawn.preloadMode", 0);
    OHOS::system::SetParameter("const.startup.nwebspawn.preloadMode", "2");

    int mode = GetRenderPreLoadMode(largeRam);
    EXPECT_EQ(mode, 2); // PRELOAD_FULL

    OHOS::system::SetParameter("const.startup.nwebspawn.preloadMode", std::to_string(savedPreloadMode));
}

/**
 * @brief ramSize > 8GB with param=0 returns PRELOAD_NO from param
 */
HWTEST_F(ArkWebPreloadCommonTest, GetRenderPreLoadMode_LargeRAM_NoMode_001, TestSize.Level1)
{
    int32_t largeRam = RAM_SIZE_8G * SIZE_KB + 1;
    int savedPreloadMode = OHOS::system::GetIntParameter("const.startup.nwebspawn.preloadMode", 0);
    OHOS::system::SetParameter("const.startup.nwebspawn.preloadMode", "0");

    int mode = GetRenderPreLoadMode(largeRam);
    EXPECT_EQ(mode, 0); // PRELOAD_NO from param

    OHOS::system::SetParameter("const.startup.nwebspawn.preloadMode", std::to_string(savedPreloadMode));
}

// ==================== PreloadArkWebLibForRender (inner, static) ====================

/**
 * @brief PRELOAD_NO mode does not call any preload
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebLibForRender_NoPreload_001, TestSize.Level1)
{
    PreloadArkWebLibForRender(0);
    // No crash = pass, no libs loaded
}

/**
 * @brief PRELOAD_PARTIAL mode preloads only adapter glue lib
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebLibForRender_PartialPreload_001, TestSize.Level1)
{
    SetDlopenMockValue(reinterpret_cast<void *>(0x1));
    PreloadArkWebLibForRender(1);
    // Should call dlopen for adapter glue only
}

/**
 * @brief PRELOAD_FULL mode preloads both adapter glue and engine
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebLibForRender_FullPreload_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", "0");
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    SetDlopenMockValue(reinterpret_cast<void *>(0x1));
    SetDlopenNsMockValue(reinterpret_cast<void *>(0x1));
    PreloadArkWebLibForRender(2);

    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

// ==================== PreloadArkWebEngineLib (static) ====================

/**
 * @brief Empty engine lib path causes early return without crash
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebEngineLib_EmptyPath_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "");
    PreloadArkWebEngineLib();
    // Should log error and return without crash
}

/**
 * @brief Valid path with successful dlopen_ns
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebEngineLib_ValidPath_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", "0");
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    SetDlopenNsMockValue(reinterpret_cast<void *>(0x1));
    PreloadArkWebEngineLib();

    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

/**
 * @brief Valid path with dlopen_ns returning nullptr
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebEngineLib_DlopenFail_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", "0");
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    SetDlopenNsMockValue(nullptr);
    PreloadArkWebEngineLib();
    // Should log error but not crash

    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

// ==================== PreloadOHOSAdptGlueSrcLib (static) ====================

/**
 * @brief Adapter glue preload with successful dlopen
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadOHOSAdptGlueSrcLib_DlopenSuccess_001, TestSize.Level1)
{
    SetDlopenMockValue(reinterpret_cast<void *>(0x1));
    PreloadOHOSAdptGlueSrcLib();
    // Should succeed without crash
}

/**
 * @brief Adapter glue preload with dlopen returning nullptr
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadOHOSAdptGlueSrcLib_DlopenFail_001, TestSize.Level1)
{
    SetDlopenMockValue(nullptr);
    PreloadOHOSAdptGlueSrcLib();
    // Should log error but not crash
    SetDlopenMockValue(reinterpret_cast<void *>(0x1)); // Restore
}

// ==================== PreloadArkWebLibForRender (public entry) ====================

/**
 * @brief Public entry point completes without crash
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebLibForRender_Entry_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");
    int webEngineEnforce = OHOS::system::GetIntParameter("web.engine.enforce", 0);
    OHOS::system::SetParameter("web.engine.enforce", "0");
    setActiveWebEngineVersion(ArkWebEngineVersion::SYSTEM_DEFAULT);

    SetSysinfoMockValue(0);
    SetDlopenMockValue(reinterpret_cast<void *>(0x1));
    SetDlopenNsMockValue(reinterpret_cast<void *>(0x1));
    PreloadArkWebLibForRender();

    OHOS::system::SetParameter("web.engine.enforce", std::to_string(webEngineEnforce));
}

/**
 * @brief sysinfo failure triggers PRELOAD_NO path
 */
HWTEST_F(ArkWebPreloadCommonTest, PreloadArkWebLibForRender_SysinfoFail_001, TestSize.Level1)
{
    OHOS::system::SetParameter("persist.arkwebcore.package_name", "com.ohos.arkwebcore");

    SetSysinfoMockValue(-1); // Simulate sysinfo failure
    SetDlopenMockValue(reinterpret_cast<void *>(0x1));
    SetDlopenNsMockValue(reinterpret_cast<void *>(0x1));
    PreloadArkWebLibForRender();
    // With sysinfo returning 0, ramSize=0 <= threshold, so PRELOAD_NO

    SetSysinfoMockValue(0); // Restore
}

} // namespace OHOS::ArkWeb
