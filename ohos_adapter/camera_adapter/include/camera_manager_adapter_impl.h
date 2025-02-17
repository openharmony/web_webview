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

#ifndef CAMERA_MANAGER_ADAPTER_IMPL_H
#define CAMERA_MANAGER_ADAPTER_IMPL_H

#include <cstdio>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <mutex>

#include "camera_manager_adapter.h"
#include "video_device_descriptor_adapter_impl.h"

#include <native_image/native_image.h>
#include <ohcamera/camera_manager.h>
#include <ohcamera/camera.h>


namespace OHOS::NWeb {

class CameraSurfaceBufferAdapterImpl : public CameraSurfaceBufferAdapter {
public:
    explicit CameraSurfaceBufferAdapterImpl(OHNativeWindowBuffer* windowBuffer, int32_t fence);

    ~CameraSurfaceBufferAdapterImpl() override;

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    uint8_t* GetBufferAddr() override;

    OHNativeWindowBuffer* GetWindowBuffer();

    int32_t GetFenceFd();

    int32_t ClearWindowBuffer();

private:
    OHNativeWindowBuffer* windowBuffer_ = nullptr;
    int32_t fenceFd_;
};

class CameraSurfaceAdapterImpl : public CameraSurfaceAdapter {
public:
    CameraSurfaceAdapterImpl() = default;
    ~CameraSurfaceAdapterImpl() = default;

    int32_t ReleaseBuffer(std::shared_ptr<CameraSurfaceBufferAdapter> bufferAdapter, int32_t fence) override;
};

class CameraManagerAdapterImpl : public CameraManagerAdapter {
public:
    static CameraManagerAdapterImpl& GetInstance();
    CameraManagerAdapterImpl() = default;

    ~CameraManagerAdapterImpl() override;

    int32_t Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback) override;

    std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> GetDevicesInfo() override;

    int32_t ReleaseCameraManger() override;

    int32_t GetExposureModes(std::vector<ExposureModeAdapter>& exposureModesAdapter) override;

    int32_t GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter) override;

    std::shared_ptr<VideoCaptureRangeAdapter> GetCaptionRangeById(RangeIDAdapter rangeId) override;

    bool IsFocusModeSupported(FocusModeAdapter focusMode) override;

    FocusModeAdapter GetCurrentFocusMode() override;

    bool IsFlashModeSupported(FlashModeAdapter flashMode) override;

    int32_t RestartSession() override;

    int32_t StopSession(CameraStopType stopType) override;

    CameraStatusAdapter GetCameraStatus() override;

    bool IsExistCaptureTask() override;

    int32_t StartStream(const std::string& deviceId, const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener) override;

    void SetForegroundFlag(bool isForeground) override;

    void SetCameraStatus(CameraStatusAdapter status) override;

    std::string GetCurrentDeviceId() override;

    OH_NativeImage* GetNativeImage();

    std::shared_ptr<CameraBufferListenerAdapter> GetBufferListener();

    std::shared_ptr<CameraStatusCallbackAdapter> GetStatusCallback();

    std::shared_ptr<CameraRotationInfoAdapter> GetRotationInfo();
private:
    VideoTransportType GetCameraTransportType(Camera_Connection connectType);
    VideoFacingModeAdapter GetCameraFacingMode(Camera_Position position);
    std::vector<std::shared_ptr<FormatAdapter>> GetCameraSupportFormats(Camera_OutputCapability *outputcapability);
    VideoPixelFormatAdapter TransToAdapterCameraFormat(Camera_Format format);
    ExposureModeAdapter GetAdapterExposureMode(Camera_ExposureMode exportMode);
    int32_t GetCameraDeviceFromId(const std::string& deviceId, Camera_Device *camera);
    Camera_Format TransToOriCameraFormat(VideoPixelFormatAdapter format);
    int32_t TransToAdapterExposureModes(
        std::vector<Camera_ExposureMode>& exposureModes, std::vector<ExposureModeAdapter>& exposureModesAdapter);
    std::shared_ptr<VideoCaptureRangeAdapter> GetExposureCompensation();
    Camera_FocusMode GetOriFocusMode(FocusModeAdapter focusMode);
    FocusModeAdapter GetAdapterFocusMode(Camera_FocusMode focusMode);
    Camera_FlashMode GetOriFlashMode(FlashModeAdapter flashMode);
    int32_t ReleaseSession();
    int32_t ReleaseSessionResource(const std::string& deviceId);
    int32_t DestroyNativeImageAndWindow();
    int32_t InitCameraInput(const std::string& deviceId);
    int32_t InitPreviewOutput(const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener);
    int32_t CreateAndStartSession();
    int32_t ErrorTypeToString(CameraErrorType errorType, std::string& errnoTypeString);
    void ReportErrorSysEvent(CameraErrorType errorType);
    bool HandleDeviceDisc(std::shared_ptr<VideoDeviceDescriptorAdapterImpl> deviceDisc,
        Camera_Device& camera, Camera_OutputCapability *outputCapability);
    int32_t RecordCameraInfo(Camera_Device &camera);
    int32_t StartStreamInner(const std::string& deviceId,
        const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener);
    Camera_Manager *cameraManager_ = nullptr;
    Camera_CaptureSession *captureSession_ = nullptr;
    Camera_Input *cameraInput_ = nullptr;
    Camera_PreviewOutput *previewOutput_ = nullptr;
    std::string deviceId_;
    std::shared_ptr<VideoCaptureParamsAdapter> captureParams_;
    const int32_t DEFAULT_FRAME_RATE = 30;
    CameraStatusAdapter status_ = CameraStatusAdapter::AVAILABLE;
    std::mutex mutex_;
    bool inputInitedFlag_ = false;
    bool isCapturing_ = false;
    bool isForegound_ = false;
    CameraManager_Callbacks cameraManagerCallback_;
    std::string wantedDeviceId_;
    OH_NativeImage *nativeImage_ = nullptr;
    std::shared_ptr<CameraBufferListenerAdapter> bufferListener_ = nullptr;
    std::shared_ptr<CameraStatusCallbackAdapter> statusCallback_ = nullptr;
    uint32_t cameraOrientation_ = 0;
    Camera_Position cameraPosition_ = CAMERA_POSITION_UNSPECIFIED;
};


} // namespace OHOS::NWeb
#endif // CAMERA_MANAGER_ADAPTER_IMPL_H
