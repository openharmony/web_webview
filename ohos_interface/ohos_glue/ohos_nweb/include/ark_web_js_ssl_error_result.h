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

#ifndef ARK_WEB_JS_SSL_ERROR_RESULT_H_
#define ARK_WEB_JS_SSL_ERROR_RESULT_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebJsSslErrorResult : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    /*--ark web()--*/
    virtual void HandleCancel() = 0;

    /**
     * @brief Handle a confirmation response from the user, the url request will
     *        be continued.
     */
    /*--ark web()--*/
    virtual void HandleConfirm() = 0;

    /**
     * @brief Handle the result if the user cancelled the url request.
     */
    /*--ark web()--*/
    virtual void HandleCancelV2(bool abortLoading) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_JS_SSL_ERROR_RESULT_H_
