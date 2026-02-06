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

#include "ohos_nweb/bridge/ark_web_drag_data_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebDragDataImpl::ArkWebDragDataImpl(std::shared_ptr<OHOS::NWeb::NWebDragData> nweb_drag_data)
    : nweb_drag_data_(nweb_drag_data)
{}

bool ArkWebDragDataImpl::SetFileUri(const ArkWebString& uri)
{
    return nweb_drag_data_->SetFileUri(ArkWebStringStructToClass(uri));
}

ArkWebString ArkWebDragDataImpl::GetLinkURL()
{
    return ArkWebStringClassToStruct(nweb_drag_data_->GetLinkURL());
}

bool ArkWebDragDataImpl::SetLinkURL(const ArkWebString& url)
{
    return nweb_drag_data_->SetLinkURL(ArkWebStringStructToClass(url));
}

ArkWebString ArkWebDragDataImpl::GetLinkTitle()
{
    return ArkWebStringClassToStruct(nweb_drag_data_->GetLinkTitle());
}

bool ArkWebDragDataImpl::SetLinkTitle(const ArkWebString& title)
{
    return nweb_drag_data_->SetLinkTitle(ArkWebStringStructToClass(title));
}

ArkWebString ArkWebDragDataImpl::GetFragmentText()
{
    return ArkWebStringClassToStruct(nweb_drag_data_->GetFragmentText());
}

bool ArkWebDragDataImpl::SetFragmentText(const ArkWebString& text)
{
    return nweb_drag_data_->SetFragmentText(ArkWebStringStructToClass(text));
}

ArkWebString ArkWebDragDataImpl::GetFragmentHtml()
{
    return ArkWebStringClassToStruct(nweb_drag_data_->GetFragmentHtml());
}

bool ArkWebDragDataImpl::SetFragmentHtml(const ArkWebString& html)
{
    return nweb_drag_data_->SetFragmentHtml(ArkWebStringStructToClass(html));
}

ArkWebString ArkWebDragDataImpl::GetImageFileName()
{
    return ArkWebStringClassToStruct(nweb_drag_data_->GetImageFileName());
}

bool ArkWebDragDataImpl::GetPixelMapSetting(const void** data, size_t& len, int& width, int& height)
{
    return nweb_drag_data_->GetPixelMapSetting(data, len, width, height);
}

bool ArkWebDragDataImpl::SetPixelMapSetting(const void* data, size_t len, int width, int height)
{
    return nweb_drag_data_->SetPixelMapSetting(data, len, width, height);
}

void ArkWebDragDataImpl::ClearImageFileNames()
{
    nweb_drag_data_->ClearImageFileNames();
}

bool ArkWebDragDataImpl::IsSingleImageContent()
{
    return nweb_drag_data_->IsSingleImageContent();
}

void ArkWebDragDataImpl::GetDragStartPosition(int& x, int& y)
{
    nweb_drag_data_->GetDragStartPosition(x, y);
}

bool ArkWebDragDataImpl::IsDragNewStyle()
{
    return nweb_drag_data_->IsDragNewStyle();
}

int ArkWebDragDataImpl::GetAllowedDragOperation()
{
    return static_cast<int>(nweb_drag_data_->GetAllowedDragOperation());
}

void ArkWebDragDataImpl::ClearDragData()
{
    nweb_drag_data_->ClearDragData();
}

} // namespace OHOS::ArkWeb
