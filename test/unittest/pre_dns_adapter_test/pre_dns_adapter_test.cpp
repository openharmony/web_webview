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

#include <gtest/gtest.h>
#include "nweb_pre_dns_adapter.h"
#include "ohos_web_data_base_adapter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::NWeb;

namespace {
bool g_testFlag1 = false;
bool g_testFlag2 = false;
bool g_testFlag3 = false;
}

namespace OHOS::NWeb {
class OhosWebDnsDataBaseAdapterMock : public OhosWebDnsDataBaseAdapter {
public:
    static OhosWebDnsDataBaseAdapterMock& GetInstance()
    {
        static OhosWebDnsDataBaseAdapterMock instance;
        return instance;
    }

    virtual ~OhosWebDnsDataBaseAdapterMock() = default;

    bool ExistHostname(const std::string& hostname) const override
    {
        return true;
    }

    void InsertHostname(const std::string& hostname) override {}
    
    void GetHostnames(std::vector<std::string>& hostnames) const override;

    void ClearAllHostname() override {}
};

void OhosWebDnsDataBaseAdapterMock::GetHostnames(std::vector<std::string>& hostnames) const
{
    if (g_testFlag1) {
        return;
    } else if (g_testFlag2) {
        hostnames.push_back("m.pinduoduo.com");
    } else if (g_testFlag3) {
        hostnames.push_back("getaddrinfo_error");
    }
}
 
class OhosAdapterHelper {
public:
    static OhosAdapterHelper &GetInstance()
    {
        static OhosAdapterHelper ohosAdapter;
        return ohosAdapter;
    }

    virtual ~OhosAdapterHelper() = default;

    OhosWebDnsDataBaseAdapter& GetWebDnsDataBaseInstance()
    {
        return OhosWebDnsDataBaseAdapterMock::GetInstance();
    }
private:
    OhosAdapterHelper() = default;

    OhosAdapterHelper(const OhosAdapterHelper&) = delete;

    OhosAdapterHelper& operator=(const OhosAdapterHelper&) = delete;
};

class PreDnsAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PreDnsAdapterTest::SetUpTestCase(void)
{}

void PreDnsAdapterTest::TearDownTestCase(void)
{}

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
    g_testFlag1 = true;
    PreDnsInThread();
    g_testFlag1 = false;

    g_testFlag2 = true;
    PreDnsInThread();
    g_testFlag2 = false;

    g_testFlag3 = true;
    PreDnsInThread();
    g_testFlag3 = false;
}

} // namespace NWeb
