/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "zip_file_reader.h"

#include <memory>
#include <sys/stat.h>

#include "constants.h"
#include "zip_file_reader_io.h"

#define WVLOG_I(fmt, ...)
#define WVLOG_E(fmt, ...)
#define WVLOG_D(fmt, ...)
#define WVLOG_W(fmt, ...)

namespace OHOS {
namespace AbilityBase {
std::shared_ptr<ZipFileReader> ZipFileReader::CreateZipFileReader(const std::string &filePath)
{
    size_t fileSize = GetFileLen(filePath);
    if (fileSize == 0) {
        WVLOG_D("fileSize error: %{public}s", filePath.c_str());
        return nullptr;
    }

    std::shared_ptr<ZipFileReader> result = std::make_shared<ZipFileReaderIo>(filePath);
    result->fileLen_ = fileSize;
    if (result->init()) {
        return result;
    }
    return nullptr;
}

ZipFileReader::~ZipFileReader()
{
    if (fd_ >= 0 && closable_) {
        close(fd_);
        fd_ = -1;
    }
}

size_t ZipFileReader::GetFileLen(const std::string &filePath)
{
    if (filePath.empty()) {
        return 0;
    }

    struct stat fileStat{};
    if (stat(filePath.c_str(), &fileStat) == 0) {
        return fileStat.st_size;
    }

    WVLOG_D("error: %{public}s : %{public}d", filePath.c_str(), errno);
    return 0;
}

bool ZipFileReader::init()
{
    if (filePath_.empty()) {
        return false;
    }
    std::string resolvePath;
    resolvePath.reserve(PATH_MAX);
    resolvePath.resize(PATH_MAX - 1);
    if (filePath_.substr(0, Constants::GetProcPrefix().size()) == Constants::GetProcPrefix()) {
        resolvePath = filePath_;
    } else {
        if (realpath(filePath_.c_str(), &(resolvePath[0])) == nullptr) {
            WVLOG_D("realpath error: %{public}s : %{public}d", resolvePath.c_str(), errno);
            return false;
        }
    }
    fd_ = open(resolvePath.c_str(), O_RDONLY);
    if (fd_ < 0) {
        WVLOG_D("open file error: %{public}s : %{public}d", resolvePath.c_str(), errno);
        return false;
    }

    return true;
}
}
}