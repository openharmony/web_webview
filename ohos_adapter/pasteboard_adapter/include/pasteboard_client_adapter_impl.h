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

#include <string>
#include "pasteboard_client_adapter.h"

namespace OHOS::NWeb {
class PasteDataRecordAdapterImpl : public PasteDataRecordAdapter {
public:
    PasteDataRecordAdapterImpl(const std::string& mimeType,
                               std::shared_ptr<std::string> htmlText,
                               std::shared_ptr<std::string> plainText){}
    explicit PasteDataRecordAdapterImpl(const std::string& mimeType){}
    bool SetHtmlText(std::shared_ptr<std::string> htmlText) override{ return false; }
    bool SetPlainText(std::shared_ptr<std::string> plainText) override{ return false; }
    bool SetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData) override{ return false; }
    bool SetUri(const std::string& uriString) override{ return false; }
    bool SetCustomData(PasteCustomData& data) override{ return false; }
    std::string GetMimeType() override{ return ""; }
    std::shared_ptr<std::string> GetHtmlText() override{ return nullptr; }
    std::shared_ptr<std::string> GetPlainText() override{ return nullptr; }
    bool GetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData) override{ return false; }
    std::shared_ptr<std::string> GetUri() override{ return nullptr; }
    std::shared_ptr<PasteCustomData> GetCustomData() override{ return nullptr; }
    void Clear(){}
};

std::shared_ptr<PasteDataRecordAdapter> PasteDataRecordAdapter::NewRecord(
    const std::string& mimeType,
    std::shared_ptr<std::string> htmlText,
    std::shared_ptr<std::string> plainText) {
        return std::make_shared<PasteDataRecordAdapterImpl>(mimeType, htmlText, plainText);
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataRecordAdapter::NewRecord(
    const std::string& mimeType) {
        return std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
}

class PasteBoardClientAdapterImpl : public PasteBoardClientAdapter {
public:
    static PasteBoardClientAdapterImpl& GetInstance() {
        static PasteBoardClientAdapterImpl instance;
        return instance;
    }
    bool GetPasteData(PasteRecordVector& data) override { return false; }
    void SetPasteData(const PasteRecordVector& data,
                      CopyOptionMode copyOption = CopyOptionMode::CROSS_DEVICE) override{}
    bool HasPasteData() override{ return false; }
    void Clear() override{}
    int32_t OpenRemoteUri(const std::string& path) override{ return 0; }
    bool IsLocalPaste() override{ return false; }
    uint32_t GetTokenId() override{ return 0; }
    int32_t AddPasteboardChangedObserver(std::shared_ptr<PasteboardObserverAdapter> callback) override{ return 0; }
    void RemovePasteboardChangedObserver(int32_t callbackId) override{}
private:
    PasteBoardClientAdapterImpl() = default;
    PasteBoardClientAdapterImpl(const PasteBoardClientAdapterImpl&) = delete;
    PasteBoardClientAdapterImpl& operator=(const PasteBoardClientAdapterImpl&) = delete;
};
} // namespace OHOS::NWeb

#endif // PASTEBOARD_CLIENT_ADAPTER_IMPL_H
