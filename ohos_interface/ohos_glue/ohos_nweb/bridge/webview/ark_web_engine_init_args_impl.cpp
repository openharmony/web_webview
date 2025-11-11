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

#include "ohos_nweb/bridge/ark_web_engine_init_args_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebEngineInitArgsImpl::ArkWebEngineInitArgsImpl(
    std::shared_ptr<OHOS::NWeb::NWebEngineInitArgs> nweb_engine_init_args)
    : nweb_engine_init_args_(nweb_engine_init_args)
{}

bool ArkWebEngineInitArgsImpl::GetIsPopup()
{
    return nweb_engine_init_args_->GetIsPopup();
}

ArkWebString ArkWebEngineInitArgsImpl::GetDumpPath()
{
    return ArkWebStringClassToStruct(nweb_engine_init_args_->GetDumpPath());
}

ArkWebStringList ArkWebEngineInitArgsImpl::GetArgsToAdd()
{
    return ArkWebStringListClassToStruct(nweb_engine_init_args_->GetArgsToAdd());
}

ArkWebStringList ArkWebEngineInitArgsImpl::GetArgsToDelete()
{
    return ArkWebStringListClassToStruct(nweb_engine_init_args_->GetArgsToDelete());
}

bool ArkWebEngineInitArgsImpl::GetIsFrameInfoDump()
{
    return nweb_engine_init_args_->GetIsFrameInfoDump();
}

bool ArkWebEngineInitArgsImpl::GetIsEnhanceSurface()
{
    return nweb_engine_init_args_->GetIsEnhanceSurface();
}

bool ArkWebEngineInitArgsImpl::GetIsMultiRendererProcess()
{
    return nweb_engine_init_args_->GetIsMultiRendererProcess();
}

ArkWebString ArkWebEngineInitArgsImpl::GetSharedRenderProcessToken()
{
    return ArkWebStringClassToStruct(nweb_engine_init_args_->GetSharedRenderProcessToken());
}

bool ArkWebEngineInitArgsImpl::GetEmulateTouchFromMouseEvent()
{
    return nweb_engine_init_args_->GetEmulateTouchFromMouseEvent();
}

} // namespace OHOS::ArkWeb
