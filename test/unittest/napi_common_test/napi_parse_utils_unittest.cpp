/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "napi_parse_utils.h"
#include "napi_parse_utils.cpp"
#include <sys/mman.h>
#include <unistd.h>
#include <regex>

#include "nweb.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "securec.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define MAX_FLOWBUF_DATA_SIZE 52428800 /* 50 MB */

namespace OHOS {
namespace NWeb {
    std::shared_ptr<NapiParseUtils> g_utils;
    class NapiParseUtilsTest : public testing::Test {
    public:
        static void SetUpTestCase(void);
        static void TearDownTestCase(void);
        void SetUp();
        void TearDown();
    };

    void NapiParseUtilsTest::SetUpTestCase(void)
    {}

    void NapiParseUtilsTest::TearDownTestCase(void)
    {}

    void NapiParseUtilsTest::SetUp(void)
    {
        g_utils = std::make_shared<NapiParseUtils>();
    }

    void NapiParseUtilsTest::TearDown(void)
    {
        g_utils = nullptr;
    }

    TEST_F(NapiParseUtilsTest, IsFormatStringOfLength)
    {
        std::string str = "str";
        bool result = false;
        result = IsFormatStringOfLength(str);
    }

    TEST_F(NapiParseUtilsTest, IsNumberOfLength)
    {
        std::string str = "str";
        bool result = false;
        result = IsNumberOfLength(str);
    }

    TEST_F(NapiParseUtilsTest, ParseJsLengthStringToInt)
    {
        std::string input = "input";
        int32_t value = 32;
        PixelUnit type = PixelUnit::PX;
        g_utils->ParseJsLengthStringToInt(input, type, value);
    }
} // namespace NWeb
} // namespace OHOS