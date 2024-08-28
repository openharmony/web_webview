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

#include "media_codec_list_adapter_impl.h"

#include <unordered_map>

#include "nweb_log.h"
#include <media/native_avcapability.h>
#include <media/native_avmagic.h>

namespace OHOS::NWeb {

MediaCodecListAdapterImpl& MediaCodecListAdapterImpl::GetInstance()
{
    static MediaCodecListAdapterImpl instance;
    return instance;
}

std::shared_ptr<CapabilityDataAdapter> MediaCodecListAdapterImpl::GetCodecCapability(
    const std::string& mime, const bool isEncoder)
{
    std::shared_ptr<CapabilityDataAdapterImpl> capabilityAdapter = std::make_shared<CapabilityDataAdapterImpl>();
    std::string& mine = const_cast<std::string&>(mime);
    OH_AVCapability *oh_AVCapability = OH_AVCodec_GetCapability(mine.c_str(), true);
    if (oh_AVCapability == nullptr) {
        WVLOG_E("OH_AVCodec_GetCapability failed.");
        return nullptr;
    }
    OH_AVRange widthRange;
    OH_AVErrCode width_error_code = OH_AVCapability_GetVideoWidthRange(oh_AVCapability, &widthRange);
    if (width_error_code != AV_ERR_OK) {
        capabilityAdapter->SetMaxWidth(0);
        WVLOG_E("OH_AVCapability_GetVideoWidthRange failed. SetMaxWidth zero");
    } else {
        capabilityAdapter->SetMaxWidth(widthRange.maxVal);
    }

    OH_AVRange heightRange;
    OH_AVErrCode height_error_code = OH_AVCapability_GetVideoHeightRange(oh_AVCapability, &heightRange);
    if (height_error_code != AV_ERR_OK) {
        capabilityAdapter->SetMaxHeight(0);
        WVLOG_E("OH_AVCapability_GetVideoHeightRange failed. SetMaxHeight zero");
    } else {
        capabilityAdapter->SetMaxHeight(heightRange.maxVal);
    }

    OH_AVRange frameRateRange;
    OH_AVErrCode frameRate_error_code = OH_AVCapability_GetVideoFrameRateRange(oh_AVCapability, &frameRateRange);
    if (frameRate_error_code != AV_ERR_OK) {
        capabilityAdapter->SetMaxframeRate(0);
        WVLOG_E("OH_AVCapability_GetVideoFrameRateRange failed. SetMaxframeRate zero");
    } else {
        capabilityAdapter->SetMaxframeRate(frameRateRange.maxVal);
    }
    return capabilityAdapter;
}
} // namespace OHOS::NWeb
