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

#ifndef ARK_WEB_DRAG_DATA_IMPL_H_
#define ARK_WEB_DRAG_DATA_IMPL_H_
#pragma once

#include "include/nweb_drag_data.h"
#include "ohos_nweb/include/ark_web_drag_data.h"

namespace OHOS::ArkWeb {

class ArkWebDragDataImpl : public ArkWebDragData {
    IMPLEMENT_REFCOUNTING(ArkWebDragDataImpl);

public:
    ArkWebDragDataImpl(std::shared_ptr<OHOS::NWeb::NWebDragData> nweb_drag_data);
    ~ArkWebDragDataImpl() = default;

    /**
     * @brief set the image file name that is being dragged.
     */
    bool SetFileUri(const ArkWebString& uri) override;

    /**
     * @brief get the link URL that is being dragged.
     */
    ArkWebString GetLinkURL() override;

    /**
     * @brief set the link URL that is being dragged.
     */
    bool SetLinkURL(const ArkWebString& url) override;

    /**
     * @brief get the title associated with the link that is being dragged.
     */
    ArkWebString GetLinkTitle() override;

    /**
     * @brief set the title associated with the link that is being dragged.
     */
    bool SetLinkTitle(const ArkWebString& title) override;

    /**
     * @brief get the plain text that is being dragged.
     */
    ArkWebString GetFragmentText() override;

    /**
     * @brief set the plain text that is being dragged.
     */
    bool SetFragmentText(const ArkWebString& text) override;

    /**
     * @brief get the text/html fragment that is being dragged.
     */
    ArkWebString GetFragmentHtml() override;

    /**
     * @brief set the text/html fragment that is being dragged.
     */
    bool SetFragmentHtml(const ArkWebString& html) override;

    /**
     * @brief get the image file name that is being dragged.
     */
    ArkWebString GetImageFileName() override;

    /**
     * @brief get the image representation data that is beging dragged.
     */
    bool GetPixelMapSetting(const void** data, size_t& len, int& width, int& height) override;

    /**
     * @brief set the image representation data that is beging dragged.
     */
    bool SetPixelMapSetting(const void* data, size_t len, int width, int height) override;

    /**
     * @brief clear the exist image file names.
     */
    void ClearImageFileNames() override;

    /**
     * @brief is single image that is being dragged.
     */
    bool IsSingleImageContent() override;

    /**
     * @brief get the positon point that is beging dragged.
     */
    void GetDragStartPosition(int& x, int& y) override;

    /**
     * @brief is drag new style.
     */
    bool IsDragNewStyle() override;

    /**
     * @brief get allowed drag operation mask for current drag.
     */
    int GetAllowedDragOperation() override;

    /**
     * @brief clear drag data for current drag.
     */
    void ClearDragData() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebDragData> nweb_drag_data_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DRAG_DATA_IMPL_H_
