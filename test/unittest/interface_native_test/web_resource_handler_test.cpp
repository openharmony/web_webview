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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "base/web/webview/interfaces/native/arkweb_scheme_handler.h"
#include "base/web/webview/interfaces/native/arkweb_error_code.h"
#include "base/web/webview/interfaces/native/arkweb_net_error_list.h"
#include "base/web/webview/interfaces/kits/nativecommon/web_resource_handler.h"

using namespace testing::ext;

namespace OHOS {
namespace NWeb {
class MockArkWeb {
public:
    MOCK_METHOD(int32_t, OH_ArkWebResourceHandler_DidReceiveResponse,
        (const ArkWeb_ResourceHandler* handler, const ArkWeb_Response* response));
    MOCK_METHOD(int32_t, OH_ArkWebResourceHandler_DidReceiveData,
        (const ArkWeb_ResourceHandler* handler, const uint8_t* buffer, int64_t buflen));
    MOCK_METHOD(int32_t, OH_ArkWebResourceHandler_DidFinish, (const ArkWeb_ResourceHandler* handler));
    MOCK_METHOD(int32_t, OH_ArkWebResourceHandler_DidFailWithErrorV2,
        (const ArkWeb_ResourceHandler* handler, ArkWeb_NetError errorCode, bool completeIfNoResponse));
    MOCK_METHOD(int32_t, OH_ArkWebResourceHandler_Destroy, (const ArkWeb_ResourceHandler* handler));
};

MockArkWeb* g_mockArkWeb = nullptr;

extern "C" {
int32_t OH_ArkWebResourceHandler_DidReceiveResponse(
    const ArkWeb_ResourceHandler* handler, const ArkWeb_Response* response)
{
    return g_mockArkWeb->OH_ArkWebResourceHandler_DidReceiveResponse(handler, response);
}

int32_t OH_ArkWebResourceHandler_DidReceiveData(
    const ArkWeb_ResourceHandler* handler, const uint8_t* buffer, int64_t buflen)
{
    return g_mockArkWeb->OH_ArkWebResourceHandler_DidReceiveData(handler, buffer, buflen);
}

int32_t OH_ArkWebResourceHandler_DidFinish(const ArkWeb_ResourceHandler* handler)
{
    return g_mockArkWeb->OH_ArkWebResourceHandler_DidFinish(handler);
}

int32_t OH_ArkWebResourceHandler_DidFailWithErrorV2(
    const ArkWeb_ResourceHandler* handler, ArkWeb_NetError errorCode, bool completeIfNoResponse)
{
    return g_mockArkWeb->OH_ArkWebResourceHandler_DidFailWithErrorV2(handler, errorCode, completeIfNoResponse);
}

int32_t OH_ArkWebResourceHandler_Destroy(const ArkWeb_ResourceHandler* handler)
{
    return g_mockArkWeb->OH_ArkWebResourceHandler_Destroy(handler);
}
}

class WebResourceHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void WebResourceHandlerTest::SetUpTestCase(void) {}

void WebResourceHandlerTest::TearDownTestCase(void) {}

void WebResourceHandlerTest::SetUp()
{
    g_mockArkWeb = new MockArkWeb();
}

void WebResourceHandlerTest::TearDown()
{
    delete g_mockArkWeb;
    g_mockArkWeb = nullptr;
}

HWTEST_F(WebResourceHandlerTest, DidReceiveResponse_WhenFinished_ReturnsError, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x5678);
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    handler.SetFinishFlag();

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidReceiveResponse(testing::_, testing::_)).Times(0);

    int32_t result = handler.DidReceiveResponse(mockResponse);
    EXPECT_EQ(ARKWEB_ERROR_UNKNOWN, result);
}

HWTEST_F(WebResourceHandlerTest, DidReceiveResponse_WhenNotFinished_CallsExternalFunction, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x5678);
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidReceiveResponse(mockHandler, mockResponse))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    int32_t result = handler.DidReceiveResponse(mockResponse);
    EXPECT_EQ(ARKWEB_SUCCESS, result);
}

HWTEST_F(WebResourceHandlerTest, DidReceiveResponseBody_WhenFinished_ReturnsError, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    handler.SetFinishFlag();

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidReceiveData(testing::_, testing::_, testing::_))
        .Times(0);

    uint8_t buffer[10] = { 0 };
    int64_t buflen = 10;
    int32_t result = handler.DidReceiveResponseBody(buffer, buflen);
    EXPECT_EQ(ARKWEB_ERROR_UNKNOWN, result);
}

HWTEST_F(WebResourceHandlerTest, DidReceiveResponseBody_WhenNotFinished_CallsExternalFunction, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    uint8_t buffer[10] = { 0 };
    int64_t buflen = 10;
    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidReceiveData(mockHandler, buffer, buflen))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    int32_t result = handler.DidReceiveResponseBody(buffer, buflen);
    EXPECT_EQ(ARKWEB_SUCCESS, result);
}

HWTEST_F(WebResourceHandlerTest, DidFinish_WhenFinished_ReturnsError, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    handler.SetFinishFlag();

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFinish(testing::_)).Times(0);

    int32_t result = handler.DidFinish();
    EXPECT_EQ(ARKWEB_ERROR_UNKNOWN, result);
}

HWTEST_F(WebResourceHandlerTest, DidFinish_WhenNotFinished_SetsFinishedFlag, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFinish(mockHandler))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    int32_t result = handler.DidFinish();
    EXPECT_EQ(ARKWEB_SUCCESS, result);

    int32_t secondResult = handler.DidFinish();
    EXPECT_EQ(ARKWEB_ERROR_UNKNOWN, secondResult);
}

HWTEST_F(WebResourceHandlerTest, DidFailWithError_WhenFinished_ReturnsError, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    handler.SetFinishFlag();

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFailWithErrorV2(testing::_, testing::_, testing::_))
        .Times(0);

    int32_t result = handler.DidFailWithError(ARKWEB_ERR_FAILED, true);
    EXPECT_EQ(ARKWEB_ERROR_UNKNOWN, result);
}

HWTEST_F(WebResourceHandlerTest, DidFailWithError_WhenNotFinished_SetsFinishedFlag, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFailWithErrorV2(mockHandler, ARKWEB_ERR_FAILED, true))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    int32_t result = handler.DidFailWithError(ARKWEB_ERR_FAILED, true);
    EXPECT_EQ(ARKWEB_SUCCESS, result);

    int32_t secondResult = handler.DidFailWithError(ARKWEB_ERR_FAILED, true);
    EXPECT_EQ(ARKWEB_ERROR_UNKNOWN, secondResult);
}

HWTEST_F(WebResourceHandlerTest, DestoryArkWebResourceHandler_WhenHandlerIsNull_DoesNothing, TestSize.Level1)
{
    WebResourceHandler handler;

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_Destroy(testing::_)).Times(0);

    EXPECT_NO_FATAL_FAILURE(handler.DestoryArkWebResourceHandler());
}

HWTEST_F(WebResourceHandlerTest, DestoryArkWebResourceHandler_WhenHandlerNotNull_DestroysHandler, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_Destroy(mockHandler))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    EXPECT_NO_FATAL_FAILURE(handler.DestoryArkWebResourceHandler());
}

HWTEST_F(WebResourceHandlerTest, DidFinish_WhenReturnsNonZero_DoesNotSetFinished, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFinish(mockHandler))
        .WillOnce(testing::Return(ARKWEB_INVALID_PARAM));

    int32_t result = handler.DidFinish();
    EXPECT_EQ(ARKWEB_INVALID_PARAM, result);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFinish(mockHandler))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    result = handler.DidFinish();
    EXPECT_EQ(ARKWEB_SUCCESS, result);
}

HWTEST_F(WebResourceHandlerTest, DidFailWithError_WhenReturnsNonZero_DoesNotSetFinished, TestSize.Level1)
{
    ArkWeb_ResourceHandler* mockHandler = reinterpret_cast<ArkWeb_ResourceHandler*>(0x1234);
    WebResourceHandler handler(mockHandler);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFailWithErrorV2(mockHandler, ARKWEB_ERR_FAILED, true))
        .WillOnce(testing::Return(ARKWEB_INVALID_PARAM));

    int32_t result = handler.DidFailWithError(ARKWEB_ERR_FAILED, true);
    EXPECT_EQ(ARKWEB_INVALID_PARAM, result);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResourceHandler_DidFailWithErrorV2(mockHandler, ARKWEB_ERR_FAILED, true))
        .WillOnce(testing::Return(ARKWEB_SUCCESS));

    result = handler.DidFailWithError(ARKWEB_ERR_FAILED, true);
    EXPECT_EQ(ARKWEB_SUCCESS, result);
}
} // namespace NWeb
} // namespace OHOS