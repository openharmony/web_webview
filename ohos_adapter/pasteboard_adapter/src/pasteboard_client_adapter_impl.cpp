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

#include "pasteboard_client_adapter_impl.h"

#include <mutex>
#include <securec.h>

#include "media_errors.h"
#include "nweb_log.h"
#include "remote_uri.h"

using namespace OHOS::MiscServices;
using namespace OHOS::DistributedFS::ModuleRemoteUri;

namespace OHOS::NWeb {
PasteboardObserverAdapterImpl::PasteboardObserverAdapterImpl(
    std::shared_ptr<PasteboardObserverAdapter> observer)
    : observer_(observer) {}

void PasteboardObserverAdapterImpl::OnPasteboardChanged()
{
    if (observer_) {
        observer_->OnPasteboardChanged();
    }
}

PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl(
    std::shared_ptr<PasteDataRecord> record)
    : record_(record) {}

PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl(
    const std::string& mimeType)
{
    builder_ = std::make_shared<PasteDataRecord::Builder>(mimeType);
    if (builder_) {
        record_ = builder_->Build();
    }
}

PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl(
    const std::string& mimeType,
    std::shared_ptr<std::string> htmlText,
    std::shared_ptr<std::string> plainText)
{
    record_ = std::make_shared<PasteDataRecord>(mimeType,
                                                htmlText,
                                                nullptr,
                                                plainText,
                                                nullptr);
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataRecordAdapter::NewRecord(
    const std::string& mimeType)
{
    return std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataRecordAdapter::NewRecord(
    const std::string& mimeType,
    std::shared_ptr<std::string> htmlText,
    std::shared_ptr<std::string> plainText)
{
    return std::make_shared<PasteDataRecordAdapterImpl>(mimeType,
                                                        htmlText,
                                                        plainText);
}

bool PasteDataRecordAdapterImpl::SetHtmlText(std::shared_ptr<std::string> htmlText)
{
    if (builder_) {
        record_ = builder_->SetHtmlText(htmlText).Build();
        return true;
    }
    WVLOG_E("record_ is null");
    return false;
}

bool PasteDataRecordAdapterImpl::SetPlainText(std::shared_ptr<std::string> plainText)
{
    if (builder_) {
        record_ = builder_->SetPlainText(plainText).Build();
        return true;
    }
    WVLOG_E("record_ is null");
    return false;
}

bool PasteDataRecordAdapterImpl::SetUri(const std::string& uriString)
{
    if (uriString.empty() || !builder_) {
        WVLOG_E("record_ or uriString is null");
        return false;
    }
    std::shared_ptr<OHOS::Uri> uri = std::make_shared<OHOS::Uri>(uriString);
    record_ = builder_->SetUri(uri).Build();
    return true;
}

bool PasteDataRecordAdapterImpl::SetCustomData(PasteCustomData& data)
{
    if (data.empty() || !builder_) {
        WVLOG_E("custom data is empty or builder_ is null");
        return false;
    }
    std::shared_ptr<MineCustomData> customData =
        std::make_shared<MineCustomData>();
    for (PasteCustomData::iterator iter = data.begin(); iter != data.end(); ++iter) {
        customData->AddItemData(iter->first, iter->second);
    }
    record_ = builder_->SetCustomData(customData).Build();
    return true;
}

ClipBoardImageAlphaType PasteDataRecordAdapterImpl::ImageToClipboardAlphaType
    (const Media::ImageInfo &imgInfo)
{
    switch (imgInfo.alphaType) {
        case Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN :
            return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE :
            return ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
        case Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL :
            return ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
        default :
            return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    }
}

ClipBoardImageColorType PasteDataRecordAdapterImpl::ImageToClipboardColorType
    (const Media::ImageInfo &imgInfo)
{
    switch (imgInfo.pixelFormat) {
        case Media::PixelFormat::RGBA_8888 :
            return ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
        case Media::PixelFormat::BGRA_8888 :
            return ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
        default :
            return ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
    }
}

Media::AlphaType PasteDataRecordAdapterImpl::ClipboardToImageAlphaType
    (ClipBoardImageAlphaType alphaType)
{
    switch (alphaType) {
        case ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN :
            return Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
        case ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE :
            return Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
        case ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED :
            return Media::AlphaType::IMAGE_ALPHA_TYPE_PREMUL;
        default :
            return Media::AlphaType::IMAGE_ALPHA_TYPE_UNKNOWN;
    }
}

Media::PixelFormat PasteDataRecordAdapterImpl::ClipboardToImageColorType
    (ClipBoardImageColorType colorType)
{
    switch (colorType) {
        case ClipBoardImageColorType::COLOR_TYPE_RGBA_8888 :
            return Media::PixelFormat::RGBA_8888;
        case ClipBoardImageColorType::COLOR_TYPE_BGRA_8888 :
            return Media::PixelFormat::BGRA_8888;
        default :
            return Media::PixelFormat::UNKNOWN;
    }
}

bool PasteDataRecordAdapterImpl::SetImgData(std::shared_ptr<ClipBoardImageData> imageData)
{
    if (imageData == nullptr) {
        WVLOG_E("imageData is null");
        return false;
    }
    Media::InitializationOptions opt;
    opt.size.width = imageData->width;
    opt.size.height = imageData->height;
    opt.pixelFormat = ClipboardToImageColorType(imageData->colorType);
    opt.alphaType = ClipboardToImageAlphaType(imageData->alphaType);
    opt.editable = true;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opt);
    if (pixelMap == nullptr) {
        WVLOG_E("create pixel map failed");
        return false;
    }
    uint64_t stride = static_cast<uint64_t>(imageData->width) << 2;
    uint64_t bufferSize = stride * static_cast<uint64_t>(imageData->height);
    uint32_t ret = pixelMap->WritePixels(reinterpret_cast<const uint8_t *>(imageData->data), bufferSize);
    if (ret != Media::SUCCESS) {
        WVLOG_E("write pixel map failed %{public}u", ret);
        return false;
    }

    std::shared_ptr<Media::PixelMap> pixelMapIn = move(pixelMap);

    if (!builder_) {
        WVLOG_E("record_ is null");
        return false;
    }
    record_ = builder_->SetPixelMap(pixelMapIn).Build();
    return true;
}

std::string PasteDataRecordAdapterImpl::GetMimeType()
{
    return (record_ != nullptr) ? record_->GetMimeType() : "";
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetHtmlText()
{
    return (record_ != nullptr) ? record_->GetHtmlText() : nullptr;
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetPlainText()
{
    return (record_ != nullptr) ? record_->GetPlainText() : nullptr;
}

bool PasteDataRecordAdapterImpl::GetImgData(ClipBoardImageData &imageData)
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is null");
        return false;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = record_->GetPixelMap();
    if (pixelMap == nullptr) {
        WVLOG_E("pixelMap is null");
        return false;
    }

    Media::ImageInfo imgInfo;
    ClearImgBuffer();
    bufferSize_ = pixelMap->GetCapacity();
    if ((bufferSize_ == 0) || (pixelMap->GetPixels() == nullptr)) {
        WVLOG_E("data in pixel map is empty");
        return false;
    }

    imgBuffer_ = static_cast<uint8_t *>(calloc(static_cast<size_t>(bufferSize_), sizeof(uint8_t)));
    if (imgBuffer_ == nullptr) {
        WVLOG_E("calloc imgbuffer failed");
        return false;
    }

    if (memcpy_s(imgBuffer_, bufferSize_, pixelMap->GetPixels(), bufferSize_)) {
        WVLOG_E("memcpy imgbuffer failed");
        ClearImgBuffer();
        return false;
    }

    int32_t width = pixelMap->GetWidth();
    int32_t height = pixelMap->GetHeight();
    pixelMap->GetImageInfo(imgInfo);
    int32_t rowBytes = pixelMap->GetRowBytes();

    imageData.colorType = ImageToClipboardColorType(imgInfo);
    imageData.alphaType = ImageToClipboardAlphaType(imgInfo);
    imageData.data = (uint32_t *)(imgBuffer_);
    imageData.dataSize = static_cast<size_t>(bufferSize_);
    imageData.width = width;
    imageData.height = height;
    imageData.rowBytes = static_cast<size_t>(rowBytes);
    return true;
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetUri()
{
    if (record_ == nullptr) {
        return nullptr;
    }
    auto uri = record_->GetUri();
    if (uri == nullptr) {
        return nullptr;
    }
    return std::make_shared<std::string>(uri->ToString());
}

std::shared_ptr<PasteCustomData> PasteDataRecordAdapterImpl::GetCustomData()
{
    if (record_ == nullptr) {
        return nullptr;
    }
    auto customData = record_->GetCustomData();
    if (customData == nullptr) {
        return nullptr;
    }
    return std::make_shared<PasteCustomData>(customData->GetItemData());
}

void PasteDataRecordAdapterImpl::ClearImgBuffer()
{
    if (imgBuffer_) {
        free(imgBuffer_);
        imgBuffer_ = nullptr;
        bufferSize_ = 0;
    }
}

void PasteDataRecordAdapterImpl::Clear()
{
    ClearImgBuffer();
}

std::shared_ptr<PasteDataRecord> PasteDataRecordAdapterImpl::GetRecord()
{
    return record_;
}

PasteDataAdapterImpl::PasteDataAdapterImpl()
    : data_(std::make_shared<PasteData>()) {}

PasteDataAdapterImpl::PasteDataAdapterImpl(
    std::shared_ptr<PasteData> data) : data_(data) {}

void PasteDataAdapterImpl::AddHtmlRecord(const std::string& html)
{
    if (data_ != nullptr) {
        data_->AddHtmlRecord(html);
    }
}

void PasteDataAdapterImpl::AddTextRecord(const std::string& text)
{
    if (data_ != nullptr) {
        data_->AddTextRecord(text);
    }
}

std::vector<std::string> PasteDataAdapterImpl::GetMimeTypes()
{
    return (data_ != nullptr) ? data_->GetMimeTypes() :
                                std::vector<std::string>();
}

std::shared_ptr<std::string> PasteDataAdapterImpl::GetPrimaryHtml()
{
    return (data_ != nullptr) ? data_->GetPrimaryHtml() : nullptr;
}

std::shared_ptr<std::string> PasteDataAdapterImpl::GetPrimaryText()
{
    return (data_ != nullptr) ? data_->GetPrimaryText() : nullptr;
}

std::shared_ptr<std::string> PasteDataAdapterImpl::GetPrimaryMimeType()
{
    return (data_ != nullptr) ? data_->GetPrimaryMimeType() : nullptr;
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataAdapterImpl::GetRecordAt(
    std::size_t index)
{
    if (data_ == nullptr || data_->GetRecordCount() <= index) {
        return nullptr;
    }
    return std::make_shared<PasteDataRecordAdapterImpl>(data_->GetRecordAt(index));
}

std::size_t PasteDataAdapterImpl::GetRecordCount()
{
    return (data_ != nullptr) ? data_->GetRecordCount() : 0;
}

PasteRecordList PasteDataAdapterImpl::AllRecords() const
{
    if (data_ == nullptr) {
        return PasteRecordList();
    }
    PasteRecordList result;
    for (auto& record: data_->AllRecords()) {
        result.push_back(std::make_shared<PasteDataRecordAdapterImpl>(record));
    }
    return result;
}

PasteBoardClientAdapterImpl& PasteBoardClientAdapterImpl::GetInstance()
{
    static PasteBoardClientAdapterImpl instance;
    return instance;
}

bool PasteBoardClientAdapterImpl::GetPasteData(PasteRecordList& data)
{
    PasteData pData;
    if (!PasteboardClient::GetInstance()->HasPasteData() ||
        !PasteboardClient::GetInstance()->GetPasteData(pData)) {
        isLocalPaste_ = false;
        tokenId_ = 0;
        return false;
    }
    for (auto& record: pData.AllRecords()) {
        data.push_back(std::make_shared<PasteDataRecordAdapterImpl>(record));
    }
    tokenId_ = pData.GetTokenId();
    isLocalPaste_ = pData.IsLocalPaste();
    return true;
}

void PasteBoardClientAdapterImpl::SetPasteData(const PasteRecordList& data)
{
    std::vector<std::shared_ptr<PasteDataRecord>> recordList;
    for (auto& record: data) {
        PasteDataRecordAdapterImpl* rawRecord =
            reinterpret_cast<PasteDataRecordAdapterImpl*>(record.get());
        if (rawRecord == nullptr) {
            continue;
        }
        recordList.push_back(rawRecord->GetRecord());
    }
    PasteData pData(recordList);
    PasteboardClient::GetInstance()->SetPasteData(pData);
}

bool PasteBoardClientAdapterImpl::HasPasteData()
{
    return PasteboardClient::GetInstance()->HasPasteData();
}

void PasteBoardClientAdapterImpl::Clear()
{
    PasteRecordList recordList;
    if (!GetPasteData(recordList)) {
        WVLOG_E("get paste data failed while clear");
        PasteboardClient::GetInstance()->Clear();
        return;
    }
    for (auto& record: recordList) {
        PasteDataRecordAdapterImpl* rawRecord =
            reinterpret_cast<PasteDataRecordAdapterImpl*>(record.get());
        if (rawRecord == nullptr) {
            continue;
        }
        rawRecord->Clear();
    }
    PasteboardClient::GetInstance()->Clear();
}

int32_t PasteBoardClientAdapterImpl::OpenRemoteUri(const std::string& path)
{
    return RemoteUri::OpenRemoteUri(path);
}

bool PasteBoardClientAdapterImpl::IsLocalPaste() const
{
    return isLocalPaste_;
}

uint32_t PasteBoardClientAdapterImpl::GetTokenId() const
{
    return tokenId_;
}

void PasteBoardClientAdapterImpl::AddPasteboardChangedObserver(
    std::shared_ptr<PasteboardObserverAdapter> callback)
{
    if (callback) {
        sptr<PasteboardObserver> observer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ObserverMap::iterator iter = reg_.find(callback.get());
            if (iter != reg_.end()) {
                return;
            }
            observer = new (std::nothrow) PasteboardObserverAdapterImpl(callback);
            if (!observer) {
                return;
            }
            reg_.emplace(std::make_pair(callback.get(), observer));
        }
        PasteboardClient::GetInstance()->AddPasteboardChangedObserver(observer);
    }
}

void PasteBoardClientAdapterImpl::RemovePasteboardChangedObserver(
    std::shared_ptr<PasteboardObserverAdapter> callback)
{
    if (callback) {
        sptr<PasteboardObserver> observer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            ObserverMap::iterator iter = reg_.find(callback.get());
            if (iter == reg_.end()) {
                return;
            }
            observer = iter->second;
            reg_.erase(iter);
        }
        PasteboardClient::GetInstance()->RemovePasteboardChangedObserver(observer);
    }
}
} // namespace OHOS::NWeb
