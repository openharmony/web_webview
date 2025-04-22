/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_GRAPHIC_ADAPTER_H
#define ARK_GRAPHIC_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_web_adapter_structs.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkVSyncAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint32_t RequestVsync(void* data, void* cb) = 0;

    /*--ark web()--*/
    virtual int64_t GetVSyncPeriod() = 0;

    /*--ark web()--*/
    virtual void SetFrameRateLinkerEnable(bool enabled) = 0;

    /*--ark web()--*/
    virtual void SetFramePreferredRate(int32_t preferredRate) = 0;

    /*--ark web()--*/
    virtual void SetOnVsyncCallback(OnVsyncCallback callback) = 0;

    /*--ark web()--*/
    virtual void SetOnVsyncEndCallback(OnVsyncCallback onVsyncEndCallback) = 0;

    /*--ark web()--*/
    virtual void SetScene(const ArkWebString& sceneName, uint32_t state) = 0;

    /*--ark web()--*/
    virtual void SetDVSyncSwitch(bool dvsyncSwitch) = 0;
};

/*--ark web(source=webcore)--*/
class ArkBufferRequestConfigAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual int32_t GetStrideAlignment() = 0;

    /*--ark web()--*/
    virtual int32_t GetFormat() = 0;

    /*--ark web()--*/
    virtual uint64_t GetUsage() = 0;

    /*--ark web()--*/
    virtual int32_t GetTimeout() = 0;

    /*--ark web()--*/
    virtual int32_t GetColorGamut() = 0;

    /*--ark web()--*/
    virtual int32_t GetTransformType() = 0;
};

/*--ark web(source=webcore)--*/
class ArkBufferFlushConfigAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetX() = 0;

    /*--ark web()--*/
    virtual int32_t GetY() = 0;

    /*--ark web()--*/
    virtual int32_t GetW() = 0;

    /*--ark web()--*/
    virtual int32_t GetH() = 0;

    /*--ark web()--*/
    virtual int64_t GetTimestamp() = 0;
};

/*--ark web(source=webview)--*/
class ArkSurfaceBufferAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetFileDescriptor() = 0;

    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual int32_t GetStride() = 0;

    /*--ark web()--*/
    virtual int32_t GetFormat() = 0;

    /*--ark web()--*/
    virtual uint32_t GetSize() = 0;

    /*--ark web()--*/
    virtual void* GetVirAddr() = 0;
};

/*--ark web(source=webcore)--*/
class ArkIBufferConsumerListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnBufferAvailable(ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer) = 0;
};

/*--ark web(source=webview)--*/
class ArkIConsumerSurfaceAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t RegisterConsumerListener(ArkWebRefPtr<ArkIBufferConsumerListenerAdapter> listener) = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseBuffer(ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence) = 0;

    /*--ark web()--*/
    virtual int32_t SetUserData(const ArkWebString& key, const ArkWebString& val) = 0;

    /*--ark web()--*/
    virtual int32_t SetQueueSize(uint32_t queueSize) = 0;
};

/*--ark web(source=webview)--*/
class ArkWindowAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void* CreateNativeWindowFromSurface(void* pSurface) = 0;

    /*--ark web()--*/
    virtual void DestroyNativeWindow(void* window) = 0;

    /*--ark web()--*/
    virtual int32_t NativeWindowSetBufferGeometry(void* window, int32_t width, int32_t height) = 0;

    /*--ark web()--*/
    virtual void NativeWindowSurfaceCleanCache(void* window) = 0;

    /*--ark web()--*/
    virtual void NativeWindowSurfaceCleanCacheWithPara(void* window, bool cleanAll) = 0;

    /*--ark web()--*/
    virtual void SetTransformHint(uint32_t rotation, void* window) = 0;

    /*--ark web()--*/
    virtual void AddNativeWindowRef(void* window) = 0;

    /*--ark web()--*/
    virtual void NativeWindowUnRef(void* window) = 0;
};

/*--ark web(source=webview)--*/
class ArkAshmemAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    static int AshmemCreate(const char* name, size_t size);
};

/*--ark web(source=webcore)--*/
class ArkFrameAvailableListener : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void* GetContext() = 0;

    /*--ark web()--*/
    virtual void* GetOnFrameAvailableCb() = 0;
};

/*--ark web(source=webview)--*/
class ArkNativeImageAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void CreateNativeImage(uint32_t textureId, uint32_t textureTarget) = 0;

    /*--ark web()--*/
    virtual void* AquireNativeWindowFromNativeImage() = 0;

    /*--ark web()--*/
    virtual int32_t AttachContext(uint32_t textureId) = 0;

    /*--ark web()--*/
    virtual int32_t DetachContext() = 0;

    /*--ark web()--*/
    virtual int32_t UpdateSurfaceImage() = 0;

    /*--ark web()--*/
    virtual int64_t GetTimestamp() = 0;

    /*--ark web()--*/
    virtual int32_t GetTransformMatrix(float matrix[16]) = 0;

    /*--ark web()--*/
    virtual int32_t GetSurfaceId(uint64_t* surfaceId) = 0;

    /*--ark web()--*/
    virtual int32_t SetOnFrameAvailableListener(ArkWebRefPtr<ArkFrameAvailableListener> listener) = 0;

    /*--ark web()--*/
    virtual int32_t UnsetOnFrameAvailableListener() = 0;

    /*--ark web()--*/
    virtual void DestroyNativeImage() = 0;

    /**
     * @Description: Create a <b>OH_NativeImage</b> as surface consumer.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual void NewNativeImage() = 0;

    /**
     * @Description: Acquire an <b>OHNativeWindowBuffer</b> for content consumer.
     * @Output windowBuffer: Indicates the pointer to an <b>OHNativeWindowBuffer</b> point.
	 * @Output acquireFenceFd: Indicates the pointer to a file descriptor handle.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int32_t AcquireNativeWindowBuffer(
        void** windowBuffer,
        int* acquireFenceFd) = 0;

    /**
     * @Description: Converts an <b>OHNativeWindowBuffer</b> instance to an <b>OH_NativeBuffer</b>.
     * @Input windowBuffer: Indicates the pointer to a <b>OHNativeWindowBuffer</b> instance.
     * @Output nativeBuffer: Indicates the pointer to a <b>OH_NativeBuffer</b> pointer.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int32_t GetNativeBuffer(
        void* windowBuffer,
        void** nativeBuffer) = 0;

    /**
     * @Description: Release the <b>OHNativeWindowBuffer</b> to the buffer queue for reuse.
     * @Input windowBuffer: Indicates the pointer to an <b>OHNativeWindowBuffer</b> instance.
     * @Input fenceFd: Indicates a file descriptor handle, which is used for timing synchronization.
     * @Return：Returns an error code, 0 is sucess, otherwise, failed.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual int32_t ReleaseNativeWindowBuffer(void* windowBuffer, int fenceFd) = 0;

    /**
     * @Description: Get the size of the <b>OHNativeWindowBuffer</b>.
     * @Input windowBuffer: Indicates the pointer to an <b>OHNativeWindowBuffer</b> instance.
     * @Output width: Indicates the width of the window buffer size.
     * @Output height: Indicates the height of the window buffer size.
     * @Since 12005
     */
    /*--ark web()--*/
    virtual void GetNativeWindowBufferSize(
        void* windowBuffer,
        uint32_t* width,
        uint32_t* height) = 0;
};

/*--ark web(source=webview)--*/
class ArkProducerSurfaceAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkSurfaceBufferAdapter> RequestBuffer(
        int32_t& fence, ArkWebRefPtr<ArkBufferRequestConfigAdapter> config) = 0;

    /*--ark web()--*/
    virtual int32_t FlushBuffer(ArkWebRefPtr<ArkSurfaceBufferAdapter> buffer, int32_t fence,
        ArkWebRefPtr<ArkBufferFlushConfigAdapter> config) = 0;
};
} // namespace OHOS::ArkWeb

#endif // Ark_GRAPHIC_ADAPTER_H
