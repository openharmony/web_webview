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

#ifndef CAMERA_ROTATION_INFO_ADAPTER_IMPL_H
#define CAMERA_ROTATION_INFO_ADAPTER_IMPL_H

#include "camera_manager_adapter.h"

namespace OHOS::NWeb {

class CameraRotationInfoAdapterImpl : public CameraRotationInfoAdapter {
public:
    CameraRotationInfoAdapterImpl() = default;

    int32_t GetRotation() override;

    bool GetIsFlipX() override;

    bool GetIsFlipY() override;

    void SetRotation(int32_t rotation);

    void SetIsFlipX(bool value);

    void SetIsFlipY(bool value);

private:
    int32_t rotation_ = 0;
    bool is_flip_x_ = false;
    bool is_flip_y_ = false;
};

} // namespace OHOS::NWeb

#endif // CAMERA_ROTATION_INFO_ADAPTER_IMPL_H
