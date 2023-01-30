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

#ifndef NWEB_SELECT_POPUP_MENU_H
#define NWEB_SELECT_POPUP_MENU_H

#include <memory>
#include <string>
#include <vector>

namespace OHOS::NWeb {

struct SelectMenuBound {
    int x = -1;
    int y = -1;
    int width = -1;
    int height = -1;
};

enum SelectPopupMenuItemType {
    SP_OPTION,
    SP_CHECKABLE_OPTION,
    SP_GROUP,
    SP_SEPARATOR,
    SP_SUBMENU,
};

enum TextDirection {
    SP_UNKNOWN,
    SP_RTL,
    SP_LTR,
};

struct SelectPopupMenuItem {
    std::string label = "";
    std::string toolTip = "";
    SelectPopupMenuItemType type = SP_OPTION;
    uint32_t action = 0;
    TextDirection textDirection = SP_UNKNOWN;
    bool enabled = false;
    bool hasTextDirectionOverride = false;
    bool checked = false;
};

struct NWebSelectPopupMenuParam {
    SelectMenuBound bounds;
    int itemHeight = -1;
    double itemFontSize = -1;
    int selectedItem = -1;
    std::vector<SelectPopupMenuItem> menuItems;
    bool rightAligned = false;
    bool allowMultipleSelection = false;
};

class NWebSelectPopupMenuCallback {
public:
    virtual ~NWebSelectPopupMenuCallback() = default;

    virtual void Continue(const std::vector<int32_t>& indices) = 0;

    virtual void Cancel() = 0;
};
}
#endif