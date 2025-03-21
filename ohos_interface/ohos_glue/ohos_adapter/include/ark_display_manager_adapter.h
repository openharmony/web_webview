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

#ifndef ARK_DISPLAY_MANAGER_ADAPTER_H
#define ARK_DISPLAY_MANAGER_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_display_adapter_vector.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkDisplayListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnCreate(uint64_t displayId) = 0;

    /*--ark web()--*/
    virtual void OnDestroy(uint64_t displayId) = 0;

    /*--ark web()--*/
    virtual void OnChange(uint64_t displayId) = 0;
};

/*--ark web(source=webcore)--*/
class ArkFoldStatusListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @Description: Folding state change.
     * @Input: foldstatus:folding status,value range id 0-4
     * 0 represents an unknown screen state,
     * 1 represents fully expanded,
     * 2 stands for using the home screen,
     * 3 stands for the use of the secondary screen,
     * 4 represents simultaneous use,
     * @Since: 14001
     */
    /*--ark web()--*/
    virtual void OnFoldStatusChanged(uint32_t foldstatus) = 0;
};

/*--ark web(source=webview)--*/
class ArkDisplayAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint64_t GetId() = 0;

    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual float GetVirtualPixelRatio() = 0;

    /*--ark web()--*/
    virtual uint32_t GetRotation() = 0;

    /*--ark web()--*/
    virtual uint32_t GetOrientation() = 0;

    /*--ark web()--*/
    virtual int32_t GetDpi() = 0;

    /*--ark web()--*/
    virtual uint32_t GetDisplayOrientation() = 0;

    /**
     * @Description: Get folding status.
     * @Return: folding status,value range id 0-4
     * 0 represents an unknown screen state,
     * 1 represents fully expanded,
     * 2 stands for using the home screen,
     * 3 stands for the use of the secondary screen,
     * 4 represents simultaneous use,
     * @Since: 14001
     */
    /*--ark web()--*/
    virtual uint32_t GetFoldStatus() = 0;

    /**
     * @Description: Does the device support folding.
     * @Return: Does it support folding.
     * @Since: 14001
     */
    /*--ark web()--*/
    virtual bool IsFoldable() = 0;

    /**
     * @Description: Get display name.
     * @Return: Display name.
     */
    /*--ark web()--*/
    virtual ArkWebString GetName() = 0;

    /**
     * @Description: Get display available width.
     * @Return: Display available width.
     */
    /*--ark web()--*/
    virtual int32_t GetAvailableWidth() = 0;

    /**
     * @Description: Get display available height.
     * @Return: Display available height.
     */
    /*--ark web()--*/
    virtual int32_t GetAvailableHeight() = 0;

    /**
     * @Description: Get display alive status.
     * @Return: Display alive status.
     */
    /*--ark web()--*/
    virtual bool GetAliveStatus() = 0;

    /**
     * @Description: Get display state.
     * @Return: Display state.
     */
    /*--ark web()--*/
    virtual uint32_t GetDisplayState() = 0;

    /**
     * @Description: Get density DPI.
     * @Return: Density DPI.
     */
    /*--ark web()--*/
    virtual int32_t GetDensityDpi() = 0;

    /**
     * @Description: Get display position X.
     * @Return: Position X.
     */
    /*--ark web()--*/
    virtual int32_t GetX() = 0;

    /**
     * @Description: Get display position Y.
     * @Return: Position Y.
     */
    /*--ark web()--*/
    virtual int32_t GetY() = 0;

    /**
     * @Description: Get display source mode.
     * @Return: Display source mode.
     */
    /*--ark web()--*/
    virtual uint32_t GetDisplaySourceMode() = 0;

    /**
     * @Description: Get display physical width.
     * @Return: Display physical width.
     */
    /*--ark web()--*/
    virtual int32_t GetPhysicalWidth() = 0;

    /**
     * @Description: Get display physical Height.
     * @Return: Display physical height.
     */
    /*--ark web()--*/
    virtual int32_t GetPhysicalHeight() = 0;

    /**
     * @Description: Get default virtual pixel ratio.
     * @Return: Default virtual pixel ratio.
     */
    /*--ark web()--*/
    virtual float GetDefaultVirtualPixelRatio() = 0;
};

/*--ark web(source=webview)--*/
class ArkDisplayManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual uint64_t GetDefaultDisplayId() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkDisplayAdapter> GetDefaultDisplay() = 0;

    /*--ark web()--*/
    virtual uint32_t RegisterDisplayListener(ArkWebRefPtr<ArkDisplayListenerAdapter> listener) = 0;

    /*--ark web()--*/
    virtual bool UnregisterDisplayListener(uint32_t id) = 0;

    /*--ark web()--*/
    virtual bool IsDefaultPortrait() = 0;

    /**
     * @Description: Regist Fold Status Listener.
     * @Input: ArkFoldStatusListenerAdapter object.
     * @Return:folding status,value range id 0-4
     * 0 represents an unknown screen state,
     * 1 represents fully expanded,
     * 2 stands for using the home screen,
     * 3 stands for the use of the secondary screen,
     * 4 represents simultaneous use,
     * @Since: 14001
     */
    /*--ark web()--*/
    virtual uint32_t RegisterFoldStatusListener(ArkWebRefPtr<ArkFoldStatusListenerAdapter> listener) = 0;

    /**
     * @Description: Unregist Fold Status Listener.
     * @Input: Listener ID.
     * @Return: return is or not success.
     * @Since: 14001
     */
    /*--ark web()--*/
    virtual bool UnregisterFoldStatusListener(uint32_t id) = 0;

    /**
     * @Description: Get primary display.
     * @Return: Primary display.
     */
    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkDisplayAdapter> GetPrimaryDisplay() = 0;

    /**
     * @Description: Get all displays list.
     * @Return: All displays list.
     */
    /*--ark web()--*/
    virtual ArkDisplayAdapterVector GetAllDisplays() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_MANAGER_ADAPTER_H
