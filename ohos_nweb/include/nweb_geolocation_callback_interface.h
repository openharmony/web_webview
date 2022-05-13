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

#ifndef NWEB_GEOLOCATION_CALLBACK_INTERFACE_H
#define NWEB_GEOLOCATION_CALLBACK_INTERFACE_H

#include <string>

#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebGeolocationCallbackInterface {
public:
    NWebGeolocationCallbackInterface() = default;

    virtual ~NWebGeolocationCallbackInterface() = default;
    /**
    * @brief Report the geolocation permission status from usrs.
    *
    * @param origin The origin that ask for the geolocation permission.
    * @param allow The geolocation permission status.
    * @param retain Whether to allow the geolocation permission status to be
    * saved to the system.
    */
    virtual void GeolocationCallbackInvoke(const std::string& origin,
                                           bool allow,
                                           bool retain) = 0;
};
} // namespace OHOS::NWeb

#endif  // NWEB_GEOLOCATION_CALLBACK_INTERFACE_H
