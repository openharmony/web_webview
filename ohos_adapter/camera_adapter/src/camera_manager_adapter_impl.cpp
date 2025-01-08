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
#include <ohcamera/camera_device.h>
#include <window_manager/oh_display_manager.h>

#include "camera_rotation_info_adapter_impl.h"
#include "format_adapter_impl.h"
#include "hisysevent_adapter.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "video_capture_range_adapter_impl.h"
#include "video_control_support_adapter_impl.h"
#include "video_device_descriptor_adapter_impl.h"

namespace OHOS::NWeb {

const int MAX_CAMERA_ID_LEN = 50;

const std::unordered_map<Camera_Connection, VideoTransportType> TRANS_TYPE_MAP = {
    { Camera_Connection::CAMERA_CONNECTION_BUILT_IN, VideoTransportType::VIDEO_TRANS_TYPE_BUILD_IN },
    { Camera_Connection::CAMERA_CONNECTION_USB_PLUGIN, VideoTransportType::VIDEO_TRANS_TYPE_USB },
    { Camera_Connection::CAMERA_CONNECTION_REMOTE, VideoTransportType::VIDEO_TRANS_TYPE_REMOTE },
};

const std::unordered_map<Camera_Position, VideoFacingModeAdapter> FACING_MODE_MAP = {
    { Camera_Position::CAMERA_POSITION_UNSPECIFIED, VideoFacingModeAdapter::FACING_NONE },
    { Camera_Position::CAMERA_POSITION_FRONT, VideoFacingModeAdapter::FACING_USER },
    { Camera_Position::CAMERA_POSITION_BACK, VideoFacingModeAdapter::FACING_ENVIRONMENT },
};

const std::unordered_map<Camera_Format, VideoPixelFormatAdapter> TO_ADAPTER_PIXEL_FORMAT_MAP = {
    { Camera_Format::CAMERA_FORMAT_RGBA_8888, VideoPixelFormatAdapter::FORMAT_RGBA_8888 },
    { Camera_Format::CAMERA_FORMAT_YUV_420_SP, VideoPixelFormatAdapter::FORMAT_YUV_420_SP },
    { Camera_Format::CAMERA_FORMAT_JPEG, VideoPixelFormatAdapter::FORMAT_JPEG },
};

const std::unordered_map<VideoPixelFormatAdapter, Camera_Format> TO_OHOS_PIXEL_FORMAT_MAP = {
    { VideoPixelFormatAdapter::FORMAT_RGBA_8888, Camera_Format::CAMERA_FORMAT_RGBA_8888 },
    { VideoPixelFormatAdapter::FORMAT_YUV_420_SP, Camera_Format::CAMERA_FORMAT_YUV_420_SP },
    { VideoPixelFormatAdapter::FORMAT_JPEG, Camera_Format::CAMERA_FORMAT_JPEG },
};

const std::unordered_map<Camera_ExposureMode, ExposureModeAdapter> EXPOSURE_MODE_MAP = {
    { Camera_ExposureMode::EXPOSURE_MODE_LOCKED, ExposureModeAdapter::EXPOSURE_MODE_LOCKED },
    { Camera_ExposureMode::EXPOSURE_MODE_AUTO, ExposureModeAdapter::EXPOSURE_MODE_AUTO },
    { Camera_ExposureMode::EXPOSURE_MODE_CONTINUOUS_AUTO, ExposureModeAdapter::EXPOSURE_MODE_CONTINUOUS_AUTO },
};

const std::unordered_map<FocusModeAdapter, Camera_FocusMode> FOCUS_MODE_MAP = {
    { FocusModeAdapter::FOCUS_MODE_MANUAL, Camera_FocusMode::FOCUS_MODE_MANUAL },
    { FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO, Camera_FocusMode::FOCUS_MODE_CONTINUOUS_AUTO },
    { FocusModeAdapter::FOCUS_MODE_AUTO, Camera_FocusMode::FOCUS_MODE_AUTO },
    { FocusModeAdapter::FOCUS_MODE_LOCKED, Camera_FocusMode::FOCUS_MODE_LOCKED },
};

const std::unordered_map<Camera_FocusMode, FocusModeAdapter> ADAPTER_FOCUS_MODE_MAP = {
    { Camera_FocusMode::FOCUS_MODE_MANUAL, FocusModeAdapter::FOCUS_MODE_MANUAL },
    { Camera_FocusMode::FOCUS_MODE_CONTINUOUS_AUTO, FocusModeAdapter::FOCUS_MODE_CONTINUOUS_AUTO },
    { Camera_FocusMode::FOCUS_MODE_AUTO, FocusModeAdapter::FOCUS_MODE_AUTO },
    { Camera_FocusMode::FOCUS_MODE_LOCKED, FocusModeAdapter::FOCUS_MODE_LOCKED },
};

const std::unordered_map<FlashModeAdapter, Camera_FlashMode> FLASH_MODE_MAP = {
    { FlashModeAdapter::FLASH_MODE_CLOSE, Camera_FlashMode::FLASH_MODE_CLOSE },
    { FlashModeAdapter::FLASH_MODE_OPEN, Camera_FlashMode::FLASH_MODE_OPEN },
    { FlashModeAdapter::FLASH_MODE_AUTO, Camera_FlashMode::FLASH_MODE_AUTO },
    { FlashModeAdapter::FLASH_MODE_ALWAYS_OPEN, Camera_FlashMode::FLASH_MODE_ALWAYS_OPEN },
};

const std::unordered_map<Camera_Status, CameraStatusAdapter> CAMERA_STATUS_MAP = {
    { Camera_Status::CAMERA_STATUS_APPEAR, CameraStatusAdapter::APPEAR },
    { Camera_Status::CAMERA_STATUS_DISAPPEAR, CameraStatusAdapter::DISAPPEAR },
    { Camera_Status::CAMERA_STATUS_AVAILABLE, CameraStatusAdapter::AVAILABLE },
    { Camera_Status::CAMERA_STATUS_UNAVAILABLE, CameraStatusAdapter::UNAVAILABLE },
};

const std::unordered_map<CameraErrorType, std::string> ERROR_TYPE_MAP = {
    { CameraErrorType::CAMERA_NOT_CLOSE, "CAMERA_NOT_CLOSE" },
    { CameraErrorType::INPUT_ALREADY_INIT, "INPUT_ALREADY_INIT" },
    { CameraErrorType::CAMERA_MANAGER_IS_NULL, "CAMERA_MANAGER_IS_NULL" },
    { CameraErrorType::GET_CAMERA_OBJ_FAILED, "GET_CAMERA_OBJ_FAILED" },
    { CameraErrorType::CREATE_INPUT_FAILED, "CREATE_INPUT_FAILED" },
    { CameraErrorType::INPUT_OPEN_FAILED, "INPUT_OPEN_FAILED" },
    { CameraErrorType::INPUT_NOT_INIT, "INPUT_NOT_INIT" },
    { CameraErrorType::CREATE_PREVIEW_SURFACE_FAILED, "CREATE_PREVIEW_SURFACE_FAILED" },
    { CameraErrorType::CREATE_PREVIEW_OUTPUT_FAILED, "CREATE_PREVIEW_OUTPUT_FAILED" },
    { CameraErrorType::CREATE_CAPTURE_SESSION_FAILED, "CREATE_CAPTURE_SESSION_FAILED" },
    { CameraErrorType::ADD_INPUT_FAILED, "ADD_INPUT_FAILED" },
    { CameraErrorType::ADD_OUTPUT_FAILED, "ADD_OUTPUT_FAILED" },
    { CameraErrorType::START_SESSION_FAILED, "START_SESSION_FAILED" },
    { CameraErrorType::INPUT_OR_OUTPUT_IS_NULL, "INPUT_OR_OUTPUT_IS_NULL" },
    { CameraErrorType::COMMIT_CONFIG_FAILED, "COMMIT_CONFIG_FAILED" },
};

namespace {
const int32_t ROTATION_0 = 0;
const int32_t ROTATION_90 = 90;
const int32_t ROTATION_180 = 180;
const int32_t ROTATION_270 = 270;
const int32_t ROTATION_FULL = 360;

std::shared_ptr<CameraRotationInfoAdapter> FillRotationInfo(
    int roration, bool isFlipX, bool isFlipY)
{
    std::shared_ptr<CameraRotationInfoAdapterImpl> rotationInfo = std::make_shared<CameraRotationInfoAdapterImpl>();
    if (!rotationInfo) {
        WVLOG_E("new CameraRotationInfo failed");
        return nullptr;
    }
    rotationInfo->SetRotation(roration);
    rotationInfo->SetIsFlipX(isFlipX);
    rotationInfo->SetIsFlipY(isFlipY);
    return rotationInfo;
}

void OnFrameAvailable(void *context)
{
    if (context == nullptr) {
        WVLOG_E("context is null!!");
        return;
    }
    CameraManagerAdapterImpl* cameraManagerAdapterImpl = static_cast<CameraManagerAdapterImpl*>(context);
    WVLOG_D("OnFrameAvailable is called");
    OHNativeWindowBuffer* windowBuffer = nullptr;
    int fenceFd = -1;
    int32_t result = OH_NativeImage_AcquireNativeWindowBuffer(cameraManagerAdapterImpl->GetNativeImage(),
        &windowBuffer, &fenceFd);
    if (result != 0) {
        WVLOG_E("acquire buffer failed");
        return;
    }
    WVLOG_D("acquire buffer ret = %{public}d, buffer = %{public}p, fenceFd=%{public}d", result, windowBuffer, fenceFd);
    std::shared_ptr<CameraRotationInfoAdapter> rotationInfo = cameraManagerAdapterImpl->GetRotationInfo();
    auto bufferAdapter = std::make_shared<CameraSurfaceBufferAdapterImpl>(windowBuffer, fenceFd);
    auto surfaceAdapter = std::make_shared<CameraSurfaceAdapterImpl>();
    if (cameraManagerAdapterImpl->GetBufferListener() == nullptr) {
        WVLOG_E("listen is null");
        return;
    }
    cameraManagerAdapterImpl->GetBufferListener()->OnBufferAvailable(
        surfaceAdapter, std::move(bufferAdapter), std::move(rotationInfo));
}

int32_t GetDisplayRotation()
{
    NativeDisplayManager_Rotation displayRotation;
    NativeDisplayManager_ErrorCode errCode =
        OH_NativeDisplayManager_GetDefaultDisplayRotation(&displayRotation);
    if (errCode != DISPLAY_MANAGER_OK) {
        WVLOG_E("failed to get DisplayRotation errCode=%{public}d", errCode);
        return 0;
    }

    // Camera rotation angle: counterclockwise, screen rotation angle: clockwise
    switch (displayRotation) {
        case DISPLAY_MANAGER_ROTATION_0: {
            return ROTATION_0;
        }
        case DISPLAY_MANAGER_ROTATION_90: {
            return ROTATION_270;
        }
        case DISPLAY_MANAGER_ROTATION_180: {
            return ROTATION_180;
        }
        case DISPLAY_MANAGER_ROTATION_270: {
            return ROTATION_90;
        }
        default: {
            return ROTATION_0;
        }
    }
}
} // namespace

std::shared_ptr<CameraRotationInfoAdapter> CameraManagerAdapterImpl::GetRotationInfo()
{
    int32_t displayRotation = GetDisplayRotation();
    int32_t rotation = 0;
    if (cameraPosition_ == CAMERA_POSITION_BACK) {
        // Image correction angle = Camera installation angle - Screen rotation angle
        rotation = (cameraOrientation_ + ROTATION_FULL - displayRotation) % ROTATION_FULL;
    }

    if (cameraPosition_ == CAMERA_POSITION_FRONT) {
        // Image correction angle = Camera installation angle + Screen rotation angle
        rotation = (cameraOrientation_ + displayRotation) % ROTATION_FULL;
    }

    // The camera has been flipped. No flipping is required here.
    return FillRotationInfo(rotation, false, false);
}

int32_t CameraManagerAdapterImpl::RecordCameraInfo(Camera_Device &camera)
{
    Camera_ErrorCode errCode = OH_CameraDevice_GetCameraOrientation(&camera, &cameraOrientation_);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed to get Camera Orientation");
        ReportErrorSysEvent(CameraErrorType::CREATE_INPUT_FAILED);
        return CAMERA_ERROR;
    }
    cameraPosition_ = camera.cameraPosition;
    WVLOG_I("camera info orientation=%{public}d  position=%{public}d", cameraOrientation_, cameraPosition_);
    return CAMERA_OK;
}

VideoTransportType CameraManagerAdapterImpl::GetCameraTransportType(Camera_Connection connectType)
{
    auto item = TRANS_TYPE_MAP.find(connectType);
    if (item == TRANS_TYPE_MAP.end()) {
        WVLOG_E("concect type %{public}d not found", connectType);
        return VideoTransportType::VIDEO_TRANS_TYPE_OTHER;
    }
    return item->second;
}

VideoFacingModeAdapter CameraManagerAdapterImpl::GetCameraFacingMode(Camera_Position position)
{
    auto item = FACING_MODE_MAP.find(position);
    if (item == FACING_MODE_MAP.end()) {
        WVLOG_E("position type %{public}d not found", position);
        return VideoFacingModeAdapter::FACING_NONE;
    }
    return item->second;
}

VideoPixelFormatAdapter CameraManagerAdapterImpl::TransToAdapterCameraFormat(Camera_Format format)
{
    auto item = TO_ADAPTER_PIXEL_FORMAT_MAP.find(format);
    if (item == TO_ADAPTER_PIXEL_FORMAT_MAP.end()) {
        WVLOG_E("to adapter pixel format type %{public}d not found", format);
        return VideoPixelFormatAdapter::FORMAT_UNKNOWN;
    }
    return item->second;
}

Camera_Format CameraManagerAdapterImpl::TransToOriCameraFormat(VideoPixelFormatAdapter format)
{
    auto item = TO_OHOS_PIXEL_FORMAT_MAP.find(format);
    if (item == TO_OHOS_PIXEL_FORMAT_MAP.end()) {
        WVLOG_E("to pixel format type %{public}d not found", format);
        return Camera_Format::CAMERA_FORMAT_RGBA_8888;
    }
    return item->second;
}

ExposureModeAdapter CameraManagerAdapterImpl::GetAdapterExposureMode(Camera_ExposureMode exportMode)
{
    auto item = EXPOSURE_MODE_MAP.find(exportMode);
    if (item == EXPOSURE_MODE_MAP.end()) {
        WVLOG_E("to exposure mode %{public}d not found", exportMode);
        return ExposureModeAdapter::EXPOSURE_MODE_UNSUPPORTED;
    }
    return item->second;
}

Camera_FocusMode CameraManagerAdapterImpl::GetOriFocusMode(FocusModeAdapter focusMode)
{
    auto item = FOCUS_MODE_MAP.find(focusMode);
    if (item == FOCUS_MODE_MAP.end()) {
        WVLOG_E("adapter focus mode %{public}d not found", focusMode);
        return Camera_FocusMode::FOCUS_MODE_MANUAL;
    }
    return item->second;
}

Camera_FlashMode CameraManagerAdapterImpl::GetOriFlashMode(FlashModeAdapter flashMode)
{
    auto item = FLASH_MODE_MAP.find(flashMode);
    if (item == FLASH_MODE_MAP.end()) {
        WVLOG_E("adapter flash mode %{public}d not found", flashMode);
        return Camera_FlashMode::FLASH_MODE_CLOSE;
    }
    return item->second;
}

FocusModeAdapter CameraManagerAdapterImpl::GetAdapterFocusMode(Camera_FocusMode focusMode)
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

CameraManagerAdapterImpl::~CameraManagerAdapterImpl()
{
    ReleaseCameraManger();
}

int32_t CameraManagerAdapterImpl::ErrorTypeToString(CameraErrorType errorType, std::string& errnoTypeString)
{
    auto item = ERROR_TYPE_MAP.find(errorType);
    if (item == ERROR_TYPE_MAP.end()) {
        WVLOG_E("ori error type %{public}d not found", errorType);
        return CAMERA_ERROR;
    }

    errnoTypeString = item->second;
    return CAMERA_OK;
}

void CameraManagerAdapterImpl::ReportErrorSysEvent(CameraErrorType errorType)
{
    WVLOG_I("ReportErrorSysEvent: %{public}d", errorType);
    const std::string CAMERA_CAPTURE_ERROR = "CAMERA_CAPTURE_ERROR";
    const std::string DEVICE_ID = "DEVICE_ID";
    const std::string ERROR_DESC = "ERROR_DESC";
    std::string errnoTypeString = "";
    if (ErrorTypeToString(errorType, errnoTypeString) != CAMERA_OK) {
        return;
    }
    OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(CAMERA_CAPTURE_ERROR,
        HiSysEventAdapter::EventType::FAULT, { DEVICE_ID, wantedDeviceId_, ERROR_DESC, errnoTypeString });
}

CameraStatusAdapter GetAdapterCameraStatus(Camera_Status status)
{
    auto item = CAMERA_STATUS_MAP.find(status);
    if (item == CAMERA_STATUS_MAP.end()) {
        WVLOG_E("ori camera status %{public}d not found", status);
        return CameraStatusAdapter::APPEAR;
    }
    return item->second;
}

void OnCameraStatusChanged(Camera_Manager* cameraManager, Camera_StatusInfo *cameraStatusInfo)
{
    std::string callbackDeviceId;
    if (cameraStatusInfo == nullptr) {
        return;
    }
    if (cameraStatusInfo->camera) {
        callbackDeviceId = cameraStatusInfo->camera->cameraId;
    }
    std::string currentDeviceId = CameraManagerAdapterImpl::GetInstance().GetCurrentDeviceId();

    WVLOG_I("OnCameraStatusChanged: callbackdeviceID %{public}s, currentDeviceId:%{public}s, status %{public}d",
        callbackDeviceId.c_str(), currentDeviceId.c_str(), cameraStatusInfo->status);
    CameraStatusAdapter cameraStatusAdapter = GetAdapterCameraStatus(cameraStatusInfo->status);

    std::shared_ptr<CameraStatusCallbackAdapter> statusCallback = 
        CameraManagerAdapterImpl::GetInstance().GetStatusCallback();
    if (statusCallback) {
        switch (cameraStatusAdapter) {
            case CameraStatusAdapter::AVAILABLE:
                WVLOG_I("do not handle status AVAILABLE");
                return;
            case CameraStatusAdapter::UNAVAILABLE:
                WVLOG_I("do not handle status UNAVAILABLE");
                return;
            case CameraStatusAdapter::APPEAR:
            case CameraStatusAdapter::DISAPPEAR:
                break;
            default:
                WVLOG_I("unknow status");
                return;
        }
        WVLOG_I("start do statusCallback");
        statusCallback->OnCameraStatusChanged(cameraStatusAdapter, callbackDeviceId);
    }
    return;
}

int32_t CameraManagerAdapterImpl::Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);

    WVLOG_I("create CameraManagerAdapterImpl");
    Camera_ErrorCode errCode = CAMERA_DEVICE_DISABLED;
    if (cameraManager_ == nullptr) {
        errCode = OH_Camera_GetCameraManager(&cameraManager_);
        if (errCode != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("Failed to get camera manager!");
            return CAMERA_ERROR;
        }
    }
    statusCallback_ = cameraStatusCallback;
    cameraManagerCallback_ = {
        .onCameraStatus = OnCameraStatusChanged,
    };
    errCode = OH_CameraManager_RegisterCallback(cameraManager_, &cameraManagerCallback_);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed camera manager to registerCallback!");
        return CAMERA_ERROR;
    }
    return CAMERA_OK;
}

std::vector<std::shared_ptr<FormatAdapter>> CameraManagerAdapterImpl::GetCameraSupportFormats(
    Camera_OutputCapability *outputcapability)
{
    std::vector<std::shared_ptr<FormatAdapter>> captureFormats;
    if (outputcapability == nullptr) {
        return captureFormats;
    }
    for (uint32_t i = 0; i < outputcapability->previewProfilesSize; i++) {
        if (outputcapability->previewProfiles[i] == nullptr) {
            continue;
        }
        std::shared_ptr<FormatAdapterImpl> format = std::make_shared<FormatAdapterImpl>();
        if (!format) {
            WVLOG_E("new FormatAdapter failed");
            return captureFormats;
        }
        format->SetWidth(outputcapability->previewProfiles[i]->size.width);
        format->SetHeight(outputcapability->previewProfiles[i]->size.height);
        format->SetFrameRate(DEFAULT_FRAME_RATE);
        format->SetPixelFormat(TransToAdapterCameraFormat(outputcapability->previewProfiles[i]->format));
        captureFormats.push_back(format);
    }
    return captureFormats;
}

bool CameraManagerAdapterImpl::HandleDeviceDisc(std::shared_ptr<VideoDeviceDescriptorAdapterImpl> deviceDisc,
    Camera_Device& camera, Camera_OutputCapability *outputCapability)
{
    if (!deviceDisc) {
        WVLOG_E("new VideoDeviceDescriptorAdapterl failed");
        auto errCode = OH_CameraManager_DeleteSupportedCameraOutputCapability(cameraManager_, outputCapability);
        if (errCode != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("delete outputCapability failed");
        }
        return false;
    }
    
    deviceDisc->SetDisplayName(camera.cameraId);
    deviceDisc->SetDeviceId(camera.cameraId);
    deviceDisc->SetModelId(camera.cameraId);

    std::shared_ptr<VideoControlSupportAdapterImpl> controlSupport =
        std::make_shared<VideoControlSupportAdapterImpl>();
    if (!controlSupport) {
        WVLOG_E("new VideoControlSupportAdapter failed");
        auto errCode = OH_CameraManager_DeleteSupportedCameraOutputCapability(cameraManager_, outputCapability);
        if (errCode != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("delete outputCapability failed");
        }
        return false;
    }
    controlSupport->SetPan(false);
    controlSupport->SetTilt(false);
    controlSupport->SetZoom(false);
    deviceDisc->SetControlSupport(controlSupport);

    deviceDisc->SetTransportType(GetCameraTransportType(camera.connectionType));
    deviceDisc->SetFacingMode(GetCameraFacingMode(camera.cameraPosition));

    deviceDisc->SetSupportCaptureFormats(GetCameraSupportFormats(outputCapability));
    WVLOG_I("deviceDisc  id:%{public}s, control pan:%{public}d tilt:%{public}d, zoom:%{public}d \
        transType:%{public}d, facingMode:%{public}d",
        deviceDisc->GetDeviceId().c_str(), deviceDisc->GetControlSupport()->GetPan(),
        deviceDisc->GetControlSupport()->GetTilt(), deviceDisc->GetControlSupport()->GetZoom(),
        deviceDisc->GetTransportType(), deviceDisc->GetFacingMode());
    return true;
}

std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> CameraManagerAdapterImpl::GetDevicesInfo()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cameraManager_ == nullptr) {
        WVLOG_E("camera manager is nullptr");
        return std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>();
    }
    Camera_Device *cameras = nullptr;
    uint32_t size = 0;
    Camera_ErrorCode errCode = OH_CameraManager_GetSupportedCameras(cameraManager_, &cameras, &size);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera list failed");
        return std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>();
    }
    if (size == 0) {
        WVLOG_E("No cameras are available!!!");
        return std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>();
    }

    std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> devicesDiscriptor;

    for (uint32_t i = 0; i < size; i++) {
        Camera_OutputCapability *outputCapability = nullptr;
        errCode = OH_CameraManager_GetSupportedCameraOutputCapability(cameraManager_, &cameras[i], &outputCapability);
        if (errCode != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("get camera Supported Camera Output Capability failed");
        }
        if (outputCapability == nullptr) {
            WVLOG_E("outputCapability is null");
            continue;
        }
        std::shared_ptr<VideoDeviceDescriptorAdapterImpl> deviceDisc = 
            std::make_shared<VideoDeviceDescriptorAdapterImpl>();
        if (!HandleDeviceDisc(deviceDisc, cameras[i], outputCapability)) {
            errCode = OH_CameraManager_DeleteSupportedCameraOutputCapability(cameraManager_, outputCapability);
            if (errCode != Camera_ErrorCode::CAMERA_OK) {
                WVLOG_E("delete camera list failed");
            }
            continue;
        }
        devicesDiscriptor.emplace_back(std::move(deviceDisc));
        errCode = OH_CameraManager_DeleteSupportedCameraOutputCapability(cameraManager_, outputCapability);
        if (errCode != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("delete camera list failed");
        }
    }

    errCode = OH_CameraManager_DeleteSupportedCameras(cameraManager_, cameras, size);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("delete outputCapability failed");
    }
    return devicesDiscriptor;
}

int32_t CameraManagerAdapterImpl::GetCameraDeviceFromId(const std::string& deviceId, Camera_Device *camera)
{
    Camera_Device *cameras = nullptr;
    uint32_t size = 0;
    Camera_ErrorCode errCode = OH_CameraManager_GetSupportedCameras(cameraManager_, &cameras, &size);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera list failed");
        return CAMERA_ERROR;
    }
    if (size == 0) {
        WVLOG_E("No cameras are availabele!!!");
        return CAMERA_ERROR;
    }
    for (uint32_t i = 0; i < size; i++) {
        if (cameras[i].cameraId == deviceId) {
            for (int j = 0; j < MAX_CAMERA_ID_LEN && j < strlen(cameras[i].cameraId) + 1; j++) {
                camera->cameraId[j] = cameras[i].cameraId[j];
            }
            camera->cameraPosition = cameras[i].cameraPosition;
            camera->cameraType = cameras[i].cameraType;
            camera->connectionType = cameras[i].connectionType;

            errCode = OH_CameraManager_DeleteSupportedCameras(cameraManager_, cameras, size);
            if (errCode != Camera_ErrorCode::CAMERA_OK) {
                WVLOG_E("delete camera list failed");
                return CAMERA_ERROR;
            }
            return CAMERA_OK;
        }
    }
    return CAMERA_ERROR;
}

int32_t CameraManagerAdapterImpl::InitCameraInput(const std::string& deviceId)
{
    if (status_ == CameraStatusAdapter::UNAVAILABLE) {
        WVLOG_E("camera is not closed");
        ReportErrorSysEvent(CameraErrorType::CAMERA_NOT_CLOSE);
        return CAMERA_ERROR;
    }

    if (inputInitedFlag_) {
        WVLOG_E("input is already inited");
        ReportErrorSysEvent(CameraErrorType::INPUT_ALREADY_INIT);
        return CAMERA_ERROR;
    }

    if (cameraManager_ == nullptr) {
        WVLOG_E("camera manager is nullptr");
        ReportErrorSysEvent(CameraErrorType::CAMERA_MANAGER_IS_NULL);
        return CAMERA_NULL_ERROR;
    }

    if (cameraInput_ == nullptr) {
        WVLOG_I("camera input create %{public}s", deviceId.c_str());
        char cameraIdData[MAX_CAMERA_ID_LEN + 1]{0};
        Camera_Device camera;
        camera.cameraId = cameraIdData;
        if (GetCameraDeviceFromId(deviceId, &camera) != CAMERA_OK) {
            WVLOG_E("No cameras are available!!!");
            ReportErrorSysEvent(CameraErrorType::GET_CAMERA_OBJ_FAILED);
            return CAMERA_NULL_ERROR;
        }

        if (RecordCameraInfo(camera) != CAMERA_OK) {
            return CAMERA_ERROR;
        }

        Camera_ErrorCode errCode = OH_CameraManager_CreateCameraInput(cameraManager_, &camera, &cameraInput_);
        if (errCode != Camera_ErrorCode::CAMERA_OK || cameraInput_ == nullptr) {
            WVLOG_E("Failed to create CameraInput");
            ReportErrorSysEvent(CameraErrorType::CREATE_INPUT_FAILED);
            return CAMERA_ERROR;
        }
        errCode = OH_CameraInput_Open(cameraInput_);
        if (errCode != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("Failed to open CameraInput, err code %{public}d.", errCode);
            OH_CameraInput_Release(cameraInput_);
            cameraInput_ = nullptr;
            ReportErrorSysEvent(CameraErrorType::INPUT_OPEN_FAILED);
            return CAMERA_ERROR;
        }
        deviceId_ = deviceId;
        inputInitedFlag_ = true;
    }
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::InitPreviewOutput(const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
    std::shared_ptr<CameraBufferListenerAdapter> listener)
{
    if (!inputInitedFlag_) {
        WVLOG_E("input is not inited");
        return CAMERA_ERROR;
    }
    if (cameraManager_ == nullptr) {
        WVLOG_E("camera manager is null");
        return CAMERA_ERROR;
    }
    if (previewOutput_ == nullptr) {
        WVLOG_I("preview output create");
        nativeImage_ = OH_NativeImage_Create(0, 0);
        if (nativeImage_ == nullptr) {
            WVLOG_E("NativeImage create failed");
            return CAMERA_ERROR;
        }
        OH_ConsumerSurface_SetDefaultUsage(nativeImage_, NATIVEBUFFER_USAGE_CPU_READ);
        OHNativeWindow *window = OH_NativeImage_AcquireNativeWindow(nativeImage_);
        if (window == nullptr) {
            WVLOG_E("NativeImage acquire window failed");
            return CAMERA_ERROR;
        }
        uint64_t surfaceID;
        if (OH_NativeWindow_GetSurfaceId(window, &surfaceID) != Camera_ErrorCode::CAMERA_OK) {
            return CAMERA_ERROR;
        }
        if (OH_NativeImage_SetOnFrameAvailableListener(nativeImage_,
            OH_OnFrameAvailableListener{(void*)this, OnFrameAvailable}) != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("create preview output failed");
            return CAMERA_ERROR;
        }

        std::string surfaceIdStr = std::to_string(surfaceID);
        const char* surfaceId = surfaceIdStr.c_str();
        Camera_Profile previewProfile ;
        previewProfile.size.width =  captureParams->GetWidth();
        previewProfile.size.height = captureParams->GetHeight();
        previewProfile.format = TransToOriCameraFormat(captureParams->GetPixelFormat());
        if (OH_CameraManager_CreatePreviewOutput(cameraManager_,
            &previewProfile, surfaceId, &previewOutput_) != Camera_ErrorCode::CAMERA_OK
            || previewOutput_ == nullptr) {
            WVLOG_E("Failed to create previewOutput");
            ReportErrorSysEvent(CameraErrorType::CREATE_PREVIEW_OUTPUT_FAILED);
            return CAMERA_ERROR;
        }
        captureParams_ = captureParams;
        bufferListener_ = listener;
    }
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::TransToAdapterExposureModes(
    std::vector<Camera_ExposureMode>& exposureModes, std::vector<ExposureModeAdapter>& exposureModesAdapter)
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
    std::vector<Camera_ExposureMode> exposureSuportModes;
    Camera_ExposureMode exposureModes[] = {
        Camera_ExposureMode::EXPOSURE_MODE_LOCKED,
        Camera_ExposureMode::EXPOSURE_MODE_AUTO,
        Camera_ExposureMode::EXPOSURE_MODE_CONTINUOUS_AUTO
    };
    for (Camera_ExposureMode mode : exposureModes) {
        bool isSupported = false;
        Camera_ErrorCode ret = OH_CaptureSession_IsExposureModeSupported(captureSession_, mode, &isSupported);
        if (ret != Camera_ErrorCode::CAMERA_OK) {
            WVLOG_E("get camera exposure Mode failed");
            continue;
        }
        if (isSupported) {
            exposureSuportModes.push_back(mode);
        } else {
            continue;
        }
    }
    TransToAdapterExposureModes(exposureSuportModes, exposureModesAdapter);
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get current exposure modes");
        return CAMERA_ERROR;
    }
    Camera_ExposureMode exposureMode;
    Camera_ErrorCode errCode = OH_CaptureSession_GetExposureMode(captureSession_, &exposureMode);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera exposure Mode failed");
        return CAMERA_ERROR;
    }
    exposureModeAdapter = GetAdapterExposureMode(exposureMode);
    return CAMERA_OK;
}

std::shared_ptr<VideoCaptureRangeAdapter> CameraManagerAdapterImpl::GetExposureCompensation()
{
    if (captureSession_ == nullptr) {
        return nullptr;
    }

    std::shared_ptr<VideoCaptureRangeAdapterImpl> rangeVal = std::make_shared<VideoCaptureRangeAdapterImpl>();
    if (!rangeVal) {
        WVLOG_E("new VideoCaptureRangeAdapter failed");
        return nullptr;
    }
    float minExposureBias;
    float maxExposureBias;
    float step;
    Camera_ErrorCode errCode = OH_CaptureSession_GetExposureBiasRange(captureSession_,
        &minExposureBias, &maxExposureBias, &step);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera Exposure bias Range failed");
    }
    float exposureCompos;
    errCode = OH_CaptureSession_GetExposureBias(captureSession_, &exposureCompos);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera Exposure bias failed");
    }
    rangeVal->SetMin(minExposureBias);
    rangeVal->SetMax(maxExposureBias);
    rangeVal->SetStep(step);
    rangeVal->SetCurrent(exposureCompos);
    return rangeVal;
}

std::shared_ptr<VideoCaptureRangeAdapter> CameraManagerAdapterImpl::GetCaptionRangeById(RangeIDAdapter rangeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get %{public}d range info", rangeId);
        return nullptr;
    }

    std::shared_ptr<VideoCaptureRangeAdapter> result = nullptr;
    if (rangeId == RangeIDAdapter::RANGE_ID_EXP_COMPENSATION) {
        result = GetExposureCompensation();
        if (!result) {
            WVLOG_E("get exposure compensation failed.");
        }
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
    bool isSupported;
    Camera_ErrorCode errCode = OH_CaptureSession_IsFocusModeSupported(captureSession_,
    GetOriFocusMode(focusMode), &isSupported);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera list failed");
    }
    if (!isSupported) {
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
    Camera_FocusMode oriFocusMode;
    Camera_ErrorCode errCode = OH_CaptureSession_GetFocusMode(captureSession_, &oriFocusMode);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera list failed");
    }
    return GetAdapterFocusMode(oriFocusMode);
}

bool CameraManagerAdapterImpl::IsFlashModeSupported(FlashModeAdapter flashMode)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (captureSession_ == nullptr) {
        WVLOG_E("captureSession is nullptr when get support flash mode");
        return false;
    }
    Camera_FlashMode oriFlashMode = GetOriFlashMode(flashMode);
    bool isSupported;
    Camera_ErrorCode errCode = OH_CaptureSession_IsFlashModeSupported(captureSession_, oriFlashMode, &isSupported);
    if (errCode != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("get camera list failed");
    }
    if (!isSupported) {
        return false;
    }
    return true;
}

int32_t CameraManagerAdapterImpl::CreateAndStartSession()
{
    if (status_ == CameraStatusAdapter::UNAVAILABLE) {
        WVLOG_E("camera is already opened");
        ReportErrorSysEvent(CameraErrorType::CAMERA_NOT_CLOSE);
        return CAMERA_ERROR;
    }

    if ((cameraInput_ == nullptr) || (previewOutput_ == nullptr)) {
        WVLOG_E("cameraInput_ or previewOutput_ is null");
        ReportErrorSysEvent(CameraErrorType::INPUT_OR_OUTPUT_IS_NULL);
        return CAMERA_ERROR;
    }

    WVLOG_I("CreateCaptureSession");
    int32_t result = OH_CameraManager_CreateCaptureSession(cameraManager_, &captureSession_);
    if (result != CAMERA_OK || captureSession_ == nullptr) {
        WVLOG_E("Failed to create capture session");
        ReportErrorSysEvent(CameraErrorType::CREATE_CAPTURE_SESSION_FAILED);
        return CAMERA_ERROR;
    }
    if (OH_CaptureSession_BeginConfig(captureSession_) != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed to begin confiture session");
        return CAMERA_ERROR;
    }
    if (OH_CaptureSession_AddInput(captureSession_, cameraInput_) != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed to add input");
        return CAMERA_ERROR;
    }
    if (OH_CaptureSession_AddPreviewOutput(captureSession_, previewOutput_) != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed to add preview output");
        ReportErrorSysEvent(CameraErrorType::ADD_OUTPUT_FAILED);
        return CAMERA_ERROR;
    }
    if (OH_CaptureSession_CommitConfig(captureSession_) != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed to commit config");
        ReportErrorSysEvent(CameraErrorType::COMMIT_CONFIG_FAILED);
        return CAMERA_ERROR;
    }
    if (OH_CaptureSession_Start(captureSession_) != Camera_ErrorCode::CAMERA_OK) {
        WVLOG_E("Failed to start session");
        ReportErrorSysEvent(CameraErrorType::START_SESSION_FAILED);
        return CAMERA_ERROR;
    }
    status_ = CameraStatusAdapter::UNAVAILABLE;
    isCapturing_ = true;
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::RestartSession()
{
    std::lock_guard<std::mutex> lock(restart_mutex_);
    WVLOG_I("RestartSession %{public}s", deviceId_.c_str());
    if (!isCapturing_) {
        WVLOG_E("this web tab is not capturing");
        return CAMERA_OK;
    }

    if (cameraManager_ == nullptr) {
        WVLOG_E("cameraManager_ is null when start session");
        return CAMERA_ERROR;
    }

    if (cameraInput_ != nullptr) {
        OH_CameraInput_Release(cameraInput_);
        cameraInput_ = nullptr;
    }

    if (previewOutput_ != nullptr) {
        OH_PreviewOutput_Stop(previewOutput_);
        OH_PreviewOutput_Release(previewOutput_);
        previewOutput_ = nullptr;
        DestroyNativeImageAndWindow();
    }

    inputInitedFlag_ = false;
    captureSession_ = nullptr;
    status_ = CameraStatusAdapter::AVAILABLE;

    if (StartStream(deviceId_, captureParams_, bufferListener_) != CAMERA_OK) {
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
        OH_CaptureSession_Stop(captureSession_);
        OH_CaptureSession_Release(captureSession_);
        captureSession_ = nullptr;
    }
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::ReleaseSessionResource(const std::string& deviceId)
{
    WVLOG_I("release session resource");
    if (deviceId_ != deviceId) {
        WVLOG_E("deviceId is not used");
        return CAMERA_OK;
    }
    if (cameraInput_ != nullptr) {
        OH_CameraInput_Release(cameraInput_);
        cameraInput_ = nullptr;
    }

    if (previewOutput_ != nullptr) {
        OH_PreviewOutput_Stop(previewOutput_);
        OH_PreviewOutput_Release(previewOutput_);
        previewOutput_ = nullptr;
        DestroyNativeImageAndWindow();
    }
    status_ = CameraStatusAdapter::AVAILABLE;
    inputInitedFlag_ = false;
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::DestroyNativeImageAndWindow()
{
    if (nativeImage_ != nullptr) {
        OH_NativeImage_Destroy(&nativeImage_);
        nativeImage_ = nullptr;
    }
    return CAMERA_OK;
}

int32_t CameraManagerAdapterImpl::ReleaseCameraManger()
{
    std::lock_guard<std::mutex> lock(mutex_);
    WVLOG_I("release camera manger");
    ReleaseSessionResource(deviceId_);
    ReleaseSession();
    status_ = CameraStatusAdapter::AVAILABLE;
    inputInitedFlag_ = false;
    isForegound_ = false;
    bufferListener_ = nullptr;
    statusCallback_ = nullptr;
    OH_CameraManager_UnregisterCallback(cameraManager_, &cameraManagerCallback_);
    OH_Camera_DeleteCameraManager(cameraManager_);
    cameraManager_ = nullptr;
    return CAMERA_OK;
}

CameraStatusAdapter CameraManagerAdapterImpl::GetCameraStatus()
{
    return status_;
}

void CameraManagerAdapterImpl::SetCameraStatus(CameraStatusAdapter status)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WVLOG_I("set camera status %{public}d", status);
    status_ = status;
}

std::string CameraManagerAdapterImpl::GetCurrentDeviceId()
{
    return deviceId_;
}

OH_NativeImage* CameraManagerAdapterImpl::GetNativeImage()
{
    return nativeImage_;
}

std::shared_ptr<CameraBufferListenerAdapter> CameraManagerAdapterImpl::GetBufferListener()
{
    return bufferListener_;
}

std::shared_ptr<CameraStatusCallbackAdapter> CameraManagerAdapterImpl::GetStatusCallback()
{
    return statusCallback_;
}

bool CameraManagerAdapterImpl::IsExistCaptureTask()
{
    if (cameraManager_ == nullptr) {
        WVLOG_E("cameraManager is nullptr");
        return false;
    }
    return isCapturing_;
}

void CameraManagerAdapterImpl::SetForegroundFlag(bool isForeground)
{
    isForegound_ = isForeground;
}

int32_t CameraManagerAdapterImpl::StartStream(const std::string& deviceId,
    const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
    std::shared_ptr<CameraBufferListenerAdapter> listener)
{
    std::lock_guard<std::mutex> lock(mutex_);
    wantedDeviceId_ = deviceId;
    if ((cameraManager_ == nullptr) || (listener == nullptr)) {
        WVLOG_E("cameraManager or listener is null when start session");
        return CAMERA_ERROR;
    }

    if (captureParams == nullptr) {
        WVLOG_E("captureParams is null");
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

CameraSurfaceBufferAdapterImpl::CameraSurfaceBufferAdapterImpl(OHNativeWindowBuffer* windowBuffer, int32_t fence)
    : windowBuffer_(windowBuffer), fenceFd_(fence) {}

CameraSurfaceBufferAdapterImpl::~CameraSurfaceBufferAdapterImpl()
{
    if (windowBuffer_ != nullptr) {
        OH_NativeImage *nativeImage = CameraManagerAdapterImpl::GetInstance().GetNativeImage();
        if (nativeImage != nullptr) {
            OH_NativeImage_ReleaseNativeWindowBuffer(nativeImage, windowBuffer_, fenceFd_);
        }
    }
}

int32_t CameraSurfaceBufferAdapterImpl::GetFileDescriptor()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return -1;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return -1;
    }
    return handle->fd;
}

int32_t CameraSurfaceBufferAdapterImpl::GetWidth()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return -1;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return -1;
    }
    return handle->width;
}

int32_t CameraSurfaceBufferAdapterImpl::GetHeight()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return -1;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return -1;
    }
    return handle->height;
}

int32_t CameraSurfaceBufferAdapterImpl::GetStride()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return -1;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return -1;
    }
    return handle->stride;
}

int32_t CameraSurfaceBufferAdapterImpl::GetFormat()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return -1;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return -1;
    }
    return handle->format;
}

uint32_t CameraSurfaceBufferAdapterImpl::GetSize()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return 0;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return -1;
    }
    return handle->size;
}

uint8_t* CameraSurfaceBufferAdapterImpl::GetBufferAddr()
{
    if (!windowBuffer_) {
        WVLOG_E("window buffer is nullptr");
        return 0;
    }
    auto handle = OH_NativeWindow_GetBufferHandleFromNative(windowBuffer_);
    if (handle == nullptr) {
        WVLOG_E("window buffer handle is nullptr");
        return 0;
    }
    return static_cast<uint8_t*>(handle->virAddr);
}

OHNativeWindowBuffer* CameraSurfaceBufferAdapterImpl::GetWindowBuffer()
{
    return windowBuffer_;
}

int32_t CameraSurfaceBufferAdapterImpl::ClearWindowBuffer()
{
    windowBuffer_ = nullptr;
    return 0;
}

int32_t CameraSurfaceBufferAdapterImpl::GetFenceFd()
{
    return fenceFd_;
}

int32_t CameraSurfaceAdapterImpl::ReleaseBuffer(
    std::shared_ptr<CameraSurfaceBufferAdapter> bufferAdapter, int32_t fence)
{
    if (!bufferAdapter) {
        WVLOG_E("bufferAdapter is nullptr");
        return -1;
    }
    auto bufferImpl = static_cast<CameraSurfaceBufferAdapterImpl*>(bufferAdapter.get());
    OH_NativeImage *nativeImage = CameraManagerAdapterImpl::GetInstance().GetNativeImage();
    if (nativeImage == nullptr) {
        return -1;
    }
    int32_t ret = OH_NativeImage_ReleaseNativeWindowBuffer(nativeImage, bufferImpl->GetWindowBuffer(),
        bufferImpl->GetFenceFd());
    if (ret!= 0) {
        WVLOG_E("release buffer failed");
        return ret;
    }
    bufferImpl->ClearWindowBuffer();
    return ret;
}

} // namespace OHOS::NWeb
