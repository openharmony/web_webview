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
#include <memory>
#include <unordered_map>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "parameters.h"

#define private public
#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "nweb_c_api.h"
#include "nweb_config_helper.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"


using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;

namespace OHOS {
namespace NWebConfig {

const auto XML_ATTR_NAME = "name";

class MockNWebConfigHelper : public NWebConfigHelper {
public:
    MOCK_METHOD1(ParseConfig, void(std::shared_ptr<NWebEngineInitArgsImpl> initArgs));
    MOCK_METHOD1(ParseNWebLTPOConfig, void(xmlNodePtr nodePtr));
    MOCK_METHOD2(ParseDeleteConfig, void(const xmlNodePtr &rootPtr,
        std::shared_ptr<NWebEngineInitArgsImpl> initArgs));
    MOCK_METHOD2(ParseWebConfigXml, void(const std::string& configFilePath,
        std::shared_ptr<NWebEngineInitArgsImpl> initArgs));
};

class NWebConfigHelperTest : public ::testing::Test {
protected:
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs = std::make_shared<NWebEngineInitArgsImpl>();
    xmlNodePtr rootElement;
    xmlNodePtr childNodePtr;
    std::unique_ptr<xmlChar[]> content;
    NWebConfigHelper &helper = NWebConfigHelper::Instance();
    void SetUp() override
    {
        rootElement = xmlNewNode(NULL, BAD_CAST "root");
        childNodePtr = xmlNewNode(NULL, BAD_CAST "child");
        int contentSize = 10;
        content = std::make_unique<xmlChar[]>(contentSize);
    }
    void TearDown() override {
        if (rootElement != nullptr) {
            xmlFreeNode(rootElement);
        }
        if (childNodePtr != nullptr) {
            xmlFreeNode(childNodePtr);
        }
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
    EXPECT_NE(initArgs, nullptr);
    NWebConfigHelper::Instance().ParseWebConfigXml(configFilePath, initArgs);
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
    xmlNodePtr node = xmlNewNode(nullptr, BAD_CAST "node");
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_FALSE(NWebConfigHelper::Instance().perfConfig_.empty());
}

/**
 * @tc.name: ParsePerfConfig_CommentNode
 * @tc.desc: ParsePerfConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParsePerfConfig_CommentNode, TestSize.Level0)
{
    xmlNodePtr node = xmlNewNode(nullptr, BAD_CAST "comment");
    NWebConfigHelper::Instance().ParsePerfConfig(node);
    EXPECT_FALSE(NWebConfigHelper::Instance().perfConfig_.empty());
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
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

/**
 * @tc.name: ParseDeleteConfig_ValidNode
 * @tc.desc: ParseDeleteConfig.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NWebConfigHelperTest, ParseDeleteConfig_ValidNode, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
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
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "invalid");
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
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
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
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
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
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    xmlChar *content = xmlNodeGetContent(childNodePtr);
    xmlNodeSetContent(childNodePtr, content);
    xmlFree(content);
    NWebConfigHelper::Instance().ParseDeleteConfig(rootPtr, initArgs);
}

/**
 * @tc.name  : safeGetPropAsInt_ShouldReturnDefaultValue_WhenPropNotExist
 * @tc.number: OHOS_NWEB_001
 * @tc.desc  : Test safeGetPropAsInt function when the property does not exist.
 */
HWTEST_F(NWebConfigHelperTest, safeGetPropAsInt_ShouldReturnDefaultValue_WhenPropNotExist,
    testing::ext::TestSize.Level0)
{
    int defaultValue = 10;
    int result = NWebConfigHelper::Instance().safeGetPropAsInt(rootElement,
      BAD_CAST "non_existent_prop", defaultValue);
    EXPECT_EQ(result, defaultValue);
}

/**
 * @tc.name  : safeGetPropAsInt_ShouldReturnPropValue_WhenPropExist
 * @tc.number: OHOS_NWEB_002
 * @tc.desc  : Test safeGetPropAsInt function when the property exists.
 */
HWTEST_F(NWebConfigHelperTest, safeGetPropAsInt_ShouldReturnPropValue_WhenPropExist, testing::ext::TestSize.Level0)
{
    xmlNewProp(rootElement, BAD_CAST "test_prop", BAD_CAST "20");
    int result = NWebConfigHelper::Instance().safeGetPropAsInt(rootElement, BAD_CAST "test_prop", 10);
    EXPECT_EQ(result, 20);
}

/**
 * @tc.name  : safeGetPropAsInt_ShouldReturnDefaultValue_WhenPropValueNotInt
 * @tc.number: OHOS_NWEB_003
 * @tc.desc  : Test safeGetPropAsInt function when the property value is not an integer.
 */
HWTEST_F(NWebConfigHelperTest, safeGetPropAsInt_ShouldReturnDefaultValue_WhenPropValueNotInt,
    testing::ext::TestSize.Level0)
{
    xmlNewProp(rootElement, BAD_CAST "test_prop", BAD_CAST "not_an_integer");
    int defaultValue = 0;
    int result = NWebConfigHelper::Instance().safeGetPropAsInt(rootElement, BAD_CAST "test_prop", defaultValue);
    EXPECT_EQ(result, defaultValue);
}

/**
 * @tc.name  : WriteConfigValueToSysPara_ShouldSetParameter_WhenConfigNameIsFlowBufferConfigMaxFdNumber
 * @tc.number: NWebConfigHelperTest_001
 * @tc.desc  : Test WriteConfigValueToSysPara method when configName is "flowBufferConfig/maxFdNumber"
 */
HWTEST_F(NWebConfigHelperTest,
    WriteConfigValueToSysPara_ShouldSetParameter_WhenConfigNameIsFlowBufferConfigMaxFdNumber, TestSize.Level0)
{
    std::string configName = "flowBufferConfig/maxFdNumber";
    std::string value = "1024";
    NWebConfigHelper::Instance().WriteConfigValueToSysPara(configName, value);
    std::string actualValue = OHOS::system::GetParameter("web.flowbuffer.maxfd", "");
    EXPECT_EQ(value, actualValue);
}

/**
 * @tc.name  : WriteConfigValueToSysPara_ShouldNotSetParameter_WhenConfigNameIsNotFlowBufferConfigMaxFdNumber
 * @tc.number: NWebConfigHelperTest_002
 * @tc.desc  : Test WriteConfigValueToSysPara method when configName is not "flowBufferConfig/maxFdNumber"
 */
HWTEST_F(NWebConfigHelperTest,
    WriteConfigValueToSysPara_ShouldNotSetParameter_WhenConfigNameIsNotFlowBufferConfigMaxFdNumber, TestSize.Level0)
{
    std::string configName = "someOtherConfig";
    std::string value = "1024";
    NWebConfigHelper::Instance().WriteConfigValueToSysPara(configName, value);
    std::string actualValue = OHOS::system::GetParameter("web.flowbuffer.maxfd", "");
    EXPECT_EQ(value, actualValue);
}

/**
 * @tc.name  : ParseNWebLTPOConfig_ShouldHandleInvalidNode_WhenNodeIsNull
 * @tc.number: NWebConfigHelperTest_001
 * @tc.desc  : Test when node is null then ParseNWebLTPOConfig should handle it correctly
 */
HWTEST_F(NWebConfigHelperTest, ParseNWebLTPOConfig_ShouldHandleInvalidNode_WhenNodeIsNull, TestSize.Level0)
{
    xmlNodePtr rootPtr = xmlNewNode(nullptr, BAD_CAST "valid");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    EXPECT_NE(rootPtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(rootPtr, childNodePtr);
    NWebConfigHelper::Instance().ParseNWebLTPOConfig(rootPtr);
}

/**
 * @tc.name  : ParseNWebLTPOConfig_ShouldHandleInvalidNode_WhenNodeIsComment
 * @tc.number: NWebConfigHelperTest_002
 * @tc.desc  : Test when node is a comment then ParseNWebLTPOConfig should handle it correctly
 */
HWTEST_F(NWebConfigHelperTest, ParseNWebLTPOConfig_ShouldHandleInvalidNode_WhenNodeIsComment, TestSize.Level0)
{
    xmlNodePtr nodePtr = xmlNewComment((xmlChar *)"This is a comment");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    EXPECT_NE(nodePtr, nullptr);
    EXPECT_NE(childNodePtr, nullptr);
    xmlAddChild(nodePtr, childNodePtr);
    NWebConfigHelper::Instance().ParseNWebLTPOConfig(nodePtr);
    xmlFreeNode(nodePtr);
}

/**
 * @tc.name  : ParseNWebLTPOConfig_ShouldHandleValidNode_WhenNameIsValid
 * @tc.number: NWebConfigHelperTest_004
 * @tc.desc  : Test when node and name are valid then ParseNWebLTPOConfig should handle it correctly
 */
HWTEST_F(NWebConfigHelperTest, ParseNWebLTPOConfig_ShouldHandleValidNode_WhenNameIsValid, TestSize.Level0)
{
    xmlNodePtr nodePtr = xmlNewNode(nullptr, BAD_CAST "testNode");
    EXPECT_NE(nodePtr, nullptr);
    xmlNewProp(nodePtr, BAD_CAST(XML_ATTR_NAME), BAD_CAST("validName"));
    NWebConfigHelper::Instance().ParseNWebLTPOConfig(nodePtr);
    xmlFreeNode(nodePtr);
}

/**
 * @tc.name  : ParseNWebLTPOConfig_ShouldHandleInvalidDynamicNode_WhenDynamicNodeIsComment
 * @tc.number: NWebConfigHelperTest_006
 * @tc.desc  : Test when dynamic node is a comment then ParseNWebLTPOConfig should handle it correctly
 */
HWTEST_F(NWebConfigHelperTest, ParseNWebLTPOConfig_ShouldHandleInvalidDynamicNode_WhenDynamicNodeIsComment,
    TestSize.Level0)
{
    xmlNodePtr nodePtr = xmlNewNode(nullptr, BAD_CAST "testNode");
    EXPECT_NE(nodePtr, nullptr);
    xmlNewProp(nodePtr, BAD_CAST(XML_ATTR_NAME), BAD_CAST("validName"));
    xmlNodePtr dynamicNodePtr = xmlNewComment((xmlChar *)"This is a comment");
    EXPECT_NE(dynamicNodePtr, nullptr);
    xmlAddChild(nodePtr, dynamicNodePtr);
    NWebConfigHelper::Instance().ParseNWebLTPOConfig(nodePtr);
    xmlFreeNode(nodePtr);
}

/**
 * @tc.name  : GetChildrenNode_ShouldReturnNode_WhenNodeExists
 * @tc.number: NWebConfigHelperTest_001
 * @tc.desc  : Test GetChildrenNode function when the node exists
 */
HWTEST_F(NWebConfigHelperTest, GetChildrenNode_ShouldReturnNode_WhenNodeExists, TestSize.Level0)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr childNode = nullptr;
    xmlAddChild(rootNode, childNode);
    xmlNodePtr resultNode = NWebConfigHelper::Instance().GetChildrenNode(rootNode, "child");
    EXPECT_EQ(resultNode, childNode);
    xmlFreeNode(rootNode);
    xmlFreeNode(childNode);
}

/**
 * @tc.name  : GetChildrenNode_ShouldReturnNull_WhenNodeNotExists
 * @tc.number: NWebConfigHelperTest_002
 * @tc.desc  : Test GetChildrenNode function when the node does not exist
 */
HWTEST_F(NWebConfigHelperTest, GetChildrenNode_ShouldReturnNull_WhenNodeNotExists, TestSize.Level0)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr childNode = nullptr;
    xmlAddChild(rootNode, childNode);
    xmlNodePtr resultNode = NWebConfigHelper::Instance().GetChildrenNode(rootNode, "non_existent_child");
    EXPECT_EQ(resultNode, nullptr);
    xmlFreeNode(rootNode);
    xmlFreeNode(childNode);
}

/**
 * @tc.name  : GetChildrenNode_ShouldReturnNull_WhenNodeNameIsEmpty
 * @tc.number: NWebConfigHelperTest_004
 * @tc.desc  : Test GetChildrenNode function when the node name is empty
 */
HWTEST_F(NWebConfigHelperTest, GetChildrenNode_ShouldReturnNull_WhenNodeNameIsEmpty, TestSize.Level0)
{
    xmlNodePtr rootNode = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr resultNode = NWebConfigHelper::Instance().GetChildrenNode(rootNode, "");
    EXPECT_EQ(resultNode, nullptr);
    xmlFreeNode(rootNode);
}

/**
 * @tc.name  : ReadConfig_ShouldHandleInvalidNode_WhenNodeIsNull
 * @tc.number: NWebConfigHelperTest_001
 * @tc.desc  : Test when node is null then ReadConfig should skip the node
 */
HWTEST_F(NWebConfigHelperTest, ReadConfig_ShouldHandleInvalidNode_WhenNodeIsNull, TestSize.Level0)
{
    rootElement->xmlChildrenNode = nullptr;
    NWebConfigHelper::Instance().ReadConfig(rootElement, initArgs);
    EXPECT_EQ(initArgs->GetArgsToAdd().size(), 0);
}

/**
 * @tc.name  : ReadConfig_ShouldHandleInvalidNode_WhenNodeIsComment
 * @tc.number: NWebConfigHelperTest_002
 * @tc.desc  : Test when node is a comment then ReadConfig should skip the node
 */
HWTEST_F(NWebConfigHelperTest, ReadConfig_ShouldHandleInvalidNode_WhenNodeIsComment, TestSize.Level0)
{
    rootElement->type = xmlElementType::XML_COMMENT_NODE;
    NWebConfigHelper::Instance().ReadConfig(rootElement, initArgs);
    EXPECT_EQ(initArgs->GetArgsToAdd().size(), 0);
}

/**
 * @tc.name  : ReadConfig_ShouldHandleInvalidNode_WhenChildNodeIsNull
 * @tc.number: NWebConfigHelperTest_003
 * @tc.desc  : Test when child node is null then ReadConfig should skip the child node
 */
HWTEST_F(NWebConfigHelperTest, ReadConfig_ShouldHandleInvalidNode_WhenChildNodeIsNull, TestSize.Level0)
{
    xmlNodePtr rootElement = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    rootElement->xmlChildrenNode = childNodePtr;
    childNodePtr->xmlChildrenNode = nullptr;
    NWebConfigHelper::Instance().ReadConfig(rootElement, initArgs);
    EXPECT_EQ(initArgs->GetArgsToAdd().size(), 0);
}

/**
 * @tc.name  : ReadConfig_ShouldHandleInvalidNode_WhenChildNodeIsComment
 * @tc.number: NWebConfigHelperTest_004
 * @tc.desc  : Test when child node is a comment then ReadConfig should skip the child node
 */
HWTEST_F(NWebConfigHelperTest, ReadConfig_ShouldHandleInvalidNode_WhenChildNodeIsComment, TestSize.Level0)
{
    xmlNodePtr rootElement = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    rootElement->xmlChildrenNode = childNodePtr;
    childNodePtr->type = xmlElementType::XML_COMMENT_NODE;
    NWebConfigHelper::Instance().ReadConfig(rootElement, initArgs);
    EXPECT_EQ(initArgs->GetArgsToAdd().size(), 0);
}

/**
 * @tc.name  : ReadConfig_ShouldHandleInvalidNode_WhenContentIsNull
 * @tc.number: NWebConfigHelperTest_005
 * @tc.desc  : Test when content is null then ReadConfig should skip the node
 */
HWTEST_F(NWebConfigHelperTest, ReadConfig_ShouldHandleInvalidNode_WhenContentIsNull, TestSize.Level0)
{
    xmlNodePtr rootElement = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");
    rootElement->xmlChildrenNode = childNodePtr;
    childNodePtr->xmlChildrenNode = nullptr;
    NWebConfigHelper::Instance().ReadConfig(rootElement, initArgs);
    EXPECT_EQ(initArgs->GetArgsToAdd().size(), 0);
}

/**
 * @tc.name  : ReadConfig_ShouldHandleValidNode_WhenContentIsValid
 * @tc.number: NWebConfigHelperTest_006
 * @tc.desc  : Test when content is valid then ReadConfig should add the param to initArgs
 */
HWTEST_F(NWebConfigHelperTest, ReadConfig_ShouldHandleValidNode_WhenContentIsValid, TestSize.Level0)
{
    xmlNodePtr rootElement = xmlNewNode(nullptr, BAD_CAST "root");
    xmlNodePtr childNodePtr = xmlNewNode(nullptr, BAD_CAST "child");rootElement->xmlChildrenNode = childNodePtr;
    rootElement->content = (xmlChar *)"valid_content";
    rootElement->type = XML_TEXT_NODE;
    childNodePtr->xmlChildrenNode = nullptr;
    NWebConfigHelper::Instance().ReadConfig(rootElement, initArgs);
    EXPECT_EQ(initArgs->GetArgsToAdd().size(), 0);
}

/**
 * @tc.name  : GetConfigPath_ShouldReturnSystemPath_WhenConfigPathIsInvalid
 * @tc.number: NWebConfigHelperTest_001
 * @tc.desc  : Test scenario for GetConfigPath when config path is invalid
 */
HWTEST_F(NWebConfigHelperTest, GetConfigPath_ShouldReturnSystemPath_WhenConfigPathIsInvalid, TestSize.Level0)
{
    std::string configFileName = "invalid_config_file";
    std::string expectedPath = "/system/" + configFileName;
    EXPECT_EQ(NWebConfigHelper::Instance().GetConfigPath(configFileName), expectedPath);
}

/**
 * @tc.name  : NWebConfigHelper_ReadConfigIfNeeded_ShouldParseConfig_WhenPerfConfigEmpty
 * @tc.number: NWebConfigHelper_Test_001
 * @tc.desc  : Test that ReadConfigIfNeeded parses config when perfConfig is empty
 */
HWTEST_F(NWebConfigHelperTest, NWebConfigHelper_ReadConfigIfNeeded_ShouldParseConfig_WhenPerfConfigEmpty,
    TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.clear();
    EXPECT_TRUE(NWebConfigHelper::Instance().perfConfig_.empty());
    NWebConfigHelper::Instance().ReadConfigIfNeeded();
}

/**
 * @tc.name  : NWebConfigHelper_ReadConfigIfNeeded_ShouldNotParseConfig_WhenPerfConfigNotEmpty
 * @tc.number: NWebConfigHelper_Test_002
 * @tc.desc  : Test that ReadConfigIfNeeded does not parse config when perfConfig is not empty
 */
HWTEST_F(NWebConfigHelperTest, NWebConfigHelper_ReadConfigIfNeeded_ShouldNotParseConfig_WhenPerfConfigNotEmpty,
    TestSize.Level0)
{
    NWebConfigHelper::Instance().perfConfig_.insert(std::make_pair("some_config", ""));
    NWebConfigHelper::Instance().ReadConfigIfNeeded();
    MockNWebConfigHelper *mock = new MockNWebConfigHelper();
    EXPECT_CALL(*mock, ParseConfig(initArgs)).Times(0);
    delete mock;
}
} // NWebConfig
} // OHOS```