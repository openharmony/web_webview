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

#ifndef ARK_OHOS_NATIVE_BUFFER_ADAPTER_H
#define ARK_OHOS_NATIVE_BUFFER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkOhosNativeBufferAdapter : public virtual ArkWebBaseRefCounted {
public:
    ArkOhosNativeBufferAdapter() = default;

    virtual ~ArkOhosNativeBufferAdapter() = default;

    /**
     * @Description: Adds the reference count of a OH_NativeBuffer.
     * @Input buffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual void AcquireBuffer(void* buffer) = 0;

    /**
     * @Description: Decreases the reference count of a OH_NativeBuffer and, when the reference count reaches 0, \n
     *               destroys this OH_NativeBuffer.
     * @Input buffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual void Release(void* buffer) = 0;

    /**
     * @Description: Get the egl buffer from the <b>OH_NativeBuffer</b> instance.
     * @Input buffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Output eglBuffer: Indicates the pointer to a egl buffer pointer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int GetEGLBuffer(void* buffer, void** eglBuffer) = 0;

    /**
     * @Description: Free egl buffer.
     * @Input eglBuffer: Indicates the pointer to an egl buffer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int FreeEGLBuffer(void* eglBuffer) = 0;

    /**
     * @Description: Converts an <b>OHNativeWindowBuffer</b> instance to an <b>OH_NativeBuffer</b>.
     * @Input nativeWindowBuffer: Indicates the pointer to a <b>OHNativeWindowBuffer</b> instance.
     * @Output nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> pointer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int NativeBufferFromNativeWindowBuffer(void* nativeWindowBuffer, void** nativeBuffer) = 0;

    /**
     * @Description: Get the sequence number of native buffer .
     * @Input nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> instance.
     * @Return：Returns a sequence number.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual uint32_t GetSeqNum(void* nativeBuffer) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_NATIVE_BUFFER_ADAPTER_H
