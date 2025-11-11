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

#ifndef NWEB_MOCK_ACCESSTOKEN_KIT_H
#define NWEB_MOCK_ACCESSTOKEN_KIT_H

#include <cstdint>

#include "../../../../../../base/security/access_token/interfaces/innerkits/accesstoken/include/accesstoken_kit.h"
#include "../../../../../../base/security/access_token/interfaces/innerkits/accesstoken/include/tokenid_kit.h"

namespace OHOS::NWeb {
class MockAccesstokenKit {
public:
    static void MockIsSystemApp(const bool isSystemApp);
    static void MockIsSACalling(const bool mockGetTokenTypeFlagRet);
    static void MockAccessTokenKitRet(int32_t accessTokenKitRet);
    static void ChangeMockStateToInit();
    static void VerifyCallingPermission(const bool isCallingPermission);
    static void MockGetHapTokenInfo(int32_t ret);
    static void MockGetHapTokenInfo(Security::AccessToken::HapTokenInfo& info);
};
} // namespace OHOS::NWeb

#endif // NWEB_MOCK_ACCESSTOKEN_KIT_H