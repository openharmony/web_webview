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

#include <cstring>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <unordered_map>

#include "nweb_config_helper.h"
#include "nweb_init_params.h"

using namespace testing;

namespace OHOS {
namespace NWebConfig {

class NWebConfigHelperTest : public ::testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void TearDown();

protected:
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs;
    NWebConfigHelper nWebConfigHelper;
    void SetUp() override
    {
        initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    }
};

/**
 * @tc.name: ParseWebConfigXml_FileNotFound
 * @tc.desc: ParseWebConfigXml.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseWebConfigXml_FileNotFound, TestSize.Level0)
{
    std::string configFilePath = "nonexistent.xml";
    EXPECT_NO_THROW(NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs));
}

/**
 * @tc.name: ParseWebConfigXml_InvalidRootElement
 * @tc.desc: ParseWebConfigXml.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseWebConfigXml_InvalidRootElement, TestSize.Level0)
{
    std::string configFilePath = "invalid_root.xml";
    EXPECT_NO_THROW(NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs));
}

/**
 * @tc.name: ParseWebConfigXml_ValidInitConfig
 * @tc.desc: ParseWebConfigXml.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseWebConfigXml_ValidInitConfig, TestSize.Level0)
{
    std::string configFilePath = "valid_init.xml";
    EXPECT_NO_THROW(NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs));
}

/**
 * @tc.name: ParseWebConfigXml_ValidDeleteConfig
 * @tc.desc: ParseWebConfigXml.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseWebConfigXml_ValidDeleteConfig, TestSize.Level0)
{
    std::string configFilePath = "valid_delete.xml";
    EXPECT_NO_THROW(NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs));
}

/**
 * @tc.name: ParseWebConfigXml_ValidPerformanceConfig
 * @tc.desc:ParseWebConfigXml.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseWebConfigXml_ValidPerformanceConfig, TestSize.Level0)
{
    std::string configFilePath = "valid_performance.xml";
    EXPECT_NO_THROW(NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs));
}

/**
 * @tc.name: ParseWebConfigXml_ValidLTPOConfig
 * @tc.desc: ParseWebConfigXml.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseWebConfigXml_ValidLTPOConfig, TestSize.Level0)
{
    std::string configFilePath = "valid_ltpo.xml";
    EXPECT_NO_THROW(NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs));
}

/**
 * @tc.name: GetPerfConfig_ShouldReturnEmptyVector_WhenSettingNameNotExist
 * @tc.desc: GetPerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, GetPerfConfig_ShouldReturnEmptyVector_WhenSettingNameNotExist, TestSize.Level0)
{
    std::string settingName = "NonExistentSetting";
    std::vector<FrameRateSetting> result = NWebConfigHelper::Instance().GetPerfConfig(settingName);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name: GetPerfConfig_ShouldReturnNonEmptyVector_WhenSettingNameExist
 * @tc.desc: GetPerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, GetPerfConfig_ShouldReturnNonEmptyVector_WhenSettingNameExist, TestSize.Level0)
{
    std::string settingName = "ExistentSetting";
    NWebConfigHelper::Instance().ltpoConfig_[settingName] = {FrameRateSetting(30)};
    std::vector<FrameRateSetting> result = NWebConfigHelper::Instance().GetPerfConfig(settingName);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result[0].frameRate, 30);
}

/**
 * @tc.name: ParsePerfConfig_ShouldReturnEmptyString_WhenConfigNotFound
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_ShouldReturnEmptyString_WhenConfigNotFound, TestSize.Level0)
{
    std::string configNodeName = "non_existent_config";
    std::string argsNodeName = "non_existent_args";
    std::string result = NWebConfigHelper::Instance().ParsePerfConfig(configNodeName, argsNodeName);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: ParsePerfConfig_ShouldReturnValue_WhenConfigFound
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_ShouldReturnValue_WhenConfigFound, TestSize.Level0)
{
    std::string configNodeName = "existent_config";
    std::string argsNodeName = "existent_args";
    std::string expectedValue = "expected_value";
    NWebConfigHelper::Instance().perfConfig_[configNodeName + "/" + argsNodeName] = expectedValue;
    std::string result = NWebConfigHelper::Instance().ParsePerfConfig(configNodeName, argsNodeName);
    EXPECT_EQ(result, expectedValue);
}

/**
 * @tc.name: ParsePerfConfig_NullNode
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_NullNode, TestSize.Level0)
{
    xmlNodePtr node = nullptr;
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_TRUE(NWebConfigHelper::Instance().perfConfig_.empty());
}

/**
 * @tc.name: ParsePerfConfig_CommentNode
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_CommentNode, TestSize.Level0)
{
    xmlNodePtr node = xmlNewNode(nullptr, "comment");
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_TRUE(NWebConfigHelper::Instance().perfConfig_.empty());
    xmlFreeNode(node);
}

/**
 * @tc.name: ParsePerfConfig_ValidNode
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_ValidNode, TestSize.Level0)
 {
    xmlNodePtr node = xmlNewNode(nullptr, "node");
    xmlNodePtr childNode = xmlNewNode(nullptr, "childNode");
    EXPECT_NE(node, nullptr);
    EXPECT_NE(childNode, nullptr);
    xmlNodeAddContent(childNode, "value");
    xmlAddChild(node, childNode);
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_EQ(hNWebConfigHelper::Instance().perfConfig_["node/childNode"], "value");
    xmlFreeNode(childNode);
    xmlFreeNode(node);
}

/**
 * @tc.name: ParsePerfConfig_InvalidChildNode
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_InvalidChildNode, TestSize.Level0)
 {
    xmlNodePtr node = xmlNewNode(nullptr, "node");
    xmlNodePtr childNode = xmlNewNode(nullptr, "comment");
    EXPECT_NE(node, nullptr);
    EXPECT_NE(childNode, nullptr);
    xmlAddChild(node, childNode);
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_TRUE(NWebConfigHelper::Instance().perfConfig_.empty());
    xmlFreeNode(childNode);
    xmlFreeNode(node);
}

/**
 * @tc.name: ParsePerfConfig_NullContent
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_NullContent, TestSize.Level0)
 {
    xmlNodePtr node = xmlNewNode(nullptr, "node");
    xmlNodePtr childNode = xmlNewNode(nullptr, "childNode");
    EXPECT_NE(node, nullptr);
    EXPECT_NE(childNode, nullptr);
    xmlAddChild(node, childNode);
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_TRUE(NWebConfigHelper::Instance().perfConfig_.empty());
    xmlFreeNode(childNode);
    xmlFreeNode(node);
 }

/**
 * @tc.name: ParseDeleteConfig_NullRootPtr
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_NullRootPtr, TestSize.Level0)
{
    xmlNodePtr rootPtr = nullptr;
    std::string result = NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
    EXPECT_EQ(result, "");
}

/**
 * @tc.name: ParseDeleteConfig_ValidNode
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_ValidNode, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    xmlChar *content = xmlNodeGetContent(childNodePtr);
    xmlNodeSetContent(childNodePtr, content);
    xmlFree(content);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

/**
 * @tc.name: ParseDeleteConfig_InvalidChildNode
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_InvalidChildNode, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, "invalid");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

/**
 * @tc.name: ParseDeleteConfig_NullContent
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_NullContent, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

/**
 * @tc.name: ParseDeleteConfig_NotFoundConfig
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_NotFoundConfig, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    xmlChar *content = xmlNodeGetContent(childNodePtr);
    xmlNodeSetContent(childNodePtr, content);
    xmlFree(content);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

/**
 * @tc.name: ParseDeleteConfig_EmptyParam
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_EmptyParam, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    xmlChar *content = xmlNodeGetContent(childNodePtr);
    xmlNodeSetContent(childNodePtr, content);
    xmlFree(content);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

} // NWebConfig
} // OHOS```