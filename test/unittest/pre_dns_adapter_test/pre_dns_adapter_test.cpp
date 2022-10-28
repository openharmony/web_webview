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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "nweb_pre_dns_adapter.h"
#include "ohos_adapter_helper.h"
#include "ohos_web_data_base_adapter.h"
#define private public
#include "ohos_web_dns_data_base_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;

namespace OHOS::NWeb {
class OhosWebDnsDataBaseAdapterMock : public OhosWebDnsDataBaseAdapterImpl {
public:
    MOCK_CONST_METHOD1(GetHostnames, void(std::vector<std::string>&));
};

class PreDnsAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

static OhosWebDnsDataBaseAdapterMock *g_adapterMock;
OhosWebDnsDataBaseAdapter &OhosAdapterHelper::GetWebDnsDataBaseInstance()
{
    std::cout << "GetWebDnsDataBaseInstance\n";
    return *g_adapterMock;
}

void PreDnsAdapterTest::SetUpTestCase(void)
{
    g_adapterMock = new OhosWebDnsDataBaseAdapterMock();
    ASSERT_NE(g_adapterMock, nullptr);
}

void PreDnsAdapterTest::TearDownTestCase(void)
{
    delete g_adapterMock;
}

void PreDnsAdapterTest::SetUp(void)
{}

void PreDnsAdapterTest::TearDown(void)
{}

/**
 * @tc.name: PreDnsAdapterTest_001.
 * @tc.desc: test pre dns
 * @tc.type: FUNC
 * @tc.require: issueI5W74R
 */
HWTEST_F(PreDnsAdapterTest, PreDnsAdapterTest_001, TestSize.Level1)
{
    std::vector<std::string> hostInfo;
    EXPECT_CALL(*g_adapterMock, GetHostnames(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::SetArgReferee<0>(hostInfo));
    PreDnsInThread();

    hostInfo.push_back("m.pinduoduo.com");
    hostInfo.push_back("mcdn.pinduoduo.com");
    EXPECT_CALL(*g_adapterMock, GetHostnames(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::SetArgReferee<0>(hostInfo));
    PreDnsInThread();

    hostInfo.push_back("test_error");
    EXPECT_CALL(*g_adapterMock, GetHostnames(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::SetArgReferee<0>(hostInfo));
    PreDnsInThread();
}

} // namespace NWeb
