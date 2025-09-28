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

#include "ohos_nweb/bridge/ark_web_drag_data_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDragDataWrapper::ArkWebDragDataWrapper(ArkWebRefPtr<ArkWebDragData> ark_web_drag_data)
    : ark_web_drag_data_(ark_web_drag_data)
{}

bool ArkWebDragDataWrapper::SetFileUri(const std::string& uri)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(uri);

    bool flag = ark_web_drag_data_->SetFileUri(stUrl);

    ArkWebStringStructRelease(stUrl);
    return flag;
}

std::string ArkWebDragDataWrapper::GetLinkURL()
{
    ArkWebString stUrl = ark_web_drag_data_->GetLinkURL();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

bool ArkWebDragDataWrapper::SetLinkURL(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    bool flag = ark_web_drag_data_->SetLinkURL(stUrl);

    ArkWebStringStructRelease(stUrl);
    return flag;
}

std::string ArkWebDragDataWrapper::GetLinkTitle()
{
    ArkWebString stTitle = ark_web_drag_data_->GetLinkTitle();

    std::string objTitle = ArkWebStringStructToClass(stTitle);
    ArkWebStringStructRelease(stTitle);
    return objTitle;
}

bool ArkWebDragDataWrapper::SetLinkTitle(const std::string& title)
{
    ArkWebString stTitle = ArkWebStringClassToStruct(title);

    bool flag = ark_web_drag_data_->SetLinkTitle(stTitle);

    ArkWebStringStructRelease(stTitle);
    return flag;
}

std::string ArkWebDragDataWrapper::GetFragmentText()
{
    ArkWebString stText = ark_web_drag_data_->GetFragmentText();

    std::string objText = ArkWebStringStructToClass(stText);
    ArkWebStringStructRelease(stText);
    return objText;
}

bool ArkWebDragDataWrapper::SetFragmentText(const std::string& text)
{
    ArkWebString stText = ArkWebStringClassToStruct(text);

    bool flag = ark_web_drag_data_->SetFragmentText(stText);

    ArkWebStringStructRelease(stText);
    return flag;
}

std::string ArkWebDragDataWrapper::GetFragmentHtml()
{
    ArkWebString stHtml = ark_web_drag_data_->GetFragmentHtml();

    std::string objHtml = ArkWebStringStructToClass(stHtml);
    ArkWebStringStructRelease(stHtml);
    return objHtml;
}

bool ArkWebDragDataWrapper::SetFragmentHtml(const std::string& html)
{
    ArkWebString stHtml = ArkWebStringClassToStruct(html);

    bool flag = ark_web_drag_data_->SetFragmentHtml(stHtml);

    ArkWebStringStructRelease(stHtml);
    return flag;
}

std::string ArkWebDragDataWrapper::GetImageFileName()
{
    ArkWebString stName = ark_web_drag_data_->GetImageFileName();

    std::string objName = ArkWebStringStructToClass(stName);
    ArkWebStringStructRelease(stName);
    return objName;
}

bool ArkWebDragDataWrapper::GetPixelMapSetting(const void** data, size_t& len, int& width, int& height)
{
    return ark_web_drag_data_->GetPixelMapSetting(data, len, width, height);
}

bool ArkWebDragDataWrapper::SetPixelMapSetting(const void* data, size_t len, int width, int height)
{
    return ark_web_drag_data_->SetPixelMapSetting(data, len, width, height);
}

void ArkWebDragDataWrapper::ClearImageFileNames()
{
    ark_web_drag_data_->ClearImageFileNames();
}

bool ArkWebDragDataWrapper::IsSingleImageContent()
{
    return ark_web_drag_data_->IsSingleImageContent();
}

void ArkWebDragDataWrapper::GetDragStartPosition(int& x, int& y)
{
    ark_web_drag_data_->GetDragStartPosition(x, y);
}

bool ArkWebDragDataWrapper::IsDragNewStyle()
{
    return ark_web_drag_data_->IsDragNewStyle();
}

OHOS::NWeb::NWebDragData::DragOperationsMask ArkWebDragDataWrapper::GetAllowedDragOperation() const
{
    return static_cast<OHOS::NWeb::NWebDragData::DragOperationsMask>(ark_web_drag_data_->GetAllowedDragOperation());
}
} // namespace OHOS::ArkWeb
