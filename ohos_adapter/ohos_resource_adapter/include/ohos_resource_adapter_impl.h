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

#ifndef OHOS_RESOURCE_ADAPTER_IMPL_H
#define OHOS_RESOURCE_ADAPTER_IMPL_H

#include "extractor.h"
#include "file_mapper.h"
#include "ohos_resource_adapter.h"

#include "napi/native_api.h"
#include <rawfile/raw_file_manager.h>
#include <resourcemanager/ohresmgr.h>
#include <resourcemanager/resmgr_common.h>

namespace OHOS::NWeb {
class OhosFileMapperImpl : public OhosFileMapper {
public:
    OhosFileMapperImpl(std::unique_ptr<OHOS::AdapterUtils::FileMapper> fileMap,
        const std::shared_ptr<OHOS::AdapterUtils::Extractor>& extractor);

    ~OhosFileMapperImpl() override = default;

    int32_t GetFd() override;

    int32_t GetOffset() override;

    std::string GetFileName() override;

    bool IsCompressed() override;

    void* GetDataPtr() override;

    size_t GetDataLen() override;

    bool UnzipData(uint8_t** dest, size_t& len) override;

private:
    std::shared_ptr<OHOS::AdapterUtils::Extractor> extractor_ = nullptr;

    std::unique_ptr<OHOS::AdapterUtils::FileMapper> fileMap_ = nullptr;
};

class OhosResourceAdapterImpl : public OhosResourceAdapter {
public:
    explicit OhosResourceAdapterImpl(const std::string& hapPath);

    ~OhosResourceAdapterImpl() override;

    bool GetRawFileData(const std::string& rawFile, size_t& len,
        uint8_t** dest, bool isSys = false) override;

    std::shared_ptr<OhosFileMapper> GetRawFileMapper(const std::string& rawFile,
        bool isSys = false) override;

    bool IsRawFileExist(const std::string& rawFile, bool isSys = false) override;

    bool GetRawFileLastModTime(const std::string& rawFile,
        uint16_t& date, uint16_t& time, bool isSys = false) override;

    bool GetRawFileLastModTime(const std::string& rawFile, time_t& time, bool isSys = false) override;

    static bool GetResourceString(const std::string& bundleName, const std::string& moduleName,
        const int32_t resId, std::string& result);

    static void setApplicationResourceManager(napi_env env, napi_value jsResMgr);

private:
    void Init(const std::string& hapPath);

    static std::string GetModuleName(const char *configStr, size_t len);

    static std::string ParseModuleName(const std::shared_ptr<OHOS::AdapterUtils::Extractor> &manager);

    static bool GetRawFileData(const std::shared_ptr<OHOS::AdapterUtils::Extractor>& manager,
        const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest);

    static bool HasEntry(const std::shared_ptr<OHOS::AdapterUtils::Extractor>& manager,
        const std::string& rawFile);

    static bool GetFileInfo(const std::shared_ptr<OHOS::AdapterUtils::Extractor>& manager,
        const std::string& rawFile, OHOS::AdapterUtils::FileInfo& info);

    static std::shared_ptr<OhosFileMapper> GetRawFileMapper(
        const std::shared_ptr<OHOS::AdapterUtils::Extractor>& manager,
        const std::string& rawFile);

    std::shared_ptr<OHOS::AdapterUtils::Extractor> sysExtractor_;
    std::shared_ptr<OHOS::AdapterUtils::Extractor> extractor_;

    static NativeResourceManager* mgr_;
};
}  // namespace OHOS::NWeb

#endif  // OHOS_RESOURCE_ADAPTER_IMPL_H