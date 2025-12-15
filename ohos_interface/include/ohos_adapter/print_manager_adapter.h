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

#ifndef PRINT_MANAGER_ADAPTER_H
#define PRINT_MANAGER_ADAPTER_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::NWeb {

struct PrintRangeAdapter {
    uint32_t startPage;
    uint32_t endPage;
    std::vector<uint32_t> pages;
};

struct PrintPageSizeAdapter {
    uint32_t width;
    uint32_t height;
};

struct PrintMarginAdapter {
    uint32_t top;
    uint32_t bottom;
    uint32_t left;
    uint32_t right;
};

struct PrintAttributesAdapter {
    uint32_t copyNumber;
    PrintRangeAdapter pageRange;
    bool isSequential;
    PrintPageSizeAdapter pageSize;
    bool isLandscape;
    uint32_t colorMode;
    uint32_t duplexMode;
    PrintMarginAdapter margin;
    bool hasOption;
    std::string option;
    uint32_t display_header_footer;
    uint32_t print_backgrounds;
};

/*[clang::lto_visibility_public]:Prevent this class from being optimized away at compile time*/ 
class [[clang::lto_visibility_public]] PrintWriteResultCallbackAdapter {
public:
    PrintWriteResultCallbackAdapter() = default;
    virtual ~PrintWriteResultCallbackAdapter() = default;

    virtual void WriteResultCallback(std::string jobId, uint32_t code) = 0;
};

class PrintDocumentAdapterAdapter {
public:
    PrintDocumentAdapterAdapter() = default;
    virtual ~PrintDocumentAdapterAdapter() = default;

    virtual void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::shared_ptr<PrintWriteResultCallbackAdapter> callback) = 0;

    virtual void OnJobStateChanged(const std::string& jobId, uint32_t state) = 0;
};

class PrintManagerAdapter {
public:
    PrintManagerAdapter() = default;

    virtual ~PrintManagerAdapter() = default;

    virtual int32_t StartPrint(
        const std::vector<std::string>& fileList, const std::vector<uint32_t>& fdList, std::string& taskId) = 0;

    virtual int32_t Print(const std::string& printJobName, const std::shared_ptr<PrintDocumentAdapterAdapter> listener,
        const PrintAttributesAdapter& printAttributes) = 0;

    virtual int32_t Print(const std::string& printJobName, const std::shared_ptr<PrintDocumentAdapterAdapter> listener,
        const PrintAttributesAdapter& printAttributes, void* contextToken) = 0;
};

} // namespace OHOS::NWeb

#endif // PRINT_MANAGER_ADAPTER_H
