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

#include "ohos_nweb/bridge/ark_web_engine_init_args_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebEngineInitArgsWrapper::ArkWebEngineInitArgsWrapper(ArkWebRefPtr<ArkWebEngineInitArgs> ark_web_engine_init_args)
    : ark_web_engine_init_args_(ark_web_engine_init_args)
{}

bool ArkWebEngineInitArgsWrapper::GetIsPopup()
{
    return ark_web_engine_init_args_->GetIsPopup();
}

std::string ArkWebEngineInitArgsWrapper::GetDumpPath()
{
    ArkWebString stDumpPath = ark_web_engine_init_args_->GetDumpPath();

    std::string objDumpPath = ArkWebStringStructToClass(stDumpPath);
    ArkWebStringStructRelease(stDumpPath);
    return objDumpPath;
}

std::list<std::string> ArkWebEngineInitArgsWrapper::GetArgsToAdd()
{
    ArkWebStringList stArgsToAdd = ark_web_engine_init_args_->GetArgsToAdd();

    std::list<std::string> objArgsToAdd = ArkWebStringListStructToClass(stArgsToAdd);
    ArkWebStringListStructRelease(stArgsToAdd);
    return objArgsToAdd;
}

std::list<std::string> ArkWebEngineInitArgsWrapper::GetArgsToDelete()
{
    ArkWebStringList stArgsToDelete = ark_web_engine_init_args_->GetArgsToDelete();

    std::list<std::string> objArgsToDelete = ArkWebStringListStructToClass(stArgsToDelete);
    ArkWebStringListStructRelease(stArgsToDelete);
    return objArgsToDelete;
}

bool ArkWebEngineInitArgsWrapper::GetIsFrameInfoDump()
{
    return ark_web_engine_init_args_->GetIsFrameInfoDump();
}

bool ArkWebEngineInitArgsWrapper::GetIsEnhanceSurface()
{
    return ark_web_engine_init_args_->GetIsEnhanceSurface();
}

bool ArkWebEngineInitArgsWrapper::GetIsMultiRendererProcess()
{
    return ark_web_engine_init_args_->GetIsMultiRendererProcess();
}

std::string ArkWebEngineInitArgsWrapper::GetSharedRenderProcessToken()
{
    ArkWebString arkwebSharedRenderProcessToken = ark_web_engine_init_args_->GetSharedRenderProcessToken();
    std::string sharedRenderProcessToken = ArkWebStringStructToClass(arkwebSharedRenderProcessToken);
    ArkWebStringStructRelease(arkwebSharedRenderProcessToken);
    return sharedRenderProcessToken;
}

bool ArkWebEngineInitArgsWrapper::GetEmulateTouchFromMouseEvent()
{
    return ark_web_engine_init_args_->GetEmulateTouchFromMouseEvent();
}

} // namespace OHOS::ArkWeb
