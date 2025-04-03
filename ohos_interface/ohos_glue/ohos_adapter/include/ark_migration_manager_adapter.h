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

#ifndef ARK_MIGRATION_MANAGER_ADAPTER_H
#define ARK_MIGRATION_MANAGER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkMigrationListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnMigrationReply(int32_t errorCode, int32_t succussCount, const ArkWebInt32Vector& errorIndex,
        const ArkWebInt32Vector& codeIndex) = 0;
};

/*--ark web(source=webview)--*/
class ArkMigrationManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void SetMigrationParam(
        const ArkWebString& bundleName, const ArkWebString& abilityName, const ArkWebString& token) = 0;

    /*--ark web()--*/
    virtual bool SendMigrationRequest(void* jsonData) = 0;

    /*--ark web()--*/
    virtual uint32_t RegisterMigrationListener(ArkWebRefPtr<ArkMigrationListenerAdapter> listener) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MIGRATION_MANAGER_ADAPTER_H
