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

#include "ohos_nweb/cpptoc/ark_web_output_frame_callback_cpptoc.h"

#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

bool ARK_WEB_CALLBACK ark_web_output_frame_callback_handle(
    struct _ark_web_output_frame_callback_t* self, const char* buffer, uint32_t width, uint32_t height)
{
    ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

    ARK_WEB_CPPTOC_CHECK_PARAM(self, false);

    ARK_WEB_CPPTOC_CHECK_PARAM(buffer, false);

    // Execute
    return ArkWebOutputFrameCallbackCppToC::Get(self)->Handle(buffer, width, height);
}

} // namespace

ArkWebOutputFrameCallbackCppToC::ArkWebOutputFrameCallbackCppToC()
{
    GetStruct()->handle = ark_web_output_frame_callback_handle;
}

ArkWebOutputFrameCallbackCppToC::~ArkWebOutputFrameCallbackCppToC() {}

template<>
ArkWebBridgeType ArkWebCppToCRefCounted<ArkWebOutputFrameCallbackCppToC, ArkWebOutputFrameCallback,
    ark_web_output_frame_callback_t>::kBridgeType = ARK_WEB_OUTPUT_FRAME_CALLBACK;

} // namespace OHOS::ArkWeb