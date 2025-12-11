/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NWEB_PRINT_MANAGER_ADAPTER_H
#define NWEB_PRINT_MANAGER_ADAPTER_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::NWeb {

const uint32_t NWEB_PRINT_ATTR_ID_IS_VALID = 0;
const uint32_t NWEB_PRINT_ATTR_ID_COPY_NUMBER = 1;
const uint32_t NWEB_PRINT_ATTR_ID_PAGE_RANGE_START = 2;
const uint32_t NWEB_PRINT_ATTR_ID_PAGE_RANGE_END = 3;
const uint32_t NWEB_PRINT_ATTR_ID_PAGE_RANGE_ARRAY = 4;
const uint32_t NWEB_PRINT_ATTR_ID_PAGE_IS_SEQUENTIAL = 5;
const uint32_t NWEB_PRINT_ATTR_ID_PAGE_SIZE_WIDTH = 6;
const uint32_t NWEB_PRINT_ATTR_ID_PAGE_SIZE_HEIGHT = 7;
const uint32_t NWEB_PRINT_ATTR_ID_IS_LANDSCAPE = 8;
const uint32_t NWEB_PRINT_ATTR_ID_COLOR_MODE = 9;
const uint32_t NWEB_PRINT_ATTR_ID_DUPLEX_MODE = 10;
const uint32_t NWEB_PRINT_ATTR_ID_MARGIN_TOP = 11;
const uint32_t NWEB_PRINT_ATTR_ID_MARGIN_BOTTOM = 12;
const uint32_t NWEB_PRINT_ATTR_ID_MARGIN_LEFT = 13;
const uint32_t NWEB_PRINT_ATTR_ID_MARGIN_RIGHT = 14;
const uint32_t NWEB_PRINT_ATTR_ID_HAS_OPTION = 15;
const uint32_t NWEB_PRINT_ATTR_ID_OPTION = 16;
const uint32_t NWEB_PRINT_ATTR_ID_HEADER_FOOTER = 17;
const uint32_t NWEB_PRINT_ATTR_ID_PRINT_BACKGROUNDS = 18;

class NWebPrintAttributesAdapter {
public:
    virtual ~NWebPrintAttributesAdapter() = default;
    virtual bool GetBool(uint32_t attrId) = 0;
    virtual uint32_t GetUInt32(uint32_t attrId) = 0;
    virtual std::string GetString(uint32_t attrId) = 0;
    virtual std::vector<uint32_t> GetUint32Vector(uint32_t attrId) = 0;
};

class NWebPrintWriteResultCallbackAdapter {
public:
    NWebPrintWriteResultCallbackAdapter() = default;
    virtual ~NWebPrintWriteResultCallbackAdapter() = default;

    virtual void WriteResultCallback(const std::string& jobId, uint32_t code) = 0;
};

class NWebPrintDocumentAdapterAdapter {
public:
    NWebPrintDocumentAdapterAdapter() = default;
    virtual ~NWebPrintDocumentAdapterAdapter() = default;

    virtual void OnStartLayoutWrite(const std::string& jobId,
        std::shared_ptr<NWebPrintAttributesAdapter> oldAttrs,
        std::shared_ptr<NWebPrintAttributesAdapter> newAttrs, uint32_t fd,
        std::shared_ptr<NWebPrintWriteResultCallbackAdapter> callback) = 0;

    virtual void OnJobStateChanged(const std::string& jobId, uint32_t state) = 0;
};

} // namespace OHOS::NWeb

#endif // NWEB_PRINT_MANAGER_ADAPTER_H
