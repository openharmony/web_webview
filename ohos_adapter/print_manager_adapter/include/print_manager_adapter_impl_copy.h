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

#ifndef PRINT_MANAGER_ADAPTER_IMPL_H
#define PRINT_MANAGER_ADAPTER_IMPL_H

#include "print_manager_adapter.h"

#if defined(NWEB_PRINT_ENABLE)
#include "iprint_callback.h"
#include "print_callback.h"
#include "print_manager_client.h"
#endif

namespace OHOS::NWeb {

#if defined(NWEB_PRINT_ENABLE)
class PrintDocumentAdapterImpl : public OHOS::Print::PrintDocumentAdapter {
public:
    PrintDocumentAdapterImpl(const std::shared_ptr<PrintDocumentAdapterAdapter> cb);

    void onStartLayoutWrite(const std::string& jobId, const OHOS::Print::PrintAttributes& oldAttrs,
        const OHOS::Print::PrintAttributes& newAttrs, uint32_t fd,
        std::function<void(std::string, uint32_t)> writeResultCallback) override;

    void onJobStateChanged(const std::string& jobId, uint32_t state) override;

private:
    PrintAttributesAdapter ConvertPrintingParameters(Print::PrintAttributes attrs);

    std::shared_ptr<PrintDocumentAdapterAdapter> cb_;
};
#endif

class PrintWriteResultCallbackAdapterImpl : public PrintWriteResultCallbackAdapter {
public:
    PrintWriteResultCallbackAdapterImpl(std::function<void(std::string, uint32_t)>& cb);

    void WriteResultCallback(std::string jobId, uint32_t code) override;

private:
    std::function<void(std::string, uint32_t)> cb_;
};

class PrintManagerAdapterImpl : public PrintManagerAdapter {
public:
    static PrintManagerAdapterImpl& GetInstance();

    ~PrintManagerAdapterImpl() override = default;

    int32_t StartPrint(
        const std::vector<std::string>& fileList, const std::vector<uint32_t>& fdList, std::string& taskId) override;

    int32_t Print(const std::string& printJobName, const std::shared_ptr<PrintDocumentAdapterAdapter> listener,
        const PrintAttributesAdapter& printAttributes) override;

    int32_t Print(const std::string& printJobName, const std::shared_ptr<PrintDocumentAdapterAdapter> listener,
        const PrintAttributesAdapter& printAttributes, void* contextToken) override;

private:
    PrintManagerAdapterImpl() = default;

    PrintManagerAdapterImpl(const PrintManagerAdapterImpl& other) = delete;

    PrintManagerAdapterImpl& operator=(const PrintManagerAdapterImpl&) = delete;
};

} // namespace OHOS::NWeb

#endif // PRINT_MANAGER_ADAPTER_IMPL_H
