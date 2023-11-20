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

#ifndef NWEB_ACCESSIBILITY_ACCESSIBILITY_NODE_INFO_H
#define NWEB_ACCESSIBILITY_ACCESSIBILITY_NODE_INFO_H

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {
struct OHOS_NWEB_EXPORT NWebAccessibilityNodeInfo {
public:
    int32_t accessibilityId = -1;
    size_t itemCounts = 0;
    int32_t currentIndex = 0;
    int32_t startIndex = 0;
    int32_t endIndex = 0;
    int32_t selectionStart = 0;
    int32_t selectionEnd = 0;
    int32_t inputType = -1;
    int32_t liveRegion = -1;
    std::string componentType;
    std::string hint;
    std::string content;
    std::string error;
    std::vector<int32_t> childIds;
    int32_t parentId = -1;
    int32_t gridRows = -1;
    int32_t gridColumns = -1;
    int32_t gridSelectedMode = -1;
    int32_t gridItemRow = -1;
    int32_t gridItemRowSpan = -1;
    int32_t gridItemColumn = -1;
    int32_t gridItemColumnSpan = -1;
    bool heading = false;
    std::vector<uint32_t> actions;
    int32_t pageId = -1;

    size_t rectX = 0;
    size_t rectY = 0;
    size_t rectWidth = 0;
    size_t rectHeight = 0;

    bool checked = false;
    bool editable : 1;
    bool enabled : 1;
    bool focused : 1;
    bool selected : 1;
    bool checkable : 1;
    bool clickable : 1;
    bool focusable : 1;
    bool scrollable : 1;
    bool password : 1;
    bool visible : 1;
    bool clicked : 1;
    bool hinting : 1;
    bool pluralLineSupported : 1;
    bool popupSupported : 1;
    bool contentInvalid : 1;
    bool deletable : 1;
    bool accessibilityFocus : 1;

};
}  // namespace OHOS::NWeb

#endif  // NWEB_ACCESSIBILITY_ACCESSIBILITY_NODE_INFO_H