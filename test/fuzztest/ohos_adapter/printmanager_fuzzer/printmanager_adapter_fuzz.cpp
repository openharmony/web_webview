/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "printmanager_adapter_fuzz.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "print_manager_adapter_impl.h"

namespace OHOS::NWeb {

const size_t MAX_LEN_STRING = 256;
const size_t MAX_FD_LIST_SIZE = 10;

bool StartPrintFuzzTest(PrintManagerAdapterImpl& adapter, const uint8_t* data, size_t size)
{
    std::vector<std::string> fileList;
    std::vector<uint32_t> fdList;

    for (size_t i = 0; i < (size % MAX_FD_LIST_SIZE + 1); ++i) {
        std::string fileName(reinterpret_cast<const char*>(data), std::min(size, MAX_LEN_STRING - 1));
        fileList.push_back(fileName);
        fdList.push_back(i);
    }

    std::string taskId;

    adapter.StartPrint(fileList, fdList, taskId);
    return true;
}

bool PrintFuzzTest(PrintManagerAdapterImpl& adapter, const uint8_t* data, size_t size)
{
    std::string printJobName(reinterpret_cast<const char*>(data), std::min(size, MAX_LEN_STRING - 1));
    std::shared_ptr<PrintDocumentAdapterAdapter> listener = nullptr;
    PrintAttributesAdapter printAttributes;

    adapter.Print(printJobName, listener, printAttributes);
    return true;
}

bool PrintWithContextFuzzTest(PrintManagerAdapterImpl& adapter, const uint8_t* data, size_t size)
{
    std::string printJobName(reinterpret_cast<const char*>(data), std::min(size, MAX_LEN_STRING - 1));
    std::shared_ptr<PrintDocumentAdapterAdapter> listener = nullptr;
    PrintAttributesAdapter printAttributes;
    void* contextToken = nullptr;

    adapter.Print(printJobName, listener, printAttributes, contextToken);

    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size == 0) {
        return 0;
    }

    PrintManagerAdapterImpl& adapter = PrintManagerAdapterImpl::GetInstance();

    enum PrintFuzzOperation { START_PRINT, PRINT, PRINT_WITH_CONTEXT, MAX_PRINT_OPERATION };

    PrintFuzzOperation operation = static_cast<PrintFuzzOperation>(data[0] % MAX_PRINT_OPERATION);

    switch (operation) {
        case START_PRINT:
            return StartPrintFuzzTest(adapter, data + 1, size - 1);
        case PRINT:
            return PrintFuzzTest(adapter, data + 1, size - 1);
        case PRINT_WITH_CONTEXT:
            return PrintWithContextFuzzTest(adapter, data + 1, size - 1);
        default:
            return 0;
    }

    return 0;
}

} // namespace OHOS::NWeb