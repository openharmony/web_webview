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

#ifndef GRAPHIC_ADAPTER_H
#define GRAPHIC_ADAPTER_H

#include <functional>
#include "adapter_export.h"

namespace OHOS::NWeb {

enum class VSyncErrorCode {
    SUCCESS,
    ERROR,
};

typedef void (*NWebVSyncCb)(int64_t, void*);

class VSyncAdapter {
public:
    VSyncAdapter() = default;

    virtual ~VSyncAdapter() = default;

    virtual VSyncErrorCode RequestVsync(void* data, NWebVSyncCb cb) = 0;

    virtual int64_t GetVSyncPeriod() = 0;

    virtual void SetFrameRateLinkerEnable(bool enabled) = 0;

    virtual void SetFramePreferredRate(int32_t preferredRate) = 0;

    virtual void SetOnVsyncCallback(void (*callback)()) = 0;

    virtual void SetOnVsyncEndCallback(void (*onVsyncEndCallback)()) {}

    virtual void SetDVSyncSwitch(bool dvsyncSwitch) = 0;
};

// be consistent with rom/foundation/graphic/graphic_2d/interfaces/inner_api/common/graphic_common_c.h:GSError
enum GSErrorCode {
    GSERROR_OK = 0,
};

// be consistent with rom/drivers/peripheral/display/interfaces/include/display_type.h:PixelFormat
enum PixelFormatAdapter {
    PIXEL_FMT_CLUT8 = 0,                /**< CLUT8 format */
    PIXEL_FMT_CLUT1,                    /**< CLUT1 format */
    PIXEL_FMT_CLUT4,                    /**< CLUT4 format */
    PIXEL_FMT_RGB_565,                  /**< RGB565 format */
    PIXEL_FMT_RGBA_5658,                /**< RGBA5658 format */
    PIXEL_FMT_RGBX_4444,                /**< RGBX4444 format */
    PIXEL_FMT_RGBA_4444,                /**< RGBA4444 format */
    PIXEL_FMT_RGB_444,                  /**< RGB444 format */
    PIXEL_FMT_RGBX_5551,                /**< RGBX5551 format */
    PIXEL_FMT_RGBA_5551,                /**< RGBA5551 format */
    PIXEL_FMT_RGB_555,                  /**< RGB555 format */
    PIXEL_FMT_RGBX_8888,                /**< RGBX8888 format */
    PIXEL_FMT_RGBA_8888,                /**< RGBA8888 format */
    PIXEL_FMT_RGB_888,                  /**< RGB888 format */
    PIXEL_FMT_BGR_565,                  /**< BGR565 format */
    PIXEL_FMT_BGRX_4444,                /**< BGRX4444 format */
    PIXEL_FMT_BGRA_4444,                /**< BGRA4444 format */
    PIXEL_FMT_BGRX_5551,                /**< BGRX5551 format */
    PIXEL_FMT_BGRA_5551,                /**< BGRA5551 format */
    PIXEL_FMT_BGRX_8888,                /**< BGRX8888 format */
    PIXEL_FMT_BGRA_8888,                /**< BGRA8888 format */
    PIXEL_FMT_YUV_422_I,                /**< YUV422 interleaved format */
    PIXEL_FMT_YCBCR_422_SP,             /**< YCBCR422 semi-planar format */
    PIXEL_FMT_YCRCB_422_SP,             /**< YCRCB422 semi-planar format */
    PIXEL_FMT_YCBCR_420_SP,             /**< YCBCR420 semi-planar format */
    PIXEL_FMT_YCRCB_420_SP,             /**< YCRCB420 semi-planar format */
    PIXEL_FMT_YCBCR_422_P,              /**< YCBCR422 planar format */
    PIXEL_FMT_YCRCB_422_P,              /**< YCRCB422 planar format */
    PIXEL_FMT_YCBCR_420_P,              /**< YCBCR420 planar format */
    PIXEL_FMT_YCRCB_420_P,              /**< YCRCB420 planar format */
    PIXEL_FMT_YUYV_422_PKG,             /**< YUYV422 packed format */
    PIXEL_FMT_UYVY_422_PKG,             /**< UYVY422 packed format */
    PIXEL_FMT_YVYU_422_PKG,             /**< YVYU422 packed format */
    PIXEL_FMT_VYUY_422_PKG,             /**< VYUY422 packed format */
    PIXEL_FMT_VENDER_MASK = 0X7FFF0000, /**< vendor mask format */
    PIXEL_FMT_BUTT = 0X7FFFFFFF         /**< Invalid pixel format */
};

enum class ColorGamutAdapter : int32_t {
    INVALID = -1,        /**< Invalid */
    NATIVE = 0,          /**< Native or default */
    STANDARD_BT601 = 1,  /**< Standard BT601 */
    STANDARD_BT709 = 2,  /**< Standard BT709 */
    DCI_P3 = 3,          /**< DCI P3 */
    SRGB = 4,            /**< SRGB */
    ADOBE_RGB = 5,       /**< Adobe RGB */
    DISPLAY_P3 = 6,      /**< display P3 */
    BT2020 = 7,          /**< BT2020 */
    BT2100_PQ = 8,       /**< BT2100 PQ */
    BT2100_HLG = 9,      /**< BT2100 HLG */
    DISPLAY_BT2020 = 10, /**< Display BT2020 */
};

enum class TransformTypeAdapter : int32_t {
    ROTATE_NONE = 0, /**< No rotation */
    ROTATE_90,       /**< Rotation by 90 degrees */
    ROTATE_180,      /**< Rotation by 180 degrees */
    ROTATE_270,      /**< Rotation by 270 degrees */
    FLIP_H,          /**< Flip horizontally */
    FLIP_V,          /**< Flip vertically */
    FLIP_H_ROT90,    /**< Flip horizontally and rotate 90 degrees */
    FLIP_V_ROT90,    /**< Flip vertically and rotate 90 degrees */
    FLIP_H_ROT180,   /**< Flip horizontally and rotate 180 degrees */
    FLIP_V_ROT180,   /**< Flip vertically and rotate 180 degrees */
    FLIP_H_ROT270,   /**< Flip horizontally and rotate 270 degrees */
    FLIP_V_ROT270,   /**< Flip vertically and rotate 270 degrees */
    ROTATE_BUTT      /**< Invalid operation */
};

class BufferRequestConfigAdapter {
public:
    BufferRequestConfigAdapter() = default;

    virtual ~BufferRequestConfigAdapter() = default;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual int32_t GetStrideAlignment() = 0;

    virtual int32_t GetFormat() = 0;

    virtual uint64_t GetUsage() = 0;

    virtual int32_t GetTimeout() = 0;

    virtual ColorGamutAdapter GetColorGamut() = 0;

    virtual TransformTypeAdapter GetTransformType() = 0;

    virtual int64_t GetTimestamp() = 0;
};

class BufferFlushConfigAdapter {
public:
    BufferFlushConfigAdapter() = default;

    virtual ~BufferFlushConfigAdapter() = default;

    virtual int32_t GetX() = 0;

    virtual int32_t GetY() = 0;

    virtual int32_t GetW() = 0;

    virtual int32_t GetH() = 0;

    virtual int64_t GetTimestamp() = 0;
};

class SurfaceBufferAdapter {
public:
    SurfaceBufferAdapter() = default;

    virtual ~SurfaceBufferAdapter() = default;

    virtual int32_t GetFileDescriptor() = 0;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual int32_t GetStride() = 0;

    virtual int32_t GetFormat() = 0;

    virtual uint32_t GetSize() = 0;

    virtual void* GetVirAddr() = 0;

protected:
    SurfaceBufferAdapter(const SurfaceBufferAdapter&) = delete;

    SurfaceBufferAdapter& operator=(const SurfaceBufferAdapter&) = delete;
};

class IBufferConsumerListenerAdapter {
public:
    virtual ~IBufferConsumerListenerAdapter() = default;

    virtual void OnBufferAvailable(std::shared_ptr<SurfaceBufferAdapter> buffer) = 0;
};

class IConsumerSurfaceAdapter {
public:
    IConsumerSurfaceAdapter() = default;

    virtual ~IConsumerSurfaceAdapter() = default;

    virtual int32_t RegisterConsumerListener(std::shared_ptr<IBufferConsumerListenerAdapter> listener) = 0;

    virtual int32_t ReleaseBuffer(std::shared_ptr<SurfaceBufferAdapter> buffer, int32_t fence) = 0;

    virtual int32_t SetUserData(const std::string& key, const std::string& val) = 0;

    virtual int32_t SetQueueSize(uint32_t queueSize) = 0;
};

using NWebNativeWindow = void*;

class WindowAdapter {
public:
    WindowAdapter() = default;

    virtual ~WindowAdapter() = default;

    virtual NWebNativeWindow CreateNativeWindowFromSurface(void* pSurface) = 0;

    virtual void DestroyNativeWindow(NWebNativeWindow window) = 0;

    virtual int32_t NativeWindowSetBufferGeometry(NWebNativeWindow window, int32_t width, int32_t height) = 0;

    virtual void NativeWindowSurfaceCleanCache(NWebNativeWindow window) = 0;

    virtual void NativeWindowSurfaceCleanCacheWithPara(NWebNativeWindow window, bool cleanAll) = 0;

    virtual void AddNativeWindowRef(NWebNativeWindow window) = 0;

    virtual void NativeWindowUnRef(NWebNativeWindow window) = 0;
};

class  OHOS_ADAPTER_EXPORT AshmemAdapter {
public:
    static int AshmemCreate(const char* name, size_t size);
};

typedef void (*OnFrameAvailableCb)(void* ctx);

class FrameAvailableListener {
public:
    FrameAvailableListener() = default;

    virtual ~FrameAvailableListener() = default;

    virtual void* GetContext() = 0;

    virtual OnFrameAvailableCb GetOnFrameAvailableCb() = 0;
};

class NativeImageAdapter {
public:
    NativeImageAdapter() = default;

    virtual ~NativeImageAdapter() = default;

    virtual void CreateNativeImage(uint32_t textureId, uint32_t textureTarget) = 0;

    virtual NWebNativeWindow AquireNativeWindowFromNativeImage() = 0;

    virtual int32_t AttachContext(uint32_t textureId) = 0;

    virtual int32_t DetachContext() = 0;

    virtual int32_t UpdateSurfaceImage() = 0;

    virtual int64_t GetTimestamp() = 0;

    virtual int32_t GetTransformMatrix(float matrix[16]) = 0;

    virtual int32_t GetSurfaceId(uint64_t* surfaceId) = 0;

    virtual int32_t SetOnFrameAvailableListener(std::shared_ptr<FrameAvailableListener> listener) = 0;

    virtual int32_t UnsetOnFrameAvailableListener() = 0;

    virtual void DestroyNativeImage() = 0;
};

class ProducerSurfaceAdapter {
public:
    ProducerSurfaceAdapter() = default;

    virtual ~ProducerSurfaceAdapter() = default;

    virtual std::shared_ptr<SurfaceBufferAdapter> RequestBuffer(
        int32_t& fence, std::shared_ptr<BufferRequestConfigAdapter> config) = 0;

    virtual int32_t FlushBuffer(std::shared_ptr<SurfaceBufferAdapter> buffer, int32_t fence,
        std::shared_ptr<BufferFlushConfigAdapter> config) = 0;
};

} // namespace OHOS::NWeb

#endif // GRAPHIC_ADAPTER_H
