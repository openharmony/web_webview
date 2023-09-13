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

#ifndef CAMERA_MANAGER_ADAPTER_H
#define CAMERA_MANAGER_ADAPTER_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::NWeb {

typedef struct VideoControlSupportTag {
    bool pan = false;
    bool tilt = false;
    bool zoom = false;
} VideoControlSupport;

enum class VideoTransportType {
    VIDEO_TRANS_TYPE_BUILD_IN,
    VIDEO_TRANS_TYPE_USB,
    VIDEO_TRANS_TYPE_REMOTE,
    VIDEO_TRANS_TYPE_OTHER
};

enum class VideoFacingModeAdapter {
    FACING_NONE = 0,
    FACING_USER,
    FACING_ENVIRONMENT,

    NUM_FACING_MODES
};

enum class VideoPixelFormatAdapter {
    FORMAT_RGBA_8888,
    FORMAT_YCBCR_420_888,
    FORMAT_YUV_420_SP,
    FORMAT_JPEG,
    FORMAT_UNKNOWN
};

enum class ExposureModeAdapter {
    EXPOSURE_MODE_UNSUPPORTED = -1,
    EXPOSURE_MODE_LOCKED = 0,
    EXPOSURE_MODE_AUTO,
    EXPOSURE_MODE_CONTINUOUS_AUTO
};

enum class FocusModeAdapter {
    FOCUS_MODE_MANUAL = 0,
    FOCUS_MODE_CONTINUOUS_AUTO,
    FOCUS_MODE_AUTO,
    FOCUS_MODE_LOCKED,
};

enum class FlashModeAdapter {
    FLASH_MODE_CLOSE = 0,
    FLASH_MODE_OPEN,
    FLASH_MODE_AUTO,
    FLASH_MODE_ALWAYS_OPEN,
};

enum class RangeIDAdapter {
    RANGE_ID_EXP_COMPENSATION,
};

enum class CameraStopType {
    TO_BACK = 0,
    NORMAL
};

typedef struct FormatAdapterTag {
    uint32_t width;
    uint32_t height;
    float frameRate;
    VideoPixelFormatAdapter pixelFormat;
} FormatAdapter;

enum CameraStatusAdapter {
    APPEAR = 0,
    DISAPPEAR,
    AVAILABLE,
    UNAVAILABLE
};

typedef struct VideoDeviceDescriptorTag {
    std::string displayName;
    std::string deviceId;
    std::string modelId;
    VideoControlSupport controlSupport;
    VideoTransportType transportType =
        VideoTransportType::VIDEO_TRANS_TYPE_OTHER;
    VideoFacingModeAdapter facing = VideoFacingModeAdapter::FACING_NONE;
    std::vector<FormatAdapter> supportCaptureFormats;
} VideoDeviceDescriptor;
typedef struct VideoCaptureParamsAdapterTag {
    FormatAdapter captureFormat;
    bool enableFaceDetection;
} VideoCaptureParamsAdapter;

typedef struct VideoCaptureRangeAdapterTag {
    double min;
    double max;
    double step;
    double current;
} VideoCaptureRangeAdapter;

enum CameraManagerAdapterCode : int32_t {
    CAMERA_OK = 0,
    CAMERA_ERROR = -1,
    CAMERA_NULL_ERROR = -2,
};

typedef struct CameraRotationInfoTag {
    int32_t rotation;
    bool isFlipX;
    bool isFlipY;
} CameraRotationInfo;

class CameraSurfaceBufferAdapter {
public:
    CameraSurfaceBufferAdapter() = default;

    virtual ~CameraSurfaceBufferAdapter() = default;

    virtual int32_t GetFileDescriptor() const = 0;

    virtual int32_t GetWidth() const = 0;

    virtual int32_t GetHeight() const = 0;

    virtual int32_t GetStride() const = 0;

    virtual int32_t GetFormat() const = 0;

    virtual uint32_t GetSize() const = 0;

    virtual uint8_t* GetBufferAddr() = 0;

protected:
    CameraSurfaceBufferAdapter(const CameraSurfaceBufferAdapter&) = delete;

    CameraSurfaceBufferAdapter& operator=(const CameraSurfaceBufferAdapter&) = delete;
};

class CameraSurfaceAdapter {
public:
    CameraSurfaceAdapter() = default;

    virtual ~CameraSurfaceAdapter() = default;

    virtual int32_t ReleaseBuffer(std::unique_ptr<CameraSurfaceBufferAdapter> buffer, int32_t fence) = 0;
};

class CameraBufferListenerAdapter {
public:
    virtual ~CameraBufferListenerAdapter() = default;

    virtual void OnBufferAvailable(std::shared_ptr<CameraSurfaceAdapter> surface,
        std::unique_ptr<CameraSurfaceBufferAdapter> buffer,
        CameraRotationInfo rotationInfo) = 0;
};

class CameraStatusCallbackAdapter {
public:
    virtual ~CameraStatusCallbackAdapter() = default;

    virtual void OnCameraStatusChanged(CameraStatusAdapter cameraStatusAdapter) = 0;
};

class CameraManagerAdapter {
public:
    CameraManagerAdapter() = default;

    virtual ~CameraManagerAdapter() = default;

    virtual int32_t Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback) = 0;

    virtual void GetDevicesInfo(std::vector<VideoDeviceDescriptor> &devicesDiscriptor) = 0;

    virtual int32_t ReleaseCameraManger() = 0;

    virtual int32_t GetExposureModes(std::vector<ExposureModeAdapter>& exposureModesAdapter) = 0;

    virtual int32_t GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter) = 0;

    virtual int32_t GetCaptionRangeById(RangeIDAdapter rangeId, VideoCaptureRangeAdapter& rangeVal) = 0;

    virtual bool IsFocusModeSupported(FocusModeAdapter focusMode);

    virtual FocusModeAdapter GetCurrentFocusMode() = 0;

    virtual bool IsFlashModeSupported(FlashModeAdapter flashMode) = 0;

    virtual int32_t RestartSession() = 0;

    virtual int32_t StopSession(CameraStopType stopType) = 0;

    virtual CameraStatusAdapter GetCameraStatus() = 0;

    virtual bool IsExistCaptureTask() = 0;

    virtual int32_t StartStream(const std::string &deviceId,
        const VideoCaptureParamsAdapter &captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener) = 0;

    virtual void SetForegroundFlag(bool isForeground) = 0;

    virtual void SetCameraStatus(CameraStatusAdapter status) = 0;
};
} // namespace OHOS::NWeb

#endif // CAMERA_MANAGER_ADAPTER_H