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

#ifndef ARK_WEB_DRAG_DATA_H_
#define ARK_WEB_DRAG_DATA_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebDragData : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief set the image file name that is being dragged.
     */
    /*--ark web()--*/
    virtual bool SetFileUri(const ArkWebString& uri) = 0;

    /**
     * @brief get the link URL that is being dragged.
     */
    /*--ark web()--*/
    virtual ArkWebString GetLinkURL() = 0;

    /**
     * @brief set the link URL that is being dragged.
     */
    /*--ark web()--*/
    virtual bool SetLinkURL(const ArkWebString& url) = 0;

    /**
     * @brief get the title associated with the link that is being dragged.
     */
    /*--ark web()--*/
    virtual ArkWebString GetLinkTitle() = 0;

    /**
     * @brief set the title associated with the link that is being dragged.
     */
    /*--ark web()--*/
    virtual bool SetLinkTitle(const ArkWebString& title) = 0;

    /**
     * @brief get the plain text that is being dragged.
     */
    /*--ark web()--*/
    virtual ArkWebString GetFragmentText() = 0;

    /**
     * @brief set the plain text that is being dragged.
     */
    /*--ark web()--*/
    virtual bool SetFragmentText(const ArkWebString& text) = 0;

    /**
     * @brief get the text/html fragment that is being dragged.
     */
    /*--ark web()--*/
    virtual ArkWebString GetFragmentHtml() = 0;

    /**
     * @brief set the text/html fragment that is being dragged.
     */
    /*--ark web()--*/
    virtual bool SetFragmentHtml(const ArkWebString& html) = 0;

    /**
     * @brief get the image file name that is being dragged.
     */
    /*--ark web()--*/
    virtual ArkWebString GetImageFileName() = 0;

    /**
     * @brief get the image representation data that is beging dragged.
     */
    /*--ark web()--*/
    virtual bool GetPixelMapSetting(const void** data, size_t& len, int& width, int& height) = 0;

    /**
     * @brief set the image representation data that is beging dragged.
     */
    /*--ark web()--*/
    virtual bool SetPixelMapSetting(const void* data, size_t len, int width, int height) = 0;

    /**
     * @brief clear the exist image file names.
     */
    /*--ark web()--*/
    virtual void ClearImageFileNames() = 0;

    /**
     * @brief is single image that is being dragged.
     */
    /*--ark web()--*/
    virtual bool IsSingleImageContent() = 0;

    /**
     * @brief get the positon point that is beging dragged.
     */
    /*--ark web()--*/
    virtual void GetDragStartPosition(int& x, int& y) = 0;

    /**
     * @Description: Obtains whether the drag style is new.
     * @Return: Returns an drag style, true is drag a new style, otherwise, drag a old style.
     */
    /*--ark web()--*/
    virtual bool IsDragNewStyle() = 0;

    /**
     * @Description: get allowed drag operation mask.
     */
    /*--ark web()--*/
    virtual int GetAllowedDragOperation() = 0;

    /**
     * @Description: clear drag data for current drag.
     */
    /*--ark web()--*/
    virtual void ClearDragData() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DRAG_DATA_H_
