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

#ifndef ARK_WEB_URL_RESOURCE_ERROR_H_
#define ARK_WEB_URL_RESOURCE_ERROR_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebUrlResourceError : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Get the error code.
     *
     * @return The error code.
     */
    /*--ark web()--*/
    virtual int ErrorCode() = 0;

    /**
     * @brief Get the error info.
     *
     * @return The error info.
     */
    /*--ark web()--*/
    virtual ArkWebString ErrorInfo() = 0;

    /**
     * @brief Get the custom error code.
     *
     * @return The custom error code.
     */
    /*--ark web()--*/
    virtual int CustomErrorCode() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_URL_RESOURCE_ERROR_H_
