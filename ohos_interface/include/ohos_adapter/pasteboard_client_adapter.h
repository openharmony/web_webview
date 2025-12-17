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

#ifndef PASTEBOARD_CLIENT_ADAPTER_H
#define PASTEBOARD_CLIENT_ADAPTER_H

#include <map>
#include <memory>
#include <vector>

namespace OHOS::NWeb {

enum class ClipBoardImageColorType { COLOR_TYPE_RGBA_8888, COLOR_TYPE_BGRA_8888, COLOR_TYPE_UNKNOWN };

enum class ClipBoardImageAlphaType {
    ALPHA_TYPE_OPAQUE,
    ALPHA_TYPE_PREMULTIPLIED,
    ALPHA_TYPE_POSTMULTIPLIED,
    ALPHA_TYPE_UNKNOWN
};

enum class CopyOptionMode { NONE = 0, IN_APP = 1, LOCAL_DEVICE = 2, CROSS_DEVICE = 3 };

class ClipBoardImageDataAdapter {
public:
    ClipBoardImageDataAdapter() = default;

    virtual ~ClipBoardImageDataAdapter() = default;

    virtual ClipBoardImageColorType GetColorType() = 0;

    virtual ClipBoardImageAlphaType GetAlphaType() = 0;

    virtual uint32_t* GetData() = 0;

    virtual size_t GetDataSize() = 0;

    virtual size_t GetRowBytes() = 0;

    virtual int32_t GetWidth() = 0;

    virtual int32_t GetHeight() = 0;

    virtual void SetColorType(ClipBoardImageColorType color) = 0;

    virtual void SetAlphaType(ClipBoardImageAlphaType alpha) = 0;

    virtual void SetData(uint32_t* data) = 0;

    virtual void SetDataSize(size_t size) = 0;

    virtual void SetRowBytes(size_t rowBytes) = 0;

    virtual void SetWidth(int32_t width) = 0;

    virtual void SetHeight(int32_t height) = 0;
};

class PasteDataRecordAdapter;
class PasteDataAdapter;
using PasteRecordVector = std::vector<std::shared_ptr<PasteDataRecordAdapter>>;
using PasteCustomData = std::map<std::string, std::vector<uint8_t>>;

class PasteboardObserverAdapter {
public:
    PasteboardObserverAdapter() = default;

    virtual ~PasteboardObserverAdapter() = default;

    virtual void OnPasteboardChanged() = 0;
};

class PasteBoardClientAdapter {
public:
    PasteBoardClientAdapter() = default;

    virtual ~PasteBoardClientAdapter() = default;

    virtual bool GetPasteData(PasteRecordVector& data) = 0;

    virtual void SetPasteData(
        const PasteRecordVector& data, CopyOptionMode copyOption = CopyOptionMode::CROSS_DEVICE) = 0;

    virtual bool HasPasteData() = 0;

    virtual void Clear() = 0;

    virtual int32_t OpenRemoteUri(const std::string& path) = 0;

    virtual bool IsLocalPaste() = 0;

    virtual uint32_t GetTokenId() = 0;

    virtual int32_t AddPasteboardChangedObserver(std::shared_ptr<PasteboardObserverAdapter> callback) = 0;

    virtual void RemovePasteboardChangedObserver(int32_t callbackId) = 0;

    virtual bool HasType(const char* type) { return false; }
};

class PasteDataRecordAdapter {
public:
    PasteDataRecordAdapter() = default;

    virtual ~PasteDataRecordAdapter() = default;

    static std::shared_ptr<PasteDataRecordAdapter> NewRecord(const std::string& mimeType);

    static std::shared_ptr<PasteDataRecordAdapter> NewRecord(
        const std::string& mimeType, std::shared_ptr<std::string> htmlText, std::shared_ptr<std::string> plainText);

    virtual bool SetHtmlText(std::shared_ptr<std::string> htmlText) = 0;

    virtual bool SetPlainText(std::shared_ptr<std::string> plainText) = 0;

    virtual bool SetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData) = 0;

    virtual std::string GetMimeType() = 0;

    virtual std::shared_ptr<std::string> GetHtmlText() = 0;

    virtual std::shared_ptr<std::string> GetPlainText() = 0;

    virtual bool GetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData) = 0;

    virtual bool SetUri(const std::string& uriString) = 0;

    virtual bool SetCustomData(PasteCustomData& data) = 0;

    virtual std::shared_ptr<std::string> GetUri() = 0;

    virtual std::shared_ptr<PasteCustomData> GetCustomData() = 0;

    virtual bool SetHtmlTextV2(std::shared_ptr<std::string> htmlText) { return false; }

    virtual bool SetPlainTextV2(std::shared_ptr<std::string> plainText) { return false; }

    virtual std::shared_ptr<std::string> GetHtmlTextV2() { return nullptr; }

    virtual std::shared_ptr<std::string> GetPlainTextV2() { return nullptr; }

    virtual bool SetCustomDataV2(PasteCustomData& data) { return false; }

    virtual std::shared_ptr<std::string> GetUriV2() { return nullptr; }

    virtual std::shared_ptr<PasteCustomData> GetCustomDataV2() { return nullptr; }
};

class PasteDataAdapter {
public:
    PasteDataAdapter() = default;

    virtual ~PasteDataAdapter() = default;

    virtual void AddHtmlRecord(const std::string& html) = 0;

    virtual void AddTextRecord(const std::string& text) = 0;

    virtual std::vector<std::string> GetMimeTypes() = 0;

    virtual std::shared_ptr<std::string> GetPrimaryHtml() = 0;

    virtual std::shared_ptr<std::string> GetPrimaryText() = 0;

    virtual std::shared_ptr<std::string> GetPrimaryMimeType() = 0;

    virtual std::shared_ptr<PasteDataRecordAdapter> GetRecordAt(std::size_t index) = 0;

    virtual std::size_t GetRecordCount() = 0;

    virtual PasteRecordVector AllRecords() = 0;
};

} // namespace OHOS::NWeb

#endif
