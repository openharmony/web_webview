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

#ifndef MEDIA_CODEC_LIST_ADAPTER_IMPL_H
#define MEDIA_CODEC_LIST_ADAPTER_IMPL_H

#include "media_codec_adapter.h"
#include "capability_data_adapter_impl.h"

namespace OHOS::NWeb {
class MediaCodecListAdapterImpl : public MediaCodecListAdapter {
public:
    static MediaCodecListAdapterImpl& GetInstance();

    MediaCodecListAdapterImpl() = default;

    ~MediaCodecListAdapterImpl() override = default;

    std::shared_ptr<CapabilityDataAdapter> GetCodecCapability(const std::string& mime, const bool isEncoder) override;
};

} // namespace OHOS::NWeb
#endif // MEDIA_CODEC_LIST_ADAPTER_IMPL_H
