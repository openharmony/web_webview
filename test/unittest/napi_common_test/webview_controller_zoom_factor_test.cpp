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

#include <limits>
#include <memory>

#include <gtest/gtest.h>

#include "nweb_helper.h"

#include "../common/nweb_create_window.h"
#include "napi_webview_controller.h"
#include "webview_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::NWeb {
namespace {
constexpr int32_t NWEB_ID = 1;
constexpr double BELOW_MINIMUM_FACTOR = 0.1;
constexpr double MINIMUM_FACTOR = 0.25;
constexpr double MAXIMUM_FACTOR = 5.0;
constexpr double ABOVE_MAXIMUM_FACTOR = 10.0;

class ZoomFactorNWeb final : public NWebMock {
public:
    void SetZoomFactor(double factor) override
    {
        setZoomFactorCalled_ = true;
        lastZoomFactor_ = factor;
    }

    double GetZoomFactor() override
    {
        return zoomFactor_;
    }

    void SetZoomFactorForTest(double factor)
    {
        zoomFactor_ = factor;
    }

    bool WasSetZoomFactorCalled() const
    {
        return setZoomFactorCalled_;
    }

    double GetLastZoomFactor() const
    {
        return lastZoomFactor_;
    }

private:
    bool setZoomFactorCalled_ = false;
    double lastZoomFactor_ = 1.0;
    double zoomFactor_ = 1.0;
};

class ZoomFactorNWebEngine final : public NWebEngine {
public:
    explicit ZoomFactorNWebEngine(std::shared_ptr<NWeb> nweb) : nweb_(std::move(nweb)) {}

    std::shared_ptr<NWeb> CreateNWeb(std::shared_ptr<NWebCreateInfo>) override
    {
        return nweb_;
    }

    std::shared_ptr<NWeb> GetNWeb(int32_t) override
    {
        return nweb_;
    }

    std::shared_ptr<NWebDataBase> GetDataBase() override
    {
        return nullptr;
    }

    std::shared_ptr<NWebWebStorage> GetWebStorage() override
    {
        return nullptr;
    }

    std::shared_ptr<NWebCookieManager> GetCookieManager() override
    {
        return nullptr;
    }

    std::shared_ptr<NWebDownloadManager> GetDownloadManager() override
    {
        return nullptr;
    }

    void SetWebTag(int32_t, const char*) override {}
    void InitializeWebEngine(std::shared_ptr<NWebEngineInitArgs>) override {}
    void PrepareForPageLoad(const std::string&, bool, int32_t) override {}
    void SetWebDebuggingAccess(bool) override {}
    void AddIntelligentTrackingPreventionBypassingList(const std::vector<std::string>&) override {}
    void RemoveIntelligentTrackingPreventionBypassingList(const std::vector<std::string>&) override {}
    void ClearIntelligentTrackingPreventionBypassingList() override {}
    void PauseAllTimers() override {}
    void ResumeAllTimers() override {}
    void PrefetchResource(const std::shared_ptr<NWebEnginePrefetchArgs>&,
        const std::map<std::string, std::string>&, const std::string&, const uint32_t&) override {}
    void SetRenderProcessMode(RenderProcessMode) override {}
    RenderProcessMode GetRenderProcessMode() override
    {
        return RenderProcessMode::SINGLE_MODE;
    }
    void ClearPrefetchedResource(const std::vector<std::string>&) override {}
    void WarmupServiceWorker(const std::string&) override {}
    void SetHostIP(const std::string&, const std::string&, int32_t) override {}
    void ClearHostIP(const std::string&) override {}
    void SetWholeWebDrawing() override {}

private:
    std::shared_ptr<NWeb> nweb_;
};
} // namespace

class WebviewControllerZoomFactorTest : public testing::Test {
public:
    void SetUp() override
    {
        nweb_ = std::make_shared<ZoomFactorNWeb>();
        NWebHelper::Instance().SetNWebEngineForTest(std::make_shared<ZoomFactorNWebEngine>(nweb_));
    }

    void TearDown() override
    {
        NWebHelper::Instance().SetNWebEngineForTest(nullptr);
    }

protected:
    std::shared_ptr<ZoomFactorNWeb> nweb_;
};

class NapiWebviewControllerZoomFactorTest : public WebviewControllerZoomFactorTest {
public:
    void SetUp() override
    {
        WebviewControllerZoomFactorTest::SetUp();
        ASSERT_EQ(napi_create_ark_runtime(&env_), napi_ok);
        ASSERT_EQ(napi_create_object(env_, &receiver_), napi_ok);
        ASSERT_EQ(napi_wrap(env_, receiver_, &controller_, nullptr, nullptr, nullptr), napi_ok);
        ASSERT_EQ(napi_create_function(env_, "setZoomFactor", NAPI_AUTO_LENGTH, SetZoomFactorCallback,
            nullptr, &setZoomFactorFunction_), napi_ok);
        ASSERT_EQ(napi_create_function(env_, "getZoomFactor", NAPI_AUTO_LENGTH, GetZoomFactorCallback,
            nullptr, &getZoomFactorFunction_), napi_ok);
    }

    void TearDown() override
    {
        if (env_ != nullptr) {
            EXPECT_EQ(napi_destroy_ark_runtime(&env_), napi_ok);
        }
        WebviewControllerZoomFactorTest::TearDown();
    }

protected:
    static napi_value SetZoomFactorCallback(napi_env env, napi_callback_info info)
    {
        return NapiWebviewController::SetZoomFactor(env, info);
    }

    static napi_value GetZoomFactorCallback(napi_env env, napi_callback_info info)
    {
        return NapiWebviewController::GetZoomFactor(env, info);
    }

    void ExpectExceptionAndClear()
    {
        bool isPending = false;
        ASSERT_EQ(napi_is_exception_pending(env_, &isPending), napi_ok);
        EXPECT_TRUE(isPending);
        napi_value exception = nullptr;
        ASSERT_EQ(napi_get_and_clear_last_exception(env_, &exception), napi_ok);
        EXPECT_NE(exception, nullptr);
    }

    WebviewController controller_ { NWEB_ID };
    napi_env env_ = nullptr;
    napi_value receiver_ = nullptr;
    napi_value setZoomFactorFunction_ = nullptr;
    napi_value getZoomFactorFunction_ = nullptr;
};

/**
 * @tc.name: WebviewControllerZoomFactorTest_SetZoomFactorBoundary_001
 * @tc.desc: Verify the real WebviewController clamps finite zoom factors to the supported range.
 * @tc.type: FUNC
 */
HWTEST_F(WebviewControllerZoomFactorTest, WebviewControllerZoomFactorTest_SetZoomFactorBoundary_001, TestSize.Level1)
{
    WebviewController controller(NWEB_ID);

    EXPECT_EQ(controller.SetZoomFactor(BELOW_MINIMUM_FACTOR), NO_ERROR);
    EXPECT_DOUBLE_EQ(nweb_->GetLastZoomFactor(), MINIMUM_FACTOR);

    EXPECT_EQ(controller.SetZoomFactor(MINIMUM_FACTOR), NO_ERROR);
    EXPECT_DOUBLE_EQ(nweb_->GetLastZoomFactor(), MINIMUM_FACTOR);

    EXPECT_EQ(controller.SetZoomFactor(MAXIMUM_FACTOR), NO_ERROR);
    EXPECT_DOUBLE_EQ(nweb_->GetLastZoomFactor(), MAXIMUM_FACTOR);

    EXPECT_EQ(controller.SetZoomFactor(ABOVE_MAXIMUM_FACTOR), NO_ERROR);
    EXPECT_DOUBLE_EQ(nweb_->GetLastZoomFactor(), MAXIMUM_FACTOR);
    EXPECT_TRUE(nweb_->WasSetZoomFactorCalled());
}

/**
 * @tc.name: WebviewControllerZoomFactorTest_SetGetZoomFactor_001
 * @tc.desc: Verify the real WebviewController rejects non-finite input and gets the NWeb factor.
 * @tc.type: FUNC
 */
HWTEST_F(WebviewControllerZoomFactorTest, WebviewControllerZoomFactorTest_SetGetZoomFactor_001, TestSize.Level1)
{
    WebviewController controller(NWEB_ID);
    double factor = 0.0;

    EXPECT_EQ(controller.SetZoomFactor(std::numeric_limits<double>::quiet_NaN()), PARAM_CHECK_ERROR);
    EXPECT_EQ(controller.SetZoomFactor(std::numeric_limits<double>::infinity()), PARAM_CHECK_ERROR);

    nweb_->SetZoomFactorForTest(1.37);
    EXPECT_EQ(controller.GetZoomFactor(factor), NO_ERROR);
    EXPECT_DOUBLE_EQ(factor, 1.37);
}

/**
 * @tc.name: NapiWebviewControllerZoomFactorTest_SetGetZoomFactor_001
 * @tc.desc: Verify NAPI set/get zoom factor success paths use the real controller.
 * @tc.type: FUNC
 */
HWTEST_F(NapiWebviewControllerZoomFactorTest, NapiWebviewControllerZoomFactorTest_SetGetZoomFactor_001, TestSize.Level1)
{
    napi_value input = nullptr;
    napi_value result = nullptr;
    ASSERT_EQ(napi_create_double(env_, ABOVE_MAXIMUM_FACTOR, &input), napi_ok);
    ASSERT_EQ(napi_call_function(env_, receiver_, setZoomFactorFunction_, 1, &input, &result), napi_ok);
    EXPECT_DOUBLE_EQ(nweb_->GetLastZoomFactor(), MAXIMUM_FACTOR);

    nweb_->SetZoomFactorForTest(1.37);
    ASSERT_EQ(napi_call_function(env_, receiver_, getZoomFactorFunction_, 0, nullptr, &result), napi_ok);
    double factor = 0.0;
    ASSERT_EQ(napi_get_value_double(env_, result, &factor), napi_ok);
    EXPECT_DOUBLE_EQ(factor, 1.37);
}

/**
 * @tc.name: NapiWebviewControllerZoomFactorTest_SetZoomFactorParameterError_001
 * @tc.desc: Verify NAPI set zoom factor rejects invalid argument count and type.
 * @tc.type: FUNC
 */
HWTEST_F(NapiWebviewControllerZoomFactorTest,
    NapiWebviewControllerZoomFactorTest_SetZoomFactorParameterError_001, TestSize.Level1)
{
    napi_value result = nullptr;
    EXPECT_EQ(napi_call_function(env_, receiver_, setZoomFactorFunction_, 0, nullptr, &result), napi_pending_exception);
    ExpectExceptionAndClear();

    napi_value input = nullptr;
    ASSERT_EQ(napi_create_string_utf8(env_, "invalid", NAPI_AUTO_LENGTH, &input), napi_ok);
    EXPECT_EQ(napi_call_function(env_, receiver_, setZoomFactorFunction_, 1, &input, &result), napi_pending_exception);
    ExpectExceptionAndClear();

    ASSERT_EQ(napi_create_double(env_, std::numeric_limits<double>::quiet_NaN(), &input), napi_ok);
    EXPECT_EQ(napi_call_function(env_, receiver_, setZoomFactorFunction_, 1, &input, &result), napi_pending_exception);
    ExpectExceptionAndClear();

    ASSERT_EQ(napi_create_double(env_, std::numeric_limits<double>::infinity(), &input), napi_ok);
    EXPECT_EQ(napi_call_function(env_, receiver_, setZoomFactorFunction_, 1, &input, &result), napi_pending_exception);
    ExpectExceptionAndClear();
}

/**
 * @tc.name: NapiWebviewControllerZoomFactorTest_SetGetZoomFactorNotInitialized_001
 * @tc.desc: Verify NAPI set/get zoom factor rejects an uninitialized controller.
 * @tc.type: FUNC
 */
HWTEST_F(NapiWebviewControllerZoomFactorTest,
    NapiWebviewControllerZoomFactorTest_SetGetZoomFactorNotInitialized_001, TestSize.Level1)
{
    napi_value input = nullptr;
    napi_value result = nullptr;
    ASSERT_EQ(napi_create_double(env_, MINIMUM_FACTOR, &input), napi_ok);
    NWebHelper::Instance().SetNWebEngineForTest(nullptr);

    EXPECT_EQ(napi_call_function(env_, receiver_, setZoomFactorFunction_, 1, &input, &result), napi_pending_exception);
    ExpectExceptionAndClear();
    EXPECT_EQ(napi_call_function(env_, receiver_, getZoomFactorFunction_, 0, nullptr, &result), napi_pending_exception);
    ExpectExceptionAndClear();
}
} // namespace OHOS::NWeb
