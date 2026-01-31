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

#include <nlohmann/json.hpp>
#include "nweb_log.h"

namespace OHOS::NWeb {
namespace {
#if defined(NWEB_PRINT_ENABLE)
static const std::string OptionStart = "{\"custom_option\":{";
static const std::string OptionEnd = "}}";

static const std::string HeaderStart = "\"header_and_footer\":{\"default_value\":";
static const std::string HeaderEnd = "},";

static const std::string BackgroundStart = "\"background_graphics\":{\"default_value\":";
static const std::string BackgroundEnd = "},";

std::shared_ptr<OHOS::Print::PrintAttributes> CreateAttrsWithCustomOption(const PrintAttributesAdapter& printAttr)
{
    auto attr = std::make_shared<OHOS::Print::PrintAttributes>();
    std::string customOptions;

    if (printAttr.display_header_footer != UINT32_MAX) {
        std::string option = !!printAttr.display_header_footer ? "true" : "false";
        customOptions += HeaderStart + option + HeaderEnd;
    }

    if (printAttr.print_backgrounds != UINT32_MAX) {
        std::string option = !!printAttr.print_backgrounds ? "true" : "false";
        customOptions += BackgroundStart + option + BackgroundEnd;
    }

    // remove last comma
    if (!customOptions.empty() && customOptions.back() == ',') {
        customOptions.pop_back();
    }

    attr->SetOption(OptionStart + customOptions + OptionEnd);
    WVLOG_D("Start Print With Custom Option: Header=%{public}u, Background=%{public}u",
        printAttr.display_header_footer, printAttr.print_backgrounds);
    return attr;
}
#endif
}

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
    auto attributes = CreateAttrsWithCustomOption(printAttributes);
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
    auto attributes = CreateAttrsWithCustomOption(printAttributes);
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

static void ParseCustomOptions(const OHOS::Print::PrintAttributes& attrs, PrintAttributesAdapter& printAttr)
{
    printAttr.display_header_footer = UINT32_MAX;
    printAttr.print_backgrounds = UINT32_MAX;

    if (!attrs.HasOption()) {
        WVLOG_D("No Custom Option");
        return;
    }

    nlohmann::json option = nlohmann::json::parse(attrs.GetOption(), nullptr, false);
    if (option.is_discarded()) {
        WVLOG_D("Custom Option Format Error");
        return;
    }

    if (option.contains("header_and_footer") && option["header_and_footer"].contains("value")) {
        auto value = option["header_and_footer"]["value"];
        if (value.is_boolean()) {
            printAttr.display_header_footer = value.get<bool>();
        }
    }

    if (option.contains("background_graphics") && option["background_graphics"].contains("value")) {
        auto value = option["background_graphics"]["value"];
        if (value.is_boolean()) {
            printAttr.print_backgrounds = value.get<bool>();
        }
    }

    WVLOG_D("Parse Custom Option: Header=%{public}u, Background=%{public}u",
        printAttr.display_header_footer, printAttr.print_backgrounds);
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
    printAttributesAdapter.margin = printMarginAdapter;
    ParseCustomOptions(attrs, printAttributesAdapter);
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
