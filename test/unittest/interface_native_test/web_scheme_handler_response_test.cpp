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

#include "base/web/webview/interfaces/kits/nativecommon/web_scheme_handler_response.h"

using namespace testing::ext;

namespace OHOS {
namespace NWeb {
class MockArkWeb {
public:
    MOCK_METHOD(void, OH_ArkWebResponse_GetUrl, (const ArkWeb_Response* response, char** url));
    MOCK_METHOD(void, OH_ArkWebResponse_GetStatusText, (const ArkWeb_Response* response, char** statusText));
    MOCK_METHOD(void, OH_ArkWebResponse_GetMimeType, (const ArkWeb_Response* response, char** mimeType));
    MOCK_METHOD(void, OH_ArkWebResponse_GetCharset, (const ArkWeb_Response* response, char** charset));
    MOCK_METHOD(
        void, OH_ArkWebResponse_GetHeaderByName, (const ArkWeb_Response* response, const char* name, char** value));
    MOCK_METHOD(void, OH_ArkWeb_DestroyResponse, (ArkWeb_Response * response));
};

MockArkWeb* g_mockArkWeb = nullptr;

extern "C" {
void OH_ArkWebResponse_GetUrl(const ArkWeb_Response* response, char** url)
{
    g_mockArkWeb->OH_ArkWebResponse_GetUrl(response, url);
}

void OH_ArkWebResponse_GetStatusText(const ArkWeb_Response* response, char** statusText)
{
    g_mockArkWeb->OH_ArkWebResponse_GetStatusText(response, statusText);
}

void OH_ArkWebResponse_GetMimeType(const ArkWeb_Response* response, char** mimeType)
{
    g_mockArkWeb->OH_ArkWebResponse_GetMimeType(response, mimeType);
}

void OH_ArkWebResponse_GetCharset(const ArkWeb_Response* response, char** charset)
{
    g_mockArkWeb->OH_ArkWebResponse_GetCharset(response, charset);
}

void OH_ArkWebResponse_GetHeaderByName(const ArkWeb_Response* response, const char* name, char** value)
{
    g_mockArkWeb->OH_ArkWebResponse_GetHeaderByName(response, name, value);
}

void OH_ArkWeb_DestroyResponse(ArkWeb_Response* response)
{
    g_mockArkWeb->OH_ArkWeb_DestroyResponse(response);
}
}

class WebSchemeHandlerResponseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void WebSchemeHandlerResponseTest::SetUpTestCase(void) {}

void WebSchemeHandlerResponseTest::TearDownTestCase(void) {}

void WebSchemeHandlerResponseTest::SetUp()
{
    g_mockArkWeb = new MockArkWeb();
}

void WebSchemeHandlerResponseTest::TearDown()
{
    delete g_mockArkWeb;
    g_mockArkWeb = nullptr;
}

HWTEST_F(WebSchemeHandlerResponseTest, GetUrl_WhenResponseIsNull_ReturnsNull, TestSize.Level1)
{
    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(nullptr)).Times(1);
    WebSchemeHandlerResponse response(nullptr);
    char* url = response.GetUrl();
    EXPECT_EQ(nullptr, url);
}

HWTEST_F(WebSchemeHandlerResponseTest, GetUrl_WhenResponseIsNotNull_ReturnsUrl, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x1234);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(mockResponse)).Times(1);

    {
        WebSchemeHandlerResponse response(mockResponse);

        char* testUrl = const_cast<char*>("https://example.com");
        EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResponse_GetUrl(mockResponse, testing::_))
            .WillOnce(testing::SetArgPointee<1>(testUrl));

        char* url = response.GetUrl();

        EXPECT_NE(nullptr, url);
        EXPECT_STREQ("https://example.com", url);
    }
}

HWTEST_F(WebSchemeHandlerResponseTest, GetStatusText_WhenResponseIsNull_ReturnsNull, TestSize.Level1)
{
    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(nullptr)).Times(1);
    WebSchemeHandlerResponse response(nullptr);
    char* statusText = response.GetStatusText();
    EXPECT_EQ(nullptr, statusText);
}

HWTEST_F(WebSchemeHandlerResponseTest, GetStatusText_WhenResponseIsNotNull_ReturnsStatusText, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x1234);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(mockResponse)).Times(1);

    {
        WebSchemeHandlerResponse response(mockResponse);

        char* testStatusText = const_cast<char*>("OK");
        EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResponse_GetStatusText(mockResponse, testing::_))
            .WillOnce(testing::SetArgPointee<1>(testStatusText));

        char* statusText = response.GetStatusText();

        EXPECT_NE(nullptr, statusText);
        EXPECT_STREQ("OK", statusText);
    }
}

HWTEST_F(WebSchemeHandlerResponseTest, GetMimeType_WhenResponseIsNull_ReturnsNull, TestSize.Level1)
{
    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(nullptr)).Times(1);
    WebSchemeHandlerResponse response(nullptr);
    char* mimeType = response.GetMimeType();
    EXPECT_EQ(nullptr, mimeType);
}

HWTEST_F(WebSchemeHandlerResponseTest, GetMimeType_WhenResponseIsNotNull_ReturnsMimeType, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x1234);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(mockResponse)).Times(1);

    {
        WebSchemeHandlerResponse response(mockResponse);

        char* testMimeType = const_cast<char*>("text/html");
        EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResponse_GetMimeType(mockResponse, testing::_))
            .WillOnce(testing::SetArgPointee<1>(testMimeType));

        char* mimeType = response.GetMimeType();

        EXPECT_NE(nullptr, mimeType);
        EXPECT_STREQ("text/html", mimeType);
    }
}

HWTEST_F(WebSchemeHandlerResponseTest, GetEncoding_WhenResponseIsNull_ReturnsNull, TestSize.Level1)
{
    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(nullptr)).Times(1);
    WebSchemeHandlerResponse response(nullptr);
    char* encoding = response.GetEncoding();
    EXPECT_EQ(nullptr, encoding);
}

HWTEST_F(WebSchemeHandlerResponseTest, GetEncoding_WhenResponseIsNotNull_ReturnsEncoding, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x1234);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(mockResponse)).Times(1);

    {
        WebSchemeHandlerResponse response(mockResponse);

        char* testEncoding = const_cast<char*>("UTF-8");
        EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResponse_GetCharset(mockResponse, testing::_))
            .WillOnce(testing::SetArgPointee<1>(testEncoding));

        char* encoding = response.GetEncoding();

        EXPECT_NE(nullptr, encoding);
        EXPECT_STREQ("UTF-8", encoding);
    }
}

HWTEST_F(WebSchemeHandlerResponseTest, GetHeaderByName_WhenResponseIsNull_ReturnsNull, TestSize.Level1)
{
    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(nullptr)).Times(1);
    WebSchemeHandlerResponse response(nullptr);
    char* headerValue = response.GetHeaderByName("Content-Type");
    EXPECT_EQ(nullptr, headerValue);
}

HWTEST_F(WebSchemeHandlerResponseTest, GetHeaderByName_WhenResponseIsNotNull_ReturnsHeaderValue, TestSize.Level1)
{
    ArkWeb_Response* mockResponse = reinterpret_cast<ArkWeb_Response*>(0x1234);

    EXPECT_CALL(*g_mockArkWeb, OH_ArkWeb_DestroyResponse(mockResponse)).Times(1);

    {
        WebSchemeHandlerResponse response(mockResponse);

        char* testHeaderValue = const_cast<char*>("application/json");
        EXPECT_CALL(*g_mockArkWeb, OH_ArkWebResponse_GetHeaderByName(mockResponse, "Content-Type", testing::_))
            .WillOnce(testing::SetArgPointee<2>(testHeaderValue));

        char* headerValue = response.GetHeaderByName("Content-Type");

        EXPECT_NE(nullptr, headerValue);
        EXPECT_STREQ("application/json", headerValue);
    }
}
} // namespace NWeb
} // namespace OHOS