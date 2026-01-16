/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_USER_AGENT_METADATA_ACK_H_
#define ARK_WEB_USER_AGENT_METADATA_ACK_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebUserAgentMetadataAck : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void SetBrandVersionList(const ArkWebStringVector& brand, const ArkWebStringVector& majorVersion,
        const ArkWebStringVector& fullVersion) = 0;

    /*--ark web()--*/
    virtual void SetArchitecture(const ArkWebString& arch) = 0;

    /*--ark web()--*/
    virtual void SetBitness(const ArkWebString& bitness) = 0;

    /*--ark web()--*/
    virtual void SetFormFactors(const ArkWebStringVector& formFactors) = 0;

    /*--ark web()--*/
    virtual void SetFullVersion(const ArkWebString& fullVersion) = 0;

    /*--ark web()--*/
    virtual void SetMobile(const bool isMobile) = 0;

    /*--ark web()--*/
    virtual void SetModel(const ArkWebString& model) = 0;

    /*--ark web()--*/
    virtual void SetPlatform(const ArkWebString& platform) = 0;

    /*--ark web()--*/
    virtual void SetPlatformVersion(const ArkWebString& platformVersion) = 0;

    /*--ark web()--*/
    virtual void SetWow64(const bool isWow64) = 0;

    /*--ark web()--*/
    virtual void GetBrandVersionList(
        ArkWebStringVector& brand, ArkWebStringVector& majorVersion, ArkWebStringVector& fullVersion) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetArchitecture() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetBitness() = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetFormFactors() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetFullVersion() = 0;

    /*--ark web()--*/
    virtual bool GetMobile() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetModel() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetPlatform() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetPlatformVersion() = 0;

    /*--ark web()--*/
    virtual bool GetWow64() = 0;
};
} // namespace OHOS::ArkWeb

#endif // ARK_WEB_USER_AGENT_METADATA_ACK_H_
