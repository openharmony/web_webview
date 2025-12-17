/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_WEBVIEW_CONTROLLER_PRINT_STRUCTS_H
#define NWEB_WEBVIEW_CONTROLLER_PRINT_STRUCTS_H

#include <memory>
#include <string>
#include <vector>

#include "nweb.h"
#include "nweb_helper.h"
#include "print_manager_adapter.h"

namespace OHOS {
namespace NWeb {

class WebPrintAttributes : public NWebPrintAttributesAdapter {
public:
    explicit WebPrintAttributes(const PrintAttributesAdapter& attrs) : attrs_(attrs) {}
    bool GetBool(uint32_t attrId) override;
    uint32_t GetUInt32(uint32_t attrId) override;
    std::string GetString(uint32_t attrId) override;
    std::vector<uint32_t> GetUint32Vector(uint32_t attrId) override;
private:
    PrintAttributesAdapter attrs_;
};

class WebPrintDocument {
public:
    explicit WebPrintDocument(NWebPrintDocumentAdapterAdapter* webPrintdocV2) : printDocAdapterV2_(webPrintdocV2) {}
    ~WebPrintDocument() = default;
    void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::function<void(std::string, uint32_t)> writeResultCallback);

    void OnJobStateChanged(const std::string& jobId, uint32_t state);

private:
    std::unique_ptr<NWebPrintDocumentAdapterAdapter> printDocAdapterV2_ = nullptr;
};

class WebPrintWriteResultCallbackAdapterV2 : public NWebPrintWriteResultCallbackAdapter {
public:
    explicit WebPrintWriteResultCallbackAdapterV2(std::function<void(std::string, uint32_t)>& cb) : cb_(cb) {}

    void WriteResultCallback(const std::string& jobId, uint32_t code) override;

private:
    std::function<void(std::string, uint32_t)> cb_ = nullptr;
};
} // namespace NWeb
} // namespace OHOS

#endif // NWEB_WEBVIEW_CONTROLLER_PRINT_STRUCTS_H