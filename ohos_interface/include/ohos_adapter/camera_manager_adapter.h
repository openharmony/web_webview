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

enum class CameraErrorType : int32_t {
    CAMERA_NOT_CLOSE,
    INPUT_ALREADY_INIT,
    CAMERA_MANAGER_IS_NULL,
    GET_CAMERA_OBJ_FAILED,
    CREATE_INPUT_FAILED,
    INPUT_OPEN_FAILED,
    INPUT_NOT_INIT,
    CREATE_PREVIEW_SURFACE_FAILED,
    CREATE_PREVIEW_OUTPUT_FAILED,
    CREATE_CAPTURE_SESSION_FAILED,
    ADD_INPUT_FAILED,
    ADD_OUTPUT_FAILED,
    COMMIT_CONFIG_FAILED,
    START_SESSION_FAILED,
    INPUT_OR_OUTPUT_IS_NULL,
};

enum class VideoTransportType : int32_t {
    VIDEO_TRANS_TYPE_BUILD_IN,
    VIDEO_TRANS_TYPE_USB,
    VIDEO_TRANS_TYPE_REMOTE,
    VIDEO_TRANS_TYPE_OTHER
};

enum class VideoFacingModeAdapter : int32_t { FACING_NONE = 0, FACING_USER, FACING_ENVIRONMENT, NUM_FACING_MODES };

enum class VideoPixelFormatAdapter : int32_t {
    FORMAT_RGBA_8888,
    FORMAT_YCBCR_420_888,
    FORMAT_YUV_420_SP,
    FORMAT_JPEG,
    FORMAT_UNKNOWN
};

enum class ExposureModeAdapter : int32_t {
    EXPOSURE_MODE_UNSUPPORTED = -1,
    EXPOSURE_MODE_LOCKED = 0,
    EXPOSURE_MODE_AUTO,
    EXPOSURE_MODE_CONTINUOUS_AUTO
};

enum class FocusModeAdapter : int32_t {
    FOCUS_MODE_MANUAL = 0,
    FOCUS_MODE_CONTINUOUS_AUTO,
    FOCUS_MODE_AUTO,
    FOCUS_MODE_LOCKED,
};

enum class FlashModeAdapter : int32_t {
    FLASH_MODE_CLOSE = 0,
    FLASH_MODE_OPEN,
    FLASH_MODE_AUTO,
    FLASH_MODE_ALWAYS_OPEN,
};

enum class RangeIDAdapter : int32_t {
    RANGE_ID_EXP_COMPENSATION,
};

enum class CameraStopType : int32_t { TO_BACK = 0, NORMAL };

enum class CameraStatusAdapter : int32_t {
    APPEAR = 0,
    DISAPPEAR,
    AVAILABLE,
    UNAVAILABLE,
};

enum class FlashStatusAdapter : int32_t {
    OFF = 0,
    ON,
    UNAVAILABLE,
};

enum CameraManagerAdapterCode : int32_t {
    CAMERA_OK = 0,
    CAMERA_ERROR = -1,
    CAMERA_NULL_ERROR = -2,
};

class VideoControlSupportAdapter {
public:
    VideoControlSupportAdapter() = default;

    virtual ~VideoControlSupportAdapter() = default;

    virtual bool GetPan() = 0;

    virtual bool GetTilt() = 0;

    virtual bool GetZoom() = 0;
};

class FormatAdapter {
public:
    FormatAdapter() = default;

    virtual ~FormatAdapter() = default;

    virtual uint32_t GetWidth() = 0;

    virtual uint32_t GetHeight() = 0;

    virtual float GetFrameRate() = 0;

    virtual VideoPixelFormatAdapter GetPixelFormat() = 0;
};

class VideoDeviceDescriptorAdapter {
public:
    VideoDeviceDescriptorAdapter() = default;

    virtual ~VideoDeviceDescriptorAdapter() = default;

    virtual std::string GetDisplayName() = 0;

    virtual std::string GetDeviceId() = 0;

    virtual std::string GetModelId() = 0;

    virtual std::shared_ptr<VideoControlSupportAdapter> GetControlSupport() = 0;

    virtual VideoTransportType GetTransportType() = 0;

    virtual VideoFacingModeAdapter GetFacingMode() = 0;

    virtual std::vector<std::shared_ptr<FormatAdapter>> GetSupportCaptureFormats() = 0;
};

class VideoCaptureParamsAdapter {
public:
    VideoCaptureParamsAdapter() = default;

    virtual ~VideoCaptureParamsAdapter() = default;

    virtual uint32_t GetWidth() = 0;

    virtual uint32_t GetHeight() = 0;

    virtual float GetFrameRate() = 0;

    virtual VideoPixelFormatAdapter GetPixelFormat() = 0;

    virtual bool GetEnableFaceDetection() = 0;
};

class VideoCaptureRangeAdapter {
public:
    VideoCaptureRangeAdapter() = default;

    virtual ~VideoCaptureRangeAdapter() = default;

    virtual double GetMin() = 0;

    virtual double GetMax() = 0;

    virtual double GetStep() = 0;

    virtual double GetCurrent() = 0;
};

class CameraRotationInfoAdapter {
public:
    CameraRotationInfoAdapter() = default;

    virtual ~CameraRotationInfoAdapter() = default;

    virtual int32_t GetRotation() = 0;

    virtual bool GetIsFlipX() = 0;

    virtual bool GetIsFlipY() = 0;
};

class CameraSurfaceBufferAdapter {
public:
    CameraSurfaceBufferAdapter() = default;

    virtual ~CameraSurfaceBufferAdapter() = default;

    virtual int32_t GetFileDescriptor() = 0;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual int32_t GetStride() = 0;

    virtual int32_t GetFormat() = 0;

    virtual uint32_t GetSize() = 0;

    virtual uint8_t* GetBufferAddr() = 0;

protected:
    CameraSurfaceBufferAdapter(const CameraSurfaceBufferAdapter&) = delete;

    CameraSurfaceBufferAdapter& operator=(const CameraSurfaceBufferAdapter&) = delete;
};

class CameraSurfaceAdapter {
public:
    CameraSurfaceAdapter() = default;

    virtual ~CameraSurfaceAdapter() = default;

    virtual int32_t ReleaseBuffer(std::shared_ptr<CameraSurfaceBufferAdapter> buffer, int32_t fence) = 0;
};

class CameraBufferListenerAdapter {
public:
    virtual ~CameraBufferListenerAdapter() = default;

    virtual void OnBufferAvailable(std::shared_ptr<CameraSurfaceAdapter> surface,
        std::shared_ptr<CameraSurfaceBufferAdapter> buffer,
        std::shared_ptr<CameraRotationInfoAdapter> rotationInfo) = 0;
};

class CameraStatusCallbackAdapter {
public:
    virtual ~CameraStatusCallbackAdapter() = default;

    virtual void OnCameraStatusChanged(CameraStatusAdapter cameraStatusAdapter, const std::string callBackDeviceId) = 0;

    virtual void OnFlashlightStatusChanged(FlashStatusAdapter flashStatusAdapter, const std::string cameraID) = 0;
};

class CameraManagerAdapter {
public:
    CameraManagerAdapter() = default;

    virtual ~CameraManagerAdapter() = default;

    virtual int32_t Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback) = 0;

    virtual std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> GetDevicesInfo() = 0;

    virtual int32_t ReleaseCameraManger() = 0;

    virtual int32_t GetExposureModes(std::vector<ExposureModeAdapter>& exposureModesAdapter) = 0;

    virtual int32_t GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter) = 0;

    virtual std::shared_ptr<VideoCaptureRangeAdapter> GetCaptionRangeById(RangeIDAdapter rangeId) = 0;

    virtual bool IsFocusModeSupported(FocusModeAdapter focusMode) = 0;

    virtual FocusModeAdapter GetCurrentFocusMode() = 0;

    virtual bool IsFlashModeSupported(FlashModeAdapter flashMode) = 0;

    virtual int32_t GetSupportedFlashModes(std::vector<FlashModeAdapter>& flashModesAdapter) = 0;

    virtual bool HasFlash() = 0;

    virtual int32_t GetCurrentFlashMode(FlashModeAdapter& flashModeAdapter) = 0;

    virtual int32_t SetFlashMode(FlashModeAdapter flashMode) = 0;

    virtual int32_t RestartSession() = 0;

    virtual int32_t StopSession(CameraStopType stopType) = 0;

    virtual CameraStatusAdapter GetCameraStatus() = 0;

    virtual bool IsExistCaptureTask() = 0;

    virtual int32_t StartStream(const std::string& deviceId,
        const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener) = 0;

    virtual void SetForegroundFlag(bool isForeground) = 0;

    virtual void SetCameraStatus(CameraStatusAdapter status) = 0;

    virtual std::string GetCurrentDeviceId() = 0;
};

} // namespace OHOS::NWeb

#endif // CAMERA_MANAGER_ADAPTER_H
