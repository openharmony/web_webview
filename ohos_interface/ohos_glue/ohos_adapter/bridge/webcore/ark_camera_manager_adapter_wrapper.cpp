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

#include "ohos_adapter/bridge/ark_camera_manager_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_camera_buffer_listener_adapter_impl.h"
#include "ohos_adapter/bridge/ark_camera_status_callback_adapter_impl.h"
#include "ohos_adapter/bridge/ark_video_capture_params_adapter_impl.h"
#include "ohos_adapter/bridge/ark_video_capture_range_adapter_wrapper.h"
#include "ohos_adapter/ctocpp/ark_video_device_descriptor_adapter_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCameraManagerAdapterWrapper::ArkCameraManagerAdapterWrapper(ArkWebRefPtr<ArkCameraManagerAdapter> ref) : ctocpp_(ref)
{}

int32_t ArkCameraManagerAdapterWrapper::Create(std::shared_ptr<NWeb::CameraStatusCallbackAdapter> cameraStatusCallback)
{
    if (!ctocpp_) {
        return -1;
    }
    if (CHECK_SHARED_PTR_IS_NULL(cameraStatusCallback)) {
        return ctocpp_->Create(nullptr);
    }

    return ctocpp_->Create(new ArkCameraStatusCallbackAdapterImpl(cameraStatusCallback));
}

std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>> ArkCameraManagerAdapterWrapper::GetDevicesInfo()
{
    if (!ctocpp_) {
        return std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>>();
    }

    ArkVideoDeviceDescriptorAdapterVector ark_vector = ctocpp_->GetDevicesInfo();

    std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>> result =
        ArkVideoDeviceDescriptorAdapterVectorStructToClass(ark_vector);

    ArkVideoDeviceDescriptorAdapterVectorStructRelease(ark_vector);
    return result;
}

int32_t ArkCameraManagerAdapterWrapper::ReleaseCameraManger()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->ReleaseCameraManger();
}

int32_t ArkCameraManagerAdapterWrapper::GetExposureModes(std::vector<NWeb::ExposureModeAdapter>& exposureModesAdapter)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebInt32Vector vec = {0, nullptr, nullptr};
    int32_t result = ctocpp_->GetExposureModes(vec);
    for (int count = 0; count < vec.size; count++) {
        exposureModesAdapter.push_back((NWeb::ExposureModeAdapter)vec.value[count]);
    }

    ArkWebBasicVectorStructRelease(vec);

    return result;
}

int32_t ArkCameraManagerAdapterWrapper::GetCurrentExposureMode(NWeb::ExposureModeAdapter& exposureModeAdapter)
{
    if (!ctocpp_) {
        return -1;
    }
    int32_t temp;
    int32_t result = ctocpp_->GetCurrentExposureMode(temp);
    exposureModeAdapter = (NWeb::ExposureModeAdapter)temp;
    return result;
}

std::shared_ptr<NWeb::VideoCaptureRangeAdapter> ArkCameraManagerAdapterWrapper::GetCaptionRangeById(
    NWeb::RangeIDAdapter rangeId)
{
    if (!ctocpp_) {
        return nullptr;
    }

    ArkWebRefPtr<ArkVideoCaptureRangeAdapter> adapter = ctocpp_->GetCaptionRangeById((int32_t)rangeId);
    if (CHECK_REF_PTR_IS_NULL(adapter)) {
        return nullptr;
    }

    return std::make_shared<ArkVideoCaptureRangeAdapterWrapper>(adapter);
}

bool ArkCameraManagerAdapterWrapper::IsFocusModeSupported(NWeb::FocusModeAdapter focusMode)
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsFocusModeSupported((int32_t)focusMode);
}

NWeb::FocusModeAdapter ArkCameraManagerAdapterWrapper::GetCurrentFocusMode()
{
    if (!ctocpp_) {
        return NWeb::FocusModeAdapter::FOCUS_MODE_MANUAL;
    }
    return (NWeb::FocusModeAdapter)ctocpp_->GetCurrentFocusMode();
}

bool ArkCameraManagerAdapterWrapper::IsFlashModeSupported(NWeb::FlashModeAdapter flashMode)
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsFlashModeSupported((int32_t)flashMode);
}

int32_t ArkCameraManagerAdapterWrapper::RestartSession()
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->RestartSession();
}

int32_t ArkCameraManagerAdapterWrapper::StopSession(NWeb::CameraStopType stopType)
{
    if (!ctocpp_) {
        return -1;
    }
    return ctocpp_->StopSession((int32_t)stopType);
}

NWeb::CameraStatusAdapter ArkCameraManagerAdapterWrapper::GetCameraStatus()
{
    if (!ctocpp_) {
        return NWeb::CameraStatusAdapter::UNAVAILABLE;
    }
    return (NWeb::CameraStatusAdapter)ctocpp_->GetCameraStatus();
}

bool ArkCameraManagerAdapterWrapper::IsExistCaptureTask()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsExistCaptureTask();
}

int32_t ArkCameraManagerAdapterWrapper::StartStream(const std::string& deviceId,
    const std::shared_ptr<NWeb::VideoCaptureParamsAdapter> captureParams,
    std::shared_ptr<NWeb::CameraBufferListenerAdapter> listener)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString str = ArkWebStringClassToStruct(deviceId);
    int32_t result;

    if (CHECK_SHARED_PTR_IS_NULL(listener) && CHECK_SHARED_PTR_IS_NULL(captureParams)) {
        result = ctocpp_->StartStream(str, nullptr, nullptr);
    } else if (!CHECK_SHARED_PTR_IS_NULL(listener) && !CHECK_SHARED_PTR_IS_NULL(captureParams)) {
        result = ctocpp_->StartStream(
            str, new ArkVideoCaptureParamsAdapterImpl(captureParams), new ArkCameraBufferListenerAdapterImpl(listener));
    } else if (CHECK_SHARED_PTR_IS_NULL(listener)) {
        result = ctocpp_->StartStream(str, new ArkVideoCaptureParamsAdapterImpl(captureParams), nullptr);
    } else {
        result = ctocpp_->StartStream(str, nullptr, new ArkCameraBufferListenerAdapterImpl(listener));
    }

    ArkWebStringStructRelease(str);
    return result;
}

void ArkCameraManagerAdapterWrapper::SetForegroundFlag(bool isForeground)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->SetForegroundFlag(isForeground);
}

void ArkCameraManagerAdapterWrapper::SetCameraStatus(NWeb::CameraStatusAdapter status)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->SetCameraStatus((int32_t)status);
}

std::string ArkCameraManagerAdapterWrapper::GetCurrentDeviceId()
{
    if (!ctocpp_) {
        return "";
    }
    ArkWebString str = ctocpp_->GetCurrentDeviceId();
    std::string result = ArkWebStringStructToClass(str);
    ArkWebStringStructRelease(str);
    return result;
}

} // namespace OHOS::ArkWeb
