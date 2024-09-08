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

#include "print_manager_adapter_impl.h"

#include "nweb_log.h"

namespace OHOS::NWeb {
// static
std::shared_ptr<PrintDocumentAdapterAdapter> PrintDocumentAdapterImpl::cb_ = nullptr;
Print_WriteResultCallback PrintWriteResultCallbackAdapterImpl::cb_ = nullptr;
PrintManagerAdapterImpl& PrintManagerAdapterImpl::GetInstance()
{
    static PrintManagerAdapterImpl instance;
    return instance;
}

int32_t PrintManagerAdapterImpl::StartPrint(
    const std::vector<std::string>& fileList, const std::vector<uint32_t>& fdList, std::string& taskId)
{
    WVLOG_W("StartPrint is not used.");
    return 0;
}

int32_t PrintManagerAdapterImpl::Print(const std::string& printJobName,
    const std::shared_ptr<PrintDocumentAdapterAdapter> listener, const PrintAttributesAdapter& printAttributes)
{
    std::shared_ptr<PrintDocumentAdapterImpl> adapter = std::make_shared<PrintDocumentAdapterImpl>(listener);
    
    Print_PrintDocCallback printDocCallback = {
        .startLayoutWriteCb = adapter->onStartLayoutWrite,
        .jobStateChangedCb = adapter->onJobStateChanged,
    };
    Print_ErrorCode ret = OH_Print_StartPrintByNative(printJobName.c_str(), printDocCallback, nullptr);
    if (ret != 0) {
        WVLOG_E("print failed, failed id = %{public}d", ret);
        return -1;
    }
    return ret;
}

int32_t PrintManagerAdapterImpl::Print(const std::string& printJobName,
    const std::shared_ptr<PrintDocumentAdapterAdapter> listener, const PrintAttributesAdapter& printAttributes,
    void* contextToken)
{
    std::shared_ptr<PrintDocumentAdapterImpl> adapter = std::make_shared<PrintDocumentAdapterImpl>(listener);
    
    Print_PrintDocCallback printDocCallback = {
        .startLayoutWriteCb = adapter->onStartLayoutWrite,
        .jobStateChangedCb = adapter->onJobStateChanged,
    };
    Print_ErrorCode ret = OH_Print_StartPrintByNative(printJobName.c_str(), printDocCallback, contextToken);
    if (ret != 0) {
        WVLOG_E("print failed, failed id = %{public}d", ret);
        return -1;
    }
    return ret;
}

PrintDocumentAdapterImpl::PrintDocumentAdapterImpl(const std::shared_ptr<PrintDocumentAdapterAdapter> cb)
{
    cb_ = cb;
}

PrintAttributesAdapter PrintDocumentAdapterImpl::ConvertPrintingParameters(const Print_PrintAttributes *attrs)
{
    PrintAttributesAdapter printAttributesAdapter;
    printAttributesAdapter.copyNumber = attrs->copyNumber;
    PrintRangeAdapter printRangeAdapter;
    printRangeAdapter.startPage = attrs->pageRange.startPage;
    printRangeAdapter.endPage = attrs->pageRange.endPage;
    std::vector<uint32_t> pages;
    for (uint32_t i = 0; i < attrs->pageRange.pagesArrayLen; i++) {
        pages.push_back(attrs->pageRange.pagesArray[i]);
    }
    printRangeAdapter.pages = pages;
    printAttributesAdapter.pageRange = printRangeAdapter;
    printAttributesAdapter.isSequential = attrs->isSequential;
    PrintPageSizeAdapter printPageSizeAdapter;
    printPageSizeAdapter.width = attrs->pageSize.width;
    printPageSizeAdapter.height = attrs->pageSize.height;
    printAttributesAdapter.pageSize = printPageSizeAdapter;
    printAttributesAdapter.isLandscape = attrs->isLandscape;
    printAttributesAdapter.colorMode = attrs->colorMode;
    printAttributesAdapter.duplexMode = attrs->duplexMode;
    PrintMarginAdapter printMarginAdapter;
    printMarginAdapter.top = attrs->pageMargin.topMargin;
    printMarginAdapter.bottom = attrs->pageMargin.bottomMargin;
    printMarginAdapter.left = attrs->pageMargin.leftMargin;
    printMarginAdapter.right = attrs->pageMargin.rightMargin;
    return printAttributesAdapter;
}

void PrintDocumentAdapterImpl::onStartLayoutWrite(const char *jobId, uint32_t fd, const Print_PrintAttributes *oldAttrs,
    const Print_PrintAttributes *newAttrs, Print_WriteResultCallback writeResultCallback)
{
    if (!cb_) {
        return;
    }

    std::shared_ptr<PrintWriteResultCallbackAdapter> callback =
        std::make_shared<PrintWriteResultCallbackAdapterImpl>(writeResultCallback);
    if (!callback) {
        return;
    }

    cb_->OnStartLayoutWrite(
        jobId, ConvertPrintingParameters(oldAttrs), ConvertPrintingParameters(newAttrs), fd, callback);
}

void PrintDocumentAdapterImpl::onJobStateChanged(const char *jobId, uint32_t state)
{
    if (!cb_) {
        return;
    }
    std::string id = std::string(jobId);
    cb_->OnJobStateChanged(jobId, state);
}

PrintWriteResultCallbackAdapterImpl::PrintWriteResultCallbackAdapterImpl(
    Print_WriteResultCallback cb)
{
    cb_ = cb;
}

void PrintWriteResultCallbackAdapterImpl::WriteResultCallback(std::string jobId, uint32_t code)
{
    if (cb_) {
        cb_(jobId.c_str(), code);
    }
}

} // namespace OHOS::NWeb
