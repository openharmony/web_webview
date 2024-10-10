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

#include "ohos_adapter/bridge/ark_paste_board_client_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_paste_board_observer_adapter_impl.h"
#include "ohos_adapter/bridge/ark_paste_data_record_adapter_wrapper.h"
#include "ohos_adapter/ctocpp/ark_paste_data_record_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_paste_record_vector_ctocpp.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

static ArkPasteRecordVector ArkPasteRecordVectorClassToStruct(const NWeb::PasteRecordVector& class_value)
{
    ArkPasteRecordVector struct_value = { .size = class_value.size(), .ark_web_mem_free_func = ArkWebMemFree };
    if (struct_value.size > 0) {
        struct_value.value = (ark_paste_data_record_adapter_t**)ArkWebMemMalloc(
            sizeof(ark_paste_data_record_adapter_t*) * struct_value.size);
        if (struct_value.value == nullptr) {
            struct_value.size = 0;
            return struct_value;
        }
        
        int count = 0;
        for (auto it = class_value.begin(); it != class_value.end(); it++) {
            std::shared_ptr<ArkPasteDataRecordAdapterWrapper> data =
                std::static_pointer_cast<ArkPasteDataRecordAdapterWrapper>(*it);
            struct_value.value[count] = ArkPasteDataRecordAdapterCToCpp::Revert(data->ctocpp_);
            count++;
        }
    }
    return struct_value;
}

ArkPasteBoardClientAdapterWrapper::ArkPasteBoardClientAdapterWrapper(ArkWebRefPtr<ArkPasteBoardClientAdapter> ref)
    : ctocpp_(ref)
{}

bool ArkPasteBoardClientAdapterWrapper::GetPasteData(NWeb::PasteRecordVector& data)
{
    if (!ctocpp_) {
        return false;
    }
    ArkPasteRecordVector ark_vector;
    bool result = ctocpp_->GetPasteData(ark_vector);
    data = ArkPasteRecordVectorStructToClass(ark_vector);
    ArkPasteRecordVectorStructRelease(ark_vector);
    return result;
}

void ArkPasteBoardClientAdapterWrapper::SetPasteData(
    const NWeb::PasteRecordVector& data, NWeb::CopyOptionMode copyOption)
{
    if (!ctocpp_) {
        return;
    }
    ArkPasteRecordVector ark_vector = ArkPasteRecordVectorClassToStruct(data);
    ctocpp_->SetPasteData(ark_vector, (int32_t)copyOption);
    ArkPasteRecordVectorStructRelease(ark_vector);
}

bool ArkPasteBoardClientAdapterWrapper::HasPasteData()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->HasPasteData();
}

void ArkPasteBoardClientAdapterWrapper::Clear()
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->Clear();
}

int32_t ArkPasteBoardClientAdapterWrapper::OpenRemoteUri(const std::string& path)
{
    if (!ctocpp_) {
        return -1;
    }
    ArkWebString str = ArkWebStringClassToStruct(path);
    int32_t result = ctocpp_->OpenRemoteUri(str);
    ArkWebStringStructRelease(str);
    return result;
}

bool ArkPasteBoardClientAdapterWrapper::IsLocalPaste()
{
    if (!ctocpp_) {
        return false;
    }
    return ctocpp_->IsLocalPaste();
}

uint32_t ArkPasteBoardClientAdapterWrapper::GetTokenId()
{
    if (!ctocpp_) {
        return 0;
    }
    return ctocpp_->GetTokenId();
}

int32_t ArkPasteBoardClientAdapterWrapper::AddPasteboardChangedObserver(
    std::shared_ptr<NWeb::PasteboardObserverAdapter> callback)
{
    if (!ctocpp_) {
        return -1;
    }

    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        return ctocpp_->AddPasteboardChangedObserver(nullptr);
    }

    return ctocpp_->AddPasteboardChangedObserver(new ArkPasteBoardObserverAdapterImpl(callback));
}

void ArkPasteBoardClientAdapterWrapper::RemovePasteboardChangedObserver(int32_t callbackId)
{
    if (!ctocpp_) {
        return;
    }
    ctocpp_->RemovePasteboardChangedObserver(callbackId);
}

} // namespace OHOS::ArkWeb
