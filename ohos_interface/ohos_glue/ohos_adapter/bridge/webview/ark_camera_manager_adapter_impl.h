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

#ifndef ARK_CAMERA_MANAGER_ADAPTER_IMPL_H
#define ARK_CAMERA_MANAGER_ADAPTER_IMPL_H
#pragma once

#include "camera_manager_adapter.h"
#include "ohos_adapter/include/ark_camera_manager_adapter.h"

namespace OHOS::ArkWeb {

class ArkCameraManagerAdapterImpl : public ArkCameraManagerAdapter {
public:
    explicit ArkCameraManagerAdapterImpl(NWeb::CameraManagerAdapter&);

    int32_t Create(ArkWebRefPtr<ArkCameraStatusCallbackAdapter> cameraStatusCallback) override;

    ArkVideoDeviceDescriptorAdapterVector GetDevicesInfo() override;

    int32_t ReleaseCameraManger() override;

    int32_t GetExposureModes(ArkWebInt32Vector& exposureModesAdapter) override;

    int32_t GetCurrentExposureMode(int32_t& exposureModeAdapter) override;

    ArkWebRefPtr<ArkVideoCaptureRangeAdapter> GetCaptionRangeById(int32_t rangeId) override;

    bool IsFocusModeSupported(int32_t focusMode) override;

    int32_t GetCurrentFocusMode() override;

    bool IsFlashModeSupported(int32_t flashMode) override;

    int32_t GetSupportedFlashModes(ArkWebInt32Vector& flashModesAdapter) override;

    bool HasFlash() override;

    int32_t GetCurrentFlashMode(int32_t& flashModeAdapter) override;

    int32_t SetFlashMode(int32_t flashMode) override;

    int32_t RestartSession() override;

    int32_t StopSession(int32_t stopType) override;

    int32_t GetCameraStatus() override;

    bool IsExistCaptureTask() override;

    int32_t StartStream(const ArkWebString& deviceId, const ArkWebRefPtr<ArkVideoCaptureParamsAdapter> captureParams,
        ArkWebRefPtr<ArkCameraBufferListenerAdapter> listener) override;

    void SetForegroundFlag(bool isForeground) override;

    void SetCameraStatus(int32_t status) override;

    ArkWebString GetCurrentDeviceId() override;

private:
    NWeb::CameraManagerAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkCameraManagerAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_CAMERA_MANAGER_ADAPTER_IMPL_H
