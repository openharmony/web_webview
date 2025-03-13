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

#include "drawingtextadapter_fuzzer.h"
#include "ohos_drawing_text_adapter_impl.h"

using namespace OHOS::NWeb;
namespace OHOS {
bool DrawingTextFontAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    size_t callCount = data[0] % 10;
    for (size_t i = 0; i < callCount; ++i) {
        OhosDrawingTextFontAdapter &adapter = OhosDrawingTextFontAdapterImpl::GetInstance();
        void* drawingArray = nullptr;
        void* drawingString = nullptr;
        void* drawingFontDescriptor = nullptr;
        const void* drawString = nullptr;

        adapter.GetSystemFontFullNamesByType(i, &drawingArray);
        adapter.GetFontDescriptorByFullName(drawingString, i, &drawingFontDescriptor);
        adapter.GetSystemFontFullNameByIndex(drawingArray, i, &drawString);
        adapter.DestroyFontDescriptor(drawingFontDescriptor);
        adapter.DestroySystemFontFullNames(drawingArray);
    }
    return true;
}

bool DrawingTextTypographyAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if((data == nullptr) || (size == 0)) {
        return false;
    }

    size_t callCount = data[0] % 10;
    for (size_t i = 0; i < callCount; ++i) {
        OhosDrawingTextTypographyAdapter &adapter = OhosDrawingTextTypographyAdapterImpl::GetInstance();
        void* fontConfigInfoErrorCode = nullptr;
        void* fontConfigInfo = nullptr;
        void* drawingArray = nullptr;
        int32_t testNum = i;

        adapter.GetSystemFontConfigInfo(fontConfigInfoErrorCode, &fontConfigInfo);
        adapter.GetDrawingArraySize(drawingArray, testNum);
        adapter.DestroySystemFontConfigInfo(fontConfigInfo);
    }
    return true;
}
} // namespace OHOS


/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code oh data */
    OHOS::DrawingTextFontAdapterFuzzTest(data, size);
    OHOS::DrawingTextTypographyAdapterFuzzTest(data, size);
    return 0;
}
