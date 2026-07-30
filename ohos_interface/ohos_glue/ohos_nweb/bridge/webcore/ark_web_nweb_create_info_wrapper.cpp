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

#include "ohos_nweb/bridge/ark_web_nweb_create_info_wrapper.h"

#include "ohos_nweb/bridge/ark_web_engine_init_args_wrapper.h"
#include "ohos_nweb/bridge/ark_web_output_frame_callback_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNWebCreateInfoWrapper::ArkWebNWebCreateInfoWrapper(ArkWebRefPtr<ArkWebNWebCreateInfo> ark_web_nweb_create_info)
    : ark_web_nweb_create_info_(ark_web_nweb_create_info)
{}

uint32_t ArkWebNWebCreateInfoWrapper::GetWidth()
{
    return ark_web_nweb_create_info_->GetWidth();
}

uint32_t ArkWebNWebCreateInfoWrapper::GetHeight()
{
    return ark_web_nweb_create_info_->GetHeight();
}

bool ArkWebNWebCreateInfoWrapper::GetIsIncognitoMode()
{
    return ark_web_nweb_create_info_->GetIsIncognitoMode();
}

void* ArkWebNWebCreateInfoWrapper::GetProducerSurface()
{
    return ark_web_nweb_create_info_->GetProducerSurface();
}

void* ArkWebNWebCreateInfoWrapper::GetEnhanceSurfaceInfo()
{
    return ark_web_nweb_create_info_->GetEnhanceSurfaceInfo();
}

std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> ArkWebNWebCreateInfoWrapper::GetEngineInitArgs()
{
    ArkWebRefPtr<ArkWebEngineInitArgs> ark_web_engine_init_args = ark_web_nweb_create_info_->GetEngineInitArgs();
    if (CHECK_REF_PTR_IS_NULL(ark_web_engine_init_args)) {
        return nullptr;
    }

    return std::make_shared<ArkWebEngineInitArgsWrapper>(ark_web_engine_init_args);
}

std::shared_ptr<OHOS::NWeb::NWebOutputFrameCallback> ArkWebNWebCreateInfoWrapper::GetOutputFrameCallback()
{
    ArkWebRefPtr<ArkWebOutputFrameCallback> ark_web_output_frame_callback =
        ark_web_nweb_create_info_->GetOutputFrameCallback();
    if (CHECK_REF_PTR_IS_NULL(ark_web_output_frame_callback)) {
        return nullptr;
    }

    return std::make_shared<ArkWebOutputFrameCallbackWrapper>(ark_web_output_frame_callback);
}

uint64_t ArkWebNWebCreateInfoWrapper::GetAsyncStackId()
{
    return ark_web_nweb_create_info_->GetAsyncStackId();
}

} // namespace OHOS::ArkWeb
