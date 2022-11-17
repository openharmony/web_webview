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

#ifndef DISPLAY_MANAGER_ADAPTER_H
#define DISPLAY_MANAGER_ADAPTER_H

#include <memory>
#include <refbase.h>

namespace OHOS::NWeb {
using DisplayId = uint64_t;
enum class RotationType : uint32_t {
    ROTATION_0,
    ROTATION_90,
    ROTATION_180,
    ROTATION_270,
    ROTATION_BUTT,
};

enum class OrientationType : uint32_t {
    UNSPECIFIED = 0,
    VERTICAL = 1,
    HORIZONTAL = 2,
    REVERSE_VERTICAL = 3,
    REVERSE_HORIZONTAL = 4,
    SENSOR = 5,
    SENSOR_VERTICAL = 6,
    SENSOR_HORIZONTAL = 7,
    BUTT,
};

class DisplayListenerAdapter {
public:
    DisplayListenerAdapter() = default;

    virtual ~DisplayListenerAdapter() = default;

    virtual void OnCreate(DisplayId) = 0;
    
    virtual void OnDestroy(DisplayId) = 0;
    
    virtual void OnChange(DisplayId) = 0;
};

class DisplayAdapter {
public:
    DisplayAdapter() = default;

    virtual ~DisplayAdapter() = default;

    virtual DisplayId GetId() = 0;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual float GetVirtualPixelRatio() = 0;

    virtual RotationType GetRotation() = 0;

    virtual OrientationType GetOrientation() = 0;
};

class DisplayManagerAdapter {
public:
    DisplayManagerAdapter() = default;

    virtual ~DisplayManagerAdapter() = default;

    virtual DisplayId GetDefaultDisplayId() = 0;

    virtual std::shared_ptr<DisplayAdapter> GetDefaultDisplay() = 0;

    virtual bool RegisterDisplayListener(std::shared_ptr<DisplayListenerAdapter> listener) = 0;

    virtual bool UnregisterDisplayListener(std::shared_ptr<DisplayListenerAdapter> listener) = 0;

    virtual bool IsDefaultPortrait() = 0;
};
}  // namespace OHOS::NWeb

#endif  // DISPLAY_MANAGER_ADAPTER_H