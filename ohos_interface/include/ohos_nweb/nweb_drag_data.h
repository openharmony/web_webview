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

#ifndef NWEB_DRAG_DATA_H
#define NWEB_DRAG_DATA_H

#include <memory>
#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebDragData {
public:
    ///
    // "Verb" of a drag-and-drop operation as negotiated between the source and
    // destination. These constants match their equivalents in WebCore's
    // DragActions.h and should not be renumbered.
    ///
    enum class DragOperation : unsigned char {
        DRAG_OPERATION_NONE = 0,
        DRAG_OPERATION_COPY = 1,
        DRAG_OPERATION_LINK = 2,
        DRAG_OPERATION_GENERIC = 4,
        DRAG_OPERATION_PRIVATE = 8,
        DRAG_OPERATION_MOVE = 16,
        DRAG_OPERATION_DELETE = 32,
        DRAG_OPERATION_EVERY = UINT_MAX
    };

    // Mask of the allowed drag-and-drop operations.
    // These constants match their equivalents in NSDragOperation and
    // should not be renumbered.
    enum class DragOperationsMask {
        DRAG_ALLOW_NONE = 0,
        DRAG_ALLOW_COPY = 1,
        DRAG_ALLOW_LINK = 2,
        DRAG_ALLOW_MOVE = 16,
        DRAG_ALLOW_EVERY = UINT_MAX
    };
    NWebDragData() = default;
    virtual ~NWebDragData() = default;

    // get the link URL that is being dragged.
    virtual std::string GetLinkURL() = 0;

    // get the plain text that is being dragged.
    virtual std::string GetFragmentText() = 0;

    // get the text/html fragment that is being dragged.
    virtual std::string GetFragmentHtml() = 0;

    // get the image representation of drag data.
    virtual bool GetPixelMapSetting(const void** data, size_t& len, int& width, int& height) = 0;

    // set the text/html fragment that is being dragged.
    virtual bool SetFragmentHtml(const std::string& html) = 0;

    // set the image representation of drag data.
    virtual bool SetPixelMapSetting(const void* data, size_t len, int width, int height) = 0;

    // set the link URL that is being dragged.
    virtual bool SetLinkURL(const std::string& url) = 0;

    // set the plain text that is being dragged.
    virtual bool SetFragmentText(const std::string& Text) = 0;

    // get the title associated with the link that is being dragged.
    virtual std::string GetLinkTitle() = 0;

    // set the title associated with the link that is being dragged.
    virtual bool SetLinkTitle(const std::string& title) = 0;

    // get the positon of the drag point.
    virtual void GetDragStartPosition(int& x, int& y) = 0;

    // is single iamge that is being dragged.
    virtual bool IsSingleImageContent() = 0;

    // set the iamge file name that is being dragged.
    virtual bool SetFileUri(const std::string& uri) = 0;

    // get the iamge file name that is being dragged.
    virtual std::string GetImageFileName() = 0;

    // clear the exist iamge file names.
    virtual void ClearImageFileNames() = 0;

    // is drag new style.
    virtual bool IsDragNewStyle() {
        return false;
    }

    // get allowed drag operation mask for current drag.
    virtual DragOperationsMask GetAllowedDragOperation() const = 0;

    // set allowed drag operation for current drag.
    virtual void SetAllowedDragOperation(DragOperationsMask allowed_op) = 0;
};

} // namespace OHOS::NWeb

#endif