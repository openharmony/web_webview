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

#ifndef NWEB_DATA_BASE_H
#define NWEB_DATA_BASE_H

#include <string>
#include <vector>

#include "nweb_export.h"

namespace OHOS::NWeb {
class OHOS_NWEB_EXPORT NWebDataBase {
public:
    enum WebPermissionType {
        GEOLOCATION_TYPE
    };
    NWebDataBase() = default;

    virtual ~NWebDataBase() = default;

    /**
     * @brief Get whether instances holds any http authentication credentials.
     *
     * @return true if instances saved any http authentication credentials.
     */
    virtual bool ExistHttpAuthCredentials() const = 0;

    /**
     * @brief clear all saved authentication credentials.
     *
     */
    virtual void DeleteHttpAuthCredentials() = 0;

    /**
     * @brief save http authentication credentials.
     *
     * @param host the host to which the credentials apply.
     * @param realm the realm to which the credentials apply.
     * @param username the username.
     * @param password the password.
     */
    virtual void SaveHttpAuthCredentials(const std::string& host, const std::string& realm,
        const std::string& username, const char* password) = 0;

    /**
     * @brief get username and password.
     *
     * @param host the host to which the credentials apply.
     * @param realm the realm to which the credentials apply.
     * @param username the username.
     * @param password the password.
     * @param passwordSize the password array size.
     */
    virtual void GetHttpAuthCredentials(const std::string& host, const std::string& realm,
        std::string& username, char* password, uint32_t passwordSize) const = 0;

    /**
     * @brief gets whether the instance holds the specifies permission for the specified source.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @return true if instances saved origin specifies permission.
     */
    virtual bool ExistPermissionByOrigin(const std::string& origin, int type) = 0;

    /**
     * @brief get specifies permission type result by origin.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @param result saved result.
     * @return return whether there is a saved result.
     */
    virtual bool GetPermissionResultByOrigin(const std::string& origin, int type, bool& result) = 0;

    /**
     * @brief set specifies permission type result by origin.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @param result set result.
     * @return 0 if successfully set specifies permission type result by origin other return error id.
     */
    virtual int SetPermissionByOrigin(const std::string& origin, int type, bool result) = 0;

    /**
     * @brief delete specifies permission type by origin.
     *
     * @param origin url source.
     * @param type specifies permission type.
     * @return 0 if successfully delete specifies permission type result by origin other return error id.
     */
    virtual int ClearPermissionByOrigin(const std::string& origin, int type) = 0;

    /**
     * @brief delete all specifies permission type.
     *
     * @param type specifies permission type.
     */
    virtual void ClearAllPermission(int type) = 0;

    /**
     * @brief obtains all origins of a specified permission type.
     *
     * @param type specifies permission type.
     * @return return all origin.
     */
    virtual std::vector<std::string> GetOriginsByPermission(int type) = 0;
};
} // namespace OHOS::NWeb

#endif  // NWEB_DATA_BASE_H
