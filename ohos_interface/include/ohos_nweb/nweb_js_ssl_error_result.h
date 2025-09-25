/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NWEB_JS_SSL_ERROR_RESULT_H
#define NWEB_JS_SSL_ERROR_RESULT_H

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {

class OHOS_NWEB_EXPORT NWebJSSslErrorResult {
public:
    virtual ~NWebJSSslErrorResult() = default;
    /**
     * @brief Handle a confirmation response from the user, the url request will be continued.
     */
    virtual void HandleConfirm() = 0;
    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    virtual void HandleCancel() = 0;
    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    virtual void HandleCancelV2(bool abortLoading) {}
};

class OHOS_NWEB_EXPORT NWebJSAllSslErrorResult {
public:
    virtual ~NWebJSAllSslErrorResult() = default;
    /**
     * @brief Handle a confirmation response from the user, the url request will be continued.
     */
    virtual void HandleConfirm() = 0;
    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    virtual void HandleCancel() = 0;
    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    virtual void HandleCancelV2(bool abortLoading) {}
};

} // namespace OHOS::NWeb

#endif