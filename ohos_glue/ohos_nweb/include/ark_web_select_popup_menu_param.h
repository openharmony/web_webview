/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_SELECT_POPUP_MENU_PARAM_H_
#define ARK_WEB_SELECT_POPUP_MENU_PARAM_H_
#pragma once

#include "ohos_nweb/include/ark_web_select_menu_bound.h"
#include "ohos_nweb/include/ark_web_select_popup_menu_item_vector.h"

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkWebSelectPopupMenuParam : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual ArkWebSelectPopupMenuItemVector GetMenuItems() = 0;

    /*--ark web()--*/
    virtual int GetItemHeight() = 0;

    /*--ark web()--*/
    virtual int GetSelectedItem() = 0;

    /*--ark web()--*/
    virtual double GetItemFontSize() = 0;

    /*--ark web()--*/
    virtual bool GetIsRightAligned() = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkWebSelectMenuBound> GetSelectMenuBound() = 0;

    /*--ark web()--*/
    virtual bool GetIsAllowMultipleSelection() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_SELECT_POPUP_MENU_PARAM_H_