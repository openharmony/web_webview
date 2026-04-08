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

#ifndef ARK_CAMERA_MANAGER_ADAPTER_H
#define ARK_CAMERA_MANAGER_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_format_adapter_vector.h"
#include "ohos_adapter/include/ark_video_device_descriptor_adapter_vector.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkVideoControlSupportAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool GetPan() = 0;

    /*--ark web()--*/
    virtual bool GetTilt() = 0;

    /*--ark web()--*/
    virtual bool GetZoom() = 0;
};

/*--ark web(source=webview)--*/
class ArkFormatAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual uint32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual float GetFrameRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetPixelFormat() = 0;
};

/*--ark web(source=webview)--*/
class ArkVideoDeviceDescriptorAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebString GetDisplayName() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetDeviceId() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetModelId() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkVideoControlSupportAdapter> GetControlSupport() = 0;

    /*--ark web()--*/
    virtual int32_t GetTransportType() = 0;

    /*--ark web()--*/
    virtual int32_t GetFacingMode() = 0;

    /*--ark web()--*/
    virtual ArkFormatAdapterVector GetSupportCaptureFormats() = 0;
};

/*--ark web(source=webcore)--*/
class ArkVideoCaptureParamsAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual uint32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual float GetFrameRate() = 0;

    /*--ark web()--*/
    virtual int32_t GetPixelFormat() = 0;

    /*--ark web()--*/
    virtual bool GetEnableFaceDetection() = 0;
};

/*--ark web(source=webview)--*/
class ArkVideoCaptureRangeAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual double GetMin() = 0;

    /*--ark web()--*/
    virtual double GetMax() = 0;

    /*--ark web()--*/
    virtual double GetStep() = 0;

    /*--ark web()--*/
    virtual double GetCurrent() = 0;
};

/*--ark web(source=webview)--*/
class ArkCameraRotationInfoAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetRotation() = 0;

    /*--ark web()--*/
    virtual bool GetIsFlipX() = 0;

    /*--ark web()--*/
    virtual bool GetIsFlipY() = 0;
};

/*--ark web(source=webview)--*/
class ArkCameraSurfaceBufferAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetFileDescriptor() = 0;

    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual int32_t GetStride() = 0;

    /*--ark web()--*/
    virtual int32_t GetFormat() = 0;

    /*--ark web()--*/
    virtual uint32_t GetSize() = 0;

    /*--ark web()--*/
    virtual uint8_t* GetBufferAddr() = 0;
};

/*--ark web(source=webview)--*/
class ArkCameraSurfaceAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t ReleaseBuffer(ArkWebRefPtr<ArkCameraSurfaceBufferAdapter> buffer, int32_t fence) = 0;
};

/*--ark web(source=webcore)--*/
class ArkCameraBufferListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnBufferAvailable(ArkWebRefPtr<ArkCameraSurfaceAdapter> surface,
        ArkWebRefPtr<ArkCameraSurfaceBufferAdapter> buffer,
        ArkWebRefPtr<ArkCameraRotationInfoAdapter> rotationInfo) = 0;
};

/*--ark web(source=webcore)--*/
class ArkCameraStatusCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnCameraStatusChanged(int32_t cameraStatusAdapter, const ArkWebString callBackDeviceId) = 0;

    /*--ark web()--*/
    virtual void OnFlashlightStatusChanged(int32_t flashStatusAdapter, const ArkWebString cameraID) = 0;
};

/*--ark web(source=webview)--*/
class ArkCameraManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t Create(ArkWebRefPtr<ArkCameraStatusCallbackAdapter> cameraStatusCallback) = 0;

    /*--ark web()--*/
    virtual ArkVideoDeviceDescriptorAdapterVector GetDevicesInfo() = 0;

    /*--ark web()--*/
    virtual int32_t ReleaseCameraManger() = 0;

    /*--ark web()--*/
    virtual int32_t GetExposureModes(ArkWebInt32Vector& exposureModesAdapter) = 0;

    /*--ark web()--*/
    virtual int32_t GetCurrentExposureMode(int32_t& exposureModeAdapter) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkVideoCaptureRangeAdapter> GetCaptionRangeById(int32_t rangeId) = 0;

    /*--ark web()--*/
    virtual bool IsFocusModeSupported(int32_t focusMode) = 0;

    /*--ark web()--*/
    virtual int32_t GetCurrentFocusMode() = 0;

    /*--ark web()--*/
    virtual bool IsFlashModeSupported(int32_t flashMode) = 0;

    /*--ark web()--*/
    virtual int32_t RestartSession() = 0;

    /*--ark web()--*/
    virtual int32_t StopSession(int32_t stopType) = 0;

    /*--ark web()--*/
    virtual int32_t GetCameraStatus() = 0;

    /*--ark web()--*/
    virtual bool IsExistCaptureTask() = 0;

    /*--ark web()--*/
    virtual int32_t StartStream(const ArkWebString& deviceId,
        const ArkWebRefPtr<ArkVideoCaptureParamsAdapter> captureParams,
        ArkWebRefPtr<ArkCameraBufferListenerAdapter> listener) = 0;

    /*--ark web()--*/
    virtual void SetForegroundFlag(bool isForeground) = 0;

    /*--ark web()--*/
    virtual void SetCameraStatus(int32_t status) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetCurrentDeviceId() = 0;

    /*--ark web()--*/
    virtual int32_t GetSupportedFlashModes(ArkWebInt32Vector& flashModesAdapter) = 0;

    /*--ark web()--*/
    virtual bool HasFlash() = 0;

    /*--ark web()--*/
    virtual int32_t GetCurrentFlashMode(int32_t& flashModeAdapter) = 0;

    /*--ark web()--*/
    virtual int32_t SetFlashMode(int32_t flashMode) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_CAMERA_MANAGER_ADAPTER_H
