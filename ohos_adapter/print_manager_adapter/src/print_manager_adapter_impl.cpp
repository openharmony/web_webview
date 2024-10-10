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
PrintManagerAdapterImpl& PrintManagerAdapterImpl::GetInstance()
{
    static PrintManagerAdapterImpl instance;
    return instance;
}

int32_t PrintManagerAdapterImpl::StartPrint(
    const std::vector<std::string>& fileList, const std::vector<uint32_t>& fdList, std::string& taskId)
{
#if defined(NWEB_PRINT_ENABLE)
    int32_t ret = OHOS::Print::PrintManagerClient::GetInstance()->StartPrint(fileList, fdList, taskId);
    if (ret != 0) {
        WVLOG_E("StartPrint failed, failed id = %{public}d", ret);
        return -1;
    }
    return ret;
#else
    return -1;
#endif
}

int32_t PrintManagerAdapterImpl::Print(const std::string& printJobName,
    const std::shared_ptr<PrintDocumentAdapterAdapter> listener, const PrintAttributesAdapter& printAttributes)
{
#if defined(NWEB_PRINT_ENABLE)
    OHOS::Print::PrintDocumentAdapter* adapter = new (std::nothrow) PrintDocumentAdapterImpl(listener);
    if (!adapter) {
        WVLOG_E("adapter get failed");
        return -1;
    }
    sptr<OHOS::Print::IPrintCallback> iCallback = new (std::nothrow) OHOS::Print::PrintCallback(adapter);
    if (!iCallback) {
        WVLOG_E("iCallback get failed");
        return -1;
    }
    auto attributes = std::make_shared<OHOS::Print::PrintAttributes>();
    if (!attributes) {
        WVLOG_E("attributes get failed");
        return -1;
    }
    int32_t ret = OHOS::Print::PrintManagerClient::GetInstance()->Print(printJobName, iCallback, attributes);
    if (ret != 0) {
        WVLOG_E("print failed, failed id = %{public}d", ret);
        return -1;
    }
    return ret;
#else
    return -1;
#endif
}

int32_t PrintManagerAdapterImpl::Print(const std::string& printJobName,
    const std::shared_ptr<PrintDocumentAdapterAdapter> listener, const PrintAttributesAdapter& printAttributes,
    void* contextToken)
{
#if defined(NWEB_PRINT_ENABLE)
    OHOS::Print::PrintDocumentAdapter* adapter = new (std::nothrow) PrintDocumentAdapterImpl(listener);
    if (!adapter) {
        WVLOG_E("adapter get failed");
        return -1;
    }
    sptr<OHOS::Print::IPrintCallback> iCallback = new (std::nothrow) OHOS::Print::PrintCallback(adapter);
    if (!iCallback) {
        WVLOG_E("iCallback get failed");
        return -1;
    }
    auto attributes = std::make_shared<OHOS::Print::PrintAttributes>();
    if (!attributes) {
        WVLOG_E("attributes get failed");
        return -1;
    }
    int32_t ret =
        OHOS::Print::PrintManagerClient::GetInstance()->Print(printJobName, iCallback, attributes, contextToken);
    if (ret != 0) {
        WVLOG_E("print failed, failed id = %{public}d", ret);
        return -1;
    }
    return ret;
#else
    return -1;
#endif
}

#if defined(NWEB_PRINT_ENABLE)
PrintDocumentAdapterImpl::PrintDocumentAdapterImpl(const std::shared_ptr<PrintDocumentAdapterAdapter> cb)
{
    cb_ = cb;
}

PrintAttributesAdapter PrintDocumentAdapterImpl::ConvertPrintingParameters(OHOS::Print::PrintAttributes attrs)
{
    PrintAttributesAdapter printAttributesAdapter;
    printAttributesAdapter.copyNumber = attrs.GetCopyNumber();
    PrintRangeAdapter printRangeAdapter;
    Print::PrintRange range;
    attrs.GetPageRange(range);
    printRangeAdapter.startPage = range.GetStartPage();
    printRangeAdapter.endPage = range.GetEndPage();
    std::vector<uint32_t> pages;
    range.GetPages(pages);
    printRangeAdapter.pages = pages;
    printAttributesAdapter.pageRange = printRangeAdapter;
    printAttributesAdapter.isSequential = attrs.GetIsSequential();
    PrintPageSizeAdapter printPageSizeAdapter;
    Print::PrintPageSize printPageSize;
    attrs.GetPageSize(printPageSize);
    printPageSizeAdapter.width = printPageSize.GetWidth();
    printPageSizeAdapter.height = printPageSize.GetHeight();
    printAttributesAdapter.pageSize = printPageSizeAdapter;
    printAttributesAdapter.isLandscape = attrs.GetIsLandscape();
    printAttributesAdapter.colorMode = attrs.GetColorMode();
    printAttributesAdapter.duplexMode = attrs.GetDuplexMode();
    PrintMarginAdapter printMarginAdapter;
    Print::PrintMargin printMargin;
    attrs.GetMargin(printMargin);
    printMarginAdapter.top = printMargin.GetTop();
    printMarginAdapter.bottom = printMargin.GetBottom();
    printMarginAdapter.left = printMargin.GetLeft();
    printMarginAdapter.right = printMargin.GetRight();
    return printAttributesAdapter;
}

void PrintDocumentAdapterImpl::onStartLayoutWrite(const std::string& jobId,
    const OHOS::Print::PrintAttributes& oldAttrs, const OHOS::Print::PrintAttributes& newAttrs, uint32_t fd,
    std::function<void(std::string, uint32_t)> writeResultCallback)
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

void PrintDocumentAdapterImpl::onJobStateChanged(const std::string& jobId, uint32_t state)
{
    if (!cb_) {
        return;
    }
    cb_->OnJobStateChanged(jobId, state);
}
#endif

PrintWriteResultCallbackAdapterImpl::PrintWriteResultCallbackAdapterImpl(
    std::function<void(std::string, uint32_t)>& cb)
{
    cb_ = cb;
}

void PrintWriteResultCallbackAdapterImpl::WriteResultCallback(std::string jobId, uint32_t code)
{
    cb_(jobId, code);
}

} // namespace OHOS::NWeb
