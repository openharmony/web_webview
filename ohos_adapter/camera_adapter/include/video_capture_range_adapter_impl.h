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

#ifndef VIDEO_CAPTURE_RANGE_ADAPTER_IMPL_H
#define VIDEO_CAPTURE_RANGE_ADAPTER_IMPL_H

#include "camera_manager_adapter.h"

namespace OHOS::NWeb {

class VideoCaptureRangeAdapterImpl : public VideoCaptureRangeAdapter {
public:
    VideoCaptureRangeAdapterImpl() = default;

    double GetMin() override;

    double GetMax() override;

    double GetStep() override;

    double GetCurrent() override;

    void SetMin(double min);

    void SetMax(double max);

    void SetStep(double step);

    void SetCurrent(double current);

private:
    double min_ = 0.0;
    double max_ = 0.0;
    double step_ = 0.0;
    double current_ = 0.0;
};

} // namespace OHOS::NWeb

#endif // VIDEO_CAPTURE_RANGE_ADAPTER_IMPL_H
