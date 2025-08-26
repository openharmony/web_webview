/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <cstdint>
#include <dirent.h>
#include <dlfcn.h>
#include <memory>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>

#include "config_policy_utils.h"
#include "nweb_config_helper.h"
#include "nweb_log.h"
#include "parameters.h"

namespace {
const std::string WEB_CONFIG_PATH = "etc/web/web_config.xml";
const std::string INIT_CONFIG = "initConfig";
const std::string DELETE_CONFIG = "deleteArgsConfig";
const std::string PERFORMANCE_CONFIG = "performanceConfig";
// The config used in base/web/webview
const std::string BASE_WEB_CONFIG = "baseWebConfig";
const std::string WEB_ANIMATION_DYNAMIC_SETTING_CONFIG = "property_animation_dynamic_settings";
const auto XML_ATTR_NAME = "name";
const auto XML_ATTR_MIN = "min";
const auto XML_ATTR_MAX = "max";
const auto XML_ATTR_FPS = "preferred_fps";
const std::unordered_map<std::string_view, std::function<std::string(std::string&)>> configMap = {
    { "renderConfig/renderProcessCount",
        [](std::string& contentStr) { return std::string("--renderer-process-limit=") + contentStr; } },
    { "mediaConfig/backgroundMediaShouldSuspend",
        [](std::string& contentStr) {
            return contentStr == "false" ? std::string("--disable-background-media-suspend") : std::string();
        } },
    { "loadurlSocPerfConfig/loadurlSocPerfParam",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-loadurl-soc-perf") : std::string();
        } },
    { "mouseWheelSocPerfConfig/mouseWheelSocPerfParam",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-mousewheel-soc-perf") : std::string();
        } },
    { "touchEventConfig/touchEventShouldRegister",
        [](std::string& contentStr) {
            return contentStr == "false" ? std::string("--disable-touch-event-register") : std::string();
        } },
    { "settingConfig/enableWaitForUsername",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-wait-for-username") : std::string();
        } },
    { "settingConfig/enableMaxNumberOfSavedFrames",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-max-number-of-saved-frames") : std::string();
        } },
    { "settingConfig/enableNumRasterThreads",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-num-raster-threads") : std::string();
        } },
    { "settingConfig/enableSingleRenderProcess",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-single-render-process") : std::string();
        } },
    { "userAgentConfig/userAgentValue",
        [](std::string& contentStr) { return std::string("--ohos-user-agent-value=") + contentStr; } },
    { "settingConfig/enableSimpleBackendIsDefault",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-simple-backend-is-default") : std::string();
        } },
    { "settingConfig/enableEmbedMode",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-embed-mode") : std::string();
        } },
    { "settingConfig/enableWebViewImplForLargeScreen",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-web-view-impl-for-large-screen") : std::string();
        } },
    { "settingConfig/enableDeleteUnusedResourcesDelay",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-delete-unused-resources-delay") : std::string();
        } },
    { "settingConfig/enableSetHttpCacheMaxSize",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-set-http-cache-max-size") : std::string();
        } },
    { "settingConfig/enableCookieConfigPersistSession",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-cookie-config-persist-session") : std::string();
        } },
    { "settingConfig/enableDoubleTapForPlatform",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-double-tap-for-platform") : std::string();
        } },
    { "settingConfig/enableIgnoreLockdownMode",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-Ignore-lockdown-mode") : std::string();
        } },
    { "settingConfig/enablePrinting",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-printing") : std::string();
        } },
    { "settingConfig/enableHttpCacheSimple",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-http-cache-simple") : std::string();
        } },
    { "settingConfig/enableCalcTabletMode",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-enable-calc-tablet-mode") : std::string();
        } },
    { "settingConfig/disableMobileStyleSheet",
        [](std::string& contentStr) {
            return contentStr == "true" ? std::string("--ohos-disable-mobile-style-sheet") : std::string();
        } },
    { "outOfProcessGPUConfig/enableOopGpu",
        [](std::string& contentStr) { return contentStr == "true" ? std::string("--in-process-gpu") : std::string(); } }
};
} // namespace

namespace OHOS::NWeb {
NWebConfigHelper &NWebConfigHelper::Instance()
{
    static NWebConfigHelper helper;
    return helper;
}

void NWebConfigHelper::ReadConfigIfNeeded()
{
    if (perfConfig_.empty()) {
        std::shared_ptr<NWebEngineInitArgsImpl> initArgs = std::make_shared<NWebEngineInitArgsImpl>();
        NWebConfigHelper::Instance().ParseConfig(initArgs);
    }
}

std::string NWebConfigHelper::GetConfigPath(const std::string &configFileName)
{
    char buf[PATH_MAX + 1];
    char *configPath = GetOneCfgFile(configFileName.c_str(), buf, PATH_MAX + 1);
    char tmpPath[PATH_MAX + 1] = { 0 };
    if (!configPath || strlen(configPath) == 0 || strlen(configPath) > PATH_MAX || !realpath(configPath, tmpPath)) {
        WVLOG_I("can not get customization config file");
        return "/system/" + configFileName;
    }
    return std::string(tmpPath);
}

void NWebConfigHelper::ReadConfig(const xmlNodePtr &rootPtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs)
{
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (curNodePtr->name == nullptr || curNodePtr->type == XML_COMMENT_NODE) {
            WVLOG_E("invalid node!");
            continue;
        }
        std::string nodeName = reinterpret_cast<const char *>(curNodePtr->name);
        for (xmlNodePtr curChildNodePtr = curNodePtr->xmlChildrenNode; curChildNodePtr != nullptr;
            curChildNodePtr = curChildNodePtr->next) {
            if (curChildNodePtr->name == nullptr || curChildNodePtr->type == XML_COMMENT_NODE) {
                WVLOG_E("invalid node!");
                continue;
            }
            std::string childNodeName = reinterpret_cast<const char *>(curChildNodePtr->name);
            xmlChar *content = xmlNodeGetContent(curChildNodePtr);
            if (content == nullptr) {
                WVLOG_E("read xml node error: nodeName:(%{public}s)", curChildNodePtr->name);
                continue;
            }
            std::string contentStr = reinterpret_cast<const char *>(content);
            xmlFree(content);
            auto it = configMap.find(nodeName + "/" + childNodeName);
            if (it == configMap.end()) {
                WVLOG_W("not found for web_config: %{public}s/%{public}s", nodeName.c_str(), childNodeName.c_str());
                continue;
            }
            std::string param = it->second(contentStr);
            if (!param.empty()) {
                initArgs->AddArg(param);
            }
        }
    }
}

xmlNodePtr NWebConfigHelper::GetChildrenNode(xmlNodePtr NodePtr, const std::string &childrenNodeName)
{
    WVLOG_D("GetChildrenNode:(%{public}s)", childrenNodeName.c_str());
    for (xmlNodePtr curNodePtr = NodePtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (curNodePtr->name == nullptr || curNodePtr->type == XML_COMMENT_NODE) {
            WVLOG_E("invalid node!");
            continue;
        }
        if (!xmlStrcmp(curNodePtr->name, reinterpret_cast<const xmlChar*>(childrenNodeName.c_str()))) {
            return curNodePtr;
        }
    }
    return nullptr;
}

void NWebConfigHelper::ParseConfig(std::shared_ptr<NWebEngineInitArgsImpl> initArgs)
{
    CfgFiles* cfgFiles = GetCfgFiles(WEB_CONFIG_PATH.c_str());
    std::string defaultConfigPath = "/system/" + WEB_CONFIG_PATH;
    if (cfgFiles == nullptr) {
        WVLOG_E("Not found webConfigxml,read system config");
        ParseWebConfigXml(defaultConfigPath, initArgs);
        return;
    }

    // When i is 0 ,it means /system/ + WEB_CONFIG_PATH, ignore
    for (int32_t i = 1; i < MAX_CFG_POLICY_DIRS_CNT; i++) {
        auto cfgFilePath = cfgFiles->paths[i];
        if (!cfgFilePath || *(cfgFilePath) == '\0') {
            break;
        }
        WVLOG_D("web config file path:%{public}s", cfgFilePath);
        if (!cfgFilePath || strlen(cfgFilePath) == 0 || strlen(cfgFilePath) > PATH_MAX) {
            WVLOG_W("can not get customization config file");
            ParseWebConfigXml(defaultConfigPath, initArgs);
            continue;
        }
        ParseWebConfigXml(cfgFiles->paths[i], initArgs);
    }
    FreeCfgFiles(cfgFiles);
}

void NWebConfigHelper::ParseWebConfigXml(const std::string& configFilePath,
    std::shared_ptr<NWebEngineInitArgsImpl> initArgs)
{
    xmlDocPtr docPtr = xmlReadFile(configFilePath.c_str(), nullptr, XML_PARSE_NOBLANKS);
    if (docPtr == nullptr) {
        WVLOG_E("load xml error!");
        return;
    }

    xmlNodePtr rootPtr = xmlDocGetRootElement(docPtr);
    if (rootPtr == nullptr || rootPtr->name == nullptr ||
        xmlStrcmp(rootPtr->name, reinterpret_cast<const xmlChar *>("WEB"))) {
        WVLOG_E("get root element failed!");
        xmlFreeDoc(docPtr);
        return;
    }

    xmlNodePtr initNodePtr = GetChildrenNode(rootPtr, INIT_CONFIG);
    if (initNodePtr != nullptr) {
        WVLOG_D("read config from init node");
        ReadConfig(initNodePtr, initArgs);
    } else {
        WVLOG_D("read config from root node");
        ReadConfig(rootPtr, initArgs);
    }

    xmlNodePtr deleteNodePtr = GetChildrenNode(rootPtr, DELETE_CONFIG);
    if (deleteNodePtr != nullptr) {
        WVLOG_D("read config from delete node");
        ParseDeleteConfig(deleteNodePtr, initArgs);
    }

    if (perfConfig_.empty()) {
        xmlNodePtr perfNodePtr = GetChildrenNode(rootPtr, PERFORMANCE_CONFIG);
        if (perfNodePtr != nullptr) {
            ParsePerfConfig(perfNodePtr);
        }
        xmlNodePtr adapterNodePtr = GetChildrenNode(rootPtr, BASE_WEB_CONFIG);
        if (adapterNodePtr != nullptr) {
            ParsePerfConfig(adapterNodePtr);
        }
    }

    if (ltpoConfig_.empty()) {
        xmlNodePtr ltpoConfigNodePtr = GetChildrenNode(rootPtr, WEB_ANIMATION_DYNAMIC_SETTING_CONFIG);
        if (ltpoConfigNodePtr != nullptr) {
            ParseNWebLTPOConfig(ltpoConfigNodePtr);
        }
    }
    xmlFreeDoc(docPtr);
}

void NWebConfigHelper::ParseNWebLTPOConfig(xmlNodePtr nodePtr)
{
    for (xmlNodePtr curNodePtr = nodePtr->xmlChildrenNode; curNodePtr; curNodePtr = curNodePtr->next) {
        if (curNodePtr->name == nullptr || curNodePtr->type == XML_COMMENT_NODE) {
            WVLOG_E("invalid node!");
            continue;
        }
        char* namePtr = (char *)xmlGetProp(curNodePtr, BAD_CAST(XML_ATTR_NAME));
        if (!namePtr) {
            WVLOG_E("invalid name!");
            continue;
        }
        std::string settingName(namePtr);
        xmlFree(namePtr);
        std::vector<FrameRateSetting> frameRateSetting;
        for (xmlNodePtr curDynamicNodePtr = curNodePtr->xmlChildrenNode; curDynamicNodePtr;
            curDynamicNodePtr = curDynamicNodePtr->next) {
            if (curDynamicNodePtr->name == nullptr || curDynamicNodePtr->type == XML_COMMENT_NODE) {
                WVLOG_E("invalid node!");
                continue;
            }
            FrameRateSetting setting;
            int defaultValue = 0;
            setting.min_ = safeGetPropAsInt(curDynamicNodePtr, BAD_CAST(XML_ATTR_MIN), defaultValue);
            setting.max_ = safeGetPropAsInt(curDynamicNodePtr, BAD_CAST(XML_ATTR_MAX), defaultValue);
            setting.preferredFrameRate_ = safeGetPropAsInt(curDynamicNodePtr, BAD_CAST(XML_ATTR_FPS), defaultValue);
            if ((setting.max_ >= 0 && setting.min_ >= setting.max_) || setting.preferredFrameRate_ <= 0) {
                continue;
            }
            frameRateSetting.emplace_back(setting);
        }
        ltpoConfig_[settingName] = frameRateSetting;
    }
}

std::vector<FrameRateSetting> NWebConfigHelper::GetPerfConfig(const std::string& settingName)
{
    if (ltpoConfig_.find(settingName) == ltpoConfig_.end()) {
        WVLOG_E("%{public}s is not exist", settingName.c_str()) ;
        return {};
    }
    return ltpoConfig_[settingName];
}

void NWebConfigHelper::ParsePerfConfig(xmlNodePtr NodePtr)
{
    WVLOG_D("read performance config");
    for (xmlNodePtr curNodePtr = NodePtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (curNodePtr->name == nullptr || curNodePtr->type == XML_COMMENT_NODE) {
            WVLOG_E("invalid node!");
            continue;
        }
        std::string nodeName = reinterpret_cast<const char*>(curNodePtr->name);
        for (xmlNodePtr curChildNodePtr = curNodePtr->xmlChildrenNode; curChildNodePtr != nullptr;
             curChildNodePtr = curChildNodePtr->next) {
            if (curChildNodePtr->name == nullptr || curChildNodePtr->type == XML_COMMENT_NODE) {
                WVLOG_E("invalid node!");
                continue;
            }
            std::string childNodeName = reinterpret_cast<const char*>(curChildNodePtr->name);
            xmlChar* content = xmlNodeGetContent(curChildNodePtr);
            if (content == nullptr) {
                WVLOG_E("read xml node error: nodeName:(%{public}s)", childNodeName.c_str());
                continue;
            }
            std::string contentStr = reinterpret_cast<const char*>(content);
            xmlFree(content);
            perfConfig_.emplace(nodeName + "/" + childNodeName, contentStr);
            WriteConfigValueToSysPara(nodeName + "/" + childNodeName, contentStr);
        }
    }
}

std::string NWebConfigHelper::ParsePerfConfig(const std::string &configNodeName, const std::string &argsNodeName)
{
    auto it = perfConfig_.find(configNodeName + "/" + argsNodeName);
    if (it == perfConfig_.end()) {
        WVLOG_W("not found perf config for web_config: %{public}s/%{public}s", configNodeName.c_str(),
                argsNodeName.c_str());
        return "";
    }
    WVLOG_D("find performance config %{public}s/%{public}s, value is %{public}s.", configNodeName.c_str(),
        argsNodeName.c_str(), it->second.c_str());
    return it->second;
}

void NWebConfigHelper::WriteConfigValueToSysPara(const std::string &configName, const std::string &value)
{
    if (configName == "flowBufferConfig/maxFdNumber") {
        OHOS::system::SetParameter("web.flowbuffer.maxfd", value);
    }
}

void NWebConfigHelper::ParseDeleteConfig(const xmlNodePtr &rootPtr, std::shared_ptr<NWebEngineInitArgsImpl> initArgs)
{
    for (xmlNodePtr curNodePtr = rootPtr->xmlChildrenNode; curNodePtr != nullptr; curNodePtr = curNodePtr->next) {
        if (curNodePtr->name == nullptr || curNodePtr->type == XML_COMMENT_NODE) {
            WVLOG_E("invalid node!");
            continue;
        }
        std::string nodeName = reinterpret_cast<const char *>(curNodePtr->name);
        for (xmlNodePtr curChildNodePtr = curNodePtr->xmlChildrenNode; curChildNodePtr != nullptr;
            curChildNodePtr = curChildNodePtr->next) {
            if (curChildNodePtr->name == nullptr || curChildNodePtr->type == XML_COMMENT_NODE) {
                WVLOG_E("invalid node!");
                continue;
            }
            std::string childNodeName = reinterpret_cast<const char *>(curChildNodePtr->name);
            xmlChar *content = xmlNodeGetContent(curChildNodePtr);
            if (content == nullptr) {
                WVLOG_E("read xml node error: nodeName:(%{public}s)", curChildNodePtr->name);
                continue;
            }
            std::string contentStr = reinterpret_cast<const char *>(content);
            xmlFree(content);
            auto it = configMap.find(nodeName + "/" + childNodeName);
            if (it == configMap.end()) {
                WVLOG_W("not found for web_config: %{public}s/%{public}s", nodeName.c_str(), childNodeName.c_str());
                continue;
            }
            std::string param = it->second(contentStr);
            if (!param.empty()) {
                initArgs->AddDeleteArg(param);
            }
        }
    }
}

int NWebConfigHelper::safeGetPropAsInt(xmlNode* node, const xmlChar* propName, int defaultValue)
{
    xmlChar* propValue = xmlGetProp(node, propName);
    if (propValue == nullptr) {
        return defaultValue;
    }
    int value = atoi((const char*)propValue);
    xmlFree(propValue);
    return value;
}

} // namespace OHOS::NWeb
