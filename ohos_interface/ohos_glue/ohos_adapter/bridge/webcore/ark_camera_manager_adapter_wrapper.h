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
#ifndef ARK_CAMERA_MANAGER_ADAPTER_WRAPPER_H
#define ARK_CAMERA_MANAGER_ADAPTER_WRAPPER_H
#pragma once

#include "camera_manager_adapter.h"
#include "ohos_adapter/include/ark_camera_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkCameraManagerAdapterWrapper : public OHOS::NWeb::CameraManagerAdapter {
public:
    explicit ArkCameraManagerAdapterWrapper(ArkWebRefPtr<ArkCameraManagerAdapter>);

    int32_t Create(std::shared_ptr<NWeb::CameraStatusCallbackAdapter> cameraStatusCallback) override;

    std::vector<std::shared_ptr<NWeb::VideoDeviceDescriptorAdapter>> GetDevicesInfo() override;

    int32_t ReleaseCameraManger() override;

    int32_t GetExposureModes(std::vector<NWeb::ExposureModeAdapter>& exposureModesAdapter) override;

    int32_t GetCurrentExposureMode(NWeb::ExposureModeAdapter& exposureModeAdapter) override;

    std::shared_ptr<NWeb::VideoCaptureRangeAdapter> GetCaptionRangeById(NWeb::RangeIDAdapter rangeId) override;

    bool IsFocusModeSupported(NWeb::FocusModeAdapter focusMode) override;

    NWeb::FocusModeAdapter GetCurrentFocusMode() override;

    bool IsFlashModeSupported(NWeb::FlashModeAdapter flashMode) override;

    int32_t GetSupportedFlashModes(std::vector<NWeb::FlashModeAdapter>& flashModesAdapter) override;

    bool HasFlash() override;

    int32_t GetCurrentFlashMode(NWeb::FlashModeAdapter& flashModeAdapter) override;

    int32_t SetFlashMode(NWeb::FlashModeAdapter flashMode) override;

    int32_t RestartSession() override;

    int32_t StopSession(NWeb::CameraStopType stopType) override;

    NWeb::CameraStatusAdapter GetCameraStatus() override;

    bool IsExistCaptureTask() override;

    int32_t StartStream(const std::string& deviceId,
        const std::shared_ptr<NWeb::VideoCaptureParamsAdapter> captureParams,
        std::shared_ptr<NWeb::CameraBufferListenerAdapter> listener) override;

    void SetForegroundFlag(bool isForeground) override;

    void SetCameraStatus(NWeb::CameraStatusAdapter status) override;

    std::string GetCurrentDeviceId() override;

private:
    ArkWebRefPtr<ArkCameraManagerAdapter> ctocpp_;
};
} // namespace OHOS::ArkWeb

#endif // ARK_CAMERA_MANAGER_ADAPTER_WRAPPER_H
