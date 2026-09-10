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

#ifndef ARK_WEB_NWEB_CREATE_INFO_WRAPPER_H_
#define ARK_WEB_NWEB_CREATE_INFO_WRAPPER_H_
#pragma once

#include "include/nweb.h"
#include "ohos_nweb/include/ark_web_nweb_create_info.h"

namespace OHOS::ArkWeb {

class ArkWebNWebCreateInfoWrapper : public OHOS::NWeb::NWebCreateInfo {
public:
    ArkWebNWebCreateInfoWrapper(ArkWebRefPtr<ArkWebNWebCreateInfo> ark_web_nweb_create_info);
    ~ArkWebNWebCreateInfoWrapper() = default;

    uint32_t GetWidth() override;

    uint32_t GetHeight() override;

    bool GetIsIncognitoMode() override;

    void* GetProducerSurface() override;

    void* GetEnhanceSurfaceInfo() override;

    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> GetEngineInitArgs() override;

    std::shared_ptr<OHOS::NWeb::NWebOutputFrameCallback> GetOutputFrameCallback() override;

    uint64_t GetAsyncStackId() override;

private:
    ArkWebRefPtr<ArkWebNWebCreateInfo> ark_web_nweb_create_info_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_NWEB_CREATE_INFO_WRAPPER_H_
