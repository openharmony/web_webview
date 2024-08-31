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

#ifndef VIDEO_DEVICE_DESCRIPTOR_ADAPTER_IMPL_H
#define VIDEO_DEVICE_DESCRIPTOR_ADAPTER_IMPL_H

#include "camera_manager_adapter.h"

namespace OHOS::NWeb {

class VideoDeviceDescriptorAdapterImpl : public VideoDeviceDescriptorAdapter {
public:
    VideoDeviceDescriptorAdapterImpl() = default;

    std::string GetDisplayName() override;

    std::string GetDeviceId() override;

    std::string GetModelId() override;

    std::shared_ptr<VideoControlSupportAdapter> GetControlSupport() override;

    VideoTransportType GetTransportType() override;

    VideoFacingModeAdapter GetFacingMode() override;

    std::vector<std::shared_ptr<FormatAdapter>> GetSupportCaptureFormats() override;

    void SetDisplayName(std::string name);

    void SetDeviceId(std::string deviceId);

    void SetModelId(std::string modelId);

    void SetControlSupport(std::shared_ptr<VideoControlSupportAdapter>);

    void SetTransportType(VideoTransportType);

    void SetFacingMode(VideoFacingModeAdapter);

    void SetSupportCaptureFormats(std::vector<std::shared_ptr<FormatAdapter>>);

private:
    std::string display_name_ = "";
    std::string device_id_ = "";
    std::string model_id_ = "";
    std::shared_ptr<VideoControlSupportAdapter> control_support_ = nullptr;
    VideoTransportType transport_type_ = VideoTransportType::VIDEO_TRANS_TYPE_OTHER;
    VideoFacingModeAdapter facing_ = VideoFacingModeAdapter::FACING_NONE;
    std::vector<std::shared_ptr<FormatAdapter>> support_formats_;
};

} // namespace OHOS::NWeb
#endif // VIDEO_DEVICE_DESCRIPTOR_ADAPTER_IMPL_H
