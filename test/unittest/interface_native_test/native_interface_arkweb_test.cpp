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

#include "parameter.h"
#include "parameters.h"

#define private public
#include "arkweb_utils.h"
#include "base/web/webview/interfaces/native/native_javascript_execute_callback.h"
#include "native_interface_arkweb.h"

using namespace testing;
using namespace testing::ext;

namespace {
    ArkWeb_ErrorCode g_errorCode = ArkWeb_ErrorCode::ARKWEB_SUCCESS;
    void OH_ArkWeb_OnCookieSaveCallbackImpl(ArkWeb_ErrorCode errorCode) {
        g_errorCode = errorCode;
    }
}

namespace OHOS {
namespace NWeb {

class NativeInterfaceArkWebTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NativeInterfaceArkWebTest::SetUpTestCase(void) {}

void NativeInterfaceArkWebTest::TearDownTestCase(void) {}

void NativeInterfaceArkWebTest::SetUp(void) {}

void NativeInterfaceArkWebTest::TearDown(void) {}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_OH_NativeArkWeb_RunJavaScript_01
 * @tc.desc  : Test OH_NativeArkWeb_RunJavaScript
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_OH_NativeArkWeb_RunJavaScript_01, TestSize.Level1) {
    const char* webTag = "";
    const char* jsCode = "";
    NativeArkWeb_OnJavaScriptCallback callback = nullptr;
    OH_NativeArkWeb_RunJavaScript(webTag, jsCode, callback);
}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_OH_NativeArkWeb_RegisterJavaScriptProxy_01
 * @tc.desc  : Test OH_NativeArkWeb_RegisterJavaScriptProxy
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_OH_NativeArkWeb_RegisterJavaScriptProxy_01, TestSize.Level1) {
    const char* webTag = "";
    const char* objName = "";
    const char* methodName[3] = { "method1", "method2", "method3" };
    NativeArkWeb_OnJavaScriptProxyCallback callback[3] = { nullptr, nullptr, nullptr };
    int32_t size = 3;
    OH_NativeArkWeb_RegisterJavaScriptProxy(webTag, objName, methodName, callback, size, false);
}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_UnregisterJavaScriptProxy_01
 * @tc.desc  : Test OH_NativeArkWeb_UnregisterJavaScriptProxy
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_UnregisterJavaScriptProxy_01, TestSize.Level1) {
    const char* webTag = "";
    const char* objName = "";
    OH_NativeArkWeb_UnregisterJavaScriptProxy(webTag, objName);
}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_SetDestroyCallback_01
 * @tc.desc  : Test OH_NativeArkWeb_SetDestroyCallback
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_SetDestroyCallback_01, TestSize.Level1) {
    const char* webTag = "";
    NativeArkWeb_OnDestroyCallback callback = nullptr;
    OH_NativeArkWeb_SetDestroyCallback(webTag, callback);
}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_GetDestroyCallback_01
 * @tc.desc  : Test OH_NativeArkWeb_GetDestroyCallback
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_GetDestroyCallback_01, TestSize.Level1) {
    const char* webTag = "";
    NativeArkWeb_OnDestroyCallback callback = OH_NativeArkWeb_GetDestroyCallback(webTag);
    EXPECT_TRUE(callback == nullptr);
}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_SetJavaScriptProxyValidCallback_01
 * @tc.desc  : Test OH_NativeArkWeb_SetJavaScriptProxyValidCallback
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_SetJavaScriptProxyValidCallback_01, TestSize.Level1) {
    const char* webTag = "";
    NativeArkWeb_OnValidCallback callback = nullptr;
    OH_NativeArkWeb_SetJavaScriptProxyValidCallback(webTag, callback);
}

/**
 * @tc.name  : OHNativeInterfaceArkWebTest_OH_NativeArkWeb_GetJavaScriptProxyValidCallback_01
 * @tc.desc  : Test OH_NativeArkWeb_GetJavaScriptProxyValidCallback
 */
HWTEST_F(NativeInterfaceArkWebTest,
         OHNativeInterfaceArkWebTest_OH_NativeArkWeb_GetJavaScriptProxyValidCallback_01, TestSize.Level1) {
    const char* webTag = "";
    NativeArkWeb_OnValidCallback callback =
        OH_NativeArkWeb_GetJavaScriptProxyValidCallback(webTag);
    EXPECT_TRUE(callback == nullptr);
}

/**
 * @tc.name  : OH_NativeArkWeb_GetBlanklessInfoWithKey_01
 * @tc.desc  : Test OH_NativeArkWeb_GetBlanklessInfoWithKey
 */
 HWTEST_F(NativeInterfaceArkWebTest, OH_NativeArkWeb_GetBlanklessInfoWithKey_01, TestSize.Level1) {
    bool enabled = OHOS::system::GetBoolParameter("web.blankless.enabled", false);

    OHOS::system::SetParameter("web.blankless.enabled", "true");
    auto info1 = OH_NativeArkWeb_GetBlanklessInfoWithKey("", "");
    EXPECT_EQ(info1.errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS);
    auto info2 = OH_NativeArkWeb_GetBlanklessInfoWithKey("", "OH_NativeArkWeb_GetBlanklessInfoWithKey");
    EXPECT_EQ(info2.errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_UNKNOWN);

    OHOS::system::SetParameter("web.blankless.enabled", "false");
    auto info3 = OH_NativeArkWeb_GetBlanklessInfoWithKey("", "");
    EXPECT_EQ(info3.errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT);
    auto info4 = OH_NativeArkWeb_GetBlanklessInfoWithKey("", "OH_NativeArkWeb_GetBlanklessInfoWithKey");
    EXPECT_EQ(info4.errCode,  ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT);

    OHOS::system::SetParameter("web.blankless.enabled", enabled ? "true" : "false");
}

/**
 * @tc.name  : OH_NativeArkWeb_SetBlanklessLoadingWithKey_01
 * @tc.desc  : Test OH_NativeArkWeb_SetBlanklessLoadingWithKey
 */
HWTEST_F(NativeInterfaceArkWebTest, OH_NativeArkWeb_SetBlanklessLoadingWithKey_01, TestSize.Level1) {
    bool enabled = OHOS::system::GetBoolParameter("web.blankless.enabled", false);

    OHOS::system::SetParameter("web.blankless.enabled", "true");
    auto errCode1 = OH_NativeArkWeb_SetBlanklessLoadingWithKey("", "", true);
    EXPECT_EQ(errCode1, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS);
    auto errCode2 = OH_NativeArkWeb_SetBlanklessLoadingWithKey("", "OH_NativeArkWeb_SetBlanklessLoadingWithKey", false);
    EXPECT_EQ(errCode2, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_UNKNOWN);

    OHOS::system::SetParameter("web.blankless.enabled", "false");
    auto errCode3 = OH_NativeArkWeb_SetBlanklessLoadingWithKey("", "", true);
    EXPECT_EQ(errCode3, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT);
    auto errCode4 = OH_NativeArkWeb_SetBlanklessLoadingWithKey("", "OH_NativeArkWeb_SetBlanklessLoadingWithKey", false);
    EXPECT_EQ(errCode4, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT);

    OHOS::system::SetParameter("web.blankless.enabled", enabled ? "true" : "false");
}

/**
 * @tc.name  : OH_NativeArkWeb_ClearBlanklessLoadingCache_01
 * @tc.desc  : Test OH_NativeArkWeb_ClearBlanklessLoadingCache
 */
HWTEST_F(NativeInterfaceArkWebTest, OH_NativeArkWeb_ClearBlanklessLoadingCache_01, TestSize.Level1) {
    bool enabled = OHOS::system::GetBoolParameter("web.blankless.enabled", false);

    OHOS::system::SetParameter("web.blankless.enabled", "false");
    OH_NativeArkWeb_ClearBlanklessLoadingCache(nullptr, 0);

    OHOS::system::SetParameter("web.blankless.enabled", "true");
    OH_NativeArkWeb_ClearBlanklessLoadingCache(nullptr, 0);

    const char* keys1[] = {};
    EXPECT_EQ(sizeof(keys1) / sizeof(const char*), 0);
    OH_NativeArkWeb_ClearBlanklessLoadingCache(keys1, 0);

    std::string longStr;
    // Test for key length over MAX_KEY_LENGTH, which is 2048.
    for (uint32_t idx = 0; idx < 3000; idx++) {
        longStr += "0";
    }
    const char* keys2[3];
    keys2[0] = "ClearBlanklessLoadingCache";
    keys2[1] = "";
    keys2[2] = longStr.c_str();
    EXPECT_EQ(sizeof(keys2) / sizeof(const char*), 3);
    OH_NativeArkWeb_ClearBlanklessLoadingCache(keys2, 3);

    const char* keys3[101];
    // Test for key size over MAX_KEY_COUNT, which is 100.
    for (uint32_t idx = 0; idx < 101; idx++) {
        keys3[idx] = "test";
    }
    EXPECT_EQ(sizeof(keys3) / sizeof(const char*), 101);
    OH_NativeArkWeb_ClearBlanklessLoadingCache(keys3, 101);

    const char* keys4[] = { nullptr };
    EXPECT_EQ(sizeof(keys4) / sizeof(const char*), 1);
    OH_NativeArkWeb_ClearBlanklessLoadingCache(keys4, 1);

    OHOS::system::SetParameter("web.blankless.enabled", enabled ? "true" : "false");
}

/**
 * @tc.name  : OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity_01
 * @tc.desc  : Test OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity
 */
HWTEST_F(NativeInterfaceArkWebTest, OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity_01, TestSize.Level1) {
    bool enabled = OHOS::system::GetBoolParameter("web.blankless.enabled", false);

    OHOS::system::SetParameter("web.blankless.enabled", "true");
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(0), 0);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(20), 20);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(100), 100);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(1000), 100);

    OHOS::system::SetParameter("web.blankless.enabled", "false");
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(0), 0);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(20), 0);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(100), 0);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(1000), 0);

    OHOS::system::SetParameter("web.blankless.enabled", enabled ? "true" : "false");
}

/**
 * @tc.name  : OH_NativeArkWeb_Blankless_Dual_Core_01
 * @tc.desc  : Test OH_NativeArkWeb_Blankless_Dual_Core
 */
HWTEST_F(NativeInterfaceArkWebTest, OH_NativeArkWeb_Blankless_Dual_Core_01, TestSize.Level1) {
    bool defaultStatus = OHOS::system::GetBoolParameter("web.blankless.enabled", false);
    OHOS::system::SetParameter("web.blankless.enabled", "true");
    auto version = ArkWeb::getActiveWebEngineVersion();
    ArkWeb::setActiveWebEngineVersion(ArkWeb::ArkWebEngineVersion::M114);
    auto info = OH_NativeArkWeb_GetBlanklessInfoWithKey("", "");
    EXPECT_EQ(info.errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT);
    auto errCode = OH_NativeArkWeb_SetBlanklessLoadingWithKey("", "", true);
    EXPECT_EQ(errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_DEVICE_NOT_SUPPORT);
    OH_NativeArkWeb_ClearBlanklessLoadingCache(nullptr, 0);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(20), 0);

    ArkWeb::setActiveWebEngineVersion(ArkWeb::ArkWebEngineVersion::M132);
    info = OH_NativeArkWeb_GetBlanklessInfoWithKey("", "");
    EXPECT_EQ(info.errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS);
    errCode = OH_NativeArkWeb_SetBlanklessLoadingWithKey("", "", true);
    EXPECT_EQ(errCode, ArkWeb_BlanklessErrorCode::ARKWEB_BLANKLESS_ERR_INVALID_ARGS);
    OH_NativeArkWeb_ClearBlanklessLoadingCache(nullptr, 0);
    EXPECT_EQ(OH_NativeArkWeb_SetBlanklessLoadingCacheCapacity(20), 20);

    ArkWeb::setActiveWebEngineVersion(version);
    OHOS::system::SetParameter("web.blankless.enabled", defaultStatus ? "true" : "false");
}

/**
 * @tc.name  : OH_ArkWebCookieManager_SaveCookieSync_01
 * @tc.desc  : Test OH_ArkWebCookieManager_SaveCookieSync
 */
HWTEST_F(NativeInterfaceArkWebTest, OH_ArkWebCookieManager_SaveCookieSync_01, TestSize.Level1) {
    ArkWeb_ErrorCode ret = OH_ArkWebCookieManager_SaveCookieSync();
    EXPECT_EQ(ret, ArkWeb_ErrorCode::ARKWEB_COOKIE_MANAGER_INITIALIZE_FAILED);
}

/**
 * @tc.name  : OH_ArkWebCookieManager_SaveCookieAsync_01
 * @tc.desc  : Test OH_ArkWebCookieManager_SaveCookieAsync
 */
HWTEST_F(NativeInterfaceArkWebTest, OH_ArkWebCookieManager_SaveCookieAsync_01, TestSize.Level1) {
    OH_ArkWeb_OnCookieSaveCallback callback = nullptr;
    g_errorCode = ArkWeb_ErrorCode::ARKWEB_SUCCESS;
    OH_ArkWebCookieManager_SaveCookieAsync(callback);
    EXPECT_EQ(g_errorCode, ArkWeb_ErrorCode::ARKWEB_SUCCESS);

    g_errorCode = ArkWeb_ErrorCode::ARKWEB_SUCCESS;
    callback = OH_ArkWeb_OnCookieSaveCallbackImpl;
    OH_ArkWebCookieManager_SaveCookieAsync(callback);
    EXPECT_EQ(g_errorCode, ArkWeb_ErrorCode::ARKWEB_COOKIE_MANAGER_INITIALIZE_FAILED);
}

} // namespace NWeb
} // namesapce OHOS