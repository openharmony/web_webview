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

#ifndef NWEB_CONTEXT_MENU_PARAMS_H
#define NWEB_CONTEXT_MENU_PARAMS_H

#include <memory>
#include <string>

#include "nweb_export.h"
#include "nweb_touch_handle_state.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebContextMenuParams {
public:
    enum ContextMenuTypeFlags {
        CM_TF_NONE = 0,
        CM_TF_PAGE = 1 << 0,
        CM_TF_FRAME = 1 << 1,
        CM_TF_LINK = 1 << 2,
        CM_TF_MEDIA = 1 << 3,
        CM_TF_SELECTION = 1 << 4,
        CM_TF_EDITABLE = 1 << 5,
    };

    enum ContextMenuMediaType {
        CM_MT_NONE,
        CM_MT_IMAGE,
    };

    enum ContextMenuEditStateFlags {
        CM_ES_NONE = 0,
        CM_ES_CAN_CUT = 1 << 0,
        CM_ES_CAN_COPY = 1 << 1,
        CM_ES_CAN_PASTE = 1 << 2,
        CM_ES_CAN_SELECT_ALL = 1 << 3,
    };

    enum ContextMenuInputFieldType {
        CM_IT_NONE = 0,
        CM_IT_PLAINTEXT = 1,
        CM_IT_PASSWORD = 2,
        CM_IT_NUMBER = 3,
        CM_IT_TELEPHONE = 4,
        CM_IT_OTHER = 5,
    };

    enum ContextMenuSourceType {
        CM_ST_NONE = 0,
        CM_ST_MOUSE = 1,
        CM_ST_LONG_PRESS = 2,
    };

    virtual ~NWebContextMenuParams() = default;

    virtual int32_t GetXCoord() = 0;

    virtual int32_t GetYCoord() = 0;

    virtual int32_t GetContextMenuTypeFlags() = 0;

    virtual std::string GetLinkUrl() = 0;

    virtual std::string GetUnfilteredLinkUrl() = 0;

    virtual std::string GetSourceUrl() = 0;

    virtual bool HasImageContents() = 0;

    virtual std::string GetTitleText() = 0;

    virtual std::string GetPageUrl() = 0;

    virtual ContextMenuMediaType GetMediaType() = 0;

    virtual bool IsEditable() = 0;

    virtual int32_t GetEditStateFlags() = 0;

    virtual ContextMenuSourceType GetSourceType() = 0;

    virtual ContextMenuInputFieldType GetInputFieldType() = 0;

    virtual std::string GetSelectionText() = 0;
};

class OHOS_NWEB_EXPORT NWebQuickMenuParams {
public:
    enum QuickMenuEditStateFlags {
        QM_EF_NONE = 0,
        QM_EF_CAN_ELLIPSIS = 1 << 0,
        QM_EF_CAN_CUT = 1 << 1,
        QM_EF_CAN_COPY = 1 << 2,
        QM_EF_CAN_PASTE = 1 << 3,
        QM_EF_CAN_SELECT_ALL = 1 << 4,
    };

    virtual ~NWebQuickMenuParams() = default;

    virtual int32_t GetXCoord() = 0;

    virtual int32_t GetYCoord() = 0;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual int32_t GetEditStateFlags() = 0;

    virtual std::shared_ptr<NWebTouchHandleState> GetTouchHandleState(
        NWebTouchHandleState::TouchHandleType type) = 0;
};

enum MenuEventFlags {
    EF_NONE = 0,
    EF_CAPS_LOCK_ON = 1 << 0,
    EF_SHIFT_DOWN = 1 << 1,
    EF_CONTROL_DOWN = 1 << 2,
    EF_ALT_DOWN = 1 << 3,
    EF_LEFT_MOUSE_BUTTON = 1 << 4,
    EF_MIDDLE_MOUSE_BUTTON = 1 << 5,
    EF_RIGHT_MOUSE_BUTTON = 1 << 6,
};

enum MenuCommandId {
    CI_IMAGE_COPY = 0,
    CI_COPY = 1,
    CI_PASTE = 2,
    CI_CUT = 3,
    CI_SELECT_ALL = 4,
    CI_DELETE = 5,
};

class OHOS_NWEB_EXPORT NWebContextMenuCallback {
public:
    virtual ~NWebContextMenuCallback() = default;

    virtual void Continue(int32_t commandId, MenuEventFlags flag) = 0;

    virtual void Cancel() = 0;
};

class OHOS_NWEB_EXPORT NWebQuickMenuCallback {
public:
    virtual ~NWebQuickMenuCallback() = default;

    virtual void Continue(int32_t commandId, MenuEventFlags flag) = 0;

    virtual void Cancel() = 0;
};
}
#endif