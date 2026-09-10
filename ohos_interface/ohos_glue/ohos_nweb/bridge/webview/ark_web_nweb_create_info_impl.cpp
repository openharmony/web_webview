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

#include "ohos_nweb/bridge/ark_web_nweb_create_info_impl.h"

#include "ohos_nweb/bridge/ark_web_engine_init_args_impl.h"
#include "ohos_nweb/bridge/ark_web_output_frame_callback_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebNWebCreateInfoImpl::ArkWebNWebCreateInfoImpl(std::shared_ptr<OHOS::NWeb::NWebCreateInfo> nweb_create_info)
    : nweb_create_info_(nweb_create_info)
{}

uint32_t ArkWebNWebCreateInfoImpl::GetWidth()
{
    return nweb_create_info_->GetWidth();
}

uint32_t ArkWebNWebCreateInfoImpl::GetHeight()
{
    return nweb_create_info_->GetHeight();
}

bool ArkWebNWebCreateInfoImpl::GetIsIncognitoMode()
{
    return nweb_create_info_->GetIsIncognitoMode();
}

void* ArkWebNWebCreateInfoImpl::GetProducerSurface()
{
    return nweb_create_info_->GetProducerSurface();
}

void* ArkWebNWebCreateInfoImpl::GetEnhanceSurfaceInfo()
{
    return nweb_create_info_->GetEnhanceSurfaceInfo();
}

ArkWebRefPtr<ArkWebEngineInitArgs> ArkWebNWebCreateInfoImpl::GetEngineInitArgs()
{
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> nweb_engine_init_args = nweb_create_info_->GetEngineInitArgs();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_engine_init_args)) {
        return nullptr;
    }

    return new ArkWebEngineInitArgsImpl(nweb_engine_init_args);
}

ArkWebRefPtr<ArkWebOutputFrameCallback> ArkWebNWebCreateInfoImpl::GetOutputFrameCallback()
{
    std::shared_ptr<OHOS::NWeb::NWebOutputFrameCallback> nweb_output_frame_callback =
        nweb_create_info_->GetOutputFrameCallback();
    if (CHECK_SHARED_PTR_IS_NULL(nweb_output_frame_callback)) {
        return nullptr;
    }

    return new ArkWebOutputFrameCallbackImpl(nweb_output_frame_callback);
}

uint64_t ArkWebNWebCreateInfoImpl::GetAsyncStackId()
{
    return nweb_create_info_->GetAsyncStackId();
}

} // namespace OHOS::ArkWeb
