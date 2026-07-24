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

#ifndef ARK_WEB_URL_RESOURCE_ERROR_WRAPPER_H_
#define ARK_WEB_URL_RESOURCE_ERROR_WRAPPER_H_
#pragma once

#include "include/nweb_url_resource_error.h"
#include "ohos_nweb/include/ark_web_url_resource_error.h"

namespace OHOS::ArkWeb {

class ArkWebUrlResourceErrorWrapper : public OHOS::NWeb::NWebUrlResourceError {
public:
    ArkWebUrlResourceErrorWrapper(ArkWebRefPtr<ArkWebUrlResourceError> ark_web_url_resource_error);
    ~ArkWebUrlResourceErrorWrapper() = default;

    /**
     * @brief Get the error code.
     *
     * @return The error code.
     */
    int ErrorCode() override;

    /**
     * @brief Get the error info.
     *
     * @return The error info.
     */
    std::string ErrorInfo() override;

    /**
     * @brief Get the custom error code.
     *
     * @return The custom error code.
     */
    int CustomErrorCode() override;

private:
    ArkWebRefPtr<ArkWebUrlResourceError> ark_web_url_resource_error_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_URL_RESOURCE_ERROR_WRAPPER_H_
