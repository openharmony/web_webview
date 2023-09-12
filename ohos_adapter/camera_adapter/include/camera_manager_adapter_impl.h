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

#include "camera_manager_adapter.h"
#include "camera_manager.h"


#include <cstdio>
#include <fcntl.h>
#include <securec.h>
#include <sys/time.h>
#include <unistd.h>

namespace OHOS::NWeb {
using namespace OHOS::CameraStandard;

enum class SurfaceType {
    INVALID = 0,
    PREVIEW,
    SECOND_PREVIEW,
    PHOTO,
    VIDEO
};

class CameraSurfaceListener;

class CameraSurfaceBufferAdapterImpl : public CameraSurfaceBufferAdapter {
public:
    explicit CameraSurfaceBufferAdapterImpl(sptr<SurfaceBuffer> buffer);

    ~CameraSurfaceBufferAdapterImpl() override = default;

    int32_t GetFileDescriptor() const override;

    int32_t GetWidth() const override;

    int32_t GetHeight() const override;

    int32_t GetStride() const override;

    int32_t GetFormat() const override;

    uint32_t GetSize() const override;

    uint8_t* GetBufferAddr() override;

    sptr<SurfaceBuffer>& GetBuffer();

private:
    sptr<SurfaceBuffer> buffer_ = nullptr;
};

class CameraManagerAdapterCallback : public CameraManagerCallback {
public:
    explicit CameraManagerAdapterCallback(std::shared_ptr<CameraStatusCallbackAdapter> callback);
    ~CameraManagerAdapterCallback() = default;
    void OnCameraStatusChanged(const CameraStatusInfo &cameraStatusInfo) const override;
    void OnFlashlightStatusChanged(const std::string &cameraID,
                                   const FlashStatus flashStatus) const override;

private:
    CameraStatusAdapter GetAdapterCameraStatus(CameraStatus status) const;
    std::shared_ptr<CameraStatusCallbackAdapter> statusCallback_;
};

class CameraManagerAdapterImpl : public CameraManagerAdapter {
public:
    static CameraManagerAdapterImpl& GetInstance();
    CameraManagerAdapterImpl() = default;

    ~CameraManagerAdapterImpl() override = default;

    int32_t Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback) override;

    void GetDevicesInfo(std::vector<VideoDeviceDescriptor> &devicesDiscriptor) override;

    int32_t ReleaseCameraManger() override;

    int32_t GetExposureModes(std::vector<ExposureModeAdapter>& exposureModesAdapter) override;

    int32_t GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter) override;

    int32_t GetCaptionRangeById(RangeIDAdapter rangeId, VideoCaptureRangeAdapter& rangeVal) override;

    bool IsFocusModeSupported(FocusModeAdapter focusMode) override;

    FocusModeAdapter GetCurrentFocusMode() override;

    bool IsFlashModeSupported(FlashModeAdapter flashMode) override;

    int32_t RestartSession() override;

    int32_t StopSession(CameraStopType stopType) override;

    CameraStatusAdapter GetCameraStatus() override;

    bool IsExistCaptureTask() override;

    int32_t StartStream(const std::string &deviceId,
        const VideoCaptureParamsAdapter &captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener) override;

    void SetForegroundFlag(bool isForeground) override;

    void SetCameraStatus(CameraStatusAdapter status) override;

private:
    VideoTransportType GetCameraTransportType(ConnectionType connectType);
    VideoFacingModeAdapter GetCameraFacingMode(CameraPosition position);
    std::vector<FormatAdapter> GetCameraSupportFormats(sptr<CameraOutputCapability> outputcapability);
    VideoPixelFormatAdapter TransToAdapterCameraFormat(CameraFormat format);
    ExposureModeAdapter GetAdapterExposureMode(ExposureMode exportMode);
    CameraFormat TransToOriCameraFormat(VideoPixelFormatAdapter format);
    int32_t TransToAdapterExposureModes(
        std::vector<ExposureMode>& exposureModes,
        std::vector<ExposureModeAdapter>& exposureModesAdapter);
    int32_t GetExposureCompensation(VideoCaptureRangeAdapter& rangeVal);
    FocusMode GetOriFocusMode(FocusModeAdapter focusMode);
    FocusModeAdapter GetAdapterFocusMode(FocusMode focusMode);
    FlashMode GetOriFlashMode(FlashModeAdapter flashMode);
    int32_t ReleaseSession();
    int32_t ReleaseSessionResource(const std::string &deviceId);
    int32_t InitCameraInput(const std::string &deviceId);
    int32_t InitPreviewOutput(const VideoCaptureParamsAdapter &captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener);
    int32_t CreateAndStartSession();
    sptr<CameraManager> cameraManager_;
    sptr<CaptureSession> captureSession_;
    sptr<CaptureInput> cameraInput_;
    sptr<IConsumerSurface> previewSurface_;
    sptr<CameraSurfaceListener> previewSurfaceListener_;
    sptr<CaptureOutput> previewOutput_;
    std::string deviceId_;
    VideoCaptureParamsAdapter captureParams_;
    std::shared_ptr<CameraBufferListenerAdapter> listener_;
    const int32_t DEFAULT_FRAME_RATE = 30;
    const uint32_t RANGE_MAX_SIZE = 2;
    const uint32_t RANGE_MIN_INDEX = 0;
    const uint32_t RANGE_MAX_INDEX = 1;
    CameraStatusAdapter status_ = CameraStatusAdapter::AVAILABLE;
    std::mutex mutex_;
    bool inputInitedFlag_ = false;
    bool isCapturing_ = false;
    bool isForegound_ = false;
    std::mutex restart_mutex_;
    std::shared_ptr<CameraManagerAdapterCallback> cameraMngrCallback_;
};

class CameraSurfaceListener : public IBufferConsumerListener {
public:
    CameraSurfaceListener(SurfaceType surfaceType,
                    sptr<IConsumerSurface> surface,
                    std::shared_ptr<CameraBufferListenerAdapter> listener);
    virtual ~CameraSurfaceListener() = default;
    void OnBufferAvailable() override;

private:
    CameraRotationInfo GetRotationInfo(GraphicTransformType transform);
    CameraRotationInfo FillRotationInfo(int roration, bool isFlipX, bool isFlipY);
    SurfaceType surfaceType_;
    sptr<IConsumerSurface> surface_;
    std::shared_ptr<CameraBufferListenerAdapter> listener_ = nullptr;
    const int32_t ROTATION_0 = 0;
    const int32_t ROTATION_90 = 90;
    const int32_t ROTATION_180 = 180;
    const int32_t ROTATION_270 = 270;
};

class CameraSurfaceAdapterImpl : public CameraSurfaceAdapter {
public:
    CameraSurfaceAdapterImpl();

    CameraSurfaceAdapterImpl(sptr<IConsumerSurface> surface);

    ~CameraSurfaceAdapterImpl() = default;

    int32_t ReleaseBuffer(std::unique_ptr<CameraSurfaceBufferAdapter> bufferAdapter, int32_t fence) override;

private:
    sptr<IConsumerSurface> cSurface_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif // CAMERA_MANAGER_ADAPTER_IMPL_H