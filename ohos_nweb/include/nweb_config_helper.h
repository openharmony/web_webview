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

#ifndef NWEB_CONFIG_HELPER_H
#define NWEB_CONFIG_HELPER_H

#include "libxml/parser.h"
#include "libxml/tree.h"
#include "nweb_init_params.h"
#include "nweb_helper.h"
namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebConfigHelper {
public:
    static NWebConfigHelper &Instance();
    ~NWebConfigHelper() = default;

    void ParseConfig(std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    void ReadConfigIfNeeded();
    std::string ParsePerfConfig(const std::string &configNodeName, const std::string &argsNodeName);
    std::vector<FrameRateSetting> GetPerfConfig(const std::string& settingName);

private:
    NWebConfigHelper() = default;
    std::string GetConfigPath(const std::string &configFileName);
    void ParseNWebLTPOConfig(xmlNodePtr nodePtr);
    void ReadConfig(const xmlNodePtr &rootPtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    xmlNodePtr GetChildrenNode(xmlNodePtr NodePtr, const std::string &childrenNodeName);
    void ParsePerfConfig(xmlNodePtr NodePtr);
    void ParseWebConfigXml(const std::string &configFilePath, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    void WriteConfigValueToSysPara(const std::string &configName, const std::string &value);
    void ParseDeleteConfig(const xmlNodePtr &rootPtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    std::map<std::string, std::string> perfConfig_;
    std::map<std::string, std::vector<FrameRateSetting>> ltpoConfig_;
};
} // namespace OHOS

#endif // NWEB_CONFIG_HELPER_H
