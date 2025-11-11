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

#include "bundle_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {
using namespace std;
using namespace OHOS::AAFwk;

std::vector<AbilityInfo> BundleMgrClient::mockAbilityInfo ;
std::vector<ExtensionAbilityInfo> BundleMgrClient::mockExtensionAbilityInfo;
bool BundleMgrClient::mockRet = true;

bool BundleMgrClient::GetBundleInfo(const string &bundleName,
    const BundleFlag flag, BundleInfo &bundleInfo, int32_t userId)
{
    if (!mockRet) {
        return false;
    }
    bundleInfo.extensionInfos = mockExtensionAbilityInfo;
    bundleInfo.abilityInfos = mockAbilityInfo;
    return true;
}
} // namespace AppExecFwk
} // namespace OHOS
