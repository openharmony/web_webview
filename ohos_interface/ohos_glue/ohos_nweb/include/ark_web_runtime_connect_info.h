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

#ifndef ARK_WEB_RUNTIME_CONNECTINFO_H_
#define ARK_WEB_RUNTIME_CONNECTINFO_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebRuntimeConnectInfo : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Retrieves application bundle identifier.
     *
     * @return bundleName
     */
    /*--ark web()--*/
    virtual ArkWebString GetBundleName() = 0;
    /**
     * @brief Gets the origin of the extension.
     *
     * @return extensionOrigin
     */
    /*--ark web()--*/
    virtual ArkWebString GetExtensionOrigin() = 0;
    /**
     * @brief Gets the read end of the message pipe.
     *
     * @return messageReadPipe
     */
    /*--ark web()--*/
    virtual int GetMessageReadPipe() = 0;
    /**
     * @brief Gets the write end of the message pipe.
     *
     * @return messageWritePipe
     */
    /*--ark web()--*/
    virtual int GetMessageWritePipe() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_RUNTIME_CONNECTINFO_H_
