/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "zip_file.h"

#include <ostream>

#include "constants.h"
#include "file_mapper.h"
#include "file_path_utils.h"
#include "zip_file_reader.h"
#include "zlib.h"

#define WVLOG_I(fmt, ...)
#define WVLOG_E(fmt, ...)
#define WVLOG_D(fmt, ...)
#define WVLOG_W(fmt, ...)

namespace OHOS {
namespace AdapterUtils {
namespace {
constexpr uint32_t MAX_FILE_NAME = 4096;
constexpr uint32_t UNZIP_BUFFER_SIZE = 1024;
constexpr uint32_t UNZIP_BUF_IN_LEN = 160 * UNZIP_BUFFER_SIZE;   // in  buffer length: 160KB
constexpr uint32_t UNZIP_BUF_OUT_LEN = 320 * UNZIP_BUFFER_SIZE;  // out buffer length: 320KB
constexpr uint32_t LOCAL_HEADER_SIGNATURE = 0x04034b50;
constexpr uint32_t CENTRAL_SIGNATURE = 0x02014b50;
constexpr uint32_t EOCD_SIGNATURE = 0x06054b50;
constexpr uint32_t DATA_DESC_SIGNATURE = 0x08074b50;
constexpr uint32_t FLAG_DATA_DESC = 0x8;
constexpr uint8_t INFLATE_ERROR_TIMES = 5;
constexpr uint8_t MAP_FILE_SUFFIX = 4;
constexpr char FILE_SEPARATOR_CHAR = '/';
constexpr const char* WRONG_FILE_SEPARATOR = "//";
constexpr uint32_t CACHE_CASE_THRESHOLD = 10000;

void GetTreeFileList(std::shared_ptr<DirTreeNode> root, const std::string &rootPath,
    std::vector<std::string> &assetList)
{
    if (root == nullptr) {
        return;
    }
    if (!root->isDir && !rootPath.empty()) {
        assetList.push_back(rootPath);
    } else {
        std::string prefix = rootPath;
        if (!prefix.empty()) {
            prefix.push_back(FILE_SEPARATOR_CHAR);
        }
        for (const auto &child : root->children) {
            GetTreeFileList(child.second, prefix + child.first, assetList);
        }
    }
}

void AddEntryToTree(const std::string &fileName, std::shared_ptr<DirTreeNode> root)
{
    if (root == nullptr) {
        return;
    }
    size_t cur = 0;
    auto parent = root;
    do {
        while (cur < fileName.size() && fileName[cur] == FILE_SEPARATOR_CHAR) {
            cur++;
        }
        if (cur >= fileName.size()) {
            break;
        }
        auto next = fileName.find(FILE_SEPARATOR_CHAR, cur);
        auto nodeName = fileName.substr(cur, next - cur);
        auto it = parent->children.find(nodeName);
        if (it != parent->children.end()) {
            parent = it->second;
        } else {
            auto node = std::make_shared<DirTreeNode>();
            node->isDir = next != std::string::npos;
            parent->children.emplace(nodeName, node);
            parent = node;
        }
        cur = next;
    } while (cur != std::string::npos);
}

inline bool IsRootDir(const std::string &dirName)
{
    return dirName.size() == 1 && dirName.back() == FILE_SEPARATOR_CHAR;
}
}  // namespace

ZipEntry::ZipEntry(const CentralDirEntry &centralEntry)
{
    compressionMethod = centralEntry.compressionMethod;
    uncompressedSize = centralEntry.uncompressedSize;
    compressedSize = centralEntry.compressedSize;
    localHeaderOffset = centralEntry.localHeaderOffset;
    crc = centralEntry.crc;
    flags = centralEntry.flags;
    modifiedTime = centralEntry.modifiedTime;
    modifiedDate = centralEntry.modifiedDate;
}

ZipFile::ZipFile(const std::string &pathName) : pathName_(pathName) {}

ZipFile::~ZipFile()
{
    Close();
}

bool ZipFile::CheckEndDir(const EndDir &endDir) const
{
    size_t lenEndDir = sizeof(EndDir);
    if ((endDir.numDisk != 0) || (endDir.signature != EOCD_SIGNATURE) || (endDir.startDiskOfCentralDir != 0) ||
        (endDir.offset >= fileLength_) || (endDir.totalEntriesInThisDisk != endDir.totalEntries) ||
        (endDir.commentLen != 0) ||
        // central dir can't overlap end of central dir
        ((endDir.offset + endDir.sizeOfCentralDir + lenEndDir) > fileLength_)) {
        WVLOG_W("failed:fileLen: %{public}llu, signature: %{public}u, numDisk: %{public}hu, "
            "startDiskOfCentralDir: %{public}hu, totalEntriesInThisDisk: %{public}hu, totalEntries: %{public}hu, "
            "sizeOfCentralDir: %{public}u, offset: %{public}u, commentLen: %{public}hu",
            fileLength_, endDir.signature, endDir.numDisk, endDir.startDiskOfCentralDir, endDir.totalEntriesInThisDisk,
            endDir.totalEntries, endDir.sizeOfCentralDir, endDir.offset, endDir.commentLen);
        return false;
    }
    return true;
}

bool ZipFile::ParseEndDirectory()
{
    size_t endDirLen = sizeof(EndDir);
    size_t endFilePos = fileStartPos_ + fileLength_;

    if (fileLength_ <= endDirLen) {
        WVLOG_E("fileStartPos_:(%{public}llu) <= fileLength_:(%{public}llu)",
            fileStartPos_, fileLength_);
        return false;
    }

    size_t eocdPos = endFilePos - endDirLen;
    if (!zipFileReader_->ReadBuffer(reinterpret_cast<uint8_t*>(&endDir_), eocdPos, sizeof(EndDir))) {
        WVLOG_E("read EOCD failed");
        return false;
    }

    centralDirPos_ = endDir_.offset + fileStartPos_;

    return CheckEndDir(endDir_);
}

bool ZipFile::ParseOneEntry(uint8_t* &entryPtr)
{
    if (entryPtr == nullptr) {
        WVLOG_E("null entryPtr");
        return false;
    }

    CentralDirEntry directoryEntry;
    if (memcpy(&directoryEntry, entryPtr, sizeof(CentralDirEntry)) == NULL) {
        WVLOG_E("Mem copy directory entry failed");
        return false;
    }

    if (directoryEntry.signature != CENTRAL_SIGNATURE) {
        WVLOG_E("check signature failed");
        return false;
    }

    entryPtr += sizeof(CentralDirEntry);
    size_t fileLength = (directoryEntry.nameSize >= MAX_FILE_NAME) ? (MAX_FILE_NAME - 1) : directoryEntry.nameSize;
    std::string fileName(fileLength, 0);
    if (memcpy(&(fileName[0]), entryPtr, fileLength) == NULL) {
        WVLOG_E("Mem copy file name failed");
        return false;
    }

    ZipEntry currentEntry(directoryEntry);
    currentEntry.fileName = fileName;
    entriesMap_[fileName] = currentEntry;
    entryPtr += directoryEntry.nameSize + directoryEntry.extraSize + directoryEntry.commentSize;
    return true;
}

std::shared_ptr<DirTreeNode> ZipFile::MakeDirTree() const
{
    WVLOG_I("called");
    auto root = std::make_shared<DirTreeNode>();
    root->isDir = true;
    for (const auto &[fileName, entry]: entriesMap_) {
        AddEntryToTree(fileName, root);
    }
    return root;
}

std::shared_ptr<DirTreeNode> ZipFile::GetDirRoot()
{
    if (!isOpen_) {
        return nullptr;
    }
    std::lock_guard guard(dirRootMutex_);
    if (dirRoot_ == nullptr) {
        dirRoot_ = MakeDirTree();
    }
    return dirRoot_;
}

bool ZipFile::ParseAllEntries()
{
    auto centralData = zipFileReader_->ReadBuffer(static_cast<size_t>(centralDirPos_),
        static_cast<size_t>(endDir_.sizeOfCentralDir));
    if (centralData.empty()) {
        WVLOG_E("centralData empty for [%{public}s] failed", pathName_.c_str());
        return false;
    }

    bool ret = true;
    uint8_t *entryPtr = reinterpret_cast<uint8_t *>(centralData.data());
    for (uint16_t i = 0; i < endDir_.totalEntries; i++) {
        if (!ParseOneEntry(entryPtr)) {
            WVLOG_E("Parse entry[%{public}d] failed", i);
            ret = false;
            break;
        }
    }

    return ret;
}

bool ZipFile::Open()
{
    if (isOpen_) {
        WVLOG_E("opened");
        return true;
    }

    if (pathName_.length() > PATH_MAX) {
        WVLOG_E("failed:path length:%{public}u",
            static_cast<unsigned int>(pathName_.length()));
        return false;
    }
    std::string realPath;
    realPath.reserve(PATH_MAX);
    realPath.resize(PATH_MAX - 1);
    if (pathName_.substr(0, Constants::GetProcPrefix().size()) == Constants::GetProcPrefix()) {
        realPath = pathName_;
    } else {
        if (realpath(pathName_.c_str(), &(realPath[0])) == nullptr) {
            WVLOG_E("realpath error: %{public}d, pathName: %{public}s", errno, pathName_.c_str());
            return false;
        }
    }

    zipFileReader_ = ZipFileReader::CreateZipFileReader(realPath);
    if (!zipFileReader_) {
        WVLOG_E("open file(%{public}s) failed", pathName_.c_str());
        return false;
    }

    if (fileLength_ == 0) {
        auto fileLength = zipFileReader_->GetFileLen();
        fileLength_ = static_cast<ZipPos>(fileLength);
        if (fileStartPos_ >= fileLength_) {
            WVLOG_E("pos > length");
            zipFileReader_.reset();
            return false;
        }

        fileLength_ -= fileStartPos_;
    }

    bool result = ParseEndDirectory();
    if (result) {
        result = ParseAllEntries();
    }
    // it means open file success.
    isOpen_ = true;
    return result;
}

void ZipFile::Close()
{
    if (!isOpen_ || zipFileReader_ == nullptr) {
        WVLOG_D("not opened");
        return;
    }

    isOpen_ = false;
    entriesMap_.clear();
    {
        std::lock_guard guard(dirRootMutex_);
        dirRoot_.reset();
    }
    pathName_ = "";

    zipFileReader_.reset();
}

// Get all file zipEntry in this file
const ZipEntryMap &ZipFile::GetAllEntries() const
{
    return entriesMap_;
}

bool ZipFile::HasEntry(const std::string &entryName) const
{
    return entriesMap_.find(entryName) != entriesMap_.end();
}

void ZipFile::SetCacheMode(CacheMode cacheMode)
{
    std::lock_guard lock(dirRootMutex_);
    cacheMode_ = cacheMode;
    if (!UseDirCache()) {
        dirRoot_.reset();
    }
}

bool ZipFile::UseDirCache() const
{
    auto mode = cacheMode_;
    bool useCache = mode == CacheMode::CACHE_ALL;
    if (mode == CacheMode::CACHE_CASE && entriesMap_.size() >= CACHE_CASE_THRESHOLD) {
        useCache = true;
    }
    return useCache;
}

bool ZipFile::IsDirExist(const std::string &dir)
{
    if (dir.empty()) {
        WVLOG_E("dir empty");
        return false;
    }
    if (IsRootDir(dir)) {
        return true;
    }
    if (dir.find(WRONG_FILE_SEPARATOR) != std::string::npos) {
        WVLOG_W("Wrong format");
        return false;
    }

    auto tmpDir = dir;
    if (tmpDir.front() == FILE_SEPARATOR_CHAR) {
        tmpDir.erase(tmpDir.begin());
    }
    if (tmpDir.back() != FILE_SEPARATOR_CHAR) {
        tmpDir.push_back(FILE_SEPARATOR_CHAR);
    }
    if (entriesMap_.count(tmpDir) > 0) {
        return true;
    }
    tmpDir.pop_back();
    if (entriesMap_.count(tmpDir) > 0) {
        WVLOG_W("file not dir");
        return false;
    }

    if (UseDirCache()) {
        return IsDirExistCache(tmpDir);
    }
    return IsDirExistNormal(tmpDir);
}

void ZipFile::GetAllFileList(const std::string &srcPath, std::vector<std::string> &assetList)
{
    if (srcPath.empty()) {
        WVLOG_W("empty dir");
        return;
    }
    if (IsRootDir(srcPath)) {
        for (const auto &[fileName, fileInfo] : entriesMap_) {
            if (!fileName.empty() && fileName.back() != FILE_SEPARATOR_CHAR) {
                assetList.push_back(fileName);
            }
        }
        return;
    }
    if (srcPath.find(WRONG_FILE_SEPARATOR) != std::string::npos) {
        WVLOG_W("Wrong format");
        return;
    }

    auto tmpDir = srcPath;
    if (tmpDir.front() == FILE_SEPARATOR_CHAR) {
        tmpDir.erase(tmpDir.begin());
    }
    if (tmpDir.back() != FILE_SEPARATOR_CHAR) {
        tmpDir.push_back(FILE_SEPARATOR_CHAR);
    }
    if (entriesMap_.count(tmpDir) > 0) {
        return;
    }
    tmpDir.pop_back();
    if (entriesMap_.count(tmpDir) > 0) {
        WVLOG_W("file not dir");
        return;
    }

    if (UseDirCache()) {
        GetAllFileListCache(tmpDir, assetList);
    } else {
        GetAllFileListNormal(tmpDir, assetList);
    }
}

void ZipFile::GetChildNames(const std::string &srcPath, std::set<std::string> &fileSet)
{
    if (srcPath.empty()) {
        WVLOG_E("empty dir");
        return;
    }
    if (srcPath.find(WRONG_FILE_SEPARATOR) != std::string::npos) {
        WVLOG_W("Wrong input format");
        return;
    }
    auto tmpDir = srcPath;
    if (!IsRootDir(tmpDir)) {
        if (tmpDir.front() == FILE_SEPARATOR_CHAR) {
            tmpDir.erase(tmpDir.begin());
        }
        if (tmpDir.back() != FILE_SEPARATOR_CHAR) {
            tmpDir.push_back(FILE_SEPARATOR_CHAR);
        }
        if (entriesMap_.count(tmpDir) > 0) {
            return;
        }
        tmpDir.pop_back();
        if (entriesMap_.count(tmpDir) > 0) {
            WVLOG_W("file not dir");
            return;
        }
    }

    if (UseDirCache()) {
        GetChildNamesCache(tmpDir, fileSet);
    } else {
        GetChildNamesNormal(tmpDir, fileSet);
    }
}

bool ZipFile::IsDirExistCache(const std::string &dir)
{
    auto parent = GetDirRoot();
    if (parent == nullptr) {
        WVLOG_E("null parent");
        return false;
    }
    size_t cur = 0;
    do {
        while (cur < dir.size() && dir[cur] == FILE_SEPARATOR_CHAR) {
            cur++;
        }
        if (cur >= dir.size()) {
            break;
        }
        auto next = dir.find(FILE_SEPARATOR_CHAR, cur);
        auto nodeName = dir.substr(cur, next - cur);
        auto it = parent->children.find(nodeName);
        if (it == parent->children.end()) {
            WVLOG_D("dir not found, dir : %{public}s", dir.c_str());
            return false;
        }
        parent = it->second;
        cur = next;
    } while (cur != std::string::npos);

    return true;
}

void ZipFile::GetAllFileListCache(const std::string &srcPath, std::vector<std::string> &assetList)
{
    auto parent = GetDirRoot();
    if (parent == nullptr) {
        WVLOG_E("null parent");
        return;
    }

    auto rootName = srcPath.back() == FILE_SEPARATOR_CHAR ?
        srcPath.substr(0, srcPath.length() - 1) : srcPath;

    size_t cur = 0;
    do {
        while (cur < rootName.size() && rootName[cur] == FILE_SEPARATOR_CHAR) {
            cur++;
        }
        if (cur >= rootName.size()) {
            break;
        }
        auto next = rootName.find(FILE_SEPARATOR_CHAR, cur);
        auto nodeName = rootName.substr(cur, next - cur);
        auto it = parent->children.find(nodeName);
        if (it == parent->children.end()) {
            WVLOG_E("srcPath not found: %{public}s", rootName.c_str());
            return;
        }
        parent = it->second;
        cur = next;
    } while (cur != std::string::npos);

    GetTreeFileList(parent, rootName, assetList);
}

void ZipFile::GetChildNamesCache(const std::string &srcPath, std::set<std::string> &fileSet)
{
    size_t cur = 0;
    auto parent = GetDirRoot();
    if (parent == nullptr) {
        WVLOG_E("null parent");
        return;
    }
    do {
        while (cur < srcPath.size() && srcPath[cur] == FILE_SEPARATOR_CHAR) {
            cur++;
        }
        if (cur >= srcPath.size()) {
            break;
        }
        auto next = srcPath.find(FILE_SEPARATOR_CHAR, cur);
        auto nodeName = srcPath.substr(cur, next - cur);
        auto it = parent->children.find(nodeName);
        if (it == parent->children.end()) {
            WVLOG_I("srcPath not found: %{public}s", srcPath.c_str());
            return;
        }
        parent = it->second;
        cur = next;
    } while (cur != std::string::npos);

    for (const auto &child : parent->children) {
        fileSet.insert(child.first);
    }
}

bool ZipFile::IsDirExistNormal(const std::string &dir)
{
    auto targetDir = dir;
    if (targetDir.back() != FILE_SEPARATOR_CHAR) {
        targetDir.push_back(FILE_SEPARATOR_CHAR);
    }
    for (const auto &[fileName, fileInfo] : entriesMap_) {
        if (fileName.size() > targetDir.size() && fileName.substr(0, targetDir.size()) == targetDir) {
            return true;
        }
    }
    return false;
}

void ZipFile::GetAllFileListNormal(const std::string &srcPath, std::vector<std::string> &assetList)
{
    auto targetDir = srcPath;
    if (targetDir.back() != FILE_SEPARATOR_CHAR) {
        targetDir.push_back(FILE_SEPARATOR_CHAR);
    }
    for (const auto &[fileName, fileInfo] : entriesMap_) {
        if (fileName.size() > targetDir.size() && fileName.back() != FILE_SEPARATOR_CHAR &&
            fileName.substr(0, targetDir.size()) == targetDir) {
            assetList.push_back(fileName);
        }
    }
}

void ZipFile::GetChildNamesNormal(const std::string &srcPath, std::set<std::string> &fileSet)
{
    auto targetDir = srcPath;
    if (targetDir.back() != FILE_SEPARATOR_CHAR) {
        targetDir.push_back(FILE_SEPARATOR_CHAR);
    }
    if (IsRootDir(srcPath)) {
        for (const auto &[fileName, fileInfo] : entriesMap_) {
            auto nextPos = fileName.find(FILE_SEPARATOR_CHAR);
            fileSet.insert(nextPos == std::string::npos ? fileName : fileName.substr(0, nextPos));
        }
        return;
    }
    for (const auto &[fileName, fileInfo] : entriesMap_) {
        if (fileName.size() > targetDir.size() && fileName.substr(0, targetDir.size()) == targetDir) {
            fileSet.insert(fileName.substr(targetDir.size(),
                fileName.find(FILE_SEPARATOR_CHAR, targetDir.size()) - targetDir.size()));
        }
    }
}

bool ZipFile::GetEntry(const std::string &entryName, ZipEntry &resultEntry) const
{
    auto iter = entriesMap_.find(entryName);
    if (iter != entriesMap_.end()) {
        resultEntry = iter->second;
        return true;
    }
    return false;
}

size_t ZipFile::GetLocalHeaderSize(const uint16_t nameSize, const uint16_t extraSize) const
{
    return sizeof(LocalHeader) + nameSize + extraSize;
}

bool ZipFile::CheckDataDesc(const ZipEntry &zipEntry, const LocalHeader &localHeader) const
{
    uint32_t crcLocal = 0;
    uint32_t compressedLocal = 0;
    uint32_t uncompressedLocal = 0;

    if (localHeader.flags & FLAG_DATA_DESC) {  // use data desc
        DataDesc dataDesc;
        auto descPos = zipEntry.localHeaderOffset + GetLocalHeaderSize(localHeader.nameSize, localHeader.extraSize);
        descPos += fileStartPos_ + zipEntry.compressedSize;

        if (!zipFileReader_->ReadBuffer(reinterpret_cast<uint8_t*>(&dataDesc), descPos, sizeof(DataDesc))) {
            WVLOG_E("ReadBuffer failed");
            return false;
        }

        if (dataDesc.signature != DATA_DESC_SIGNATURE) {
            WVLOG_E("check signature failed");
            return false;
        }

        crcLocal = dataDesc.crc;
        compressedLocal = dataDesc.compressedSize;
        uncompressedLocal = dataDesc.uncompressedSize;
    } else {
        crcLocal = localHeader.crc;
        compressedLocal = localHeader.compressedSize;
        uncompressedLocal = localHeader.uncompressedSize;
    }

    if ((zipEntry.crc != crcLocal) || (zipEntry.compressedSize != compressedLocal) ||
        (zipEntry.uncompressedSize != uncompressedLocal)) {
        WVLOG_E("size corrupted");
        return false;
    }

    return true;
}

bool ZipFile::CheckCoherencyLocalHeader(const ZipEntry &zipEntry, uint16_t &extraSize) const
{
    // current only support store and Z_DEFLATED method
    if ((zipEntry.compressionMethod != Z_DEFLATED) && (zipEntry.compressionMethod != 0)) {
        WVLOG_E("compressionMethod(%{public}d) not support", zipEntry.compressionMethod);
        return false;
    }

    auto nameSize = zipEntry.fileName.length();
    auto startPos = fileStartPos_ + zipEntry.localHeaderOffset;
    size_t buffSize = sizeof(LocalHeader) + nameSize;
    auto buff = zipFileReader_->ReadBuffer(startPos, buffSize);
    if (buff.size() < buffSize) {
        WVLOG_E("read header failed");
        return false;
    }

    LocalHeader localHeader = {0};
    if (memcpy(&localHeader, buff.data(), sizeof(LocalHeader)) == NULL) {
        WVLOG_E("memcpy localheader failed");
        return false;
    }
    if ((localHeader.signature != LOCAL_HEADER_SIGNATURE) ||
        (zipEntry.compressionMethod != localHeader.compressionMethod)) {
        WVLOG_E("signature or compressionMethod failed");
        return false;
    }

    if (localHeader.nameSize != nameSize && nameSize < MAX_FILE_NAME - 1) {
        WVLOG_E("name corrupted");
        return false;
    }
    std::string fileName = buff.substr(sizeof(LocalHeader));
    if (zipEntry.fileName != fileName) {
        WVLOG_E("name corrupted");
        return false;
    }

    if (!CheckDataDesc(zipEntry, localHeader)) {
        WVLOG_E("check data desc failed");
        return false;
    }

    extraSize = localHeader.extraSize;
    return true;
}

size_t ZipFile::GetEntryStart(const ZipEntry &zipEntry, const uint16_t extraSize) const
{
    ZipPos startOffset = zipEntry.localHeaderOffset;
    // get data offset, add signature+localheader+namesize+extrasize
    startOffset += GetLocalHeaderSize(zipEntry.fileName.length(), extraSize);
    startOffset += fileStartPos_;  // add file start relative to file stream

    return startOffset;
}

bool ZipFile::InitZStream(z_stream &zstream) const
{
    // init zlib stream
    if (memset(&zstream, 0, sizeof(z_stream))) {
        WVLOG_E("stream buffer init failed");
        return false;
    }
    int32_t zlibErr = inflateInit2(&zstream, -MAX_WBITS);
    if (zlibErr != Z_OK) {
        WVLOG_E("init failed");
        return false;
    }

    BytePtr bufOut = new (std::nothrow) Byte[UNZIP_BUF_OUT_LEN];
    if (bufOut == nullptr) {
        WVLOG_E("null bufOut");
        return false;
    }

    BytePtr bufIn = new (std::nothrow) Byte[UNZIP_BUF_IN_LEN];
    if (bufIn == nullptr) {
        WVLOG_E("null bufIn");
        delete[] bufOut;
        return false;
    }
    zstream.next_out = bufOut;
    zstream.next_in = bufIn;
    zstream.avail_out = UNZIP_BUF_OUT_LEN;
    return true;
}

ZipPos ZipFile::GetEntryDataOffset(const ZipEntry &zipEntry, const uint16_t extraSize) const
{
    // get entry data offset relative file
    ZipPos offset = zipEntry.localHeaderOffset;

    offset += GetLocalHeaderSize(zipEntry.fileName.length(), extraSize);
    offset += fileStartPos_;

    return offset;
}

bool ZipFile::GetDataOffsetRelative(const ZipEntry &zipEntry, ZipPos &offset, uint32_t &length) const
{
    uint16_t extraSize = 0;
    if (!CheckCoherencyLocalHeader(zipEntry, extraSize)) {
        WVLOG_E("check coherency local header failed");
        return false;
    }

    offset = GetEntryDataOffset(zipEntry, extraSize);
    length = zipEntry.compressedSize;
    return true;
}

bool ZipFile::ExtractFileFromMMap(const std::string &file, void *mmapDataPtr,
    std::unique_ptr<uint8_t[]> &dataPtr, size_t &len) const
{
    ZipEntry zipEntry;
    if (!GetEntry(file, zipEntry)) {
        WVLOG_E("not find file");
        return false;
    }

    if (!zipEntry.compressionMethod) {
        WVLOG_E("file[%{public}s] is not extracted", file.c_str());
        return false;
    }

    uint16_t extraSize = 0;
    if (!CheckCoherencyLocalHeader(zipEntry, extraSize)) {
        WVLOG_E("check coherency local header failed");
        return false;
    }

    return UnzipWithInflatedFromMMap(zipEntry, extraSize, mmapDataPtr, dataPtr, len);
}

bool ZipFile::UnzipWithInflatedFromMMap(const ZipEntry &zipEntry, const uint16_t extraSize,
    void *mmapDataPtr, std::unique_ptr<uint8_t[]> &dataPtr, size_t &len) const
{
    z_stream zstream;
    if (!InitZStream(zstream)) {
        WVLOG_E("Init zstream failed");
        return false;
    }

    BytePtr bufIn = zstream.next_in;
    BytePtr bufOut = zstream.next_out;

    bool ret = true;
    int32_t zlibErr = Z_OK;
    uint32_t remainCompressedSize = zipEntry.compressedSize;
    size_t inflateLen = 0;
    uint8_t errorTimes = 0;

    len = zipEntry.uncompressedSize;
    dataPtr = std::make_unique<uint8_t[]>(len);
    uint8_t *dstDataPtr = static_cast<uint8_t *>(dataPtr.get());
    void *mmapSrcDataPtr = mmapDataPtr;

    while ((remainCompressedSize > 0) || (zstream.avail_in > 0)) {
        if (!ReadZStreamFromMMap(bufIn, mmapSrcDataPtr, zstream, remainCompressedSize)) {
            ret = false;
            break;
        }

        zlibErr = inflate(&zstream, Z_SYNC_FLUSH);
        if ((zlibErr >= Z_OK) && (zstream.msg != nullptr)) {
            WVLOG_E("unzip error: %{public}d, msg: %{public}s", zlibErr, zstream.msg);
            ret = false;
            break;
        }

        inflateLen = UNZIP_BUF_OUT_LEN - zstream.avail_out;
        if (!CopyInflateOut(zstream, inflateLen, &dstDataPtr, bufOut, errorTimes)) {
            break;
        }
    }

    // free all dynamically allocated data structures except the next_in and next_out for this stream.
    zlibErr = inflateEnd(&zstream);
    if (zlibErr != Z_OK) {
        WVLOG_E("inflateEnd error: %{public}d", zlibErr);
        ret = false;
    }

    delete[] bufOut;
    delete[] bufIn;
    return ret;
}

bool ZipFile::CopyInflateOut(z_stream &zstream, size_t inflateLen, uint8_t** dstDataPtr,
    BytePtr bufOut, uint8_t &errorTimes) const
{
    if (inflateLen > 0) {
        if (memcpy(*dstDataPtr, bufOut, inflateLen) == NULL) {
            WVLOG_E("Mem copy failed");
            return false;
        }

        *dstDataPtr += inflateLen;
        zstream.next_out = bufOut;
        zstream.avail_out = UNZIP_BUF_OUT_LEN;
        errorTimes = 0;
    } else {
        errorTimes++;
    }
    if (errorTimes >= INFLATE_ERROR_TIMES) {
        WVLOG_E("data is abnormal");
        return false;
    }

    return true;
}

bool ZipFile::ReadZStreamFromMMap(const BytePtr &buffer, void* &dataPtr,
    z_stream &zstream, uint32_t &remainCompressedSize) const
{
    if (!dataPtr) {
        WVLOG_E("null dataPtr");
        return false;
    }

    uint8_t *srcDataPtr = static_cast<uint8_t *>(dataPtr);
    if (zstream.avail_in == 0) {
        size_t remainBytes = (remainCompressedSize > UNZIP_BUF_IN_LEN) ? UNZIP_BUF_IN_LEN : remainCompressedSize;
        size_t readBytes = sizeof(Byte) * remainBytes;
        if (memcpy(buffer, srcDataPtr, readBytes) == NULL) {
            WVLOG_E("Mem copy failed");
            return false;
        }
        srcDataPtr += readBytes;
        remainCompressedSize -= remainBytes;
        zstream.avail_in = remainBytes;
        zstream.next_in = buffer;
    }
    dataPtr = srcDataPtr;
    return true;
}

std::unique_ptr<FileMapper> ZipFile::CreateFileMapper(const std::string &fileName, FileMapperType type) const
{
    ZipEntry zipEntry;
    if (!GetEntry(fileName, zipEntry)) {
        WVLOG_E("GetEntry failed hapPath %{public}s", fileName.c_str());
        return nullptr;
    }

    ZipPos offset = 0;
    uint32_t length = 0;
    if (!GetDataOffsetRelative(zipEntry, offset, length)) {
        WVLOG_E("GetDataOffsetRelative failed hapPath %{public}s", fileName.c_str());
        return nullptr;
    }
    bool compress = zipEntry.compressionMethod > 0;
    if (type == FileMapperType::SAFE_ABC && compress) {
        WVLOG_W("Entry is compressed for safe: %{public}s", fileName.c_str());
    }
    std::unique_ptr<FileMapper> fileMapper = std::make_unique<FileMapper>();
    auto result = false;
    if (type == FileMapperType::NORMAL_MEM) {
        result = fileMapper->CreateFileMapper(zipFileReader_, fileName, offset, length, compress);
    } else {
        result = fileMapper->CreateFileMapper(fileName, compress, zipFileReader_->GetFd(), offset, length, type);
        if (result && type == FileMapperType::SAFE_ABC) {
            zipFileReader_->SetClosable(false);
        }
    }

    if (!result) {
        return nullptr;
    }
    return fileMapper;
}

bool ZipFile::ExtractToBufByName(const std::string &fileName, std::unique_ptr<uint8_t[]> &dataPtr,
    size_t &len) const
{
    ZipEntry zipEntry;
    if (!GetEntry(fileName, zipEntry)) {
        if (fileName.length() > MAP_FILE_SUFFIX && fileName.substr(fileName.length() - MAP_FILE_SUFFIX) != ".map") {
            WVLOG_E("GetEntry failed hapPath %{public}s", fileName.c_str());
        }
        return false;
    }
    uint16_t extraSize = 0;
    if (!CheckCoherencyLocalHeader(zipEntry, extraSize)) {
        WVLOG_E("check coherency local header failed");
        return false;
    }

    ZipPos offset = GetEntryDataOffset(zipEntry, extraSize);
    uint32_t length = zipEntry.compressedSize;
    auto dataTmp = std::make_unique<uint8_t[]>(length);
    if (!zipFileReader_->ReadBuffer(dataTmp.get(), offset, length)) {
        WVLOG_E("read file failed, len[%{public}zu] fileName: %{public}s, offset: %{public}zu",
            len, fileName.c_str(), (size_t)offset);
        dataTmp.reset();
        return false;
    }

    if (zipEntry.compressionMethod > 0) {
        return UnzipWithInflatedFromMMap(zipEntry, extraSize, dataTmp.get(), dataPtr, len);
    }

    len = length;
    dataPtr = std::move(dataTmp);

    return true;
}
}  // namespace AdapterUtils
}  // namespace OHOS
