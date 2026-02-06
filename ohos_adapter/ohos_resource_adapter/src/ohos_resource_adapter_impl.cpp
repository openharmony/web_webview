/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "ohos_resource_adapter_impl.h"

#include <ctime>
#include <securec.h>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <fcntl.h>
#include <fstream>
#include <json/json.h>
#include <sys/stat.h>

#include "arkweb_utils.h"
#include "application_context.h"
#include "bundle_mgr_proxy.h"
#include "extractor.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "locale_config.h"
#include "nweb_config_helper.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "parameter.h"
#include "parameters.h"
#include "system_ability_definition.h"

using namespace OHOS::AbilityBase;

namespace {
const std::string NWEB_HAP_PATH = "/system/app/com.ohos.nweb/NWeb.hap";
const std::string NWEB_HAP_PATH_1 = "/system/app/NWeb/NWeb.hap";
const std::string ARKWEBCORE_HAP_SANDBOX_PATH = "/data/storage/el1/bundle/nweb/entry.hap";
const std::string PERSIST_ARKWEBCORE_INSTALL_PATH = "persist.arkwebcore.install_path";
const std::string NWEB_HAP_PATH_MODULE_UPDATE = "/module_update/ArkWebCore/app/com.ohos.nweb/NWeb.hap";
const std::string HAP_REAL_PATH_PREFIX = "/data/app/el1/bundle/public/";
const std::string HAP_SANDBOX_PATH_PREFIX = "/data/storage/el1/bundle/nweb/";

const std::string NWEB_BUNDLE_NAME = "com.ohos.nweb";
const std::string NWEB_PACKAGE = "entry";
const std::string RAWFILE_PREFIX = "resources/rawfile/";
const std::string BUNDLE_NAME_PREFIX = "bundleName:";
const std::string MODULE_NAME_PREFIX = "moduleName:";
constexpr uint32_t TM_YEAR_BITS = 9;
constexpr uint32_t TM_MON_BITS = 5;
constexpr uint32_t TM_MIN_BITS = 5;
constexpr uint32_t TM_HOUR_BITS = 11;
constexpr uint32_t START_YEAR = 1900;
} // namespace

namespace OHOS::NWeb {
namespace {
std::shared_ptr<Global::Resource::ResourceManager> GetResourceMgr(
    const std::string& bundleName, const std::string& moduleName)
{
    std::shared_ptr<AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::ApplicationContext::GetApplicationContext();
    if (!context) {
        WVLOG_E("Failed to get application context.");
        return nullptr;
    }

    if (bundleName.empty() || moduleName.empty()) {
        return context->GetResourceManager();
    }
    auto moduleContext = context->CreateModuleContext(bundleName, moduleName);
    if (!moduleContext) {
        WVLOG_E("Failed to crate module context, bundleName: %{public}s, moduleName: %{public}s.",
            bundleName.c_str(), moduleName.c_str());
        return nullptr;
    }
    return moduleContext->GetResourceManager();
}

bool ParseRawFile(const std::string& rawFile,
    std::string& bundleName, std::string& moduleName, std::string& fileName)
{
    if (rawFile.substr(0, RAWFILE_PREFIX.size()) != RAWFILE_PREFIX) {
        WVLOG_D("ParseRawFile failed, rawfile: %{public}s", rawFile.c_str());
        return false;
    }

    std::string subStr = rawFile.substr(RAWFILE_PREFIX.size());
    if (subStr.substr(0, BUNDLE_NAME_PREFIX.size()) != BUNDLE_NAME_PREFIX) {
        return false;
    }
    subStr = subStr.substr(BUNDLE_NAME_PREFIX.size());
    size_t pos = subStr.find('/');
    if (pos == std::string::npos) {
        WVLOG_D("ParseRawFile bundleName failed, rawfile: %{public}s", rawFile.c_str());
        return false;
    }
    bundleName = subStr.substr(0, pos);

    subStr = subStr.substr(pos + 1);
    if (subStr.substr(0, MODULE_NAME_PREFIX.size()) != MODULE_NAME_PREFIX) {
        return false;
    }
    subStr = subStr.substr(MODULE_NAME_PREFIX.size());
    pos = subStr.find('/');
    if (pos == std::string::npos) {
        WVLOG_D("ParseRawFile moduleName failed, rawfile: %{public}s", rawFile.c_str());
        return false;
    }
    moduleName = subStr.substr(0, pos);

    fileName = subStr.substr(pos + 1);
    if (fileName.empty()) {
        WVLOG_D("ParseRawFile fileName failed, rawfile: %{public}s", rawFile.c_str());
        return false;
    }
    return true;
}


std::string GetArkWebHapPath(const std::string& arkWebCoreHapPathOverride,
                             std::vector<std::pair<std::string, int>>& errorMessage)
{
    std::string prefixPath = WEBVIEW_SANDBOX_PATH;
    struct stat statbuf;
    if (fstatat(AT_FDCWD, arkWebCoreHapPathOverride.c_str(), &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
        std::string sandboxPath = OhosResourceAdapterImpl::ConvertToSandboxPath(arkWebCoreHapPathOverride, prefixPath);
        if (fstatat(AT_FDCWD, sandboxPath.c_str(), &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
            WVLOG_D("eixt HAP_arkWebCoreHapPathOverride");
            return sandboxPath;
        }
    }
    errorMessage.emplace_back("access arkWebCoreHapPathOverride path failed", errno);

    const std::string& webPlayGround = NWebConfigHelper::Instance().GetWebPlayGroundHapPath();
    if (!webPlayGround.empty()) {
        if (fstatat(AT_FDCWD, webPlayGround.c_str(), &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
            return webPlayGround;
        }
        WVLOG_E("GetWebPlayGroundHapPath file not found, %{public}s", webPlayGround.c_str());
    }

    std::string installPath = OHOS::ArkWeb::GetArkwebInstallPath();
    if (fstatat(AT_FDCWD, installPath.c_str(), &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
        WVLOG_D("exit install_path,%{public}s", installPath.c_str());
        return installPath;
    }
    errorMessage.emplace_back("access nweb install path failed", errno);

    if (fstatat(AT_FDCWD, WEBVIEW_SANDBOX_HAP_PATH, &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
        WVLOG_D("exit WEBVIEW_SANDBOX_HAP_PATH");
        return WEBVIEW_SANDBOX_HAP_PATH;
    }
    errorMessage.emplace_back("access arkwebcore hap sandbox path failed", errno);
    if (fstatat(AT_FDCWD, WEBVIEW_APP_HAP_PATH2, &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
        WVLOG_D("exit WEBVIEW_APP_HAP_PATH2");
        return WEBVIEW_APP_HAP_PATH2;
    }
    errorMessage.emplace_back("access ohos nweb hap path failed", errno);
    if (fstatat(AT_FDCWD, WEBVIEW_APP_HAP_PATH, &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
        WVLOG_D("exit WEBVIEW_APP_HAP_PATH");
        return WEBVIEW_APP_HAP_PATH;
    }
    errorMessage.emplace_back("access nweb hap path failed", errno);
    if (fstatat(AT_FDCWD, WEBVIEW_HAP_PATH, &statbuf, AT_SYMLINK_NOFOLLOW) == 0) {
        WVLOG_D("exit WEBVIEW_HAP_PATH");
        return WEBVIEW_HAP_PATH;
    }
    errorMessage.emplace_back("access nweb hap module update path failed", errno);
    return "";
}

} // namespace

OhosFileMapperImpl::OhosFileMapperImpl(std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap,
    const std::shared_ptr<Extractor>& extractor): extractor_(extractor), fileMap_(std::move(fileMap))
{
}

int32_t OhosFileMapperImpl::GetFd()
{
    return -1;
}

int32_t OhosFileMapperImpl::GetOffset()
{
    return fileMap_ ? fileMap_->GetOffset(): -1;
}

std::string OhosFileMapperImpl::GetFileName()
{
    return fileMap_ ? fileMap_->GetFileName(): "";
}

bool OhosFileMapperImpl::IsCompressed()
{
    return fileMap_ ? fileMap_->IsCompressed(): false;
}

void* OhosFileMapperImpl::GetDataPtr()
{
    return fileMap_ ? fileMap_->GetDataPtr(): nullptr;
}

size_t OhosFileMapperImpl::GetDataLen()
{
    return fileMap_ ? fileMap_->GetDataLen(): 0;
}

bool OhosFileMapperImpl::UnzipData(uint8_t** dest, size_t& len)
{
    if (extractor_ && IsCompressed()) {
        std::unique_ptr<uint8_t[]> data;
        bool result = extractor_->UnzipData(std::move(fileMap_), data, len);
        if (result) {
            *dest = data.release();
        }
        return result;
    }
    return false;
}

std::string OhosResourceAdapterImpl::arkWebCoreHapPathOverride_ = "";
std::string OhosResourceAdapterImpl::ConvertToSandboxPath(const std::string& installPath, const std::string& prefixPath)
{
    if (installPath.empty()) {
        return "";
    }
    size_t result = installPath.find(HAP_REAL_PATH_PREFIX);
    if (result != std::string::npos) {
        size_t pos = installPath.find_last_of('/');
        if (pos != std::string::npos && pos != installPath.size() - 1) {
            return prefixPath + installPath.substr(pos + 1);
        }
    }
    return installPath;
}

OhosResourceAdapterImpl::OhosResourceAdapterImpl(const std::string& hapPath)
{
    Init(hapPath);
}

void OhosResourceAdapterImpl::Init(const std::string& hapPath)
{
    bool newCreate = false;
    std::vector<std::pair<std::string, int>> errorMessage;
    std::string arkWebHapPath = GetArkWebHapPath(arkWebCoreHapPathOverride_, errorMessage);
    if (!arkWebHapPath.empty()) {
        sysExtractor_ = ExtractorUtil::GetExtractor(arkWebHapPath, newCreate);
        if (!sysExtractor_) {
            WVLOG_E("RuntimeExtractor create failed for %{public}s", arkWebHapPath.c_str());
        }
    }
    for (const auto& err : errorMessage) {
        WVLOG_D("%{public}s, errno(%{public}d): %{public}s", err.first.c_str(), err.second, strerror(err.second));
    }
    if (hapPath.empty()) {
        return;
    }
    std::string loadPath = ExtractorUtil::GetLoadFilePath(hapPath);
    extractor_ = ExtractorUtil::GetExtractor(loadPath, newCreate);
    if (!extractor_) {
        WVLOG_E("RuntimeExtractor create failed for %{public}s", hapPath.c_str());
    }
}

bool OhosResourceAdapterImpl::GetRawFileData(const std::string& rawFile, size_t& len,
    uint8_t** dest, bool isSys)
{
    std::unique_ptr<uint8_t[]> data;
    bool result;
    if (isSys) {
        result =  GetRawFileData(sysExtractor_, rawFile, len, data);
        if (result) {
            *dest = data.release();
        }
        return result;
    }
    std::string bundleName;
    std::string moduleName;
    std::string fileName;
    if (ParseRawFile(rawFile, bundleName, moduleName, fileName)) {
        auto resourceManager = GetResourceMgr(bundleName, moduleName);
        if (!resourceManager) {
            result = GetRawFileData(extractor_, rawFile, len, data);
            if (result) {
                *dest = data.release();
            }
            return result;
        }
        auto state = resourceManager->GetRawFileFromHap(fileName, len, data);
        if (state != Global::Resource::SUCCESS) {
            WVLOG_E("GetRawFileFromHap failed, state: %{public}d, fileName: %{public}s", state, fileName.c_str());
            result = GetRawFileData(extractor_, rawFile, len, data);
            if (result) {
                *dest = data.release();
            }
            return result;
        }
        *dest = data.release();
        return true;
    }

    result = GetRawFileData(extractor_, rawFile, len, data);
    if (result) {
        *dest = data.release();
    }
    return result;
}

bool OhosResourceAdapterImpl::GetResourceString(const std::string& bundleName,
    const std::string& moduleName, const int32_t resId, std::string& result)
{
    auto resourceManager = GetResourceMgr(bundleName, moduleName);
    if (!resourceManager) {
        return false;
    }
    if (resourceManager->GetStringById(resId, result) == Global::Resource::SUCCESS) {
        return true;
    }
    return false;
}

std::shared_ptr<OhosFileMapper> OhosResourceAdapterImpl::GetRawFileMapper(const std::string& rawFile,
    bool isSys)
{
    return GetRawFileMapper(isSys? sysExtractor_: extractor_, rawFile);
}

bool OhosResourceAdapterImpl::IsRawFileExist(const std::string& rawFile, bool isSys)
{
    return HasEntry(isSys? sysExtractor_: extractor_, rawFile);
}

bool OhosResourceAdapterImpl::GetRawFileLastModTime(const std::string& rawFile,
    uint16_t& date, uint16_t& time, bool isSys)
{
    FileInfo info;
    if (GetFileInfo(isSys? sysExtractor_: extractor_, rawFile, info)) {
        date = info.lastModDate;
        time = info.lastModTime;
        return true;
    }
    return false;
}

bool OhosResourceAdapterImpl::GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys)
{
    FileInfo info;
    if (GetFileInfo(isSys? sysExtractor_: extractor_, rawFile, info)) {
        uint16_t modifiedDate = info.lastModDate;
        uint16_t modifiedTime = info.lastModTime;
        struct tm newTime;
        newTime.tm_year = ((modifiedDate >> TM_YEAR_BITS) & 0x7f) + START_YEAR;
        newTime.tm_mon = (modifiedDate >> TM_MON_BITS) & 0xf;
        newTime.tm_mday = modifiedDate & 0x1f;
        newTime.tm_hour = (modifiedTime >> TM_HOUR_BITS) & 0x1f;
        newTime.tm_min = (modifiedTime >> TM_MIN_BITS) & 0x2f;
        newTime.tm_sec = (modifiedTime << 1) & 0x1f;
        newTime.tm_isdst = 0;
        time = mktime(&newTime);
        return true;
    }
    return false;
}

// static
bool OhosResourceAdapterImpl::HasEntry(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
    const std::string& rawFile)
{
    if (!manager) {
        return false;
    }
    return manager->HasEntry(rawFile);
}

bool OhosResourceAdapterImpl::GetFileInfo(const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
    const std::string& rawFile, OHOS::AbilityBase::FileInfo& info)
{
    if (!manager) {
        return false;
    }
    return manager->GetFileInfo(rawFile, info);
}

std::string OhosResourceAdapterImpl::GetModuleName(const char *configStr, size_t len)
{
    if (configStr == nullptr) {
        return std::string();
    }
    std::string config(configStr, len);
    static const char *key = "\"moduleName\"";
    auto idx = config.find(key);
    if (idx == std::string::npos) {
        return std::string();
    }
    auto start = config.find("\"", idx + strlen(key));
    if (start == std::string::npos) {
        return std::string();
    }
    auto end = config.find("\"", start + 1);
    if (end == std::string::npos || end < start + 1) {
        return std::string();
    }

    std::string retStr = std::string(configStr + start + 1, end - start - 1);
    return retStr;
}

std::string OhosResourceAdapterImpl::ParseModuleName(const std::shared_ptr<Extractor> &manager)
{
    if (manager == nullptr) {
        return std::string();
    }
    std::unique_ptr<uint8_t[]> configBuf;
    size_t len;
    bool ret = manager->ExtractToBufByName("config.json", configBuf, len);
    if (!ret) {
        WVLOG_E("failed to get config data from ability");
        return std::string();
    }
    // parse config.json
    std::string mName = GetModuleName(reinterpret_cast<char *>(configBuf.get()), len);
    if (mName.size() == 0) {
        WVLOG_E("parse moduleName from config.json error");
        return std::string();
    }
    return mName;
}

bool OhosResourceAdapterImpl::GetRawFileData(const std::shared_ptr<Extractor>& manager,
    const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest)
{
    if (!manager) {
        return false;
    }
    if (manager->IsStageModel()) {
        return manager->ExtractToBufByName(rawFile, dest, len);
    }
    std::string moduleName = OhosResourceAdapterImpl::ParseModuleName(manager);
    std::string rawFilePath("assets/");
    rawFilePath.append(moduleName);
    rawFilePath.append("/");
    rawFilePath.append(rawFile);
    WVLOG_E("fa filepath:%{public}s", rawFilePath.c_str());
    return manager->ExtractToBufByName(rawFilePath, dest, len);
}

std::shared_ptr<OhosFileMapper> OhosResourceAdapterImpl::GetRawFileMapper(
    const std::shared_ptr<OHOS::AbilityBase::Extractor>& manager,
    const std::string& rawFile)
{
    if (!manager) {
        return nullptr;
    }
    std::unique_ptr<OHOS::AbilityBase::FileMapper> fileMap;
    auto& systemPropertiesAdapter = OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance();
    if (systemPropertiesAdapter.GetWebOptimizationValue()) {
        fileMap = manager->GetMmapData(rawFile);
    } else {
        fileMap = manager->GetData(rawFile);
    }
    if (fileMap == nullptr) {
        return nullptr;
    }
    bool isCompressed = fileMap->IsCompressed();
    return std::make_shared<OhosFileMapperImpl>(std::move(fileMap), isCompressed ? manager: nullptr);
}

std::string OhosResourceAdapterImpl::GetArkWebVersion()
{
    std::string hapPath = OHOS::ArkWeb::GetArkwebInstallPath();
    const std::string packInfoPath = "pack.info";

    OHOS::AbilityBase::Extractor extractor(hapPath);
    if (!extractor.Init()) {
        WVLOG_E("Failed to initialize extractor for HAP file: %{public}s", hapPath.c_str());
        return "";
    }

    std::ostringstream contentStream;
    bool ret = extractor.ExtractByName(packInfoPath, contentStream);
    if (!ret) {
        WVLOG_E("Failed to extract pack.info from HAP: %{public}s", hapPath.c_str());
        return "";
    }

    std::string configContent = contentStream.str();

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(configContent, root)) {
        WVLOG_E("Failed to parse pack.info from HAP: %{public}s", hapPath.c_str());
        return "";
    }

    if (root.isMember("summary") &&
        root["summary"].isMember("app") &&
        root["summary"]["app"].isMember("version") &&
        root["summary"]["app"]["version"].isMember("name")) {
        return root["summary"]["app"]["version"]["name"].asString();
    }

    WVLOG_E("Version information not found in pack.info from HAP: %{public}s", hapPath.c_str());

    WVLOG_E("Failed to get ArkWeb version from any of the specified paths");
    return "";
}

void OhosResourceAdapterImpl::SetArkWebCoreHapPathOverride(const std::string& hapPath)
{
    arkWebCoreHapPathOverride_ = hapPath;
}

std::string OhosResourceAdapterImpl::GetSystemLanguage()
{
    return OHOS::Global::I18n::LocaleConfig::GetSystemLanguage();
}

}  // namespace OHOS::NWeb
