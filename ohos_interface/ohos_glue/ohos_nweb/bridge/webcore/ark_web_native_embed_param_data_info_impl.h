/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_NATIVE_EMBED_PARAM_DATA_INFO_IMPL_H_
#define ARK_WEB_NATIVE_EMBED_PARAM_DATA_INFO_IMPL_H_
#pragma once

#include "include/nweb_handler.h"
#include "ohos_nweb/include/ark_web_native_embed_param_data_info.h"

namespace OHOS::ArkWeb {

class ArkWebNativeEmbedParamDataInfoImpl : public ArkWebNativeEmbedParamDataInfo {
    IMPLEMENT_REFCOUNTING(ArkWebNativeEmbedParamDataInfoImpl);

public:
    ArkWebNativeEmbedParamDataInfoImpl(
        std::shared_ptr<OHOS::NWeb::NWebNativeEmbedParamDataInfo> nweb_native_embed_param_data_info);
    ~ArkWebNativeEmbedParamDataInfoImpl() = default;

    ArkWebString GetEmbedId() override;

    ArkWebString GetObjectAttributeId() override;

    ArkWebNativeEmbedParamItemVector GetParamItems() override;

private:
    std::shared_ptr<OHOS::NWeb::NWebNativeEmbedParamDataInfo> nweb_native_embed_param_data_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NATIVE_EMBED_PARAM_DATA_INFO_IMPL_H_