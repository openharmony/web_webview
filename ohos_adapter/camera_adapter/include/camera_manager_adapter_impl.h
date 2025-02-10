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
#include <securec.h>
#include <sys/time.h>
#include <unistd.h>

#include "camera_manager_adapter.h"

#if defined(NWEB_CAMERA_ENABLE)
#include "camera_manager.h"
#endif

namespace OHOS::NWeb {
#if defined(NWEB_CAMERA_ENABLE)
using namespace OHOS::CameraStandard;

enum class SurfaceType { INVALID = 0, PREVIEW, SECOND_PREVIEW, PHOTO, VIDEO };

class CameraSurfaceListener;

class CameraSurfaceBufferAdapterImpl : public CameraSurfaceBufferAdapter {
public:
    explicit CameraSurfaceBufferAdapterImpl(sptr<SurfaceBuffer> buffer);

    ~CameraSurfaceBufferAdapterImpl() override = default;

    int32_t GetFileDescriptor() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    int32_t GetStride() override;

    int32_t GetFormat() override;

    uint32_t GetSize() override;

    uint8_t* GetBufferAddr() override;

    sptr<SurfaceBuffer>& GetBuffer();

private:
    sptr<SurfaceBuffer> buffer_ = nullptr;
};

class CameraManagerAdapterCallback : public CameraManagerCallback {
public:
    explicit CameraManagerAdapterCallback(std::shared_ptr<CameraStatusCallbackAdapter> callback);
    ~CameraManagerAdapterCallback() = default;
    void OnCameraStatusChanged(const CameraStatusInfo& cameraStatusInfo) const override;
    void OnFlashlightStatusChanged(const std::string& cameraID, const FlashStatus flashStatus) const override;

private:
    CameraStatusAdapter GetAdapterCameraStatus(CameraStatus status) const;
    std::shared_ptr<CameraStatusCallbackAdapter> statusCallback_;
};
#endif

class CameraManagerAdapterImpl : public CameraManagerAdapter {
public:
    static CameraManagerAdapterImpl& GetInstance();
    CameraManagerAdapterImpl() = default;

    ~CameraManagerAdapterImpl() override = default;

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

#if defined(NWEB_CAMERA_ENABLE)
private:
    VideoTransportType GetCameraTransportType(ConnectionType connectType);
    VideoFacingModeAdapter GetCameraFacingMode(CameraPosition position);
    std::vector<std::shared_ptr<FormatAdapter>> GetCameraSupportFormats(sptr<CameraOutputCapability> outputcapability);
    VideoPixelFormatAdapter TransToAdapterCameraFormat(CameraFormat format);
    ExposureModeAdapter GetAdapterExposureMode(ExposureMode exportMode);
    CameraFormat TransToOriCameraFormat(VideoPixelFormatAdapter format);
    int32_t TransToAdapterExposureModes(
        std::vector<ExposureMode>& exposureModes, std::vector<ExposureModeAdapter>& exposureModesAdapter);
    std::shared_ptr<VideoCaptureRangeAdapter> GetExposureCompensation();
    FocusMode GetOriFocusMode(FocusModeAdapter focusMode);
    FocusModeAdapter GetAdapterFocusMode(FocusMode focusMode);
    FlashMode GetOriFlashMode(FlashModeAdapter flashMode);
    int32_t ReleaseSession();
    int32_t ReleaseSessionResource(const std::string& deviceId);
    int32_t InitCameraInput(const std::string& deviceId);
    int32_t InitPreviewOutput(const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener);
    int32_t CreateAndStartSession();
    int32_t ErrorTypeToString(CameraErrorType errorType, std::string& errnoTypeString);
    void ReportErrorSysEvent(CameraErrorType errorType);
    int32_t StartStreamInner(const std::string& deviceId,
        const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<CameraBufferListenerAdapter> listener);
    std::string GetCameraDisplayName(const std::string& cameraId, const CameraPosition& position);
    sptr<CameraManager> cameraManager_;
    sptr<CaptureSession> captureSession_;
    sptr<CaptureInput> cameraInput_;
    sptr<IConsumerSurface> previewSurface_;
    sptr<CameraSurfaceListener> previewSurfaceListener_;
    sptr<CaptureOutput> previewOutput_;
    std::string deviceId_;
    std::shared_ptr<VideoCaptureParamsAdapter> captureParams_;
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
    std::shared_ptr<CameraManagerAdapterCallback> cameraMngrCallback_;
    std::string wantedDeviceId_;
#endif
};

#if defined(NWEB_CAMERA_ENABLE)
class CameraSurfaceListener : public IBufferConsumerListener {
public:
    CameraSurfaceListener(
        SurfaceType surfaceType, sptr<IConsumerSurface> surface, std::shared_ptr<CameraBufferListenerAdapter> listener);
    virtual ~CameraSurfaceListener() = default;
    void OnBufferAvailable() override;

private:
    std::shared_ptr<CameraRotationInfoAdapter> GetRotationInfo(GraphicTransformType transform);
    std::shared_ptr<CameraRotationInfoAdapter> FillRotationInfo(int32_t rotation, bool isFlipX, bool isFlipY);
    int32_t GetScreenRotation();
    int32_t GetPictureRotation();
    bool IsNeedCorrectRotation();
    SurfaceType surfaceType_;
    sptr<IConsumerSurface> surface_;
    std::shared_ptr<CameraBufferListenerAdapter> listener_ = nullptr;
    const int32_t ROTATION_0 = 0;
    const int32_t ROTATION_90 = 90;
    const int32_t ROTATION_180 = 180;
    const int32_t ROTATION_270 = 270;
    const int32_t ROTATION_MAX = 360;
};

class CameraSurfaceAdapterImpl : public CameraSurfaceAdapter {
public:
    CameraSurfaceAdapterImpl();

    CameraSurfaceAdapterImpl(sptr<IConsumerSurface> surface);

    ~CameraSurfaceAdapterImpl() = default;

    int32_t ReleaseBuffer(std::shared_ptr<CameraSurfaceBufferAdapter> bufferAdapter, int32_t fence) override;

private:
    sptr<IConsumerSurface> cSurface_ = nullptr;
};
#endif
} // namespace OHOS::NWeb
#endif // CAMERA_MANAGER_ADAPTER_IMPL_H
