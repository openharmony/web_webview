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

#include "cpptoc/ark_print_write_result_callback_adapter_cpptoc.h"

#include "cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_print_write_result_callback_adapter_write_result_callback(
    struct _ark_print_write_result_callback_adapter_t* self, ArkWebString jobId, uint32_t code)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, );

    // Execute
    ArkPrintWriteResultCallbackAdapterCppToC::Get(self)->WriteResultCallback(jobId, code);
}

} // namespace

ArkPrintWriteResultCallbackAdapterCppToC::ArkPrintWriteResultCallbackAdapterCppToC()
{
    GetStruct()->write_result_callback = ark_print_write_result_callback_adapter_write_result_callback;
}

ArkPrintWriteResultCallbackAdapterCppToC::~ArkPrintWriteResultCallbackAdapterCppToC() {}

template<>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkPrintWriteResultCallbackAdapterCppToC, ArkPrintWriteResultCallbackAdapter,
    ark_print_write_result_callback_adapter_t>::kBridgeType = ARK_PRINT_WRITE_RESULT_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb