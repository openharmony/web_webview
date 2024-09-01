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

#include "ohos_resource_adapter_impl.h"

#include <ctime>
#include <sstream>
#include <cerrno>
#include <cstring>
#include <unistd.h>

#include "nweb_log.h"
#include "ohos_adapter_helper.h"

using namespace OHOS::AbilityBase;

namespace {
const std::string NWEB_HAP_PATH = "/system/app/com.ohos.nweb/NWeb.hap";
const std::string NWEB_HAP_PATH_1 = "/system/app/NWeb/NWeb.hap";
const std::string NWEB_HAP_PATH_MODULE_UPDATE = "/module_update/ArkWebCore/app/com.ohos.nweb/NWeb.hap";
const std::string PERSIST_ARKWEBCORE_INSTALL_PATH 
    = "/module_update/ArkWebCore/app/com.huawei.hmos.arkwebcore/ArwWebCore.hap";
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

NativeResourceManager* OHOS::NWEB::OhosResourceAdapterImpl::mgr_ = nullptr;

namespace OHOS::NWeb {
namespace {

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

std::string GetNWebHapPath()
{
    if (access(NWEB_HAP_PATH.c_str(), F_OK) == 0) {
        WVLOG_D("eixt NWEB_HAP_PATH");
        return NWEB_HAP_PATH;
    }
    WVLOG_W("access ohos nweb hap path failed, errno(%{public}d): %{public}s", errno, strerror(errno));
    if (access(NWEB_HAP_PATH_1.c_str(), F_OK) == 0) {
        WVLOG_D("eixt NWEB_HAP_PATH_1");
        return NWEB_HAP_PATH_1;
    }
    WVLOG_W("access nweb hap path failed, errno(%{public}d): %{public}s", errno, strerror(errno));
    if (access(NWEB_HAP_PATH_MODULE_UPDATE.c_str(), F_OK) == 0) {
        WVLOG_D("eixt NWEB_HAP_PATH_MODULE_UPDATE");
        return NWEB_HAP_PATH_MODULE_UPDATE;
    }
    WVLOG_W("access nweb hap module update path failed, errno(%{public}d): %{public}s", errno, strerror(errno));
    std::string install_path = PERSIST_ARKWEBCORE_INSTALL_PATH;
    if (access(install_path.c_str(), F_OK) == 0) {
        WVLOG_D("eixt install_path");
        return install_path;
    }
    WVLOG_E("access nweb install path failed, errno(%{public}d): %{public}s", errno, strerror(errno));
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

OhosResourceAdapterImpl::OhosResourceAdapterImpl(const std::string& hapPath)
{
    Init(hapPath);
}

OhosResourceAdapterImpl::~OhosResourceAdapterImpl()
{
    OH_ResourceManager_ReleaseNativeResourceManager(mgr_);
}

void OhosResourceAdapterImpl::Init(const std::string& hapPath)
{
    bool newCreate = false;
    std::string nwebHapPath = GetNWebHapPath();
    if (!nwebHapPath.empty()) {
        sysExtractor_ = ExtractorUtil::GetExtractor(nwebHapPath, newCreate);
        if (!sysExtractor_) {
            WVLOG_E("RuntimeExtractor create failed for %{public}s", nwebHapPath.c_str());
        }
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
       RawFile *rawFileTmp = OH_ResourceManager_OpenRawFile(mgr_, fileName.c_str());
       if (rawFileTmp != nullptr) {
            WVLOG_D("OH_ResourceManager_OpenRawFile success");
       }
       long length = OH_ResourceManager_GetRawFileSize(rawFileTmp);
       data = std::make_unique<uint8_t[]>(length);
       if (length == 0L) {
            WVLOG_E("OH_ResourceManager_OpenRawFile failed");
            return false;
       }
       *dest = data.release();
       len = static_cast<size_t>(length);
       OH_ResourceManager_CloseRawFile(rawFileTmp);
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
    WVLOG_D("OhosResourceAdapterImpl::GetResourceString");
    char *res;
    ResourceManager_ErrorCode errorCode = OH_ResourceManager_GetString(mgr_, resId, &res);
    if (errorCode == ResourceManager_ErrorCode::SUCCESS) {
        result = res;
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
    WVLOG_D("OhosResourceAdapterImpl::GetRawFileMapper");
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
}  // namespace OHOS::NWeb