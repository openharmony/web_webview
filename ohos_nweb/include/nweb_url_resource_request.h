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

#ifndef NWEB_URL_RESOURCE_REQUEST_H
#define NWEB_URL_RESOURCE_REQUEST_H

#include <map>
#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebUrlResourceRequest {
public:
    NWebUrlResourceRequest() = default;

    virtual ~NWebUrlResourceRequest() = default;

    /**
     * @brief Get the method associated with the request, for example "GET".
     *
     * @retval method the method associated with the request.
     */
    virtual const std::string &Method() = 0;

    /**
     * @brief Get the headers associated with the request.
     *
     * @retval the headers associated with the request.
     */
    virtual const std::map<std::string, std::string> &RequestHeaders() = 0;

    /**
     * @brief Get the URL for which the resource request was made.
     *
     * @retval URL url string
     */
    virtual const std::string &Url() = 0;

    /**
     * @brief Get whether a gesture (such as a click) was associated with the
     * request.
     *
     * @retval gesture whether was associated with the request.
     */
    virtual bool FromGesture() = 0;

    /**
     * @brief Get whether the request was made in order to fetch the main frame's
     * document.
     *
     * @retval Is main frame
     */
    virtual bool IsAboutMainFrame() = 0;

    /**
     * @brief Get whether the request was a result of a server-side redirect.
     *
     * @retval is redirect
     */
    virtual bool IsRequestRedirect() = 0;
};
}  // namespace OHOS::NWeb

#endif  // NWEB_URL_RESOURCE_REQUEST_H
