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

#include "ctocpp/ark_decoder_callback_adapter_ctocpp.h"

#include "ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
void ArkDecoderCallbackAdapterCToCpp::OnError(int32_t errorType, int32_t errorCode)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_decoder_callback_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_error, );

    // Execute
    _struct->on_error(_struct, errorType, errorCode);
}

ARK_WEB_NO_SANITIZE
void ArkDecoderCallbackAdapterCToCpp::OnStreamChanged(const ArkDecoderFormat& format)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_decoder_callback_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_stream_changed, );

    // Execute
    _struct->on_stream_changed(_struct, &format);
}

ARK_WEB_NO_SANITIZE
void ArkDecoderCallbackAdapterCToCpp::OnNeedInputData(uint32_t index, ArkOhosBuffer buffer)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_decoder_callback_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_need_input_data, );

    // Execute
    _struct->on_need_input_data(_struct, index, buffer);
}

ARK_WEB_NO_SANITIZE
void ArkDecoderCallbackAdapterCToCpp::OnNeedOutputData(uint32_t index, ArkBufferInfo info, uint32_t flag)
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_decoder_callback_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, );

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, on_need_output_data, );

    // Execute
    _struct->on_need_output_data(_struct, index, info, flag);
}

ArkDecoderCallbackAdapterCToCpp::ArkDecoderCallbackAdapterCToCpp() {}

ArkDecoderCallbackAdapterCToCpp::~ArkDecoderCallbackAdapterCToCpp() {}

template<>
ArkWebBridgeType ArkWebCToCppRefCounted<ArkDecoderCallbackAdapterCToCpp, ArkDecoderCallbackAdapter,
    ark_decoder_callback_adapter_t>::kBridgeType = ARK_DECODER_CALLBACK_ADAPTER;

} // namespace OHOS::ArkWeb