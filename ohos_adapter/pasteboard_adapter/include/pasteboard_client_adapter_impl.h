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

#include "paste_data.h"
#include "paste_data_record.h"
#include "pasteboard_client.h"

namespace OHOS::NWeb {
class PasteDataRecordAdapterImpl : public PasteDataRecordAdapter {
public:
    explicit PasteDataRecordAdapterImpl(
        std::shared_ptr<MiscServices::PasteDataRecord> record);
    PasteDataRecordAdapterImpl(const std::string& mimeType,
                               std::shared_ptr<std::string> htmlText,
                               std::shared_ptr<std::string> plainText);
    std::string GetMimeType() override;
    std::shared_ptr<std::string> GetHtmlText() override;
    std::shared_ptr<std::string> GetPlainText() override;
    std::shared_ptr<MiscServices::PasteDataRecord> GetRecord();
private:
    std::shared_ptr<MiscServices::PasteDataRecord> record_;
};

class PasteDataAdapterImpl : public PasteDataAdapter {
public:
    PasteDataAdapterImpl();
    explicit PasteDataAdapterImpl(std::shared_ptr<MiscServices::PasteData> data);
    void AddHtmlRecord(const std::string &html) override;
    void AddTextRecord(const std::string &text) override;
    std::vector<std::string> GetMimeTypes() override;
    std::shared_ptr<std::string> GetPrimaryHtml() override;
    std::shared_ptr<std::string> GetPrimaryText() override;
    std::shared_ptr<std::string> GetPrimaryMimeType() override;
    std::shared_ptr<PasteDataRecordAdapter> GetRecordAt(std::size_t index) override;
    std::size_t GetRecordCount() override;
    PasteRecordList AllRecords() const override;
private:
    std::shared_ptr<MiscServices::PasteData> data_;
};

class PasteBoardClientAdapterImpl : public PasteBoardClientAdapter {
public:
    PasteBoardClientAdapterImpl() = default;
    static PasteBoardClientAdapterImpl& GetInstance();
    bool GetPasteData(PasteRecordList& data) override;
    void SetPasteData(const PasteRecordList& data) override;
    bool HasPasteData() override;
    void Clear() override;
};
}

#endif