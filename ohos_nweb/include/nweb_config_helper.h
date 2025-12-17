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

#include "bundle_mgr_interface.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "iservice_registry.h"
#include "nweb_init_params.h"
#include "nweb_helper.h"
#include "system_ability_definition.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebConfigHelper {
public:
    static NWebConfigHelper &Instance();
    ~NWebConfigHelper() = default;

    void ParseConfig(std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    void ReadConfigIfNeeded();
    std::string ParsePerfConfig(const std::string &configNodeName, const std::string &argsNodeName);
    std::vector<FrameRateSetting> GetPerfConfig(const std::string& settingName);
    bool IsLTPODynamicApp(const std::string& bundleName);
    int32_t GetLTPOStrategy();
    int safeGetPropAsInt(xmlNode* node, const xmlChar* propName, int defaultValue);
    std::string GetBundleName();
    void SetBundleName(const std::string& bundleName);
    bool GetNWebDvsyncSwitch();
    bool IsWebPlayGroundEnable();
    const std::string& GetWebPlayGroundInitArg();
    const std::string& GetWebPlayGroundHapPath();
    std::string GetConfigPath(const std::string &configFileName);
    std::vector<std::string> GetConfigPathsInPriorityOrder(const std::string& relativePath);

private:
    NWebConfigHelper();
    void ParseNWebLTPOConfig(xmlNodePtr nodePtr);
    void ReadConfig(const xmlNodePtr &rootPtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    xmlNodePtr GetChildrenNode(xmlNodePtr NodePtr, const std::string &childrenNodeName);
    void ParsePerfConfig(xmlNodePtr NodePtr);
    void ParseWebConfigXml(const std::string &configFilePath, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    void WriteConfigValueToSysPara(const std::string &configName, const std::string &value);
    void ParseDeleteConfig(const xmlNodePtr &rootPtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    void ParseNWebLTPOApp(xmlNodePtr nodePtr);
    void ParseNWebLTPOStrategy(xmlNodePtr nodePtr);
    void ParseNWebDvsync(xmlNodePtr nodePtr);
    void ParseNWebDvsyncSwitch(xmlNodePtr nodePtr);
    void ParseWindowOrientationConfig(xmlNodePtr nodePtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs);
    bool IsDeveloperModeEnabled();
    bool IsPerfConfigEmpty();

    std::map<std::string, std::string> perfConfig_;
    std::map<std::string, std::vector<FrameRateSetting>> ltpoConfig_;
    std::set<std::string> ltpoAllowedApps_ {};
    int32_t ltpoStrategy_ {0};
    std::string bundleName_;
    bool dvsyncSwitch_;
    mutable std::mutex lock_;

    // add for web play ground
    const std::string PLAYGROUND = "enableArkWebPlayGround";
    const std::string NULL_STR = "";
    const std::string SINGLE_PROCESS = "--single-process";
    const std::string PLAY_GROUND_HAP_PATH = "/data/storage/el1/bundle/entry/resources/resfile/nweb.hap";
    bool web_play_ground_enabled_ = false;
};
} // namespace OHOS

#endif // NWEB_CONFIG_HELPER_H