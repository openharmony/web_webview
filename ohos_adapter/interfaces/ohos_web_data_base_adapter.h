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

#ifndef OHOS_WEB_DATA_BASE_ADAPTER_H
#define OHOS_WEB_DATA_BASE_ADAPTER_H

#include <string>
#include <vector>

namespace OHOS::NWeb {
class OhosWebDataBaseAdapter {
public:
    OhosWebDataBaseAdapter() = default;

    virtual ~OhosWebDataBaseAdapter() = default;

    virtual bool ExistHttpAuthCredentials() const = 0;

    virtual void DeleteHttpAuthCredentials() = 0;

    virtual void SaveHttpAuthCredentials(const std::string& host, const std::string& realm,
        const std::string& username, const char* password) = 0;

    virtual void GetHttpAuthCredentials(const std::string& host, const std::string& realm,
        std::vector<std::string>& usernamePassword) const = 0;
};
} // namespace OHOS::NWeb
#endif // OHOS_WEB_DATA_BASE_ADAPTER_H