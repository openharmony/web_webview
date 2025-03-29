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

#include "ohos_adapter/bridge/ark_migration_listener_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMigrationListenerAdapterWrapper::ArkMigrationListenerAdapterWrapper(ArkWebRefPtr<ArkMigrationListenerAdapter> ref)
    : ctocpp_(ref)
{}

void ArkMigrationListenerAdapterWrapper::OnMigrationReply(int32_t errorCode, int32_t successCount,
    const std::vector<int32_t>& errorIndex, const std::vector<int32_t>& codeList)
{
    ArkWebInt32Vector errorIndexVec;
    ArkWebInt32Vector codeListVec;

    errorIndexVec = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(errorIndex);
    codeListVec = ArkWebBasicVectorClassToStruct<int32_t, ArkWebInt32Vector>(codeList);
    
    ctocpp_->OnMigrationReply(errorCode, successCount, errorIndexVec, codeListVec);
    ArkWebBasicVectorStructRelease(errorIndexVec);
    ArkWebBasicVectorStructRelease(codeListVec);
}

} // namespace OHOS::ArkWeb
