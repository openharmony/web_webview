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

#include "camera_manager_adapter_impl.h"

namespace OHOS::NWeb {
CameraManagerAdapterImpl& CameraManagerAdapterImpl::GetInstance()
{
    static CameraManagerAdapterImpl instance;
    return instance;
}

int32_t CameraManagerAdapterImpl::Create(std::shared_ptr<CameraStatusCallbackAdapter> cameraStatusCallback)
{
    return -1;
}

std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>> CameraManagerAdapterImpl::GetDevicesInfo()
{
    return std::vector<std::shared_ptr<VideoDeviceDescriptorAdapter>>();
}

int32_t CameraManagerAdapterImpl::GetExposureModes(std::vector<ExposureModeAdapter>& exposureModesAdapter)
{
    return -1;
}

int32_t CameraManagerAdapterImpl::GetCurrentExposureMode(ExposureModeAdapter& exposureModeAdapter)
{
    return -1;
}

std::shared_ptr<VideoCaptureRangeAdapter> CameraManagerAdapterImpl::GetCaptionRangeById(RangeIDAdapter rangeId)
{
    return nullptr;
}

bool CameraManagerAdapterImpl::IsFocusModeSupported(FocusModeAdapter focusMode)
{
    return false;
}

FocusModeAdapter CameraManagerAdapterImpl::GetCurrentFocusMode()
{
    return FocusModeAdapter::FOCUS_MODE_MANUAL;
}

bool CameraManagerAdapterImpl::IsFlashModeSupported(FlashModeAdapter focusMode)
{
    return false;
}

int32_t CameraManagerAdapterImpl::GetSupportedFlashModes(std::vector<FlashModeAdapter>& flashModesAdapter)
{
    return -1;
}

bool CameraManagerAdapterImpl::HasFlash()
{
    return false;
}

FlashModeAdapter CameraManagerAdapterImpl::GetFlashMode()
{
    return FlashModeAdapter::FLASH_MODE_CLOSE;
}

int32_t CameraManagerAdapterImpl::SetFlashMode(FlashModeAdapter flashMode)
{
    return -1;
}

int32_t CameraManagerAdapterImpl::RestartSession()
{
    return -1;
}

int32_t CameraManagerAdapterImpl::StopSession(CameraStopType stopType)
{
    return -1;
}

int32_t CameraManagerAdapterImpl::ReleaseCameraManger()
{
    return -1;
}

CameraStatusAdapter CameraManagerAdapterImpl::GetCameraStatus()
{
    return CameraStatusAdapter::UNAVAILABLE;
}

void CameraManagerAdapterImpl::SetCameraStatus(CameraStatusAdapter status) {}

std::string CameraManagerAdapterImpl::GetCurrentDeviceId()
{
    return std::string();
}

bool CameraManagerAdapterImpl::IsExistCaptureTask()
{
    return false;
}

void CameraManagerAdapterImpl::SetForegroundFlag(bool isForeground) {}

int32_t CameraManagerAdapterImpl::StartStream(const std::string& deviceId,
    const std::shared_ptr<VideoCaptureParamsAdapter> captureParams,
    std::shared_ptr<CameraBufferListenerAdapter> listener)
{
    return -1;
}
} // namespace OHOS::NWeb
