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

#include "ohos_adapter/ctocpp/ark_audio_info_adapter_ctocpp.h"

#include "ohos_adapter/ctocpp/ark_audio_capture_info_adapter_ctocpp.h"
#include "ohos_adapter/ctocpp/ark_audio_enc_info_adapter_ctocpp.h"

#include "base/ctocpp/ark_web_ctocpp_macros.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioCaptureInfoAdapter> ArkAudioInfoAdapterCToCpp::GetMicCapInfo()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_audio_info_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_mic_cap_info, nullptr);

    // Execute
    ark_audio_capture_info_adapter_t* _retval = _struct->get_mic_cap_info(_struct);

    // Return type: refptr_same
    return ArkAudioCaptureInfoAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioCaptureInfoAdapter> ArkAudioInfoAdapterCToCpp::GetInnerCapInfo()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_audio_info_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_inner_cap_info, nullptr);

    // Execute
    ark_audio_capture_info_adapter_t* _retval = _struct->get_inner_cap_info(_struct);

    // Return type: refptr_same
    return ArkAudioCaptureInfoAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioEncInfoAdapter> ArkAudioInfoAdapterCToCpp::GetAudioEncInfo()
{
    ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

    ark_audio_info_adapter_t* _struct = GetStruct();
    ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

    ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_audio_enc_info, nullptr);

    // Execute
    ark_audio_enc_info_adapter_t* _retval = _struct->get_audio_enc_info(_struct);

    // Return type: refptr_same
    return ArkAudioEncInfoAdapterCToCpp::Invert(_retval);
}

ArkAudioInfoAdapterCToCpp::ArkAudioInfoAdapterCToCpp() {}

ArkAudioInfoAdapterCToCpp::~ArkAudioInfoAdapterCToCpp() {}

template<>
ArkWebBridgeType
    ArkWebCToCppRefCounted<ArkAudioInfoAdapterCToCpp, ArkAudioInfoAdapter, ark_audio_info_adapter_t>::kBridgeType =
        ARK_AUDIO_INFO_ADAPTER;

} // namespace OHOS::ArkWeb