/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "zip_file_reader_io.h"

#include <cstdlib>

#define WVLOG_I(fmt, ...)
#define WVLOG_E(fmt, ...)
#define WVLOG_D(fmt, ...)
#define WVLOG_W(fmt, ...)

namespace OHOS {
namespace AdapterUtils {
namespace {
constexpr size_t BIG_FILE_SIZE = 1u << 31;
}
std::string ZipFileReaderIo::ReadBuffer(size_t startPos, size_t bufferSize)
{
    std::string result;
    if (fd_ < 0 || startPos >= fileLen_ || bufferSize > fileLen_ - startPos) {
        WVLOG_D("failed: %{public}s, startPos: %{public}zu, bufferSize: %{public}zu, fileLen: %{public}zu",
            filePath_.c_str(), startPos, bufferSize, fileLen_);
        return result;
    }

    result.resize(bufferSize);
    if (!ReadBuffer((uint8_t*)result.data(), startPos, bufferSize)) {
        result.clear();
    }

    return result;
}

bool ZipFileReaderIo::ReadBuffer(uint8_t *dst, size_t startPos, size_t bufferSize)
{
    if (dst == nullptr || fd_ < 0 || startPos >= fileLen_ || bufferSize > fileLen_ - startPos) {
        WVLOG_D("failed: %{public}s, startPos: %{public}zu, bufferSize: %{public}zu, fileLen: %{public}zu",
            filePath_.c_str(), startPos, bufferSize, fileLen_);
        return false;
    }

    auto remainSize = bufferSize;
    ssize_t nread = 0;
    do {
        nread = pread(fd_, dst, remainSize, startPos);
        if (nread <= 0) {
            break;
        }

        startPos += (size_t)nread;
        dst += nread;
        remainSize -= (size_t)nread;
    } while (remainSize > 0);
    if (remainSize > 0) {
        WVLOG_D("readfile error: %{public}s-%{public}d", filePath_.c_str(), errno);
        return false;
    }

    if (bufferSize > BIG_FILE_SIZE) {
        WVLOG_D("big file io success: %{public}zu", bufferSize);
    }
    return true;
}
}
}