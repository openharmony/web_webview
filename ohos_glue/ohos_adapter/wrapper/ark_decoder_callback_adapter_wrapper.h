/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_DECODER_CALLBACK_ADAPTER_WRAPPER_H
#define ARK_DECODER_CALLBACK_ADAPTER_WRAPPER_H

#pragma once

#include "include/ark_media_codec_decoder_adapter.h"
#include "media_codec_decoder_adapter.h"

namespace OHOS::ArkWeb {

class ArkDecoderCallbackAdapterWrapper : public OHOS::NWeb::DecoderCallbackAdapter {
public:
    ArkDecoderCallbackAdapterWrapper(ArkWebRefPtr<ArkDecoderCallbackAdapter>);

    void OnError(OHOS::NWeb::ErrorType errorType, int32_t errorCode) override;

    void OnStreamChanged(const OHOS::NWeb::DecoderFormat& format) override;

    void OnNeedInputData(uint32_t index, OHOS::NWeb::OhosBuffer buffer) override;

    void OnNeedOutputData(uint32_t index, OHOS::NWeb::BufferInfo info, OHOS::NWeb::BufferFlag flag) override;

private:
    ArkWebRefPtr<ArkDecoderCallbackAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DECODER_CALLBACK_ADAPTER_WRAPPER_H