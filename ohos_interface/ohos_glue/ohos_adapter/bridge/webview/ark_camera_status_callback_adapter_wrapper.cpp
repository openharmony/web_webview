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

#include "ohos_adapter/bridge/ark_camera_status_callback_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkCameraStatusCallbackAdapterWrapper::ArkCameraStatusCallbackAdapterWrapper(
    ArkWebRefPtr<ArkCameraStatusCallbackAdapter> ref)
    : ctocpp_(ref)
{}

void ArkCameraStatusCallbackAdapterWrapper::OnCameraStatusChanged(
    NWeb::CameraStatusAdapter cameraStatusAdapter, const std::string callBackDeviceId)
{
    ArkWebString str = ArkWebStringClassToStruct(callBackDeviceId);
    ctocpp_->OnCameraStatusChanged((int32_t)cameraStatusAdapter, str);
    ArkWebStringStructRelease(str);
}

void ArkCameraStatusCallbackAdapterWrapper::OnFlashlightStatusChanged(
    NWeb::FlashStatusAdapter flashStatusAdapter, const std::string cameraID)
{
    ArkWebString str = ArkWebStringClassToStruct(cameraID);
    ctocpp_->OnFlashlightStatusChanged((int32_t)flashStatusAdapter, str);
    ArkWebStringStructRelease(str);
}

} // namespace OHOS::ArkWeb
