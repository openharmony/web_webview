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

#include "camera_manager_adapter_impl.h"

#include <unordered_map>

#include "nweb_log.h"

namespace OHOS::NWeb {
using namespace OHOS::CameraStandard;
const std::unordered_map<ConnectionType, VideoTransportType> TRANS_TYPE_MAP = {
    {CAMERA_CONNECTION_BUILT_IN, VideoTransportType::VIDEO_TRANS_TYPE_BUILD_IN},
    {CAMERA_CONNECTION_USB_PLUGIN, VideoTransportType::VIDEO_TRANS_TYPE_USB},
    {CAMERA_CONNECTION_REMOTE, VideoTransportType::VIDEO_TRANS_TYPE_REMOTE},
};

const std::unordered_map<CameraPosition, VideoFacingModeAdapter> FACING_MODE_MAP = {
    {CAMERA_POSITION_UNSPECIFIED, VideoFacingModeAdapter::FACING_NONE},
    {CAMERA_POSITION_FRONT, VideoFacingModeAdapter::FACING_USER},
    {CAMERA_POSITION_BACK, VideoFacingModeAdapter::FACING_ENVIRONMENT},
};

const std::unordered_map<CameraFormat, VideoPixelFormatAdapter> TO_ADAPTER_PIXEL_FORMAT_MAP = {
    {CAMERA_FORMAT_RGBA_8888, VideoPixelFormatAdapter::FORMAT_RGBA_8888},
    {CAMERA_FORMAT_YCBCR_420_888, VideoPixelFormatAdapter::FORMAT_YCBCR_420_888},
    {CAMERA_FORMAT_YUV_420_SP, VideoPixelFormatAdapter::FORMAT_YUV_420_SP},
    {CAMERA_FORMAT_JPEG, VideoPixelFormatAdapter::FORMAT_JPEG},
    {CAMERA_FORMAT_INVALID, VideoPixelFormatAdapter::FORMAT_UNKNOWN},
};

const std::unordered_map<VideoPixelFormatAdapter, CameraFormat> TO_OHOS_PIXEL_FORMAT_MAP = {
    {VideoPixelFormatAdapter::FORMAT_RGBA_8888, CAMERA_FORMAT_RGBA_8888},
    {VideoPixelFormatAdapter::FORMAT_YCBCR_420_888, CAMERA_FORMAT_YCBCR_420_888},
    {VideoPixelFormatAdapter::FORMAT_YUV_420_SP, CAMERA_FORMAT_YUV_420_SP},
    {VideoPixelFormatAdapter::FORMAT_JPEG, CAMERA_FORMAT_JPEG},
    {VideoPixelFormatAdapter::FORMAT_UNKNOWN, CAMERA_FORMAT_INVALID},
};

const std::unordered_map<ExposureMode, ExposureModeAdapter> EXPOSURE_MODE_MAP = {
    {EXPOSURE_MODE_UNSUPPORTED, ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED},
    {EXPOSURE_MODE_LOCKED, ExposureModeAdapter::EXPOSURE_MODE_LOCKED},
    {EXPOSURE_MODE_AUTO, ExposureModeAdapter::EXPOSURE_MODE_AUTO},
    {EXPOSURE_MODE_CONTINUOUS_AUTO, ExposureModeAdapter::EXPOSURE_MODE_CONTINUOUS_AUTO},
};

const std::unordered_map<FocusModeAdapter, FocusMode> FOCUS_MODE_MAP = {
    {FocusModeAdapter::FOCUS_MODE_MANUAL, FOCUS_MODE_MANUAL},
    {FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO, FOCUS_MODE_CONTINUOUS_AUTO},
    {FocusModeAdapter::FOCUS_MODE_AUTO, FOCUS_MODE_AUTO},
    {FocusModeAdapter::FOCUS_MODE_LOCKED, FOCUS_MODE_LOCKED},
};

const std::unordered_map<FocusMode, FocusModeAdapter> ADAPTER_FOCUS_MODE_MAP = {
    {FOCUS_MODE_MANUAL, FocusModeAdapter::FOCUS_MODE_MANUAL},
    {FOCUS_MODE_CONTINUOUS_AUTO, FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO},
    {FOCUS_MODE_AUTO, FocusModeAdapter::FOCUS_MODE_AUTO},
    {FOCUS_MODE_LOCKED, FocusModeAdapter::FOCUS_MODE_LOCKED},
};

const std::unordered_map<FlashModeAdapter, FlashMode> FLASH_MODE_MAP = {
    {FlashModeAdapter::FLASH_MODE_CLOSE, FLASH_MODE_CLOSE},
    {FlashModeAdapter::FLASH_MODE_OPEN, FLASH_MODE_OPEN},
    {FlashModeAdapter::FLASH_MODE_AUTO, FLASH_MODE_AUTO},
    {FlashModeAdapter::FLASH_MODE_ALWAYS_OPEN, FLASH_MODE_ALWAYS_OPEN},
};

const std::unordered_map<CameraStatus, CameraStatusAdapter> CAMERA_STATUS_MAP = {
    {CAMERA_STATUS_APPEAR, CameraStatusAdapter::APPEAR},
    {CAMERA_STATUS_DISAPPEAR, CameraStatusAdapter::DISAPPEAR},
    {CAMERA_STATUS_AVAILABLE, CameraStatusAdapter::AVAILABLE},
    {CAMERA_STATUS_UNAVAILABLE, CameraStatusAdapter::UNAVAILABLE},
};

VideoTransportType CameraManagerAdapterImpl::GetCameraTransportType(ConnectionType connectType)
{
    auto item = TRANS_TYPE_MAP.find(connectType);
    if (item == TRANS_TYPE_MAP.end()) {
        WVLOG_E("concect type %{public}d not found", connectType);
        return VideoTransportType::VIDEO_TRANS_TYPE_OTHER;
    }
    return item->second;
}

VideoFacingModeAdapter CameraManagerAdapterImpl::GetCameraFacingMode(CameraPosition position)
{
    auto item = FACING_MODE_MAP.find(position);
    if (item == FACING_MODE_MAP.end()) {
        WVLOG_E("position type %{public}d not found", position);
        return VideoFacingModeAdapter::FACING_NONE;
    }
    return item->second;
}

VideoPixelFormatAdapter CameraManagerAdapterImpl::TransToAdapterCameraFormat(CameraFormat format)
{
    auto item = TO_ADAPTER_PIXEL_FORMAT_MAP.find(format);
    if (item == TO_ADAPTER_PIXEL_FORMAT_MAP.end()) {
        WVLOG_E("to adapter pixel format type %{public}d not found", format);
        return VideoPixelFormatAdapter::FORMAT_UNKNOWN;
    }
    return item->second;
}

CameraFormat CameraManagerAdapterImpl::TransToOriCameraFormat(VideoPixelFormatAdapter format)
{
    auto item = TO_OHOS_PIXEL_FORMAT_MAP.find(format);
    if (item == TO_OHOS_PIXEL_FORMAT_MAP.end()) {
        WVLOG_E("to pixel format type %{public}d not found", format);
        return CAMERA_FORMAT_INVALID;
    }
    return item->second;
}

ExposureModeAdapter CameraManagerAdapterImpl::GetAdapterExposureMode(ExposureMode exportMode)
{
    auto item = EXPOSURE_MODE_MAP.find(exportMode);
    if (item == EXPOSURE_MODE_MAP.end()) {
        WVLOG_E("to exposure mode %{public}d not found", exportMode);
        return ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED;
    }
    return item->second;
}

FocusMode CameraManagerAdapterImpl::GetOriFocusMode(FocusModeAdapter focusMode)
{
    auto item = FOCUS_MODE_MAP.find(focusMode);
    if (item == FOCUS_MODE_MAP.end()) {
        WVLOG_E("adapter focus mode %{public}d not found", focusMode);
        return FOCUS_MODE_MANUAL;
    }
    return item->second;
}

FlashMode CameraManagerAdapterImpl::GetOriFlashMode(FlashModeAdapter flashMode)
{
    auto item = FLASH_MODE_MAP.find(flashMode);
    if (item == FLASH_MODE_MAP.end()) {
        WVLOG_E("adapter flash mode %{public}d not found", flashMode);
        return FLASH_MODE_CLOSE;
    }
    return item->second;
}

FocusModeAdapter CameraManagerAdapterImpl::GetAdapterFocusMode(FocusMode focusMode)
{
    auto item = ADAPTER_FOCUS_MODE_MAP.find(focusMode);
    if (item == ADAPTER_FOCUS_MODE_MAP.end()) {
        WVLOG_E("ori focus mode %{public}d not found", focusMode);
        return FocusModeAdapter::FOCUS_MODE_MANUAL;
    }
    return item->second;
}

CameraManagerAdapterImpl& CameraManagerAdapterImpl::GetInstance()
{
    static CameraManagerAdapterImpl instance;
    return instance;
}

int32_t CameraManagerAdapterImpl::Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    WVLOG_I("create CameraManagerAdapterImpl");
    if (cameraManager_ == nullptr) {
        cameraManager_ = CameraManager::GetInstance();
        if (cameraManager_ == nullptr) {
            WVLOG_E("Failed to get camera manager!");
            return CAMERA_ERROR;
        }
    }
    cameraMngrCallback_ = std::make_shared<CameraManagerAdapterCallback>(cameraStatusCallback);
    cameraManager_->SetCallback(cameraMngrCallback_);
    return CAMERA_OK;
}

std::vector<FormatAdapter> CameraManagerAdapterImpl::GetCameraSupportFormats(
    sptr<CameraOutputCapability> outputcapability)
{
    std::vector<FormatAdapter> captureFormats;
    
    std::vector<Profile> previewProfiles = outputcapability->GetPreviewProfiles();
    for (auto i : previewProfiles) {
        FormatAdapter format;
        format.width = i.GetSize().width;
        format.height = i.GetSize().height;
        format.frameRate = DEFAULT_FRAME_RATE;
        format.pixelFormat = TransToAdapterCameraFormat(i.GetCameraFormat());
        captureFormats.push_back(format);
    }
    return captureFormats;
}

void CameraManagerAdapterImpl::GetDevicesInfo(std::vector<VideoDeviceDescriptor> &devicesDiscriptor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cameraManager_ == nullptr) {
        WVLOG_E("camera manager is nullptr");
        return;
    }

    std::vector<sptr<CameraDevice>> cameraObjList = cameraManager_->GetSupportedCameras();
    if (cameraObjList.size() == 0) {
        WVLOG_E("No cameras are available!!!");
        return;
    }
    for (auto cameraObj : cameraObjList) {
        sptr<CameraOutputCapability> outputcapability =
            cameraManager_->GetSupportedOutputCapability(cameraObj);
        VideoDeviceDescriptor deviceDisc;
        deviceDisc.displayName = cameraObj->GetID();
        deviceDisc.deviceId = cameraObj->GetID();
        deviceDisc.modelId = cameraObj->GetID();
        deviceDisc.controlSupport.pan = false;
        deviceDisc.controlSupport.tilt = false;
        deviceDisc.controlSupport.zoom = false;
        deviceDisc.transportType = GetCameraTransportType(cameraObj->GetConnectionType());
        deviceDisc.facing = GetCameraFacingMode(cameraObj->GetPosition());

        deviceDisc.supportCaptureFormats = GetCameraSupportFormats(outputcapability);
        WVLOG_I("deviceDisc  id:%{public}s, control pan:%{public}d tilt:%{public}d, zoom:%{public}d \
            transType:%{public}d, facingMode:%{public}d",
            deviceDisc.deviceId.c_str(),
            deviceDisc.controlSupport.pan,
            deviceDisc.controlSupport.tilt,
            deviceDisc.controlSupport.zoom,
            deviceDisc.transportType,
            deviceDisc.facing);
        devicesDiscriptor.emplace_back(std::move(deviceDisc));
    }
}

int32_t CameraManagerAdapterImpl::InitCameraInput(const std::string &deviceId)
{
    int32_t result = CAMERA_ERROR;

    if (status_ == CameraStatusAdapter::UNAVAILABLE) {
        WVLOG_E("camera is not closed");
        return result;
    }

    if (inputInitedFlag_) {
        WVLOG_E("input is already inited");
        return result;
    }

    if (cameraManager_ == nullptr) {
        WVLOG_E("camera manager is nullptr");
        return CAMERA_NULL_ERROR;
    }

    if (cameraInput_ == nullptr) {
        WVLOG_I("camera input create %{public}s", deviceId.c_str());
        sptr<CameraDevice> cameraObj = cameraManager_->GetCameraDeviceFromId(deviceId);
        if (cameraObj == nullptr) {
            WVLOG_E("No cameras are available!!!");
            return CAMERA_NULL_ERROR;
        }

        cameraInput_ = cameraManager_->CreateCameraInput(cameraObj);
        if (cameraInput_ == nullptr) {
            WVLOG_E("Failed to create CameraInput");
            return result;
        }
        int32_t ret = cameraInput_->Open();
        if (ret != CAMERA_OK) {
            WVLOG_E("Failed to open CameraInput, err code %{public}d.", ret);
            return result;
        }
        deviceId_ = deviceId;
        inputInitedFlag_ = true;
    }

    result = CAMERA_OK;
    return result;
}

int32_t CameraManagerAdapterImpl::InitPreviewOutput(const VideoCaptureParamsAdapter &captureParams,
    std::shared_ptr<CameraBufferListenerAdapter> listener)
{
    int32_t result = CAMERA_ERROR;
    Size previewSize;

    if (!inputInitedFlag_) {
        WVLOG_E("input is not inited");
        return result;
    }

    if (cameraManager_ == nullptr) {
        WVLOG_E("camera manager is null");
        return result;
    }

    if (previewOutput_ == nullptr) {
        WVLOG_I("preview output create");
        previewSurface_ = IConsumerSurface::Create();
        if (previewSurface_ ==  nullptr) {
            WVLOG_E("previewSurface_ is null");
            return result;
        }
        previewSize.width = captureParams.captureFormat.width;
        previewSize.height = captureParams.captureFormat.height;
        previewSurface_->SetDefaultWidthAndHeight(previewSize.width, previewSize.height);
        previewSurface_->SetUserData(CameraManager::surfaceFormat,
            std::to_string(TransToOriCameraFormat(captureParams.captureFormat.pixelFormat)));
        Profile previewproFile = Profile(static_cast<CameraFormat>(
                                        TransToOriCameraFormat(captureParams.captureFormat.pixelFormat)),
                                         previewSize);
        WVLOG_I("preview output format: %{public}d, w: %{public}d, h: %{public}d",
            TransToOriCameraFormat(captureParams.captureFormat.pixelFormat), previewSize.width, previewSize.height);
        previewSurfaceListener_ = new(std::nothrow) CameraSurfaceListener(SurfaceType::PREVIEW,
                                                                    previewSurface_,
                                                                    (listener));
        previewSurface_->RegisterConsumerListener((sptr<IBufferConsumerListener> &)previewSurfaceListener_);
        sptr<IBufferProducer> bp = previewSurface_->GetProducer();
        sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
        previewOutput_ = cameraManager_->CreatePreviewOutput(previewproFile, pSurface);
        if (previewOutput_ == nullptr) {
            WVLOG_E("Failed to create previewOutput");
            return result;
        }
        captureParams_ = captureParams;
        listener_ = listener;
    }
    result = CAMERA_OK;
    return result;
}

int32_t CameraManagerAdapterImpl::TransToAdapterExposureModes(
    std::vector<ExposureMode>& exposureModes,
    std::vector<ExposureModeAdapter>& exposureModesAdapter)
{
    for (auto exportMode : exposureModes) {
        exposureModesAdapter.push_back(GetAdapterExposureMode(exportMode));
    }

    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::GetExposureModes(std::vector<ExposureModeAdapter>& exposureModesAdapter)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get exposure modes");
        return CAMERA_ERROR;
    }
    std::vector<ExposureMode> exposureModes;
    if (captureSession_->GetSupportedExposureModes(exposureModes) != SUCCESS) {
        WVLOG_E("get inner exposure modes faileds");
        return CAMERA_ERROR;
    }

    TransToAdapterExposureModes(exposureModes, exposureModesAdapter);

    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get current exposure modes");
        return CAMERA_ERROR;
    }

    ExposureMode exposureMode = captureSession_->GetExposureMode();
    exposureModeAdapter = GetAdapterExposureMode(exposureMode);
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::GetExposureCompensation(VideoCaptureRangeAdapter& rangeVal)
{
    if (captureSession_ == nullptr) {
        return CAMERA_ERROR;
    }
    std::vector<float> exposureBiasRange = captureSession_->GetExposureBiasRange();
    int32_t exposureCompos = captureSession_->GetExposureValue();
    if (exposureBiasRange.size() == RANGE_MAX_SIZE) {
        rangeVal.min = exposureBiasRange.at(RANGE_MIN_INDEX);
        rangeVal.max = exposureBiasRange.at(RANGE_MAX_INDEX);
    }
    
    rangeVal.current = exposureCompos;
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::GetCaptionRangeById(RangeIDAdapter rangeId, VideoCaptureRangeAdapter& rangeVal)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get %{public}d range info", rangeId);
        return CAMERA_ERROR;
    }
    int32_t result = CAMERA_OK;
    switch (rangeId) {
        case RangeIDAdapter::RANGE_ID_EXP_COMPENSATION:
            if (GetExposureCompensation(rangeVal) != CAMERA_OK) {
                WVLOG_E("get exposure compensation failed.");
                result = CAMERA_ERROR;
            }
            break;
        default:
            result = CAMERA_ERROR;
            break;
    }
    return result;
}

bool CameraManagerAdapterImpl::IsFocusModeSupported(FocusModeAdapter focusMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get support focuc mode");
        return false;
    }
    if (!captureSession_->IsFocusModeSupported(GetOriFocusMode(focusMode))) {
        return false;
    }
    return true;
}

FocusModeAdapter CameraManagerAdapterImpl::GetCurrentFocusMode()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get support focuc mode");
        return FocusModeAdapter::FOCUS_MODE_MANUAL;
    }

    FocusMode oriFocusMode = captureSession_->GetFocusMode();
    return GetAdapterFocusMode(oriFocusMode);
}

bool CameraManagerAdapterImpl::IsFlashModeSupported(FlashModeAdapter focusMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get support flash mode");
        return false;
    }
    if (!captureSession_->IsFlashModeSupported(GetOriFlashMode(focusMode))) {
        return false;
    }
    return true;
}

int32_t CameraManagerAdapterImpl::CreateAndStartSession()
{
    int32_t result = CAMERA_ERROR;
    if (status_ == CameraStatusAdapter::UNAVAILABLE) {
        WVLOG_E("camera is already opened");
        return result;
    }

    if ((cameraInput_ == nullptr) || (previewOutput_ == nullptr)) {
        WVLOG_E("cameraInput_ or previewOutput_ is null");
        return result;
    }

    WVLOG_I("CreateCaptureSession");
    captureSession_ = cameraManager_->CreateCaptureSession();
    if (captureSession_ == nullptr) {
        WVLOG_E("Failed to create capture session");
        return result;
    }
    WVLOG_I("BeginConfig");
    captureSession_->BeginConfig();
    WVLOG_I("AddInput");

    result = captureSession_->AddInput(cameraInput_);
    if (result != CAMERA_OK) {
        WVLOG_E("Failed to add in put");
        return result;
    }

    WVLOG_I("AddOutput");
    result = captureSession_->AddOutput(previewOutput_);
    if (result != CAMERA_OK) {
        WVLOG_E("Failed to add preview output");
        return result;
    }
    WVLOG_I("CommitConfig");
    result = captureSession_->CommitConfig();
    if (result != CAMERA_OK) {
        WVLOG_E("Failed to commit config");
        return result;
    }
    WVLOG_I("Start");
    result = captureSession_->Start();
    if (result != CAMERA_OK) {
        WVLOG_E("Failed to start session");
        return result;
    }
    result = CAMERA_OK;
    status_ = CameraStatusAdapter::UNAVAILABLE;
    isCapturing_ = true;
    return result;
}

int32_t CameraManagerAdapterImpl::RestartSession()
{
    std::lock_guard<std::mutex> lock(restart_mutex_);
    WVLOG_I("RestartSession %{public}s", deviceId_.c_str());
    if (!isCapturing_) {
        WVLOG_E("this web tab is not capturing");
        return CAMERA_OK;
    }

    if (!((isForegound_) && (status_ == CameraStatusAdapter::AVAILABLE))) {
        WVLOG_E("can not restart session, status:%{public}d, isForeground:%{public}d", status_, isForegound_);
        return CAMERA_OK;
    }

    if (cameraManager_ == nullptr) {
        WVLOG_E("cameraManager_ is null when start session");
        return CAMERA_ERROR;
    }

    cameraInput_ = nullptr;
    previewOutput_ = nullptr;
    previewSurface_ = nullptr;
    previewSurfaceListener_ = nullptr;
    inputInitedFlag_ = false;
    captureSession_ = nullptr;

    if (StartStream(deviceId_, captureParams_, listener_) != CAMERA_OK) {
        WVLOG_E("restart stream failed");
        ReleaseSessionResource(deviceId_);
        ReleaseSession();
        return CAMERA_ERROR;
    }
    status_ = CameraStatusAdapter::UNAVAILABLE;
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::StopSession(CameraStopType stopType)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WVLOG_I("StopSession");
    if (status_ == CameraStatusAdapter::AVAILABLE) {
        WVLOG_E("camera is already closed when stop session");
        return CAMERA_OK;
    }
    ReleaseSessionResource(deviceId_);
    ReleaseSession();
    
    if (stopType == CameraStopType::NORMAL) {
        isCapturing_ = false;
    }
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::ReleaseSession()
{
    WVLOG_I("release session");
    if (captureSession_ != nullptr) {
        captureSession_->Stop();
        captureSession_->Release();
        captureSession_ = nullptr;
    }

    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::ReleaseSessionResource(const std::string &deviceId)
{
    WVLOG_I("release session resource");
    if (deviceId_ != deviceId) {
        WVLOG_E("deviceId is not used");
        return CAMERA_OK;
    }
    if (cameraInput_ != nullptr) {
        cameraInput_->Release();
        cameraInput_ = nullptr;
    }

    if (previewOutput_ != nullptr) {
        ((sptr<PreviewOutput> &)previewOutput_)->Stop();
        previewOutput_->Release();
        previewOutput_ = nullptr;
    }

    previewSurface_ = nullptr;
    previewSurfaceListener_ = nullptr;
    status_ = CameraStatusAdapter::AVAILABLE;
    inputInitedFlag_ = false;
    isForegound_ = false;

    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::ReleaseCameraManger()
{
    std::lock_guard<std::mutex> lock(mutex_);
    WVLOG_I("release camera manger");
    ReleaseSessionResource(deviceId_);
    ReleaseSession();
    cameraManager_ = nullptr;
    status_ = CameraStatusAdapter::AVAILABLE;
    inputInitedFlag_ = false;
    isForegound_ = false;
    cameraMngrCallback_ = nullptr;
    return CAMERA_OK;
}

CameraStatusAdapter CameraManagerAdapterImpl::GetCameraStatus()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

void CameraManagerAdapterImpl::SetCameraStatus(CameraStatusAdapter status)
{
    std::lock_guard<std::mutex> lock(mutex_);
    status_ = status;
}

bool CameraManagerAdapterImpl::IsExistCaptureTask()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cameraManager_ == nullptr) {
        WVLOG_E("cameraManager_ is nullptr");
        return false;
    }
    return isCapturing_;
}

void CameraManagerAdapterImpl::SetForegroundFlag(bool isForeground)
{
    isForegound_ = isForeground;
}


int32_t CameraManagerAdapterImpl::StartStream(const std::string &deviceId,
    const VideoCaptureParamsAdapter &captureParams,
    std::shared_ptr<CameraBufferListenerAdapter> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if ((cameraManager_ == nullptr) || (listener == nullptr)) {
        WVLOG_E("cameraManager or listener is null when start session");
        return CAMERA_ERROR;
    }

    if (InitCameraInput(deviceId) != CAMERA_OK) {
        WVLOG_E("init camera input failed");
        ReleaseSessionResource(deviceId);
        return CAMERA_ERROR;
    }

    if (InitPreviewOutput(captureParams, listener) != CAMERA_OK) {
        WVLOG_E("init camera preview output failed");
        ReleaseSessionResource(deviceId);
        return CAMERA_ERROR;
    }

    if (CreateAndStartSession() != CAMERA_OK) {
        WVLOG_E("create session failed");
        ReleaseSession();
        return CAMERA_ERROR;
    }

    return CAMERA_OK;
}

CameraSurfaceBufferAdapterImpl::CameraSurfaceBufferAdapterImpl(sptr<SurfaceBuffer> buffer) : buffer_(buffer) {}

int32_t CameraSurfaceBufferAdapterImpl::GetFileDescriptor() const
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetFileDescriptor();
}

int32_t CameraSurfaceBufferAdapterImpl::GetWidth() const
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetWidth();
}

int32_t CameraSurfaceBufferAdapterImpl::GetHeight() const
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetHeight();
}

int32_t CameraSurfaceBufferAdapterImpl::GetStride() const
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetStride();
}

int32_t CameraSurfaceBufferAdapterImpl::GetFormat() const
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return -1;
    }
    return buffer_->GetFormat();
}

uint32_t CameraSurfaceBufferAdapterImpl::GetSize() const
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return 0;
    }
    return buffer_->GetSize();
}

sptr<SurfaceBuffer>& CameraSurfaceBufferAdapterImpl::GetBuffer()
{
    return buffer_;
}

uint8_t* CameraSurfaceBufferAdapterImpl::GetBufferAddr()
{
    if (!buffer_) {
        WVLOG_E("buffer_ is nullptr");
        return 0;
    }
    return static_cast<uint8_t *>(buffer_->GetVirAddr());
}

CameraSurfaceListener::CameraSurfaceListener(SurfaceType type,
    sptr<IConsumerSurface> surface,
    std::shared_ptr<CameraBufferListenerAdapter> listener)
    : surfaceType_(type), surface_(surface), listener_(listener) {
}

CameraRotationInfo CameraSurfaceListener::FillRotationInfo(int roration, bool isFlipX, bool isFlipY)
{
    CameraRotationInfo rotationInfo;
    rotationInfo.rotation = roration;
    rotationInfo.isFlipX = isFlipX;
    rotationInfo.isFlipY = isFlipY;
    return rotationInfo;
}

CameraRotationInfo CameraSurfaceListener::GetRotationInfo(GraphicTransformType transform)
{
    switch (transform) {
        case GraphicTransformType::GRAPHIC_ROTATE_NONE: {
            return FillRotationInfo(ROTATION_0, false, false);
        }
        case GraphicTransformType::GRAPHIC_ROTATE_90: {
            return FillRotationInfo(ROTATION_90, false, false);
        }
        case GraphicTransformType::GRAPHIC_ROTATE_180: {
            return FillRotationInfo(ROTATION_180, false, false);
        }
        case GraphicTransformType::GRAPHIC_ROTATE_270: {
            return FillRotationInfo(ROTATION_270, false, false);
        }
        case GraphicTransformType::GRAPHIC_FLIP_H: {
            return FillRotationInfo(ROTATION_0, false, true);
        }
        case GraphicTransformType::GRAPHIC_FLIP_V: {
            return FillRotationInfo(ROTATION_0, true, false);
        }
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT90: {
            return FillRotationInfo(ROTATION_90, false, true);
        }
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT90: {
            return FillRotationInfo(ROTATION_90, true, false);
        }
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT180: {
            return FillRotationInfo(ROTATION_180, false, true);
        }
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT180: {
            return FillRotationInfo(ROTATION_180, true, false);
        }
        case GraphicTransformType::GRAPHIC_FLIP_H_ROT270: {
            return FillRotationInfo(ROTATION_270, false, true);
        }
        case GraphicTransformType::GRAPHIC_FLIP_V_ROT270: {
            return FillRotationInfo(ROTATION_270, true, false);
        }
        default: {
            return FillRotationInfo(ROTATION_0, false, false);
        }
    }
}

void CameraSurfaceListener::OnBufferAvailable()
{
    int32_t flushFence = 0;
    int64_t timestamp = 0;
    OHOS::Rect damage;
    OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
    if (surface_ == nullptr) {
        WVLOG_E("OnBufferAvailable:surface_ is null");
        return;
    }
    surface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
    if (buffer != nullptr) {
        uint32_t size = buffer->GetSize();

        CameraRotationInfo rotationInfo = GetRotationInfo(surface_->GetTransform());
        WVLOG_D("OnBufferAvailable, surfaceType_: %{public}d, size: %{public}d, width: %{public}d,\
            height: %{public}d, type: %{public}d, ratation: %{public}d, FilyY: %{public}d, FilyX: %{public}d",
            surfaceType_, size, buffer->GetWidth(), buffer->GetHeight(), surface_->GetTransform(),
            (int32_t)rotationInfo.rotation, rotationInfo.isFlipY, rotationInfo.isFlipX);
        auto bufferAdapter = std::make_unique<CameraSurfaceBufferAdapterImpl>(buffer);
        auto surfaceAdapter = std::make_shared<CameraSurfaceAdapterImpl>(surface_);
        if (listener_ != nullptr) {
            listener_->OnBufferAvailable(surfaceAdapter, std::move(bufferAdapter), rotationInfo);
        }
    } else {
        WVLOG_E("AcquireBuffer failed!");
    }
}

CameraSurfaceAdapterImpl::CameraSurfaceAdapterImpl(sptr<IConsumerSurface> surface) : cSurface_(surface) {}

int32_t CameraSurfaceAdapterImpl::ReleaseBuffer(std::unique_ptr<CameraSurfaceBufferAdapter> bufferAdapter,
                                                int32_t fence)
{
    if (!cSurface_ || !bufferAdapter) {
        WVLOG_E("cSurface_ or bufferAdapter is nullptr");
        return -1;
    }
    auto bufferImpl = static_cast<CameraSurfaceBufferAdapterImpl*>(bufferAdapter.get());
    return cSurface_->ReleaseBuffer(bufferImpl->GetBuffer(), fence);
}

CameraManagerAdapterCallback::CameraManagerAdapterCallback(std::shared_ptr<CameraStatusCallbackAdapter>
    cameraStatusCallback): statusCallback_(cameraStatusCallback)
{
    WVLOG_I("Create CameraManagerAdapterCallback");
}

CameraStatusAdapter CameraManagerAdapterCallback::GetAdapterCameraStatus(CameraStatus status) const
{
    auto item = CAMERA_STATUS_MAP.find(status);
    if (item == CAMERA_STATUS_MAP.end()) {
        WVLOG_E("ori camera status %{public}d not found", status);
        return CameraStatusAdapter::APPEAR;
    }
    return item->second;
}

void CameraManagerAdapterCallback::OnCameraStatusChanged(const CameraStatusInfo &cameraStatusInfo) const
{
    WVLOG_I("OnCameraStatusChanged: %{public}d", cameraStatusInfo.cameraStatus);
    CameraStatusAdapter cameraStatusAdapter = GetAdapterCameraStatus(cameraStatusInfo.cameraStatus);
    if (statusCallback_) {
        WVLOG_I("adapter status callback");
        statusCallback_->OnCameraStatusChanged(cameraStatusAdapter);
    }
    return;
}

void CameraManagerAdapterCallback::OnFlashlightStatusChanged(const std::string &cameraID,
                                                             const FlashStatus flashStatus) const
{
    return;
}
}  // namespace OHOS::NWeb