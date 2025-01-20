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

#include "hisysevent_adapter.h"
#include "media_errors.h"
#include "nweb_log.h"
#include "ohos_adapter_helper.h"
#include "pasteboard_error.h"
#include "remote_uri.h"

using namespace OHOS::MiscServices;
using namespace OHOS::DistributedFS::ModuleRemoteUri;

namespace OHOS::NWeb {
constexpr char PASTE_BOARD_ERROR[] = "PASTE_BOARD_ERROR";
constexpr char ERROR_CODE[] = "ERROR_CODE";
constexpr char RECORD_SIZE[] = "RECORD_SIZE";
constexpr char DATA_TYPE[] = "DATA_TYPE";
constexpr char MIMETYPE_HYBRID[] = "hybrid";
constexpr char MIMETYPE_NULL[] = "null";
constexpr char SPAN_STRING_TAG[] = "openharmony.styled-string";
constexpr char CUSTOM_DATA_TAG[] = "openharmony.arkweb.custom-data";
constexpr char CUSTOM_DATA_TAG2[] = "ohos/custom-data";

PasteboardObserverAdapterImpl::PasteboardObserverAdapterImpl(
    std::shared_ptr<PasteboardObserverAdapter> observer)
    : observer_(observer) {}

void PasteboardObserverAdapterImpl::OnPasteboardChanged()
{
    if (observer_) {
        observer_->OnPasteboardChanged();
    }
}

PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl()
{
    record_ = std::make_shared<PasteDataRecord>();
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
    // To adapt to multiple entries, the mime type of the record will be specified when writing the first data.
    (void)mimeType;
    return std::make_shared<PasteDataRecordAdapterImpl>();
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
    return SetHtmlText(htmlText, nullptr);
}

bool PasteDataRecordAdapterImpl::SetHtmlText(
    std::shared_ptr<std::string> htmlText, std::shared_ptr<std::string> plainText)
{
    if (!record_ || !htmlText) {
        WVLOG_E("record_ or htmlText is null");
        return false;
    }
    auto utdId = UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::HTML);
    auto object = std::make_shared<Object>();
    if (!object) {
        WVLOG_E("udmf object create failed");
        return false;
    }
    object->value_[UDMF::UNIFORM_DATA_TYPE] = utdId;
    object->value_[UDMF::HTML_CONTENT] = *htmlText;
    if (plainText) {
        object->value_[UDMF::PLAIN_CONTENT] = *plainText;
    }
    record_->AddEntry(utdId, std::make_shared<PasteDataEntry>(utdId, object));
    return true;
}

bool PasteDataRecordAdapterImpl::SetPlainText(std::shared_ptr<std::string> plainText)
{
    if (!record_ || !plainText) {
        WVLOG_E("record_ or plainText is null");
        return false;
    }
    if (auto htmlText = record_->GetHtmlText(); htmlText && !htmlText->empty()) {
        SetHtmlText(htmlText, plainText);
    }
    auto utdId = UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::PLAIN_TEXT);
    auto object = std::make_shared<Object>();
    if (!object) {
        WVLOG_E("udmf object create failed");
        return false;
    }
    object->value_[UDMF::UNIFORM_DATA_TYPE] = utdId;
    object->value_[UDMF::CONTENT] = *plainText;
    record_->AddEntry(utdId, std::make_shared<PasteDataEntry>(utdId, object));
    return true;
}

bool PasteDataRecordAdapterImpl::SetUri(const std::string& uriString)
{
    if (!record_ || uriString.empty()) {
        WVLOG_E("record_ or uriString is null");
        return false;
    }
    auto utdId = UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::FILE_URI);
    record_->AddEntry(utdId, std::make_shared<PasteDataEntry>(utdId, uriString));
    return true;
}

bool PasteDataRecordAdapterImpl::SetCustomData(PasteCustomData& data)
{
    if (!record_ || data.empty()) {
        WVLOG_E("custom data is empty or record_ is null");
        return false;
    }
    for (PasteCustomData::iterator iter = data.begin(); iter != data.end(); ++iter) {
        record_->AddEntry(iter->first, std::make_shared<PasteDataEntry>(iter->first, iter->second));
    }
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

bool PasteDataRecordAdapterImpl::SetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData)
{
    if (imageData == nullptr) {
        WVLOG_E("imageData is null");
        return false;
    }
    Media::InitializationOptions opt;
    opt.size.width = imageData->GetWidth();
    opt.size.height = imageData->GetHeight();
    opt.pixelFormat = ClipboardToImageColorType(imageData->GetColorType());
    opt.alphaType = ClipboardToImageAlphaType(imageData->GetAlphaType());
    opt.editable = true;
    std::unique_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opt);
    if (pixelMap == nullptr) {
        WVLOG_E("create pixel map failed");
        return false;
    }
    uint64_t stride = static_cast<uint64_t>(imageData->GetWidth()) << 2;
    uint64_t bufferSize = stride * static_cast<uint64_t>(imageData->GetHeight());
    uint32_t ret = pixelMap->WritePixels(reinterpret_cast<const uint8_t *>(imageData->GetData()), bufferSize);
    if (ret != Media::SUCCESS) {
        WVLOG_E("write pixel map failed %{public}u", ret);
        return false;
    }

    std::shared_ptr<Media::PixelMap> pixelMapIn = move(pixelMap);

    if (!record_) {
        WVLOG_E("record_ is null");
        return false;
    }
    auto utdId = UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::SYSTEM_DEFINED_PIXEL_MAP);
    auto object = std::make_shared<Object>();
    if (!object) {
        WVLOG_E("udmf object create failed");
        return false;
    }
    object->value_[UDMF::UNIFORM_DATA_TYPE] = utdId;
    object->value_[UDMF::PIXEL_MAP] = pixelMapIn;
    record_->AddEntry(utdId, std::make_shared<PasteDataEntry>(utdId, object));
    return true;
}

std::string PasteDataRecordAdapterImpl::GetMimeType()
{
    return (record_ != nullptr) ? record_->GetMimeType() : "";
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetHtmlText()
{
    if (!record_) {
        WVLOG_E("record_ is null");
        return nullptr;
    }
    auto entry = record_->GetEntry(UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::HTML));
    return (entry ? entry->ConvertToHtml() : nullptr);
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetPlainText()
{
    if (!record_) {
        WVLOG_E("record_ is null");
        return nullptr;
    }
    auto entry = record_->GetEntry(UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::PLAIN_TEXT));
    if (entry) {
        return entry->ConvertToPlianText();
    }
    auto tempPlainText = record_->GetPlainText();
    if (tempPlainText && !tempPlainText->empty()) {
        WVLOG_I("Compatibility adaptation with old versions of read plainText");
        return tempPlainText;
    }
    return nullptr;
}

std::shared_ptr<Media::PixelMap> PasteDataRecordAdapterImpl::GetPixelMap()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is null");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    auto entry = record_->GetEntry(UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::SYSTEM_DEFINED_PIXEL_MAP));
    if (entry) {
        pixelMap = entry->ConvertToPixelMap();
    }
    if (pixelMap == nullptr) {
        pixelMap = record_->GetPixelMap();
        if (pixelMap) {
            WVLOG_I("Compatibility adaptation with old versions of read pixelMap");
        }
    }
    return pixelMap;
}

bool PasteDataRecordAdapterImpl::GetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData)
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is null");
        return false;
    }

    if (imageData == nullptr) {
        WVLOG_E("imageData is null");
        return false;
    }

    std::shared_ptr<Media::PixelMap> pixelMap = GetPixelMap();
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

    imageData->SetColorType(ImageToClipboardColorType(imgInfo));
    imageData->SetAlphaType(ImageToClipboardAlphaType(imgInfo));
    imageData->SetData((uint32_t *)(imgBuffer_));
    imageData->SetDataSize(static_cast<size_t>(bufferSize_));
    imageData->SetWidth(width);
    imageData->SetHeight(height);
    imageData->SetRowBytes(static_cast<size_t>(rowBytes));
    return true;
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetUri()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is null");
        return nullptr;
    }
    auto entry = record_->GetEntry(UDMF::UtdUtils::GetUtdIdFromUtdEnum(UDMF::FILE_URI));
    if (entry == nullptr) {
        return nullptr;
    }
    auto uri = entry->ConvertToUri();
    if (uri == nullptr) {
        return nullptr;
    }
    return std::make_shared<std::string>(uri->ToString());
}

std::shared_ptr<PasteCustomData> PasteDataRecordAdapterImpl::GetCustomData()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is null");
        return nullptr;
    }
    auto customData = std::make_shared<PasteCustomData>();
    if (customData == nullptr) {
        WVLOG_E("customData create failed");
        return nullptr;
    }
    for (auto type : { SPAN_STRING_TAG, CUSTOM_DATA_TAG, CUSTOM_DATA_TAG2 }) {
        auto entry = record_->GetEntry(type);
        if (!entry) {
            continue;
        }
        auto data = entry->ConvertToCustomData();
        if (!data) {
            continue;
        }
        for (const auto& [key, value] : data->GetItemData()) {
            customData->emplace(key, value);
        }
    }
    // Compatibility adaptation with old versions of read customData
    if (record_->GetCustomData()) {
        for (const auto& [key, value] : record_->GetCustomData()->GetItemData()) {
            customData->emplace(key, value);
        }
    }

    return (customData->empty() ? nullptr : customData);
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

PasteRecordVector PasteDataAdapterImpl::AllRecords()
{
    if (data_ == nullptr) {
        return PasteRecordVector();
    }
    PasteRecordVector result;
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

MiscServices::ShareOption PasteBoardClientAdapterImpl::TransitionCopyOption(CopyOptionMode copyOption)
{
    auto shareOption = MiscServices::ShareOption::CrossDevice;
    switch (copyOption) {
        case CopyOptionMode::IN_APP:
            shareOption = MiscServices::ShareOption::InApp;
            break;
        case CopyOptionMode::LOCAL_DEVICE:
            shareOption = MiscServices::ShareOption::LocalDevice;
            break;
        case CopyOptionMode::CROSS_DEVICE:
            shareOption = MiscServices::ShareOption::CrossDevice;
            break;
        default:
            break;
    }
    return shareOption;
}

void ReportPasteboardErrorEvent(int32_t errorCode, int32_t recordSize, const std::string &dataType)
{
    OhosAdapterHelper::GetInstance().GetHiSysEventAdapterInstance().Write(PASTE_BOARD_ERROR,
        HiSysEventAdapter::EventType::FAULT, { ERROR_CODE, std::to_string(errorCode),
            RECORD_SIZE, std::to_string(recordSize), DATA_TYPE, dataType });
}

std::string GetPasteMimeTypeExtention(const PasteRecordVector& data)
{
    if (data.empty()) {
        return MIMETYPE_NULL;
    }
    bool isHybrid = false;
    std::string primaryMimeType = data.front()->GetMimeType();
    for (auto &item : data) {
        if (primaryMimeType != item->GetMimeType()) {
            isHybrid = true;
            break;
        }
    }
    if (isHybrid) {
        return MIMETYPE_HYBRID;
    }
    return primaryMimeType;
}

bool PasteBoardClientAdapterImpl::GetPasteData(PasteRecordVector& data)
{
    PasteData pData;
    if (!PasteboardClient::GetInstance()->HasPasteData() ||
        PasteboardClient::GetInstance()->GetPasteData(pData) != static_cast<int32_t>(PasteboardError::E_OK)) {
        WVLOG_E("no data to paste or get data from clipboard failed");
        ReportPasteboardErrorEvent(PasteboardClient::GetInstance()->GetPasteData(pData),
            pData.AllRecords().size(), GetPasteMimeTypeExtention(data));
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

void PasteBoardClientAdapterImpl::SetPasteData(const PasteRecordVector& data, CopyOptionMode copyOption)
{
    if (copyOption == CopyOptionMode::NONE) {
        WVLOG_E("SetPasteData failed, copy option mode is 'NONE'");
        return;
    }
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
    auto shareOption = TransitionCopyOption(copyOption);
    pData.SetShareOption(shareOption);
    int32_t ret = PasteboardClient::GetInstance()->SetPasteData(pData);
    if (ret != static_cast<int32_t>(PasteboardError::E_OK)) {
        WVLOG_E("set paste data to clipboard failed");
        ReportPasteboardErrorEvent(ret, pData.AllRecords().size(), GetPasteMimeTypeExtention(data));
    }
}

bool PasteBoardClientAdapterImpl::HasPasteData()
{
    return PasteboardClient::GetInstance()->HasPasteData();
}

void PasteBoardClientAdapterImpl::Clear()
{
    PasteRecordVector recordVector;
    if (!GetPasteData(recordVector)) {
        WVLOG_E("get paste data failed while clear");
        PasteboardClient::GetInstance()->Clear();
        return;
    }
    for (auto& record: recordVector) {
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

bool PasteBoardClientAdapterImpl::IsLocalPaste()
{
    return isLocalPaste_;
}

uint32_t PasteBoardClientAdapterImpl::GetTokenId()
{
    return tokenId_;
}

int32_t PasteBoardClientAdapterImpl::AddPasteboardChangedObserver(
    std::shared_ptr<PasteboardObserverAdapter> callback)
{
    static int32_t count = 0;
    int32_t id = -1;
    if (callback) {
        sptr<PasteboardObserver> observer;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            observer = new (std::nothrow) PasteboardObserverAdapterImpl(callback);
            if (!observer) {
                return -1;
            }

            id = count++;
            if (count < 0) {
                count = 0;
            }
            reg_.emplace(std::make_pair(id, observer));
        }
        PasteboardClient::GetInstance()->AddPasteboardChangedObserver(observer);
    }
    return id;
}

void PasteBoardClientAdapterImpl::RemovePasteboardChangedObserver(
    int32_t callbackId)
{
    sptr<PasteboardObserver> observer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ObserverMap::iterator iter = reg_.find(callbackId);
        if (iter == reg_.end()) {
            return;
        }
        observer = iter->second;
        reg_.erase(iter);
    }
    PasteboardClient::GetInstance()->RemovePasteboardChangedObserver(observer);
}
} // namespace OHOS::NWeb
