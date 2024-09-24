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

#include "extractor.h"
#include "nweb_log.h"

#include <fstream>
#include <sstream>
#include "constants.h"
#include "file_path_utils.h"
#include <algorithm>

using namespace OHOS::NWeb;
namespace OHOS {
namespace AdapterUtils {
namespace {
constexpr char EXT_NAME_ABC[] = ".abc";
}
Extractor::Extractor(const std::string &source) : zipFile_(source)
{
    hapPath_ = source;
}

Extractor::~Extractor()
{}

bool Extractor::Init()
{
    if (!zipFile_.Open()) {
        WVLOG_E("open zip file failed");
        return false;
    }
    initial_ = true;
    return true;
}

bool Extractor::GetFileBuffer(const std::string& srcPath, std::ostringstream& dest)
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;

    if (!ExtractToBufByName(srcPath, data, dataLen)) {
        return false;
    }

    dest.write(reinterpret_cast<char*>(data.get()), dataLen);
    return true;
}

bool Extractor::GetFileList(const std::string& srcPath, std::vector<std::string>& assetList)
{
    if (!initial_) {
        WVLOG_E("not init");
        return false;
    }

    if (srcPath.empty()) {
        WVLOG_E("empty srcPath");
        return false;
    }
    zipFile_.GetAllFileList(srcPath, assetList);
    if (assetList.empty()) {
        WVLOG_W("empty dir: %{public}s", srcPath.c_str());
    }

    return true;
}

bool Extractor::HasEntry(const std::string &fileName) const
{
    if (!initial_) {
        WVLOG_E("not init");
        return false;
    }

    return zipFile_.HasEntry(fileName);
}

bool Extractor::IsDirExist(const std::string &dir)
{
    if (!initial_) {
        WVLOG_E("not init");
        return false;
    }
    if (dir.empty()) {
        WVLOG_E("dir empty");
        return false;
    }
    return zipFile_.IsDirExist(dir);
}

bool Extractor::ExtractByName(const std::string &fileName, std::ostream &dest) const
{
    std::unique_ptr<uint8_t[]> data;
    size_t dataLen = 0;
    if (!ExtractToBufByName(fileName, data, dataLen)) {
        WVLOG_E("ExtractFile fail: %{public}s", fileName.c_str());
        return false;
    }
    dest.write(reinterpret_cast<char*>(data.get()), dataLen);
    return true;
}

void Extractor::GetSpecifiedTypeFiles(std::vector<std::string> &fileNames, const std::string &suffix)
{
    auto &entryMap = zipFile_.GetAllEntries();
    for (const auto &entry : entryMap) {
        std::string fileName = entry.first;
        auto position = fileName.rfind('.');
        if (position != std::string::npos) {
            std::string suffixStr = fileName.substr(position);
            std::transform(suffixStr.begin(), suffixStr.end(), suffixStr.begin(), 
                [](unsigned char c){ return std::tolower(c); });
            if (suffixStr == suffix) {
                fileNames.emplace_back(fileName);
            }
        }
    }
}

std::unique_ptr<FileMapper> Extractor::GetData(const std::string &fileName) const
{
    std::string relativePath = GetRelativePath(fileName);
    return zipFile_.CreateFileMapper(relativePath, FileMapperType::NORMAL_MEM);
}

std::shared_ptr<FileMapper> Extractor::GetSafeData(const std::string &fileName)
{
    std::string relativePath = GetRelativePath(fileName);
    if (!StringEndWith(relativePath, EXT_NAME_ABC, sizeof(EXT_NAME_ABC) - 1)) {
        return nullptr;
    }

    return zipFile_.CreateFileMapper(relativePath, FileMapperType::SAFE_ABC);
}

std::unique_ptr<FileMapper> Extractor::GetMmapData(const std::string &fileName)
{
    std::string relativePath = GetRelativePath(fileName);
    return zipFile_.CreateFileMapper(relativePath, FileMapperType::SHARED_MMAP);
}

bool Extractor::UnzipData(std::unique_ptr<FileMapper> fileMapper,
    std::unique_ptr<uint8_t[]> &dataPtr, size_t &len) const
{
    if (!initial_) {
        WVLOG_E("not init");
        return false;
    }

    if (!fileMapper) {
        WVLOG_E("null fileMapper");
        return false;
    }

    if (!zipFile_.ExtractFileFromMMap(fileMapper->GetFileName(), fileMapper->GetDataPtr(), dataPtr, len)) {
        WVLOG_E("ExtractFileFromMMap failed");
        return false;
    }
    return true;
}

bool Extractor::IsStageModel()
{
    if (isStageModel_.has_value()) {
        return isStageModel_.value();
    }
    isStageModel_ = !zipFile_.HasEntry("config.json");
    return isStageModel_.value();
}

bool Extractor::ExtractToBufByName(const std::string &fileName, std::unique_ptr<uint8_t[]> &dataPtr,
    size_t &len) const
{
    std::string relativePath = GetRelativePath(fileName);
    return zipFile_.ExtractToBufByName(relativePath, dataPtr, len);
}

bool Extractor::GetFileInfo(const std::string &fileName, FileInfo &fileInfo) const
{
    std::string relativePath = GetRelativePath(fileName);
    ZipEntry zipEntry;
    if (!zipFile_.GetEntry(relativePath, zipEntry)) {
        WVLOG_E("Get entry failed");
        return false;
    }

    ZipPos offset = 0;
    uint32_t length = 0;
    if (!zipFile_.GetDataOffsetRelative(zipEntry, offset, length)) {
        WVLOG_E("GetDataOffsetRelative failed");
        return false;
    }

    fileInfo.fileName = fileName;
    fileInfo.offset = static_cast<uint32_t>(offset);
    fileInfo.length = static_cast<uint32_t>(length);
    fileInfo.lastModTime = zipEntry.modifiedTime;
    fileInfo.lastModDate = zipEntry.modifiedDate;
    return true;
}

bool Extractor::GetFileList(const std::string &srcPath, std::set<std::string> &fileSet)
{
    if (!initial_) {
        WVLOG_E("not init");
        return false;
    }

    if (srcPath.empty()) {
        WVLOG_E("empty srcPath");
        return false;
    }

    zipFile_.GetChildNames(srcPath, fileSet);
    if (fileSet.empty()) {
        WVLOG_D("empty dir: %{public}s", srcPath.c_str());
    }

    return true;
}

bool Extractor::IsHapCompress(const std::string &fileName) const
{
    std::string relativePath = GetRelativePath(fileName);
    ZipEntry zipEntry;
    if (!zipFile_.GetEntry(relativePath, zipEntry)) {
        WVLOG_E("GetEntry failed fileName: %{public}s", fileName.c_str());
        return false;
    }
    return zipEntry.compressionMethod > 0;
}

std::mutex ExtractorUtil::mapMutex_;
std::unordered_map<std::string, std::shared_ptr<Extractor>> ExtractorUtil::extractorMap_;
std::string ExtractorUtil::GetLoadFilePath(const std::string &hapPath)
{
    std::string loadPath;
    if (StringStartWith(hapPath, Constants::ABS_CODE_PATH, std::string(Constants::ABS_CODE_PATH).length())) {
        loadPath = GetLoadPath(hapPath);
    } else {
        loadPath = hapPath;
    }
    return loadPath;
}

std::shared_ptr<Extractor> ExtractorUtil::GetExtractor(const std::string &hapPath, bool &newCreate, bool cache)
{
    newCreate = false;
    if (hapPath.empty()) {
        WVLOG_E("empty hapPath");
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> mapMutex(mapMutex_);
        auto mapIter = extractorMap_.find(hapPath);
        if (mapIter != extractorMap_.end()) {
            WVLOG_D("hapPath: %{private}s", hapPath.c_str());
            return mapIter->second;
        }
    }

    std::shared_ptr<Extractor> extractor = std::make_shared<Extractor>(hapPath);
    if (!extractor->Init()) {
        WVLOG_D("create failed for %{private}s", hapPath.c_str());
        return nullptr;
    }
    if (cache) {
        std::lock_guard<std::mutex> mapMutex(mapMutex_);
        extractorMap_.emplace(hapPath, extractor);
        WVLOG_D("extractor cache size: %{public}zu", extractorMap_.size());
    }
    newCreate = true;
    return extractor;
}

void ExtractorUtil::DeleteExtractor(const std::string &hapPath)
{
    if (hapPath.empty()) {
        WVLOG_E("empty hapPath");
        return;
    }

    std::lock_guard<std::mutex> mapMutex(mapMutex_);
    auto mapIter = extractorMap_.find(hapPath);
    if (mapIter != extractorMap_.end()) {
        WVLOG_I("hapPath: %{public}s", hapPath.c_str());
        extractorMap_.erase(mapIter);
    }
}
}  // namespace AdapterUtils
}  // namespace OHOS
