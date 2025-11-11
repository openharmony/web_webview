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

#ifndef MOCK_BUNDLE_MANAGER_CLIENT_H
#define MOCK_BUNDLE_MANAGER_CLIENT_H

#include <string>
#include <vector>
#include <ability_manager_client.h>

namespace OHOS {
namespace AppExecFwk {
class BundleMgrClient {
public:
    BundleMgrClient() = default;
    virtual ~BundleMgrClient() = default;

    bool GetBundleInfo(const std::string &bundleName, const BundleFlag flag,
        BundleInfo &bundleInfo, int32_t userId);
    static std::vector<AbilityInfo> mockAbilityInfo;
    static std::vector<ExtensionAbilityInfo> mockExtensionAbilityInfo;
    static bool mockRet;
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // MOCK_BUNDLE_MANAGER_CLIENT_H
