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

#include "base/include/ark_web_log_utils.h"
#include "ohos_adapter/bridge/ark_ohos_native_buffer_adapter_wrapper.h"

#define HILOG_TAG "NATIVE_BUFFER_ADAPTER"

namespace OHOS::ArkWeb {

ArkOhosNativeBufferAdapterWrapper::ArkOhosNativeBufferAdapterWrapper(ArkWebRefPtr<ArkOhosNativeBufferAdapter> ref)
    : ctocpp_(ref) {}


void ArkOhosNativeBufferAdapterWrapper::AcquireBuffer(void* buffer)
{
    ctocpp_->AcquireBuffer(buffer);
}

void ArkOhosNativeBufferAdapterWrapper::Release(void* buffer)
{
    ctocpp_->Release(buffer);
}

int ArkOhosNativeBufferAdapterWrapper::GetEGLBuffer(void* buffer, void** eglBuffer)
{
    return ctocpp_->GetEGLBuffer(buffer, eglBuffer);
}

int ArkOhosNativeBufferAdapterWrapper::FreeEGLBuffer(void* eglBuffer)
{
    return ctocpp_->FreeEGLBuffer(eglBuffer);
}

int ArkOhosNativeBufferAdapterWrapper::NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer)
{
    return ctocpp_->NativeBufferFromNativeWindowBuffer(nativeWindowBuffer, nativeBuffer);
}

uint32_t ArkOhosNativeBufferAdapterWrapper::GetSeqNum(void* nativeBuffer)
{
    return ctocpp_->GetSeqNum(nativeBuffer);
}
} // namespace OHOS::ArkWeb
