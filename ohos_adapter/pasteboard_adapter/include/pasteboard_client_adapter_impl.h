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
#ifndef PASTEBOARD_CLIENT_ADAPTER_IMPL_H
#define PASTEBOARD_CLIENT_ADAPTER_IMPL_H

#include "pasteboard_client_adapter.h"

#include <mutex>
#include <database/udmf/udmf.h>
#include <database/pasteboard/oh_pasteboard.h>

namespace OHOS::NWeb {
class PasteDataRecordAdapterImpl : public PasteDataRecordAdapter {
public:
    explicit PasteDataRecordAdapterImpl(
        OH_UdmfRecord* record);
    PasteDataRecordAdapterImpl(const std::string& mimeType,
                               std::shared_ptr<std::string> htmlText,
                               std::shared_ptr<std::string> plainText);
    ~PasteDataRecordAdapterImpl() override;
    explicit PasteDataRecordAdapterImpl(const std::string& mimeType);
    bool SetHtmlText(std::shared_ptr<std::string> htmlText) override;
    bool SetPlainText(std::shared_ptr<std::string> plainText) override;
    bool SetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData) override;
    bool SetUri(const std::string& uriString) override;
    bool SetCustomData(PasteCustomData& data) override;
    std::string GetMimeType() override;
    std::shared_ptr<std::string> GetHtmlText() override;
    std::shared_ptr<std::string> GetPlainText() override;
    OH_UdmfRecord* GetRecord();
    bool GetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData) override;
    std::shared_ptr<std::string> GetUri() override;
    std::shared_ptr<PasteCustomData> GetCustomData() override;
    void Clear();
private:
    OH_UdmfRecord* record_ = nullptr;
    std::string mimeType_ = "";
    uint8_t *imgBuffer_ = nullptr;
    uint32_t bufferSize_ = 0;
    void ClearImgBuffer();
};

class PasteDataAdapterImpl : public PasteDataAdapter {
public:
    PasteDataAdapterImpl();
    ~PasteDataAdapterImpl() override;
    explicit PasteDataAdapterImpl(OH_UdmfData* data);
    void AddHtmlRecord(const std::string &html) override;
    void AddTextRecord(const std::string &text) override;
    std::vector<std::string> GetMimeTypes() override;
    std::shared_ptr<std::string> GetPrimaryHtml() override;
    std::shared_ptr<std::string> GetPrimaryText() override;
    std::shared_ptr<std::string> GetPrimaryMimeType() override;
    std::shared_ptr<PasteDataRecordAdapter> GetRecordAt(std::size_t index) override;
    std::size_t GetRecordCount() override;
    PasteRecordVector AllRecords() override;
private:
    OH_UdmfData* data_ = nullptr;
};

typedef struct {
    std::shared_ptr<PasteboardObserverAdapter> callback;
} PasteBoardCallback;
using ObserverMap =
    std::map<int32_t, OH_PasteboardObserver*>;
class PasteBoardClientAdapterImpl : public PasteBoardClientAdapter {
public:
    static PasteBoardClientAdapterImpl& GetInstance();
    ~PasteBoardClientAdapterImpl() override;
    bool GetPasteData(PasteRecordVector& data) override;
    void SetPasteData(const PasteRecordVector& data,
                      CopyOptionMode copyOption = CopyOptionMode::CROSS_DEVICE) override;
    bool HasPasteData() override;
    void Clear() override;
    int32_t OpenRemoteUri(const std::string& path) override;
    bool IsLocalPaste() override;
    uint32_t GetTokenId() override;
    int32_t AddPasteboardChangedObserver(std::shared_ptr<PasteboardObserverAdapter> callback) override;
    void RemovePasteboardChangedObserver(int32_t callbackId) override;
private:
    PasteBoardClientAdapterImpl();
    PasteBoardClientAdapterImpl(const PasteBoardClientAdapterImpl&) = delete;
    PasteBoardClientAdapterImpl& operator=(const PasteBoardClientAdapterImpl&) = delete;
    uint32_t tokenId_ = 0;
    bool isLocalPaste_ = false;
    ObserverMap reg_;
    std::mutex mutex_;
    std::string webviewPasteDataTag_ = "WebviewPasteDataTag";
    Udmf_ShareOption TransitionCopyOption(CopyOptionMode copyOption);
    OH_Pasteboard* pasteboard_ = nullptr;
    std::shared_ptr<PasteBoardCallback> pasteCallback_;
};
} // namespace OHOS::NWeb

#endif // PASTEBOARD_CLIENT_ADAPTER_IMPL_H
