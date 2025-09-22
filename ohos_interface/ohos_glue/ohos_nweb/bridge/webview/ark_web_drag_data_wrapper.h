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

#ifndef ARK_WEB_DRAG_DATA_WRAPPER_H_
#define ARK_WEB_DRAG_DATA_WRAPPER_H_
#pragma once

#include "include/nweb_drag_data.h"
#include "ohos_nweb/include/ark_web_drag_data.h"

namespace OHOS::ArkWeb {

class ArkWebDragDataWrapper : public OHOS::NWeb::NWebDragData {
public:
    ArkWebDragDataWrapper(ArkWebRefPtr<ArkWebDragData> ark_web_drag_data);
    ~ArkWebDragDataWrapper() = default;

    /**
     * @brief set the image file name that is being dragged.
     */
    bool SetFileUri(const std::string& uri) override;

    /**
     * @brief get the link URL that is being dragged.
     */
    std::string GetLinkURL() override;

    /**
     * @brief set the link URL that is being dragged.
     */
    bool SetLinkURL(const std::string& url) override;

    /**
     * @brief get the title associated with the link that is being dragged.
     */
    std::string GetLinkTitle() override;

    /**
     * @brief set the title associated with the link that is being dragged.
     */
    bool SetLinkTitle(const std::string& title) override;

    /**
     * @brief get the plain text that is being dragged.
     */
    std::string GetFragmentText() override;

    /**
     * @brief set the plain text that is being dragged.
     */
    bool SetFragmentText(const std::string& text) override;

    /**
     * @brief get the text/html fragment that is being dragged.
     */
    std::string GetFragmentHtml() override;

    /**
     * @brief set the text/html fragment that is being dragged.
     */
    bool SetFragmentHtml(const std::string& html) override;

    /**
     * @brief get the image file name that is being dragged.
     */
    std::string GetImageFileName() override;

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

private:
    ArkWebRefPtr<ArkWebDragData> ark_web_drag_data_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_DRAG_DATA_WRAPPER_H_
