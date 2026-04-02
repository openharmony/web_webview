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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or wrapperied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_WEB_ALL_SSL_ERROR_INFO_H_
#define ARK_WEB_ALL_SSL_ERROR_INFO_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebAllSslErrorInfo : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetError() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetUrl() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetOriginalUrl() = 0;

    /*--ark web()--*/
    virtual ArkWebString GetReferrer() = 0;

    /*--ark web()--*/
    virtual bool GetIsFatalError() = 0;

    /*--ark web()--*/
    virtual bool GetIsMainFrame() = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetCertChainData() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_ALL_SSL_ERROR_INFO_H_
