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

#ifndef OHOS_RESOURCE_ADAPTER_H
#define OHOS_RESOURCE_ADAPTER_H

#include <string>
#include <sys/types.h>

namespace OHOS::NWeb {

class OhosFileMapper {
public:
    OhosFileMapper() = default;
    virtual ~OhosFileMapper() = default;

    virtual int32_t GetFd() = 0;

    virtual int32_t GetOffset() = 0;

    virtual std::string GetFileName() = 0;

    virtual bool IsCompressed() = 0;

    virtual void* GetDataPtr() = 0;

    virtual size_t GetDataLen() = 0;

    virtual bool UnzipData(uint8_t** dest, size_t& len) = 0;
};

class OhosResourceAdapter {
public:
    OhosResourceAdapter() = default;
    virtual ~OhosResourceAdapter() = default;

    virtual bool GetRawFileData(const std::string& rawFile, size_t& len, uint8_t** dest, bool isSys = false) = 0;

    virtual std::shared_ptr<OhosFileMapper> GetRawFileMapper(const std::string& rawFile, bool isSys = false) = 0;

    virtual bool IsRawFileExist(const std::string& rawFile, bool isSys = false) = 0;

    virtual bool GetRawFileLastModTime(
        const std::string& rawFile, uint16_t& date, uint16_t& time, bool isSys = false) = 0;

    virtual bool GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys = false) = 0;

    virtual std::string GetSystemLanguage() = 0;
};

} // namespace OHOS::NWeb

#endif // OHOS_RESOURCE_ADAPTER_H
